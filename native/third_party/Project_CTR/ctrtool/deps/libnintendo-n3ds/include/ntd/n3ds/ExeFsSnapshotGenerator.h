#pragma once
#include <tc/io/VirtualFileSystem.h>

namespace ntd { namespace n3ds {

struct ExeFsSnapshotGenerator : public tc::io::VirtualFileSystem::FileSystemSnapshot
{
public:
	ExeFsSnapshotGenerator(const std::shared_ptr<tc::io::IStream>& stream, bool verify_hashes = true);
private:
	ExeFsSnapshotGenerator();
};

}} // namespace ntd::n3ds