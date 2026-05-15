#include <tc/string/TranscodeUtil.h>
#include <tc/ArgumentException.h>

#include <tc/string/detail/utf8.h>
#include <tc/string/detail/utf16.h>

void tc::string::TranscodeUtil::UTF8ToUTF32(const std::string& src, std::u32string& dst)
{
	size_t done = 0;
	dst.clear();
	for (size_t i = 0; i < src.length(); i += done)
	{
		// get number of leading high bits in first byte
		uint8_t prefix = detail::get_utf8_prefix(src[i]);
		if (prefix == 1 || prefix > 4) // 1 is reserved for trailer bytes
		{
			throw tc::ArgumentException("not a UTF-8 string");
		}

		// if there are no prefix bits, this is ASCII
		if (prefix == 0)
		{
			dst.push_back(src[i]);
			done = 1;
		}
		// otherwise this is a multibyte character
		else
		{
			// there must be enough characters
			if ((i + prefix) > src.length())
			{
				throw tc::ArgumentException("not a UTF-8 string");
			}

			char32_t uni = detail::get_utf8_data(prefix, src[i]);

			for (uint8_t j = 1; j < prefix; j++)
			{
				if (detail::utf8_has_prefix(1, src[i + j]) == false)
				{
					throw tc::ArgumentException("not a UTF-8 string");
				}

				uni <<= 6;
				uni |= detail::get_utf8_data(1, src[i + j]);
			}

			if (uni >= detail::kUtf16HighSurrogateStart && uni <= detail::kUtf16LowSurrogateEnd)
			{
				throw tc::ArgumentException("not a UTF-8 string");
			}
				
			if (uni > detail::kUtf16EncodeMax)
			{
				throw tc::ArgumentException("not a UTF-8 string");
			}
				
			dst.push_back(uni);
			done = prefix;
		}

	}
}

void tc::string::TranscodeUtil::UTF16ToUTF32(const std::u16string& src, std::u32string& dst)
{
	size_t done = 0;
	dst.clear();
	for (size_t i = 0; i < src.length(); i+=done)
	{
		// this isn't a utf16 reserved character, so just add to unicode string
		if (src[i] < detail::kUtf16HighSurrogateStart || src[i] > detail::kUtf16LowSurrogateEnd)
		{
			dst.push_back(src[i]);
			done = 1;
		}
		// otherwise we need to decode it
		else
		{
			// check that the high surrogate char exists first 
			if (src[i] < detail::kUtf16HighSurrogateStart || src[i] > detail::kUtf16HighSurrogateEnd)
			{
				throw tc::ArgumentException("not a UTF-16 string");
			}
			// check that the low surrogate char exists next
			if (i >= src.length() - 1 || src[i + 1] < detail::kUtf16LowSurrogateStart || src[i + 1] > detail::kUtf16LowSurrogateEnd)
			{
				throw tc::ArgumentException("not a UTF-16 string");
			}

			char32_t uni = ((src[i] & detail::kUtf16SurrogateMask) << detail::kUtf16SurrogateBits) | (src[i + 1] & detail::kUtf16SurrogateMask) | 0x10000;

			dst.push_back(uni);
			done = 2;
		}
	}
}

void tc::string::TranscodeUtil::UTF32ToUTF8(const std::u32string& src, std::string& dst)
{
	dst.clear();
	for (size_t i = 0; i < src.length(); i++)
	{
		if (src[i] <= detail::kUtf8AsciiEnd)
		{
			dst.push_back((char)src[i]);
		}
		else if (src[i] <= detail::kUtf82ByteEnd)
		{
			dst.push_back(detail::make_utf8(2, (uint8_t)(src[i] >> 6)));
			dst.push_back(detail::make_utf8(1, (uint8_t)(src[i] >> 0)));
		}
		else if (src[i] <= detail::kUtf83ByteEnd)
		{
			dst.push_back(detail::make_utf8(3, (uint8_t)(src[i] >> 12)));
			dst.push_back(detail::make_utf8(1, (uint8_t)(src[i] >> 6)));
			dst.push_back(detail::make_utf8(1, (uint8_t)(src[i] >> 0)));
		}
		else if (src[i] <= detail::kUtf84ByteEnd)
		{
			dst.push_back(detail::make_utf8(4, (uint8_t)(src[i] >> 18)));
			dst.push_back(detail::make_utf8(1, (uint8_t)(src[i] >> 12)));
			dst.push_back(detail::make_utf8(1, (uint8_t)(src[i] >> 6)));
			dst.push_back(detail::make_utf8(1, (uint8_t)(src[i] >> 0)));
		}
		else
		{
			throw tc::ArgumentException("not a UTF-16 string");
		}
	}
}

void tc::string::TranscodeUtil::UTF32ToUTF16(const std::u32string& src, std::u16string& dst)
{
	dst.clear();
	for (size_t i = 0; i < src.size(); i++)
	{
		char32_t uni = src[i];
		if (uni < detail::kUtf16NonNativeStart)
		{
			dst.push_back((char16_t)uni);
		}
		else
		{
			uni -= detail::kUtf16NonNativeStart;
			dst.push_back(((uni >> detail::kUtf16SurrogateBits) & detail::kUtf16SurrogateMask) + detail::kUtf16HighSurrogateStart);
			dst.push_back((uni & detail::kUtf16SurrogateMask) + detail::kUtf16LowSurrogateStart);
		}
	}
}


void tc::string::TranscodeUtil::UTF8ToUTF16(const std::string& src, std::u16string& dst)
{
	std::u32string unicode;
	TranscodeUtil::UTF8ToUTF32(src, unicode);
	TranscodeUtil::UTF32ToUTF16(unicode, dst);
}

void tc::string::TranscodeUtil::UTF16ToUTF8(const std::u16string& src, std::string& dst)
{
	std::u32string unicode;
	TranscodeUtil::UTF16ToUTF32(src, unicode);
	TranscodeUtil::UTF32ToUTF8(unicode, dst);
}