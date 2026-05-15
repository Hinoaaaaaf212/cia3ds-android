#pragma once
#include "types.h"
#include <tc/io.h>

namespace ctrtool
{

void writeSubStreamToFile(const std::shared_ptr<tc::io::IStream>& in_stream, int64_t offset, int64_t length, const tc::io::Path& out_path, tc::ByteData& cache);
void writeSubStreamToFile(const std::shared_ptr<tc::io::IStream>& in_stream, int64_t offset, int64_t length, const tc::io::Path& out_path, size_t cache_size = 0x10000);
void writeStreamToFile(const std::shared_ptr<tc::io::IStream>& in_stream, const tc::io::Path& out_path, tc::ByteData& cache);
void writeStreamToFile(const std::shared_ptr<tc::io::IStream>& in_stream, const tc::io::Path& out_path, size_t cache_size = 0x10000);
void writeStreamToStream(const std::shared_ptr<tc::io::IStream>& in_stream, const std::shared_ptr<tc::io::IStream>& out_stream, tc::ByteData& cache);
void writeStreamToStream(const std::shared_ptr<tc::io::IStream>& in_stream, const std::shared_ptr<tc::io::IStream>& out_stream, size_t cache_size = 0x10000);

}