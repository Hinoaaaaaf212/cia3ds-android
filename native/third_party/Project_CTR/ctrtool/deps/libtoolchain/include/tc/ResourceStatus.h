	/**
	 * @file ResourceStatus.h
	 * @brief Declaration of tc::ResourceStatus
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2019/01/16
	 **/
#pragma once
#include <bitset>

namespace tc {

	/**
	 * @enum ResourceStatusFlag
	 * @brief Flags for ResourceStatus
	 **/
enum ResourceStatusFlag
{
	RESFLAG_READY, /**< Resource is ready for use */
	RESFLAG_ERROR, /**< Resource encountered an error */
	RESFLAG_NOINIT, /**< Resource is not initialized */
};

	/**
	 * @brief Bitset indicating resource state information (see @ref ResourceStatusFlag)
	 **/
using ResourceStatus = std::bitset<32>; 

} // namespace tc