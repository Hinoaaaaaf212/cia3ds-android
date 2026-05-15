#include "lib.h"
#include "elf.h"

static const u32 ELF_MAGIC = 0x7f454c46;

typedef enum elf_bit_format_types
{
	elf_32_bit = 1,
	elf_64_bit = 2,
} elf_bit_format_types;

typedef enum elf_endianness
{
	elf_little_endian = 1,
	elf_big_endian = 2,
} elf_endianness;

typedef enum elf_type
{
	elf_relocatable = 1,
	elf_executeable = 2,
	elf_shared = 3,
	elf_core = 4,
} elf_type;

typedef enum elf_target_architecture
{
	elf_arm = 0x28,
} elf_target_architecture;

typedef struct elf_hdr
{
	u8 magic[4];
	u8 bitFormat;
	u8 endianness;
	u8 elfVersion;
	u8 os;
	u8 padding0[8];
	u8 type[2];
	u8 targetArchitecture[2];
	u8 version[4];
	u8 entryPoint[4];
	u8 programHeaderTableOffset[4];
	u8 sectionHeaderTableOffset[4];
	u8 flags[4];
	u8 headerSize[2];
	u8 programHeaderEntrySize[2];
	u8 programHeaderEntryCount[2];
	u8 sectionTableEntrySize[2];
	u8 sectionHeaderEntryCount[2];
	u8 sectionHeaderNameEntryIndex[2];
} elf_hdr;

/* taken from elf specs, will not follow global style */

/* Section header.  */
typedef struct elf_shdr
{
	u8 name[4];		/* Section name (string tbl index) */
	u8 type[4];		/* Section type */
	u8 flags[4];		/* Section flags */
	u8 addr[4];		/* Section virtual addr at execution */
	u8 offset[4];		/* Section file offset */
	u8 size[4];		/* Section size in bytes */
	u8 link[4];		/* Link to another section */
	u8 info[4];		/* Additional section information */
	u8 addralign[4];		/* Section alignment */
	u8 entsize[4];		/* Entry size if section holds table */
} elf_shdr;

/* Program segment header.  */
typedef struct elf_phdr
{
	u8 type[4];			/* Segment type */
	u8 offset[4];		/* Segment file offset */
	u8 vaddr[4];		/* Segment virtual address */
	u8 paddr[4];		/* Segment physical address */
	u8 filesz[4];		/* Segment size in file */
	u8 memsz[4];		/* Segment size in memory */
	u8 flags[4];		/* Segment flags */
	u8 align[4];		/* Segment alignment */
} elf_phdr;

// ELF Functions

int elf_ProcessHeader(elf_context *elf)
{
	const elf_hdr *hdr = (const elf_hdr*)elf->file;

	/* Check conditions for valid CTR ELF */
	if (u8_to_u32(hdr->magic, BE) != ELF_MAGIC)
		return NOT_ELF_FILE;
	if (hdr->bitFormat != elf_32_bit)
		return NOT_CTR_ARM_ELF;
	if (hdr->endianness != elf_little_endian)
		return NOT_CTR_ARM_ELF;
	if (u8_to_u16(hdr->targetArchitecture, LE) != elf_arm)
		return NOT_CTR_ARM_ELF;
	if (u8_to_u16(hdr->type, LE) != elf_executeable)
		return NON_EXECUTABLE_ELF;

	elf->phdrOffset = u8_to_u32(hdr->programHeaderTableOffset, LE);
	elf->segmentNum = u8_to_u16(hdr->programHeaderEntryCount, LE);
	elf->segments = calloc(elf->segmentNum, sizeof(elf_segment));
	if (!elf->segments) {
		fprintf(stderr, "[ELF ERROR] Not enough memory\n");
		return MEM_ERROR;
	}

	elf->shdrOffset = u8_to_u32(hdr->sectionHeaderTableOffset, LE);
	elf->shdrNameIndex = u8_to_u16(hdr->sectionHeaderNameEntryIndex, LE);
	elf->sectionNum = u8_to_u16(hdr->sectionHeaderEntryCount, LE);
	elf->sections = calloc(elf->sectionNum, sizeof(elf_section));
	if (!elf->sections) {
		fprintf(stderr, "[ELF ERROR] Not enough memory\n");
		return MEM_ERROR;
	}

	return 0;
}

void elf_PopulateSections(elf_context *elf)
{
	const elf_shdr *shdr = (const elf_shdr *)(elf->file + elf->shdrOffset);
	const char *nameTable = (const char*)(elf->file + u8_to_u32(shdr[elf->shdrNameIndex].offset, LE));

	for (int i = 0; i < elf->sectionNum; i++) {
		elf->sections[i].name = nameTable + u8_to_u32(shdr[i].name, LE);
		elf->sections[i].type = u8_to_u32(shdr[i].type, LE);
		elf->sections[i].flags = u8_to_u32(shdr[i].flags, LE);
		elf->sections[i].fileOffset = u8_to_u32(shdr[i].offset, LE);
		elf->sections[i].size = u8_to_u32(shdr[i].size, LE);
		elf->sections[i].ptr = elf->file + elf->sections[i].fileOffset;
		elf->sections[i].vAddr = u8_to_u32(shdr[i].addr, LE);
		elf->sections[i].alignment = u8_to_u32(shdr[i].addralign, LE);
	}
}

void elf_PopulateSegments(elf_context *elf)
{
	const elf_phdr *phdr = (const elf_phdr *)(elf->file + elf->phdrOffset);

	for (int i = 0; i < elf->segmentNum; i++) {
		elf->segments[i].type = u8_to_u32(phdr[i].type, LE);
		elf->segments[i].flags = u8_to_u32(phdr[i].flags, LE);
		elf->segments[i].fileOffset = u8_to_u32(phdr[i].offset, LE);
		elf->segments[i].fileSize = u8_to_u32(phdr[i].filesz, LE);
		elf->segments[i].ptr = elf->file + elf->segments[i].fileOffset;
		elf->segments[i].pAddr = u8_to_u32(phdr[i].paddr, LE);
		elf->segments[i].vAddr = u8_to_u32(phdr[i].vaddr, LE);
		elf->segments[i].memSize = u8_to_u32(phdr[i].memsz, LE);
		elf->segments[i].alignment = u8_to_u32(phdr[i].align, LE);
	}
}

int elf_Init(elf_context *elf, const u8 *elfFile)
{
	elf->file = elfFile;
	
	int result;

	if((result = elf_ProcessHeader(elf))) return result;

	elf_PopulateSections(elf);
	elf_PopulateSegments(elf);
	return 0;
}

void elf_Free(elf_context *elf)
{
	free(elf->sections);
	free(elf->segments);
	memset(elf, 0, sizeof(elf_context));
}

u16 elf_SectionNum(elf_context *ctx)
{
	return ctx->sectionNum;
}

const elf_section* elf_GetSections(elf_context *ctx)
{
	return ctx->sections;
}

u16 elf_SegmentNum(elf_context *ctx) 
{
	return ctx->segmentNum;
}

const elf_segment* elf_GetSegments(elf_context *ctx)
{
	return ctx->segments;
}
