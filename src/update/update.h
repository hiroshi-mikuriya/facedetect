#pragma once

namespace pierry{
	/*!
	データベースを最新の状態へ更新
	@param[in] db
	@param[out] o 進捗通知する出力ストリーム
	@retval 0 成功
	@retval 1 失敗
	*/
	int update_impl(boost::filesystem::path const & db, std::ostream & o);

	/*!
	データベースを最新の状態へ更新（データベースをロックする）
	@param[in] db
	@param[out] o 進捗通知する出力ストリーム
	@retval 0 成功
	@retval 1 失敗
	*/
	int lock_update_impl(boost::filesystem::path const & db, std::ostream & o);
}