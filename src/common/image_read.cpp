#include "stdafx.h"
#include "image_read.h"
#include "hi_pass.h"

namespace
{
	/*!
	縮尺画像を取得する.
	@param[in] m 画像イメージ.
	@param[in] zoom 縮尺率.
	@return 縮尺画像.
	*/
	cv::Mat zoom_image(cv::Mat const & m, double zoom)
	{
		if (zoom <= 0){
			throw std::runtime_error("zoom should be greater than 0");
		}
		if (!m.data){
			throw std::runtime_error("image is nil");
		}
		cv::Mat dst = m;
		if (zoom != 1.0){
			int const interpolate = zoom < 1.0 ? cv::INTER_AREA : cv::INTER_CUBIC;
			cv::resize(dst, dst, cv::Size(), zoom, zoom, interpolate);
		}
		return dst;
	}

	/*!
	長辺を調整する
	@param[in] m 元画像
	@param[in] width 調整後の長辺のピクセル数
	@return 長辺を調整した画像
	*/
	cv::Mat adjust_image(cv::Mat const & m, int width)
	{
		auto const s = (m.rows < m.cols) ? cv::Size(width, width * m.rows / m.cols) : cv::Size(width * m.cols / m.rows, width);
		cv::Mat dst;
		cv::resize(m, dst, s);
		return dst;
	}

	cv::Mat read_master_impl(boost::filesystem::path const & path, std::function<cv::Mat(cv::Mat const &)> conv)
	{
		auto m = cv::imread(path.string(), 0);
		if (!m.data){
			throw std::invalid_argument("failed to open image file.");
		}
		return conv(m);
	}

	cv::Rect get_trim_area(cv::Size const & s, double r)
	{
		assert(0 < r && r <= 1.0);
		return{
			static_cast<int>(s.width * (1 - r) / 2),
			static_cast<int>(s.height * (1 - r) / 2),
			static_cast<int>(s.width * r),
			static_cast<int>(s.height * r)
		};
	}

	cv::Rect get_trim_area(cv::Size const & s, cv::Size const & d)
	{
		assert(d.width <= s.width && d.height <= s.height);
		auto x = (s.width - d.width) / 2;
		auto y = (s.height - d.height) / 2;
		return{
			x, y,
			d.width, d.height };
	}

	cv::Mat read_sample_impl(boost::filesystem::path const & path, std::function<cv::Mat(cv::Mat const &)> conv)
	{
		auto img = cv::imread(path.string(), 1);
		if (!img.data){
			throw std::invalid_argument((boost::format("Failed to open %s in function read_sample.") % path.filename().string()).str().c_str());
		}
		// 以前のバージョンとサイズを完璧に合わせるために、無駄な計算をしている。.
		// TODO: サイズの調整.
		auto destSize = get_trim_area(img.size(), 0.8).size();
		img = pierry::hipass_filtered_image(img);
		auto trimRect = get_trim_area(img.size(), destSize);
		return conv(img(trimRect));
	}

}

cv::Mat pierry::read_master_with_zoom(boost::filesystem::path const & path, double zoom)
{
	return read_master_impl(path, [&](cv::Mat const & m){
		return zoom_image(m, zoom);
	});
}

cv::Mat pierry::read_master_with_width(boost::filesystem::path const & path, int width)
{
	return read_master_impl(path, [&](cv::Mat const & m){
		return adjust_image(m, width);
	});
}

cv::Mat pierry::read_sample_with_zoom(boost::filesystem::path const & path, double zoom)
{
	return read_sample_impl(path, [&](cv::Mat const & m){
		return zoom_image(m, zoom);
	});
}

cv::Mat pierry::read_sample_with_width(boost::filesystem::path const & path, int width)
{
	return read_sample_impl(path, [&](cv::Mat const & m){
		return adjust_image(m, width);
	});
}
