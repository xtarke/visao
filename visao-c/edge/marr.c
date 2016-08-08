/* Marr/Hildreth edge detection */

#include "stdio.h"
#include "cv.h"
#include "highgui.h"

#define PI 3.1415926535

/* The image header data structure      */
struct header 
{
	int nr, nc;             /* Rows and columns in the image */
	int oi, oj;             /* Origin */
};

/*      The IMAGE data structure        */
struct image 
{
		struct header *info;            /* Pointer to header */
		unsigned char **data;           /* Pixel values */
};

#define SQRT2 1.414213562
#define BLACK 0
#define WHITE 1

long seed = 132531;
typedef struct image * IMAGE;

#if defined (MAX)
int    PBM_SE_ORIGIN_COL=0, PBM_SE_ORIGIN_ROW=0;
char **arg;
int maxargs;
#else
extern int PBM_SE_ORIGIN_COL, PBM_SE_ORIGIN_ROW;
#endif

int range (IMAGE im, int i, int j);
void print_se (IMAGE p);
IMAGE Input_PBM (char *fn);
IMAGE Output_PBM (IMAGE image, char *filename);
void get_num_pbm (FILE *f, char *b, int *bi, int *res);
void pbm_getln (FILE *f, char *b);
void pbm_param (char *s);
struct image  *newimage (int nr, int nc);
void freeimage (struct image  *z);
void sys_abort (int val, char *mess);
void CopyVarImage (IMAGE *a, IMAGE *b);
void Display (IMAGE x);
float ** f2d (int nr, int nc);
void srand32 (long k);
double drand32 ();
void copy (IMAGE *a, IMAGE b);

float gauss(float x, float sigma);
float LoG (float x, float sigma);
float meanGauss (float x, float sigma);
void marr (float s, IMAGE im);
void dolap (float **x, int nr, int nc, float **y);
void zero_cross (float **lapim, IMAGE im);
float norm (float x, float y);
float distance (float a, float b, float c, float d);
void convolution (IMAGE im, float **mask, int nr, int nc, float **res,
	int NR, int NC);



float norm (float x, float y)
{
	return (float) sqrt ( (double)(x*x + y*y) );
}

float distance (float a, float b, float c, float d)
{
	return norm ( (a-c), (b-d) );
}

void marr (float s, IMAGE im)
{
	int width;
	float **smx;
	int i,j,k,n;
	float **lgau, z;

/* Create a Gaussian and a derivative of Gaussian filter mask  */
	width = 3.35*s + 0.33;
	n = width+width + 1;
	printf ("Smoothing with a Gaussian of size %dx%d\n", n, n);
	lgau = f2d (n, n);
	for (i=0; i<n; i++)
	  for (j=0; j<n; j++)
	    lgau[i][j] = LoG (distance ((float)i, (float)j,
			 (float)width, (float)width), s);

/* Convolution of source image with a Gaussian in X and Y directions  */
	smx = f2d (im->info->nr, im->info->nc);
	printf ("Convolution with LoG:\n");
	convolution (im, lgau, n, n, smx, im->info->nr, im->info->nc);

/* Locate the zero crossings */
	printf ("Zero crossings:\n");
	zero_cross (smx, im);

/* Clear the boundary */
	for (i=0; i<im->info->nr; i++)
	{
	  for (j=0; j<=width; j++) im->data[i][j] = 0;
	  for (j=im->info->nc-width-1; j<im->info->nc; j++)
		im->data[i][j] = 0;
	}
	for (j=0; j<im->info->nc; j++)
	{
	  for (i=0; i<= width; i++) im->data[i][j] = 0;
	  for (i=im->info->nr-width-1; i<im->info->nr; i++)
		im->data[i][j] = 0;
	}

	free(smx[0]); free(smx);
	free(lgau[0]); free(lgau);
}

/*	Gaussian	*/
float gauss(float x, float sigma)
{
    return (float)exp((double) ((-x*x)/(2*sigma*sigma)));
}

float meanGauss (float x, float sigma)
{
	float z;

	z = (gauss(x,sigma)+gauss(x+0.5,sigma)+gauss(x-0.5,sigma))/3.0;
	z = z/(PI*2.0*sigma*sigma);
	return z;
}

float LoG (float x, float sigma)
{
	float x1;

	x1 = gauss (x, sigma);
	return (x*x-2*sigma*sigma)/(sigma*sigma*sigma*sigma) * x1;
}


void convolution (IMAGE im, float **mask, int nr, int nc, float **res,
	int NR, int NC)
{
	int i,j,ii,jj, n, m, k, kk;
	float x, y;

	k = nr/2; kk = nc/2;
	for (i=0; i<NR; i++)
	  for (j=0; j<NC; j++)
	  {
	    x = 0.0;
	    for (ii=0; ii<nr; ii++)
	    {
	      n = i - k + ii;
	      if (n<0 || n>=NR) continue;
	      for (jj=0; jj<nc; jj++)
	      {
		m = j - kk + jj;
		if (m<0 || m>=NC) continue;
		x += mask[ii][jj] * (float)(im->data[n][m]);
	      }
	    }
	    res[i][j] = x;
	  }
}

void zero_cross (float **lapim, IMAGE im)
{
	int i,j,k,n,m, dx, dy;
	float x, y, z;
	int xi,xj,yi,yj, count = 0;
	IMAGE deriv;

	for (i=1; i<im->info->nr-1; i++)
	  for (j=1; j<im->info->nc-1; j++)
	  {
	    im->data[i][j] = 0;
	    if(lapim[i-1][j]*lapim[i+1][j]<0) {im->data[i][j]=255; continue;}
	    if(lapim[i][j-1]*lapim[i][j+1]<0) {im->data[i][j]=255; continue;}
	    if(lapim[i+1][j-1]*lapim[i-1][j+1]<0) {im->data[i][j]=255; continue;}
	    if(lapim[i-1][j-1]*lapim[i+1][j+1]<0) {im->data[i][j]=255; continue;}
	  }
}

/*	An alternative way to compute a Laplacian	*/
void dolap (float **x, int nr, int nc, float **y)
{
	int i,j,k,n,m;
	float u,v;

	for (i=1; i<nr-1; i++)
	  for (j=1; j<nc-1; j++)
	  {
	    y[i][j] = (x[i][j+1]+x[i][j-1]+x[i-1][j]+x[i+1][j]) - 4*x[i][j];
	    if (u>y[i][j]) u = y[i][j];
	    if (v<y[i][j]) v = y[i][j];
	  }
}



/*      PRINT_SE - Print a structuring element to the screen    */
void print_se (IMAGE p)
{
	int i,j;

	printf ("\n=====================================================\n");
	if (p == NULL)
	  printf (" Structuring element is NULL.\n");
	else 
	{
	  printf ("Structuring element: %dx%d origin at (%d,%d)\n",
		p->info->nr, p->info->nc, p->info->oi, p->info->oj);
	  for (i=0; i<p->info->nr; i++)
	  {
	    printf ("	");
	    for (j=0; j<p->info->nc; j++)
	      printf ("%4d ", p->data[i][j]);
	    printf ("\n");
	  }
	}
	printf ("\n=====================================================\n");
}

struct image  *newimage (int nr, int nc)
{
	struct image  *x;                /* New image */
	int i;
	unsigned char *p;

	if (nr < 0 || nc < 0) {
		printf ("Error: Bad image size (%d,%d)\n", nr, nc);
		return 0;
	}

/*      Allocate the image structure    */
	x = (struct image  *) malloc( sizeof (struct image) );
	if (!x) {
		printf ("Out of storage in NEWIMAGE.\n");
		return 0;
	}

/*      Allocate and initialize the header      */

	x->info = (struct header *)malloc( sizeof(struct header) );
	if (!(x->info)) 
	{
		printf ("Out of storage in NEWIMAGE.\n");
		return 0;
	}
	x->info->nr = nr;       x->info->nc = nc;
	x->info->oi = x->info->oj = 0;

/*      Allocate the pixel array        */

	x->data = (unsigned char **)malloc(sizeof(unsigned char *)*nr); 

/* Pointers to rows */
	if (!(x->data)) 
	{
		printf ("Out of storage in NEWIMAGE.\n");
		return 0;
	}

	x->data[0] = (unsigned char *)malloc (nr*nc);
	p = x->data[0];
	if (x->data[0]==0)
	  {
		printf ("Out of storage. Newimage  \n");
		exit(1);
	  }

	for (i=1; i<nr; i++) 
	{
	  x->data[i] = (p+i*nc);
	}

	return x;
}

void freeimage (struct image  *z)
{
/*      Free the storage associated with the image Z    */

	if (z != 0) 
	{
	   free (z->data[0]);
	   free (z->info);
	   free (z->data);
	   free (z);
	}
}

void sys_abort (int val, char *mess)
{
	fprintf (stderr, "**** System library ABORT %d: %s ****\n", 
			val, mess);
	exit (2);
}

void copy (IMAGE *a, IMAGE b)
{
	CopyVarImage (a, &b);
}

void CopyVarImage (IMAGE *a, IMAGE *b)
{
	int i,j;

	if (a == b) return;
	if (*a) freeimage (*a);
	*a = newimage ((*b)->info->nr, (*b)->info->nc);
	if (*a == 0) sys_abort (0, "No more storage.\n");

	for (i=0; i<(*b)->info->nr; i++)
	  for (j=0; j< (*b)->info->nc; j++)
	    (*a)->data[i][j] = (*b)->data[i][j];
	(*a)->info->oi = (*b)->info->oi;
	(*a)->info->oj = (*b)->info->oj;
}

float ** f2d (int nr, int nc)
{
	float **x;
	int i;

	x = (float **)calloc ( nr, sizeof (float *) );
	if (x == 0)
	{
	  fprintf (stderr, "Out of storage: F2D.\n");
	  exit (1);
	}

	for (i=0; i<nr; i++)
	{  
	  x[i] = (float *) calloc ( nc, sizeof (float)  );
	  if (x[i] == 0)
	  {
	    fprintf (stderr, "Out of storage: F2D %d.\n", i);
	    exit (1);
	  }
	}
	return x;
}

void free2d (float **x, int nr)
{
	int i;

	for (i=0; i<nr; i++)
		free(x[i]);
	free (x);
}

/* Small system random number generator */
double drand32 ()
{
	static long a=16807L, m=2147483647L,
		    q=127773L, r = 2836L;
	long lo, hi, test;

	hi = seed / q;
	lo = seed % q;
	test = a*lo -r*hi;
	if (test>0) seed = test;
	else seed = test + m;

	return (double)seed/(double)m;
}

void srand32 (long k)
{
	seed = k;
}


IplImage *toOpenCV (IMAGE x)
{
	IplImage *img;
	int i=0, j=0;
	CvScalar s;
	
	img = cvCreateImage(cvSize(x->info->nc, x->info->nr),8, 1);
	for (i=0; i<x->info->nr; i++)
	{
		for (j=0; j<x->info->nc; j++)
		{
			s.val[0] = x->data[i][j];
			cvSet2D (img, i,j,s);
		}
	}
	return img;
}

IMAGE fromOpenCV (IplImage *x)
{
	IMAGE img;
	int color=0, i=0;
	int k=0, j=0;
	CvScalar s;
	
	if ((x->depth==IPL_DEPTH_8U) &&(x->nChannels==1))								// 1 Pixel (grey) image
		img = newimage (x->height, x->width);
	else if ((x->depth==8) && (x->nChannels==3)) //Color
	{
		color = 1;
		img = newimage (x->height, x->width);
	}
	else return 0;

	for (i=0; i<x->height; i++)
	{
		for (j=0; j<x->width; j++)
		{
			s = cvGet2D (x, i, j);
			if (color) 
			  k = (unsigned char)((s.val[0] + s.val[1] + s.val[2])/3);
			else k = (unsigned char)(s.val[0]);
			img->data[i][j] = k;
		}
	}
	return img;
}

/* Display an image on th escreen */
void display_image (IMAGE x)
{
	IplImage *image = 0;
	char wn[20];
	int i;

	image = toOpenCV (x);
	if (image <= 0) return;

	for (i=0; i<19; i++) wn[i] = (char)((drand32()*26) + 'a');
	wn[19] = '\0';
	cvNamedWindow( wn, CV_WINDOW_AUTOSIZE );
	cvShowImage( wn, image );
	cvWaitKey(0);
	cvReleaseImage( &image );
}

void save_image (IMAGE x, char *name)
{
	IplImage *image = 0;

	image = toOpenCV (x);
	if (image <0) return;

	cvSaveImage( name, image );
	cvReleaseImage( &image );
}

IMAGE get_image (char *name)
{
	IMAGE x=0;
	IplImage *image = 0;

	image = cvLoadImage(name, 0);
	if (image <= 0) return 0;
	x = fromOpenCV (image);
	cvReleaseImage( &image );
	return x;
}

IMAGE grab_image ()
{
	CvCapture  *camera = 0;
	IplImage *image = 0;
	IMAGE x;

	camera = cvCaptureFromCAM( CV_CAP_ANY );
	if( !camera )								// Get a camera?
	{
		fprintf(stderr, "Can’t initialize camera\n");
		return 0;
	}
	image = cvQueryFrame( camera );						
	x = fromOpenCV (image);
	cvReleaseCapture( &camera );
	return x;
}

int get_RGB (IMAGE *r, IMAGE *g, IMAGE *b, char *name)
{
	IplImage *image = 0;
	IMAGE i1, i2, i3;
	int i,j;

	// Read the image from a file into Ip1Image
	image = cvLoadImage(name, 1);
	if (image <= 0) return 0;

	// Create three AIPCV images of correct size
	i1 = newimage (image->height, image->width);
	i2 = newimage (image->height, image->width);
	i3 = newimage (image->height, image->width);

	// Copy pixels from Ip1Image to AIPCV images
	for (i=0; i<image->height; i++)
		for (j=0; j<image->width; j++)
		{
		  i1->data[i][j] = (image->imageData+i*image->widthStep)[j*image->nChannels+0];
		  i2->data[i][j] = (image->imageData+i*image->widthStep)[j*image->nChannels+1];
		  i3->data[i][j] = (image->imageData+i*image->widthStep)[j*image->nChannels+2];
		}
	cvReleaseImage(&image);
	*r = i3; 
	*g = i2;
	*b = i1;
	return 1;
}

int save_RGB (IMAGE r, IMAGE g, IMAGE b, char *name)
{
	IplImage *image = 0;
	int i,j,k;

	if ( (r->info->nc != g->info->nc) || (r->info->nr != g->info->nr) ) return 0;
	if ( (r->info->nc != b->info->nc) || (r->info->nr != b->info->nr) ) return 0;

	// Create an  IplImage
	image = cvCreateImage(cvSize(r->info->nc, r->info->nr),IPL_DEPTH_8U,3);
	if (image <= 0) return 0;


	// Copy pixels from AIPCV images into Ip1Image
/*		for (i=0; i<r->info->nr; i++)
		{
			for (j=0; j<r->info->nc; j++)
			{
				s.val[0] = b->data[i][j];
				s.val[1] = g->data[i][j];
				s.val[2] = r->data[i][j];
				cvSet2D (image, i,j,s);
		}
	} */
		
	for (i=0; i<image->height; i++)
		for (j=0; j<image->width; j++)
		{
		  (image->imageData+i*image->widthStep)[j*image->nChannels+0] = b->data[i][j];
		  (image->imageData+i*image->widthStep)[j*image->nChannels+1] = g->data[i][j];
		  (image->imageData+i*image->widthStep)[j*image->nChannels+2] = r->data[i][j];
		} 
	k = cvSaveImage(name, image);
	cvReleaseImage(&image);
	return 1-k;

}

int main ()
{
	int i,j,n;
	float s=1.0;
	FILE *params;
	IMAGE im1, im2;
	char name[128];

	// Try to read an image
	printf ("Enter path to the image file to be processed: ");
	scanf ("%s", name);
	printf ("Opening file '%s'\n", name);
	im1 = get_image(name);
	printf ("Enter standard deviation: ");
	scanf ("%f", &s);

	display_image (im1);

/* Look for parameter file */
im2 = newimage (im1->info->nr, im1->info->nc);
	for (i=0; i<im1->info->nr; i++)
	  for (j=0; j<im1->info->nc; j++)
	    im2->data[i][j] = im1->data[i][j];

/* Apply the filter */
	marr (s-0.8, im1);
	marr (s+0.8, im2);
    
	for (i=0; i<im1->info->nr; i++)
	  for (j=0; j<im1->info->nc; j++)
	    if (im1->data[i][j] > 0 && im2->data[i][j] > 0)
		im1->data[i][j] = 0;
	    else im1->data[i][j] = 255;

	display_image (im1);
	save_image (im1, "marr.jpg");

	return 0;
}