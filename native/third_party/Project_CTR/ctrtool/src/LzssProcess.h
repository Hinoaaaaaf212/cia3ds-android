#pragma once
#include "types.h"
#include <tc/Optional.h>
#include <tc/io/Path.h>
#include <tc/io/IStream.h>

namespace ctrtool {

class LzssProcess
{
public:
	LzssProcess();

	void setInputStream(const std::shared_ptr<tc::io::IStream>& input_stream);
	void setExtractPath(const tc::io::Path& extract_path);

	void process();
private:
	std::string mModuleLabel;

	std::shared_ptr<tc::io::IStream> mInputStream;
	tc::Optional<tc::io::Path> mExtractPath;
};

}