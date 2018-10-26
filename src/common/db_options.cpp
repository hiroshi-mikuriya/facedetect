#include "stdafx.h"
#include "db_options.h"
#include "define.h"

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

const int PIERRY_SERIALIZE_VERSION = 1; // シリアライズの形式を変更するたびに加算する。
BOOST_CLASS_VERSION(pierry::db_info, PIERRY_SERIALIZE_VERSION)
BOOST_CLASS_VERSION(pierry::subdb_info, PIERRY_SERIALIZE_VERSION)
BOOST_CLASS_VERSION(pierry::orb_params, PIERRY_SERIALIZE_VERSION)
BOOST_CLASS_VERSION(pierry::freak_params, PIERRY_SERIALIZE_VERSION)
BOOST_CLASS_VERSION(pierry::version_info, PIERRY_SERIALIZE_VERSION)
BOOST_CLASS_VERSION(pierry::zoom_or_pix, PIERRY_SERIALIZE_VERSION)

namespace boost{
	namespace serialization{
		/*!
		バージョン情報をシリアライズする
		*/
		template<class Archive>
		void serialize(Archive& ar, pierry::version_info & obj, const unsigned int version)
		{
			static_cast<void>(version); // unused
			using namespace boost::serialization;
			ar & make_nvp("major", obj.major);
			ar & make_nvp("minor", obj.minor);
			ar & make_nvp("revision", obj.revision);
		}

		/*!
		cv::OrbFeatureDetector のパラメータをシリアライズする
		*/
		template<class Archive>
		void serialize(Archive& ar, pierry::orb_params & obj, const unsigned int version)
		{
			static_cast<void>(version); // unused
			using namespace boost::serialization;
			ar & make_nvp("nfeatures", obj.nfeatures);
			ar & make_nvp("scaleFactor", obj.scaleFactor);
			ar & make_nvp("nlevels", obj.nlevels);
			ar & make_nvp("edgeThreshold", obj.edgeThreshold);
			ar & make_nvp("firstLevel", obj.firstLevel);
			ar & make_nvp("WTA_K", obj.WTA_K);
			ar & make_nvp("scoreType", obj.scoreType);
			ar & make_nvp("patchSize", obj.patchSize);
		}

		template<class Archive>
		void serialize(Archive& ar, pierry::freak_params & obj, const unsigned int version)
		{
			static_cast<void>(version); // unused
			using namespace boost::serialization;
			ar & make_nvp("orientationNormalized", obj.orientationNormalized);
			ar & make_nvp("scaleNormalized", obj.scaleNormalized);
			ar & make_nvp("patternScale", obj.patternScale);
			ar & make_nvp("nOctaves", obj.nOctaves);
			ar & make_nvp("selectedPairs", obj.selectedPairs);
		}

		template<class Archive>
		void serialize(Archive& ar, pierry::db_info & obj, const unsigned int version)
		{
			static_cast<void>(version); // unused
			using namespace boost::serialization;
			ar & make_nvp("orb", obj.orb);
			ar & make_nvp("freak", obj.freak);
			ar & make_nvp("version", obj.lib_version);
			ar & make_nvp("default_scales", obj.default_scales);
		}

		/*!
		サブDBのオプションをシリアライズする
		*/
		template<class Archive>
		void serialize(Archive& ar, pierry::subdb_info & obj, const unsigned int version)
		{
			static_cast<void>(version); // unused
			using namespace boost::serialization;
			ar & make_nvp("scales", obj.scales);
			ar & make_nvp("master_directory", obj.master_directory);
			ar & make_nvp("file_count", obj.file_count);
		}

		/*!
		pierry::zoom_or_pixをシリアライズする
		*/
		template<class Archive>
		void serialize(Archive& ar, pierry::zoom_or_pix & obj, const unsigned int version)
		{
			static_cast<void>(version); // unused
			using namespace boost::serialization;
			int type = !obj.is_initialized() ? 0 : obj.is_zoom() ? 1 : 2;
			ar & make_nvp("type", type);
			switch (type){
			default:
				break;
			case 1:	// zoom
			{
				double zoom = obj.zoom();
				ar & make_nvp("zoom", zoom);
				obj = zoom;
			}
				break;
			case 2:	// pix
			{
				int pix = obj.pix();
				ar & make_nvp("pix", pix);
				obj = pix;
			}
				break;
			}
		}
	}
}
namespace{
	template<typename OptionT>
	OptionT read(boost::filesystem::path const & path, std::string const & tag)
	{
		OptionT op;
		std::ifstream ifs(path.string().c_str());
		if (!ifs){
			throw std::runtime_error((boost::format("failed to read db option file <%s>.") % path.string()).str());
		}
		boost::archive::text_iarchive(ifs) >> boost::serialization::make_nvp(tag.c_str(), op);
		return op;
	}

	template<typename OptionT>
	void write(boost::filesystem::path const & path, std::string const & tag, OptionT const & op)
	{
		std::ofstream ofs(path.string().c_str());
		boost::archive::text_oarchive(ofs) << boost::serialization::make_nvp(tag.c_str(), op);
	}
}


pierry::db_info pierry::read_db_info(boost::filesystem::path const & db)
{
	return read<db_info>(db / pierry::db_info_file(), pierry::parent_db_tag());
}

void pierry::write_db_info(boost::filesystem::path const & db, db_info const & src)
{
	write<db_info>(db / pierry::db_info_file(), pierry::parent_db_tag(), src);
}

pierry::subdb_info pierry::read_subdb_info(boost::filesystem::path const & db, std::string const & name)
{
	return read_subdb_info(db / name);
}

void pierry::write_subdb_info(boost::filesystem::path const & db, std::string const & name, subdb_info const & src)
{
	write_subdb_info(db / name, src);
}

pierry::subdb_info pierry::read_subdb_info(boost::filesystem::path const & subdb)
{
	auto serialize = read<subdb_info>(subdb / pierry::subdb_info_file(), pierry::parent_db_tag());
	return serialize;
}

void pierry::write_subdb_info(boost::filesystem::path const & subdb, subdb_info const & src)
{
	write<subdb_info>(subdb / pierry::subdb_info_file(), pierry::parent_db_tag(), src);
}
