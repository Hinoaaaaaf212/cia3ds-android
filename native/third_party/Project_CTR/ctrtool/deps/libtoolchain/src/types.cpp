#include <tc/types.h>

bool tc::is_size_t_not_64bit()
{
	return uint64_t(std::numeric_limits<size_t>::max()) < std::numeric_limits<uint64_t>::max();
}

bool tc::is_size_t_too_large_for_int64_t(size_t val)
{
	return uint64_t(std::numeric_limits<int64_t>::max()) < uint64_t(val);
}

bool tc::is_uint64_t_too_large_for_int64_t(uint64_t val)
{
	return uint64_t(std::numeric_limits<int64_t>::max()) < val;
}

bool tc::is_int64_t_too_large_for_size_t(int64_t val)
{
	return uint64_t(std::numeric_limits<size_t>::max()) < uint64_t(val) || val < 0;
}

bool tc::is_uint64_t_too_large_for_size_t(uint64_t val)
{
	return uint64_t(std::numeric_limits<size_t>::max()) < val;
}