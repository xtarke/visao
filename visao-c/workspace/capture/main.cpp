
#include <iostream>
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

int main(int argc, char **argv) {
    
    int camera_id, frames = 0;
    VideoCapture camera;
    
    
    if (argc < 2) {
        cerr << "Insuficient parameters: capture <integer::cam_id>" << endl;
        exit(EXIT_FAILURE);
    }
    
    String args(argv[1]);
    
    camera.open(stoi(args));
    
    camera.set(CV_CAP_PROP_FRAME_WIDTH,1280);
    camera.set(CV_CAP_PROP_FRAME_HEIGHT,720);
    
    while(1) {
         Mat frame;
         char key;
                  
         //capture one frame
         camera >> frame;
        
         // Show image for feedback
         imshow("frame", frame);
        
         
         //wait for a key for 30ms: should be called render images on imshow();
         key = (char) waitKey(30);
         
         if(key >= 0){
            
            //Stop if ESQ key
            if (key == 27) break;
            
            //Save image on space key
            if (key == ' '){
                
                String frame_name("Frame_");
                frame_name += to_string(frames);
                frame_name += ".png";               
                               
                cout << "Saving frame to: " << frame_name << endl;
                
                imwrite(frame_name, frame);
                
                frames++;                
            }           
         }
    } 
   
    camera.release();   
   
    return 0;
}
