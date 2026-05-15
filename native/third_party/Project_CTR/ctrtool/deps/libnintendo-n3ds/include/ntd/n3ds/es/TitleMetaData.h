#pragma once
#include <tc/types.h>
#include <tc/io/IStream.h>
#include <ntd/n3ds/es/Signature.h>

#include <tc/ArgumentNullException.h>
#include <tc/ArgumentOutOfRangeException.h>
#include <tc/InvalidOperationException.h>

namespace ntd { namespace n3ds { namespace es {

struct TitleMetaData
{
public:
	TitleMetaData() :
		signature(),
		title_id(0),
		title_version(0),
		content_info()
	{
		memset(calculated_hash.data(), 0, calculated_hash.size());
		twl_custom_data.public_save_data_size = 0;
		twl_custom_data.private_save_data_size = 0;
		twl_custom_data.flag = 0;
		ctr_custom_data.save_data_size = 0;
		ctr_custom_data.is_snake_only = false;
	}

	struct ContentInfo
	{
	public:
		ContentInfo() :
			id(0),
			index(0),
			is_encrypted(0),
			is_optional(0),
			size(0)
		{
			memset(hash.data(), 0, hash.size());
		}

		ContentInfo(uint32_t id, uint16_t index, bool is_encrypted, bool is_optional, int64_t size, std::array<byte_t, 32>& hash) :
			id(id),
			index(index),
			is_encrypted(is_encrypted),
			is_optional(is_optional),
			size(size),
			hash(hash)
		{
		}
	public:
		uint32_t id;
		uint16_t index;
		bool is_encrypted;
		bool is_optional;
		int64_t size;
		std::array<byte_t, 32> hash;
	};

public:
	// these fields are only used in deserialisation
	ntd::n3ds::es::Signature signature;
	std::array<byte_t, 32> calculated_hash;


	// these fields are used in both deserialisation & serialisation
	uint64_t title_id;
	uint16_t title_version;
	struct TwlCustomData
	{
		uint32_t public_save_data_size;
		uint32_t private_save_data_size;
		uint8_t flag;
	} twl_custom_data;

	struct CtrCustomData
	{
		uint32_t save_data_size;
		bool is_snake_only;
	} ctr_custom_data;

	std::vector<ContentInfo> content_info;
};

class TitleMetaDataDeserialiser : public TitleMetaData
{
public:
	// tmd stream
	TitleMetaDataDeserialiser(const std::shared_ptr<tc::io::IStream>& tmd_stream);
private:
	TitleMetaDataDeserialiser();
	std::string mModuleLabel;
};

/*
class TitleMetaDataSerialiser : public tc::io::IStream
{
public:
	// tmd TitleMetaData, issuer, RsaKey
	TitleMetaDataSerialiser();
private:
	std::string mModuleLabel;
}
*/

}}} // namespace ntd::n3ds::es