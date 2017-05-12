#include "FaceDetection.h"
#include "opencv2/opencv.hpp"

FaceDetection::FaceDetection(StereoCapture &capture){
    
    cameras = &capture;
    
    //-- 1. Load the cascade
    if( !face_cascade.load("lbpcascade_frontalface.xml") ){ 
        std::cerr << "--(!)Error loading face cascade: lbpcascade_frontalface.xml\n";
    }
    
    if( !eyes_cascade.load("haarcascade_eye_tree_eyeglasses.xml") ){ 
        std::cerr << "--(!)Error loading face cascade: lbpcascade_frontalface.xml\n";        
    };
    
    
    frame_width = cameras->getFrameWidth();
    frame_height = cameras->getFrameHeight();
    
}

FaceDetection::FaceDetection(int _frame_width, int _frame_height){
    //-- 1. Load the cascade
    if( !face_cascade.load("lbpcascade_frontalface.xml") ){ 
        std::cerr << "--(!)Error loading face cascade: lbpcascade_frontalface.xml\n";
    }
    
    if( !eyes_cascade.load("haarcascade_eye_tree_eyeglasses.xml") ){ 
        std::cerr << "--(!)Error loading face cascade: lbpcascade_frontalface.xml\n";        
    };
    
    frame_width = _frame_width;
    frame_height = _frame_height;
}


Mat FaceDetection::detect(Mat frame, FacePosition *pos) {
    
    if (face_cascade.empty())  {
        std::cerr << "--(!)Error loading face cascade: lbpcascade_frontalface.xml\n";
        return frame;
    }
    
    if (eyes_cascade.empty())  {
        std::cerr << "--(!)Error loading face cascade: haarcascade_eye_tree_eyeglasses.xml\n";
        return frame;
    }
    
    if (!cameras->isOpen()){
        std::cerr << "--(!)Cameras not ready\n";
        return frame;
    }
    
    return detectAndDisplay(frame, pos);
       
}

Mat FaceDetection::detectAndDisplay(Mat frame, FacePosition *pos)
{
    std::vector<Rect> faces;
    Mat frame_gray;
   
    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );

    //-- Detect faces
    face_cascade.detectMultiScale( frame_gray, faces, 1.1, 5, 0, Size(50, 50) );

    pos->detected = false;
    
    if (faces.size() > 0)
    //for( size_t i = 0; i < faces.size(); i++ )
    {
        Mat faceROI = frame_gray(faces[0]);
        std::vector<Rect> eyes;

        //-- In each face, detect eyes
     //   eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CASCADE_SCALE_IMAGE, Size(30, 30) );
       // if( eyes.size() == 2)
       // {
            //-- Draw the face
            Point center( faces[0].x + faces[0].width/2, faces[0].y + faces[0].height/2 );
            ellipse( frame, center, Size( faces[0].width/2, faces[0].height/2 ), 0, 0, 360, Scalar( 255, 0, 0 ), 2, 8, 0 );
            
            pos->x =  ((faces[0].x + faces[0].width/2) / (float)frame_width);
            pos->y = ((faces[0].y + faces[0].height/2) / (float)frame_height);  
            
            pos->detected = true;
            
        //}
    }
    
    return frame;
}
