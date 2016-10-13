#include <iostream>

#include "StereoCapture.h"


int main(int argc, char **argv) {
    
    if (argc < 3) {
        cerr << "Insuficient parameters: capture <integer::cam_id> <integer::cam_id>" << endl;
        exit(EXIT_FAILURE);
    }
    
    String args_1(argv[1]);
    String args_2(argv[2]);
    
    StereoCapture capture (stoi(args_1), stoi(args_2),1280, 720);
    
    
    std::cout << "Hello, world!" << std::endl;
    return 0;
}
