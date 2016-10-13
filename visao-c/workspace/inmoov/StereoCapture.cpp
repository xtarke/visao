#include "StereoCapture.h"



StereoCapture::StereoCapture (int leftCamIndex, int rightCamIndex, int _frame_width, int _frame_height)
{
    left_Cam.open(leftCamIndex);
    rigth_Cam.open(rightCamIndex);
    
    if(!left_Cam.isOpened() || !rigth_Cam.isOpened()) {
        cerr << "StereoCapture: could not open both cameras" << endl;
        exit(EXIT_FAILURE);
    }
        
    
    left_Cam.set(CV_CAP_PROP_FRAME_WIDTH,_frame_width);
    left_Cam.set(CV_CAP_PROP_FRAME_HEIGHT,_frame_height);
    rigth_Cam.set(CV_CAP_PROP_FRAME_WIDTH,_frame_width);
    rigth_Cam.set(CV_CAP_PROP_FRAME_HEIGHT,_frame_height);    
    
    frame_height = _frame_height;
    frame_width = _frame_width;   
    
}

StereoCapture::~StereoCapture(){
    left_Cam.release();   
    rigth_Cam.release();     
}

void StereoCapture::stop_cam(){
    left_Cam.release();   
    rigth_Cam.release();       
}

void StereoCapture::capture()
{
    //capture one frame
    left_Cam >> left_Frame;
    rigth_Cam >> right_Frame;     
}

