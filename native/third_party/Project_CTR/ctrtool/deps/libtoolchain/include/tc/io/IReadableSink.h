	/**
	 * @file IReadableSink.h
	 * @brief Declaration of tc::io::IReadableSink
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2020/02/07
	 **/
#pragma once
#include <tc/io/ISink.h>
#include <tc/io/ISource.h>

namespace tc { namespace io {

	/**
	 * @class IReadableSink
	 * @brief An interface defining a byte data sink that can be also provide an ISource.
	 **/
class IReadableSink : public tc::io::ISink
{
public:
		/// Destructor
	virtual ~IReadableSink() = default;

		/// Convert to ISource
	virtual std::shared_ptr<tc::io::ISource>& toSource() = 0;
};

}} // namespace tc::io