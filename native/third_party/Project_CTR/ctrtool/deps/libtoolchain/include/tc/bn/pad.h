	/**
	 * @file pad.h
	 * @brief Declaration of tc::bn::pad
	 * @author Jack (jakcron)
	 * @version 0.2
	 * @date 2022/02/05
	 */
#pragma once
#include <tc/types.h>

namespace tc { namespace bn {

	/**
	 * @class pad
	 * @brief This class creates padding.
	 * 
	 * @tparam T size in bytes of the padding.
	 */ 
template <size_t T>
class pad 
{
public:
		/// Returns size of padding in bytes
	size_t size() const { return mArray.size(); }
private:
	std::array<uint8_t, T> mArray;
};

}} // namespace tc::bn