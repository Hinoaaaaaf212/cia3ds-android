#include "util.h"

#include <tc/io/FileStream.h>
#include <tc/io/SubStream.h>
#include <tc/io/IOUtil.h>

void ctrtool::writeSubStreamToFile(const std::shared_ptr<tc::io::IStream>& in_stream, int64_t offset, int64_t length, const tc::io::Path& out_path, tc::ByteData& cache)
{
	writeStreamToStream(std::make_shared<tc::io::SubStream>(tc::io::SubStream(in_stream, offset, length)), std::make_shared<tc::io::FileStream>(tc::io::FileStream(out_path, tc::io::FileMode::Create, tc::io::FileAccess::Write)), cache);
}

void ctrtool::writeSubStreamToFile(const std::shared_ptr<tc::io::IStream>& in_stream, int64_t offset, int64_t length, const tc::io::Path& out_path, size_t cache_size)
{
	writeStreamToStream(std::make_shared<tc::io::SubStream>(tc::io::SubStream(in_stream, offset, length)), std::make_shared<tc::io::FileStream>(tc::io::FileStream(out_path, tc::io::FileMode::Create, tc::io::FileAccess::Write)), cache_size);
}

void ctrtool::writeStreamToFile(const std::shared_ptr<tc::io::IStream>& in_stream, const tc::io::Path& out_path, tc::ByteData& cache)
{
	writeStreamToStream(in_stream, std::make_shared<tc::io::FileStream>(tc::io::FileStream(out_path, tc::io::FileMode::Create, tc::io::FileAccess::Write)), cache);
}

void ctrtool::writeStreamToFile(const std::shared_ptr<tc::io::IStream>& in_stream, const tc::io::Path& out_path, size_t cache_size)
{
	writeStreamToStream(in_stream, std::make_shared<tc::io::FileStream>(tc::io::FileStream(out_path, tc::io::FileMode::Create, tc::io::FileAccess::Write)), cache_size);
}

void ctrtool::writeStreamToStream(const std::shared_ptr<tc::io::IStream>& in_stream, const std::shared_ptr<tc::io::IStream>& out_stream, tc::ByteData& cache)
{
	// iterate thru child files
	size_t cache_read_len;
	
	in_stream->seek(0, tc::io::SeekOrigin::Begin);
	out_stream->seek(0, tc::io::SeekOrigin::Begin);
	for (int64_t remaining_data = in_stream->length(); remaining_data > 0;)
	{
		cache_read_len = in_stream->read(cache.data(), cache.size());
		if (cache_read_len == 0)
		{
			throw tc::io::IOException("ctrtool::writeStreamToStream()", "Failed to read from source streeam.");
		}

		out_stream->write(cache.data(), cache_read_len);

		remaining_data -= int64_t(cache_read_len);
	}
}

void ctrtool::writeStreamToStream(const std::shared_ptr<tc::io::IStream>& in_stream, const std::shared_ptr<tc::io::IStream>& out_stream, size_t cache_size)
{
	tc::ByteData cache = tc::ByteData(cache_size);
	writeStreamToStream(in_stream, out_stream, cache);
}