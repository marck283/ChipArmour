#ifndef MSPM0L2228_RANDOM_H
#define MSPM0L2228_RANDOM_H

#include "ti_msp_dl_config.h"
#include <string.h>
#include "random.h"
#include "chacha20_ietf_random.h"
#include "chiparmour.h"

/**
 * @brief Initialize the hardware TRNG peripheral.
 * @note This function configures the TRNG and waits for command completion
 *       before returning.
 */
static inline void trng_init(void) {
    DL_TRNG_disablePower(TRNG);
    DL_TRNG_reset(TRNG);
    DL_TRNG_enablePower(TRNG);
    delay_cycles(POWER_STARTUP_DELAY);
    DL_TRNG_setClockDivider(TRNG, DL_TRNG_CLOCK_DIVIDE_2);
    DL_TRNG_setDecimationRate(TRNG, DL_TRNG_DECIMATION_RATE_4);
    DL_TRNG_sendCommand(TRNG, DL_TRNG_CMD_NORM_FUNC);
    while (!DL_TRNG_isCommandDone(TRNG));
}

/**
 * @brief Fill a buffer with entropy from the hardware TRNG.
 * @param[out] buf Destination buffer.
 * @param length Number of bytes to read.
 */
static inline void trng_fill(uint8_t *buf, uint32_t length) {
    while (length > 0) {
        while (!DL_TRNG_isCaptureReady(TRNG));

        uint32_t random_word = DL_TRNG_getCapture(TRNG);
        uint32_t bytes_num = MIN(length, 4);
        memcpy(buf, &random_word, bytes_num);

        buf += bytes_num;
        length -= MIN(length, bytes_num);
    }
}

/**
 * @brief Write a 32-bit value using a fault-hardened verification step.
 * @param[out] dst Destination where the value is written.
 * @param val Value to store.
 * @post The provided value is written to dst.
 * @post A fatal security response is triggered if the verification fails.
 */
static inline void sec_write_u32(volatile uint32_t *dst, uint32_t val) {
    *dst = val;
    IF_FAILIN (val != *dst) {
        causer_panic();
    }
}

#endif
