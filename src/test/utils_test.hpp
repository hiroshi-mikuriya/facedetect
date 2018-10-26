#if ! defined TEST_MODE
#error "Use this file with TEST_MODE."
#endif

// to help intellisense
#include "stdafx.h"
#include "update.h"
#include "utils.h"
#include "define.h"
#include "db_locker.h"
#include <boost/uuid/sha1.hpp>

TEST(update, sha1_base)
{
    boost::uuids::detail::sha1 sha1;
    unsigned int digest[5] = { 0 };
    sha1.get_digest( digest );
    unsigned int expected[5]={0xda39a3ee,0x5e6b4b0d,0x3255bfef,0x95601890,0xafd80709};
    EXPECT_TRUE( pierry::same_array( digest, expected) );
}


TEST(utils, same_array)
{
	int a0[5] = { 1, 2, 3, 4, 5 };
	int a1[5] = { 1, 2, 3, 4, 5 };
	int b[5] = { 1, 2, 3, 4, 5 + (1 << 31) };
	int c[5] = { 0, 2, 3, 4, 5 };
	EXPECT_TRUE(pierry::same_array(a0, a1));
	EXPECT_FALSE(pierry::same_array(a0, b));
	EXPECT_FALSE(pierry::same_array(a1, c));
}

TEST(utils, normalized_can_normalize)
{
	using pierry::normalized;
	using boost::filesystem::complete;
	EXPECT_EQ(complete("hoge"), normalized("hoge"));
	EXPECT_EQ(complete("hoge"), normalized("hoge/"));
	EXPECT_EQ(complete("hoge"), normalized("hoge/."));
}

TEST(utils, end_of_normalized_is_not_backslash)
{
	using pierry::normalized;
	using boost::filesystem::complete;
	auto impl = [](char e, std::string const & fn){
		auto no = normalized(fn).string();
		EXPECT_EQ(e, no.at(no.size() - 1));
	};
	impl('e', "hoge");
	impl('e', "hoge/");
	impl('e', "hoge/./././");
	impl('e', "hoge/././.");
}

