#include "FirmProcess.h"
#include <tc/io.h>
#include <tc/cli.h>
#include <tc/crypto.h>
#include <tc/ArgumentNullException.h>

#include <tc/crypto/Aes128CbcEncryptedStream.h>

ctrtool::FirmProcess::FirmProcess() :
	mModuleLabel("ctrtool::FirmProcess"),
	mInputStream(),
	mKeyBag(),
	mShowInfo(false),
	mVerbose(false),
	mVerify(false),
	mExtractPath(),
	mFirmwareType(FirmwareType_Nand),
	mSignatureState(SignatureState_Unchecked)
{
	memset((byte_t*)&mHeader, 0, sizeof(mHeader));
	memset(mValidFirmSectionHash.data(), ValidState::Unchecked, mValidFirmSectionHash.size());
}

void ctrtool::FirmProcess::setInputStream(const std::shared_ptr<tc::io::IStream>& input_stream)
{
	mInputStream = input_stream;
}

void ctrtool::FirmProcess::setKeyBag(const ctrtool::KeyBag& key_bag)
{
	mKeyBag = key_bag;
}

void ctrtool::FirmProcess::setCliOutputMode(bool show_info)
{
	mShowInfo = show_info;
}

void ctrtool::FirmProcess::setVerboseMode(bool verbose)
{
	mVerbose = verbose;
}

void ctrtool::FirmProcess::setVerifyMode(bool verify)
{
	mVerify = verify;
}

void ctrtool::FirmProcess::setExtractPath(const tc::io::Path& extract_path)
{
	mExtractPath = extract_path;
}

void ctrtool::FirmProcess::setFirmwareType(FirmwareType type)
{
	mFirmwareType = type;
}

void ctrtool::FirmProcess::process()
{
	// begin processing
	importHeader();
	generateSectionStreams();

	if (mVerify)
	{
		verifyHashes();
		verifySignature();
	}
		
	if (mShowInfo)
		printData();

	if (mExtractPath.isSet())
		extractSections();
}

void ctrtool::FirmProcess::importHeader()
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
	if (mInputStream->length() < (sizeof(ntd::n3ds::FirmwareHeader)))
	{
		throw tc::InvalidOperationException(mModuleLabel, "Input stream too small to import header.");
	}
	mInputStream->seek(0, tc::io::SeekOrigin::Begin);
	mInputStream->read((byte_t*)&mHeader, sizeof(mHeader));

	if (mHeader.struct_magic.unwrap() != mHeader.kStructMagic)
	{
		throw tc::InvalidOperationException(mModuleLabel, "Invalid struct magic.");
	}
}

void ctrtool::FirmProcess::generateSectionStreams()
{
	tc::crypto::Aes128CbcEncryptedStream::key_t aes_key;
	tc::crypto::Aes128CbcEncryptedStream::iv_t aes_iv;

	// generate AES key
	memset(aes_key.data(), 0, aes_key.size());
	memset(aes_iv.data(), 0, aes_iv.size());
	if (mFirmwareType == FirmwareType_Ngc)
	{
		auto key_itr = mKeyBag.firmware_key.find(mKeyBag.FIRM_NGC_KEY);
		if (key_itr != mKeyBag.firmware_key.end())
		{
			memcpy(aes_key.data(), key_itr->second.data(), 16);
		}
	}
	else if (mFirmwareType == FirmwareType_Nor)
	{
		auto key_itr = mKeyBag.firmware_key.find(mKeyBag.FIRM_NOR_KEY);
		if (key_itr != mKeyBag.firmware_key.end())
		{
			memcpy(aes_key.data(), key_itr->second.data(), 16);
		}
	}
	else if (mFirmwareType == FirmwareType_Sdmc)
	{
		auto key_itr = mKeyBag.firmware_key.find(mKeyBag.FIRM_SD_KEY);
		if (key_itr != mKeyBag.firmware_key.end())
		{
			memcpy(aes_key.data(), key_itr->second.data(), 16);
		}
	}

	for (size_t i = 0; i < mHeader.section.size(); i++)
	{
		if (mHeader.section[i].size.unwrap() > 0)
		{
			std::shared_ptr<tc::io::IStream> raw_stream = std::make_shared<tc::io::SubStream>(tc::io::SubStream(mInputStream, mHeader.section[i].offset.unwrap(), mHeader.section[i].size.unwrap()));

			switch (mFirmwareType)
			{
				case FirmwareType_Nand:
					mSectionStreams[i] = raw_stream;
					break;
				case FirmwareType_Ngc:
				case FirmwareType_Nor:
				case FirmwareType_Sdmc:
					createSectionAesIv(aes_iv, mHeader.section[i]);
					mSectionStreams[i] = std::make_shared<tc::crypto::Aes128CbcEncryptedStream>(tc::crypto::Aes128CbcEncryptedStream(raw_stream, aes_key, aes_iv));
					break;
			}
		}
		else
		{
			mSectionStreams[i] = nullptr;
		}
	}
}

void ctrtool::FirmProcess::verifyHashes()
{
	tc::crypto::Sha256Generator hash_calc;
	std::array<byte_t, hash_calc.kHashSize> hash;
	tc::ByteData cache = tc::ByteData(0x10000);

	// get encryption key

	for (size_t i = 0; i < mHeader.section.size(); i++)
	{
		if (mHeader.section[i].size.unwrap() > 0 && mSectionStreams[i] != nullptr)
		{
			auto& hdr_hash = mHeader.section[i].hash;

			mSectionStreams[i]->seek(0, tc::io::SeekOrigin::Begin);
			hash_calc.initialize();
			for (size_t j = mSectionStreams[i]->length(); j > 0;)
			{
				size_t read_len = std::min<size_t>(j, cache.size());
				read_len = mSectionStreams[i]->read(cache.data(), read_len);

				hash_calc.update(cache.data(), read_len);

				j -= read_len;
			}
			hash_calc.getHash(hash.data());

			mValidFirmSectionHash[i] = memcmp(hash.data(), hdr_hash.data(), hash.size()) == 0? ValidState::Good : ValidState::Fail;

			if (mValidFirmSectionHash[i] != ValidState::Good)
			{
				fmt::print(stderr, "[{} ERROR] FIRM section {:d} SHA2-256 hash was invalid.\n", mModuleLabel, i);
			}
		}
	}
}

void ctrtool::FirmProcess::verifySignature()
{
	byte_t key_id = 0;

	switch (mFirmwareType)
	{
		case FirmwareType_Nand:
		case FirmwareType_Sdmc:
			key_id = mKeyBag.RSAKEY_FIRM_NAND;
			break;
		case FirmwareType_Ngc:
		case FirmwareType_Nor:
			key_id = mKeyBag.RSAKEY_FIRM_RECOVERY;
	}

	byte_t valid_signature = ValidState::Unchecked;
	bool is_sighax = false;

	// validate header signature
	if (mKeyBag.rsa_key.find(key_id) != mKeyBag.rsa_key.end())
	{
		std::array<byte_t, tc::crypto::Sha256Generator::kHashSize> hash;
		tc::crypto::RsaKey pubkey = mKeyBag.rsa_key[key_id];

		// generate hash
		size_t offset = 0;
		size_t size = sizeof(mHeader) - sizeof(mHeader.signature);
		tc::crypto::GenerateSha256Hash(hash.data(), ((byte_t*)&mHeader) + offset, size);

		// validate signature
		valid_signature = tc::crypto::VerifyRsa2048Pkcs1Sha256(mHeader.signature.data(), hash.data(), pubkey) ? ValidState::Good : ValidState::Fail;
	}
	else
	{
		fmt::print(stderr, "[{} ERROR] Could not load {} RSA2048 public key.\n", mModuleLabel, key_id == mKeyBag.RSAKEY_FIRM_NAND ? "FIRM_NAND" : "FIRM_RECOVERY");
		valid_signature = ValidState::Fail;
	}

	// check if signature is sighax
	if (mKeyBag.rsa_sighax_signature.find(key_id) != mKeyBag.rsa_sighax_signature.end())
	{
		auto signature = mKeyBag.rsa_sighax_signature[key_id];

		is_sighax = memcmp(signature.data(), mHeader.signature.data(), mHeader.signature.size()) == 0;
	}
	else
	{
		fmt::print(stderr, "[{} ERROR] Could not load {} SigHax RSA2048 signature.\n", mModuleLabel, key_id == mKeyBag.RSAKEY_FIRM_NAND ? "FIRM_NAND" : "FIRM_RECOVERY");
		is_sighax = false;
	}

	// test if signature was valid
	if (valid_signature == ValidState::Good)
	{
		mSignatureState = SignatureState_Good;
	}
	// check if sighax
	else if (valid_signature == ValidState::Fail && is_sighax == true)
	{
		fmt::print(stderr, "[{} ERROR] Signature for FIRM was invalid (SigHax).\n", mModuleLabel);
		mSignatureState = SignatureState_SigHax;
	}
	else
	{
		fmt::print(stderr, "[{} ERROR] Signature for FIRM was invalid.\n", mModuleLabel);
		mSignatureState = SignatureState_Fail;
	}
}

void ctrtool::FirmProcess::printData()
{
	fmt::print("\n");
	fmt::print("FIRM:\n");
	fmt::print("Signature: {:8}     {}\n", getSignatureStateString(mSignatureState) , tc::cli::FormatUtil::formatBytesAsStringWithLineLimit(mHeader.signature.data(), mHeader.signature.size(), true, "", 0x20, 24, false));
	fmt::print("Magic:                  {}\n", "FIRM");
	fmt::print("Priority:               {:d}\n", mHeader.priority.unwrap());
	fmt::print("Entrypoint ARM11:       0x{:08x}\n", mHeader.entrypoint_arm11.unwrap());
	fmt::print("Entrypoint ARM9:        0x{:08x}\n", mHeader.entrypoint_arm9.unwrap());
	fmt::print("\n");
	for (size_t i = 0; i < mHeader.section.size(); i++)
	{
		if (mHeader.section[i].size.unwrap() == 0) continue;
		
		fmt::print("Section {:d}\n", i);
		fmt::print(" Offset:                0x{:08x}\n", mHeader.section[i].offset.unwrap());
		fmt::print(" Address:               0x{:08x}\n", mHeader.section[i].address.unwrap());
		fmt::print(" Size:                  0x{:08x}\n", mHeader.section[i].size.unwrap());
		fmt::print(" Copy Method:           {} (0x{:08x})\n", getCopyMethodString(mHeader.section[i].copy_method.unwrap()), mHeader.section[i].copy_method.unwrap());
		fmt::print(" Hash: {:6}           {}\n", getValidString(mValidFirmSectionHash[i]), tc::cli::FormatUtil::formatBytesAsString(mHeader.section[i].hash.data(), mHeader.section[i].hash.size(), true, ""));
	}
}

void ctrtool::FirmProcess::extractSections()
{
	tc::io::LocalFileSystem local_fs;
	std::shared_ptr<tc::io::IStream> in_stream;
	std::shared_ptr<tc::io::IStream> out_stream;

	for (size_t i = 0; i < mHeader.section.size(); i++)
	{
		if (mHeader.section[i].size.unwrap() > 0 && mSectionStreams[i] != nullptr)
		{
			in_stream = mSectionStreams[i];

			// create output file name
			std::string f_name = fmt::format("firm_{:d}_{:08x}.bin", i, mHeader.section[i].address.unwrap());
			
			// create output file path
			tc::io::Path f_path = mExtractPath.get() + f_name;

			// open out stream
			local_fs.createDirectory(mExtractPath.get());
			local_fs.openFile(f_path, tc::io::FileMode::OpenOrCreate, tc::io::FileAccess::Write, out_stream);

			if (mVerbose)
			{
				fmt::print(stderr, "[{} LOG] Saving section {} to {}...\n", mModuleLabel, i, f_path.to_string());
			}

			tc::ByteData filedata = tc::ByteData(in_stream->length());
			in_stream->seek(0, tc::io::SeekOrigin::Begin);
			in_stream->read(filedata.data(), filedata.size());

			out_stream->seek(0, tc::io::SeekOrigin::Begin);
			out_stream->write(filedata.data(), filedata.size());
		}
	}
}

void ctrtool::FirmProcess::createSectionAesIv(std::array<byte_t, 16>& iv, const ntd::n3ds::FirmwareHeader::SectionHeader& section)
{
	tc::bn::le32<uint32_t>* aes_iv_words = (tc::bn::le32<uint32_t>*)(iv.data());
	aes_iv_words[0].wrap(section.offset.unwrap());
	aes_iv_words[1].wrap(section.address.unwrap());
	aes_iv_words[2].wrap(section.size.unwrap());
	aes_iv_words[3].wrap(section.size.unwrap());
}

std::string ctrtool::FirmProcess::getValidString(byte_t validstate)
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

std::string ctrtool::FirmProcess::getSignatureStateString(byte_t signature_state)
{
	std::string ret_str;

	switch(signature_state)
	{
		case SignatureState_Unchecked: 
			ret_str = "";
			break;
		case SignatureState_Good: 
			ret_str = "(GOOD)";
			break;
		case SignatureState_Fail: 
			ret_str = "(FAIL)";
			break;
		case SignatureState_SigHax: 
			ret_str = "(SIGHAX)";
			break;
	}

	return ret_str;
}

std::string ctrtool::FirmProcess::getCopyMethodString(uint32_t method)
{
	std::string ret_str;

	switch(method)
	{
		case ntd::n3ds::FirmwareHeader::SectionHeader::CopyMethod_NDMA :
			ret_str = "NDMA";
			break;
		case ntd::n3ds::FirmwareHeader::SectionHeader::CopyMethod_XDMA :
			ret_str = "XDMA";
			break;
		case ntd::n3ds::FirmwareHeader::SectionHeader::CopyMethod_MEMCPY :
			ret_str = "MEMCPY";
			break;
		default:
			ret_str = "Unknown";
			break;
	}

	return ret_str;
}