#include "ca_ret_uint.h"

CA_UINT_TYPE(64) _ca_ret_u64(CA_UINT_TYPE(64) value, CA_UINT_TYPE(64) magic, UINT_TYPE(64) maxdelay)
{
    __cmp_and_panic(magic.invvalue, (UINT_TYPE(64)) ~magic.value, !=);
    _CA_RET_UINT(64, value, maxdelay);
    __cmp_and_panic(magic.invvalue, (UINT_TYPE(64)) ~magic.value, !=);
}

CA_UINT_TYPE(64) ca_retfast_u64(UINT_TYPE(64) value)
{
    return CA_RETFAST_INIT(64, value);
}

CA_UINT_TYPE(32) _ca_ret_u32(CA_UINT_TYPE(32) value, CA_UINT_TYPE(32) magic, UINT_TYPE(32) maxdelay)
{
    __cmp_and_panic(magic.invvalue, (UINT_TYPE(32)) ~magic.value, !=);
    _CA_RET_UINT(32, value, maxdelay);
    __cmp_and_panic(magic.invvalue, (UINT_TYPE(32)) ~magic.value, !=);
}

CA_UINT_TYPE(32) ca_retfast_u32(UINT_TYPE(32) value)
{
    return CA_RETFAST_INIT(32, value);
}

CA_UINT_TYPE(16) _ca_ret_u16(CA_UINT_TYPE(16) value, CA_UINT_TYPE(16) magic, UINT_TYPE(16) maxdelay)
{
    __cmp_and_panic(magic.invvalue, (UINT_TYPE(16)) ~magic.value, !=);
    _CA_RET_UINT(16, value, maxdelay);
    __cmp_and_panic(magic.invvalue, (UINT_TYPE(16)) ~magic.value, !=);
}

CA_UINT_TYPE(16) ca_retfast_u16(UINT_TYPE(16) value)
{
    return CA_RETFAST_INIT(16, value);
}

CA_UINT_TYPE(8) _ca_ret_u8(CA_UINT_TYPE(8) value, CA_UINT_TYPE(8) magic, UINT_TYPE(8) maxdelay)
{
    __cmp_and_panic(magic.invvalue, (UINT_TYPE(8)) ~magic.value, !=);
    _CA_RET_UINT(8, value, maxdelay);
    __cmp_and_panic(magic.invvalue, (UINT_TYPE(8)) ~magic.value, !=);
}

CA_UINT_TYPE(8) ca_retfast_u8(UINT_TYPE(8) value)
{
    return CA_RETFAST_INIT(8, value);
}
