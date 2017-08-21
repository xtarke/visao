#include "StereoCapture.h"


StereoCapture::StereoCapture (int _leftCamIndex, int _rightCamIndex, int _frame_width, int _frame_height)
{
//     left_Cam.open(_leftCamIndex);
//     rigth_Cam.open(_rightCamIndex);
//     
//     if(!left_Cam.isOpened() || !rigth_Cam.isOpened()) {
//         cerr << "StereoCapture: could not open both cameras" << endl;        
// #ifdef ABORT_ON_ERROR
//         exit(EXIT_FAILURE);
// #endif
//     }
//         
//     left_Cam.set(CV_CAP_PROP_FRAME_WIDTH,_frame_width);
//     left_Cam.set(CV_CAP_PROP_FRAME_HEIGHT,_frame_height);
//     rigth_Cam.set(CV_CAP_PROP_FRAME_WIDTH,_frame_width);
//     rigth_Cam.set(CV_CAP_PROP_FRAME_HEIGHT,_frame_height);    
     
    frame_height = _frame_height;
    frame_width = _frame_width;   
    
    rightCamIndex = _rightCamIndex;
    leftCamIndex = _leftCamIndex;
    
}

StereoCapture::StereoCapture (int leftCamIndex, int rightCamIndex)
{
    left_Cam.open(leftCamIndex);
    rigth_Cam.open(rightCamIndex);
        
    if(!left_Cam.isOpened() || !rigth_Cam.isOpened()) {
        cerr << "StereoCapture: could not open both cameras" << endl;
#ifdef ABORT_ON_ERROR
        exit(EXIT_FAILURE);
#endif
    }
        
    frame_height = left_Cam.get(CV_CAP_PROP_FRAME_HEIGHT);
    frame_width = rigth_Cam.get(CV_CAP_PROP_FRAME_WIDTH);    
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


bool StereoCapture::isOpen()
{
    if(!left_Cam.isOpened() || !rigth_Cam.isOpened())
        return false;
    
    
    return true;   
}

bool StereoCapture::start_cam() 
{
    left_Cam.open(leftCamIndex);
    rigth_Cam.open(rightCamIndex);
    
    if(!left_Cam.isOpened() || !rigth_Cam.isOpened()) {
        cerr << "StereoCapture: could not reopen both cameras" << endl;
#ifdef ABORT_ON_ERROR
        exit(EXIT_FAILURE);
#endif
        return false;
    }
        
    
    if (frame_height != 0 && frame_width != 0) {
        left_Cam.set(CV_CAP_PROP_FRAME_WIDTH, frame_width);
        left_Cam.set(CV_CAP_PROP_FRAME_HEIGHT, frame_height);
        rigth_Cam.set(CV_CAP_PROP_FRAME_WIDTH, frame_width);
        rigth_Cam.set(CV_CAP_PROP_FRAME_HEIGHT, frame_height);        
    }
       
    return true;   
}
