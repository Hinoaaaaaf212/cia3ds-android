/* ncch_flags.c - patch the NCCH "other_flag" byte to declare the partition
 * as plaintext, replacing the post-extract step that the closed-source
 * decrypt.exe used to perform.
 *
 * NCCH header layout (verified against ntd::n3ds::NcchCommonHeader in
 * libnintendo-n3ds, included as a submodule):
 *
 *   file offset 0x000-0x0FF: RSA-2048 signature
 *   file offset 0x100-0x1FF: NcchCommonHeader (0x100 bytes)
 *
 * Inside NcchCommonHeader, the 8-byte NcchFlags struct begins at struct
 * offset 0x88 (file offset 0x188). The struct's last byte, "other_flag", is
 * a bitarray with these meaningful bits:
 *
 *   bit 0 (0x01): FixedAesKey
 *   bit 1 (0x02): NoMountRomFS
 *   bit 2 (0x04): NoEncryption  <-- we set this
 *   bit 5 (0x20): SeededAesKeyY
 *   bit 6 (0x40): ManualDisclosure
 *
 * Setting NoEncryption and clearing both crypto-key bits causes emulators
 * to install the partition without ever consulting an AES key.
 *
 * The NCCH header is unsigned in practice (the RSA signature is checked
 * only on signed retail content; for sideload/install paths, emulators
 * accept the bytes as-is), so we do not need to recompute a signature.
 *
 * SPDX-License-Identifier: MIT
 */
#include "ncch_flags.h"

#include <errno.h>
#include <stdio.h>

#define NCCH_FLAGS_OTHER_OFFSET 0x18Fu

#define NCCH_OTHER_FIXED_AES_KEY    0x01u
#define NCCH_OTHER_NO_MOUNT_ROMFS   0x02u
#define NCCH_OTHER_NO_ENCRYPTION    0x04u
#define NCCH_OTHER_SEEDED_AES_KEY   0x20u

int ncch_flags_mark_decrypted(const char *path) {
    if (!path) {
        return EINVAL;
    }

    FILE *f = fopen(path, "rb+");
    if (!f) {
        return errno ? errno : EIO;
    }

    if (fseek(f, NCCH_FLAGS_OTHER_OFFSET, SEEK_SET) != 0) {
        int err = errno ? errno : EIO;
        fclose(f);
        return err;
    }

    int byte = fgetc(f);
    if (byte == EOF) {
        fclose(f);
        return EIO;
    }

    unsigned char patched = (unsigned char)byte;
    patched &= (unsigned char)~(NCCH_OTHER_FIXED_AES_KEY | NCCH_OTHER_SEEDED_AES_KEY);
    patched |= NCCH_OTHER_NO_ENCRYPTION;

    if ((unsigned char)byte == patched) {
        /* Already marked decrypted. Treat as success. */
        fclose(f);
        return 0;
    }

    if (fseek(f, NCCH_FLAGS_OTHER_OFFSET, SEEK_SET) != 0) {
        int err = errno ? errno : EIO;
        fclose(f);
        return err;
    }

    if (fputc(patched, f) == EOF) {
        int err = errno ? errno : EIO;
        fclose(f);
        return err;
    }

    if (fflush(f) != 0) {
        int err = errno ? errno : EIO;
        fclose(f);
        return err;
    }

    if (fclose(f) != 0) {
        return errno ? errno : EIO;
    }

    return 0;
}
