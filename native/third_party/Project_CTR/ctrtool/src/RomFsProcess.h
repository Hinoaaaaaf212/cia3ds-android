#pragma once
#include "types.h"
#include "KeyBag.h"
#include <tc/Optional.h>
#include <tc/io/IFileSystem.h>
#include <ntd/n3ds/romfs.h>

namespace ctrtool {

class RomFsProcess
{
public:
	RomFsProcess();

	void setInputStream(const std::shared_ptr<tc::io::IStream>& input_stream);
	void setKeyBag(const ctrtool::KeyBag& key_bag);
	void setCliOutputMode(bool show_header_info, bool show_fs);
	void setVerboseMode(bool verbose);
	void setVerifyMode(bool verify);
	void setExtractPath(const tc::io::Path& extract_path);


	void process();
private:
	std::string mModuleLabel;

	std::shared_ptr<tc::io::IStream> mInputStream;
	ctrtool::KeyBag mKeyBag;
	bool mShowHeaderInfo;
	bool mShowFs;
	bool mVerbose;
	bool mVerify;
	tc::Optional<tc::io::Path> mExtractPath;

	ntd::n3ds::RomFsHeader mHeader;
	std::shared_ptr<tc::io::IFileSystem> mFsReader;
	std::shared_ptr<tc::io::IStream> mStaticCrr;

	void processHeader();
	void printHeader();
	void processCrr();
	void printFs();
	void extractFs();

	void visitDir(const tc::io::Path& v_path, const tc::io::Path& l_path, bool extract_fs, bool print_fs);
};

}