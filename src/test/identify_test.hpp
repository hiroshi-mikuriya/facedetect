#if ! defined TEST_MODE
#error "Use this file with TEST_MODE."
#endif

TEST(identify_test, get_master_from_db)
{
	using namespace pierry;
	cv::Mat features(20, 1, CV_8UC1);
	for (int row = 0; row < features.rows; ++row){
		for (int col = 0; col < features.cols; ++col){
			features.at<uint8_t>(row, col) = static_cast<uint8_t>(row);
		}
	}
	std::vector<cv::KeyPoint> key_points(features.rows);
	for (size_t ix = 0; ix < key_points.size(); ++ix){
		key_points[ix].size = static_cast<float>(ix);
	}
	pierry::feature_points db = { key_points, features };
	name_index_t const n[] = {
			{ { "sub", "a" }, 0, 2 },
			{ { "sub", "b" }, 2, 5 },
			{ { "sub", "c" }, 5, 9 },
			{ { "sub", "d" }, 9, 13 },
			{ { "sub", "e" }, 13, 16 },
			{ { "sub", "f" }, 16, 20 },
	};
	std::vector<name_index_t> names(std::begin(n), std::end(n));

	auto const a = get_master_from_db(db, names, { "sub", "a" });
	EXPECT_EQ(0, a.m_features.at<uint8_t>(0));
	EXPECT_EQ(1, a.m_features.at<uint8_t>(1));
	EXPECT_EQ(0, a.m_key_points[0].size);
	EXPECT_EQ(1, a.m_key_points[1].size);
	auto const b = get_master_from_db(db, names, { "sub", "b" });
	EXPECT_EQ(2, b.m_features.at<uint8_t>(0));
	EXPECT_EQ(3, b.m_features.at<uint8_t>(1));
	EXPECT_EQ(4, b.m_features.at<uint8_t>(2));
	EXPECT_EQ(2, b.m_key_points[0].size);
	EXPECT_EQ(3, b.m_key_points[1].size);
	EXPECT_EQ(4, b.m_key_points[2].size);
	auto const c = get_master_from_db(db, names, { "sub", "c" });
	EXPECT_EQ(5, c.m_features.at<uint8_t>(0));
	EXPECT_EQ(6, c.m_features.at<uint8_t>(1));
	EXPECT_EQ(7, c.m_features.at<uint8_t>(2));
	EXPECT_EQ(8, c.m_features.at<uint8_t>(3));
	EXPECT_EQ(5, c.m_key_points[0].size);
	EXPECT_EQ(6, c.m_key_points[1].size);
	EXPECT_EQ(7, c.m_key_points[2].size);
	EXPECT_EQ(8, c.m_key_points[3].size);
}