
#include <platform_usart_pvt.h>


void ReceiveUartData(void);


static UsartConfigSetting *sLinuxUsartSetting = '\0';


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
    sLinuxUsartSetting = usart_setting;

    ReceiveUartData();  //testing purpose only
}

void ReceiveUartData(void)
{
    u8 api_frame[] = {0x32, 0x12, 0x34, 0x7E, 0x00, 0x07, 0x88, 0x01, 'D', 'H', 0x00, 0x02, 0x01, 0xe7, 0x02, 0x01, 0xe7, 0x7E, 0x00, 0x07, 0x88, 0x01, 'D', 'H', 0x00, 0x02, 0x01, 0xe7};

    if(sLinuxUsartSetting->pRxFunc != '\0')
    {
        sLinuxUsartSetting->pRxFunc(api_frame, sizeof(api_frame));
    }
}

