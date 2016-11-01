#include "StereoVision.h"

#include "opencv2/opencv.hpp"
#include <iostream>  
#include <fstream>


StereoVision::StereoVision(){
    
}


StereoVision::~StereoVision(){
    
}





void StereoVision::calibrate(StereoCapture &capture){
    
    int  nx=9, ny=6, frame = 0, n_boards =40, N;
    
    int count1 = 0,count2 = 0, result1=0, result2=0;	
    
    int  successes1 = 0,successes2 = 0 ;
    const int maxScale = 1;
    const float squareSize = 0.023f;		//Set this to your actual square size
    CvSize imageSize = {0,0};
    CvSize board_sz = cvSize( nx,ny );
    
    int i, j, n = nx*ny, N1 = 0, N2 = 0;
    vector<CvPoint2D32f> points[2];
    vector<int> npoints;
    vector<CvPoint3D32f> objectPoints;
    vector<CvPoint2D32f> temp1(n); 
    vector<CvPoint2D32f> temp2(n);
    
    double M1[3][3], M2[3][3], D1[5], D2[5];
    double R[3][3], T[3], E[3][3], F[3][3];
	double Q[4][4];
        
    CvMat _Qcalib  = cvMat(4,4, CV_64F, Q);
    CvMat _M1calib = cvMat(3, 3, CV_64F, M1 );
    CvMat _M2calib = cvMat(3, 3, CV_64F, M2 );
    CvMat _D1 	   = cvMat(1, 5, CV_64F, D1 );
    CvMat _D2      = cvMat(1, 5, CV_64F, D2 );
    CvMat _R       = cvMat(3, 3, CV_64F, R );
    CvMat _Tcalib  = cvMat(3, 1, CV_64F, T );
    CvMat _E       = cvMat(3, 3, CV_64F, E );
    CvMat _F       = cvMat(3, 3, CV_64F, F );
    
    //Start webcam
    printf("\nWebcams are starting ...\n");
    
    
    // grabb->stereoGrabberInitFrames();
    //grabb->stereoGrabberGrabFrames();
    capture.capture();
    
    //IplImage *frame1 = grabb->imageLeft;
    //IplImage* gray_fr1 = cvCreateImage( cvGetSize(frame1), 8, 1 );
    
    IplImage frame1(capture.get_left_Frame());
    IplImage* gray_fr1 = cvCreateImage( cvGetSize(&frame1), 8, 1 );
    
        
    // IplImage *frame2 = grabb->imageRight;
    // IplImage* gray_fr2 = cvCreateImage( cvGetSize(frame2), 8, 1 );
    
    IplImage frame2(capture.get_left_Frame());
    IplImage* gray_fr2 = cvCreateImage( cvGetSize(&frame2), 8, 1 );
    
    imageSize = cvGetSize(&frame1);

    //Thu anh chessboard ve cho viec calib camera	
    cvNamedWindow( "camera2", 1 );
    cvNamedWindow( "camera1", 1 );
    cvNamedWindow("corners camera1",1);
    cvNamedWindow("corners camera2",1);	
    printf("\nWant to capture %d chessboards for calibrate:", n_boards);	
    
    while((successes1<n_boards)||(successes2<n_boards))						
    {

        //--------Find and Draw chessboard--------------------------------------------------	
        if((frame++ % 20) == 0)
        {
            //----------------CAM1-------------------------------------------------------------------------------------------------------
            result1 = cvFindChessboardCorners( &frame1, board_sz,&temp1[0], &count1,CV_CALIB_CB_ADAPTIVE_THRESH|CV_CALIB_CB_FILTER_QUADS);
            cvCvtColor( &frame1, gray_fr1, CV_BGR2GRAY );
            //cvEqualizeHist(gray_fr1,gray_fr1);
            
            
            cout << result1 << endl;

            //----------------CAM2--------------------------------------------------------------------------------------------------------
            result2 = cvFindChessboardCorners( &frame2, board_sz,&temp2[0], &count2,CV_CALIB_CB_ADAPTIVE_THRESH|CV_CALIB_CB_FILTER_QUADS);
            cvCvtColor( &frame2, gray_fr2, CV_BGR2GRAY );
            
            //cvEqualizeHist(gray_fr2,gray_fr2);
            
              
            cout << result2 << endl;
            
            if(count1==n&&count2==n&&result1&&result2)
            {
                cvFindCornerSubPix( gray_fr1, &temp1[0], count1,cvSize(11, 11), 
                                    cvSize(-1,-1),cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS,30, 0.01) );
                cvDrawChessboardCorners( &frame1, board_sz, &temp1[0], count1, result1 );
                cvShowImage( "corners camera1", &frame1 );
                N1 = points[0].size();
                points[0].resize(N1 + n, cvPoint2D32f(0,0));
                copy( temp1.begin(), temp1.end(), points[0].begin() + N1 );
                ++successes1;
                
                cvFindCornerSubPix( gray_fr2, &temp2[0], count2,cvSize(11, 11), cvSize(-1,-1),cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS,30, 0.01) );
                cvDrawChessboardCorners( &frame2, board_sz, &temp2[0], count2, result2 );
                cvShowImage( "corners camera2", &frame2 );
                N2 = points[1].size();
                points[1].resize(N2 + n, cvPoint2D32f(0,0));
                copy( temp2.begin(), temp2.end(), points[1].begin() + N2 );
                ++successes2;
                
                printf("\nNumber of couple Chessboards were found: %d", successes2);
            }  else  {	
                cvShowImage( "corners camera2", gray_fr2 );	
                cvShowImage( "corners camera1", gray_fr1 );	
            }
            

            capture.capture();
            frame1 = capture.get_left_Frame();
            cvShowImage("Frame_Left", &frame1);
            
            frame2 = capture.get_right_Frame();
            cvShowImage("Frame_right", &frame2);
            
            if(cvWaitKey(15)==27) break;
        }
    }

    //grabb->stereoGrabberStopCam();
    capture.stop_cam();
    
    
    cvDestroyWindow("camera1");
    cvDestroyWindow("camera2");
    cvDestroyWindow("corners camera1");
    cvDestroyWindow("corners camera2");	
    printf("\nDone Capture!");


    //--------------Compute for calibration-------------------
    N = n_boards*n;
    objectPoints.resize(N);
    for( i = 0; i < ny; i++ )
            for(j = 0; j < nx; j++ )   objectPoints[i*nx + j] = cvPoint3D32f(i*squareSize, j*squareSize, 0);
    for( i = 1; i < n_boards; i++ ) copy( objectPoints.begin(), objectPoints.begin() + n, objectPoints.begin() + i*n );
    npoints.resize(n_boards,n);

    CvMat _objectPoints = cvMat(1, N, CV_32FC3, &objectPoints[0] );
    CvMat _imagePoints1 = cvMat(1, N, CV_32FC2, &points[0][0] );
    CvMat _imagePoints2 = cvMat(1, N, CV_32FC2, &points[1][0] );
    CvMat _npoints = cvMat(1, npoints.size(), CV_32S, &npoints[0] );
    cvSetIdentity(&_M1calib);
    cvSetIdentity(&_M2calib);
    cvZero(&_D1);
    cvZero(&_D2);

    printf("\nRunning stereo calibration ...");
    fflush(stdout);
    
    
    CvTermCriteria term_crit = cvTermCriteria( CV_TERMCRIT_ITER + CV_TERMCRIT_EPS,100,1e-6);
    
    cvStereoCalibrate( &_objectPoints, &_imagePoints1, &_imagePoints2, &_npoints,&_M1calib, &_D1, 
                       &_M2calib, &_D2,imageSize, &_R, &_Tcalib, &_E, &_F,
        CV_CALIB_FIX_ASPECT_RATIO +
        CV_CALIB_ZERO_TANGENT_DIST +
        CV_CALIB_SAME_FOCAL_LENGTH +
        CV_CALIB_FIX_K3, term_crit );

    printf("\nDone Calibration");
    //-------------Chinh meo cho anh------------------------------------------
    cvUndistortPoints( &_imagePoints1, &_imagePoints1,&_M1calib, &_D1, 0, &_M1calib );
    cvUndistortPoints( &_imagePoints2, &_imagePoints2,&_M2calib, &_D2, 0, &_M2calib );

    //--------Tinh toan va canh anh, dung giai thuat bouguet-------------------
    CvMat* mx1calib = cvCreateMat( imageSize.height,imageSize.width, CV_32F );
    CvMat* my1calib = cvCreateMat( imageSize.height,imageSize.width, CV_32F );
    CvMat* mx2calib = cvCreateMat( imageSize.height,imageSize.width, CV_32F );
    CvMat* my2calib = cvCreateMat( imageSize.height,imageSize.width, CV_32F );

    double R1[3][3], R2[3][3], P1[3][4], P2[3][4];
    CvMat _R1 = cvMat(3, 3, CV_64F, R1);
    CvMat _R2 = cvMat(3, 3, CV_64F, R2);
    //Calib dung Bouguet
    CvMat _P1 = cvMat(3, 4, CV_64F, P1);
    CvMat _P2 = cvMat(3, 4, CV_64F, P2);
    cvStereoRectify( &_M1calib, &_M2calib, &_D1, &_D2, imageSize,&_R, &_Tcalib,&_R1, &_R2, &_P1, &_P2, &_Qcalib,0/*CV_CALIB_ZERO_DISPARITY*/ );
    //Tim tham so cho cvRemap()
    cvInitUndistortRectifyMap(&_M1calib,&_D1,&_R1,&_P1,mx1calib,my1calib);
    cvInitUndistortRectifyMap(&_M2calib,&_D2,&_R2,&_P2,mx2calib,my2calib);

    printf("\nSaving matries for later use ...\n");
    cvSave("./CalibFile/M1.yml",&_M1calib);
//	cvSave("CalibFile//D1.yml",&_D1);
//	cvSave("CalibFile//R1.yml",&_R1);
//	cvSave("CalibFile//P1.yml",&_P1);
    cvSave("./CalibFile/M2.yml",&_M2calib);
//	cvSave("CalibFile//D2.yml",&_D2);
//	cvSave("CalibFile//R2.yml",&_R2);
//	cvSave("CalibFile//P2.yml",&_P2);
    cvSave("./CalibFile/Q.yml",&_Qcalib);
    cvSave("./CalibFile/T.yml",&_Tcalib);
    cvSave("./CalibFile/mx1.yml",mx1calib);
    cvSave("./CalibFile/my1.yml",my1calib);
    cvSave("./CalibFile/mx2.yml",mx2calib);
    cvSave("./CalibFile/my2.yml",my2calib);
   
}

void StereoVision::load_correlation(string FileName)
{
    std::ifstream P_File;
        
    P_File.open(FileName);
    
    if (!P_File) {
        cerr << "load_correlation: Unable to open file Correlation file!" << endl;;
	cerr << "Loading defaults";
        
        stereoDispWindowSize = 33;        
        stereoDispTextureThreshold = 20;        
        stereoDispUniquenessRatio = 15;        
        stereoNumDisparities = 48;	
	threshold = 29;	
	blobArea = 1500;
	
	stereoPreFilterSize = 63;    
        stereoPreFilterCap = 63;
        stereoSavePointCloudValue = 0;
    }       
    else{
        string temp; 
        int value; 
        while(P_File >> temp)
        {
            P_File >> value;
            stereoDispWindowSize = value;
            P_File >> temp;
            P_File >> value;
            stereoDispTextureThreshold = value;
            P_File>>temp;
            P_File>>value;
            stereoDispUniquenessRatio = value;
            P_File >> temp;
            P_File >> value;
            stereoNumDisparities = value;
            P_File >> temp;
            P_File >> value;
            threshold = value;
            P_File >> temp;
            P_File >> value;
            blobArea = value;
        }
        
        stereoPreFilterSize = 63;
        stereoPreFilterCap = 63; 
        stereoSavePointCloudValue = 0; 
//		stereoFunc->stereoSaveOriginal = 0;

        P_File.close();
	}
}
