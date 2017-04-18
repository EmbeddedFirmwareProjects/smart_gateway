#include <platform_usart_pvt.h>

static s16 LinuxSerialSend(u8 *pdata, u16 len)
{
    u16 count = 0x00;

    LOG_INFO(("\n<< %s >>", __func__));

    LOG_INFO(("\nSend Packet:: "));
    for(count = 0x00; count < len; count++)
    {
        LOG_INFO((" %x", pdata[count]));
    }
    return 0;
}
void LinuxSerialInit(UsartConfigSetting *usart_setting)
{
    LOG_INFO(("\n<< %s >>", __func__));

    usart_setting->pTxFunc = LinuxSerialSend;
}


