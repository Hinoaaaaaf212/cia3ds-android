#include <tc/io/OverlayedSource.h>
#include <tc/io/IOUtil.h>

const std::string tc::io::OverlayedSource::kClassName = "tc::io::OverlayedSource";

tc::io::OverlayedSource::OverlayedSource() :
	mBaseSource(),
	mOverlaySourceInfos()
{

}

tc::io::OverlayedSource::OverlayedSource(const std::shared_ptr<tc::io::ISource>& base_source, const std::shared_ptr<tc::io::ISource>& overlay_source, int64_t offset, int64_t length) :
	OverlayedSource(base_source, {OverlaySourceInfo{overlay_source, offset, length}})
{

}

tc::io::OverlayedSource::OverlayedSource(const std::shared_ptr<tc::io::ISource>& base_source, const std::vector<OverlaySourceInfo>& overlay_source_infos)
{
	// throw exception if the base source is null
	if (base_source == nullptr)
	{
		throw tc::ArgumentNullException(kClassName+"::OverlayedSource()", "base_source was null.");
	}
		
	// copy base source ptr
	mBaseSource = base_source;

	// check/import overlay sources
	for (auto itr = overlay_source_infos.begin(); itr != overlay_source_infos.end(); itr++)
	{
		// skip regions with no length
		if (itr->length == 0)
		{
			continue;
		}

		// throw exception if a overlay source is null
		if (itr->overlay_source == nullptr)
		{
			throw tc::ArgumentNullException(kClassName+"::OverlayedSource()", "overlay_source was null.");
		}

		// throw exception if overly region offset is negative
		if (itr->offset < 0)
		{
			throw tc::ArgumentOutOfRangeException(kClassName+"::OverlayedSource()", "Invalid overlay region. Overlay offset is negative.");
		}

		// throw exception if the overlay region offset is beyond the length of the base source
		if (itr->offset > mBaseSource->length())
		{
			throw tc::ArgumentOutOfRangeException(kClassName+"::OverlayedSource()", "Invalid overlay region. Overlay offset beyond length of base_source.");
		}

		// throw exception if the overlay region exceeds the length of the base source
		if ((itr->offset + itr->length) > mBaseSource->length())
		{
			throw tc::ArgumentOutOfRangeException(kClassName+"::OverlayedSource()", "Invalid overlay region. Overlay region exceeds the length of base_source.");
		}

		// throw exception if the overlay region exceeds the length of the overlay source
		if (itr->length > itr->overlay_source->length())
		{
			throw tc::ArgumentOutOfRangeException(kClassName+"::OverlayedSource()", "Invalid overlay region. Overlay region exceeds the length of overlay_source.");
		}

		// save overlay source info
		mOverlaySourceInfos.push_back(*itr);
	}
}

int64_t tc::io::OverlayedSource::length()
{
	// return 0 if mBaseSource is null, otherwise deref the pointer and get the length
	return mBaseSource == nullptr ? 0 : mBaseSource->length();
}

tc::ByteData tc::io::OverlayedSource::pullData(int64_t offset, size_t count)
{
	// return empty byte_data if the base is empty
	if (mBaseSource == nullptr)
		return tc::ByteData();

	size_t read_len = IOUtil::getReadableCount(this->length(), offset, count);

	// if the read length is zero then return now.
	if (read_len == 0)
		return tc::ByteData();

	// get base source byte_data, this will be overwritten with regions 
	tc::ByteData out = mBaseSource->pullData(offset, count);

	// iterate thru the overlays
	for (auto itr = mOverlaySourceInfos.begin(); itr != mOverlaySourceInfos.end(); itr++)
	{
		int64_t overlay_pull_offset = 0;
		size_t overlay_pull_count = 0;

		// skip overlay if the pullable count is 0
		getOverlaySourcePullableRegion(offset, count, *itr, overlay_pull_offset, overlay_pull_count);
		if (overlay_pull_count == 0)
		{
			continue;
		}

		// pull data from overlay
		ByteData overlay_pull = itr->overlay_source->pullData(overlay_pull_offset, overlay_pull_count);

		// adjust the outsize to be the minimum of the ByteData & attempted pull_count
		overlay_pull_count = std::min<size_t>(overlay_pull.size(), overlay_pull_count);
		
		// copy into out buffer
		int64_t overlay_offset_in_out = (overlay_pull_offset + itr->offset) - offset;
		memcpy(out.data() + overlay_offset_in_out, overlay_pull.data(), overlay_pull_count);
	}

	return out;
}

void tc::io::OverlayedSource::getOverlaySourcePullableRegion(int64_t base_pull_offset, size_t base_pull_count, const OverlaySourceInfo& overlay_info, int64_t& overlay_pull_offset, size_t& overlay_pull_count)
{
	int64_t overlay_relative_start_offset = base_pull_offset - overlay_info.offset;
	int64_t overlay_relative_end_offset = overlay_relative_start_offset + int64_t(base_pull_count);

	// if the start offset > overlay length: then the data starts after the overlay ends
	// if the end offset < 0: then the data ends before the overlay begins
	if (overlay_relative_start_offset > overlay_info.length || overlay_relative_end_offset < 0)
	{
		overlay_pull_offset = 0;
		overlay_pull_count = 0;
	}
	// otherwise some or all of the overlay can be used
	else
	{
		// the offset must be reset to zero if it is negative
		overlay_pull_offset = overlay_relative_start_offset > 0 ? overlay_relative_start_offset : 0;

		// getReadableSize will cap the amount read if it exceeds the base_length
		overlay_pull_count = IOUtil::getReadableCount(overlay_info.length, overlay_pull_offset, size_t(overlay_relative_end_offset - overlay_pull_offset));
	}
}