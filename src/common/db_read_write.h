#pragma once

namespace pierry{
    /*!
     特徴点をファイルから読み込む
     @param[in] path ファイルパス
     @return 特徴点
     */
    std::vector<cv::KeyPoint> read_key_points(boost::filesystem::path const & path);
    
    /*!
     特徴量をファイルから読み込む
     @param[in] path ファイルパス
     @return 特徴量
     */
    cv::Mat read_features(boost::filesystem::path const & path);
    
    /*!
     特徴点をファイルへ書き込む
     @param[in] src 特徴点
     @param[in] path ファイルパス
     */
    void write_key_points(std::vector<cv::KeyPoint> const & src, boost::filesystem::path const & path);
    
    /*!
     特徴量をファイルへ書き込む
     @param[in] src 特徴量
     @param[in] path ファイルパス
     */
    void write_features(cv::Mat const & src, boost::filesystem::path const & path);
}