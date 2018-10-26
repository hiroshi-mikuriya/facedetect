#pragma once

namespace pierry{
	/*!
	データベースからサブDBを削除する
	@param[in] db データベースのパス
	@param[in] name サブDBの名前
	@param[in] with_update サブDB削除後、全てのDBアップデート有無
	@param[out] o 進捗通知する出力ストリーム
	@retval 0 成功
	@retval 1 失敗
	*/
	int remove_impl(boost::filesystem::path const & db, std::string const & name, bool with_update, std::ostream & o);
}