#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace cv;

/** @function main */
int main(int argc, char** argv)
{
	VideoCapture cap(1); // open the default camera
	if(!cap.isOpened())  // check if we succeeded
		return -1;

	Mat frame;
	Mat gray_mat;
	Mat canny_mat;

	namedWindow("edges",1);
	for(;;)
	{
		Mat frame;
		std::vector<Vec3f> circles;

		cap >> frame; // get a new frame from camera

		cvtColor(frame, gray_mat, COLOR_BGR2GRAY);
		GaussianBlur(gray_mat, gray_mat, Size(7,7), 1.5, 1.5);


		 // Apply the Hough Transform to find the circles
		 HoughCircles( gray_mat, circles, CV_HOUGH_GRADIENT, 1,
				 gray_mat.rows/1, 200, 100, 0, 0 );


		 Canny(gray_mat, canny_mat, 0, 30, 3);

		 /// Draw the circles detected
		 for( size_t i = 0; i < circles.size(); i++ )
		 {
		     Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		     int radius = cvRound(circles[i][2]);
		     // circle center
		     circle(canny_mat, center, 3, Scalar(0,255,0), -1, 8, 0 );
		     // circle outline
		     circle(canny_mat, center, radius, Scalar(0,0,255), 3, 8, 0 );
		  }

		if (circles.size() > 0){
			 std::cout << "Achei" << "\n";

			 imwrite("Gray_Image.jpg", canny_mat );

		}


		imshow("frame", frame);
		if(waitKey(30) >= 0) break;
	}


	// the camera will be deinitialized automatically in VideoCapture destructor
	return 0;
} 

//Mat src, src_gray;
//
// /// Read the image
// src = imread( argv[1], 1 );
//
// if( !src.data )
//   { return -1; }
//
// /// Convert it to gray
// cvtColor( src, src_gray, CV_BGR2GRAY );
//
// /// Reduce the noise so we avoid false circle detection
// GaussianBlur( src_gray, src_gray, Size(9, 9), 2, 2 );
//
// std::vector<Vec3f> circles;
//
// /// Apply the Hough Transform to find the circles
// HoughCircles( src_gray, circles, CV_HOUGH_GRADIENT, 1, src_gray.rows/8, 200, 100, 0, 0 );
//
// /// Draw the circles detected
// for( size_t i = 0; i < circles.size(); i++ )
// {
//     Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
//     int radius = cvRound(circles[i][2]);
//     // circle center
//     circle( src, center, 3, Scalar(0,255,0), -1, 8, 0 );
//     // circle outline
//     circle( src, center, radius, Scalar(0,0,255), 3, 8, 0 );
//  }
//
// /// Show your results
// namedWindow( "Hough Circle Transform Demo", CV_WINDOW_AUTOSIZE );
// imshow( "Hough Circle Transform Demo", src );
//
// waitKey(0);
