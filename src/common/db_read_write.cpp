#include "stdafx.h"
#include "db_read_write.h"

namespace
{
	template<typename T>
	cv::Mat read_mat_data(std::istream & is, int depth)
	{
		int cols, rows;
		is.read(reinterpret_cast<char*>(&rows), sizeof(rows));
		is.read(reinterpret_cast<char*>(&cols), sizeof(cols));
		// TODO: ファイルサイズが不正なら例外を投げたほうがよい
		cv::Mat dst = cv::Mat::zeros(rows, cols, CV_MAKETYPE(depth, 1));
		assert(!is.eof());
		is.read(reinterpret_cast<char*>(dst.ptr()), dst.size().area() * sizeof(T));
		return dst;
	}

	template<typename T>
	void write_mat_data(std::ostream & of, cv::Mat const & src)
	{
		of.write(reinterpret_cast<char const*>(src.ptr()), src.size().area() * sizeof(T));
	}

}

std::vector<cv::KeyPoint> pierry::read_key_points(boost::filesystem::path const & path)
{
	std::ifstream is(path.string(), std::ios::in | std::ios::binary);
	if (!is){
		throw std::runtime_error("failed to read key point's file.");
	}
	size_t size;
	is.read(reinterpret_cast<char*>(&size), sizeof(size));
	// TODO: ファイルサイズが不正なら例外を投げたほうがよい
	std::vector<cv::KeyPoint> dst;
	dst.reserve(size);
	for (size_t ix = 0; ix < size; ++ix) {
		cv::KeyPoint k;
		is.read(reinterpret_cast<char*>(&k.angle), sizeof(k.angle));
		is.read(reinterpret_cast<char*>(&k.class_id), sizeof(k.class_id));
		is.read(reinterpret_cast<char*>(&k.octave), sizeof(k.octave));
		is.read(reinterpret_cast<char*>(&k.pt), sizeof(k.pt));
		is.read(reinterpret_cast<char*>(&k.response), sizeof(k.response));
		is.read(reinterpret_cast<char*>(&k.size), sizeof(k.size));
		dst.push_back(k);
	}
	return dst;
}

cv::Mat pierry::read_features(boost::filesystem::path const & path)
{
	std::ifstream is(path.string(), std::ios::in | std::ios::binary);
	if (!is){
		throw std::runtime_error("failed to read feature's file.");
	}
	int flags;
	is.read(reinterpret_cast<char*>(&flags), sizeof(flags));
	int const depth = CV_MAT_DEPTH(flags);
	switch (depth){
	default:
		throw std::invalid_argument("Invalid depth in function read_features.");
	case CV_8U:
		return read_mat_data<std::uint8_t>(is, depth);
	case CV_8S:
		return read_mat_data<std::int8_t>(is, depth);
	case CV_16U:
		return read_mat_data<std::uint16_t>(is, depth);
	case CV_16S:
		return read_mat_data<std::int16_t>(is, depth);
	case CV_32S:
		return read_mat_data<std::int32_t>(is, depth);
	case CV_32F:
		return read_mat_data<float>(is, depth);
	case CV_64F:
		return read_mat_data<double>(is, depth);
	}
}

void pierry::write_key_points(std::vector<cv::KeyPoint> const & src, boost::filesystem::path const & path)
{
	std::ofstream of(path.string(), std::ios::out | std::ios::binary);
	if (!of){
		throw std::runtime_error("failed to create key point file.");
	}
	size_t const size = src.size();
	of.write(reinterpret_cast<char const*>(&size), sizeof(size));
	for (auto const & k : src){
		of.write(reinterpret_cast<char const*>(&k.angle), sizeof(k.angle));
		of.write(reinterpret_cast<char const*>(&k.class_id), sizeof(k.class_id));
		of.write(reinterpret_cast<char const*>(&k.octave), sizeof(k.octave));
		of.write(reinterpret_cast<char const*>(&k.pt), sizeof(k.pt));
		of.write(reinterpret_cast<char const*>(&k.response), sizeof(k.response));
		of.write(reinterpret_cast<char const*>(&k.size), sizeof(k.size));
	}
}

void pierry::write_features(cv::Mat const & src, boost::filesystem::path const & path)
{
	std::ofstream of(path.string(), std::ios::out | std::ios::binary);
	if (!of){
		throw std::logic_error("failed to create feature's file.");
	}
	of.write(reinterpret_cast<char const*>(&src.flags), sizeof(src.flags));
	of.write(reinterpret_cast<char const*>(&src.rows), sizeof(src.rows));
	of.write(reinterpret_cast<char const*>(&src.cols), sizeof(src.cols));
	switch (CV_MAT_DEPTH(src.flags)){
	default:
		throw std::invalid_argument("Invalid type in function write_features.");
	case CV_8U:
		return write_mat_data<std::uint8_t>(of, src);
	case CV_8S:
		return write_mat_data<std::int8_t>(of, src);
	case CV_16U:
		return write_mat_data<std::uint16_t>(of, src);
	case CV_16S:
		return write_mat_data<std::int16_t>(of, src);
	case CV_32S:
		return write_mat_data<std::int32_t>(of, src);
	case CV_32F:
		return write_mat_data<float>(of, src);
	case CV_64F:
		return write_mat_data<double>(of, src);
	}
}

#ifdef TEST_MODE
#include <gtest/gtest.h>

TEST(db_read_write, read_write_mat_test)
{
	cv::Mat m = cv::Mat::zeros(128, 64, CV_8UC1);
	auto const end = m.end<uchar>();
	for (auto it = m.begin<uchar>(); it != end; ++it){
		*it = static_cast<uchar>(rand());
	}
	auto const path = (boost::filesystem::temp_directory_path() / "ghfanfdsaneuiahgarognag").string();
	pierry::write_features(m, path);
	cv::Mat const copy = pierry::read_features(path);
	EXPECT_EQ(m.dims, copy.dims);
	EXPECT_EQ(m.type(), copy.type());
	EXPECT_EQ(m.depth(), copy.depth());
	EXPECT_EQ(m.channels(), copy.channels());
	EXPECT_EQ(m.size(), copy.size());
	for (int row = 0; row < m.rows; ++row){
		for (int col = 0; col < m.cols; ++col){
			EXPECT_EQ(m.at<uchar>(row, col), copy.at<uchar>(row, col));
		}
	}
}

#endif // TEST_MODE