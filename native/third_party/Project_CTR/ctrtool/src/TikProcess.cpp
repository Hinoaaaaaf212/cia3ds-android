#include "TikProcess.h"
#include <tc/io.h>
#include <tc/cli.h>
#include <tc/crypto.h>
#include <tc/ArgumentNullException.h>
#include <tc/NotSupportedException.h>

ctrtool::TikProcess::TikProcess() :
	mModuleLabel("ctrtool::TikProcess"),
	mInputStream(),
	mKeyBag(),
	mShowInfo(false),
	mVerbose(false),
	mVerify(false),
	mIssuerSigner(),
	mCertImportedIssuerSigner(),
	mCertChain(),
	mCertSigValid(),
	mTicket(),
	mTicketSigValid(ValidState::Unchecked),
	mDecryptedTitleKey()
{
}

void ctrtool::TikProcess::setInputStream(const std::shared_ptr<tc::io::IStream>& input_stream)
{
	mInputStream = input_stream;
}

void ctrtool::TikProcess::setKeyBag(const ctrtool::KeyBag& key_bag)
{
	mKeyBag = key_bag;
}

void ctrtool::TikProcess::setCliOutputMode(bool show_info)
{
	mShowInfo = show_info;
}

void ctrtool::TikProcess::setVerboseMode(bool verbose)
{
	mVerbose = verbose;
}

void ctrtool::TikProcess::setVerifyMode(bool verify)
{
	mVerify = verify;
}

void ctrtool::TikProcess::process()
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

void ctrtool::TikProcess::importIssuerProfiles()
{
	// import issuer profiles from keybag
	for (auto itr = mKeyBag.broadon_rsa_signer.begin(); itr != mKeyBag.broadon_rsa_signer.end(); itr++)
	{
		brd::es::ESSigType sigType = itr->first == "Root" ? brd::es::ESSigType::RSA4096_SHA256 : brd::es::ESSigType::RSA2048_SHA256;
		mIssuerSigner.insert(std::pair<std::string, std::shared_ptr<ntd::n3ds::es::ISigner>>(itr->first, std::make_shared<ntd::n3ds::es::RsaSigner>(ntd::n3ds::es::RsaSigner(sigType, itr->first, itr->second.key))));
	}
}

void ctrtool::TikProcess::importData()
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

	// process ticket
	{
		mTicket = ntd::n3ds::es::TicketDeserialiser(mInputStream);
		mTicketSigValid = ValidState::Unchecked;

		// determine title key
		if (mKeyBag.common_key.find(mTicket.key_id) != mKeyBag.common_key.end())
		{
			if (mVerbose)
			{
				fmt::print(stderr, "[{} LOG] Decrypting titlekey from ticket.\n", mModuleLabel);
			}
			
			// get common key
			auto common_key = mKeyBag.common_key[mTicket.key_id];
			
			// initialise iv
			std::array<byte_t, 16> title_key_iv;
			memset(title_key_iv.data(), 0, title_key_iv.size());
			((tc::bn::be64<uint64_t>*)(&(title_key_iv[0])))->wrap(mTicket.title_id);

			// decrypt title key
			std::array<byte_t, 16> title_key;
			tc::crypto::DecryptAes128Cbc(title_key.data(), mTicket.title_key.data(), title_key.size(), common_key.data(), common_key.size(), title_key_iv.data(), title_key_iv.size());
		
			mDecryptedTitleKey = title_key;
		}
		else
		{
			fmt::print(stderr, "[{} LOG] Cannot determine titlekey.\n", mModuleLabel);
		}
	}

	// process trailing cert chain (this assumes ntd::n3ds::es::TicketDeserialiser leaves the stream position at the end of the ticket data)
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

void ctrtool::TikProcess::verifyData()
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
				fmt::print(stderr, "[{} LOG] Public key \"{}\" (for certificate \"{}\") was not present in the certificate chain. The public key included with CTRTool was used instead.\n", mModuleLabel, mCertChain[i].signature.issuer, mCertChain[i].subject);
			}
			mCertSigValid[i] = keybag_issuer_itr->second->verifyHash(mCertChain[i].calculated_hash.data(), mCertChain[i].signature.sig.data()) ? ValidState::Good : ValidState::Fail;
		}
		else
		{
			// cannot locate rsa key to verify
			fmt::print(stderr, "[{} LOG] Could not locate public key for \"{}\" (certificate).\n", mModuleLabel, mCertChain[i].signature.issuer);
			mCertSigValid[i] = ValidState::Fail;
		}

		// log certificate signature validation error
		if (mCertSigValid[i] != ValidState::Good)
		{
			fmt::print(stderr, "[{} LOG] Signature for Certificate \"{}\" was invalid.\n", mModuleLabel, mCertChain[i].signature.issuer);
		}
	}

	// verify ticket
	{
		// verify ticket
		auto local_issuer_itr = mCertImportedIssuerSigner.find(mTicket.signature.issuer);
		auto keybag_issuer_itr = mIssuerSigner.find(mTicket.signature.issuer);

		// first try with the issuer profiles imported from the local certificates
		if (local_issuer_itr != mCertImportedIssuerSigner.end() && local_issuer_itr->second->getSigType() == mTicket.signature.sig_type)
		{
			mTicketSigValid = local_issuer_itr->second->verifyHash(mTicket.calculated_hash.data(), mTicket.signature.sig.data()) ? ValidState::Good : ValidState::Fail;
		}
		// fallback try with the keybag imported issuer
		else if (keybag_issuer_itr != mIssuerSigner.end() && keybag_issuer_itr->second->getSigType() == mTicket.signature.sig_type)
		{
			// only show this warning when there are certificates appended to the ticket (only tickets downloaded from CDN will have an appended certificate chain)
			if (mCertChain.size() != 0)
			{
				fmt::print(stderr, "[{} ERROR] Public key \"{}\" (for ticket) was not present in the appended certificate chain. The public key included with CTRTool was used instead.\n", mModuleLabel, mTicket.signature.issuer);
			}
			mTicketSigValid = keybag_issuer_itr->second->verifyHash(mTicket.calculated_hash.data(), mTicket.signature.sig.data()) ? ValidState::Good : ValidState::Fail;
		}
		else
		{
			// cannot locate rsa key to verify
			fmt::print(stderr, "[{} ERROR] Could not locate public key \"{}\" (for ticket).\n", mModuleLabel, mTicket.signature.issuer);
			mTicketSigValid = ValidState::Fail;
		}

		// log ticket signature validation error
		if (mTicketSigValid != ValidState::Good)
		{
			fmt::print(stderr, "[{} ERROR] Signature for Ticket was invalid.\n", mModuleLabel);
		}
	}
}

void ctrtool::TikProcess::printData()
{
	{
		fmt::print("Ticket:\n");
		fmt::print("|- DigitalSignature: {:s} \n", getValidString(mTicketSigValid));
		fmt::print("|  |- SigType:    {:s} (0x{:x})\n", getSigTypeString(mTicket.signature.sig_type), (uint32_t)mTicket.signature.sig_type);
		fmt::print("|  |- Issuer:     {:s}\n", mTicket.signature.issuer);
		fmt::print("|  \\- Signature:  {:s}\n", getTruncatedBytesString(mTicket.signature.sig.data(), mTicket.signature.sig.size(), mVerbose));
		fmt::print("|- TitleKey:      {}", tc::cli::FormatUtil::formatBytesAsString(mTicket.title_key.data(), mTicket.title_key.size(), true, ""));
		if (mDecryptedTitleKey.isSet())
		{
			fmt::print(" (decrypted: {})", tc::cli::FormatUtil::formatBytesAsString(mDecryptedTitleKey.get().data(), mDecryptedTitleKey.get().size(), true, ""));
		}
		fmt::print("\n");
		fmt::print("|- TicketId:      {:016x}\n", mTicket.ticket_id);
		fmt::print("|- DeviceId:      {:08x}\n", mTicket.device_id);
		fmt::print("|- TitleId:       {:016x}\n", mTicket.title_id);
		fmt::print("|- TicketVersion: {} ({:d})\n", getTitleVersionString(mTicket.ticket_version), mTicket.ticket_version);
		fmt::print("|- LicenseType:   {:02x}\n", mTicket.license_type);
		fmt::print("|- KeyId:         {:02x}\n", mTicket.key_id);
		fmt::print("|- ECAccountID:   {:08x}\n", mTicket.ec_account_id);
		fmt::print("|- DemoLaunchCnt: {:d}\n", mTicket.launch_count);
		fmt::print("\\- EnabledContent:\n");
		std::vector<size_t> enabled_content;
		for (size_t i = 0; i < mTicket.enabled_content.size(); i++)
		{
			if (mTicket.enabled_content.test(i))
			{
				enabled_content.push_back(i);
			}
		}
		for (size_t i = 0; i < enabled_content.size(); i++)
		{
			fmt::print("   {:1}- 0x{:04x}\n", (i+1 < enabled_content.size() ? "|" : "\\"), enabled_content[i]);
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

std::string ctrtool::TikProcess::getValidString(byte_t validstate)
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

std::string ctrtool::TikProcess::getTruncatedBytesString(const byte_t* data, size_t len, bool do_not_truncate)
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

std::string ctrtool::TikProcess::getSigTypeString(brd::es::ESSigType sig_type)
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

std::string ctrtool::TikProcess::getCertificatePublicKeyTypeString(brd::es::ESCertPubKeyType public_key_type)
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

std::string ctrtool::TikProcess::getTitleVersionString(uint16_t version)
{
	return fmt::format("{major:d}.{minor:d}.{build:d}", fmt::arg("major", (uint32_t)((version >> 10) & 0x3F)), fmt::arg("minor", (uint32_t)((version >> 4) & 0x3F)), fmt::arg("build", (uint32_t)(version & 0xF)));
}