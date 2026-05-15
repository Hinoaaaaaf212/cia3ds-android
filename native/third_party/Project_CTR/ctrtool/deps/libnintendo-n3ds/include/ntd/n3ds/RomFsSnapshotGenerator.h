#pragma once
#include <tc/ByteData.h>
#include <tc/io/VirtualFileSystem.h>
#include <ntd/n3ds/romfs.h>

namespace ntd { namespace n3ds {

struct RomFsSnapshotGenerator : public tc::io::VirtualFileSystem::FileSystemSnapshot
{
public:
	RomFsSnapshotGenerator(const std::shared_ptr<tc::io::IStream>& stream);
private:
	RomFsSnapshotGenerator();

	std::shared_ptr<tc::io::IStream> mBaseStream;

	int64_t mDataOffset;

	tc::ByteData mDirEntryTable;
	std::map<uint32_t, size_t> mDirParentVaddrMap;
	inline ntd::n3ds::RomFsDirectoryEntry* getDirEntry(uint32_t vaddr) { return (ntd::n3ds::RomFsDirectoryEntry*)(mDirEntryTable.data() + vaddr); }

	tc::ByteData mFileEntryTable;
	inline ntd::n3ds::RomFsFileEntry* getFileEntry(uint32_t vaddr) { return (ntd::n3ds::RomFsFileEntry*)(mFileEntryTable.data() + vaddr); }

	void addFile(const ntd::n3ds::RomFsFileEntry* file_entry, size_t parent_dir);
	void addDirectory(const ntd::n3ds::RomFsDirectoryEntry* dir_entry, size_t parent_dir);
};

}} // namespace ntd::n3ds