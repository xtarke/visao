
/* Dlib face detection: *
 *  
 * based on public example */


#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <dlib/opencv/cv_image.h>
#include <iostream>


#include "opencv2/opencv.hpp"

using namespace cv;
using namespace dlib;
using namespace std;

int main(int argc, char** argv)
{
    
     if (argc == 1)
    {
        cout << "Call this program like this:" << endl;
        cout << "./dlib_face shape_predictor_68_face_landmarks.dat <video_id>" << endl;
        cout << "\nYou can get the shape_predictor_68_face_landmarks.dat file from:\n";
        cout << "http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2" << endl;
        return 0;
    }
            
    VideoCapture cap(0); // open the default camera
   
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    Mat edges;
    namedWindow("edges",1);
    
     // We need a face detector.  We will use this to get bounding boxes for
    // each face in an image.
    frontal_face_detector detector = get_frontal_face_detector();
    // And we also need a shape_predictor.  This is the tool that will predict face
    // landmark positions given an image and face bounding box.  Here we are just
    // loading the model from the shape_predictor_68_face_landmarks.dat file you gave
    // as a command line argument.
    shape_predictor sp;
    deserialize(argv[1]) >> sp;
    
    for(;;)
    {
        Mat frame;
        Mat edges;
        cap >> frame; // get a new frame from camera
        cvtColor(frame, edges, COLOR_BGR2GRAY);
        
        //array2d<rgb_pixel> img(frame);
        //load_image(img, cv_image(frame));
        array2d<rgb_pixel> img;
        assign_image(img, cv_image<bgr_pixel>(edges));
        
        // Make the image larger so we can detect small faces.
        //pyramid_up(img);
        
        std::vector<dlib::rectangle> dets = detector(img);
        cout << "Number of faces detected: " << dets.size() << endl;
        
        //GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
        //Canny(edges, edges, 0, 30, 3);
        imshow("edges", edges);
        if(waitKey(30) == 'q') break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
} 
