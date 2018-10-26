#pragma once

#include "db_options.h"

namespace pierry{

	/*!
	DB情報
	*/
	struct db_infomations
	{
		db_info op;	///< DBオプション
		std::map<std::string, subdb_info> sub;	///< サブDBオプション
	};

	/*!
	DB情報を取得する
	@param[in] 親DBのパス
	@return 全てのサブDB情報
	*/
	db_infomations read_all_db_infomations(boost::filesystem::path const & db);
}