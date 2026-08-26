#ifndef CA_RET_UINT_H
#define CA_RET_UINT_H

#include <stdint.h>
#include "panic.h"
#include "random.h"

#define ca_get_delay(maxdelay) _Generic((maxdelay), \
    uint64_t: (RANDOM_UINT(64, (maxdelay))) % UINT64_MAX, \
    uint32_t: (RANDOM_UINT(32, (maxdelay))) % UINT32_MAX, \
    uint16_t: (RANDOM_UINT(16, (maxdelay))) % UINT16_MAX, \
    uint8_t: (RANDOM_UINT(8, (maxdelay))) % UINT8_MAX, \
    default: 0 \
)

#define CA_RET_STATE_TYPE(bitsize) ca_ret_u##bitsize##_state_t
#define CA_UINT_TYPE(bitsize) ca_uint##bitsize##_t /**< ChipArmour type for unsigned integer of specified bit size */

#define CA_RET_STATE_INIT(bitsize) \
    (CA_RET_STATE_TYPE(bitsize)) { \
        .delay = ca_get_delay((maxdelay)), \
        .i = 0, \
        .local_value = 0 \
    }

#define CA_RET_UINT_STATE(value, maxdelay) _Generic((value), \
    uint64_t: CA_RET_STATE_INIT(64), \
    uint32_t: CA_RET_STATE_INIT(32), \
    uint16_t: CA_RET_STATE_INIT(16), \
    uint8_t: CA_RET_STATE_INIT(8) \
)

#define CA_RETFAST_INIT(bitsize, val) \
    (CA_UINT_TYPE(bitsize)) { \
        .value = (val), \
        .invvalue = (~(val)) \
    }

#define SET_STATE_LOCAL_VALUE(bitsize, state, value) \
    do { \
        ca_landmine(); \
        __cmp_and_panic((value).invvalue, (uint##bitsize##_t) (~(value).value), !=); \
        state.local_value = (value).value - state.delay; \
        __cmp_and_panic((value).invvalue, (uint##bitsize##_t) (~(value).value), !=); \
    } while (0)

#define STATE_INCREMENT_AND_CHECK(state) \
    do { \
        state.i++; \
        state.local_value++; \
        __cmp_and_panic(state.i, state.delay, >); \
        ca_landmine(); \
    } while (0)

#define CHECK_DELAY(state, retval) \
    do { \
        FULL_IF_FAILIN (ca_comp((state).i, (state).delay, ==)) { \
            return (retval); \
        } \
    } while (0)

#define _CA_RET_UINT(bitsize, value, maxdelay) \
    do { \
        ca_landmine(); \
        volatile CA_RET_STATE_TYPE(bitsize) state = CA_RET_UINT_STATE(value.value, maxdelay); \
        SET_STATE_LOCAL_VALUE(bitsize, state, value); \
        CA_UINT_TYPE(bitsize) invalid_rv = {0}; \
        while (ca_comp(_ca_sram_FEED7431, FEED7431, ==)) { \
            STATE_INCREMENT_AND_CHECK(state); \
            CHECK_DELAY(state, CA_RETFAST_INIT(bitsize, state.local_value)); \
            CHECK_DELAY(state, invalid_rv); \
            ca_landmine(); \
            CHECK_DELAY(state, invalid_rv); \
            __cmp_and_panic(state.i, state.delay, >); \
        } \
        ca_landmine(); \
        ca_panic(); \
        return (invalid_rv); \
    } while(0)

/**
 * @defgroup ca_ret_uint ChipArmour Return Unsigned Integer structures and functions
 * @brief Structures and functions to return uint values with fault injection armouring.
 * @{
 */

/**
 * @brief uint64_t returned by ca_ret_N functions, must be passed to comparison
 * functions.
*/
typedef struct {
    uint64_t value; //!< The actual value
    uint64_t invvalue; //!< The bitwise inverse of the value, used for integrity checks to detect fault injection attempts
} ca_uint64_t;

/**
 * @brief uint32_t returned by ca_ret_N functions, must be passed to comparison
 * functions.
*/
typedef struct {
    uint32_t value; //!< The actual value
    uint32_t invvalue; //!< The bitwise inverse of the value, used for integrity checks to detect fault injection attempts
} ca_uint32_t;

/**
 * @brief uint16_t returned by ca_ret_N functions, must be passed to comparison
 * functions.
*/
typedef struct {
    uint16_t value; //!< The actual value
    uint16_t invvalue; //!< The bitwise inverse of the value, used for integrity checks to detect fault injection attempts
} ca_uint16_t;

/**
 * @brief uint8_t returned by ca_ret_N functions, must be passed to comparison
 * functions.
*/
typedef struct {
    uint8_t value; //!< The actual value
    uint8_t invvalue; //!< The bitwise inverse of the value, used for integrity checks to detect fault injection attempts
} ca_uint8_t;

/**
 * @brief State struct for _ca_ret_u64 to assist with FI armouring.
 */
typedef struct {
    uint64_t delay; //!< Random delay value, generated at the start of the function this structure is used in
    uint64_t i; //!< Loop counter, incremented in the loop to create the delay
    uint64_t local_value; //!< Local copy of the value being returned, modified in the loop to create a changing state that can be checked for FI attempts
} ca_ret_u64_state_t;

/**
 * @brief State struct for _ca_ret_u32 to assist with FI armouring.
 */
typedef struct {
    uint32_t delay; //!< Random delay value, generated at the start of the function this structure is used in
    uint32_t i; //!< Loop counter, incremented in the loop to create the delay
    uint32_t local_value; //!< Local copy of the value being returned, modified in the loop to create a changing state that can be checked for FI attempts
} ca_ret_u32_state_t;

/**
 * @brief State struct for _ca_ret_u16 to assist with FI armouring.
 */
typedef struct {
    uint16_t delay; //!< Random delay value, generated at the start of the function this structure is used in
    uint16_t i; //!< Loop counter, incremented in the loop to create the delay
    uint16_t local_value; //!< Local copy of the value being returned, modified in the loop to create a changing state that can be checked for FI attempts
} ca_ret_u16_state_t;

/**
 * @brief State struct for _ca_ret_u8 to assist with FI armouring.
 */
typedef struct {
    uint8_t delay; //!< Random delay value, generated at the start of the function this structure is used in
    uint8_t i; //!< Loop counter, incremented in the loop to create the delay
    uint8_t local_value; //!< Local copy of the value being returned, modified in the loop to create a changing state that can be checked for FI attempts
} ca_ret_u8_state_t;

/**
 * @brief Returns an unsigned 64-bit value, but adds armour around the return
 * function to catch fault injection attempts.
 * 
 * @param value The value to return, passed as a ca_uint64_t struct with the value and its 
 * bitwise inverse for integrity checks.
 * @param magic A magic value for integrity checks.
 * @param maxdelay The maximum delay for the return function.
 * 
 * @return ca_uint64_t The value wrapped in a struct with its bitwise inverse for integrity checks.
 * 
 * @warning This function is not for use alone. Please use the `CA_RET_UINT` macro instead.
*/
CA_UINT_TYPE(64) _ca_ret_u64(CA_UINT_TYPE(64) value, CA_UINT_TYPE(64) magic, UINT_TYPE(64) maxdelay);

/**
 * @brief Returns a 64-bit unsigned int, but after a random delay to assist with 
 * FI armouring.
 * 
 * @param value The value to return.
 * 
 * @return ca_uint64_t The value wrapped in a struct with its bitwise inverse for integrity checks.
*/
CA_UINT_TYPE(64) ca_retfast_u64(UINT_TYPE(64) value);

/**
 * @brief Returns an unsigned 32-bit value, but adds armour around the return
 * function to catch fault injection attempts.
 * 
 * @param value The value to return, passed as a ca_uint32_t struct with the value and its 
 * bitwise inverse for integrity checks.
 * @param magic A magic value for integrity checks.
 * @param maxdelay The maximum delay for the return function.
 * 
 * @return ca_uint32_t The value wrapped in a struct with its bitwise inverse for integrity checks.
 * 
 * @warning This function is not for use alone. Please use the `CA_RET_UINT` macro instead.
*/
CA_UINT_TYPE(32) _ca_ret_u32(CA_UINT_TYPE(32) value, CA_UINT_TYPE(32) magic, UINT_TYPE(32) maxdelay);

/**
 * @brief Returns a 32-bit unsigned int, but after a random delay to assist with 
 * FI armouring.
 * 
 * @param value The value to return.
 * 
 * @return ca_uint32_t The value wrapped in a struct with its bitwise inverse for integrity checks.
*/
CA_UINT_TYPE(32) ca_retfast_u32(UINT_TYPE(32) value);

/**
 * @brief Returns an unsigned 16-bit value, but adds armour around the return
 * function to catch fault injection attempts.
 * 
 * @param value The value to return, passed as a ca_uint16_t struct with the value and its 
 * bitwise inverse for integrity checks.
 * @param magic A magic value for integrity checks.
 * @param maxdelay The maximum delay for the return function.
 * 
 * @return ca_uint16_t The value wrapped in a struct with its bitwise inverse for integrity checks.
 * 
 * @warning This function is not for use alone. Please use the `CA_RET_UINT` macro instead.
*/
CA_UINT_TYPE(16) _ca_ret_u16(CA_UINT_TYPE(16) value, CA_UINT_TYPE(16) magic, UINT_TYPE(16) maxdelay);

/**
 * @brief Returns a 16-bit unsigned int, but after a random delay to assist with 
 * FI armouring.
 * 
 * @param value The value to return.
 * 
 * @return ca_uint16_t The value wrapped in a struct with its bitwise inverse for integrity checks.
*/
CA_UINT_TYPE(16) ca_retfast_u16(UINT_TYPE(16) value);

/**
 * @brief Returns an unsigned 8-bit value, but adds armour around the return
 * function to catch fault injection attempts.
 * @param value The value to return, passed as a ca_uint8_t struct with the value and its 
 * bitwise inverse for integrity checks.
 * @param magic A magic value for integrity checks.
 * @param maxdelay The maximum delay for the return function.
 * 
 * @return ca_uint8_t The value wrapped in a struct with its bitwise inverse for integrity checks.
 * 
 * @warning This function is not for use alone. Please use the `CA_RET_UINT` macro instead.
*/
CA_UINT_TYPE(8) _ca_ret_u8(CA_UINT_TYPE(8) value, CA_UINT_TYPE(8) magic, UINT_TYPE(8) maxdelay);

/**
 * @brief Returns an 8-bit unsigned int, but after a random delay to assist with 
 * FI armouring.
 * 
 * @param value The value to return.
 * 
 * @return ca_uint8_t The value wrapped in a struct with its bitwise inverse for integrity checks.
 */
CA_UINT_TYPE(8) ca_retfast_u8(UINT_TYPE(8) value);

/**
 * @brief Returns an unsigned integer of the same type as `value`, but adds armour around the return
 * function to catch fault injection attempts.
 * @param value The value to return, passed as a ca_uintN_t struct with the value and its 
 * bitwise inverse for integrity checks.
 * @param magic A magic value for integrity checks.
 * @param maxdelay The maximum delay for the return function.
 */
#define CA_RET_UINT(value, magic, maxdelay) _Generic((value), \
    CA_UINT_TYPE(32): _ca_ret_u32(ca_retfast_u32((value)), ca_retfast_u32((magic)), (maxdelay)), \
    CA_UINT_TYPE(16): _ca_ret_u16(ca_retfast_u16((value)), ca_retfast_u16((magic)), (maxdelay)), \
    CA_UINT_TYPE(8): _ca_ret_u8(ca_retfast_u8((value)), ca_retfast_u8((magic)), (maxdelay)) \
)

/** @} */

#endif
