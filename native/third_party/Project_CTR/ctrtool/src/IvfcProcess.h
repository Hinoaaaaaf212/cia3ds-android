#pragma once
#include "types.h"
#include "KeyBag.h"
#include "RomFsProcess.h"
#include <tc/Optional.h>
#include <ntd/n3ds/ivfc.h>
#include <ntd/n3ds/IvfcStream.h>

namespace ctrtool {

class IvfcProcess
{
public:
	IvfcProcess();

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

	ntd::n3ds::IvfcCtrRomfsHeader mHeader;
	ctrtool::RomFsProcess mRomFsProcess;
	int64_t mMasterHashOffset;
	std::array<int64_t, ntd::n3ds::IvfcCtrRomfsHeader::kLevelNum> mActualLevelOffsets;
	std::array<byte_t, ntd::n3ds::IvfcCtrRomfsHeader::kLevelNum> mLevelValidation;

	void processHeader();
	void verifyLevels();
	void printHeader();
	void processRomFs();

	// string utils
	std::string getValidString(byte_t validstate);
};

}