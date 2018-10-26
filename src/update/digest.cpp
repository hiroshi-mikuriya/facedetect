#include "stdafx.h"
#include "digest.h"
#include "utils.h"
#include "define.h"
#include <boost/uuid/sha1.hpp>

namespace
{
	/*!
	ダイジェストを更新
	@param[in] dir ディレクトリ
	@param[inout] sha1 SHA-1
	*/
	void mod_digest(boost::filesystem::path const & dir, boost::uuids::detail::sha1 & sha1)
	{
		boost::filesystem::directory_iterator it(dir);
		for (; it != boost::filesystem::directory_iterator(); ++it){
			if (boost::filesystem::is_directory(it->path())){
				if (it->path().leaf().string().c_str()[0] != '.'){	// c_strにすることでバッファオーバーランを防止している
					mod_digest(it->path(), sha1);
				}
				continue;
			}
			std::ifstream ifs(it->path().string());
			while (!ifs.eof()){
				unsigned char byte = 0;
				ifs >> byte;
				sha1.process_byte(byte);
			}
		}
	}
}

/*!
データベース内のファイルからダイジェストを作成し、予めファイル保存したダイジェストと比較する。
一致ならば終了、不一致ならば今回求めたダイジェストをファイルに保存する。
また予め保存したファイルがなかった場合は、比較処理はせずファイルに保存する。
@param[in] db 親DBパス
@param[in] proc ダイジェストが一致しなかったときにする動作
@return ダイジェスト一致のときは0、それ以外のときはprocの戻り値
@note flannの保存する前後でダイジェストの作成と保存が発生し、処理が散らばってしまうのを避けるためprocを用いてこの構造にした。
*/
int pierry::digest_process(boost::filesystem::path const & db, std::function<int()> proc)
{
	auto const sub_dbs = pierry::get_sub_db_directories(db);
	std::cerr << "calculating digest." << std::endl;
	{
		boost::uuids::detail::sha1 sha1;
		unsigned char const salt[] = { 159, 172, 244, 210, 34, 81, 27, 238, 202, 101, 94, 97, 18, 25, 30, 157, 34, 250, 212, 62, 206, 192, 12, 199, 248, 134, 91, 97, 115, 150, 136, 151 };
		sha1.process_block(std::begin(salt), std::end(salt));
		for (auto const & sub : sub_dbs){
			mod_digest(sub, sha1);
		}
		unsigned int digest[5] = { 0 };
		sha1.get_digest(digest);
		auto const digest_filename = (db / pierry::digest_name()).string();
		if (boost::filesystem::exists(digest_filename) && !boost::filesystem::is_directory(digest_filename)){
			unsigned int digest2[5] = { 0 };
			std::ifstream ifs(digest_filename, std::ios::binary);
			ifs.read(reinterpret_cast<char*>(digest2), sizeof(digest2));
			if (pierry::same_array(digest, digest2)){
				std::cerr << "digest is matched." << std::endl;
				return 0;
			}
		}
		int ret = proc();
		std::cerr << "creating digest file." << std::endl;
		std::ofstream ofs(digest_filename, std::ios::binary);
		ofs.write(reinterpret_cast<char*>(digest), sizeof(digest));
		return ret;
	}
}
