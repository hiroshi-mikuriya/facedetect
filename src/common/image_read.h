#pragma once

namespace pierry{
    /*!
     マスター画像を読み込む（ズームの値で縮尺する）
     @param[in] path ファイルパス
	 @param[in] zoom ズーム
     @return マスター画像イメージ
	 @throw std::invalid_argument 画像が開けないときは例外を投げる
	 */
    cv::Mat read_master_with_zoom(boost::filesystem::path const & path, double zoom);
    
	/*!
	マスター画像を読み込む（長辺のピクセル数をwidthの値に調整する）
	@param[in] path ファイルパス
	@param[in] width 長辺のピクセル数
	@return マスター画像イメージ
	@throw std::invalid_argument 画像が開けないときは例外を投げる
	*/
	cv::Mat read_master_with_width(boost::filesystem::path const & path, int width);
	
	/*!
	サンプル画像を読み込む（ズームの値で縮尺する）
	@param[in] path ファイルパス
	@param[in] zoom ズーム
	@return サンプル画像イメージ
	@throw std::invalid_argument 画像が開けないときは例外を投げる
	*/
    cv::Mat read_sample_with_zoom(boost::filesystem::path const & path, double zoom);

	/*!
	サンプル画像を読み込む（長辺のピクセル数をwidthの値に調整する）
	@param[in] path ファイルパス
	@param[in] width 長辺のピクセル数
	@return サンプル画像イメージ
	@throw std::invalid_argument 画像が開けないときは例外を投げる
	*/
	cv::Mat read_sample_with_width(boost::filesystem::path const & path, int width);
}