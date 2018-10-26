#pragma once

#include "pierry.h"
#include "utils.h"

namespace pierry{
	/*!
	データベースを読み込む
	@param[in] db データベースのパス
	@param[in] zoom_ix マスターの縮尺率のインデックス
	@param[out] flann 検索モジュール
	@param[out] features 特徴量
	@param[out] key_points 特徴点
	@param[out] names ファイル名とインデックスのペア
	@param[out] o 進捗通知する出力ストリーム
	*/
	void load_db(
		boost::filesystem::path const & db,
		size_t zoom_ix,
		cv::flann::Index & flann,
		cv::Mat & features,
		std::vector<cv::KeyPoint> & key_points,
		std::vector<name_index_t> & names,
		std::ostream & o);

    /*!
     DBから該当するファイルを検索し、得票点順に指定した数だけ取得する
     @param[in] flann 検索モジュール
     @param[in] features 特徴量
     @param[in] key_points 特徴点
     @param[in] names マッチしたインデックスのファイル名を表す
     @param[in] src 入力画像イメージ
     @param[in] candidate_count 最大候補数
     @return 検索結果
     */
    search_results get_candidates(
        cv::flann::Index & flann,
        cv::Mat const & features,
        std::vector<cv::KeyPoint> const & key_points,
        std::vector<name_index_t> const & names,
        cv::Mat const & src,
        int candidate_count,
        db_info const & op);
}