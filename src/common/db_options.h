#pragma once

#include "pierry.h"

namespace pierry
{
	/*!
	データベース情報を読み込む
	@param[in] db データベースのパス
	@return データベース情報
	*/
	db_info read_db_info(boost::filesystem::path const & db);

	/*!
	データベース情報を保存する
	@param[in] db データベースのパス
	@param[in] src 保存する情報
	*/
	void write_db_info(boost::filesystem::path const & db, db_info const & src);

	/*!
	サブデータベース情報を読み込む
	@param[in] db データベースのパス
	@param[in] name サブデータベース名
	@return サブデータベース情報
	*/
	subdb_info read_subdb_info(boost::filesystem::path const & db, std::string const & name);

	/*!
	サブデータベース情報を書きこむ
	@param[in] db データベースのパス
	@param[in] name サブデータベース名
	@param[in] src 保存する情報
	*/
	void write_subdb_info(boost::filesystem::path const & db, std::string const & name, subdb_info const & src);

	/*!
	サブデータベース情報を読み込む
	@param[in] subdb サブデータベースのパス
	@return サブデータベース情報
	*/
	subdb_info read_subdb_info(boost::filesystem::path const & subdb);

	/*!
	サブデータベース情報を書きこむ
	@param[in] subdb サブデータベースのパス
	@param[in] src 保存する情報
	*/
	void write_subdb_info(boost::filesystem::path const & subdb, subdb_info const & src);
}
