#include <comman.h>
#include <platform_usart_pvt.h>
#include <xbee_comman.h>

extern void LinuxSerialInit(UsartConfigSetting *usart_setting);

static void CollectXbeePacket(u8 *pdata, u16 len);

static UsartConfigSetting sUsartConfigList[] = {
    {
        .channel = CHANNEL_XBEE,
        .baudRate = USART_BAUDRATE_9600,
        .parity = USART_PARITY_NO,
        .stopBit = USART_STOPBIT_1,
        .pRxFunc = CollectXbeePacket,
        .pInitFunc = LinuxSerialInit,
    },
/*    {
        .channel = CHANNEL_USER_COMMN,
        .baudRate = USART_BAUDRATE_9600,
        .parity = USART_PARITY_NO,
        .stopBit = USART_STOPBIT_1,
    },*/
};

static void CollectXbeePacket(u8 *pdata, u16 len)
{
    static u8 rxBuffer[USART_RX_BUFFER_LEN] = {0};
    static u16 pushPointer = 0x00;
    static u16 packet_len = 0x00;

    u16 count = 0x00;

    LOG_INFO0(("\n<< %s >>", __func__));

    if (pdata == '\0')      // reset previous incomplete packet on receive failure
    {
        pushPointer = 0x00;
        packet_len = 0x00;
        return ;
    }

    for(count = 0x00; count < len; count++)
    {
        if((pdata[count] == XBEE_API_FRAME_START_DELIMITER) && (pushPointer == 0x00))
        {
            memset(rxBuffer, 0x00, sizeof(rxBuffer));
            rxBuffer[pushPointer++] = pdata[count];
        }
        else if(pushPointer > 0x00)
        {
            rxBuffer[pushPointer] = pdata[count];

            if(pushPointer == 0x01)
            {
                packet_len |= (pdata[count] << 8) & 0xFF00;
            }
            else if(pushPointer == 0x02)
            {
                packet_len |= pdata[count] & 0x00FF;
            }
            if(pushPointer > 2)
            {
                if(((packet_len + 4) - 1) == pushPointer)     // start delimiter(1) + len(2) + data bytes(n) + checksum(1)
                {
                    XbeeProcessApiFrameResponse(rxBuffer, pushPointer);
                    pushPointer = 0x00;
                    packet_len = 0x00;
                }
                else
                {
                    pushPointer++;
                }
            }
            else
            {
                pushPointer++;
            }
        }
    }
}

s16 XbeeSendApiFrameRequest(u8* pdata, u16 len)
{
    u16 count = 0x00;
    u16 config_list = 0x00;

    LOG_INFO0(("\n<< %s >>", __func__));

    config_list = sizeof(sUsartConfigList)/sizeof(UsartConfigSetting);

    for(count = 0x00; count < config_list; count++)
    {
        if(sUsartConfigList[count].channel == CHANNEL_XBEE)
        {
            if(sUsartConfigList[count].pTxFunc != '\0')
            {
                return sUsartConfigList[count].pTxFunc(pdata, len);
            }
            else
            {
                LOG_ERR(("\nERR:: pTxFunc Not registered for channel %d", sUsartConfigList[count].channel));
                return -EXBEE_ZIGBEE_TX_REQUEST;
            }
        }
    }
    return 0;
}

void PlatformUsartInit(void)
{
    u16 count = 0x00;
    u16 config_list = 0x00;

    LOG_INFO0(("\n<< %s >>", __func__));

    config_list = sizeof(sUsartConfigList)/sizeof(UsartConfigSetting);

    for(count = 0x00; count < config_list; count++)
    {
        if(sUsartConfigList[count].pInitFunc != '\0')
        {
            return sUsartConfigList[count].pInitFunc(&sUsartConfigList[count]);
        }
    }
}

void testCollectXbeePacket()
{
    u8 api_frame[] = {0x32, 0x12, 0x34, 0x7E, 0x00, 0x07, AT_COMMAND_RESPONSE, 0x01, 'D', 'H', 0x00, 0x02, 0x01, 0xe7, 0x02, 0x01, 0xe7, 0x7E, 0x00, 0x07, AT_COMMAND_RESPONSE, 0x01, 'D', 'H', 0x00, 0x02, 0x01, 0xe7};
    CollectXbeePacket(api_frame, sizeof(api_frame));
}
