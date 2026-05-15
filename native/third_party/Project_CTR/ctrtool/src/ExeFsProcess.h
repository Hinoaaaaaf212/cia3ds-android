#pragma once
#include "types.h"
#include <tc/Optional.h>
#include <tc/io/IFileSystem.h>
#include <ntd/n3ds/exefs.h>

namespace ctrtool {

class ExeFsProcess
{
public:
	ExeFsProcess();

	void setInputStream(const std::shared_ptr<tc::io::IStream>& input_stream);
	void setCliOutputMode(bool show_header_info, bool show_fs);
	void setVerboseMode(bool verbose);
	void setVerifyMode(bool verify);
	void setRawMode(bool raw);
	void setDecompressCode(bool decompress_code);
	void setExtractPath(const tc::io::Path& extract_path);

	void process();
private:
	std::string mModuleLabel;

	std::shared_ptr<tc::io::IStream> mInputStream;
	bool mShowHeaderInfo;
	bool mShowFs;
	bool mVerbose;
	bool mVerify;
	bool mRaw;
	bool mDecompressCode;
	tc::Optional<tc::io::Path> mExtractPath;

	ntd::n3ds::ExeFsHeader mHeader;
	std::shared_ptr<tc::io::IFileSystem> mFsReader;
	std::array<byte_t, ntd::n3ds::ExeFsHeader::kFileNum> mSectionValidation;

	void importHeader();
	void verifyFs();
	void printHeader();
	void printFs();
	void extractFs();

	// string utils
	std::string getValidString(byte_t validstate);
};

}