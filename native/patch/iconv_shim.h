/* Android bionic has no iconv; all paths are UTF-8, so conversions are no-ops. */
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
