#pragma once

typedef enum elf_errors
{
	NOT_ELF_FILE = -10,
	NOT_CTR_ARM_ELF = -11,
	NON_EXECUTABLE_ELF = -12,
	ELF_SECTION_NOT_FOUND = -13,
	NOT_FIND_TEXT_SEGMENT = -14,
	NOT_FIND_DATA_SEGMENT = -15,
	ELF_SEGMENT_SECTION_SIZE_MISMATCH = -16,
	ELF_SEGMENTS_NOT_CONTINUOUS = -17,
	ELF_SEGMENTS_NOT_FOUND = -18,
} elf_errors;

typedef enum elf_section_type
{
	SHT_NULL,
	SHT_PROGBITS,
	SHT_SYMTAB,
	SHT_STRTAB,
	SHT_RELA,
	SHT_HASH,
	SHT_DYNAMIC,
	SHT_NOTE,
	SHT_NOBITS,
	SHT_REL,
	SHT_SHLIB,
	SHT_DYNSYM,
	SHT_UNKNOWN12,
	SHT_UNKNOWN13,
	SHT_INIT_ARRAY,
	SHT_FINI_ARRAY,
	SHT_PREINIT_ARRAY,
	SHT_GROUP,
	SHT_SYMTAB_SHNDX,
	SHT_NUM,
	SHT_ARM_EXIDX = 0x70000001,
	SHT_ARM_PREEMPTMAP,
	SHT_ARM_ATTRIBUTES,
	SHT_ARM_DEBUGOVERLAY,
	SHT_ARM_OVERLAYSECTION
} elf_section_type;

typedef enum elf_section_flag
{
	SHF_WRITE = 0x1,
	SHF_ALLOC = 0x2,
	SHF_EXECINSTR = 0x4,
	SHF_MERGE = 0x10,
	SHF_STRINGS = 0x20,
	SHF_INFO_LINK = 0x40,
	SHF_LINK_ORDER = 0x80,
	SHF_OS_NONCONFORMING = 0x100,
	SHF_GROUP = 0x200,
	SHF_TLS = 0x400
} elf_section_flag;

typedef struct elf_section
{
	const char *name;
	u32 type;
	u32 flags;
	const u8 *ptr;
	u32 fileOffset;
	u32 size;
	u32 vAddr;
	u32 alignment;
} elf_section;

typedef enum elf_program_type
{
	PT_NULL,
	PT_LOAD,
	PT_DYNAMIC,
	PT_INTERP,
	PT_NOTE,
	PT_SHLIB,
	PT_PHDR,
} elf_program_type;

typedef enum elf_program_flag
{
	PF_X = 0x1,
	PF_W = 0x2,
	PF_R = 0x4,
	PF_CTRSDK = 0x80000000,

	PF_TEXT = (PF_R|PF_X),
	PF_DATA = (PF_R|PF_W),
	PF_RODATA = PF_R
} elf_program_flag;

typedef struct elf_segment
{
	u32 type;
	u32 flags;
	const u8 *ptr;
	u32 fileOffset;
	u32 fileSize;
	u32 memSize;
	u32 vAddr;
	u32 pAddr;
	u32 alignment;
} elf_segment;

typedef struct elf_context
{
	const u8 *file;

	u32 shdrOffset;
	u16 shdrNameIndex;
	u32 phdrOffset;

	u16 sectionNum;
	elf_section *sections;

	u16 segmentNum;
	elf_segment *segments;
} elf_context;


int elf_Init(elf_context *ctx, const u8 *fp);
void elf_Free(elf_context *ctx);

u16 elf_SectionNum(elf_context *ctx);
const elf_section* elf_GetSections(elf_context *ctx);

u16 elf_SegmentNum(elf_context *ctx);
const elf_segment* elf_GetSegments(elf_context *ctx);