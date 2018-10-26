#include "stdafx.h"
#include "create.h"
#include "db_locker.h"
#include "utils.h"
#include "version.h"

int pierry::create_parent_db_impl(
	boost::filesystem::path const & db,
	std::vector<zoom_or_pix> const & scales,
	orb_params const & orb,
	freak_params const & freak)
{
	if (exist_parent_db(db)){
		throw std::runtime_error((boost::format("db %s is exists already.") % db).str());
	}
	if (!boost::filesystem::is_directory(db) && !boost::filesystem::create_directories(db)){
		throw std::runtime_error((boost::format("failed to create %s") % db).str());
	}
	version_info ver;
	ver.major = APPLICATION_MAJOR_VERSION;
	ver.minor = APPLICATION_MINOR_VERSION;
	ver.revision = APPLICATION_REVISION;
	db_info op = { orb, freak, ver };
	op.default_scales = [scales](){
		std::vector<double> dst;
		for (auto const & s : scales){
			if (!s.is_zoom()){
				throw std::runtime_error("not implemented in function pierry::create_parent_db_impl."); // TODO: Ç¢Ç‹ÇÕèké⁄ó¶Ç…ÇµÇ©ëŒâûÇµÇ»Ç¢ÅBÇ†Ç∆Ç≈ÉâÉÄÉ_Ç≤Ç∆è¡Ç∑.
			}
			dst.push_back(s.zoom());
		}
		return dst;
	}();
	write_db_info(db, op);
	create_db_lock(db);
	assert(exist_parent_db(db));
	return 0;
}

bool pierry::exist_parent_db(boost::filesystem::path const & db)
{
	if (!boost::filesystem::is_directory(db)){
		return false;
	}
	try{
		read_db_info(db);
		return exist_db_lock(db);
	}
	catch (std::exception const & e){
		std::cerr << e.what() << std::endl;
		return false;
	}
}
