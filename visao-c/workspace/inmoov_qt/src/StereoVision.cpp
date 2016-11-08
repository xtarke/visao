#include "StereoVision.h"

#include "opencv2/opencv.hpp"
#include <iostream>  
#include <fstream>


StereoVision::StereoVision(StereoCapture &capture){
   
    cameras = &capture;
    
    load_correlation("CorrelationParams.txt");
    
    // initialization();
    
    
}


StereoVision::~StereoVision(){
    
}


void StereoVision::stereoCorrelation()
{	


}


void StereoVision::stereoDetect()

{
		
	
}


void StereoVision::load_correlation(string FileName)
{
//     std::ifstream P_File;
//         
//     P_File.open(FileName);
//     
//     if (!P_File) {
//         cerr << "load_correlation: Unable to open file Correlation file!" << endl;;
// 	cerr << "Loading defaults";
//         
//         stereoDispWindowSize = 33;        
//         stereoDispTextureThreshold = 20;        
//         stereoDispUniquenessRatio = 15;        
//         stereoNumDisparities = 48;	
// 	threshold = 29;	
// 	blobArea = 1500;
// 	
// 	stereoPreFilterSize = 63;    
//         stereoPreFilterCap = 63;
//         stereoSavePointCloudValue = 0;
//     }       
//     else{
//         string temp; 
//         int value; 
//         while(P_File >> temp)
//         {
//             P_File >> value;
//             stereoDispWindowSize = value;
//             P_File >> temp;
//             P_File >> value;
//             stereoDispTextureThreshold = value;
//             P_File>>temp;
//             P_File>>value;
//             stereoDispUniquenessRatio = value;
//             P_File >> temp;
//             P_File >> value;
//             stereoNumDisparities = value;
//             P_File >> temp;
//             P_File >> value;
//             threshold = value;
//             P_File >> temp;
//             P_File >> value;
//             blobArea = value;
//         }
//         
//         stereoPreFilterSize = 63;
//         stereoPreFilterCap = 63; 
//         stereoSavePointCloudValue = 0; 
// //		stereoFunc->stereoSaveOriginal = 0;
// 
//         P_File.close();
// 	}
// 	
// 	
// 	cout << "Loaded correlations: " << endl;
//         cout << "stereoDispWindowSize: " << stereoDispWindowSize << endl;
//         cout << "stereoDispTextureThreshold: " << stereoDispTextureThreshold << endl;
//         cout << "stereoNumDisparities: " << stereoNumDisparities << endl;
//         cout << "threshold: " << threshold << endl;
//         cout << "blobArea: " << blobArea << endl;
}   

void StereoVision::StereoCalib(const vector<Mat>& imagelist, Size boardSize, float squareSize, bool displayCorners, bool useCalibrated, bool showRectified)
{
    if( imagelist.size() % 2 != 0 )
    {
        cout << "Error: the image list contains odd (non-even) number of elements\n";
        return;
    }

    const int maxScale = 2;
    // ARRAY AND VECTOR STORAGE:

    vector<vector<Point2f> > imagePoints[2];
    vector<vector<Point3f> > objectPoints;
    Size imageSize;

    int i, j, k, nimages = (int)imagelist.size()/2;

    imagePoints[0].resize(nimages);
    imagePoints[1].resize(nimages);
    vector<Mat> goodImageList;
    
    for( i = j = 0; i < nimages; i++ )
    {
        for( k = 0; k < 2; k++ )
        {
            //const string& filename = imagelist[i*2+k];
                        
            Mat img = imagelist[i*2+k];      
                        
            if(img.empty())
                break;
            if( imageSize == Size() )
                imageSize = img.size();
            else if( img.size() != imageSize )
            {
                cout << "The image " << (i*2+k) << " has the size different from the first image size. Skipping the pair\n";
                break;
            }
            bool found = false;
            
            vector<Point2f>& corners = imagePoints[k][j];
            for( int scale = 1; scale <= maxScale; scale++ )
            {
                Mat timg;
                if( scale == 1 )
                    timg = img;
                else
                    resize(img, timg, Size(), scale, scale);
                found = findChessboardCorners(timg, boardSize, corners,
                    CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE);
                
                if( found )
                {
                    if( scale > 1 )
                    {
                        Mat cornersMat(corners);
                        cornersMat *= 1./scale;
                    }
                    break;
                }
            }
            
            if( displayCorners )
            {
                cout << (i*2+k) << endl;
            
                Mat cimg, cimg1;
                cvtColor(img, cimg, COLOR_GRAY2BGR);
                drawChessboardCorners(cimg, boardSize, corners, found);
                double sf = 640./MAX(img.rows, img.cols);
                resize(cimg, cimg1, Size(), sf, sf);
                imshow("corners", cimg1);
                char c = (char)waitKey(500);
                if( c == 27 || c == 'q' || c == 'Q' ) //Allow ESC to quit
                    exit(-1);
            }
            else
                putchar('.');
            if( !found )
                break;
            cornerSubPix(img, corners, Size(11,11), Size(-1,-1),
                         TermCriteria(TermCriteria::COUNT+TermCriteria::EPS,
                                      30, 0.01));
        }
        if( k == 2 )
        {
            goodImageList.push_back(imagelist[i*2]);
            goodImageList.push_back(imagelist[i*2+1]);
            j++;
        }
    }
    cout << j << " pairs have been successfully detected.\n";
    nimages = j;
    if( nimages < 2 )
    {
        cout << "Error: too little pairs to run the calibration\n";
        return;
    }

    imagePoints[0].resize(nimages);
    imagePoints[1].resize(nimages);
    objectPoints.resize(nimages);

    for( i = 0; i < nimages; i++ )
    {
        for( j = 0; j < boardSize.height; j++ )
            for( k = 0; k < boardSize.width; k++ )
                objectPoints[i].push_back(Point3f(k*squareSize, j*squareSize, 0));
    }

    cout << "Running stereo calibration ...\n";

//     Mat cameraMatrix[2], distCoeffs[2];
//     cameraMatrix[0] = initCameraMatrix2D(objectPoints,imagePoints[0],imageSize,0);
//     cameraMatrix[1] = initCameraMatrix2D(objectPoints,imagePoints[1],imageSize,0);
//     Mat R, T, E, F;
    
    cameraMatrix[0] = initCameraMatrix2D(objectPoints,imagePoints[0],imageSize,0);
    cameraMatrix[1] = initCameraMatrix2D(objectPoints,imagePoints[1],imageSize,0);    

    double rms = stereoCalibrate(objectPoints, imagePoints[0], imagePoints[1],
                    cameraMatrix[0], distCoeffs[0],
                    cameraMatrix[1], distCoeffs[1],
                    imageSize, R, T, E, F,
                    CALIB_FIX_ASPECT_RATIO +
                    CALIB_ZERO_TANGENT_DIST +
                    CALIB_USE_INTRINSIC_GUESS +
                    CALIB_SAME_FOCAL_LENGTH +
                    CALIB_RATIONAL_MODEL +
                    CALIB_FIX_K3 + CALIB_FIX_K4 + CALIB_FIX_K5,
                    TermCriteria(TermCriteria::COUNT+TermCriteria::EPS, 100, 1e-5) );
    cout << "done with RMS error=" << rms << endl;

// CALIBRATION QUALITY CHECK
// because the output fundamental matrix implicitly
// includes all the output information,
// we can check the quality of calibration using the
// epipolar geometry constraint: m2^t*F*m1=0
    double err = 0;
    int npoints = 0;
    vector<Vec3f> lines[2];
    for( i = 0; i < nimages; i++ )
    {
        int npt = (int)imagePoints[0][i].size();
        Mat imgpt[2];
        for( k = 0; k < 2; k++ )
        {
            imgpt[k] = Mat(imagePoints[k][i]);
            undistortPoints(imgpt[k], imgpt[k], cameraMatrix[k], distCoeffs[k], Mat(), cameraMatrix[k]);
            computeCorrespondEpilines(imgpt[k], k+1, F, lines[k]);
        }
        for( j = 0; j < npt; j++ )
        {
            double errij = fabs(imagePoints[0][i][j].x*lines[1][j][0] +
                                imagePoints[0][i][j].y*lines[1][j][1] + lines[1][j][2]) +
                           fabs(imagePoints[1][i][j].x*lines[0][j][0] +
                                imagePoints[1][i][j].y*lines[0][j][1] + lines[0][j][2]);
            err += errij;
        }
        npoints += npt;
    }
    cout << "average epipolar err = " <<  err/npoints << endl;

    // save intrinsic parameters
    FileStorage fs("intrinsics.yml", FileStorage::WRITE);
    if( fs.isOpened() )
    {
        fs << "M1" << cameraMatrix[0] << "D1" << distCoeffs[0] <<
            "M2" << cameraMatrix[1] << "D2" << distCoeffs[1];
        fs.release();
    }
    else
        cout << "Error: can not save the intrinsic parameters\n";

    Mat R1, R2, P1, P2, Q;
    Rect validRoi[2];

    stereoRectify(cameraMatrix[0], distCoeffs[0],
                  cameraMatrix[1], distCoeffs[1],
                  imageSize, R, T, R1, R2, P1, P2, Q,
                  CALIB_ZERO_DISPARITY, 1, imageSize, &validRoi[0], &validRoi[1]);

    fs.open("extrinsics.yml", FileStorage::WRITE);
    if( fs.isOpened() )
    {
        fs << "R" << R << "T" << T << "R1" << R1 << "R2" << R2 << "P1" << P1 << "P2" << P2 << "Q" << Q;
        fs.release();
    }
    else
        cout << "Error: can not save the extrinsic parameters\n";

    // OpenCV can handle left-right
    // or up-down camera arrangements
    bool isVerticalStereo = fabs(P2.at<double>(1, 3)) > fabs(P2.at<double>(0, 3));

// COMPUTE AND DISPLAY RECTIFICATION
    if( !showRectified )
        return;

    Mat rmap[2][2];
// IF BY CALIBRATED (BOUGUET'S METHOD)
    if( useCalibrated )
    {
        // we already computed everything
    }
// OR ELSE HARTLEY'S METHOD
    else
 // use intrinsic parameters of each camera, but
 // compute the rectification transformation directly
 // from the fundamental matrix
    {
        vector<Point2f> allimgpt[2];
        for( k = 0; k < 2; k++ )
        {
            for( i = 0; i < nimages; i++ )
                std::copy(imagePoints[k][i].begin(), imagePoints[k][i].end(), back_inserter(allimgpt[k]));
        }
        F = findFundamentalMat(Mat(allimgpt[0]), Mat(allimgpt[1]), FM_8POINT, 0, 0);
        Mat H1, H2;
        stereoRectifyUncalibrated(Mat(allimgpt[0]), Mat(allimgpt[1]), F, imageSize, H1, H2, 3);

        R1 = cameraMatrix[0].inv()*H1*cameraMatrix[0];
        R2 = cameraMatrix[1].inv()*H2*cameraMatrix[1];
        P1 = cameraMatrix[0];
        P2 = cameraMatrix[1];
    }

    //Precompute maps for cv::remap()
    initUndistortRectifyMap(cameraMatrix[0], distCoeffs[0], R1, P1, imageSize, CV_16SC2, rmap[0][0], rmap[0][1]);
    initUndistortRectifyMap(cameraMatrix[1], distCoeffs[1], R2, P2, imageSize, CV_16SC2, rmap[1][0], rmap[1][1]);

    Mat canvas;
    double sf;
    int w, h;
    if( !isVerticalStereo )
    {
        sf = 600./MAX(imageSize.width, imageSize.height);
        w = cvRound(imageSize.width*sf);
        h = cvRound(imageSize.height*sf);
        canvas.create(h, w*2, CV_8UC3);
    }
    else
    {
        sf = 300./MAX(imageSize.width, imageSize.height);
        w = cvRound(imageSize.width*sf);
        h = cvRound(imageSize.height*sf);
        canvas.create(h*2, w, CV_8UC3);
    }

    for( i = 0; i < nimages; i++ )
    {
        for( k = 0; k < 2; k++ )
        {
            Mat img = goodImageList[i*2+k], rimg, cimg;
            
            remap(img, rimg, rmap[k][0], rmap[k][1], INTER_LINEAR);
            cvtColor(rimg, cimg, COLOR_GRAY2BGR);
            Mat canvasPart = !isVerticalStereo ? canvas(Rect(w*k, 0, w, h)) : canvas(Rect(0, h*k, w, h));
            resize(cimg, canvasPart, canvasPart.size(), 0, 0, INTER_AREA);
            if( useCalibrated )
            {
                Rect vroi(cvRound(validRoi[k].x*sf), cvRound(validRoi[k].y*sf),
                          cvRound(validRoi[k].width*sf), cvRound(validRoi[k].height*sf));
                rectangle(canvasPart, vroi, Scalar(0,0,255), 3, 8);
            }
        }

        if( !isVerticalStereo )
            for( j = 0; j < canvas.rows; j += 16 )
                line(canvas, Point(0, j), Point(canvas.cols, j), Scalar(0, 255, 0), 1, 8);
        else
            for( j = 0; j < canvas.cols; j += 16 )
                line(canvas, Point(j, 0), Point(j, canvas.rows), Scalar(0, 255, 0), 1, 8);
        imshow("rectified", canvas);
        char c = (char)waitKey();
        if( c == 27 || c == 'q' || c == 'Q' )
            break;
    }
}














































// Depracated
// void StereoVision::calibrate(StereoCapture &capture){
//     
//     int  nx=9, ny=6, frame = 0, n_boards =40, N;
//     
//     int count1 = 0,count2 = 0, result1=0, result2=0;
//     
//     char key = 0;
//     
//     int  successes1 = 0,successes2 = 0 ;
//     const int maxScale = 1;
//     const float squareSize = 0.023f;		//Set this to your actual square size
//     CvSize imageSize = {0,0};
//     CvSize board_sz = cvSize( nx,ny );
//     
//     int i, j, n = nx*ny, N1 = 0, N2 = 0;
//     vector<CvPoint2D32f> points[2];
//     vector<int> npoints;
//     vector<CvPoint3D32f> objectPoints;
//     vector<CvPoint2D32f> temp1(n); 
//     vector<CvPoint2D32f> temp2(n);
//     
//     double M1[3][3], M2[3][3], D1[5], D2[5];
//     double R[3][3], T[3], E[3][3], F[3][3];
// 	double Q[4][4];
//         
//     CvMat _Qcalib  = cvMat(4,4, CV_64F, Q);
//     CvMat _M1calib = cvMat(3, 3, CV_64F, M1 );
//     CvMat _M2calib = cvMat(3, 3, CV_64F, M2 );
//     CvMat _D1 	   = cvMat(1, 5, CV_64F, D1 );
//     CvMat _D2      = cvMat(1, 5, CV_64F, D2 );
//     CvMat _R       = cvMat(3, 3, CV_64F, R );
//     CvMat _Tcalib  = cvMat(3, 1, CV_64F, T );
//     CvMat _E       = cvMat(3, 3, CV_64F, E );
//     CvMat _F       = cvMat(3, 3, CV_64F, F );
//     
//     //Start webcam
//     printf("\nWebcams are starting ...\n");
//     
//     
//     // grabb->stereoGrabberInitFrames();
//     //grabb->stereoGrabberGrabFrames();
//     capture.capture();
//     
//     //IplImage *frame1 = grabb->imageLeft;
//     //IplImage* gray_fr1 = cvCreateImage( cvGetSize(frame1), 8, 1 );
//     
//     IplImage frame1(capture.get_left_Frame());
//     IplImage* gray_fr1 = cvCreateImage( cvGetSize(&frame1), 8, 1 );
//     
//         
//     // IplImage *frame2 = grabb->imageRight;
//     // IplImage* gray_fr2 = cvCreateImage( cvGetSize(frame2), 8, 1 );
//     
//     IplImage frame2(capture.get_left_Frame());
//     IplImage* gray_fr2 = cvCreateImage( cvGetSize(&frame2), 8, 1 );
//     
//     imageSize = cvGetSize(&frame1);
// 
//     //Thu anh chessboard ve cho viec calib camera	
//     //cvNamedWindow( "camera2", 1 );
//     //cvNamedWindow( "camera1", 1 );
//     cvNamedWindow("corners camera1",1);
//     //cvNamedWindow("corners camera2",1);	
//     printf("\nWant to capture %d chessboards for calibrate:", n_boards);	
//     
//     while((successes1<n_boards)||(successes2<n_boards))						
//     {
//         //--------Find and Draw chessboard--------------------------------------------------	
//         if((frame++ % 20) == 0)
//         {
//             //----------------CAM1-------------------------------------------------------------------------------------------------------
//             result1 = cvFindChessboardCorners( &frame1, board_sz,&temp1[0], &count1,CV_CALIB_CB_ADAPTIVE_THRESH|CV_CALIB_CB_FILTER_QUADS);
//             cvCvtColor( &frame1, gray_fr1, CV_BGR2GRAY );
//             //cvEqualizeHist(gray_fr1,gray_fr1);
//             
//             
//             cout << result1 << endl;
// 
//             //----------------CAM2--------------------------------------------------------------------------------------------------------
//             result2 = cvFindChessboardCorners( &frame2, board_sz,&temp2[0], &count2,CV_CALIB_CB_ADAPTIVE_THRESH|CV_CALIB_CB_FILTER_QUADS);
//             cvCvtColor( &frame2, gray_fr2, CV_BGR2GRAY );
//             
//             //cvEqualizeHist(gray_fr2,gray_fr2);
//             
//               
//             cout << result2 << endl;
//             
//             if(count1==n&&count2==n&&result1&&result2)
//             {
//                 cvFindCornerSubPix( gray_fr1, &temp1[0], count1,cvSize(11, 11), 
//                                     cvSize(-1,-1),cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS,30, 0.01) );
//                 cvDrawChessboardCorners( &frame1, board_sz, &temp1[0], count1, result1 );
//                 cvShowImage( "corners camera1", &frame1 );
//                 N1 = points[0].size();
//                 points[0].resize(N1 + n, cvPoint2D32f(0,0));
//                 copy( temp1.begin(), temp1.end(), points[0].begin() + N1 );
//                 ++successes1;
//                 
//                 cvFindCornerSubPix( gray_fr2, &temp2[0], count2,cvSize(11, 11), cvSize(-1,-1),cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS,30, 0.01) );
//                 cvDrawChessboardCorners( &frame2, board_sz, &temp2[0], count2, result2 );
//                 //cvShowImage( "corners camera2", &frame2 );
//                 N2 = points[1].size();
//                 points[1].resize(N2 + n, cvPoint2D32f(0,0));
//                 copy( temp2.begin(), temp2.end(), points[1].begin() + N2 );
//                 ++successes2;
//                 
//                 printf("\nNumber of couple Chessboards were found: %d", successes2);
//             }  
//              else  	
//                 cvShowImage( "corners camera1", &frame1 );
// //                 cvShowImage( "corners camera1", gray_fr1 );	
// //             }
//             
// 
//             capture.capture();
//             frame1 = capture.get_left_Frame();
// //             cvShowImage("Frame_Left", &frame1);
//             
//             frame2 = capture.get_right_Frame();
// //             cvShowImage("Frame_right", &frame2);
//             
//             key = cvWaitKey(30);
//             
//             if(key == 'q' || key == 'Q'){                
//                  destroyAllWindows();
//                 return;
//             }
//         }
//     }
// 
//     //grabb->stereoGrabberStopCam();
//     //capture.stop_cam();
//     
//     destroyAllWindows();
//     
//     
//     printf("\nDone Capture!");
// 
// 
//     //--------------Compute for calibration-------------------
//     N = n_boards*n;
//     objectPoints.resize(N);
//     for( i = 0; i < ny; i++ )
//             for(j = 0; j < nx; j++ )   objectPoints[i*nx + j] = cvPoint3D32f(i*squareSize, j*squareSize, 0);
//     for( i = 1; i < n_boards; i++ ) copy( objectPoints.begin(), objectPoints.begin() + n, objectPoints.begin() + i*n );
//     npoints.resize(n_boards,n);
// 
//     CvMat _objectPoints = cvMat(1, N, CV_32FC3, &objectPoints[0] );
//     CvMat _imagePoints1 = cvMat(1, N, CV_32FC2, &points[0][0] );
//     CvMat _imagePoints2 = cvMat(1, N, CV_32FC2, &points[1][0] );
//     CvMat _npoints = cvMat(1, npoints.size(), CV_32S, &npoints[0] );
//     cvSetIdentity(&_M1calib);
//     cvSetIdentity(&_M2calib);
//     cvZero(&_D1);
//     cvZero(&_D2);
// 
//     printf("\nRunning stereo calibration ...");
//     fflush(stdout);
//     
//     
//     CvTermCriteria term_crit = cvTermCriteria( CV_TERMCRIT_ITER + CV_TERMCRIT_EPS,100,1e-6);
//     
//     cvStereoCalibrate( &_objectPoints, &_imagePoints1, &_imagePoints2, &_npoints,&_M1calib, &_D1, 
//                        &_M2calib, &_D2,imageSize, &_R, &_Tcalib, &_E, &_F,
//         CV_CALIB_FIX_ASPECT_RATIO +
//         CV_CALIB_ZERO_TANGENT_DIST +
//         CV_CALIB_SAME_FOCAL_LENGTH +
//         CV_CALIB_FIX_K3, term_crit );
// 
//     printf("\nDone Calibration");
//     //-------------Chinh meo cho anh------------------------------------------
//     cvUndistortPoints( &_imagePoints1, &_imagePoints1,&_M1calib, &_D1, 0, &_M1calib );
//     cvUndistortPoints( &_imagePoints2, &_imagePoints2,&_M2calib, &_D2, 0, &_M2calib );
// 
//     //--------Tinh toan va canh anh, dung giai thuat bouguet-------------------
//     CvMat* mx1calib = cvCreateMat( imageSize.height,imageSize.width, CV_32F );
//     CvMat* my1calib = cvCreateMat( imageSize.height,imageSize.width, CV_32F );
//     CvMat* mx2calib = cvCreateMat( imageSize.height,imageSize.width, CV_32F );
//     CvMat* my2calib = cvCreateMat( imageSize.height,imageSize.width, CV_32F );
// 
//     double R1[3][3], R2[3][3], P1[3][4], P2[3][4];
//     CvMat _R1 = cvMat(3, 3, CV_64F, R1);
//     CvMat _R2 = cvMat(3, 3, CV_64F, R2);
//     //Calib dung Bouguet
//     CvMat _P1 = cvMat(3, 4, CV_64F, P1);
//     CvMat _P2 = cvMat(3, 4, CV_64F, P2);
//     cvStereoRectify( &_M1calib, &_M2calib, &_D1, &_D2, imageSize,&_R, &_Tcalib,&_R1, &_R2, &_P1, &_P2, &_Qcalib,0/*CV_CALIB_ZERO_DISPARITY*/ );
//     //Tim tham so cho cvRemap()
//     cvInitUndistortRectifyMap(&_M1calib,&_D1,&_R1,&_P1,mx1calib,my1calib);
//     cvInitUndistortRectifyMap(&_M2calib,&_D2,&_R2,&_P2,mx2calib,my2calib);
// 
//     printf("\nSaving matries for later use ...\n");
//     cvSave("./CalibFiles/M1.yml",&_M1calib);
// //	cvSave("CalibFile//D1.yml",&_D1);
// //	cvSave("CalibFile//R1.yml",&_R1);
// //	cvSave("CalibFile//P1.yml",&_P1);
//     cvSave("./CalibFiles/M2.yml",&_M2calib);
// //	cvSave("CalibFile//D2.yml",&_D2);
// //	cvSave("CalibFile//R2.yml",&_R2);
// //	cvSave("CalibFile//P2.yml",&_P2);
//     cvSave("./CalibFiles/Q.yml",&_Qcalib);
//     cvSave("./CalibFiles/T.yml",&_Tcalib);
//     cvSave("./CalibFiles/mx1.yml",mx1calib);
//     cvSave("./CalibFiles/my1.yml",my1calib);
//     cvSave("./CalibFiles/mx2.yml",mx2calib);
//     cvSave("./CalibFiles/my2.yml",my2calib);
//    
// }



// void StereoVision::initialization()
// {
//     //read  stereo calib file
//     // Information for Calibration File.
//     // Calibration File Format.
//     // (1) M1: Camera Matrix for camera 1 (left) [3][3] row by row
//     // (2) M2: Camera Matrix for camera 2 (right) [3][3] row by row
//     cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, 1, 1, 0, 1.4f, CV_AA);
//     
//     _M1 = (CvMat *)cvLoad("./CalibFiles/M1.yml");
//     
//     if (_M1 == NULL)
//     {
//          std::cerr << "--(!)Error loading ./CalibFiles/M1.yml\n";
//          return;        
//     }
//         
//     _M2 = (CvMat *)cvLoad("./CalibFiles/M2.yml");
//     
//     if (_M2 == NULL)
//     {
//          std::cerr << "--(!)Error loading ./CalibFiles/M2.yml\n";
//          return;        
//     }   
//         
//     _T  = (CvMat *)cvLoad("./CalibFiles/T.yml");
//     
//     if (_T == NULL)
//     {
//          std::cerr << "--(!)Error loading ./CalibFiles/T.yml\n";
//          return;        
//     }
//     
//     mx1 = (CvMat *)cvLoad("./CalibFiles/mx1.yml");
//     
//     if (mx1 == NULL)
//     {
//          std::cerr << "--(!)Error loading ./CalibFiles/mx1.yml\n";
//          return;        
//     }
//     
//     my1 = (CvMat *)cvLoad("./CalibFiles/my1.yml");
//     
//     if (my1 == NULL)
//     {
//          std::cerr << "--(!)Error loading ./CalibFiles/my1.yml\n";
//          return;        
//     }
//     
//     mx2 = (CvMat *)cvLoad("./CalibFiles/mx2.yml");
//     
//     if (mx2 == NULL)
//     {
//          std::cerr << "--(!)Error loading ./CalibFiles/mx2.yml\n";
//          return;        
//     }
//         
//     my2 = (CvMat *)cvLoad("./CalibFiles/my2.yml");
//     
//     if (my2 == NULL)
//     {
//          std::cerr << "--(!)Error loading ./CalibFiles/my2.yml\n";
//          return;        
//     }    
//     
//     //_Q = (CvMat *)cvLoad("CalibFile/Q.yml");
//     _CamData = (CvMat *)cvLoad("./CalibFiles/CamData.yml");
//     
//     if (_CamData == NULL)
//     {
//          std::cerr << "--(!)Error loading ./CalibFiles/CamData.yml\n";
//          return;        
//     }   
//     
//     
//     //0: fx(pixel), 1: fy(pixel), 2: B (baseline), 3: f(mm), 4: sensor element size, 5: baseline in mm
//     reprojectionVars[0] = cvmGet(_M1,0,0);
//     reprojectionVars[1] = cvmGet(_M1,0,0);
//     reprojectionVars[2] = (-1)*cvmGet(_T,0,0);
//     reprojectionVars[3] = cvmGet(_CamData, 0, 0);
//     reprojectionVars[4] = cvmGet(_CamData, 0, 1);
//     reprojectionVars[5] = cvmGet(_CamData, 0, 2);
// 
//     cameras->capture();
//         
//     //Loading images
//     img1 = cvCreateImage(cvSize(cameras->getFrameWidth(),cameras->getFrameWidth()), IPL_DEPTH_8U, 1);		
//     img2 = cvCreateImage(cvSize(cameras->getFrameWidth(),cameras->getFrameWidth()), IPL_DEPTH_8U, 1);
//     
//     imageSize = cvSize(img1->width,img1->height);		
//     img1r = cvCreateMat( imageSize.height,imageSize.width, CV_8U );		//rectified left image
//     img2r = cvCreateMat( imageSize.height,imageSize.width, CV_8U );		//rectified right image
//     disp  = cvCreateMat( imageSize.height,imageSize.width, CV_16S );		//disparity map
//     vdisp = cvCreateMat( imageSize.height,imageSize.width, CV_8U );
//     depthM = cvCreateMat(imageSize.height, imageSize.width, CV_32F);
//                     
//     
//     thres_img = cvCreateImage( imageSize, img1->depth, 1);
//     blobs_img = cvCreateImage( imageSize, img1->depth, 3);
//     
//     Mat frame = cameras->get_left_Frame();
//         
//     //img_detect = cvCreateImage( imageSize, grabb->imageLeft->depth, 3);    
//     img_detect = cvCreateImage( imageSize, frame.depth(), 3);
//     r_detect = cvCreateImage(imageSize,8,1);//subpixel
//     r_detect_r = cvCreateImage(imageSize,8,1);
//     g_detect = cvCreateImage(imageSize,8,1);//subpixel
//     g_detect_r = cvCreateImage(imageSize,8,1);
//     b_detect = cvCreateImage(imageSize,8,1);//subpixel
//     b_detect_r = cvCreateImage(imageSize,8,1);
//     
//     pair = cvCreateMat( imageSize.height, imageSize.width*2,CV_8UC3 );  
// 
// }
