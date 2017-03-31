#ifndef FACEDETECTION_H
#define FACEDETECTION_H

#include "StereoCapture.h"

class FaceDetection
{

public:
    
    struct FacePosition{
        float x;
        float y;       
    };    
    
    FaceDetection(StereoCapture &capture);    
    Mat detect(Mat frame, FacePosition *pos);
    
private:
    StereoCapture *cameras;
    
    CascadeClassifier face_cascade;
    CascadeClassifier eyes_cascade;
    
    Mat detectAndDisplay(Mat frame, FacePosition *pos);
    
};

#endif // FACEDETECTION_H
