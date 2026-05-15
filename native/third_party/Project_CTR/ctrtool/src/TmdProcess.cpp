#include "TmdProcess.h"
#include <tc/io.h>
#include <tc/cli.h>
#include <tc/crypto.h>
#include <tc/ArgumentNullException.h>
#include <tc/NotSupportedException.h>

ctrtool::TmdProcess::TmdProcess() :
	mModuleLabel("ctrtool::TmdProcess"),
	mInputStream(),
	mKeyBag(),
	mShowInfo(false),
	mVerbose(false),
	mVerify(false),
	mIssuerSigner(),
	mCertImportedIssuerSigner(),
	mCertChain(),
	mCertSigValid(),
	mTitleMetaData(),
	mTitleMetaDataSigValid(ValidState::Unchecked)
{
}

void ctrtool::TmdProcess::setInputStream(const std::shared_ptr<tc::io::IStream>& input_stream)
{
	mInputStream = input_stream;
}

void ctrtool::TmdProcess::setKeyBag(const ctrtool::KeyBag& key_bag)
{
	mKeyBag = key_bag;
}

void ctrtool::TmdProcess::setCliOutputMode(bool show_info)
{
	mShowInfo = show_info;
}

void ctrtool::TmdProcess::setVerboseMode(bool verbose)
{
	mVerbose = verbose;
}

void ctrtool::TmdProcess::setVerifyMode(bool verify)
{
	mVerify = verify;
}

void ctrtool::TmdProcess::process()
{
	importIssuerProfiles();
	importData();

	if (mVerify)
	{
		verifyData();
	}
		
	if (mShowInfo)
	{
		printData();
	}
}

void ctrtool::TmdProcess::importIssuerProfiles()
{
	// import issuer profiles from keybag
	for (auto itr = mKeyBag.broadon_rsa_signer.begin(); itr != mKeyBag.broadon_rsa_signer.end(); itr++)
	{
		brd::es::ESSigType sigType = itr->first == "Root" ? brd::es::ESSigType::RSA4096_SHA256 : brd::es::ESSigType::RSA2048_SHA256;
		mIssuerSigner.insert(std::pair<std::string, std::shared_ptr<ntd::n3ds::es::ISigner>>(itr->first, std::make_shared<ntd::n3ds::es::RsaSigner>(ntd::n3ds::es::RsaSigner(sigType, itr->first, itr->second.key))));
	}
}

void ctrtool::TmdProcess::importData()
{
	// validate input stream
	if (mInputStream == nullptr)
	{
		throw tc::ArgumentNullException(mModuleLabel, "Input stream was null.");
	}
	if (mInputStream->canRead() == false || mInputStream->canSeek() == false)
	{
		throw tc::InvalidOperationException(mModuleLabel, "Input stream requires read/seek permissions.");
	}

	// process tmd
	{
		mTitleMetaData = ntd::n3ds::es::TitleMetaDataDeserialiser(mInputStream);
		mTitleMetaDataSigValid = ValidState::Unchecked;
	}

	// process trailing cert chain (this assumes ntd::n3ds::es::TitleMetaDataDeserialiser leaves the stream position at the end of the tmd data)
	while (mInputStream->position() < mInputStream->length())
	{
		std::shared_ptr<tc::io::IStream> cert_stream = std::make_shared<tc::io::SubStream>(tc::io::SubStream(mInputStream, mInputStream->position(), mInputStream->length() - mInputStream->position()));
		mCertChain.push_back(ntd::n3ds::es::CertificateDeserialiser(cert_stream));
		mCertSigValid.push_back(ValidState::Unchecked);

		// update position of input stream
		//mInputStream->seek(cert_stream->position(), tc::io::SeekOrigin::Current);

		// import issuer profile from certificate
		if (mCertChain.back().public_key_type == brd::es::ESCertPubKeyType::RSA2048)
		{
			std::string issuer = fmt::format("{}-{}", mCertChain.back().signature.issuer, mCertChain.back().subject);
			brd::es::ESSigType sig_type = brd::es::ESSigType::RSA2048_SHA256;
			auto& public_key = mCertChain.back().rsa2048_public_key;
			
			mCertImportedIssuerSigner.insert(std::pair<std::string, std::shared_ptr<ntd::n3ds::es::ISigner>>(issuer, std::make_shared<ntd::n3ds::es::RsaSigner>(ntd::n3ds::es::RsaSigner(sig_type, issuer, tc::crypto::RsaPublicKey(public_key.m.data(), public_key.m.size())))));
		}
		else if (mCertChain.back().public_key_type == brd::es::ESCertPubKeyType::RSA4096)
		{
			std::string issuer = fmt::format("{}-{}", mCertChain.back().signature.issuer + mCertChain.back().subject);
			brd::es::ESSigType sig_type = brd::es::ESSigType::RSA4096_SHA256;
			auto& public_key = mCertChain.back().rsa4096_public_key;
			
			mCertImportedIssuerSigner.insert(std::pair<std::string, std::shared_ptr<ntd::n3ds::es::ISigner>>(issuer, std::make_shared<ntd::n3ds::es::RsaSigner>(ntd::n3ds::es::RsaSigner(sig_type, issuer, tc::crypto::RsaPublicKey(public_key.m.data(), public_key.m.size())))));
		}
	}
}

void ctrtool::TmdProcess::verifyData()
{
	// verify cert
	for (size_t i = 0; i < mCertChain.size(); i++)
	{
		auto local_issuer_itr = mCertImportedIssuerSigner.find(mCertChain[i].signature.issuer);
		auto keybag_issuer_itr = mIssuerSigner.find(mCertChain[i].signature.issuer);
		
		// first try with the issuer profiles imported from the local certificates
		if (local_issuer_itr != mCertImportedIssuerSigner.end() && local_issuer_itr->second->getSigType() == mCertChain[i].signature.sig_type)
		{
			mCertSigValid[i] = local_issuer_itr->second->verifyHash(mCertChain[i].calculated_hash.data(), mCertChain[i].signature.sig.data()) ? ValidState::Good : ValidState::Fail;
		}
		// fallback try with the keybag imported issuer
		else if (keybag_issuer_itr != mIssuerSigner.end() && keybag_issuer_itr->second->getSigType() == mCertChain[i].signature.sig_type)
		{
			// only show this warning for non-root signed certificates
			if (mCertChain[i].signature.issuer != "Root")
			{
				fmt::print(stderr, "[{} ERROR] Public key \"{}\" (for certificate \"{}\") was not present in the certificate chain. The public key included with CTRTool was used instead.\n", mModuleLabel, mCertChain[i].signature.issuer, mCertChain[i].subject);
			}
			mCertSigValid[i] = keybag_issuer_itr->second->verifyHash(mCertChain[i].calculated_hash.data(), mCertChain[i].signature.sig.data()) ? ValidState::Good : ValidState::Fail;
		}
		else
		{
			// cannot locate rsa key to verify
			fmt::print(stderr, "[{} ERROR] Could not locate public key for \"{}\" (certificate).\n", mModuleLabel, mCertChain[i].signature.issuer);
			mCertSigValid[i] = ValidState::Fail;
		}

		// log certificate signature validation error
		if (mCertSigValid[i] != ValidState::Good)
		{
			fmt::print(stderr, "[{} ERROR] Signature for Certificate \"{}\" was invalid.\n", mModuleLabel, mCertChain[i].signature.issuer);
		}
	}

	// verify ticket
	{
		// verify ticket
		auto local_issuer_itr = mCertImportedIssuerSigner.find(mTitleMetaData.signature.issuer);
		auto keybag_issuer_itr = mIssuerSigner.find(mTitleMetaData.signature.issuer);

		// first try with the issuer profiles imported from the local certificates
		if (local_issuer_itr != mCertImportedIssuerSigner.end() && local_issuer_itr->second->getSigType() == mTitleMetaData.signature.sig_type)
		{
			mTitleMetaDataSigValid = local_issuer_itr->second->verifyHash(mTitleMetaData.calculated_hash.data(), mTitleMetaData.signature.sig.data()) ? ValidState::Good : ValidState::Fail;
		}
		// fallback try with the keybag imported issuer
		else if (keybag_issuer_itr != mIssuerSigner.end() && keybag_issuer_itr->second->getSigType() == mTitleMetaData.signature.sig_type)
		{
			// only show this warning when there are certificates appended to the tmd (only tmd downloaded from CDN will have an appended certificate chain)
			if (mCertChain.size() != 0)
			{
				fmt::print(stderr, "[{} ERROR] Public key \"{}\" (for tmd) was not present in the appended certificate chain. The public key included with CTRTool was used instead.\n", mModuleLabel, mTitleMetaData.signature.issuer);
			}
			mTitleMetaDataSigValid = keybag_issuer_itr->second->verifyHash(mTitleMetaData.calculated_hash.data(), mTitleMetaData.signature.sig.data()) ? ValidState::Good : ValidState::Fail;
		}
		else
		{
			// cannot locate rsa key to verify
			fmt::print(stderr, "[{} ERROR] Could not locate public key \"{}\" (for tmd).\n", mModuleLabel, mTitleMetaData.signature.issuer);
			mTitleMetaDataSigValid = ValidState::Fail;
		}

		// log tmd signature validation error
		if (mTitleMetaDataSigValid != ValidState::Good)
		{
			fmt::print(stderr, "[{} ERROR] Signature for TitleMetaData was invalid.\n", mModuleLabel);
		}
	}
}

void ctrtool::TmdProcess::printData()
{	
	{
		fmt::print("TitleMetaData:\n");
		fmt::print("|- DigitalSignature: {:s}\n", getValidString(mTitleMetaDataSigValid));
		fmt::print("|  |- SigType:    {:s} (0x{:x})\n", getSigTypeString(mTitleMetaData.signature.sig_type), (uint32_t)mTitleMetaData.signature.sig_type);
		fmt::print("|  |- Issuer:     {:s}\n", mTitleMetaData.signature.issuer);
		fmt::print("|  \\- Signature:  {:s}\n", getTruncatedBytesString(mTitleMetaData.signature.sig.data(), mTitleMetaData.signature.sig.size(), mVerbose));
		fmt::print("|- TitleId:       {:016x}\n", mTitleMetaData.title_id);
		fmt::print("|- TitleVersion:  {} ({:d})\n", getTitleVersionString(mTitleMetaData.title_version), mTitleMetaData.title_version);
		fmt::print("|- CustomData:\n");
		// TWL Title
		if (isTwlTitle(mTitleMetaData.title_id))
		{
			fmt::print("|  |- PublicSaveDataSize:  0x{:x}\n", mTitleMetaData.twl_custom_data.public_save_data_size);
			fmt::print("|  |- PrivateSaveDataSize: 0x{:x}\n", mTitleMetaData.twl_custom_data.private_save_data_size);
			fmt::print("|  \\- Flag:                0x{:02x}\n", mTitleMetaData.twl_custom_data.flag);
		}
		// CTR
		else
		{
			fmt::print("|  |- SaveDataSize: 0x{:x}\n", mTitleMetaData.ctr_custom_data.save_data_size);
			fmt::print("|  \\- IsSnakeOnly: {}\n", mTitleMetaData.ctr_custom_data.is_snake_only);
		}
		fmt::print("\\- ContentInfo:\n");
		for (size_t i = 0; i < mTitleMetaData.content_info.size(); i++)
		{
			fmt::print("   {:1}- 0x{:04x}:\n", (i+1 < mTitleMetaData.content_info.size() ? "|" : "\\"), mTitleMetaData.content_info[i].index);
			fmt::print("   {:1}  |- ContentId:   0x{:08x}\n", (i+1 < mTitleMetaData.content_info.size() ? "|" : ""), mTitleMetaData.content_info[i].id);
			fmt::print("   {:1}  |- Encrypted:   {}\n", (i+1 < mTitleMetaData.content_info.size() ? "|" : ""), (mTitleMetaData.content_info[i].is_encrypted ? "YES" : "NO"));
			fmt::print("   {:1}  |- Optional:    {}\n", (i+1 < mTitleMetaData.content_info.size() ? "|" : ""), (mTitleMetaData.content_info[i].is_optional ? "YES" : "NO"));
			fmt::print("   {:1}  |- Size:        0x{:x}\n", (i+1 < mTitleMetaData.content_info.size() ? "|" : ""), mTitleMetaData.content_info[i].size);
			fmt::print("   {:1}  \\- Hash:        {}\n", (i+1 < mTitleMetaData.content_info.size() ? "|" : ""),
				tc::cli::FormatUtil::formatBytesAsString(mTitleMetaData.content_info[i].hash.data(), mTitleMetaData.content_info[i].hash.size(), true, ""));
		}
	}
	if (mCertChain.size() > 0)
	{
		fmt::print("Certificate Chain:\n");
		for (size_t i = 0; i < mCertChain.size(); i++)
		{
			#define _CERT_FORMAT_MACRO(x,y) (i+1 < mCertChain.size() ? (x) : (y))

			fmt::print("{:1}- Certificate {:d}:\n", _CERT_FORMAT_MACRO("|","\\"), i);
			fmt::print("{:1}  |- DigitalSignature: {:s}\n", _CERT_FORMAT_MACRO("|"," "), getValidString(mCertSigValid[i]));
			fmt::print("{:1}  |  |- SigType:    {:s} (0x{:x})\n", _CERT_FORMAT_MACRO("|"," "), getSigTypeString(mCertChain[i].signature.sig_type), (uint32_t)mCertChain[i].signature.sig_type);
			fmt::print("{:1}  |  |- Issuer:     {:s}\n", _CERT_FORMAT_MACRO("|"," "), mCertChain[i].signature.issuer);
			fmt::print("{:1}  |  \\- Signature:  {:s}\n", _CERT_FORMAT_MACRO("|"," "), getTruncatedBytesString(mCertChain[i].signature.sig.data(), mCertChain[i].signature.sig.size(), mVerbose));
			fmt::print("{:1}  |- Subject:       {:s}\n", _CERT_FORMAT_MACRO("|"," "), mCertChain[i].subject);
			//fmt::print("{:1}  |- Date:          {:d}\n", _CERT_FORMAT_MACRO("|"," "), mCertChain[i].date);
			fmt::print("{:1}  \\- PublicKey:     {:s} (0x{:x})\n", _CERT_FORMAT_MACRO("|"," "), getCertificatePublicKeyTypeString(mCertChain[i].public_key_type), (uint32_t)mCertChain[i].public_key_type);
			switch (mCertChain[i].public_key_type)
			{
				case brd::es::ESCertPubKeyType::RSA4096:
					fmt::print("{:1}     |- m:          {:s}\n", _CERT_FORMAT_MACRO("|"," "), getTruncatedBytesString(mCertChain[i].rsa4096_public_key.m.data(), mCertChain[i].rsa4096_public_key.m.size(), mVerbose));
					fmt::print("{:1}     \\- e:          {:s}\n", _CERT_FORMAT_MACRO("|"," "), getTruncatedBytesString(mCertChain[i].rsa4096_public_key.e.data(), mCertChain[i].rsa4096_public_key.e.size(), mVerbose));
					break;
				case brd::es::ESCertPubKeyType::RSA2048:
					fmt::print("{:1}     |- m:          {:s}\n", _CERT_FORMAT_MACRO("|"," "), getTruncatedBytesString(mCertChain[i].rsa2048_public_key.m.data(), mCertChain[i].rsa2048_public_key.m.size(), mVerbose));
					fmt::print("{:1}     \\- e:          {:s}\n", _CERT_FORMAT_MACRO("|"," "), getTruncatedBytesString(mCertChain[i].rsa2048_public_key.e.data(), mCertChain[i].rsa2048_public_key.e.size(), mVerbose));
					break;
				case brd::es::ESCertPubKeyType::ECC:
					fmt::print("{:1}     |- x:          {:s}\n", _CERT_FORMAT_MACRO("|"," "), getTruncatedBytesString(mCertChain[i].ecc233_public_key.x.data(), mCertChain[i].ecc233_public_key.x.size(), mVerbose));
					fmt::print("{:1}     \\- y:          {:s}\n", _CERT_FORMAT_MACRO("|"," "), getTruncatedBytesString(mCertChain[i].ecc233_public_key.y.data(), mCertChain[i].ecc233_public_key.y.size(), mVerbose));
					break;
				default:
					break;
			}

			#undef _CERT_FORMAT_MACRO
		}
	}
}

bool ctrtool::TmdProcess::isTwlTitle(uint64_t title_id)
{
	return ((title_id >> 47) & 1) == 1;
}

std::string ctrtool::TmdProcess::getValidString(byte_t validstate)
{
	std::string ret_str;

	switch (validstate)
	{
		case ValidState::Unchecked:
			ret_str =  "";
			break;
		case ValidState::Good:
			ret_str =  "(GOOD)";
			break;
		case ValidState::Fail:
		default:
			ret_str =  "(FAIL)";
			break;
	}

	return ret_str;
}

std::string ctrtool::TmdProcess::getTruncatedBytesString(const byte_t* data, size_t len, bool do_not_truncate)
{
	if (data == nullptr) { return fmt::format(""); }

	std::string str = "";

	if (len <= 8 || do_not_truncate)
	{
		str = tc::cli::FormatUtil::formatBytesAsString(data, len, true, "");
	}
	else
	{
		str = fmt::format("{:02X}{:02X}{:02X}{:02X}...{:02X}{:02X}{:02X}{:02X}", data[0], data[1], data[2], data[3], data[len-4], data[len-3], data[len-2], data[len-1]);
	}

	return str;
}

std::string ctrtool::TmdProcess::getSigTypeString(brd::es::ESSigType sig_type)
{
	std::string ret_str;

	switch (sig_type)
	{
		case brd::es::ESSigType::RSA4096_SHA1:
			ret_str =  "RSA-4096-SHA1";
			break;
		case brd::es::ESSigType::RSA2048_SHA1:
			ret_str =  "RSA-2048-SHA1";
			break;
		case brd::es::ESSigType::ECC_SHA1:
			ret_str =  "ECDSA-233-SHA1";
			break;
		case brd::es::ESSigType::RSA4096_SHA256:
			ret_str =  "RSA-4096-SHA256";
			break;
		case brd::es::ESSigType::RSA2048_SHA256:
			ret_str =  "RSA-2048-SHA256";
			break;
		case brd::es::ESSigType::ECC_SHA256:
			ret_str =  "ECDSA-233-SHA256";
			break;
		default:
			ret_str = fmt::format("0x{:x}", (uint32_t)sig_type);
	}

	return ret_str;
}

std::string ctrtool::TmdProcess::getCertificatePublicKeyTypeString(brd::es::ESCertPubKeyType public_key_type)
{
	std::string ret_str;

	switch (public_key_type)
	{
		case brd::es::ESCertPubKeyType::RSA4096:
			ret_str =  "RSA-4096";
			break;
		case brd::es::ESCertPubKeyType::RSA2048:
			ret_str =  "RSA-2048";
			break;
		case brd::es::ESCertPubKeyType::ECC:
			ret_str =  "ECC-233";
			break;
		default:
			ret_str = fmt::format("0x{:x}", (uint32_t)public_key_type);
	}

	return ret_str;
}

std::string ctrtool::TmdProcess::getTitleVersionString(uint16_t version)
{
	return fmt::format("{major:d}.{minor:d}.{build:d}", fmt::arg("major", (uint32_t)((version >> 10) & 0x3F)), fmt::arg("minor", (uint32_t)((version >> 4) & 0x3F)), fmt::arg("build", (uint32_t)(version & 0xF)));
}