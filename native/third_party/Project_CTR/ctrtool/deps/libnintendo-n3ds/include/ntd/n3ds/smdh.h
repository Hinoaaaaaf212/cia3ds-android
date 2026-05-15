#pragma once
#include <tc/types.h>

namespace ntd { namespace n3ds {

#pragma pack(push,1)

struct SystemMenuDataHeader
{
	static const uint32_t kStructMagic = tc::bn::make_struct_magic_uint32("SMDH");
	static const uint16_t kFormatVersion = 0;
	static const size_t kTitleNum = 16;

	struct ApplicationTitle
	{
		static const size_t kShortDescriptionLength = 0x40;
		static const size_t kLongDescriptionLength = 0x80;
		static const size_t kPublisherLength = 0x40;

		std::array<tc::bn::le16<char16_t>, kShortDescriptionLength> short_description;
		std::array<tc::bn::le16<char16_t>, kLongDescriptionLength>  long_description;
		std::array<tc::bn::le16<char16_t>, kPublisherLength>        publisher;
	};
	static_assert(sizeof(ApplicationTitle) == 0x200, "ApplicationTitle had invalid size");

	enum Language
	{
		Language_Japanese = 0,
		Language_English = 1,
		Language_French = 2,
		Language_German = 3,
		Language_Italian = 4,
		Language_Spanish = 5,
		Language_ChineseSimplified = 6,
		Language_Korean = 7,
		Language_Dutch = 8,
		Language_Portuguese = 9,
		Language_Russian = 10,
		Language_ChineseTraditional = 11,
	};

	struct ApplicationSettings
	{
		static const size_t kAgeRatingNum = 16;

		enum Organisation
		{
			Organisation_CERO = 0, // Japan
			Organisation_ESRB = 1, // North America (USA?)
			Organisation_USK = 3, // Germany
			Organisation_PEGI_GEN = 4, // Europe
			Organisation_PEGI_PRT = 6, // Portugal
			Organisation_PEGI_BBFC = 7, // UK
			Organisation_COB = 8, // Australia (& NZ?)
			Organisation_GRB = 9, // South Korea
			Organisation_CGSRR = 10 // Taiwan
		};

		enum RegionFlag
		{
			RegionFlag_Japan = 0,
			RegionFlag_NorthAmerica = 1,
			RegionFlag_Europe = 2,
			RegionFlag_Australia = 3,
			RegionFlag_China = 4,
			RegionFlag_Korea = 5,
			RegionFlag_Taiwan = 6
		};

		enum AppFlag
		{
			AppFlag_Visible = 0,
			AppFlag_Autoboot = 1,
			AppFlag_Use3dEffect = 2,
			AppFlag_RequireAcceptEula = 3,
			AppFlag_AutosaveOnExit = 4,
			AppFlag_UseExtendedBanner = 5,
			AppFlag_RatingUsed = 6,
			AppFlag_UsesSaveData = 7,
			AppFlag_RecordUsage = 8,
			AppFlag_DisableSaveDataBackup = 10,
			AppFlag_EnableMiiverseJumpArgs = 11,
			AppFlag_SnakeOnly = 12,
			AppFlag_DepositSale = 13
		};

		struct AgeRating
		{
			byte_t age : 5;
			byte_t no_age_restriction : 1;
			byte_t rating_pending : 1;
			byte_t enabled_rating : 1;
		};
		static_assert(sizeof(AgeRating) == 0x1, "AgeRating had invalid size");

		struct EulaVersion
		{
			uint32_t minor : 8;
			uint32_t major : 8;
			uint32_t : 0;
		};
		static_assert(sizeof(EulaVersion) == 0x4, "EulaVersion had invalid size");

		std::array<AgeRating, kAgeRatingNum> age_rating;
		tc::bn::bitarray<sizeof(uint32_t)>   region_lockout; // see RegionFlag
		tc::bn::le32<uint32_t>               match_maker_id;
		tc::bn::le64<uint64_t>               match_maker_bit_id;
		tc::bn::bitarray<sizeof(uint32_t)>   flags; // see AppFlag
		tc::bn::le32<EulaVersion>            eula_version;
		tc::bn::le32<float>                  optiminal_animation_frame; // for banner
		tc::bn::le32<uint32_t>               cec_id;
	};
	static_assert(sizeof(ApplicationSettings) == 0x30, "ApplicationSettings had invalid size");

	tc::bn::le32<uint32_t>                  struct_magic; // SMDH
	tc::bn::le16<uint16_t>                  version;
	tc::bn::pad<2>                          reserved0;
	std::array<ApplicationTitle, kTitleNum> title;
	ApplicationSettings                     settings;
	tc::bn::pad<8>                          reserved1;
	std::array<byte_t, 0x480>               small_icon_data;
	std::array<byte_t, 0x1200>              large_icon_data;
};
static_assert(sizeof(SystemMenuDataHeader) == 0x36C0, "SystemMenuDataHeader had invalid size");

#pragma pack(pop)

}} // namespace ntd::n3ds