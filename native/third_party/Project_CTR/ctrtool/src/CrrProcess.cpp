#include "CrrProcess.h"
#include <tc/io.h>
#include <tc/cli.h>
#include <tc/crypto.h>
#include <tc/ArgumentNullException.h>

ctrtool::CrrProcess::CrrProcess() :
	mModuleLabel("ctrtool::CrrProcess"),
	mInputStream(),
	mKeyBag(),
	mShowInfo(false),
	mVerbose(false),
	mVerify(false),
	mCrrData(false),
	mValidCertificateSignature(ValidState::Unchecked),
	mValidBodySignature(ValidState::Unchecked),
	mValidUniqueId(ValidState::Unchecked)
{
	memset((byte_t*)&mHeader, 0, sizeof(mHeader));
	memset((byte_t*)&mBodyHeader, 0, sizeof(mBodyHeader));
}

void ctrtool::CrrProcess::setInputStream(const std::shared_ptr<tc::io::IStream>& input_stream)
{
	mInputStream = input_stream;
}

void ctrtool::CrrProcess::setKeyBag(const ctrtool::KeyBag& key_bag)
{
	mKeyBag = key_bag;
}

void ctrtool::CrrProcess::setCliOutputMode(bool show_info)
{
	mShowInfo = show_info;
}

void ctrtool::CrrProcess::setVerboseMode(bool verbose)
{
	mVerbose = verbose;
}

void ctrtool::CrrProcess::setVerifyMode(bool verify)
{
	mVerify = verify;
}


void ctrtool::CrrProcess::process()
{
	// begin processing
	importData();
	if (mVerify)
		verifyData();
	if (mShowInfo)
		printData();
}

void ctrtool::CrrProcess::importData()
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

	// import header
	if (mInputStream->length() < (sizeof(ntd::n3ds::CrrHeader) + sizeof(ntd::n3ds::CrrBodyHeader)))
	{
		throw tc::InvalidOperationException(mModuleLabel, "Input stream too small to import header.");
	}
	mInputStream->seek(0, tc::io::SeekOrigin::Begin);
	mInputStream->read((byte_t*)&mHeader, sizeof(mHeader));
	mInputStream->read((byte_t*)&mBodyHeader, sizeof(mBodyHeader));

	if (mHeader.struct_magic.unwrap() != mHeader.kStructMagic)
	{
		throw tc::InvalidOperationException(mModuleLabel, "Invalid struct magic.");
	}

	if (mBodyHeader.size.unwrap() % 0x1000 != 0)
	{
		throw tc::InvalidOperationException(mModuleLabel, "CRR file size was not aligned to 0x1000 bytes.");
	}

	if (((mBodyHeader.num_hash.unwrap() * 0x20) + mBodyHeader.hash_offset.unwrap()) > mBodyHeader.size.unwrap())
	{
		throw tc::InvalidOperationException(mModuleLabel, "CRR invalid hash geometry.");
	}
	if ((mBodyHeader.module_id_offset.unwrap() + mBodyHeader.module_id_size.unwrap()) > mBodyHeader.size.unwrap())
	{
		throw tc::InvalidOperationException(mModuleLabel, "CRR invalid module_id geometry.");
	}

	if (mInputStream->length() < int64_t(mBodyHeader.size.unwrap()))
	{
		throw tc::InvalidOperationException(mModuleLabel, "Input stream too small for logical file size.");
	}

	mCrrData = tc::ByteData(mBodyHeader.size.unwrap());
	mInputStream->seek(0, tc::io::SeekOrigin::Begin);
	mInputStream->read(mCrrData.data(), mCrrData.size());
}

void ctrtool::CrrProcess::verifyData()
{
	// validate certificate signature
	if (mKeyBag.rsa_key.find(mKeyBag.RSAKEY_CRR) != mKeyBag.rsa_key.end())
	{
		std::array<byte_t, tc::crypto::Sha256Generator::kHashSize> hash;
		tc::crypto::RsaKey pubkey = mKeyBag.rsa_key[mKeyBag.RSAKEY_CRR];

		// generate hash
		size_t offset = sizeof(mHeader) - sizeof(mHeader.body_certificate);
		size_t size = sizeof(mHeader.body_certificate) - sizeof(mHeader.body_certificate.signature);
		tc::crypto::GenerateSha256Hash(hash.data(), mCrrData.data() + offset, size);

		// validate signature
		mValidCertificateSignature = tc::crypto::VerifyRsa2048Pkcs1Sha256(mHeader.body_certificate.signature.data(), hash.data(), pubkey) ? ValidState::Good : ValidState::Fail;
	}
	else
	{
		fmt::print(stderr, "[{} ERROR] Could not read static CRR public key.\n", mModuleLabel);
		mValidCertificateSignature = ValidState::Fail;
	}

	// validate body signature
	{
		std::array<byte_t, tc::crypto::Sha256Generator::kHashSize> hash;
		tc::crypto::RsaKey pubkey = tc::crypto::RsaPublicKey(mHeader.body_certificate.crr_body_public_key.data(), mHeader.body_certificate.crr_body_public_key.size());

		// generate hash
		size_t offset = sizeof(mHeader) + sizeof(mBodyHeader.signature);
		size_t size = (mBodyHeader.hash_offset.unwrap() + (mBodyHeader.num_hash.unwrap() * 0x20)) - offset;
		tc::crypto::GenerateSha256Hash(hash.data(), mCrrData.data() + offset, size);

		mValidBodySignature = tc::crypto::VerifyRsa2048Pkcs1Sha256(mBodyHeader.signature.data(), hash.data(), pubkey) ? ValidState::Good : ValidState::Fail;
	}

	// validate unique id
	{
		mValidUniqueId = ((mBodyHeader.unique_id.unwrap() & mHeader.body_certificate.unique_id_mask.unwrap()) == 0) ? ValidState::Good : ValidState::Fail;
	}

	// log validation errors
	if (mValidCertificateSignature != ValidState::Good)
	{
		fmt::print(stderr, "[{} ERROR] Signature for CRR Certificate was invalid.\n", mModuleLabel);
	}
	if (mValidBodySignature != ValidState::Good)
	{
		fmt::print(stderr, "[{} ERROR] Signature for CRR Body was invalid.\n", mModuleLabel);
	}
	if (mValidUniqueId != ValidState::Good)
	{
		fmt::print(stderr, "[{} ERROR] CRR UniqueId was invalid.\n", mModuleLabel);
	}
}

void ctrtool::CrrProcess::printData()
{
	fmt::print("\n");
	fmt::print("CRR:\n");
	fmt::print("Magic:                  {}\n", "CRR0");
	fmt::print("DebugInfo Offset:       0x{:08x}\n", mHeader.debug_info_offset.unwrap());
	fmt::print("DebugInfo Size:         0x{:08x}\n", mHeader.debug_info_size.unwrap());
	fmt::print("\n");
	fmt::print("CRR certificate:\n");
	fmt::print("UniqueIdMask:           0x{:08x}\n", mHeader.body_certificate.unique_id_mask.unwrap());
	fmt::print("UniqueIdPattern:        0x{:08x}\n", mHeader.body_certificate.unique_id_pattern.unwrap());
	fmt::print("PublicKey:              {}", tc::cli::FormatUtil::formatBytesAsStringWithLineLimit(mHeader.body_certificate.crr_body_public_key.data(), mHeader.body_certificate.crr_body_public_key.size(), true, "", 0x20, 24, false));
	fmt::print("Signature: {:6}       {}", getValidString(mValidCertificateSignature), tc::cli::FormatUtil::formatBytesAsStringWithLineLimit(mHeader.body_certificate.signature.data(), mHeader.body_certificate.signature.size(), true, "", 0x20, 24, false));
	fmt::print("\n");
	fmt::print("CRR body header:\n");
	fmt::print("Signature: {:6}       {}", getValidString(mValidBodySignature), tc::cli::FormatUtil::formatBytesAsStringWithLineLimit(mBodyHeader.signature.data(), mBodyHeader.signature.size(), true, "", 0x20, 24, false));
	fmt::print("UniqueId: {:6}        0x{:08x}\n", getValidString(mValidUniqueId), mBodyHeader.unique_id.unwrap());
	fmt::print("CRR Size:               0x{:08x}\n", mBodyHeader.size.unwrap());
	fmt::print("Hash Offset:            0x{:08x}\n", mBodyHeader.hash_offset.unwrap());
	fmt::print("Hash Num:               {:d}\n", mBodyHeader.num_hash.unwrap());
	fmt::print("ModuleId Offset:        0x{:08x}\n", mBodyHeader.module_id_offset.unwrap());
	fmt::print("ModuleId Size:          0x{:08x}\n", mBodyHeader.module_id_size.unwrap());
}

std::string ctrtool::CrrProcess::getValidString(byte_t validstate)
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