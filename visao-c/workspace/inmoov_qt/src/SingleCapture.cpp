#include "SingleCapture.h"


SingleCapture::SingleCapture(int _camIndex, int _frame_width, int _frame_height)
{    
    frame_height = _frame_height;
    frame_width = _frame_width;   
    
    camIndex = _camIndex;   
}

SingleCapture::SingleCapture(int _camIndex)
{
    camera.open(_camIndex);
        
    if(!camera.isOpened()) {
        cerr << "SingleCapture: could not open camera cv:id " << _camIndex << endl;
#ifdef ABORT_ON_ERROR
        exit(EXIT_FAILURE);
#endif
    }
        
    frame_height = camera.get(CV_CAP_PROP_FRAME_HEIGHT);
    frame_width = camera.get(CV_CAP_PROP_FRAME_WIDTH);    
}

SingleCapture::~SingleCapture(){
    camera.release();
}

void SingleCapture::stop_cam(){
    camera.release();
}

void SingleCapture::capture()
{
    //capture one frame
    camera >> frame;
}


bool SingleCapture::isOpen()
{
    if(!camera.isOpened())
        return false;
    
    
    return true;   
}

bool SingleCapture::start_cam() 
{
    camera.open(camIndex);
  
    if(!camera.isOpened()) {
        cerr << "SingleCapture: could not reopen camera cv::id " << camIndex << endl;
#ifdef ABORT_ON_ERROR
        exit(EXIT_FAILURE);
#endif
        return false;
    }
        
    
    if (frame_height != 0 && frame_width != 0) {
        camera.set(CV_CAP_PROP_FRAME_WIDTH, frame_width);
        camera.set(CV_CAP_PROP_FRAME_HEIGHT, frame_height);
    }
       
    return true;   
}
