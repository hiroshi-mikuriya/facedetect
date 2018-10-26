#include "stdafx.h"
#include "update.h"
#include "utils.h"
#include "define.h"
#include "db_locker.h"
#include "digest.h"

namespace
{
	/*!
	サブDB内の不要なファイルを全て削除する
	@param[in] sub サブDBのディレクトリ
	@param[in] masters 全マスターファイルのパス
	*/
	void remove_disused_files(
		boost::filesystem::path const & sub,
		std::vector<boost::filesystem::path> const & masters)
	{
		boost::filesystem::directory_iterator it(sub);
		for (; it != boost::filesystem::directory_iterator(); ++it){
			auto const leaf = it->path().leaf().string();
			if (boost::filesystem::is_directory(it->path())) {
				continue;
			}
			if (leaf == pierry::subdb_info_file()) {
				continue;
			}
			auto const ext = it->path().extension();
			if (ext == pierry::features_ext() || ext == pierry::keypoint_ext()) {
				auto const name = it->path().stem();
				auto found = std::find_if(masters.begin(), masters.end(), [name](boost::filesystem::path const & path) {
					return name == path.filename().string();
				});
				if (masters.end() != found) {
					continue;
				}
			}
			if (!boost::filesystem::remove(it->path())){
				throw std::runtime_error((boost::format("failed to remove %s.") % it->path().leaf().string()).str());
			}
		}
	}

	/*!
	サブDB内に不要なファイルがあれば削除、必要なファイルがなければ作成する。
	マスターディレクトリが存在しなければサブDBを削除する。
	@param[in] sub_dbs サブDB
	@param[in] op 親DBオプション
	@param[out] o 進捗通知する出力ストリーム
	*/
	void cleanup_sub_db(boost::filesystem::path const & sub, pierry::db_info const & op, std::ostream & o)
	{
		assert(boost::filesystem::is_directory(sub));
		o << boost::format("updating sub db [%s]") % sub.leaf().string() << std::endl;
		auto const sop = pierry::read_subdb_info(sub);
		if (!boost::filesystem::is_directory(sop.master_directory)){
			if (!boost::filesystem::remove_all(sub)){
				throw std::invalid_argument((boost::format("failed to remove %s.") % sub.string()).str());
			}
			return;
		}
		auto const masters = pierry::get_image_files(sop.master_directory);
		o << boost::format("master file's count : %d") % masters.size() << std::endl;
		o << "removing disused files." << std::endl;
		for (size_t zix = 0; zix < op.default_scales.size(); ++zix){
			auto const save_dir = pierry::get_leaf_directory(sub, zix);
			remove_disused_files(save_dir, masters);
		}
		o << "calculating features." << std::endl;
		for (size_t mix = 0; mix < masters.size(); ++mix) {
			o << boost::format("%4.1f%%") % (100.0 * (mix + 1) / masters.size());
            // run_threadsをするとxcodeのrelease版では動作しなくなったので、forループにした。
            for(size_t ix = 0; ix < sop.scales.size(); ++ix){
                auto const save_dir = pierry::get_leaf_directory(sub, ix);
                if (!boost::filesystem::exists(save_dir) && !boost::filesystem::create_directories(save_dir)){	// TODO: 他にも同じようなチェックしている
                    throw std::runtime_error("failed to create internal directory.");
                }
                pierry::create_feature_keypoint_file(masters[mix], save_dir, op, sop.scales[ix]);
            }
			o << "\b\b\b\b\b\b\b\b\b";
		}
		o << std::endl;
	}

	/*!
	flannファイルを作成する。
	@param[in] db データベースのパス
	@param[in] o ログを出力するストリーム
	*/
	void create_flann(boost::filesystem::path const & db, std::ostream & o)
	{
		using namespace pierry;
		db_info const op = read_db_info(db);
		o << "loading database." << std::endl;
		for (size_t zix = 0; zix < op.default_scales.size(); ++zix){
			cv::Mat features;
			std::vector<cv::KeyPoint> key_points;
			std::vector<name_index_t> names;
			for (auto const & sub : get_sub_db_directories(db)) {
				auto const sop = read_subdb_info(sub);
				auto const subdb_name = sub.leaf().string();
				read_features_and_key_points(get_leaf_directory(sub, zix), sop.master_directory, subdb_name, features, key_points, names);
			}
			o << "creating flann::index." << std::endl;
			cv::flann::Index flann(features, cv::flann::HierarchicalClusteringIndexParams(), cvflann::FLANN_DIST_HAMMING);
			o << "save flann::index." << std::endl;
			flann.save((db / flann_name(zix)).string());
		}
	}
}

int pierry::update_impl(boost::filesystem::path const & db, std::ostream & o)
{
	auto const op = read_db_info(db);
	auto const sub_dbs = get_sub_db_directories(db);
	if (sub_dbs.empty()){
		o << "now, db is empty." << std::endl;
		for (size_t zix = 0; zix < op.default_scales.size(); ++zix){
			boost::filesystem::remove(db / flann_name(zix));
		}
		return 0;
	}
	for (auto const & sub : sub_dbs) {
		cleanup_sub_db(sub, op, o);	// cleanupによってサブDB数が変更になっている可能性があるため、以降sub_dbsを使用してはいけない.
	}
	return digest_process(db, [&](){
		create_flann(db, o);
		return 0;
	});
}

int pierry::lock_update_impl(boost::filesystem::path const & db, std::ostream & o)
{
	db_locker locker(db);
	return update_impl(db, o);
}

#ifdef TEST_MODE
#include "update_test.hpp"
#endif // TEST_MODE

