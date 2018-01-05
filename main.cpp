#include <opencv2/opencv.hpp>
#include <iostream>

int main()
{
    cv::VideoCapture cap(0);
    if(!cap.isOpened()){
        std::cerr << "failed to open camera" << std::endl;
    }
    std::string cascade_path = "./haarcascade_frontalface_default.xml";
    for(;;){
        cv::Mat img;
        cap >> img;
        cv::CascadeClassifier cascade(cascade_path);
        std::vector<cv::Rect> rects;
        cascade.detectMultiScale(img, rects, 1.1, 3, 0, { 100, 100 });
        for(auto rc : rects){
            cv::rectangle(img, rc, cv::Scalar(0, 255, 0));
            std::cout << rc << std::endl;
        }
        cv::imshow("image", img);
        cv::waitKey(1);
    }
    return 0;
}