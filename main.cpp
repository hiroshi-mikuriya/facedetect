#include <opencv2/opencv.hpp>
#include "mycamera.hpp"

namespace
{
    cv::Rect operator*(cv::Rect const & rc, double r)
    {
        return cv::Rect(static_cast<int>(r * rc.x), static_cast<int>(r * rc.y), static_cast<int>(r * rc.width), static_cast<int>(r * rc.height));
    }
    cv::Rect operator/(cv::Rect const & rc, double r)
    {
        return rc * (1 / r);
    }
    cv::Size operator*(cv::Size const & sz, double r)
    {
        return cv::Size(static_cast<int>(r * sz.width), static_cast<int>(r * sz.height));
    }
}

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
        double r = 0.25;
        std::vector<cv::Rect> rects;
        cv::Mat rimg;
        cv::resize(img, rimg, cv::Size(), r, r);
        cascade.detectMultiScale(rimg, rects, 1.1, 5, 0, cv::Size(100, 100) * r);
        for(auto rc : rects){
            cv::rectangle(img, rc / r, cv::Scalar(0, 255, 0));
        }
        cv::imshow("image", img);
        cv::waitKey(1);
    }
    return 0;
}