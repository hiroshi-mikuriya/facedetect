#include <opencv2/opencv.hpp>
#include "mycamera.hpp"

int main()
{
    MyCamera camera(0);
    if(!camera.isOpened()){
        std::cerr << "failed to open camera" << std::endl;
    }
    cv::CascadeClassifier cascade("./haarcascade_frontalface_default.xml");
    for(;;){
        cv::Mat img;
        camera >> img;
        if(img.empty()){
            std::cerr << "failed to get camera image." << std::endl;
            continue;
        }
        double r = 0.2;
        std::vector<cv::Rect> rects;
        cv::Mat rimg;
        cv::resize(img, rimg, cv::Size(), r, r);
        cv::Size min(static_cast<int>(100 * r), static_cast<int>(100 * r));
        cascade.detectMultiScale(rimg, rects, 1.1, 10, 0, min);
        for(auto rc : rects){
            cv::Rect rc2 = { 
                static_cast<int>(rc.x / r), 
                static_cast<int>(rc.y / r), 
                static_cast<int>(rc.width / r), 
                static_cast<int>(rc.height / r)
            };
            cv::rectangle(img, rc2, cv::Scalar(0, 255, 0));
        }
        cv::imshow("image", img);
        cv::waitKey(1);
    }
    return 0;
}