#pragma once
#include <brd/es/es_sign.h>

namespace brd { namespace es {

// ES title type
enum class ESTitleType : uint32_t
{
    NC_TITLE = 0x1, // bit0 NetCard - End-of-life
    NG_TITLE = 0x2, // bit1 Wii/NDEV
    DS_TITLE = 0x4, // bit2 TWL/DSi
    DATA     = 0x8, // bit3 boring data title
    CT_TITLE = 0x40, // bit6 CTR/3DS
    GVM_TITLE = 0x80, // bit7 GVM = ? (from BroadOn libraries)
    CAFE_TITLE = 0x100, // bit8 WiiU (from WiiU sdk)
};

// ES content type
enum ESContentType : uint16_t
{
    ESContentType_ENCRYPTED = 0x1, // bit0 (from broadOn & b4)
    ESContentType_DISC = 0x2, // bit1 (from broadOn & b4)
    ESContentType_HASHED = 0x2, // bit1 (from b4)
    ESContentType_CFM = 0x4, // bit3 (from broadOn & b4)
    ESContentType_SHA1_HASH = 0x2000, // bit13 from b4 (wiiu sdk)
    ESContentType_OPTIONAL = 0x4000, // bit14 (from broadOn & b4)
    ESContentType_SHARED = 0x8000, // bit15 (from broadOn & b4)
};

//
// Maximum possible content index value is 64K - 2, since
// the maximum number of contents per title is 64K - 1
//
static const size_t ES_CONTENT_INDEX_MAX = 65534;


// There are a maximum of 64 CMD groups, each with a maximum of 1K CMDs
static const size_t ES_MAX_CMDS_IN_GROUP = 1024;
static const size_t ES_MAX_CMD_GROUPS = 64;

#pragma pack(push, 4)

#ifdef _WIN32
#pragma warning(disable : 4200) // silence warnings for usage of empty arrays in stucts
#endif

struct ESContentMeta
{
	tc::bn::be32<uint32_t> cid;    // 32-bit content ID
    tc::bn::be16<uint16_t> index;  // Content index, unique per title
    tc::bn::be16<uint16_t> type;   // Content type
    tc::bn::be64<uint64_t> size;   // Unencrypted content size in bytes
    Sha1Hash               hash;   // Hash of the content
};
static_assert(sizeof(ESContentMeta) == 36, "ESContentMeta size");

struct ESV1ContentMeta
{
    tc::bn::be32<uint32_t> cid;    // 32-bit content ID
    tc::bn::be16<uint16_t> index;  // Content index, unique per title
    tc::bn::be16<uint16_t> type;   // Content type
    tc::bn::be64<uint64_t> size;   // Unencrypted content size in bytes
    Sha256Hash             hash;   // Hash of the content
};
static_assert(sizeof(ESV1ContentMeta) == 48, "ESV1ContentMeta size");

struct ESTitleMetaHeader
{
    using ESTmdCustomData = std::array<uint8_t, 32>;
    using ESTmdReserved = std::array<uint8_t, 30>;

    uint8_t                   version;            // TMD version number
    uint8_t                   caCrlVersion;       // CA CRL version number
    uint8_t                   signerCrlVersion;   // Signer CRL version number
    tc::bn::be64<uint64_t>    sysVersion;         // System software version number
    tc::bn::be64<uint64_t>    titleId;            // 64-bit title id
    tc::bn::be32<ESTitleType> type;               // 32-bit title type
    tc::bn::be16<uint16_t>    groupId;
    ESTmdCustomData           customData;         // 32-byte custom data
    ESTmdReserved             reserved;           // 30-byte reserved info
    tc::bn::be32<uint32_t>    accessRights;       // Rights to system resources
    tc::bn::be16<uint16_t>    titleVersion;       // 16-bit title version
    tc::bn::be16<uint16_t>    numContents;        // Number of contents
    tc::bn::be16<uint16_t>    bootIndex;          // Boot content index
    tc::bn::be16<uint16_t>    minorTitleVersion;  // 16-bit minor title version
};
static_assert(sizeof(ESTitleMetaHeader) == 100, "ESTitleMetaHeader size");

struct ESV1ContentMetaGroup
{
    tc::bn::be16<uint16_t> offset;             // Offset content index
    tc::bn::be16<uint16_t> nCmds;              // Number of CMDs in this group
    Sha256Hash             groupHash;          // Hash for this group of CMDs
};
static_assert(sizeof(ESV1ContentMetaGroup) == 36, "ESV1ContentMetaGroup size");

struct ESV1TitleMetaHeader
{
    using ESV1ContentMetaGroupArray = std::array<ESV1ContentMetaGroup, ES_MAX_CMD_GROUPS>;

    Sha256Hash                hash;          // Hash for the CMD groups
    ESV1ContentMetaGroupArray cmdGroups;
};
static_assert(sizeof(ESV1TitleMetaHeader) == 2336, "ESV1TitleMetaHeader size");

struct ESTitleMeta
{
    ESSigRsa2048      sig;            // RSA 2048-bit sign of the TMD header
    ESTitleMetaHeader head;
    ESContentMeta     contents[];     // CMD array sorted by content index
};
static_assert(sizeof(ESTitleMeta) == 484, "ESTitleMeta size");

struct ESV1TitleMeta
{
    ESSigRsa2048        sig;            // RSA 2048-bit sign of the TMD header
    ESTitleMetaHeader   head;
    ESV1TitleMetaHeader v1Head;         // Extension to the v0 TMD header
    ESV1ContentMeta     contents[];     // CMD array sorted by content index
};
static_assert(sizeof(ESV1TitleMeta) == 2820, "ESV1TitleMeta size");

#ifdef _WIN32
#pragma warning(default : 4200)
#endif

#pragma pack(pop)

}} // namespace brd::es