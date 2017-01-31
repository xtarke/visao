#ifndef FACEDETECTION_H
#define FACEDETECTION_H

#include "StereoCapture.h"

class FaceDetection
{
private:
    StereoCapture *cameras;
    
    CascadeClassifier face_cascade;
    CascadeClassifier eyes_cascade;
    
    Mat detectAndDisplay(Mat frame);
    
public:
    FaceDetection(StereoCapture &capture);
    
    Mat detect(Mat frame);  
    
};

#endif // FACEDETECTION_H
