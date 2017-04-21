#include <comman.h>
#include <platform_usart_pvt.h>


#include <rs232.h>
#include<pthread.h>


static s8 *sUsartPortFile = "/dev/pts/11";
static u32 sUsartPort = 0x00;
static UsartConfigSetting *sLinuxUsartSetting = '\0';
static u8 sUsartRxBuffer[4096];
static pthread_t sUsartRxtid;


static void * ThreadUsartRx(void *data)
{
    u32 rx_bytes = 0x00;
    LOG_INFO(("<< %s >>\n", __func__));

    while(1)
    {
        rx_bytes = RS232_PollComport(sUsartPort, sUsartRxBuffer, sizeof(sUsartRxBuffer));
        if(rx_bytes > 0)
        {
            if(sLinuxUsartSetting->pRxFunc != '\0')
            {
                sLinuxUsartSetting->pRxFunc(sUsartRxBuffer, rx_bytes);
                memset(sUsartRxBuffer, 0x00, sizeof(sUsartRxBuffer));
            }
        }
    }

    pthread_exit(0);
}

static void StartUsartRxThread(void)
{
    int err = 0x00;

    LOG_INFO(("<< %s >>\n", __func__));

    err = pthread_create(&sUsartRxtid, NULL, &ThreadUsartRx, NULL);
    if (err != 0)
    {
        LOG_INFO(("can't create thread :[%s]\n", strerror(err)));
    }
}

static s16 LinuxSerialSend(u8 *pdata, u16 len)
{
    u16 count = 0x00;

    LOG_INFO(("<< %s >>\n", __func__));

    LOG_INFO(("\nSend Packet:: "));
    for(count = 0x00; count < len; count++)
    {
        LOG_INFO((" %x", pdata[count]));
    }

    RS232_SendBuf(sUsartPort, pdata, len);

    return 0;
}

s16 LinuxSerialInit(UsartConfigSetting *usart_setting)
{
    u32 bdrate=9600;

    LOG_INFO(("<< %s >>\n", __func__));

    usart_setting->pTxFunc = LinuxSerialSend;
    sLinuxUsartSetting = usart_setting;

    sUsartPort = RS232_OpenComport(sUsartPortFile, bdrate);
    if(sUsartPort < 0 )
    {
        LOG_ERR(("ERR:: RS232_OpenComport(): Can not open physical_port\n"));

        return -EUSART_PLATFORM_INIT_FUNC;
    }
    else
    {
        StartUsartRxThread();
        return -EUSART_OK;
    }
}

