/* ncch_flags.h - Open-source replacement for the closed-source decrypt.exe
 * step in the original Batch CIA 3DS Decryptor toolchain.
 *
 * decrypt.exe's job is to mark an extracted-and-decrypted NCCH partition as
 * "not encrypted" so 3DS emulators (Citra/Lime3DS/Azahar) can install it
 * without keys. The mark lives in a single byte of the NCCH header. This
 * file provides a minimal patch primitive that rewrites that byte.
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef CIA3DS_NCCH_FLAGS_H
#define CIA3DS_NCCH_FLAGS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Returns 0 on success, non-zero (an errno) on failure. */
int ncch_flags_mark_decrypted(const char *path);

#ifdef __cplusplus
}
#endif

#endif /* CIA3DS_NCCH_FLAGS_H */
