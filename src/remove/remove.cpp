#include "stdafx.h"
#include "remove.h"
#include "utils.h"
#include "update.h"
#include "db_locker.h"

int pierry::remove_impl(boost::filesystem::path const & db, std::string const & name, bool with_update, std::ostream & o)
{
	auto const subs = get_sub_db_directories(db);
	db_locker locker(db);
	for (auto const & sub : subs){
		if (sub.leaf().string() != name){
			continue;
		}
		o << boost::format("removing %s") % name << std::endl;
		if (boost::filesystem::remove_all(sub)){
			o << boost::format("finished to remove %s.") % name << std::endl;
			return with_update ? update_impl(db, o) : 0;
		}
		else{
			throw std::runtime_error("failed to remove sub db.");
		}
	}
	throw std::runtime_error((boost::format("%s is not exist in database.") % name).str());
}
