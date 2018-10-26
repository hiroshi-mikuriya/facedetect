#pragma once

#include "db_options.h"

namespace pierry{
	//FIXME: リンクエラー回避のための邪悪なマクロ
#if defined _CONSOLE
#define create_feature_keypoint_file create_feature_keypoint_file_HEAVEN
#define get_image_files get_image_files_HEAVEN
#define get_sub_db_directories get_sub_db_directories_HEAVEN
#define read_features_and_key_points read_features_and_key_points_HEAVEN
#define normalized normalized_HEAVEN
#define get_leaf_directory get_leaf_directory_HEAVEN
#endif

	/*!
	ディレクトリ直下の画像ファイルのパスを全て取得する
	@param[in] dir ディレクトリ
	@return 画像ファイルのパス
	*/
	std::vector<boost::filesystem::path> get_image_files(boost::filesystem::path const & dir);

	/*!
	サブDBのパスを全て取得する
	@param[in] db DBディレクトリ
	@return 全てのサブDBのパス
	*/
	std::vector < boost::filesystem::path> get_sub_db_directories(boost::filesystem::path const & db);

	/*!
	マスターファイルから特徴量、特徴点を求めサブDBへ保存する
	特徴量、特徴点のファイルが既に用意されており、マスタ―ファイルのタイムスタンプより新しい場合は、保存しない
	@param[in] master マスターファイル
	@param[in] save_dir 保存先のフォルダ
	@param[in] op 親DBのパラメータ
	@param[in] scale 画像縮尺率
	@retval 0 ファイル保存完了
	@retval 1 タイムスタンプが新しかったため保存しなかった。
	@throw std::exception エラーが発生したら例外を投げる
	*/
	int create_feature_keypoint_file(
		boost::filesystem::path const & master,
		boost::filesystem::path const & save_dir,
		pierry::db_info const & op,
		zoom_or_pix const & scale);

	/*!
	マスター画像を特定するための情報
	*/
	struct file_identifier
	{
		std::string subdb_name;	///< サブデータベース名.
		std::string file_name;	///< ファイル名.
	};

	/*!
	file_identifierの比較演算
	@param[in] a 
	@param[in] b
	@return 比較結果
	*/
	inline bool operator<(file_identifier const & a, file_identifier const & b)
	{
		return std::make_pair(a.subdb_name, a.file_name) < std::make_pair(b.subdb_name, b.file_name);
	}

	/*!
	file_identifierの比較演算
	@param[in] a
	@param[in] b
	@return 比較結果
	*/
	inline bool operator==(file_identifier const & a, file_identifier const & b)
	{
		return std::make_pair(a.subdb_name, a.file_name) == std::make_pair(b.subdb_name, b.file_name);
	}

	/*!
	マスターファイル名、サブDB名、特徴点数を持つ型
	*/
	struct name_index_t
	{
		file_identifier file;	///< マスターファイルを特定するための情報
		int begin;	///< マスターデータの中での、本ファイルの特徴点の開始位置
		int end;	///<  マスターデータの中での、本ファイルの特徴点の終了位置の次

		bool match(int ix)const{ return begin <= ix && ix < end; }
	};

	/*!
	特徴点と特徴量をDBファイルから読みだす
	@param[in] saved_dir 保存されているディレクトリ（サブDB以下のズームインデックスディレクトリ）
	@param[in] master_dir マスターファイルのディレクトリ（namesにフルパス格納するため）
	@param[in] subdb_name サブDB名（namesに格納するため）
	@param[out] features 特徴量
	@param[out] key_point 特徴点
	@param[out] names ファイル名
	*/
	void read_features_and_key_points(
		boost::filesystem::path const & saved_dir,
		boost::filesystem::path const & master_dir,
		std::string const & subdb_name,
		cv::Mat & features,
		std::vector<cv::KeyPoint> & key_point,
		std::vector<name_index_t> & names);

	/*! 末尾のバックスラッシュを含めた正規化。
	 hoge/fuga/ のような path や hoge/fuga/. のような path をともに /absolute/path/hoge/fuga にする。
	 @param[in] path 正規化前のパス。例えば "hoge/fuga/."
	 @return 正規化後の絶対パス。例えば "/absolute/path/hoge/fuga"。
	 */
	auto normalized(boost::filesystem::path const & path)->boost::filesystem::path;

	/*!
	配列の中身が一致するか確認する
	@param[in] a 配列A
	@param[in] b 配列B
	@retval true 一致
	@retval false 不一致
	*/
	template<typename Type, size_t n>
	bool same_array(Type const (&a)[n], Type const (&b)[n])
	{
		for (size_t ix = 0; ix < n; ++ix){
			if (a[ix] != b[ix]){
				return false;
			}
		}
		return true;
	}

	/*!
	サブDB内のズームごとの特徴点ファイルの保存先を取得する
	@param[in] subdb サブDBのディレクトリ
	@param[in] zoom_ix ズームのインデックス
	@return ズームのインデックスに対応したディレクトリ
	*/
	boost::filesystem::path get_leaf_directory(boost::filesystem::path const & subdb, size_t zoom_ix);

	/*!
	複数のスレッドを同時に走らせ、スレッド内で発生する例外を rethrow する。.
	@param[in] count スレッドの個数.
	@param[in] proc スレッド内で行うべき処理を示す std::functino を返す関数.
	@note proc の戻り値の引数 ix は、スレッドのインデックス.
	@note 複数のスレッド内で例外が発生した場合、インデックスが最小のスレッドの例外のみが rethrow される。.
	*/
	void run_threads(size_t count, std::function<std::function<void(void)>(size_t ix)> const & proc);
}