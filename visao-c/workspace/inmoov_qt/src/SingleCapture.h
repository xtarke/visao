#ifndef SINGLECAPTURE_H
#define SINGLECAPTURE_H

#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

class SingleCapture
{
private:
    VideoCapture camera;
    
    int frame_width;
    int frame_height;
    
    int camIndex;
       
    Mat frame;
    
public:
    
    SingleCapture(int _camIndex, int _frame_width, int _frame_height);
    SingleCapture(int _camIndex);
    ~SingleCapture();
    
    void capture();
    void stop_cam();
    bool start_cam();
    bool isOpen();
    
    int getFrameWidth() {return frame_width;};
    int getFrameHeight() {return frame_height;};
    
    Mat get_Frame() {return frame;}
    
};

#endif // SINGLECAPTURE_H
