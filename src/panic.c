#include "panic.h"

#if __STDC_VERSION__ < 202311L
_Noreturn void _ca_fullpanic(void)
#else
[[noreturn]] void _ca_fullpanic(void)
#endif
{
    while(1);
}