#ifndef CIA3DS_AES_ARMV8_H
#define CIA3DS_AES_ARMV8_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int cia3ds_aes_arm_available(void);

void cia3ds_aes_arm_encrypt(int nr, const uint8_t *rk,
                            const uint8_t *in, uint8_t *out);

void cia3ds_aes_arm_decrypt(int nr, const uint8_t *rk,
                            const uint8_t *in, uint8_t *out);

void cia3ds_aes_armv8_build_dec_schedule(uint8_t *out_rk,
                                         const uint8_t *fwd_rk,
                                         int nr);

#ifdef __cplusplus
}
#endif

#endif
