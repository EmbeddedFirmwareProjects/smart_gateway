#include <comman.h>
#include <platform_usart_pvt.h>
#include <xbee_comman.h>

static void CollectXbeePacket(u8 *pdata, u16 len);

const UsartConfigSetting sUsartConfigList[] = {
    {
        .channel = CHANNEL_XBEE,
        .baudRate = USART_BAUDRATE_9600,
        .parity = USART_PARITY_NO,
        .stopBit = USART_STOPBIT_1,
        .pRxFunc = CollectXbeePacket,
    },
    {
        .channel = CHANNEL_USER_COMMN,
        .baudRate = USART_BAUDRATE_9600,
        .parity = USART_PARITY_NO,
        .stopBit = USART_STOPBIT_1,
    },
};

static void CollectXbeePacket(u8 *pdata, u16 len)
{
    static u8 rxBuffer[USART_RX_BUFFER_LEN] = {0};
    static u16 pushPointer = 0x00;
    static u16 packet_len = 0x00;

    u16 count = 0x00;

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
                if((packet_len + 4) == pushPointer)     // start delimiter(1) + len(2) + data bytes(n) + checksum(1)
                {
                    ProcessApiFrameResponse(rxBuffer, pushPointer);
                    pushPointer = 0x00;
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

void PlatformUsartInit(void)
{
    u16 count = 0x00;
    u16 config_list = 0x00;

    config_list = sizeof(sUsartConfigList)/sizeof(UsartConfigSetting);

    for(count = 0x00; count < config_list; count++)
    {

    }
}
