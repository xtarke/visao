#ifndef FACEDETECTION_H
#define FACEDETECTION_H

#include "StereoCapture.h"

class FaceDetection
{

public:
    
    struct FacePosition{
        float x;
        float y;       
        bool detected;
    };    
    
    FaceDetection(StereoCapture &stereoCapture);
    FaceDetection(int _frame_width, int _frame_height);    
    
    Mat detect(Mat frame, FacePosition *pos);
    
private:
    StereoCapture *cameras; //must be removed
    
    
    int frame_width;
    int frame_height;
    
    CascadeClassifier face_cascade;
    CascadeClassifier eyes_cascade;
    
    Mat detectAndDisplay(Mat frame, FacePosition *pos);
    
};

#endif // FACEDETECTION_H
