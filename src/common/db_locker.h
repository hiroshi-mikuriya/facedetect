#pragma once

#include <boost/interprocess/sync/file_lock.hpp>

namespace pierry
{
	/*!
	データベースを複数プロセスから参照できないようにするためのクラス
	*/
	class db_locker
	{
		boost::interprocess::file_lock m;
	public:
		/*!
		データベースをロックする
		@param[in] db データベースのパス
		@throw std::exception エラーが発生したら例外を投げる
		*/
		db_locker(boost::filesystem::path const & db);
		/*!
		データベースをアンロックする
		*/
		~db_locker();
	};

	/*!
	データベースロックファイルを作成する
	@param[in] db データベースのパス
	@throw std::exception エラーが発生したら例外を投げる
	*/
	void create_db_lock(boost::filesystem::path const & db);

	/*!
	データベースロックファイルの存在確認
	@param[in] db データベースのパス
	@retval true 存在する
	@retval false 存在しない
	*/
	bool exist_db_lock(boost::filesystem::path const & db);
}
