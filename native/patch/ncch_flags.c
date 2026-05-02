/* NcchFlags.other_flag is at file offset 0x18F (NcchCommonHeader base 0x100 + struct offset 0x88 + byte 7).
 * Bit 2 (0x04) = NoEncryption must be set; bits 0 (FixedAesKey) and 5 (SeededAesKeyY) must be clear. */
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
