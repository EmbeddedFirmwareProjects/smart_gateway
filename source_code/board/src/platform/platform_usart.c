
#include <comman.h>
#include <platform_usart_pvt.h>


s16 LinuxSerialInit(UsartConfigSetting *usart_setting);


static UsartConfigSetting sUsartConfigList[] = {
    {
        .channel = CHANNEL_XBEE,
        .baudRate = USART_BAUDRATE_9600,
        .parity = USART_PARITY_NO,
        .stopBit = USART_STOPBIT_1,
        .pInitFunc = LinuxSerialInit,
    },
};


s16 PlatformUsartInit(u8 usart_channel, PlatformOperations *platform_operations)
{
    u16 count = 0x00;
    u16 config_list = 0x00;
    s16 ret = 0x00;

    LOG_INFO0(("\n<< %s >>", __func__));

    config_list = sizeof(sUsartConfigList)/sizeof(UsartConfigSetting);

    for(count = 0x00; count < config_list; count++)
    {
        if(usart_channel == sUsartConfigList[count].channel)
        {
            sUsartConfigList[count].pRxFunc = platform_operations->pRxFunc;
            if(sUsartConfigList[count].pInitFunc != '\0')
            {
                ret = sUsartConfigList[count].pInitFunc(&sUsartConfigList[count]);
                if(ret != EUSART_OK)
                {
                    LOG_ERR(("\nERR:: PlatformUsartInit(): init() failure %d", ret));
                    return ret;
                }
                platform_operations->pTxFunc = sUsartConfigList[count].pTxFunc;
                return EUSART_OK;
            }
            else
            {
                LOG_ERR(("\nERR:: invalid init()  %d", ret));
                return -EUSART_INVALID_INIT_PFUNC;
            }
        }
    }
    return -EUSART_INVALID_CHANNEL_OPERATIONS;
}
