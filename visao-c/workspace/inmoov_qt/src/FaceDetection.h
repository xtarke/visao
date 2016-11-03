#ifndef FACEDETECTION_H
#define FACEDETECTION_H

#include "StereoCapture.h"

class FaceDetection
{
private:
    StereoCapture *cameras;
    
    CascadeClassifier face_cascade;
    CascadeClassifier eyes_cascade;
    
    void detectAndDisplay(Mat frame);
    
public:
    FaceDetection(StereoCapture &capture);
    
    void detect();  
    
};

#endif // FACEDETECTION_H
