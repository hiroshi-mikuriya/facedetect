#include "stdafx.h"
#include "pierry.h"
#include "utils.h"
#include "add.h"
#include "create.h"
#include "identify.h"
#include "list.h"
#include "remove.h"
#include "update.h"
#include "version.h"
#include "db_options.h"
#include "features.h"
#include "hi_pass.h"
#include "image_read.h"

pierry::orb_params::orb_params()
	: nfeatures(500), scaleFactor(1.2f), nlevels(8), edgeThreshold(31), firstLevel(0), WTA_K(2), scoreType(cv::ORB::HARRIS_SCORE), patchSize(31)
{}

pierry::freak_params::freak_params()
	: orientationNormalized(true), scaleNormalized(true), patternScale(22.0f), nOctaves(4)
{}

pierry::version_info pierry::get_lib_version()
{
	return{ APPLICATION_MAJOR_VERSION, APPLICATION_MINOR_VERSION, APPLICATION_REVISION };
}

std::string pierry::get_lib_description()
{
	return APPLICATION_DESCRIPTION ", version " APPLICATION_VERSION "\n" APPLICATION_COPYRIGHT;
}

pierry::db_operator::db_operator(std::string const & db)
	: m_db(db)
{}

void pierry::db_operator::create(std::vector<zoom_or_pix> const & default_scales, pierry::orb_params const & orb, pierry::freak_params const & freak)const
{
	create_parent_db_impl(m_db, default_scales, orb, freak);
}

bool pierry::db_operator::created()const
{
	return exist_parent_db(m_db);
}

pierry::db_info pierry::db_operator::read_options()const
{
	return read_db_info(m_db);
}

void pierry::db_operator::add(
	std::string const & master_directory,
	std::vector<zoom_or_pix> const & scales,
	std::string const & name,
	bool with_update,
	std::ostream & o)const
{
	add_impl(master_directory, m_db, name, scales, with_update, o);
}

void pierry::db_operator::add(
	std::string const & master_directory,
	std::vector<zoom_or_pix> const & scales,
	bool with_update,
	std::ostream & o)const
{
	auto const name = normalized(master_directory).leaf().string();
	add_impl(master_directory, m_db, name, scales, with_update, o);
}

pierry::db_info pierry::db_operator::get_db_info()const
{
	if (!exist_parent_db(m_db)){
		throw std::runtime_error((boost::format("not found %s.") % m_db).str());
	}
	return read_db_info(m_db);
}

std::map<std::string, pierry::subdb_info> pierry::db_operator::get_subdb_infos()const
{
	std::map<std::string, pierry::subdb_info> dst;
	for (auto const & sub : read_all_db_infomations(m_db).sub){
		dst[sub.first] = { sub.second.master_directory, sub.second.scales, sub.second.file_count };
	}
	return dst;
}

void pierry::db_operator::remove(std::string const  & name, bool with_update, std::ostream & o)const
{
	remove_impl(m_db, name, with_update, o);
}

void pierry::db_operator::update(std::ostream & o)const
{
	lock_update_impl(m_db, o);
}

struct pierry::db_identifier::impl
{
	cv::flann::Index flann;
	cv::Mat features;
	std::vector<cv::KeyPoint> key_points;
	std::vector<name_index_t> names;
};

pierry::db_identifier::db_identifier(std::string const & db)
	: m_db(db)
	, m_operator(db)
{
}

void pierry::db_identifier::load(bool with_update, std::ostream & o)
{
	if (with_update){
		m_operator.update(o);
	}
	db_info op = read_db_info(m_db);
	for (size_t zix = 0; zix < op.default_scales.size(); ++zix){
		m.push_back(members_t::value_type(new impl));
		auto & back = m.back();
		load_db(m_db, zix, back->flann, back->features, back->key_points, back->names, o);
	}
}

bool pierry::db_identifier::loaded()const
{
	for (auto const & e : m){
		if (!(e->features.rows && e->key_points.size() && e->names.size())){
			return false;
		}
	}
	return true;
}

namespace
{
	pierry::search_results merge_results(std::vector<pierry::search_results> const & src)
	{
		auto copy = src;
		auto pred = [](pierry::search_result const & lhs, pierry::search_result const & rhs){
			return std::make_pair(lhs.subdb, lhs.filename) < std::make_pair(rhs.subdb, rhs.filename);
		};
		for (auto & c : copy){
			std::sort(c.begin(), c.end(), pred);	// set_intersectionのため
		}
		pierry::search_results dst {copy.front()};
		for (size_t ix = 1; ix < copy.size(); ++ix){
			decltype(dst) tmp;
			std::set_intersection(dst.begin(), dst.end(), copy[ix].begin(), copy[ix].end(), std::back_inserter(tmp), pred);
			dst = tmp;
		}
		for (auto & d : dst){
			d.score = 1;	// この時点でゴミが入っているため
			for (auto const & c : copy){
				auto it = std::lower_bound(c.begin(), c.end(), d, pred);
				assert(it != c.end());
				d.score *= it->score;
			}
		}
		std::sort(dst.begin(), dst.end(), [](pierry::search_result const & lhs, pierry::search_result const & rhs){
			return rhs.score < lhs.score;
		});
		return dst;
	}
}

namespace {
    cv::Mat create_hi_pass_image(std::string const & path)
    {
        double const r = 0.8;
		auto const m = pierry::hipass_filtered_image(cv::imread(path, 0));
		cv::Rect const rc(
			static_cast<int>(m.cols * (1 - r) / 2), 
			static_cast<int>(m.rows * (1 - r) / 2), 
			static_cast<int>(m.cols * r), 
			static_cast<int>(m.rows * r)
			);
        return m(rc);
    }
    cv::Mat zoomed_image(cv::Mat const & src, pierry::zoom_or_pix const & scale)
    {
        cv::Mat dst;
        assert(scale.is_initialized());
        if(scale.is_zoom()){
            cv::resize(src, dst, cv::Size(), scale.zoom(), scale.zoom());
        }else{
            int const pix = scale.pix();
            auto const s = src.rows < src.cols
				? cv::Size(pix, pix * src.rows / src.cols)
				: cv::Size(pix * src.cols / src.rows, pix);
            cv::resize(src, dst, s);
        }
        return dst;
    }
	cv::Mat 
		read_sample_with_zoom_or_pix(boost::filesystem::path const & path, pierry::zoom_or_pix const & zp)
	{
		if (zp.is_zoom()){
			return pierry::read_sample_with_zoom(path, zp.zoom());
		}
		else if (zp.is_pix()){
			return pierry::read_sample_with_width(path, zp.pix());
		}
		else {
			throw std::logic_error("image size is not specified.");
		}
	}
}

pierry::search_results pierry::db_identifier::search(search_params const & src)
{
	auto const op = read_db_info(m_db);
	if (op.default_scales.size() != src.scales.size()){
		throw std::invalid_argument((boost::format("zooms count should be %d.") % op.default_scales.size()).str());
	}
	std::vector<search_results> srv(m.size());
	int const count = std::max(src.candidate_count + 5, src.candidate_count * 2);
    for(size_t ix = 0; ix < m.size(); ++ix){
        auto const zim = read_sample_with_zoom_or_pix(src.filename, src.scales[ix]);
        auto const & e = this->m[ix];
        srv[ix] = get_candidates(e->flann, e->features, e->key_points, e->names, zim, count, op);
    }
	auto const results = merge_results(srv);
	return{ results.begin(), results.begin() + std::min<int>(static_cast<int>(results.size()), src.candidate_count) };
}

void pierry::db_identifier::clear()
{
	m.clear();
}

