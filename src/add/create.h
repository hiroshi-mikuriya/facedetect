#pragma once

#include "db_options.h"

namespace pierry{
	/*!
	親データベースを作成する
	@param[in] db 親データベースディレクトリ
	@param[in] scales マスター画像の縮尺率
	@param[in] orb ORBパラメータ
	@param[in] freak FREAKパラメータ
	@retval 0 成功
	@retval 1 失敗
	*/
	int create_parent_db_impl(
		boost::filesystem::path const & db,
		std::vector<zoom_or_pix> const & scales,
		orb_params const & orb,
		freak_params const & freak);

	/*!
	親DBの存在確認
	ディレクトリだけではなく必要なファイルが全て揃っていることも確認する
	@param[in] db 親データベースディレクトリ
	@retval true 存在している
	@retval false 存在しない
	*/
	bool exist_parent_db(boost::filesystem::path const & db);
}