#ifndef STEREOCAPTURE_H
#define STEREOCAPTURE_H

#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

class StereoCapture
{
private:
    VideoCapture left_Cam;
    VideoCapture rigth_Cam;
    
    int frame_width;
    int frame_height;
    
    int leftCamIndex;
    int rightCamIndex;
    
    Mat left_Frame;
    Mat right_Frame;
    
public:
    
    StereoCapture (int leftCamIndex, int rightCamIndex, int _frame_width, int _frame_height);
    StereoCapture (int leftCamIndex, int rightCamIndex);
    ~StereoCapture();
    
    void capture();
    void stop_cam();
    bool start_cam();
    bool isOpen();
    
    int getFrameWidth() {return frame_width;};
    int getFrameHeight() {return frame_height;};
    
    Mat get_left_Frame() {return left_Frame;}
    Mat get_right_Frame() {return right_Frame;}   
    
};

#endif // STEREOCAPTURE_H
