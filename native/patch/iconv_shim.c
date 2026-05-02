/* Android bionic has no iconv; since the host encoding is already UTF-8, this is a byte-copy passthrough. */
#include "iconv_shim.h"

#include <errno.h>
#include <string.h>

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
