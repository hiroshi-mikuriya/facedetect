#include "stdafx.h"
#include "add.h"
#include "update.h"
#include "create.h"
#include "utils.h"
#include "db_options.h"
#include "define.h"
#include "image_read.h"
#include "db_read_write.h"
#include "features.h"
#include "db_locker.h"

namespace
{
	/*!
	サブDBのディレクトリを作成する。
	既に存在する場合にもエラー終了はしない
	@param[in] db 親DBのパス
	@param[in] subdb_name サブDB名
	@param[in] sop サブDBオプション
	*/
	void create_sub_db_directories(
		boost::filesystem::path const & db,
		std::string const & subdb_name,
		pierry::subdb_info const & sop)
	{
		auto create_dir = [](boost::filesystem::path const & dir){
			return boost::filesystem::is_directory(dir) || boost::filesystem::create_directories(dir);
		};
		auto const sub_db = (db / subdb_name).normalize();
		if (!create_dir(sub_db)){
			throw std::runtime_error("failed to create sub db directory.");
		}
		for (size_t ix = 0; ix < sop.scales.size(); ++ix){
			auto zoom_dir = pierry::get_leaf_directory(sub_db, ix);
			if (!create_dir(zoom_dir)){
				throw std::runtime_error("failed to create internal directory.");
			}
		}
	}
}

int pierry::add_impl(
	boost::filesystem::path const & master,
	boost::filesystem::path const & db,
	std::string const & name,
	std::vector<zoom_or_pix> const & scales,
	bool with_update,
	std::ostream & o)
{
	subdb_info sop;
	sop.master_directory = master.string();
	sop.scales = [scales](){
		std::vector<double> dst;
		for (auto const & s : scales){
			if (!s.is_zoom()){
				throw std::runtime_error("not implemented in function pierry::add_impl."); // TODO: いまは縮尺率にしか対応しない。あとでラムダごと消す.
			}
			dst.push_back(s.zoom());
		}
		return dst;
	}();
	o << boost::format("master file's directory : %s\ndb directory : %s") % master.string() % db.string() << std::endl;
	if (!boost::filesystem::is_directory(master)){
		throw std::runtime_error("invalid master directory.");
	}
	if (!exist_parent_db(db)){
		throw std::runtime_error("not found db.");
	}
	auto const op = read_db_info(db);
	if (sop.scales.empty()){
		assert(!op.default_scales.empty());
		sop.scales = op.default_scales;
	}
	if (op.default_scales.size() != sop.scales.size()){
		throw std::runtime_error((boost::format("zooms count should be %d in this database.") % op.default_scales.size()).str());
	}
	o << "collecting image files." << std::endl;
	auto masters = get_image_files(master);
	sop.file_count = static_cast<int>(masters.size());
	o << boost::format("master directory : %s\nfile count : %5d\nname : %s") % master.string() % sop.file_count % name << std::endl;
	db_locker locker(db);
	auto const sub_db = (db / name).normalize();
	create_sub_db_directories(db, name, sop);
	write_subdb_info(sub_db, sop);
	o << boost::format("finished to add sub database[%s]") % name << std::endl;
	if (!with_update){
		return 0;
	}
	return pierry::update_impl(db, o);
}
