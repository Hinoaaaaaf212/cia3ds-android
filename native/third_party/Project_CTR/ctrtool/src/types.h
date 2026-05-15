#pragma once
#include <tc/types.h>
#include <fmt/core.h>


namespace ctrtool {

enum ValidState : byte_t
{
	Unchecked,
	Good,
	Fail
};

}