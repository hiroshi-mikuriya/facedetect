#include "stdafx.h"
#include "db_locker.h"
#include "define.h"
#include <boost/interprocess/sync/file_lock.hpp>

namespace
{
	std::string get_db_file(boost::filesystem::path const & db)
	{
		return (db / pierry::db_lock_name()).string();
	}
}

pierry::db_locker::db_locker(boost::filesystem::path const & db)
	: m(get_db_file(db).c_str())
{
	if (!m.try_lock()){
		throw std::runtime_error("failed to lock database.");
	}
}

pierry::db_locker::~db_locker()
{
	// m.close()
}

void pierry::create_db_lock(boost::filesystem::path const & db)
{
	std::ofstream f(get_db_file(db).c_str());
	if (!f){
		throw std::runtime_error("failed to create db lock file.");
	}
}

bool pierry::exist_db_lock(boost::filesystem::path const & db)
{
	return !boost::filesystem::is_directory(get_db_file(db)) && boost::filesystem::exists(get_db_file(db));
}
