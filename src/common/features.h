#pragma once

#include "pierry.h"

namespace pierry{

	/*!
	特徴点と特徴量
	*/
	struct feature_points
	{
		std::vector<cv::KeyPoint> m_key_points;///< 特徴点.
		cv::Mat m_features;///< 特徴量

		bool empty() const {
			return m_key_points.empty();
		}
		/*!
		画像イメージの特徴点、特徴量を求める
		@param[in] src 画像イメージ
		@param[in] orb ORBパラメータ
		@param[in] freak FREAKパラメータ
		*/
		feature_points(cv::Mat const & src, orb_params const & orb, freak_params const & freak);

		feature_points(std::vector<cv::KeyPoint> key_points, cv::Mat features) : m_key_points(key_points), m_features(features){}
		feature_points() = default;
	};
}