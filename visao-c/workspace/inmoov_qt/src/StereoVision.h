#ifndef STEREOVISION_H
#define STEREOVISION_H

#include "opencv2/opencv.hpp"
#include "StereoCapture.h"

class StereoVision
{
    StereoCapture *cameras;
    
    Mat cameraMatrix[2], distCoeffs[2];
    Mat R, T, E, F;
    
    // void initialization();
  
    void stereoDetect();
                    
public:
    
    StereoVision(StereoCapture &capture);
    ~StereoVision();
    
    void calibrate(StereoCapture &capture);
    void load_correlation(string FileName);
                    
    void StereoCalib(const vector<Mat>& imagelist, Size boardSize, float squareSize, 
                     bool displayCorners = false, bool useCalibrated=true, bool showRectified=true);
    
    void stereoCorrelation();
};

#endif // STEREOVISION_H
