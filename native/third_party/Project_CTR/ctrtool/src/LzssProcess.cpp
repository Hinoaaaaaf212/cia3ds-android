#include "LzssProcess.h"
#include "lzss.h"

#include <tc/io.h>

ctrtool::LzssProcess::LzssProcess() :
	mModuleLabel("ctrtool::LzssProcess"),
	mInputStream(),
	mExtractPath()
{
}

void ctrtool::LzssProcess::setInputStream(const std::shared_ptr<tc::io::IStream>& input_stream)
{
	mInputStream = input_stream;
}

void ctrtool::LzssProcess::setExtractPath(const tc::io::Path& extract_path)
{
	mExtractPath = extract_path;
}

void ctrtool::LzssProcess::process()
{
	if (mExtractPath.isSet())
	{
		// read input file into memory
		// check if input file is too large
		if (tc::is_int64_t_too_large_for_size_t(mInputStream->length()))
		{
			throw tc::InvalidOperationException(mModuleLabel, "Input file is too large.");
		}

		// allocate memory for file
		auto compressed_data = tc::ByteData((size_t)mInputStream->length());

		// read file
		mInputStream->seek(0, tc::io::SeekOrigin::Begin);
		mInputStream->read(compressed_data.data(), compressed_data.size());

		// decompress
		tc::ByteData decompressed_data = tc::ByteData(lzss_get_decompressed_size(compressed_data.data(), compressed_data.size()));
		lzss_decompress(compressed_data.data(), compressed_data.size(), decompressed_data.data(), decompressed_data.size());

		// open output file
		tc::io::LocalFileSystem local_fs;
		std::shared_ptr<tc::io::IStream> output_stream;
		local_fs.openFile(mExtractPath.get(), tc::io::FileMode::OpenOrCreate, tc::io::FileAccess::Write, output_stream);

		// write decompressed data
		output_stream->seek(0, tc::io::SeekOrigin::Begin);
		output_stream->write(decompressed_data.data(), decompressed_data.size());
	}
}
