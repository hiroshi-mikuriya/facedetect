#include "stdafx.h"
#include "utils.h"
#include "image_read.h"
#include "db_read_write.h"
#include "features.h"
#include "define.h"

std::vector<boost::filesystem::path> pierry::get_image_files(boost::filesystem::path const & dir)
{
	if (!boost::filesystem::is_directory(dir)){
		std::runtime_error("invalid directory.");
	}
	auto is_image_file = [](boost::filesystem::path const & path){
		auto ext = path.extension().string();
		for (auto const & e : { ".png", ".jpg", ".jpeg", ".bmp", ".tiff", ".tif" }) {
			if (ext == e){
				return true;
			}
		}
		return false;
	};
	std::vector<boost::filesystem::path> dst;
	auto it = boost::filesystem::directory_iterator(dir);
	for (; it != boost::filesystem::directory_iterator(); ++it){
		if (is_image_file(it->path())){
			dst.push_back(it->path());
		}
	}
	return dst;
}

std::vector < boost::filesystem::path> pierry::get_sub_db_directories(boost::filesystem::path const & db)
{
	if (!boost::filesystem::is_directory(db)){
		throw std::runtime_error(db.string() + " is not a directory.");
	}
	std::vector < boost::filesystem::path> dst;
	boost::filesystem::directory_iterator it(db);
	for (; it != boost::filesystem::directory_iterator(); ++it){
		if (!boost::filesystem::is_directory(it->path())){
			continue;
		}
		auto const db_option_file_path = it->path() / pierry::subdb_info_file();
		if (boost::filesystem::exists(db_option_file_path)){
			dst.push_back(it->path());
		}
	}
	return dst;
}


int pierry::create_feature_keypoint_file(
	boost::filesystem::path const & master,
	boost::filesystem::path const & save_dir,
	pierry::db_info const & op,
	zoom_or_pix const & scale)
{
	using namespace pierry;
	auto const feature_name = save_dir / (master.filename().string() + features_ext());
	auto const kp_name = save_dir / (master.filename().string() + keypoint_ext());
	auto const master_last_write_time = boost::filesystem::last_write_time(master);
	if (boost::filesystem::exists(feature_name) &&
		boost::filesystem::exists(kp_name) &&
		master_last_write_time < boost::filesystem::last_write_time(feature_name) &&
		master_last_write_time < boost::filesystem::last_write_time(kp_name))
	{
		return 1;
	}
	auto const m = scale.is_pix() ? read_master_with_width(master.string(), scale.pix()) : read_master_with_zoom(master.string(), scale.zoom());
	feature_points const kf(m, op.orb, op.freak);	// TODO: 複数のズームを受け取るようにする
	std::string const tmp_ext(".tmp");
	auto const feature_name_tmp = feature_name.string() + tmp_ext;
	auto const kp_name_tmp = kp_name.string() + tmp_ext;
	write_features(kf.m_features, feature_name_tmp);
	boost::filesystem::rename(feature_name_tmp, feature_name);	// プロセスが停止した場合に中途半端な状態のファイルができることを防止するため
	write_key_points(kf.m_key_points, kp_name_tmp);
	boost::filesystem::rename(kp_name_tmp, kp_name);
	return 0;
}

void pierry::read_features_and_key_points(
	boost::filesystem::path const & saved_dir,
	boost::filesystem::path const & master_dir,
	std::string const & subdb_name,
	cv::Mat & features,
	std::vector<cv::KeyPoint> & key_point,
	std::vector<name_index_t> & names)
{
	boost::filesystem::directory_iterator it(saved_dir);
	if (features.empty()){
		features = cv::Mat(0, 64, CV_8U);
	}
	for (; it != boost::filesystem::directory_iterator(); ++it) {
		auto const ext = it->path().extension().string();
		if (ext != features_ext()){
			assert(ext == keypoint_ext() || ext == option_file_ext());
			continue;
		}
		auto const feature_file = it->path();
		auto const image_file_name = feature_file.stem().string();
		auto const key_point_file = feature_file.parent_path() / (image_file_name + keypoint_ext());
		if (!boost::filesystem::exists(key_point_file)){
			throw std::runtime_error("not found key point file.");
		}
		auto const ff = read_features(feature_file);
		auto const kk = read_key_points(key_point_file);
		if (0 != kk.size() - ff.rows) {
			throw std::runtime_error("key point size should be features size.");
		}
		features.push_back(ff);
		key_point.insert(key_point.end(), kk.begin(), kk.end());
		int const begin = names.empty() ? 0 : names.back().end;
		int const end = begin + ff.rows;
		names.push_back({ subdb_name, (master_dir / image_file_name).string(), begin, end });
	}
}

auto pierry::normalized(boost::filesystem::path const & path)->boost::filesystem::path
{
	return boost::filesystem::complete(path / "dummy").normalize().parent_path();
}

boost::filesystem::path pierry::get_leaf_directory(boost::filesystem::path const & subdb, size_t zoom_ix)
{
	assert(boost::filesystem::is_directory(subdb));
	return subdb / (boost::format("zoom_%d") % zoom_ix).str();
}

void pierry::run_threads(size_t count, std::function<std::function<void(void)>(size_t ix)> const & proc)
{
	std::vector<std::thread> threads;
	std::vector<std::exception_ptr> eps(count);
	for (size_t ix = 0; ix < count; ++ix){
		threads.emplace_back([&proc, ix](std::exception_ptr& ep){
			try{
				proc(ix)();
			}
			catch (...){
				ep = std::current_exception();
			}
		}, std::ref(eps.at(ix)));
	}
	for (auto & th : threads){
		th.join();
	}
	for (auto & ep : eps){
		if (ep != nullptr){
			// 最初の一個しか throw しない。
			std::rethrow_exception(ep);
		}
	}
}

#ifdef TEST_MODE
#include "utils_test.hpp"
#endif // TEST_MODE
