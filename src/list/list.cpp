#include "stdafx.h"
#include "list.h"
#include "utils.h"

pierry::db_infomations pierry::read_all_db_infomations(boost::filesystem::path const & db)
{
	auto const sub_dbs = get_sub_db_directories(db);
	db_infomations dst;
	dst.op = read_db_info(db);
	for (auto const & sub : sub_dbs){
		auto const name = sub.leaf().string();
		auto const sop = read_subdb_info(sub);
		dst.sub[name] = sop;
	}
	return dst;
}
