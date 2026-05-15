#pragma once
#include <tc/types.h>

namespace ntd { namespace n3ds {

#pragma pack(push,1)

struct CroHeader
{
	static const uint32_t kStructMagic = tc::bn::make_struct_magic_uint32("CRO0");

	struct SectionEntry
	{
		tc::bn::le32<uint32_t> offset;
		union
		{
			tc::bn::le32<uint32_t> size;
			tc::bn::le32<uint32_t> num;
		};
	};
	static_assert(sizeof(SectionEntry) == 0x8, "SectionEntry had invalid size");

	using Hash = std::array<byte_t, 0x20>;

	enum CoreFunctionIndex
	{
		CoreFunctionIndex_nnroControlObject_ = 0, // 0xFFFFFFFF in CRS
		CoreFunctionIndex_OnLoad = 1, // will be called when the module is initialized. Set to 0xFFFFFFFF if not exists.
		CoreFunctionIndex_OnExit = 2, // will be called when the module is finalized. Set to 0xFFFFFFFF if not exists.
		CoreFunctionIndex_OnUnresolved = 3, // will be called when an unresolved function is called. Set to 0xFFFFFFFF if not exists.
	};

	enum SectionIndex
	{
		SectionIndex_Code = 0,
		SectionIndex_Data = 1,
		SectionIndex_ModuleName = 2,
		SectionIndex_SegmentTable = 3, // (size = num*12)
		SectionIndex_NamedExportTable = 4, // (size = num * 8)
		SectionIndex_IndexedExportTable = 5, // (size = num * 4)
		SectionIndex_ExportStrings = 6,
		SectionIndex_ExportTree = 7, // fast lookups based on a trie-like structure, (size = num * 8)
		SectionIndex_ImportModuleTable = 8, // (size = num * 20)
		SectionIndex_ImportPatches = 9, // (size = num * 12)
		SectionIndex_NamedImportTable = 10, // (size = num * 8)
		SectionIndex_IndexedImportTable = 11, // (size = num * 8)
		SectionIndex_AnonymousImportTable = 12, // (size = num * 8)
		SectionIndex_ImportStrings = 13,
		SectionIndex_Unk14 = 14,
		SectionIndex_RelocationPatches = 15, // (size = num * 12)
		SectionIndex_Unk16 = 16
	};

	std::array<Hash, 4> hash_table;
	tc::bn::le32<uint32_t> struct_magic;
	tc::bn::le32<uint32_t> name_offset;
	tc::bn::le32<uint32_t> node0; // Next loaded CRO pointer, set by RO during loading (Usually zero when the CRO is being loaded)
	tc::bn::le32<uint32_t> node1; // Previous loaded CRO pointer, set by RO during loading
	tc::bn::le32<uint32_t> file_size;
	tc::bn::le32<uint32_t> bss_size;
	tc::bn::le32<uint32_t> unk0;
	tc::bn::le32<uint32_t> unk1;
	std::array<tc::bn::le32<uint32_t>, 4> core_function_segment_offset;
	std::array<SectionEntry, 17> section;
};
static_assert(sizeof(CroHeader) == 0x138, "CroHeader had invalid size");

struct CroSegmentOffset
{
	uint32_t segment_index : 4; // Segment index for table
	uint32_t segment_offset : 28; // Offset into segment
};
static_assert(sizeof(CroSegmentOffset) == 0x4, "CroSegmentOffset had invalid size");

struct CroSegmentTableEntry
{
	enum SegmentId : uint32_t
	{
		SegmentId_Text = 0,
		SegmentId_RoData = 1,
		SegmentId_Data = 2,
		SegmentId_Bss = 4
	};

	tc::bn::le32<uint32_t> segment_offset;
	tc::bn::le32<uint32_t> segment_size;
	tc::bn::le32<SegmentId> segment_id;
};
static_assert(sizeof(CroSegmentTableEntry) == 0xC, "CroSegmentTableEntry had invalid size");

struct CroNamedExportTableEntry
{
	tc::bn::le32<uint32_t> name_offset;
	tc::bn::le32<CroSegmentOffset> segment_offset_for_export;
};
static_assert(sizeof(CroNamedExportTableEntry) == 0x8, "CroNamedExportTableEntry had invalid size");

struct CroIndexedExportTableEntry
{
	tc::bn::le32<CroSegmentOffset> segment_offset_for_export;
};
static_assert(sizeof(CroIndexedExportTableEntry) == 0x4, "CroIndexedExportTableEntry had invalid size");

struct CroNamedImportTableEntry
{
	tc::bn::le32<uint32_t> name_offset;
	tc::bn::le32<uint32_t> import_patch_list_offset; // Offset of the head of a linear list that contains the patches for this import
};
static_assert(sizeof(CroNamedImportTableEntry) == 0x8, "CroNamedImportTableEntry had invalid size");

struct CroIndexedImportTableEntry
{
	tc::bn::le32<uint32_t> export_symbol_index; // index of the export symbol
	tc::bn::le32<uint32_t> import_patch_list_offset; // Offset of the head of a linear list that contains the patches for this import
};
static_assert(sizeof(CroIndexedImportTableEntry) == 0x8, "CroIndexedImportTableEntry had invalid size");

struct CroAnonynousImportTableEntry
{
	tc::bn::le32<CroSegmentOffset> export_symbol_segment_offset;
	tc::bn::le32<uint32_t> import_patch_list_offset; // Offset of the head of a linear list that contains the patches for this import
};
static_assert(sizeof(CroAnonynousImportTableEntry) == 0x8, "CroAnonynousImportTableEntry had invalid size");

struct CroImportModuleTableEntry
{
	tc::bn::le32<uint32_t> module_name_offset;
	tc::bn::le32<uint32_t> indexed_import_num;
	tc::bn::le32<uint32_t> indexed_import_patch_list_offset; // Offset of the head of a sub list in Indexed Import Table
	tc::bn::le32<uint32_t> anonynous_import_num;
	tc::bn::le32<uint32_t> anonynous_import_patch_list_offset; // Offset of the head of a sub list in Anonymous Import Table
};
static_assert(sizeof(CroImportModuleTableEntry) == 0x14, "CroImportModuleTableEntry had invalid size");

struct CroPatchEntry
{
	enum PatchType : byte_t
	{
		PatchType_Ignore = 0,
		PatchType_WriteU32Absolute = 2, // (base+addend)
		PatchType_WriteU32Relative = 3, // (base+addend-in_ptr)
		PatchType_ThumbBranch = 10,
		PatchType_ARM32Branch = 28,
		PatchType_ModifyARM32BranchOffset = 29,
		PatchType_WriteU32Absolute_2 = 38, // duplicate of PatchType_WriteU32Absolute
		PatchType_WriteU32Relative_2 = 42, // (((signed int)base*2)/2+addend-in_ptr), otherwise err) (This is apparently a subset of relocation type for ARM ELF)
	};

	tc::bn::le32<CroSegmentOffset> segment_offset_for_output;
	PatchType patch_type;
	byte_t unk0; // For import patches, non-zero if last entry; for relocation patches, this is the referred segment index
	byte_t unk1; // For import patches, 1 is written to first entry if all symbols loaded successfully; unknown (padding?) for relocation patches
	byte_t unk2; // Unknown (padding?)
	tc::bn::le32<uint32_t> addend;
};
static_assert(sizeof(CroPatchEntry) == 0xC, "CroPatchEntry had invalid size");

#pragma pack(pop)

}} // namespace ntd::n3ds