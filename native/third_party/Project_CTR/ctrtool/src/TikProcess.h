#pragma once
#include "types.h"
#include "KeyBag.h"
#include <tc/Optional.h>

#include <ntd/n3ds/es/RsaSigner.h>
#include <ntd/n3ds/es/Certificate.h>
#include <ntd/n3ds/es/Ticket.h>

namespace ctrtool {

class TikProcess
{
public:
	TikProcess();

	void setInputStream(const std::shared_ptr<tc::io::IStream>& input_stream);
	void setKeyBag(const ctrtool::KeyBag& key_bag);
	void setCliOutputMode(bool show_header_info);
	void setVerboseMode(bool verbose);
	void setVerifyMode(bool verify);
	
	void process();
private:
	std::string mModuleLabel;

	// input args
	std::shared_ptr<tc::io::IStream> mInputStream;
	ctrtool::KeyBag mKeyBag;
	bool mShowInfo;
	bool mVerbose;
	bool mVerify;

	// process variables
	std::map<std::string, std::shared_ptr<ntd::n3ds::es::ISigner>> mIssuerSigner;
	std::map<std::string, std::shared_ptr<ntd::n3ds::es::ISigner>> mCertImportedIssuerSigner;

	std::vector<ntd::n3ds::es::Certificate> mCertChain;
	std::vector<ValidState> mCertSigValid;
	
	ntd::n3ds::es::Ticket mTicket;
	ValidState mTicketSigValid;

	tc::Optional<KeyBag::Aes128Key> mDecryptedTitleKey;

	// helper methods
	void importIssuerProfiles();
	void importData();
	void verifyData();
	void printData();

	// string utils
	std::string getValidString(byte_t validstate);
	std::string getTruncatedBytesString(const byte_t* data, size_t len, bool do_not_truncate = false);
	std::string getSigTypeString(brd::es::ESSigType sig_type);
	std::string getCertificatePublicKeyTypeString(brd::es::ESCertPubKeyType public_key_type);
	std::string getTitleVersionString(uint16_t version);
};

}