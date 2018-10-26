#include "stdafx.h"
#include <array>
#include "identify.h"
#include "update.h"
#include "utils.h"
#include "define.h"
#include "image_read.h"
#include "features.h"
#include <mutex>

//#define SHOW_BOTH_SCORES 1
//#define CSV_STDOUT 1
//#define SECOND_TEST_EXP

namespace
{
	/*!
	特徴量のインデックスと対応するファイル名を取得する.
	@param[in] names ファイル名とインデックス数のペア.
	@param[in] ix インデックス.
	@return ファイル名、サブDBのペア.
	*/
	pierry::file_identifier get_matched_filename(std::vector<pierry::name_index_t> const & names, int ix)
	{
		for (auto const & n : names){
			if (n.match(ix)){
				return n.file;
			}
		}
		throw std::runtime_error("invalid index in function get_matched_filename.");
	}

	/*!
	名前別投票数を取得.
	@param[in] indices 投票するインデックス.
	@param[in] dists 投票するインデックスとの距離.
	@param[in] names インデックス名.
	@param[in] calcScore 投票の重みを計算する関数.
	@return 名前別投票数.
	*/
	template<typename ScoreT>
	std::map<pierry::file_identifier, ScoreT> get_vote_score(
		cv::Mat const & indices,
		cv::Mat const & dists,
		std::vector<pierry::name_index_t> const & names,
		std::function<ScoreT(int)> calcScore)
	{
		assert(1 <= indices.cols);
		assert(indices.size() == dists.size());
		std::map<pierry::file_identifier, ScoreT> dst;
		for (int y = 0; y < indices.rows; ++y){
			for (int x = 0; x < indices.cols; ++x){
				int const ix = indices.at<int>(y, x);
				auto const name = get_matched_filename(names, ix);
				dst[name] += calcScore(dists.at<int>(y, x));
			}
		}
		return dst;
	}
}

void pierry::load_db(
	boost::filesystem::path const & db,
	size_t zoom_ix,
	cv::flann::Index & flann,
	cv::Mat & features,
	std::vector<cv::KeyPoint> & key_points,
	std::vector<pierry::name_index_t> & names,
	std::ostream  & o)
{
	auto const sub_dbs = get_sub_db_directories(db);
	o << "loading flann::index." << std::endl;
	for (auto const & sub : sub_dbs) {
		auto const sop = read_subdb_info(sub);
		auto const subdb_name = sub.leaf().string();
		read_features_and_key_points(get_leaf_directory(sub, zoom_ix), sop.master_directory, subdb_name, features, key_points, names);
	}
	o << "creating flann." << std::endl;
	flann.load(features, (db / flann_name(zoom_ix)).string());
//    flann.build(features, cv::flann::SavedIndexParams((db / flann_name(zoom_ix)).string()), cvflann::FLANN_DIST_HAMMING);
}

namespace
{

	/*! コンテナから最初の何個かをとりだしたコンテナを作る。要求の個数が元のコンテナの要素数より多い場合は、ある分だけ返す。.
	@tparam src_container_type もとのコンテナ。vector か deque を想定。ランダムアクセス itor が必要。.
	@tparam dest_container_type 作られるコンテナ。set とかでも。.
	@param[in] c コンテナ。.
	@param[in] req_size 必要な数。c の要素数が req_size より少ない場合は、c の要素数分だけ返す。.
	@return c の先頭 req_size 個が入っている、dest_container_type 型の.
	*/
	template< typename src_container_type, typename dest_container_type = src_container_type >
	auto take_first(src_container_type const & c, size_t req_size)->dest_container_type
	{
		size_t actual_size = req_size < c.size() ? req_size : c.size();
		return dest_container_type(c.begin(), c.begin() + actual_size);
	}

	/*!
	全マスターデータベースから特定ファイルの特徴点、特徴量を求める.
	@param[in] master_db マスターデータベース.
	@param[in] names 全マスターファイル名.
	@param[in] file 特定ファイル名.
	@return 特定ファイルの特徴点、特徴量.
	*/
	pierry::feature_points get_master_from_db(
		pierry::feature_points const & master_db,
		std::vector<pierry::name_index_t> names,
		pierry::file_identifier const & file)
	{
		auto res = *std::find_if(names.begin(), names.end(), [&file](pierry::name_index_t const & n)->bool{
			return n.file == file;	// TODO: ソート済みを利用した２分探索に変えたい.
		});
		assert(res.file == file);
		return{ std::vector<cv::KeyPoint>(master_db.m_key_points.begin() + res.begin, master_db.m_key_points.begin() + res.end),
			master_db.m_features(cv::Rect(0, res.begin, master_db.m_features.cols, res.end - res.begin)) };
	}

	/*!
	１次審査を実施し、候補を絞る.
	@param[in] indices.
	@param[in] dists.
	@param[in] names.
	@param[in] candidate_count 絞る候補数.
	@return １次審査の結果、候補となったファイル.
	*/
	auto first_test(
		cv::Mat const & indices,
		cv::Mat const & dists,
		std::vector<pierry::name_index_t> const & names,
		int candidate_count
		)->pierry::search_results
	{
		using pierry::search_result;
		using pierry::search_results;
		auto score_map = get_vote_score<double>(indices, dists, names, [](int dist) {
			const double sigma = 40;
			return std::exp(-dist*dist / (2.0 * sigma * sigma));
		});
		search_results results;
		typedef decltype(score_map) score_map_t;
		std::transform(score_map.begin(), score_map.end(), std::back_inserter(results),
			[](score_map_t::value_type const & from)->search_result{
			return search_result{ from.first.file_name, from.first.subdb_name, from.second };
		});
		std::sort(results.begin(), results.end(), [](search_result const & a, search_result const & b)->bool{
			return b.score < a.score;
		});
		return take_first(results, candidate_count);
	}
	/*!
	特徴点のペアに関する情報.
	*/
	struct relation{
		int master; ///< マスター上の特徴点のインデックス.
		int sample; ///< サンプル上の特徴点のインデックス.
		int32_t distance; ///< 特徴点の類似度.
	};

	/*!
	近傍度の高い特徴点のペアを集める.
	@param[in] indices 特徴点のインデックスを集めたもの.
	@param[in] dists 特徴点の距離を集めたもの.
	@param[in] count 戻り値に含める要素の数。この数より多く集めるように努力する（もとが少なかったらしからない）.
	@return 「特徴点のペアに関する情報」を集めたもの.
	@note distances を sort しない方が速いかも。.
	*/
	auto get_nearest_relations(cv::Mat const & indices, cv::Mat const &dists, size_t count)->std::vector<relation>
	{
		std::vector<int32_t> distances(dists.begin<int32_t>(), dists.end<int32_t>());
		if (distances.empty()){
			return std::vector<relation>();
		}
		std::sort(distances.begin(), distances.end());
		int32_t limit = distances.at(std::min(distances.size() - 1, count));
		std::vector<relation> relations;
		for (int y = 0; y < indices.rows; ++y){
			for (int x = 0; x < indices.cols; ++x){
				int32_t dist = dists.at<int32_t>(y, x);
				if (dist <= limit){
					relations.push_back({ indices.at<int>(y, x), y, dist });
				}
			}
		}
		return relations;
	}

	/*!
	二組の特徴点の関係.
	*/
	struct remoteness_ratio{
		double ratio; ///<マスター側の2点間の距離と、サンプル側の2点間の距離の比(厳密には atan2).
		double remo;///<マスター側の二点間の距離.
		int32_t dist0, dist1;///<特徴点の類似度（二組あるので変数が二つある）.
	};

	/*!
	remoteness_ratio を整列するための比較演算.
	*/
	bool operator<(remoteness_ratio const & a, remoteness_ratio const & b)
	{
		return a.ratio < b.ratio;
	}

	/*!
	マスター側とサンプル側の特徴点のリストを受け取り、特徴点のペアの関係を調査した結果を返す.
	@param[in] master_points マスター側の特徴点を集めたもの.
	@param[in] sample_points サンプル側の特徴点を集めたもの.
	@param[in] relations 類似度の高い特徴点のペアに関する情報.
	@return 「二組の特徴点の関係」を集めたもの。.
	*/
	auto collect_remoteness_ratios(
		std::vector<cv::KeyPoint> const & master_points,
		std::vector<cv::KeyPoint> const & sample_points,
		std::vector<relation> const & relations)->std::vector<remoteness_ratio>
	{
		const double MIN_DIST = 10;
		std::vector<remoteness_ratio> remoteness_ratios;
		for (size_t ai = 0; ai < relations.size(); ++ai){
			// TODO:なんかきたない。きれいにする.
			auto ra = relations.at(ai);
			auto ma = master_points.at(ra.master);
			auto sa = sample_points.at(ra.sample);
			auto da = relations.at(ai).distance;
			for (size_t bi = 0; bi < ai; ++bi){
				auto rb = relations.at(bi);
				auto mb = master_points.at(rb.master);
				auto sb = sample_points.at(rb.sample);
				auto db = relations.at(bi).distance;

				double remo_m = cv::norm(ma.pt - mb.pt);
				double remo_s = cv::norm(sa.pt - sb.pt);
				if (std::min(remo_m, remo_s) < MIN_DIST){
					continue;
				}
				remoteness_ratios.push_back({
					std::atan2(remo_m, remo_s),
					remo_m, // 撮影倍率への依存を避けるためにマスターを利用.
					da, db
				});
			}
		}
		std::sort(remoteness_ratios.begin(), remoteness_ratios.end());
		return remoteness_ratios;
	}

	/*!
	「二組の特徴点の関係」を集めたものを評価して二次審査のスコアを求める。.
	@param[in] ratios 「二組の特徴点の関係」を集めたもの.
	@note 中央値からの距離を t として、 exp(-t**2/σ**2) * (二点間のユークリッド距離)**0.5 の平均を利用。σ=3度。. 
	*/
	auto evaluate_remoteness_ratios(std::vector<remoteness_ratio> const & ratios)->double
	{
		if (ratios.size() < 3){
			// 「二組の特徴点の関係」のデータ量が少なすぎて評価不能。.
			return 0;
		}
		auto median = ratios[ratios.size() / 2].ratio;
		double sum = 0;
		const double sigma = 3.1416 / 180 * 3;
		for (auto const & r : ratios){
			double delta = r.ratio - median;
			sum += exp(-delta*delta / (sigma*sigma)) * sqrt(r.remo);
		}
		double ave = sum / ratios.size();
		return ave;
	}

	/*!
	２次審査のスコアを計算する.
	@param[in] master マスター画像の特徴量、特徴点.
	@param[in] sample サンプル画像の特徴量、特徴点.
	@return スコア.
	*/
	double calc_second_test_score(pierry::feature_points const & master, pierry::feature_points const & sample)
	{
		cv::flann::Index flann(master.m_features, cv::flann::HierarchicalClusteringIndexParams(), cvflann::FLANN_DIST_HAMMING);
		/// TODO:get_candidates に同じ計算があるのでまとめる.
		cv::Mat indices, dists;
		int knn = 8;//4位まで.
		flann.knnSearch(sample.m_features, indices, dists, knn, cv::flann::SearchParams(2000, 0, true));
		const size_t relation_size = 200;
		std::vector<relation> const relations = get_nearest_relations(indices, dists, relation_size);
		std::vector<remoteness_ratio> const remoteness_ratios = collect_remoteness_ratios(master.m_key_points, sample.m_key_points, relations);
#if defined CSV_STDOUT
		for (auto const & a : remoteness_ratios){
			std::cout << a.ratio << ",";
		}
		std::cout << std::endl;
#endif
		return evaluate_remoteness_ratios(remoteness_ratios);
	}

	/*!
	2次審査を実施する.
	@param[in] candidates 審査対象.
	@param[in] db 全マスタファイルのの特徴点、特徴量.
	@param[in] db_names 全マスターファイル名.
	@param[in] sample サンプルファイルの特徴点、特徴量.
	*/
	pierry::search_results second_test(
		pierry::search_results const & candidates,
		pierry::feature_points const & db,
		std::vector<pierry::name_index_t> const & db_names,
		pierry::feature_points const & sample,
		size_t candidate_count)
	{
		using pierry::search_result;
		pierry::search_results dst;
		std::transform(candidates.begin(), candidates.end(), std::back_inserter(dst),
			[&](search_result const & from)->search_result{
			pierry::file_identifier file = { from.subdb, from.filename };
#if defined CSV_STDOUT
			using boost::filesystem::path;
			std::cout << path(from.filename).leaf() << "," << from.score << ",,";
#endif
			auto const master = get_master_from_db(db, db_names, file);
			double second_score = calc_second_test_score(master, sample);
#if defined CSV_STDOUT
			std::cout << second_score << std::endl;
#endif
			using boost::filesystem::path;
#if defined SHOW_BOTH_SCORES
			std::cerr << boost::format("%s,%s,%s,%s\n") % path(from.filename).leaf() % from.subdb % from.score % second_score;
#endif
			return search_result{ from.filename, from.subdb, from.score*second_score };
		});

		/// TODO:first_test に同じコードがある。.
		std::sort(dst.begin(), dst.end(), [](search_result const & a, search_result const & b)->bool{
			return b.score < a.score;
		});
		return take_first(dst, candidate_count);
	}
}

pierry::search_results pierry::get_candidates(
    cv::flann::Index & flann,
    cv::Mat const & features,
    std::vector<cv::KeyPoint> const & key_points,
    std::vector<name_index_t> const & names,
    cv::Mat const & src,
    int candidate_count,
    db_info const & op)
{
#if defined CSV_STDOUT
	static std::mutex mutex;
	std::lock_guard<std::mutex> lock(mutex);
#endif

    if (key_points.empty()){
        return search_results();
    }
    feature_points const sample(src, op.orb, op.freak);
    assert(0 == sample.m_features.rows - sample.m_key_points.size());
    if (sample.m_features.empty()){
        return search_results();
    }
    cv::Mat indices, dists;
    int const knn = 4;	// knnSearchで何位まで求めるかを表す.
    flann.knnSearch(sample.m_features, indices, dists, knn, cv::flann::SearchParams(2000, 0, true));
    auto selected = first_test(indices, dists, names, candidate_count + 5);	// 2次検査で脱落するファイルがあるため、１次検査は少し多めに求める.
    feature_points const master(key_points, features);
    return second_test(selected, master, names, sample, candidate_count);
    
}

#ifdef TEST_MODE
#include "identify_test.hpp"
#endif // TEST_MODE

