#pragma once
#include "types.h"
#include "KeyBag.h"
#include <tc/Optional.h>
#include <ntd/n3ds/crr.h>

namespace ctrtool {

class CrrProcess
{
public:
	CrrProcess();

	void setInputStream(const std::shared_ptr<tc::io::IStream>& input_stream);
	void setKeyBag(const ctrtool::KeyBag& key_bag);
	void setCliOutputMode(bool show_info);
	void setVerboseMode(bool verbose);
	void setVerifyMode(bool verify);

	void process();
private:
	std::string mModuleLabel;

	std::shared_ptr<tc::io::IStream> mInputStream;
	ctrtool::KeyBag mKeyBag;
	bool mShowInfo;
	bool mVerbose;
	bool mVerify;

	ntd::n3ds::CrrHeader mHeader;
	ntd::n3ds::CrrBodyHeader mBodyHeader;
	tc::ByteData mCrrData;

	byte_t mValidCertificateSignature;
	byte_t mValidBodySignature;
	byte_t mValidUniqueId;

	void importData();
	void verifyData();
	void printData();

	// string utils
	std::string getValidString(byte_t validstate);
};

}