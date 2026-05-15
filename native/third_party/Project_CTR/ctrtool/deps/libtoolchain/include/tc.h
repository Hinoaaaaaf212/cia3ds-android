	/**
	 * @file		tc.h
	 * @brief       Declaration of the libtoolchain namespace
	 **/
#pragma once
#include <tc/types.h>

	/**
	 * @namespace   tc
	 * @brief       Root namespace for libtoolchain
	 **/
// classes
#include <tc/ByteData.h>
#include <tc/Optional.h>

// sub namespaces
#include <tc/string.h>
#include <tc/io.h>
#include <tc/os.h>
#include <tc/crypto.h>
#include <tc/cli.h>
#include <tc/bn.h>

// exceptions
#include <tc/Exception.h>
#include <tc/AccessViolationException.h>
#include <tc/ArgumentException.h>
#include <tc/ArgumentNullException.h>
#include <tc/ArgumentOutOfRangeException.h>
#include <tc/ArithmeticException.h>
#include <tc/InvalidOperationException.h>
#include <tc/NotImplementedException.h>
#include <tc/NotSupportedException.h>
#include <tc/ObjectDisposedException.h>
#include <tc/OutOfMemoryException.h>
#include <tc/OverflowException.h>
#include <tc/UnauthorisedAccessException.h>