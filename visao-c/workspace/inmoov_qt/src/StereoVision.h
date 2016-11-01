#ifndef STEREOVISION_H
#define STEREOVISION_H

#include "opencv2/opencv.hpp"
#include "StereoCapture.h"

class StereoVision
{
    
    CvMat* _M1;
    CvMat* _M2;
    CvMat* _T;
    CvMat* mx1; 
    CvMat* mx2;
    CvMat* my1; 
    CvMat* my2; 
    CvMat* _Q;			//reprojection matrix
    CvMat* _CamData;

    CvSize imageSize;	//size of input images 
    IplImage	*img1,	//left image
            *img2,	//right image
            *rgb,
            *thres_img,
            *blobs_img,
            *img_detect,
            *real_disparity;
    CvMat	*img1r,		//rectified left image
                    *img2r,		//rectified right image
                    *disp,		//disparity map
                    *vdisp,		//scaled disparity map for viewing
                    *pair,
                    *depthM;
                    
    int stereoPreFilterSize, 
		stereoPreFilterCap,  
		stereoDispWindowSize,
		stereoNumDisparities,
		stereoDispTextureThreshold,
		stereoDispUniquenessRatio,
		stereoSavePointCloudValue, 
		stereoSaveOriginal;
    
    int threshold, blobArea;
                    
public:
    
    StereoVision();
    ~StereoVision();
    
    void calibrate(StereoCapture &capture);
    void load_correlation(string FileName);
                    
    
};

#endif // STEREOVISION_H
