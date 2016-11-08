#ifndef STEREOVISION_H
#define STEREOVISION_H

#include "opencv2/opencv.hpp"
#include "StereoCapture.h"

class StereoVision
{
    
private:
    
    StereoCapture *cameras;
    
    cv::Mat cameraMatrix[2], distCoeffs[2];
    cv::Mat R, T, E, F;
    
    cv::Mat R1, R2, P1, P2, Q;
    Rect validRoi[2];
    
    
    // void initialization();
  
    void stereoDetect();
                    
public:
    
    StereoVision(StereoCapture &capture);
    ~StereoVision();
    
    void calibrate(StereoCapture &capture);
    void load_correlation(string FileName);
                    
    void StereoCalib(const vector<Mat>& imagelist, Size boardSize, float squareSize, 
                     bool displayCorners = false, bool useCalibrated=true, bool showRectified=true);
    
    bool loadCameraParameters(void);
    
    void stereoCorrelation();
};

#endif // STEREOVISION_H
