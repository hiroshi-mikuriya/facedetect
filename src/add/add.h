#pragma once

#include "pierry.h"

namespace pierry{
	/*!
	データベースへのサブDB追加処理
	@param[in] master_dir マスターファイルのディレクトリ
	@param[in] db データベースディレクトリ
	@param[in] name サブDB名
	@param[in] scales マスター画像イメージの縮尺率または長辺ピクセル数
	@param[in] with_update サブDB追加後、全てのDBアップデート有無
	@param[out] o 進捗通知する出力ストリーム
	@retval 0 成功
	@retval 1 失敗
	@note dbディレクトリが存在しない場合は新規に作成する
	*/
	int add_impl(
		boost::filesystem::path const & master_dir,
		boost::filesystem::path const & db,
		std::string const & name,
		std::vector<zoom_or_pix> const & scales,
		bool with_update,
		std::ostream & o);
}