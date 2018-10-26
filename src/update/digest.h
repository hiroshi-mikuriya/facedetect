#pragma once

namespace pierry{
	/*!
	データベース内のファイルからダイジェストを作成し、予めファイル保存したダイジェストと比較する。
	一致ならば終了、不一致ならば今回求めたダイジェストをファイルに保存する。
	また予め保存したファイルがなかった場合は、比較処理はせずファイルに保存する。
	@param[in] db 親DBパス
	@param[in] proc ダイジェストが一致しなかったときにする動作
	@return ダイジェスト一致のときは0、それ以外のときはprocの戻り値
	*/
	int digest_process(boost::filesystem::path const & db, std::function<int()> proc);
}