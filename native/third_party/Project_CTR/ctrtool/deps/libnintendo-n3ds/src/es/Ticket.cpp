#include <ntd/n3ds/es/Ticket.h>
#include <fmt/core.h>

#include <brd/es/es_ticket.h>
#include <tc/ByteData.h>
#include <tc/crypto/Sha256Generator.h>

#include <tc/cli.h>

ntd::n3ds::es::TicketDeserialiser::TicketDeserialiser(const std::shared_ptr<tc::io::IStream>& tik_stream) :
	Ticket(),
	mModuleLabel("ntd::n3ds::es::TicketDeserialiser")
{
	if (tik_stream == nullptr)
	{
		throw tc::ArgumentNullException(mModuleLabel, "Stream was null.");
	}

	if (tik_stream->length() < sizeof(brd::es::ESV1Ticket))
	{
		throw tc::ArgumentOutOfRangeException(mModuleLabel, "Stream was too small to import ticket.");
	}

	// import TIK v1 data (less the section headers)
	tc::ByteData tik_data = tc::ByteData(sizeof(brd::es::ESV1Ticket));
	tik_stream->seek(0, tc::io::SeekOrigin::Begin);
	if (tik_stream->read(tik_data.data(), tik_data.size()) < tik_data.size())
	{
		throw tc::ArgumentOutOfRangeException(mModuleLabel, "TIK had unexpected size after reading.");
	}

	// get pointer
	brd::es::ESV1Ticket* tik = (brd::es::ESV1Ticket*)tik_data.data();
	if (tik->head.sig.sigType.unwrap() != brd::es::ESSigType::RSA2048_SHA256)
	{
		throw tc::ArgumentOutOfRangeException(mModuleLabel, "TIK had unexpected signature type.");
	}
	if (tik->head.version != 1)
	{
		throw tc::ArgumentOutOfRangeException(mModuleLabel, "TIK had unexpected format version.");
	}
	if (tik->v1Head.hdrVersion.unwrap() != 1)
	{
		throw tc::ArgumentOutOfRangeException(mModuleLabel, "TIK v1 header extension had unexpected format version.");
	}
	if (tik->v1Head.hdrSize.unwrap() != sizeof(brd::es::ESV1TicketHeader))
	{
		throw tc::ArgumentOutOfRangeException(mModuleLabel, "TIK v1 header extension had header size.");
	}
	if (tik->v1Head.sectHdrOfst.unwrap() != sizeof(brd::es::ESV1TicketHeader))
	{
		throw tc::ArgumentOutOfRangeException(mModuleLabel, "TIK v1 header extension had poorly aligned sectHdrOfst.");
	}
	if (tik->v1Head.sectHdrEntrySize.unwrap() != sizeof(brd::es::ESV1SectionHeader))
	{
		throw tc::ArgumentOutOfRangeException(mModuleLabel, "TIK v1 header extension had unexpected size for section headers.");
	}
	if (tik_stream->length() < static_cast<int64_t>(sizeof(brd::es::ESTicket) + tik->v1Head.ticketSize.unwrap()))
	{
		throw tc::ArgumentOutOfRangeException(mModuleLabel, "Stream was too small for calculated ticket size.");
	}

	// import full TIK v1 data
	tik_data = tc::ByteData(sizeof(brd::es::ESTicket) + tik->v1Head.ticketSize.unwrap());
	tik_stream->seek(0, tc::io::SeekOrigin::Begin);
	if (tik_stream->read(tik_data.data(), tik_data.size()) < tik_data.size())
	{
		throw tc::ArgumentOutOfRangeException(mModuleLabel, "TIK had unexpected size after reading.");
	}

	// update pointer
	tik = (brd::es::ESV1Ticket*)tik_data.data();

	/*
	fmt::print("ESTicket:\n");
	fmt::print("sigType:          {:x}\n", tik->head.sig.sigType.unwrap());
	fmt::print("issuer:           {}\n", tik->head.sig.issuer.str());
	fmt::print("version:          {:d}\n", tik->head.version);
	fmt::print("caCrlVersion:     {:d}\n", tik->head.caCrlVersion);
	fmt::print("signerCrlVersion: {:d}\n", tik->head.signerCrlVersion);
	fmt::print("titleKey:         {}\n", tc::cli::FormatUtil::formatBytesAsString(tik->head.titleKey.data(), tik->head.titleKey.size(), true, ""));
	fmt::print("ticketId:         {:016x}\n", tik->head.ticketId.unwrap());
	fmt::print("deviceId:         {:08x}\n", tik->head.deviceId.unwrap());
	fmt::print("titleId:          {:016x}\n", tik->head.titleId.unwrap());
	fmt::print("sysAccessMask:    {}\n", tc::cli::FormatUtil::formatBytesAsString(tik->head.sysAccessMask.data(), tik->head.sysAccessMask.size(), true, ""));
	fmt::print("ticketVersion:    {:d}\n", tik->head.ticketVersion.unwrap());
	fmt::print("accessTitleId:    {:08x}\n", tik->head.accessTitleId.unwrap());
	fmt::print("accessTitleMask:  {:08x}\n", tik->head.accessTitleMask.unwrap());
	fmt::print("licenseType:      {:02x}\n", tik->head.licenseType);
	fmt::print("keyId:            {:02x}\n", tik->head.keyId);
	fmt::print("propertyMask:     {:04x}\n", tik->head.propertyMask.unwrap());
	fmt::print("customData:       {}\n", tc::cli::FormatUtil::formatBytesAsString(tik->head.customData.data(), tik->head.customData.size(), true, ""));
	fmt::print("audit:            {:02x}\n", tik->head.audit);
	fmt::print("cidxMask:         {}\n", tc::cli::FormatUtil::formatBytesAsString(tik->head.cidxMask.data(), tik->head.cidxMask.size(), true, ""));
	for (size_t i = 0; i < tik->head.limits.size(); i++)
	{
		fmt::print("lp entry {}:       code={:08x}, limit={:08x}\n", i, tik->head.limits[i].code.unwrap(), tik->head.limits[i].limit.unwrap());
	}
	fmt::print("ESV1TicketHeader:\n");
	fmt::print("hdrVersion:       {:04x}\n", tik->v1Head.hdrVersion.unwrap());
	fmt::print("hdrSize:          {:04x}\n", tik->v1Head.hdrSize.unwrap());
	fmt::print("ticketSize:       {:08x}\n", tik->v1Head.ticketSize.unwrap());
	fmt::print("sectHdrOfst:      {:08x}\n", tik->v1Head.sectHdrOfst.unwrap());
	fmt::print("nSectHdrs:        {:04x}\n", tik->v1Head.nSectHdrs.unwrap());
	fmt::print("sectHdrEntrySize: {:04x}\n", tik->v1Head.sectHdrEntrySize.unwrap());
	fmt::print("flags:            {:08x}\n", tik->v1Head.flags.unwrap());
	size_t sect_hdr_num = tik->v1Head.nSectHdrs.unwrap();
	//brd::es::ESV1SectionHeader* sect_hdr = (brd::es::ESV1SectionHeader*)(tik_data.data() + sizeof(brd::es::ESTicket) + tik->v1Head.sectHdrOfst.unwrap());
	for (size_t i = 0; i < sect_hdr_num; i++)
	{
		fmt::print("sectHdr {:d}:\n", i);
		fmt::print(" sectOfst:        {:08x}\n", tik->sectHdrs[i].sectOfst.unwrap());
		fmt::print(" nRecords:        {:08x}\n", tik->sectHdrs[i].nRecords.unwrap());
		fmt::print(" recordSize:      {:08x}\n", tik->sectHdrs[i].recordSize.unwrap());
		fmt::print(" sectionSize:     {:08x}\n", tik->sectHdrs[i].sectionSize.unwrap());
		fmt::print(" sectionType:     {:04x}\n", tik->sectHdrs[i].sectionType.unwrap());
		fmt::print(" flags:           {:04x}\n", tik->sectHdrs[i].flags.unwrap());
	}
	*/
	

	struct TicketReservedForCtr
	{
		tc::bn::pad<0x14> reserved_00;
		tc::bn::le32<uint32_t> ec_account_id;
		tc::bn::pad<0x01> reserved_01;
	};

	// generate hash
	byte_t* tik_hash_begin = (byte_t*)&tik->head.sig.issuer;
	size_t tik_hash_size = tik_data.size() - size_t(tik_hash_begin - tik_data.data());
	tc::crypto::GenerateSha256Hash(this->calculated_hash.data(), tik_hash_begin, tik_hash_size);

	// basic fields
	this->signature.sig_type = tik->head.sig.sigType.unwrap();
	this->signature.sig = tc::ByteData(tik->head.sig.sig.data(), tik->head.sig.sig.size());
	this->signature.issuer = tik->head.sig.issuer.decode();
	this->title_key = tik->head.titleKey;
	this->ticket_id = tik->head.ticketId.unwrap();
	this->device_id = tik->head.deviceId.unwrap();
	this->title_id = tik->head.titleId.unwrap();
	this->ticket_version = tik->head.ticketVersion.unwrap();
	this->license_type = tik->head.licenseType;
	this->key_id = tik->head.keyId;

	// process data from reserved field
	auto custom_data = (TicketReservedForCtr*)tik->head.reserved.data();
	this->ec_account_id = custom_data->ec_account_id.unwrap();
	
	// find the demo launch limit
	for (size_t i = 0; i < tik->head.limits.size(); i++)
	{
		if (tik->head.limits[i].code.unwrap() == (uint32_t)brd::es::ESLimitCode::NUM_LAUNCH)
		{
			launch_count = tik->head.limits[i].limit.unwrap();
		}
	}

	// process content index
	const uint32_t kMax3DSTicketContentIndexOffset = (uint32_t)0xFFFF & ~(uint32_t)0x3FF;
	for (size_t i = 0; i < tik->v1Head.nSectHdrs.unwrap(); i++)
	{
		if (tik->sectHdrs[i].sectionType.unwrap() == (uint32_t)brd::es::ESItemType::CONTENT)
		{
			if (tik->sectHdrs[i].recordSize.unwrap() != sizeof(brd::es::ESV1ContentRecord))
			{
				throw tc::InvalidOperationException(mModuleLabel, fmt::format("Invalid size for ESV1ContentRecord. (expected: 0x{:x}, got 0x{:x})", sizeof(brd::es::ESV1ContentRecord), tik->sectHdrs[i].recordSize.unwrap()));
			}
			brd::es::ESV1ContentRecord* content_records = (brd::es::ESV1ContentRecord*)(tik_data.data() + sizeof(brd::es::ESTicket) + tik->sectHdrs[i].sectOfst.unwrap());
			for (size_t j = 0; j < tik->sectHdrs[i].nRecords.unwrap(); j++)
			{
				// maximum supported value for 3DS (since content indexes cannot exceed 0xFFFF)
				if (content_records[j].offset.unwrap() > kMax3DSTicketContentIndexOffset)
				{
					fmt::print(stderr, "[{} LOG] Skipping ESV1ContentRecord[{:d}] because offset 0x{:x} was not in legal range (0x{:x} - 0x{:x})\n", mModuleLabel, j, content_records[j].offset.unwrap(), 0x0, kMax3DSTicketContentIndexOffset);
					continue;
				}

				tc::bn::bitarray<0x80>* access_mask = (tc::bn::bitarray<0x80>*)content_records[j].accessMask.data();
				for (size_t bit = 0; bit < access_mask->bit_size(); bit++)
				{
					if (access_mask->test(bit)) 
					{
						this->enabled_content.set(content_records[j].offset.unwrap() + bit);
					}
				}
			}
		}
	}
}