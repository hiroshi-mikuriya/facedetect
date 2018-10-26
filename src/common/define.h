#pragma once

namespace pierry
{
	/*!
     オプションファイルの拡張子
	*/
	inline std::string option_file_ext()
	{
		return ".txt";
	}
	
	/*!
	親データベースのパラメータファイル名
	*/
	inline std::string db_info_file()
	{
		return "db" + option_file_ext();
	}

	/*!
	親データベースファイルのルートタグ名
	*/
	inline std::string parent_db_tag()
	{
		return "db_option";
	}

	/*!
	サブデータベースのパラメータファイル名
	*/
	inline std::string subdb_info_file()
	{
		return "subdb" + option_file_ext();
	}

	/*!
	サブデータベースファイルのルートタグ名
	*/
	inline std::string sub_db_tag()
	{
		return "subdb_info";
	}

	/*!
	特徴点ファイルの拡張子
	*/
	inline std::string keypoint_ext()
	{
		return ".keypoint";
	}

	/*!
	特徴量ファイルの拡張子
	*/
	inline std::string features_ext()
	{
		return ".features";
	}

	/*!
	Flannのファイル名
	@param[in] zoom_ix ズームのインデックス（ズームごとにFlannを作成するため）
	*/
	inline std::string flann_name(size_t zoom_ix)
	{
		return (boost::format("flann_%d.bin") % zoom_ix).str();
	}

	/*!
	ダイジェスト（ハッシュ）のファイル名
	*/
	inline std::string digest_name()
	{
		return "digest.bin";
	}

	/*!
	データベースロックファイル名
	*/
	inline std::string db_lock_name()
	{
		return "db.lock";
	}
}
