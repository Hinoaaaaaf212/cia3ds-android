/* iconv_shim.c - UTF-8 passthrough implementation for Android bionic.
 *
 * makerom/src/oschar.c uses iconv() to convert between the host narrow
 * encoding and UTF-8. On Android the host narrow encoding *is* UTF-8, so
 * this implementation simply copies bytes verbatim and never reports a
 * conversion error.
 *
 * SPDX-License-Identifier: MIT
 */
#include "iconv_shim.h"

#include <errno.h>
#include <string.h>

/* A single non-NULL sentinel; iconv_open is allowed to return any non-(-1)
 * pointer for success. */
static int kIconvSentinel = 0;

iconv_t iconv_open(const char *tocode, const char *fromcode) {
    (void)tocode;
    (void)fromcode;
    return (iconv_t)&kIconvSentinel;
}

int iconv_close(iconv_t cd) {
    (void)cd;
    return 0;
}

size_t iconv(iconv_t cd, char **inbuf, size_t *inbytesleft,
             char **outbuf, size_t *outbytesleft) {
    (void)cd;
    if (!inbuf || !*inbuf || !inbytesleft) {
        return 0;
    }
    size_t to_copy = *inbytesleft;
    if (outbytesleft && to_copy > *outbytesleft) {
        errno = E2BIG;
        return (size_t)-1;
    }
    if (outbuf && *outbuf) {
        memcpy(*outbuf, *inbuf, to_copy);
        *outbuf += to_copy;
    }
    *inbuf += to_copy;
    *inbytesleft = 0;
    if (outbytesleft) {
        *outbytesleft -= to_copy;
    }
    return 0;
}
