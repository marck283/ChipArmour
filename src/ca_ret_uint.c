#include <stdint.h>
#include "ca_ret_uint.h"
#include "panic.h"

#define ca_get_delay(maxdelay) _Generic((maxdelay), \
    uint32_t: (maxdelay), \
    uint16_t: (maxdelay), \
    uint8_t: (maxdelay), \
    default: (maxdelay) \
)

#define CA_RET_STATE_TYPE(bitsize) ca_ret_u##bitsize##_state_t
#define CA_UINT_TYPE(bitsize) ca_uint##bitsize##_t

#define CA_RET_STATE_INIT(bitsize) \
    (CA_RET_STATE_TYPE(bitsize)) { \
        .delay = ca_get_delay((maxdelay)), \
        .i = 0, \
        .local_value = 0 \
    }

#define CA_RET_UINT_STATE(value, maxdelay) _Generic((value), \
    uint32_t: CA_RET_STATE_INIT(32), \
    uint16_t: CA_RET_STATE_INIT(16), \
    uint8_t: CA_RET_STATE_INIT(8) \
)

#define CA_RETFAST_INIT(bitsize, val) \
    (struct CA_UINT_TYPE(bitsize)) { \
        .value = (val), \
        .invvalue = (~(val)) \
    }

#define SET_STATE_LOCAL_VALUE(state, value) \
    do { \
        ca_landmine(); \
        __cmp_and_panic((value).invvalue, ~(value).value, !=); \
        state.local_value = (value).value - state.delay; \
        __cmp_and_panic((value).invvalue, ~(value).value, !=); \
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
        if ((state).i == (state).delay) { \
            return (retval); \
        } \
    } while (0)

#define _CA_RET_UINT(bitsize, value, maxdelay) \
    do { \
        ca_landmine(); \
        CA_RET_STATE_TYPE(bitsize) state = CA_RET_UINT_STATE(value.value, maxdelay); \
        SET_STATE_LOCAL_VALUE(state, value); \
        CA_UINT_TYPE(bitsize) invalid_rv = {0}; \
        while (ca_true()) { \
            STATE_INCREMENT_AND_CHECK(state); \
            CHECK_DELAY(state, CA_RETFAST_INIT(bitsize, state.local_value)); \
            CHECK_DELAY(state, invalid_rv); \
            ca_landmine(); \
            CHECK_DELAY(state, invalid_rv); \
            __cmp_and_panic(state.i, state.delay, >); \
        } \
        ca_landmine(); \
        ca_panic(); \
        return invalid_rv; \
    } while(0)

/**
  Returns an unsigned 32-bit value, but adds armour around the return
  function to catch fault injection attempts.
*/
ca_uint32_t _ca_ret_u32(ca_uint32_t value, ca_uint32_t magic, uint32_t maxdelay)
{
    (void)magic;
    _CA_RET_UINT(32, value, maxdelay);
}

ca_uint32_t ca_retfast_u32(uint32_t value)
{
    return CA_RETFAST_INIT(32, value);
}

/**
  Returns an unsigned 16-bit value, but adds armour around the return
  function to catch fault injection attempts.
*/
ca_uint16_t _ca_ret_u16(ca_uint16_t value, ca_uint16_t magic, uint16_t maxdelay)
{
    (void)magic;
    _CA_RET_UINT(16, value, maxdelay);
}

ca_uint16_t ca_retfast_u16(uint16_t value)
{
    return CA_RETFAST_INIT(16, value);
}

/**
  Returns an unsigned 8-bit value, but adds armour around the return
  function to catch fault injection attempts.
*/
ca_uint8_t _ca_ret_u8(ca_uint8_t value, ca_uint8_t magic, uint8_t maxdelay)
{
    (void)magic;
    _CA_RET_UINT(8, value, maxdelay);
}

ca_uint8_t ca_retfast_u8(uint8_t value)
{
    return CA_RETFAST_INIT(8, value);
}

#define CA_RET_UINT(value, magic, maxdelay) _Generic((value), \
    uint32_t: _ca_ret_u32(ca_retfast_u32((value)), ca_retfast_u32((magic)), (maxdelay)), \
    uint16_t: _ca_ret_u16(ca_retfast_u16((value)), ca_retfast_u16((magic)), (maxdelay)), \
    uint8_t: _ca_ret_u8(ca_retfast_u8((value)), ca_retfast_u8((magic)), (maxdelay)) \
)