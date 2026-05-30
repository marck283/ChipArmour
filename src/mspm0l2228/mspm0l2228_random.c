#include "mspm0l2228_random.h"
#include "monocypher.h"

#define READ_U32(x) (*(volatile uint32_t *)(x))

void random_init(void) {
    trng_init();

    uint8_t entropy[64];
    trng_fill(entropy, sizeof(entropy));
    crypto_blake2b(random_state.key, sizeof(random_state.key), entropy, sizeof(entropy));

    crypto_wipe(entropy, sizeof(entropy));

    trng_fill(random_state.nonce, sizeof(random_state.nonce));
    random_state.ctr = 0;
    random_state.trng_initialized = 0;
}

ret_t random(uint8_t * buf, uint32_t len, uint8_t upper_bound) {
    if (buf == NULL || len == 0) {
        return RET_ERR;
    }

    uint32_t blocks_needed = CEIL(len, 64); // ChaCha20 produces 64 bytes per block

    if (random_state.trng_initialized != 0) {
        return RET_ERR;
    }

    // if (blocks_needed > UINT32_MAX - random_state.ctr) { // Original check, rewritten to avoid conservative reinitialization
    if ((uint64_t)blocks_needed > ((uint64_t)UINT32_MAX + 1 - (uint64_t)random_state.ctr)) {
        random_init();
    }

    uint32_t current_ctr = random_state.ctr;
    sec_write_u32(&random_state.ctr,
                  crypto_chacha20_ietf(buf, NULL, len, random_state.key,
                                       random_state.nonce, random_state.ctr));
    ASSERT(READ_U32(&random_state.ctr) == READ_U32(&current_ctr) +
           READ_U32(&blocks_needed), "Random error", RET_ERR);

    uint16_t bound = (uint16_t)upper_bound + 1U;
    for (uint32_t i = 0; i < len; i++) {
        buf[i] %= bound;
    }

    return RET_OK;
}