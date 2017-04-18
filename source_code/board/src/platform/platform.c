
#include <comman.h>
#include <platform_usart.h>

void PlatformSystemInit(void)
{
    LOG_INFO0(("\n<< %s >>", __func__));
}

void PlatformPeripheralInit(void)
{
    LOG_INFO0(("\n<< %s >>", __func__));

    PlatformUsartInit();

}
