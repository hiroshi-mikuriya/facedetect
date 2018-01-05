#include <opencv2/opencv.hpp>
#include <iostream>

int main()
{
    cv::VideoCapture cap(0);
    if(!cap.isOpened()){
        std::cerr << "failed to open camera" << std::endl;
    }
    for(;;){
        cv::Mat img;
        cap >> img;
        cv::imshow("image", img);
        cv::waitKey(1);
    }
    return 0;
}