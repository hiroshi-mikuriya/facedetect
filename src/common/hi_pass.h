#pragma once

namespace pierry{
    /**
     * @param [in] img 入力画像。CV_32FC1 であること。
     * @param [in] kernelSize0 平均や分散をとる領域の大きさ。画像の短辺の 1/6 ぐらいが良いと思う。
     * @param [in] denoise ノイズを抑えるための値。20ぐらいがいいと思う。
     * @return 各画素の周辺の値の偏差値（厳密には、標準得点）を画素とした画像。
     * @note kernelSize の分、入力画像より出力画像が小さくなる。
     */
    auto adaptive_flatten(cv::Mat const & img, int kernelSize0, double denoise)->cv::Mat;
    
    /*!
     * ハイパスフィルタ
     * 白い紙を撮影した画像から、裏写りや照明のムラを排除した画像を作る。
     * @param [in] img0 入力画像
     * @param [in] kernelSize 平均や分散をとる領域の大きさ。画像の短辺の 1/6 ぐらいが良いと思う。
     * @param [in] denoise ノイズを抑えるための値。20ぐらいがいいと思う。
     * @param [in] rtype 出力する画像の形式。普通は CV_8U
     * @return ハイパスフィルタ実施済みの画像
     * @note kernelSize の分、入力画像より出力画像が小さくなる。
     */
    auto hipass_filtered_image(cv::Mat const & img0, int kernelSize, double denoise, int rtype)->cv::Mat;
    
    
    inline auto
    hipass_filtered_image(cv::Mat const & img, int kernelSize, double denoise)->cv::Mat
    {
        return hipass_filtered_image(img, kernelSize, denoise, CV_8UC1);
    }
    
    inline auto
    hipass_filtered_image(cv::Mat const & img)->cv::Mat
    {
        auto kernelSize = std::min(img.cols, img.rows) / 6;
        return hipass_filtered_image(img, kernelSize, 20, CV_8UC1);
    }
    
}
