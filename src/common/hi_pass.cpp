#include "stdafx.h"
#include "hi_pass.h"
//#include <omp.h>

namespace {
inline auto right(cv::Rect const& rc) -> int { return rc.x + rc.width; }

inline auto bottom(cv::Rect const& rc) -> int { return rc.y + rc.height; }

inline auto center(cv::Rect const& rc) -> cv::Point
{
    return { rc.x + rc.width / 2, rc.y + rc.height / 2 };
}

cv::Mat gray_32f(cv::Mat const& img)
{
    if (img.channels() == 3) {
        cv::Mat result;
        img.convertTo(result, CV_32FC3);
        cv::cvtColor(result, result, CV_BGR2GRAY);
        return result;
    } else if (img.channels() == 1) {
        cv::Mat result;
        img.convertTo(result, CV_32FC1);
        return result;
    } else {
        throw std::runtime_error("unexpected image type.");
    }
}
}

auto pierry::adaptive_flatten(cv::Mat const& img, int kernelSize0, double denoise)->cv::Mat
{
    auto partial_sum = [](cv::Mat const& m, cv::Rect const& rc) -> double {
		return m.at<double>(rc.y, rc.x) - m.at<double>(rc.y, right(rc)) - m.at<double>(bottom(rc), rc.x) + m.at<double>(bottom(rc), right(rc));
    };
    cv::Mat sum, sqSum;
    cv::integral(img, sum, sqSum);
    int const kernelSize = kernelSize0 | 1; // 奇数にする
    cv::Mat r = cv::Mat::zeros(img.rows - kernelSize, img.cols - kernelSize, CV_32FC1);
    int const kernelPixCount = kernelSize * kernelSize;
    const int img_rows = img.rows;
    //#pragma omp parallel for
    for (int bottom = kernelSize; bottom < img_rows; ++bottom) {
        for (int right = kernelSize; right < img.cols; ++right) {
            cv::Rect rc(right - kernelSize, bottom - kernelSize, kernelSize, kernelSize);
            double mean = partial_sum(sum, rc) / kernelPixCount;
            double sigma2 = partial_sum(sqSum, rc) / kernelPixCount;
            double sigma = std::sqrt(sigma2);
            r.at<float>(rc.tl()) = static_cast<float>((img.at<float>(center(rc)) - mean) / (sigma + denoise));
        }
    }
    return r;
}

auto pierry::hipass_filtered_image(cv::Mat const& img0, int kernelSize, double denoise, int rtype)->cv::Mat
{
    auto img = gray_32f(img0);
    auto eb = adaptive_flatten(img, kernelSize, denoise);
    float minval = -0.75f; // z得点 -0.75 (偏差値42.5)以下は黒
    float maxval = -0.05f; // z得点 -0.05 (偏差値49.5)以上は白
    cv::Mat g = (eb - minval) * (255 / (maxval - minval));
    cv::Mat g8;
    g.convertTo(g8, rtype);
    return g8;
}
