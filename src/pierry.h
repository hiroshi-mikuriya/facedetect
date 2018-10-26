#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <iostream>

namespace pierry{

	/*!
	ORBアルゴリズムで特徴点を求めるパラメータ
	*/
	struct orb_params
	{
		orb_params();
		int nfeatures;
		float scaleFactor;
		int nlevels;
		int edgeThreshold;
		int firstLevel;
		int WTA_K;
		int scoreType;
		int patchSize;
	};

	/*!
	FREAKアルゴリズムで特徴量を求めるパラメータ
	*/
	struct freak_params
	{
		freak_params();
		bool orientationNormalized;
		bool scaleNormalized;
		float patternScale;
		int nOctaves;
		std::vector<int> selectedPairs;
	};

	/*!
	ライブラリのバージョンを表す型
	*/
	struct version_info
	{
		int major;	///< メジャーバージョン
		int minor;	///< マイナーバージョン
		int revision;	///< リビジョンバージョン
	};

	/*!
	画像縮尺率または長辺ピクセル数を持つバリアント型
	*/
	class zoom_or_pix;

	/*!
	データベース情報
	*/
	struct db_info
	{
		orb_params orb;	///< ORBアルゴリズムのパラメータ
		freak_params freak;	///< FREAKアルゴリズムのパラメータ
		version_info lib_version;	///< データベースを作成時のライブラリバージョン
		std::vector<double> default_scales;	///< デフォルト使用するマスター画像ファイルの縮尺率
	};

	/*!
	サブデータベース情報
	*/
	struct subdb_info
	{
		std::string master_directory;	///< マスター画像ファイルのディレクトリ
		std::vector<double> scales;	///< 画像縮尺率
		int file_count;	///< マスターディレクトリの画像ファイル数
	};

	/*!
	ライブラリのバージョンを取得する
	@return ライブラリのバージョン
	*/
	version_info get_lib_version();

	/*!
	ライブラリのバージョンやライセンスのテキストを取得する
	@return 上記テキスト
	*/
	std::string get_lib_description();

	/*!
	データベースの読み書き操作をするクラス
	*/
	class db_operator
	{
		std::string m_db;	///< データベースのパス
	public:

		/*!
		データベースのパスを設定する
		@param[in] db データベースのパス
		*/
		explicit db_operator(std::string const & db);

		/*!
		空のデータベースを作成する
		@param[in] default_scales デフォルトの縮尺率
		@param[in] orb 特徴点を求めるパラメータ
		@param[in] freak 特徴量を求めるパラメータ
		@throw std::exception 作成失敗
		@note 作成済みの場合は何もせず終了する
		*/
		void create(std::vector<zoom_or_pix> const & default_scales, orb_params const & orb = orb_params(), freak_params const & freak = freak_params())const;

		/*!
		データベース作成有無を確認する
		@retval true 作成済み
		@retval false 未了
		*/
		bool created()const;

		/*!
		データベース情報を読み込む
		@return データベース情報
		@throw std::exception データベースが存在しない、ファイルが壊れているときなどに例外を投げる
		*/
		db_info read_options()const;

		/*!
		サブデータベースを追加する
		@param[in] master_directory 追加する画像ファイルを持つディレクトリのパス。このディレクトリ以下の全画像ファイルが追加対象となる。
		@param[in] scales 画像縮尺率または長辺ピクセル数（空の場合、デフォルトに従う）
		@param[in] name サブデータベース名
		@param[in] with_update 追加後にデータベース全体をアップデートする。連続追加する場合はfalseにし、最後の追加後にアップデートすればよい。
		@param[out] o 進捗状況を通知する出力ストリーム
		@note 処理中はデータベースをロックする
		@throw std::exception ディレクトリパスが存在しないなどのエラーが発生したら例外を投げる。
		*/
		void add(
			std::string const & master_directory,
			std::vector<zoom_or_pix> const & scales,
			std::string const & name,
			bool with_update = true,
			std::ostream & o = std::cout)const;

		/*!
		サブデータベースを追加する。nameは画像ファイルのディレクトリ名を用いる。
		@param[in] master_directory 追加する画像ファイルを持つディレクトリのパス。このディレクトリ以下の全画像ファイルが追加対象となる。
		@param[in] scales 画像縮尺率または長辺ピクセル数（空の場合、デフォルトに従う）
		@param[in] with_update 追加後にデータベース全体をアップデートする。連続追加する場合はfalseにし、最後の処理後にアップデートすればよい。
		@param[out] o 進捗状況を通知する出力ストリーム
		@note 処理中はデータベースをロックする
		@throw std::exception ディレクトリパスが存在しないなどのエラーが発生したら例外を投げる。
		*/
		void add(
			std::string const & master_directory,
			std::vector<zoom_or_pix> const & scales,
			bool with_update = true,
			std::ostream & o = std::cout)const;

		/*!
		データベース情報を取得する
		@return データベース情報
		*/
		db_info get_db_info()const;

		/*!
		サブデータベース情報を取得する
		@return first サブデータベース名
		@return second サブデーターベース情報
		@note データベースがロックされていても、情報を取得できる。ただし内容は保証しない。
		@throw std::exception データベース未作成など、エラーが発生したら例外を投げる
		*/
		std::map<std::string, subdb_info> get_subdb_infos()const;

		/*!
		サブデータベースを削除する
		@param[in] name サブデータベース名
		@param[in] with_update 削除後にデータベース全体をアップデートする。連続削除する場合はfalseにし、最後の処理後にアップデートすればよい。
		@param[out] o 進捗状況を通知する出力ストリーム
		@note 処理中はデータベースをロックする
		@throw std::exception サブデータベースが存在しないなど、エラーが発生したら例外を投げる
		*/
		void remove(std::string const  & name, bool with_update = true, std::ostream & o = std::cout)const;

		/*!
		データベースをアップデートする
		@note 処理中はデータベースをロックする
		@param[out] o 進捗状況を通知する出力ストリーム
		@throw std::exception データベース未作成など、エラーが発生したら例外を投げる.
		*/
		void update(std::ostream & o = std::cout)const;
	};

	/*!
	検索時の入力パラメータ
	*/
	struct search_params
	{
		std::string filename;	///< ファイルパス.
		std::vector<zoom_or_pix> scales;	///< 画像縮尺率または長辺ピクセル数.
		int candidate_count;	///< 結果ファイルの最大候補数.
	};

	/*!
	検索結果情報.
	*/
	struct search_result
	{
		std::string filename;	///< 検索結果ファイル名.
		std::string subdb;	///< サブデータベース名.
		double score;	///< 検索スコア.
	};

	/*!
	検索結果のコンテナ.
	*/
	typedef std::vector<search_result> search_results;


	/*!
	データベースの検索をするクラス.
	*/
	class db_identifier
	{
		struct impl;	///< メンバを外部I/Fから隠ぺいするためのクラス.
		typedef std::vector< std::shared_ptr<impl> > members_t;
		members_t m;///< 縮尺率ごとの、特徴点などの情報の集まり。.
		std::string m_db;	///< データベースのパス.
		db_operator m_operator;	///< データベースの操作を行う.
	public:

		/*!
		データベースのパスを設定する.
		@param[in] db データベースのパス.
		@note データベースのパスを設定するだけで、ロードは実行しない.
		*/
		explicit db_identifier(std::string const & db);

		/*!
		データベースをロードする.
		@param[in] with_update データベースを更新してから読み込む.
		@param[out] o 進捗状況を通知する出力ストリーム（データベースサイズが大きいときに時間がかかるため）.
		@throw std::exception ロード失敗.
		*/
		void load(bool with_update = true, std::ostream & o = std::cout);

		/*!
		データベースのロード有無判定.
		@retval true ロード済み.
		@retval false ロード未了.
		*/
		bool loaded()const;

		/*!
		データベースから画像を検索する.
		@param[in] src 検索パラメータ.
		@return 検索結果.
		@throw std::invalid_argument 検索パラメータに誤りがある.
		@throw std::exception 上記以外のエラー.
		*/
		search_results search(search_params const & src);

		/*!
		データベースの読み込みをクリアする.
		@note クリア済みで本APIを呼び出したときは何もせず終了する.
		*/
		void clear();
	};

	class zoom_or_pix
	{
		enum class type_tag{
			not_initialized, zoom, pix
		};
		type_tag m_tag;
		union{
			double zoom;
			int pix;
		}m;
	public:
		zoom_or_pix() : m_tag(type_tag::not_initialized){}
		zoom_or_pix(int pix) : m_tag(type_tag::pix){ m.pix = pix; }
		zoom_or_pix(double zoom) : m_tag(type_tag::zoom){ m.zoom = zoom; }
		zoom_or_pix(zoom_or_pix const & src) = default;
		zoom_or_pix & operator=(int pix){ *this = zoom_or_pix(pix); return *this; }
		zoom_or_pix & operator=(double zoom){ *this = zoom_or_pix(zoom); return *this; }
		zoom_or_pix & operator=(zoom_or_pix const & src){ zoom_or_pix tmp(src); swap(tmp); return *this; }
		void swap(zoom_or_pix & rhs){ std::swap(m_tag, rhs.m_tag); std::swap(m, rhs.m); }
		bool is_zoom()const{ return type_tag::zoom == m_tag; }
		bool is_pix()const{ return type_tag::pix == m_tag; }
		bool is_initialized() const { return is_zoom() || is_pix(); }
		double zoom()const{ if (!is_zoom())throw std::invalid_argument("It's not zoom."); return m.zoom; }
		int pix()const{ if (!is_pix())throw std::invalid_argument("It's not pix."); return m.pix; }
		friend inline bool operator==(zoom_or_pix const & a, zoom_or_pix const & b){
			if (a.m_tag != b.m_tag){
				return false;
			}
			switch (a.m_tag){
			case zoom_or_pix::type_tag::zoom:
				return a.zoom() == b.zoom();
			case zoom_or_pix::type_tag::pix:
				return a.pix() == b.pix();
			default:
				return true;
			}
		}
	};
}