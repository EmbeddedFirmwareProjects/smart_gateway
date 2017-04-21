
#include <comman.h>
#include <platform.h>
#include <platform_usart.h>


s16 PlatformSystemInit(void)
{
    LOG_INFO0(("<< %s >>\n", __func__));
    return 0;
}

s16 PlatformPeripheralInit(void)
{
    LOG_INFO0(("<< %s >>\n", __func__));

    return 0;
}

s16 XbeePlatformInit(PlatformOperations *platform_operations)
{
    return PlatformUsartInit(CHANNEL_XBEE, platform_operations);
}
