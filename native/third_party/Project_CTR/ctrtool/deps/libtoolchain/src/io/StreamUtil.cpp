#include <tc/io/StreamUtil.h>

int64_t tc::io::StreamUtil::getSeekResult(int64_t offset, tc::io::SeekOrigin origin, int64_t current_position, int64_t stream_length)
{
	int64_t new_pos = 0;
	switch (origin)
	{
		case (SeekOrigin::Begin):
			new_pos = offset;
			break;
		case (SeekOrigin::Current):
			new_pos = current_position + offset;
			break;
		case (SeekOrigin::End):
			new_pos = stream_length + offset;
			break;
		default:
			throw tc::ArgumentOutOfRangeException("Illegal value for origin.");
	}

	return new_pos;
}