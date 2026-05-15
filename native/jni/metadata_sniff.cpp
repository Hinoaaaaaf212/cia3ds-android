#include "metadata_sniff.h"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <sys/types.h>
#include <unistd.h>

namespace {

bool pread_full(int fd, void *buf, size_t n, off_t off) {
    uint8_t *p = (uint8_t *)buf;
    size_t got = 0;
    while (got < n) {
        ssize_t r = pread(fd, p + got, n - got, off + (off_t)got);
        if (r <= 0) return false;
        got += (size_t)r;
    }
    return true;
}

uint16_t rd_u16_le(const uint8_t *p) {
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}
uint32_t rd_u32_le(const uint8_t *p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8)
         | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}
uint64_t rd_u64_be(const uint8_t *p) {
    uint64_t v = 0;
    for (int i = 0; i < 8; ++i) v = (v << 8) | p[i];
    return v;
}
uint16_t rd_u16_be(const uint8_t *p) {
    return (uint16_t)(((uint16_t)p[0] << 8) | p[1]);
}
uint32_t rd_u32_be(const uint8_t *p) {
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16)
         | ((uint32_t)p[2] << 8)  | (uint32_t)p[3];
}

uint64_t align_up(uint64_t v, uint64_t a) {
    return (v + (a - 1)) & ~(a - 1);
}

void format_title_id(uint64_t v, std::string &out) {
    char buf[17];
    snprintf(buf, sizeof(buf), "%016llx", (unsigned long long)v);
    out = buf;
}

uint64_t sig_section_size(uint32_t sig_type) {
    switch (sig_type) {
        case 0x010000: case 0x010003: return 4 + 0x200 + 0x3C + 0x40;
        case 0x010001: case 0x010004: return 4 + 0x100 + 0x3C + 0x40;
        case 0x010002: case 0x010005: return 4 + 0x3C  + 0x40 + 0x40;
        default: return 0;
    }
}

bool sniff_cia(int fd, SniffedMetadata &out) {
    uint8_t hdr[0x20];
    if (!pread_full(fd, hdr, sizeof(hdr), 0)) return false;
    uint32_t header_size = rd_u32_le(hdr + 0x00);
    if (header_size != 0x2020) return false;
    uint32_t cert_size  = rd_u32_le(hdr + 0x08);
    uint32_t ticket_size = rd_u32_le(hdr + 0x0C);
    uint32_t tmd_size    = rd_u32_le(hdr + 0x10);

    const uint64_t kAlign = 64;
    uint64_t cia_hdr_aligned = align_up(0x2020, kAlign);
    uint64_t cert_aligned    = align_up(cert_size, kAlign);
    uint64_t ticket_aligned  = align_up(ticket_size, kAlign);
    uint64_t tmd_aligned     = align_up(tmd_size, kAlign);
    uint64_t tmd_off = cia_hdr_aligned + cert_aligned + ticket_aligned;
    uint64_t content_off = tmd_off + tmd_aligned;

    uint8_t sig_type_buf[4];
    if (!pread_full(fd, sig_type_buf, 4, (off_t)tmd_off)) return false;
    uint32_t sig_type = rd_u32_be(sig_type_buf);
    uint64_t sig_sz = sig_section_size(sig_type);
    if (sig_sz == 0) return false;

    uint8_t tmd_hdr[100];
    if (!pread_full(fd, tmd_hdr, sizeof(tmd_hdr),
                    (off_t)(tmd_off + sig_sz))) return false;
    uint64_t title_id = rd_u64_be(tmd_hdr + 0x0C);
    uint16_t title_version = rd_u16_be(tmd_hdr + 0x5C);
    uint16_t num_contents = rd_u16_be(tmd_hdr + 0x5E);

    bool tmd_says_encrypted = true;
    if (num_contents >= 1) {
        uint64_t cmd_off = tmd_off + sig_sz + sizeof(tmd_hdr) + 0x920;
        uint8_t cmd0[48];
        if (pread_full(fd, cmd0, sizeof(cmd0), (off_t)cmd_off)) {
            uint16_t type = rd_u16_be(cmd0 + 0x06);
            tmd_says_encrypted = (type & 0x1) != 0;
        }
    }

    bool ncch_says_encrypted = true;
    {
        uint8_t ncch[0x100];
        if (pread_full(fd, ncch, sizeof(ncch), (off_t)(content_off + 0x100))) {
            if (ncch[0] == 'N' && ncch[1] == 'C' && ncch[2] == 'C'
                && ncch[3] == 'H') {
                uint8_t flag7 = ncch[0x88 + 7];
                ncch_says_encrypted = (flag7 & 0x04) == 0;
            }
        }
    }

    out.is_3ds = false;
    format_title_id(title_id, out.title_id);
    char vbuf[16];
    snprintf(vbuf, sizeof(vbuf), "%u", (unsigned)title_version);
    out.title_version = vbuf;
    out.already_decrypted = !tmd_says_encrypted && !ncch_says_encrypted;
    out.valid = true;
    return true;
}

bool sniff_ncsd(int fd, SniffedMetadata &out) {
    uint8_t ncsd[0x20];
    if (!pread_full(fd, ncsd, sizeof(ncsd), 0x100)) return false;
    if (ncsd[0] != 'N' || ncsd[1] != 'C' || ncsd[2] != 'S' || ncsd[3] != 'D') {
        return false;
    }
    uint64_t title_id_le = 0;
    for (int i = 0; i < 8; ++i) {
        title_id_le |= (uint64_t)ncsd[0x08 + i] << (i * 8);
    }

    uint8_t part_table[0x20];
    if (!pread_full(fd, part_table, sizeof(part_table), 0x100 + 0x20)) return false;
    uint32_t blk0_off = rd_u32_le(part_table + 0x00);

    bool ncch_decrypted = false;
    if (blk0_off != 0) {
        uint64_t first_ncch = (uint64_t)blk0_off * 0x200ULL;
        uint8_t ncch[0x100];
        if (pread_full(fd, ncch, sizeof(ncch), (off_t)(first_ncch + 0x100))) {
            if (ncch[0] == 'N' && ncch[1] == 'C' && ncch[2] == 'C'
                && ncch[3] == 'H') {
                uint8_t flag7 = ncch[0x88 + 7];
                ncch_decrypted = (flag7 & 0x04) != 0;
            }
        }
    }

    out.is_3ds = true;
    format_title_id(title_id_le, out.title_id);
    out.title_version = "0";
    out.already_decrypted = ncch_decrypted;
    out.valid = true;
    return true;
}

}

bool sniff_metadata_from_fd(int fd, SniffedMetadata &out) {
    out = SniffedMetadata{};
    if (fd < 0) return false;
    if (sniff_cia(fd, out)) return true;
    out = SniffedMetadata{};
    if (sniff_ncsd(fd, out)) return true;
    out = SniffedMetadata{};
    return false;
}
