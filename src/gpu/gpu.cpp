#include "opencv2/opencv.hpp"
#include "opencv2/gpu/gpu.hpp"

using namespace cv;

int main(){
    Mat src = imread("car.jpg", 0);
    if (!src.data) exit(1);

    gpu::GpuMat d_src(src);
    gpu::GpuMat d_dst;

    gpu::bilateralFilter(d_src, d_dst,15, 80,80);

    Mat dst(d_dst);
    imwrite("out.png", dst);
    return 0;
} 
