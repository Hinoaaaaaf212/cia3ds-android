#pragma once
#include <tc/types.h>

namespace ntd { namespace n3ds {

#pragma pack(push,1)

struct SystemControlInfo
{
	enum Flags
	{
		Flags_CompressExefsPartition0 = 0,
		Flags_SdmcApplication = 1,
	};

	struct CodeSegmentInfo
	{
		tc::bn::le32<uint32_t> address;
		tc::bn::le32<uint32_t> num_max_pages;
		tc::bn::le32<uint32_t> code_size;
	};
	static_assert(sizeof(CodeSegmentInfo) == 0xC, "CodeSegmentInfo had incorrect size.");

public:
	tc::bn::string<8>                        name;
	tc::bn::pad<5>                           padding0;
	union
	{
	    tc::bn::bitarray<1>                  bitarray;
	    byte_t                               raw;
	}                                        flags;
	tc::bn::le16<uint16_t>                   remaster_version;
	CodeSegmentInfo                          text;
	tc::bn::le32<uint32_t>                   stack_size;
	CodeSegmentInfo                          rodata;
	tc::bn::pad<4>                           padding1;
	CodeSegmentInfo                          data;
	tc::bn::le32<uint32_t>                   bss_size;
	std::array<tc::bn::le64<uint64_t>, 0x30> dependency_list;
	tc::bn::le64<uint64_t>                   savedata_size;
	tc::bn::le64<uint64_t>                   jump_id;
	tc::bn::pad<0x30>                        padding2;
};
static_assert(sizeof(SystemControlInfo) == 0x200, "SystemControlInfo had incorrect size.");

struct AccessControlInfo
{
	enum CpuSpeed
	{
		CpuSpeed_268MHz = 0,
		CpuSpeed_804MHz = 1,
	};
	
	enum SystemMode : byte_t
	{
		SystemMode_PROD = 0, // For Apps 64MB memory
		SystemMode_DEV1 = 2, // For Apps 96MB memory
		SystemMode_DEV2 = 3, // For Apps 80MB memory
		SystemMode_DEV3 = 4, // For Apps 72MB memory
		SystemMode_DEV4 = 5, // For Apps 32MB memory
	};

	enum SystemModeExt : byte_t
	{
		SystemModeExt_LEGACY = 0, // Use SystemMode setting
		SystemModeExt_PROD   = 1, // For Apps 124MB memory
		SystemModeExt_DEV1   = 2, // For Apps 178MB memory
		//SystemModeExt_DEV2   = 3, // Unknown App memory allocation (devunit only?, doesn't exist?)
	};

	enum FileSystemAccess
	{
		FileSystemAccess_CategorySystemApplication,
		FileSystemAccess_CategoryHardwareCheck,
		FileSystemAccess_CategoryFileSystemTool,
		FileSystemAccess_Debug,
		FileSystemAccess_TwlCardBackup,
		FileSystemAccess_TwlNandData,
		FileSystemAccess_Boss,
		FileSystemAccess_DirectSdmc,
		FileSystemAccess_Core,
		FileSystemAccess_CtrNandRo,
		FileSystemAccess_CtrNandRw,
		FileSystemAccess_CtrNandRoWrite,
		FileSystemAccess_CategorySystemSettings,
		FileSystemAccess_CardBoard,
		FileSystemAccess_ExportImportIvs,
		FileSystemAccess_DirectSdmcWrite,
		FileSystemAccess_SwitchCleanup,
		FileSystemAccess_SaveDataMove,
		FileSystemAccess_Shop,
		FileSystemAccess_Shell,
		FileSystemAccess_CategoryHomeMenu,
		FileSystemAccess_ExternalSeed,
	};

	enum OtherAttribute
	{
		OtherAttribute_NotUseRomFs,
		OtherAttribute_UseExtendedSavedataAccessControl,
	};

	enum ResourceLimitDescriptorIndex
	{
		ResourceLimitDescriptorIndex_MaxCpu = 0,
	};

	enum ResourceLimitCategory : byte_t
	{
		ResourceLimitCategory_Application = 0,
		ResourceLimitCategory_SysApplet = 1,
		ResourceLimitCategory_LibApplet = 2,
		ResourceLimitCategory_Other = 3
	};

	struct Flags
	{
		uint32_t enable_l2_cache : 1; // 0=disable, 1=enable
		uint32_t cpu_speed : 1; // CpuSpeed
		uint32_t : 6;

		uint32_t system_mode_ext : 4; // SystemModeExt
		uint32_t : 4;

		uint32_t ideal_processor : 2; // 0-3, default=0
		uint32_t affinity_mask : 2; // 0-3, default=0
		uint32_t system_mode : 4; // SystemMode

		int32_t  thread_priority : 8;
	};
	static_assert(sizeof(Flags) == sizeof(uint32_t), "Flags had incorrect size.");

	// the reason for the reverse ordering is because the normal serialization of this field is: "field |= save_id", then "field = field << 20"
	struct AccessibleUniqueIds
	{
		uint64_t save_id2 : 20;
		uint64_t save_id1 : 20;
		uint64_t save_id0 : 20;
		uint64_t flag : 1;
		uint64_t : 0;
	};
	static_assert(sizeof(AccessibleUniqueIds) == sizeof(uint64_t), "AccessibleUniqueIds had incorrect size.");

	using ServiceName = tc::bn::string<8>; // Those char[8] server names

	enum DescriptorPrefix
	{
		DescriptorPrefix_InterruptNumList = 3,
		DescriptorPrefix_SysCallList = 4,
		DescriptorPrefix_ReleaseKernelVersion = 6,
		DescriptorPrefix_HandleTableSize = 7,
		DescriptorPrefix_OtherCapabilities = 8,
		DescriptorPrefix_MappingStatic = 9,
		DescriptorPrefix_MappingIo = 11
	};

	static const size_t kMaxSerializableInterruptCount = 32; // number of serializable interupts
	static const size_t kMaxInterruptValue = 0x80; // system maximum total interupts
	static const size_t kMaxInterruptDescriptorNum = 8;

	struct InterruptDescriptor
	{
		uint32_t interrupt_0 : 7;
		uint32_t interrupt_1 : 7;
		uint32_t interrupt_2 : 7;
		uint32_t interrupt_3 : 7;
		uint32_t padding_bit : 1; // zero
		uint32_t prefix_bits : 3; // all ones
	};
	static_assert(sizeof(InterruptDescriptor) == sizeof(uint32_t), "InterruptDescriptor had incorrect size.");

	static const size_t kMaxSerializableSystemCallCount = 0xC0; // this is the physical maximum storable system calls
	static const size_t kMaxSystemCallValue = 0x7d; // this is the maximum value for a system call according to the system
	static const size_t kMaxSystemCallDescriptorNum = 8;

	struct SystemCallDescriptor
	{
		uint32_t systemcall_lower_bitarray : 24;
		uint32_t systemcall_upper : 3;
		uint32_t padding_bit : 1; // zero
		uint32_t prefix_bits : 4; // all ones
	};
	static_assert(sizeof(SystemCallDescriptor) == sizeof(uint32_t), "SystemCallDescriptor had incorrect size.");

	struct ReleaseKernelVersionDescriptor
	{
		uint32_t version : 16;
		uint32_t : 9;
		uint32_t padding_bit : 1; // zero
		uint32_t prefix_bits : 6; // all ones
	};
	static_assert(sizeof(ReleaseKernelVersionDescriptor) == sizeof(uint32_t), "ReleaseKernelVersionDescriptor had incorrect size.");

	struct HandleTableSizeDescriptor
	{
		uint32_t size : 16;
		uint32_t : 8;
		uint32_t padding_bit : 1; // zero
		uint32_t prefix_bits : 7; // all ones
	};
	static_assert(sizeof(HandleTableSizeDescriptor) == sizeof(uint32_t), "HandleTableSizeDescriptor had incorrect size.");

	enum MemoryType
	{
		MemoryType_Application = 1,
		MemoryType_System = 2,
		MemoryType_Base = 3
	};

	// KernelFlags
	struct OtherCapabilitiesDescriptor
	{
		uint32_t permit_debug : 1;
		uint32_t force_debug : 1;
		uint32_t can_use_non_alphabet_and_number : 1;
		uint32_t can_write_shared_page : 1;
		uint32_t can_use_privileged_priority : 1;
		uint32_t permit_main_function_argument : 1;
		uint32_t can_share_device_memory : 1;
		uint32_t runnable_on_sleep : 1;
		uint32_t memory_type : 4; // MemoryType
		uint32_t special_memory_layout : 1;
		uint32_t can_access_core2 : 1;
		uint32_t : 9;
		uint32_t padding_bit : 1; // zero
		uint32_t prefix_bits : 8; // all ones
	};
	static_assert(sizeof(OtherCapabilitiesDescriptor) == sizeof(uint32_t), "OtherCapabilitiesDescriptor had incorrect size.");

	// MappingStaticDescriptor come in pairs: [begin_page,end_page) where begin.flag == (true: range read-only, false: range is read-write), and end.flag == (true: static mapping, false: io mapping)
	struct MappingStaticDescriptor
	{
		uint32_t page : 20; // page2addr: addr = page << 20
		uint32_t flag : 1;
		uint32_t : 1;
		uint32_t padding_bit : 1; // zero
		uint32_t prefix_bits : 9; // all ones
	};
	static_assert(sizeof(MappingStaticDescriptor) == sizeof(uint32_t), "MappingStaticDescriptor had incorrect size.");

	// MappingIODescriptor describe single page IO mappings, these are always read-write permissions. [begin_page)
	// When a multi-page IO mapping is required, these is indicated with two MappingStaticDescriptor
	struct MappingIODescriptor
	{
		uint32_t page : 20; // page2addr: addr = page << 20
		uint32_t padding_bit : 1; // zero
		uint32_t prefix_bits : 11; // all ones
	};
	static_assert(sizeof(MappingIODescriptor) == sizeof(uint32_t), "MappingIODescriptor had incorrect size.");

	enum Arm9Capability
	{
		Arm9Capability_FsMountNand = 0,
		Arm9Capability_FsMountNandRoWrite = 1,
		Arm9Capability_FsMountTwln = 2,
		Arm9Capability_FsMountWnand = 3,
		Arm9Capability_FsMountCardSpi = 4,
		Arm9Capability_UseSdif3 = 5,
		Arm9Capability_CreateSeed = 6,
		Arm9Capability_UseCardSpi = 7,
		Arm9Capability_SdApplication = 8,
		Arm9Capability_UseDirectSdmc = 9,
	};

public:
	// begin ARM11 System
	tc::bn::le64<uint64_t>                 program_id;
	tc::bn::le32<uint32_t>                 core_version;
	tc::bn::le32<Flags>                    flags;
	std::array<tc::bn::le16<uint16_t>, 16> resource_limit_descriptor; // for each descriptor use, see ResourceLimitDescriptorIndex
	union 
	{
		tc::bn::le64<uint64_t>             ext_savedata_id; // this field is used when other_attributes.test(USE_EXTENDED_SAVEDATA_ACCESS_CONTROL) == false
		tc::bn::le64<AccessibleUniqueIds>  accessible_unique_ids_1; // this field is used when other_attributes.test(USE_EXTENDED_SAVEDATA_ACCESS_CONTROL) == true
	};
	std::array<tc::bn::le32<uint32_t>, 2>  system_savedata_id;
	tc::bn::le64<AccessibleUniqueIds>      accessible_unique_ids_0; // other_attributes.test(USE_EXTENDED_SAVEDATA_ACCESS_CONTROL): true = accessible_save_ids, false: other_user_save_ids
	tc::bn::bitarray<7>                    fs_access; // FileSystemAccess
	tc::bn::bitarray<1>                    other_attributes; // OtherAttribute
	std::array<ServiceName, 34>            service_access_control; // In firmware versions prior to 9.2.0(?) this array is only 32 elements long
	tc::bn::pad<0xf>                       padding0;
	byte_t                                 resource_limit_category; // ResourceLimitCategory
	// end ARM11 System

	// begin ARM11 Kernel
	std::array<tc::bn::le32<uint32_t>, 28> kernel_descriptors; // Descripters are a collection of u32s, with masks to differentiate between different descs. Impl'd Order: SysCallControl, InteruptNumList, AddressMapping(ExHdr order: IoMaps, MemMaps, AccDesc order: MemMaps, IoMaps), OtherCapabilities, HandleTableSize, ReleaseKernelVersion
	tc::bn::pad<0x10>                      padding1;
	// end ARM11 Kernel

	// begin ARM9
	tc::bn::bitarray<0xf>                  arm9_access_control;
	byte_t                                 desc_version;
	// end ARM9
};
static_assert(sizeof(AccessControlInfo) == 0x200, "AccessControlInfo had incorrect size.");

struct ExtendedHeader
{
	SystemControlInfo system_control_info;
	AccessControlInfo access_control_info;
};
static_assert(sizeof(ExtendedHeader) == 0x400, "ExtendedHeader had invalid size");

struct AccessDescriptor
{
	using Rsa2048Block = std::array<byte_t, 0x100>;

	Rsa2048Block signature;
	Rsa2048Block ncch_rsa_modulus;
	AccessControlInfo access_control_info;
};
static_assert(sizeof(AccessDescriptor) == 0x400, "AccessDescriptor had invalid size");


#pragma pack(pop)

}} // namespace ntd::n3ds