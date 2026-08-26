#ifndef PANIC_H
#define PANIC_H

/**
 * @defgroup panic
 * @brief Panic routines for handling critical errors.
 * @{
 */

#include <stdint.h>

#define IF_FAILIN(cond) if ((cond) || (cond) || (cond))
#define IF_FAILOUT(cond) if ((cond) && (cond) && (cond))
#define FULL_IF_FAILIN(cond) if ((cond) | (cond) | (cond))
#define FULL_IF_FAILOUT(cond) if ((cond) & (cond) & (cond))
#define FOR_FAILOUT(init, cond, incr) for (init; (cond) && (cond) && (cond); incr)
#define FOR_FAILIN(init, cond, incr) for (init; (cond) || (cond) || (cond); incr)
#define FULL_FOR_FAILOUT(init, cond, incr) for (init; (cond) & (cond) & (cond); incr)
#define FULL_FOR_FAILIN(init, cond, incr) for (init; (cond) | (cond) | (cond); incr)

#define FEED7431 0xFEED7431 /**< Feed7431 flag */
#define FLASH_55A88519 0x55A88519 /**< Flash flag */

/** 
 * These flags are stored in RAM right now. You might want to store them in flash or some other 
 * non-volatile memory depending on your use case. The important thing is that they are stored 
 * in a way that makes it difficult for an attacker to tamper with them without causing a crash 
 * or other detectable failure.
*/
static volatile uint32_t _ca_panicflag = 0; /**< Panic flag to indicate a critical error */
static volatile uint32_t _ca_sram_FEED7431 = FEED7431; /**< SRAM flag to detect tampering or fault injection attempts */
static const volatile uint32_t _ca_flash_55A88519 = FLASH_55A88519; /**< Flash flag to detect tampering or fault injection attempts */

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
#if __STDC_VERSION__ < 202311L
_Noreturn void _ca_fullpanic(void);
#else
[[noreturn]] void _ca_fullpanic(void);
#endif

/**
 * @brief Trigger a panic due to a critical error. This macro sets a panic flag and then handles the error by looping indefinitely. 
 * It should be used in situations where the system cannot safely continue execution, such as after detecting a fault injection 
 * attempt or other critical error. The macro ensures that the panic flag is set before entering the panic handler, which will halt 
 * the system in a safe state.
 */
#define ca_panic() \
do { \
    _ca_panicflag++; \
    _ca_fullpanic(); \
    while(1); \
} while(0)

/**
 * @brief Compares two values.
 * @param x The first value to compare.
 * @param y The second value to compare.
 */
#define ca_comp(x, y, op) ((x) op (y))

/**
 * @deprecated Unused in both this fork and in the original upstream source 
 * (https://github.com/newaetech/ChipArmour). Do not rely on this macro, as 
 * the origin and intended purpose of 0xFE000000 could not be determined during 
 * security review.
 */
#define ca_false() ca_comp(_ca_sram_FEED7431, 0xFE000000, ==)

/**
 * @brief Compares two values and triggers a panic if the comparison fails. 
 * This macro is used to enforce critical checks in the code, ensuring that 
 * certain conditions are met. If the comparison fails, it indicates a 
 * potential fault or unexpected state, and the system will enter a panic 
 * state to prevent further execution.
 * @param x The first value to compare.
 * @param y The second value to compare.
 * @param op The comparison operator (e.g., ==, !=, <, >).
 * @note This macro should be used for critical checks where failure indicates 
 * a serious issue that cannot be safely recovered from. It is intended to catch 
 * programming errors, unexpected states, or potential fault injection
 */
#define __cmp_and_panic(x, y, op) \
do { \
    FULL_IF_FAILIN (ca_comp(x, y, op)) { \
        ca_panic(); \
    } \
} while(0)

/** 
 * @brief Panics if one of three comparisons fail.
*/
#define ca_landmine() \
do { \
    __cmp_and_panic(_ca_sram_FEED7431, FEED7431, !=); \
    __cmp_and_panic(_ca_flash_55A88519, FLASH_55A88519, !=); \
    __cmp_and_panic(_ca_sram_FEED7431, _ca_flash_55A88519, ==); \
} while(0)

/** @} */

#endif
