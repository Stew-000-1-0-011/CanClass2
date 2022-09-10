// cmsis_armcc.hより先にこいつをインクルードすること.
#include <CRSLib/cmsis_driver_include.h>
#include <cmsis_armcc.h>

#include <CRSLib/cmsis_for_cpp.h>

uint32_t stew_load_link_32(uint32_t * src)
{
    return __LDREXW(src);
}

unsigned char stew_store_conditional_32(uint32_t * dst, uint32_t val)
{
    return __STREXW(dst, val);
}

void stew_enable_irq(void)
{
    __enable_irq();
}

void stew_disable_irq(void)
{
    __disable_irq();
}