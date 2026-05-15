#include <stdlib.h>
#ifndef _WIN32
#ifndef __CYGWIN__
#define LIBICONV_PLUG
#endif
#include <iconv.h>
#endif
#include "oschar.h"

int os_fstat(const oschar_t *path)
{
	struct _osstat st;
	return os_stat(path, &st);
}

uint64_t os_fsize(const oschar_t *path)
{
	struct _osstat st;
	if (os_stat(path, &st) != 0)
		return 0;
	else
		return st.st_size;
}

int os_makedir(const oschar_t *dir)
{
#ifdef _WIN32
	return _wmkdir(dir);
#else
	return mkdir(dir, 0777);
#endif
}

uint32_t utf16_strlen(const utf16char_t *str)
{
	uint32_t i;
	for (i = 0; str[i] != 0x0; i++);
	return i;
}

void utf16_fputs(const utf16char_t *str, FILE *out)
{
	oschar_t *_str = os_CopyConvertUTF16Str(str);
	os_fputs(_str, out);
	free(_str);
}

char* strcopy_8to8(const char *src)
{
	uint32_t src_len;
	char *dst;

	if (!src)
		return NULL;

	src_len = strlen(src);

	// Allocate memory for expanded string
	dst = calloc(src_len + 1, sizeof(char));
	if (!dst)
		return NULL;

	// Copy elements from src into dst
	strncpy(dst, src, src_len);

	return dst;
}

utf16char_t* strcopy_8to16(const char *src)
{
	uint32_t src_len, i;
	utf16char_t *dst;

	if (!src)
		return NULL;

	src_len = strlen(src);

	// Allocate memory for expanded string
	dst = calloc(src_len + 1, sizeof(utf16char_t));
	if (!dst)
		return NULL;

	// Copy elements from src into dst
	for (i = 0; i < src_len; i++)
		dst[i] = src[i];

	return dst;
}


utf16char_t* strcopy_16to16(const utf16char_t *src)
{
	uint32_t src_len, i;
	utf16char_t *dst;

	if (!src)
		return NULL;

	src_len = utf16_strlen(src);

	// Allocate memory for expanded string
	dst = calloc(src_len + 1, sizeof(utf16char_t));
	if (!dst)
		return NULL;

	// Copy elements from src into dst
	for (i = 0; i < src_len; i++)
		dst[i] = src[i];

	return dst;
}

#ifndef _WIN32
utf16char_t* strcopy_UTF8toUTF16(const char *src)
{
	uint32_t src_len, dst_len;
	size_t in_bytes, out_bytes;
	utf16char_t *dst;
	char *in, *out;

	if (!src)
		return NULL;

	src_len = strlen(src);
	dst_len = src_len + 1;

	// Allocate memory for string
	dst = calloc(dst_len, sizeof(utf16char_t));
	if (!dst)
		return NULL;

	in = (char*)src;
	out = (char*)dst;
	in_bytes = src_len*sizeof(char);
	out_bytes = dst_len*sizeof(utf16char_t);

	iconv_t cd = iconv_open("UTF-16LE", "UTF-8");
	iconv(cd, &in, &in_bytes, &out, &out_bytes);
	iconv_close(cd);
	return dst;
}

char* strcopy_UTF16toUTF8(const utf16char_t *src)
{
	uint32_t src_len, dst_len;
	size_t in_bytes, out_bytes;
	char *dst;
	char *in, *out;

	if (!src)
		return NULL;

	src_len = utf16_strlen(src);
	// UTF-8 can use up to 3 bytes per UTF-16 code unit, or four for a surrogate pair
	dst_len = src_len * 3;

	// Allocate memory for string
	dst = calloc(dst_len, sizeof(char));
	if (!dst)
		return NULL;

	in = (char*)src;
	out = (char*)dst;
	in_bytes = src_len*sizeof(uint16_t);
	out_bytes = dst_len*sizeof(char);

	iconv_t cd = iconv_open("UTF-8", "UTF-16LE");
	iconv(cd, &in, &in_bytes, &out, &out_bytes);
	iconv_close(cd);
	return dst;
}
#endif

oschar_t* os_AppendToPath(const oschar_t *src, const oschar_t *add)
{
	uint32_t len;
	oschar_t *new_path;

	len = os_strlen(src) + os_strlen(add) + 0x10;
	new_path = calloc(len, sizeof(oschar_t));

#ifdef _WIN32
	_snwprintf(new_path, len, L"%s%c%s", src, OS_PATH_SEPARATOR, add);
#else
	snprintf(new_path, len, "%s%c%s", src, OS_PATH_SEPARATOR, add);
#endif

	return new_path;
}

oschar_t* os_AppendUTF16StrToPath(const oschar_t *src, const utf16char_t *add)
{
	uint32_t len;
	oschar_t *new_path, *_add;

	_add = os_CopyConvertUTF16Str(add);

	len = os_strlen(src) + os_strlen(_add) + 0x10;
	new_path = calloc(len, sizeof(oschar_t));

#ifdef _WIN32
	_snwprintf(new_path, len, L"%s%c%s", src, OS_PATH_SEPARATOR, _add);
#else
	snprintf(new_path, len, "%s%c%s", src, OS_PATH_SEPARATOR, _add);
#endif

	free(_add);
	return new_path;
}
