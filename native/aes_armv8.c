#include "aes_armv8.h"

#include <stddef.h>
#include <string.h>

#ifdef __aarch64__

#include <arm_neon.h>
#include <sys/auxv.h>
#include <asm/hwcap.h>

static int g_have_aes = -1;

int cia3ds_aes_arm_available(void) {
    if (g_have_aes < 0) {
        unsigned long hw = getauxval(AT_HWCAP);
        g_have_aes = (hw & HWCAP_AES) ? 1 : 0;
    }
    return g_have_aes;
}

void cia3ds_aes_arm_encrypt(int nr, const uint8_t *rk,
                            const uint8_t *in, uint8_t *out) {
    uint8x16_t state = vld1q_u8(in);
    for (int i = 0; i < nr - 1; ++i) {
        state = vaeseq_u8(state, vld1q_u8(rk + i * 16));
        state = vaesmcq_u8(state);
    }
    state = vaeseq_u8(state, vld1q_u8(rk + (nr - 1) * 16));
    state = veorq_u8(state, vld1q_u8(rk + nr * 16));
    vst1q_u8(out, state);
}

void cia3ds_aes_arm_decrypt(int nr, const uint8_t *rk,
                            const uint8_t *in, uint8_t *out) {
    uint8x16_t state = vld1q_u8(in);
    for (int i = 0; i < nr - 1; ++i) {
        state = vaesdq_u8(state, vld1q_u8(rk + i * 16));
        state = vaesimcq_u8(state);
    }
    state = vaesdq_u8(state, vld1q_u8(rk + (nr - 1) * 16));
    state = veorq_u8(state, vld1q_u8(rk + nr * 16));
    vst1q_u8(out, state);
}

void cia3ds_aes_armv8_build_dec_schedule(uint8_t *out_rk,
                                         const uint8_t *fwd_rk,
                                         int nr) {
    vst1q_u8(out_rk, vld1q_u8(fwd_rk + nr * 16));
    for (int i = 1; i < nr; ++i) {
        uint8x16_t k = vld1q_u8(fwd_rk + (nr - i) * 16);
        vst1q_u8(out_rk + i * 16, vaesimcq_u8(k));
    }
    vst1q_u8(out_rk + nr * 16, vld1q_u8(fwd_rk));
}

#else

int cia3ds_aes_arm_available(void) { return 0; }

void cia3ds_aes_arm_encrypt(int nr, const uint8_t *rk,
                            const uint8_t *in, uint8_t *out) {
    (void)nr; (void)rk; (void)in; (void)out;
}

void cia3ds_aes_arm_decrypt(int nr, const uint8_t *rk,
                            const uint8_t *in, uint8_t *out) {
    (void)nr; (void)rk; (void)in; (void)out;
}

void cia3ds_aes_armv8_build_dec_schedule(uint8_t *out_rk,
                                         const uint8_t *fwd_rk,
                                         int nr) {
    (void)out_rk; (void)fwd_rk; (void)nr;
}

#endif
