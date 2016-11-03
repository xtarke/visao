#include "FaceDetection.h"
#include "opencv2/opencv.hpp"

FaceDetection::FaceDetection(StereoCapture &capture){
    
    cameras = &capture;
    
    //-- 1. Load the cascade
    if( !face_cascade.load("lbpcascade_frontalface.xml") ){ 
        std::cerr << "--(!)Error loading face cascade: lbpcascade_frontalface.xml\n";
    }
}

void FaceDetection::detect() {
    
    if (face_cascade.empty())  {
        std::cerr << "--(!)Error loading face cascade: lbpcascade_frontalface.xml\n";
        return;
    }
    
    if (cameras->isOpen()){
        std::cerr << "--(!)Cameras not ready\n";
        return;
    }
       
}

void FaceDetection::detectAndDisplay( Mat frame )
{
    std::vector<Rect> faces;
    Mat frame_gray;

    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );

    //-- Detect faces
    face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0, Size(80, 80) );

    for( size_t i = 0; i < faces.size(); i++ )
    {
        Mat faceROI = frame_gray( faces[i] );
        std::vector<Rect> eyes;

        //-- In each face, detect eyes
        eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CASCADE_SCALE_IMAGE, Size(30, 30) );
        if( eyes.size() == 2)
        {
            //-- Draw the face
            Point center( faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2 );
            ellipse( frame, center, Size( faces[i].width/2, faces[i].height/2 ), 0, 0, 360, Scalar( 255, 0, 0 ), 2, 8, 0 );
        }

    }
    //-- Show what you got
    imshow( "Face", frame );
}
