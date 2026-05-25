#ifndef PANIC_H
#define PANIC_H

/**
 * @defgroup panic
 * @brief Panic routines for handling critical errors.
 * @{
 */

#define FEED7431 0xFEED7431
#define FLASH_55A88519 0x55A88519

static uint32_t _ca_panicflag = 0;
static uint32_t _ca_sram_FEED7431 = FEED7431;
static const uint32_t _ca_flash_55A88519 = FLASH_55A88519;

/**
 * @brief Handle any critical error. This function must be called through the `ca_panic()` macro, 
 * which sets a panic flag and then calls this function.
 * @note This function should never return and is designed to halt the system in a safe state. It 
 * can be used to catch any critical errors or unexpected conditions that occur during execution, 
 * such as invalid memory access, failed integrity checks, or other unrecoverable errors.
 * @warning This function will enter an infinite loop, effectively halting the system. It should 
 * only be used in situations where recovery is not possible or safe, and it should be called 
 * through the `ca_panic()` macro to ensure the panic flag is set correctly.
 */
int _ca_fullpanic(void);

#define ca_panic() \
do { \
    _ca_panicflag++; \
    _ca_fullpanic(); \
} while(0)

#define __cmp_and_panic(x, y, op) \
do { \
    if ((x) op (y)) { \
        ca_panic(); \
    } \
} while(0)

/** 
  Jumps to the panic function if one of two comparisons fail.
  */
#define ca_landmine() \
do { \
    __cmp_and_panic(_ca_sram_FEED7431, FEED7431, !=); \
    __cmp_and_panic(_ca_flash_55A88519, FLASH_55A88519, !=); \
    __cmp_and_panic(_ca_sram_FEED7431, _ca_flash_55A88519, ==); \
} while(0)

#define ca_true()  (_ca_sram_FEED7431 == FEED7431)
#define ca_false() (_ca_sram_FEED7431 == 0xFE000000)

/** @} */

#endif
