#include <iostream>

#include "StereoCapture.h"
#include "StereoVision.h"

#include "opencv2/opencv.hpp"

int main(int argc, char **argv) {
    
    if (argc < 3) {
        cerr << "Insuficient parameters: capture <integer::cam_id> <integer::cam_id>" << endl;
        exit(EXIT_FAILURE);
    }
    
    String args_1(argv[1]);
    String args_2(argv[2]);
    
    //StereoCapture capture (stoi(args_1), stoi(args_2),640, 480);
    //StereoCapture capture (stoi(args_1), stoi(args_2));       
    
    
   // capture.capture();
    
    //StereoVision vision;
    const std::string windowName = "Hough Circle Detection Demo";
    const std::string cannyThresholdTrackbarName = "Canny threshold";
    
     int cannyThreshold = 0;
    
    //namedWindow(NULL, WINDOW_AUTOSIZE );
    createTrackbar(cannyThresholdTrackbarName, windowName, &cannyThreshold,255);
    
    //imshow( windowName);
    
    waitKey(10);
    //vision.calibrate(capture);
    
    
    return 0;
}
