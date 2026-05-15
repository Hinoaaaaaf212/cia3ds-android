#pragma once
#include <brd/es/es_sign.h>

namespace brd { namespace es {

// ES license types
enum class ESLicenseType : uint8_t
{
    PERMANENT = 0,
    DEMO = 1,
    TRIAL = 2,
    RENTAL = 3,
    SUBSCRIPTION = 4,
    SERVICE = 5,
};
static const uint8_t ES_LICENSE_MASK = 0xf;

// ES title-level limit codes
enum class ESLimitCode : uint32_t
{
    DURATION_TIME = 1,
    ABSOLUTE_TIME = 2,
    NUM_TITLES = 3,
    NUM_LAUNCH = 4,
    ELAPSED_TIME = 5,
};
static const uint32_t ES_MAX_LIMIT_TYPE = 8;

// ES item-level rights
enum class ESItemType : uint32_t
{
    PERMANENT = 1,
    SUBSCRIPTION = 2,
    CONTENT = 3,
    CONTENT_CONSUMPTION = 4,
    ACCESS_TITLE = 5,
	LIMITED_RESOURCE = 6,
};

enum class ESPropertyMaskFlag : uint16_t
{
	PRE_INSTALL = 0x1, // bit0
	SHARED_TITLE = 0x2, // bit1
	ALLOW_ALL_CONTENT = 0x4, // bit2
	DEVICE_LINK_INDEPENDENT = 0x8, // bit3
	VOLATILE = 0x10, // bit4
	ELICENSE_REQUIRED = 0x20, // bit5
};

enum class ESV1SectionHeaderFlag : uint16_t
{
	COMPRESSED = 0x1 // ironically this is defined but not supported, probably for future use
};

enum class ESV2TitleKekType : byte_t
{
	AES128_CBC,
	RSA2048
};

#pragma pack(push, 4)

#ifdef _WIN32
#pragma warning(disable : 4200) // silence warnings for usage of empty arrays in stucts
#endif

struct ESLimitedPlayEntry
{
    tc::bn::be32<uint32_t> code; //ESLimitCode
    tc::bn::be32<uint32_t> limit;
};
static_assert(sizeof(ESLimitedPlayEntry) == 8, "ESLimitedPlayEntry size");

using ESSysAccessMask = std::array<byte_t, 2>;
using ESTicketCustomData = std::array<byte_t, 20>;
using ESTicketReserved = std::array<byte_t, 25>;
using ESCidxMask = std::array<byte_t, 64>;
using ESLimitedPlayArray = std::array<ESLimitedPlayEntry, 8>;

using ESReferenceId = std::array<byte_t, 16>;

using ESV1CidxMask = std::array<byte_t, 128>;

using ESV2TitleKey = std::array<byte_t, kRsa2048Size>;
using ESRightsId = std::array<byte_t, 16>;
using ESV2TicketReserved = std::array<byte_t, 8>;

struct ESTicket
{
    ESSigRsa2048              sig;               // RSA 2048-bit sign of the ticket
    Ecc233PublicKey           serverPubKey;      // Ticketing server public key
    uint8_t                   version;           // Ticket data structure version number
    uint8_t                   caCrlVersion;      // CA CRL version number
    uint8_t                   signerCrlVersion;  // Signer CRL version number
    Aes128Key                 titleKey;          // Published title key
    /* 1 byte alignment padding */
    tc::bn::be64<uint64_t>    ticketId;          // Unique 64bit ticket ID
    tc::bn::be32<uint32_t>    deviceId;          // Unique 32bit device ID
    tc::bn::be64<uint64_t>    titleId;           // Unique 64bit title ID
    ESSysAccessMask           sysAccessMask;     // 16-bit cidx mask to indicate which
                                                 // of the first 16 pieces of contents
                                                 // can be accessed by the system app
    tc::bn::be16<uint16_t>    ticketVersion;     // 16-bit ticket version
    tc::bn::be32<uint32_t>    accessTitleId;     // 32-bit title ID for access control
    tc::bn::be32<uint32_t>    accessTitleMask;   // 32-bit title ID mask
    uint8_t                   licenseType;       //
    uint8_t                   keyId;             // Common key ID
    tc::bn::be16<uint16_t>    propertyMask;      // 16-bit property mask
    ESTicketCustomData        customData;        // 20-byte custom data
    ESTicketReserved          reserved;          // 25-byte reserved info
    uint8_t                   audit;             //
    ESCidxMask                cidxMask;          // Bit-mask of the content indices
    /* 2 bytes alignment padding */
    ESLimitedPlayArray        limits;            // Limited play entries
};
static_assert(sizeof(ESTicket) == 676, "ESTicket size");

struct ESV1TicketHeader
{
    tc::bn::be16<uint16_t>  hdrVersion;         // Version of the ticket header
    tc::bn::be16<uint16_t>  hdrSize;            // Size of ticket header
    tc::bn::be32<uint32_t>  ticketSize;         // Size of the v1 portion of the ticket
    tc::bn::be32<uint32_t>  sectHdrOfst;        // Offset of the section header table
    tc::bn::be16<uint16_t>  nSectHdrs;          // Number of section headers
    tc::bn::be16<uint16_t>  sectHdrEntrySize;   // Size of each section header
    tc::bn::be32<uint32_t>  flags;              // Miscellaneous attributes
};
static_assert(sizeof(ESV1TicketHeader) == 20, "ESV1TicketHeader size");

struct ESV1SectionHeader
{
    tc::bn::be32<uint32_t>  sectOfst;       // Offset of this section
    tc::bn::be32<uint32_t>  nRecords;       // Number of records in this section
    tc::bn::be32<uint32_t>  recordSize;     // Size of each record
    tc::bn::be32<uint32_t>  sectionSize;    // Total size of this section
    tc::bn::be16<uint16_t>  sectionType;    // Type code of this section
    tc::bn::be16<uint16_t>  flags;          // Miscellaneous attributes
};
static_assert(sizeof(ESV1SectionHeader) == 20, "ESV1SectionHeader size");

struct ESV1Ticket
{
    ESTicket          head;
    ESV1TicketHeader  v1Head;
    ESV1SectionHeader sectHdrs[];
};
static_assert(sizeof(ESV1Ticket) == 696, "ESV1Ticket size");

struct ESV1PermanentRecord
{
    ESReferenceId          referenceId;        // Reference ID
    tc::bn::be32<uint32_t> referenceIdAttr;    // Reference ID attributes
};
static_assert(sizeof(ESV1PermanentRecord) == 20, "ESV1PermanentRecord size");

struct ESV1SubscriptionRecord
{
    tc::bn::be32<uint32_t>  limit;              // Expiration time
    ESReferenceId           referenceId;        // Reference ID
    tc::bn::be32<uint32_t>  referenceIdAttr;    // Reference ID attributes
};
static_assert(sizeof(ESV1SubscriptionRecord) == 24, "ESV1SubscriptionRecord size");

struct ESV1ContentRecord
{
    tc::bn::be32<uint32_t>  offset;             // Offset content index
    ESV1CidxMask            accessMask;         // Access mask
};
static_assert(sizeof(ESV1ContentRecord) == 132, "ESV1ContentRecord size");

struct ESV1ContentConsumptionRecord
{
    tc::bn::be16<uint16_t>  index;              // Content index
    tc::bn::be16<uint16_t>  code;               // Limit code
    tc::bn::be32<uint32_t>  limit;              // Limit value
};
static_assert(sizeof(ESV1ContentConsumptionRecord) == 8, "ESV1ContentConsumptionRecord size");

struct ESV1AccessTitleRecord
{
    tc::bn::be64<uint64_t>  accessTitleId;      // Access title ID
    tc::bn::be64<uint64_t>  accessTitleMask;    // Access title mask
};
static_assert(sizeof(ESV1AccessTitleRecord) == 16, "ESV1AccessTitleRecord size");

struct ESV1LimitedResourceRecord
{
    tc::bn::be32<uint32_t>  limit;              // Expiration time
    ESReferenceId           referenceId;        // Reference ID
    tc::bn::be32<uint32_t>  referenceIdAttr;    // Reference ID attributes
};
static_assert(sizeof(ESV1LimitedResourceRecord) == 24, "ESV1LimitedResourceRecord size");

struct ESV2Ticket
{
    ESSigRsa2048            sig;                // RSA 2048-bit sign of the ticket
    ESV2TitleKey            titleKey;           // Published title key
    uint8_t                 version;            // Ticket data structure version number
	uint8_t                 keyType;            // Title key encryption key type
	tc::bn::le16<uint16_t>  ticketVersion;      // 16-bit ticket version
	uint8_t                 licenseType;
    uint8_t                 keyId;              // Common key ID
    tc::bn::le16<uint16_t>  propertyMask;       // 16-bit property mask
	ESV2TicketReserved      reservedRegion;     // probably the accessTitleId & mask
	tc::bn::le64<uint64_t>  ticketId;           // Unique 64bit ticket ID
	tc::bn::le64<uint64_t>  deviceId;           // Unique 64bit device ID
	ESRightsId              rightsId;           // Unique 128bit rights ID
	tc::bn::le32<uint32_t>  accountId;          // Unique 32bit account ID
	tc::bn::le32<uint32_t>  sectTotalSize;      // Total size of sections
	tc::bn::le32<uint32_t>  sectHdrOffset;      // Offset of the section header table
	tc::bn::le16<uint16_t>  nSectHdrs;          // Number of section headers
	tc::bn::le16<uint16_t>  nSectHdrEntrySize;  // Size of each section header
};
static_assert(sizeof(ESV2Ticket) == 704, "ESV2Ticket size");

struct ESV2SectionHeader
{
    tc::bn::le32<uint32_t>  sectOfst;       // Offset of this section
    tc::bn::le32<uint32_t>  recordSize;     // Size of each record
    tc::bn::le32<uint32_t>  sectionSize;    // Total size of this section
	tc::bn::le16<uint16_t>  nRecords;       // Number of records in this section
    tc::bn::le16<uint16_t>  sectionType;    // Type code of this section
};
static_assert(sizeof(ESV2SectionHeader) == 16, "ESV2SectionHeader size");

#ifdef _WIN32
#pragma warning(default : 4200)
#endif

#pragma pack(pop)

}} // namespace brd::es