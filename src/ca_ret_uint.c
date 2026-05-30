#include "ca_ret_uint.h"

ca_uint64_t _ca_ret_u64(ca_uint64_t value, ca_uint64_t magic, uint64_t maxdelay)
{
    __cmp_and_panic(magic.invvalue, ~magic.value, !=);
    _CA_RET_UINT(64, value, maxdelay);
    __cmp_and_panic(magic.invvalue, ~magic.value, !=);
}

ca_uint64_t ca_retfast_u64(uint64_t value)
{
    return CA_RETFAST_INIT(64, value);
}

ca_uint32_t _ca_ret_u32(ca_uint32_t value, ca_uint32_t magic, uint32_t maxdelay)
{
    __cmp_and_panic(magic.invvalue, ~magic.value, !=);
    _CA_RET_UINT(32, value, maxdelay);
    __cmp_and_panic(magic.invvalue, ~magic.value, !=);
}

ca_uint32_t ca_retfast_u32(uint32_t value)
{
    return CA_RETFAST_INIT(32, value);
}

ca_uint16_t _ca_ret_u16(ca_uint16_t value, ca_uint16_t magic, uint16_t maxdelay)
{
    __cmp_and_panic(magic.invvalue, ~magic.value, !=);
    _CA_RET_UINT(16, value, maxdelay);
    __cmp_and_panic(magic.invvalue, ~magic.value, !=);
}

ca_uint16_t ca_retfast_u16(uint16_t value)
{
    return CA_RETFAST_INIT(16, value);
}

ca_uint8_t _ca_ret_u8(ca_uint8_t value, ca_uint8_t magic, uint8_t maxdelay)
{
    __cmp_and_panic(magic.invvalue, ~magic.value, !=);
    _CA_RET_UINT(8, value, maxdelay);
    __cmp_and_panic(magic.invvalue, ~magic.value, !=);
}

ca_uint8_t ca_retfast_u8(uint8_t value)
{
    return CA_RETFAST_INIT(8, value);
}

ca_uint16_t ca_retfast_u16(uint16_t value)
{
    return CA_RETFAST_INIT(16, value);
}

ca_uint8_t _ca_ret_u8(ca_uint8_t value, ca_uint8_t magic, uint8_t maxdelay)
{
    (void)magic;
    _CA_RET_UINT(8, value, maxdelay);
}

ca_uint8_t ca_retfast_u8(uint8_t value)
{
    return CA_RETFAST_INIT(8, value);
}