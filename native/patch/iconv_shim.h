/* iconv_shim.h - Android bionic has no iconv. Since both Android and the
 * 3DS NCCH/CIA tooling treat all paths as UTF-8, conversions are no-ops.
 *
 * This header is included automatically when building makerom on Android via
 * the CIA3DS_BUILD compile definition. It provides drop-in replacements for
 * the four iconv functions that makerom/src/oschar.c uses.
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef CIA3DS_ICONV_SHIM_H
#define CIA3DS_ICONV_SHIM_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *iconv_t;

iconv_t iconv_open(const char *tocode, const char *fromcode);
int iconv_close(iconv_t cd);
size_t iconv(iconv_t cd, char **inbuf, size_t *inbytesleft,
             char **outbuf, size_t *outbytesleft);

#ifdef __cplusplus
}
#endif

#endif /* CIA3DS_ICONV_SHIM_H */
