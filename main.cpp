#include <opencv2/opencv.hpp>
#include "mycamera.hpp"

int main()
{
    MyCamera cap(0);
    if(!cap.isOpened()){
        std::cerr << "failed to open camera" << std::endl;
    }
    cv::CascadeClassifier cascade("./haarcascade_frontalface_default.xml");
    for(;;){
        cv::Mat img;
        cap >> img;
        std::vector<cv::Rect> rects;
        cascade.detectMultiScale(img, rects, 1.1, 10, 0, { 100, 100 });
        for(auto rc : rects){
            cv::rectangle(img, rc, cv::Scalar(0, 255, 0));
        }
        cv::imshow("image", img);
        cv::waitKey(1);
    }
    return 0;
}