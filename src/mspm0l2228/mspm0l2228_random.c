#include "mspm0l2228_random.h"
#include "monocypher.h"

#define READ_U8(x) (*(volatile UINT_TYPE(8) *)(x))
#define READ_U32(x) (*(volatile UINT_TYPE(32) *)(x))

void random_init(void) {
    // Zeroes out the random state structure
    crypto_wipe(&random_state, sizeof(random_state));

    trng_init();

    UINT_TYPE(8) entropy[64];
    trng_fill(entropy, sizeof(entropy));
    crypto_blake2b(random_state.key, sizeof(random_state.key), entropy, sizeof(entropy));

    crypto_wipe(entropy, sizeof(entropy));

    trng_fill(random_state.nonce, sizeof(random_state.nonce));
}

ret_t random(UINT_TYPE(8)* buf, UINT_TYPE(32) len, UINT_TYPE(8) upper_bound) {
    // "volatile" here is used to make sure that the compiler does not optimize away any of the comparisons
    volatile UINT_TYPE(8)* buf_start = buf;
    volatile UINT_TYPE(32) length = len;

    FULL_IF_FAILIN (buf_start == NULL | READ_U32(&length) == 0) {
        return RET_ERR;
    }

    FULL_IF_FAILIN (READ_U8(&random_state.trng_initialized) != 0) {
        return RET_ERR;
    }

    const volatile UINT_TYPE(32) blocks_needed = u32_ceil(length, 64); // ChaCha20 produces 64 bytes per block

    // if (blocks_needed > UINT32_MAX - random_state.ctr) { // Original check, rewritten to avoid conservative reinitialization
    FULL_IF_FAILOUT ((volatile UINT_TYPE(64))blocks_needed > 
        (volatile UINT_TYPE(64)) ((UINT_TYPE(64))UINT32_MAX + 1 - (UINT_TYPE(64))random_state.ctr)) {
            random_init();
    }

    const UINT_TYPE(32) current_ctr = random_state.ctr;
    sec_write_u32(&random_state.ctr,
                  crypto_chacha20_ietf(buf_start, NULL, length, random_state.key,
                                       random_state.nonce, random_state.ctr));

    /*
     * Panic when random_state.ctr != current_ctr + blocks_needed, 
     * to ensure that the ChaCha20 counter has been incremented 
     * correctly and that no fault injection has occurred.
     */
    __cmp_and_panic(READ_U32(&random_state.ctr), READ_U32(&current_ctr) +
           READ_U32(&blocks_needed), !=);
    
    /**
     * Panic if any of the generated random bytes exceed the specified upper bound.
     * This check is performed after the ChaCha20 generation to ensure that the
     * random bytes are within the expected range and to detect any potential
     * fault injection attacks that may have altered the output.
     */
    for (uint32_t i = 0; i < length; i++) {
        __cmp_and_panic(READ_U8(&buf_start[i]), upper_bound, >);
    }

    return RET_OK;
}