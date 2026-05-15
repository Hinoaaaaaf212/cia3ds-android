	/**
	 * @file OverlayedSource.h
	 * @brief Declaration of tc::io::OverlayedSource
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/03/21
	 **/
#pragma once
#include <tc/io/ISource.h>
#include <vector>

#include <tc/ArgumentNullException.h>
#include <tc/ArgumentOutOfRangeException.h>

namespace tc { namespace io {

	/**
	 * @class OverlayedSource
	 * @brief This will replaces regions within a base source with one or more other sources, so as to override those regions in the base source when pullData() is called. 
	 **/
class OverlayedSource : public tc::io::ISource
{
public:

		/**
		 * @struct OverlaySourceInfo
		 * @brief This contains information about an overlay region. Used with @ref OverlayedSource::OverlayedSource()
		 **/
	struct OverlaySourceInfo
	{
			/// ISource to overlay with
		std::shared_ptr<tc::io::ISource> overlay_source;
			/// Offset in base source to overlay
		int64_t offset;
			/// Length of region in base source to overlay
		int64_t length;
	};

		/**
		 * @brief Default constructor
		 * @post This will create a OverlayedSource with length() == 0.
		 **/ 
	OverlayedSource();


		/**
		 * @brief Overlay base source with one source
		 * 
		 * @param[in] base_source Base source to be overlayed.
		 * @param[in] overlay_source Source to overlay onto the base source.
		 * @param[in] offset Offset in base source to overlay
		 * @param[in] length Length in base source to overlay
		 * 
		 * @throw tc::ArgumentNullException @p base_source or @p overlay_source was null.
		 * @throw tc::ArgumentOutOfRangeException @p length was greater than the length of @p overlay_source .
		 * @throw tc::ArgumentOutOfRangeException The overlay region offset is negative or the total size exceeded the length of @p base_source .
		 **/
	OverlayedSource(const std::shared_ptr<tc::io::ISource>& base_source, const std::shared_ptr<tc::io::ISource>& overlay_source, int64_t offset, int64_t length);


		/**
		 * @brief Overlay base source with multiple sources
		 * 
		 * @param[in] base_source Base source to be overlayed.
		 * @param[in] overlay_source_infos Vector of sources to overlay onto the base source. @ref OverlaySourceInfo
		 * 
		 * @throw tc::ArgumentNullException @p base_source or one of the overlay sources was null.
		 * @throw tc::ArgumentOutOfRangeException An overlay source was smaller than the region in the base source it was supposed to overlay.
		 * @throw tc::ArgumentOutOfRangeException A region to overlay in the base source either partly or entirely does not exist.
		 **/
	OverlayedSource(const std::shared_ptr<tc::io::ISource>& base_source, const std::vector<OverlaySourceInfo>& overlay_source_infos);

		/**
		 * @brief Gets the length of the source.
		 **/
	int64_t length();

		/**
		 * @brief Pull data from source
		 * 
		 * @param[in] offset Zero-based offset in source to pull data.
		 * @param[in] count The maximum number of bytes to be pull from the source.
		 *
		 * @return ByteData containing data pulled from source.
		 **/
	tc::ByteData pullData(int64_t offset, size_t count);
private:
	static const std::string kClassName;

	std::shared_ptr<tc::io::ISource> mBaseSource;
	std::vector<OverlaySourceInfo> mOverlaySourceInfos;

	void getOverlaySourcePullableRegion(int64_t base_pull_offset, size_t base_pull_count, const OverlaySourceInfo& overlay_info, int64_t& overlay_pull_offset, size_t& overlay_pull_count);
};

}} // namespace tc::io