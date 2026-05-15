#pragma once
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <wchar.h>
#endif

// Nintendo uses UTF16-LE chars for extended ASCII support
typedef uint16_t utf16char_t;

// Native OS char type for unicode support
#ifdef _WIN32
typedef wchar_t oschar_t; // UTF16-LE
#else
typedef char oschar_t; // UTF8
#endif

// Simple redirect macros for functions and types
#ifdef _WIN32
#define os_strlen wcslen
#define os_strcmp wcscmp
#define os_fputs fputws

#define os_CopyStr strcopy_16to16
#define os_CopyConvertCharStr strcopy_8to16
#define os_CopyConvertUTF16Str strcopy_16to16
#define utf16_CopyStr strcopy_16to16
#define utf16_CopyConvertOsStr strcopy_16to16

#define _osdirent _wdirent
#define _OSDIR _WDIR
#define os_readdir _wreaddir
#define os_opendir _wopendir
#define os_closedir _wclosedir
#define os_chdir _wchdir

#define _osstat _stat64
#define os_stat _wstat64

#define os_fopen _wfopen
#define OS_MODE_READ L"rb"
#define OS_MODE_WRITE L"wb"
#define OS_MODE_EDIT L"rb+"
#define OS_PATH_SEPARATOR '\\'
#else
#define os_strlen strlen
#define os_strcmp strcmp
#define os_fputs fputs

#define os_CopyStr strcopy_8to8
#define os_CopyConvertUTF16Str strcopy_UTF16toUTF8
#define os_CopyConvertCharStr strcopy_8to8
#define utf16_CopyStr strcopy_16to16
#define utf16_CopyConvertOsStr strcopy_UTF8toUTF16

#define _osdirent dirent
#define _OSDIR DIR
#define os_readdir readdir
#define os_opendir opendir
#define os_closedir closedir
#define os_chdir chdir

#define _osstat stat
#define os_stat stat

#define os_fopen fopen
#define OS_MODE_READ "rb"
#define OS_MODE_WRITE "wb"
#define OS_MODE_EDIT "rb+"
#define OS_PATH_SEPARATOR '/'
#endif

/* File related */
int os_fstat(const oschar_t* path);
uint64_t os_fsize(const oschar_t* path);
int os_makedir(const oschar_t *dir);

/* UTF16 String property functions */
uint32_t utf16_strlen(const utf16char_t* str);
void utf16_fputs(const utf16char_t *str, FILE *out);

/* String Copy and Conversion */
char* strcopy_8to8(const char *src);
utf16char_t* strcopy_8to16(const char *src);
utf16char_t* strcopy_16to16(const utf16char_t *src);
#ifndef _WIN32
utf16char_t* strcopy_UTF8toUTF16(const char *src);
char* strcopy_UTF16toUTF8(const utf16char_t *src);
#endif

/* String Append and Create */
oschar_t* os_AppendToPath(const oschar_t *src, const oschar_t *add);
oschar_t* os_AppendUTF16StrToPath(const oschar_t *src, const utf16char_t *add);