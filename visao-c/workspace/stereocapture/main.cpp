#include <iostream>
#include <sstream>
#include <iomanip>

#include "opencv2/opencv.hpp"


using namespace cv;
using namespace std;

int main(int argc, char **argv) {
    
    int frames = 0;
    VideoCapture camera_1;
    VideoCapture camera_2;
        
    if (argc < 3) {
        cerr << "Insuficient parameters: capture <integer::cam_id> <integer::cam_id>" << endl;
        exit(EXIT_FAILURE);
    }
    
    String args_1(argv[1]);
    String args_2(argv[2]);
    
    camera_1.open(stoi(args_1));
    camera_2.open(stoi(args_2));
    
    camera_1.set(CV_CAP_PROP_FRAME_WIDTH,640);
    camera_1.set(CV_CAP_PROP_FRAME_HEIGHT,480);
    camera_2.set(CV_CAP_PROP_FRAME_WIDTH,640);
    camera_2.set(CV_CAP_PROP_FRAME_HEIGHT,480);
    
    while(1) {
         Mat frame_1;
         Mat frame_2;
         Mat frame_3;
         char key;
                  
         //capture one frame
         camera_1 >> frame_1;
         camera_2 >> frame_2;
        
         hconcat(frame_1, frame_2, frame_3);
        
         // Show image for feedback
         imshow("frame", frame_3);
       
         
         //wait for a key for 30ms: should be called render images on imshow();
         key = (char) waitKey(30);
         
         if(key >= 0){
            
            //Stop if ESQ key
            if (key == 27) break;
            
            //Save image on space key
            if (key == ' '){
                
                stringstream ss;
                ss << setw(5) << setfill('0') << frames;
                
                String s = ss.str();
                
                String frame_left("left_");
                String frame_right("right_");
              
                frame_left += s + ".png";
                frame_right += s + ".png";
                                                   
                cout << "Saving frame to: " << frame_left << endl;
                cout << "Saving frame to: " << frame_right << endl;
                
                imwrite(frame_left, frame_1);
                imwrite(frame_right, frame_2);
                
                frames++;                
            }           
         }
    } 
   
    camera_1.release();   
    camera_2.release();
   
    return 0;
}
