#pragma once
#include <string>
#include <vector>
#include <array>
#include <map>
#include <tc/Optional.h>
#include <tc/io.h>
#include <tc/crypto/RsaKey.h>

namespace ctrtool {

struct KeyBag
{
	using Aes128Key = std::array<byte_t, 16>;

	// NCCH encryption keys
	enum NcchFixedKeyIndex
	{
		NCCH_APPLICATION_FIXED_KEY = 0,
		NCCH_SYSTEM_FIXED_KEY = 1,
	};
	enum NcchSecureKeyIndex
	{
		NCCH_SECURE_KEY_FW1 = 0,
		NCCH_SECURE_KEY_FW7 = 1,
		NCCH_SECURE_KEY_FW9_3 = 10,
		NCCH_SECURE_KEY_FW9_6 = 11
	};
	std::map<byte_t, Aes128Key> ncch_fixed_key;
	std::map<byte_t, Aes128Key> ncch_secure_key_x;
	
	// ticket
	enum CommonKeyIndex
	{
		COMMONKEY_APPLICATION = 0,
		COMMONKEY_SYSTEM = 1,
		COMMONKEY_UNUSED_2 = 2,
		COMMONKEY_UNUSED_3 = 3,
		COMMONKEY_UNUSED_4 = 4,
		COMMONKEY_UNUSED_5 = 5
	};
	std::map<byte_t, Aes128Key> common_key;
	tc::Optional<Aes128Key> fallback_title_key;

	// NCCH seed
	std::map<uint64_t, Aes128Key> seed_db;
	tc::Optional<Aes128Key> fallback_seed;

	// BootROM Initialized Keyslots
	enum AesKeySlot
	{
		KEYSLOT_INITIAL_DATA = 0x3B,
		KEYSLOT_ES_COMMON_KEY = 0x3D
	};
	std::map<byte_t, Aes128Key> brom_static_key_x;
	std::map<byte_t, Aes128Key> brom_static_key_y;
	std::map<byte_t, Aes128Key> brom_static_key;

	// Firmware Keys
	enum FirmwareKeyIndex
	{
		FIRM_NGC_KEY = 0,
		FIRM_NOR_KEY = 1,
		FIRM_SD_KEY = 2,
	};
	std::map<byte_t, Aes128Key> firmware_key;

	// Normal RSA Keys
	enum RsaKeyIndex
	{
		RSAKEY_FIRM_NAND = 0, // normal: read from NCSD partition
		RSAKEY_FIRM_RECOVERY = 1, // recovery: read from NWN SPI / NTRCARD
		RSAKEY_NCSD_NAND = 2,
		RSAKEY_CFA_CCI = 3,
		RSAKEY_ACCESSDESC = 4,
		RSAKEY_CRR = 5,
		RSAKEY_SECUREINFO = 6,
		RSAKEY_LOCALFRIENDCODESEED = 7,
		RSAKEY_DSP = 8,
	};
	std::map<byte_t, tc::crypto::RsaKey> rsa_key;

	// SigHax Signatures
	using Rsa2048Signature = std::array<byte_t, 0x100>;
	std::map<byte_t, Rsa2048Signature> rsa_sighax_signature;

	// BroadOn RSA Keys
	struct BroadOnRsaSignerProfile
	{
		tc::ByteData certificate;
		tc::crypto::RsaKey key;
	};
	std::map<std::string, BroadOnRsaSignerProfile> broadon_rsa_signer;
};

class KeyBagInitializer : public KeyBag
{
public:
	KeyBagInitializer(bool isDev, const tc::Optional<std::string>& fallback_title_key_str, const tc::Optional<tc::io::Path>& seed_db_path, const tc::Optional<std::string>& fallback_seed_str);
private:
	KeyBagInitializer();

	void addStaticAesKeys(bool isDev);
	void addStaticRsaKeys(bool isDev);
	void addSigHaxSignatures(bool isDev);
	void importSeedDb(const std::shared_ptr<tc::io::ISource>& seed_db_source);
	bool importFallbackKey(tc::Optional<KeyBag::Aes128Key>& key, const std::string& key_str);

#pragma pack(push,1)
	struct SeedDbHeader
	{
		tc::bn::le32<uint32_t> n_entries;
		std::array<byte_t, 0xC> padding;
	};
	static_assert(sizeof(SeedDbHeader) == 0x10, "Size of Entry was incorrect.");

	struct SeedDbEntry
	{
		tc::bn::le64<uint64_t> title_id;
		KeyBag::Aes128Key seed;
		std::array<byte_t, 8> padding;
	};
	static_assert(sizeof(SeedDbEntry) == 0x20, "Size of SeedDbEntry was incorrect.");
#pragma pack(pop)
	
};

}