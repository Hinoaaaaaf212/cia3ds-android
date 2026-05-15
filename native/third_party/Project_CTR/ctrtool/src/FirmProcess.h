#pragma once
#include "types.h"
#include "KeyBag.h"
#include <tc/io/IStream.h>
#include <ntd/n3ds/firm.h>

namespace ctrtool {

class FirmProcess
{
public:
	enum FirmwareType
	{
		FirmwareType_Nand = 0, // NAND signature, sections not encrypted
		FirmwareType_Ngc = 1, // Recovery Signature, but sections are encrypted
		FirmwareType_Nor = 2, // Recovery Signature like NGC, but different section encryption key.
		FirmwareType_Sdmc = 3, // NAND signature, but sections are encrypted.
	};

	FirmProcess();

	void setInputStream(const std::shared_ptr<tc::io::IStream>& input_stream);
	void setKeyBag(const ctrtool::KeyBag& key_bag);
	void setCliOutputMode(bool show_info);
	void setVerboseMode(bool verbose);
	void setVerifyMode(bool verify);
	void setExtractPath(const tc::io::Path& extract_path);
	void setFirmwareType(FirmwareType type);

	void process();
private:
	std::string mModuleLabel;

	std::shared_ptr<tc::io::IStream> mInputStream;
	ctrtool::KeyBag mKeyBag;
	bool mShowInfo;
	bool mVerbose;
	bool mVerify;
	tc::Optional<tc::io::Path> mExtractPath;
	FirmwareType mFirmwareType;

	ntd::n3ds::FirmwareHeader mHeader;
	
	enum SignatureState
	{
		SignatureState_Unchecked = 0,
		SignatureState_Good = 1,
		SignatureState_Fail = 2,
		SignatureState_SigHax = 3,
	};

	byte_t mSignatureState;
	std::array<std::shared_ptr<tc::io::IStream>, 4> mSectionStreams;
	std::array<byte_t, 4> mValidFirmSectionHash;

	void importHeader();
	void generateSectionStreams();
	void verifyHashes();
	void verifySignature();
	void printData();
	void extractSections();

	void createSectionAesIv(std::array<byte_t, 16>& iv, const ntd::n3ds::FirmwareHeader::SectionHeader& section);

	// string utils
	std::string getValidString(byte_t validstate);
	std::string getSignatureStateString(byte_t signature_state);
	std::string getCopyMethodString(uint32_t method);
};

}