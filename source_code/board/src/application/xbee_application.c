
#include <xbee_comman.h>
#include <test_xbee_stack.h>
#include <platform.h>


static void NotifyXbeeApiFrameResponse(u8 *pdata, u16 len);


static u8 sXbeeRxBuffer[API_FRAME_RESPONSE_BUFFER_SIZE] = {0};
static u16 sXbeePushPointer = 0x00;
static u16 sXbeeFrameLen = 0x00;

static PlatformOperations sXbeePlatformOperations = {
        .pRxFunc = NotifyXbeeApiFrameResponse,
};


s16 XbeeApplicationInit(void)
{
    return XbeePlatformInit(&sXbeePlatformOperations);
}

void XbeeHouseKeeping(void)
{
    //testXbeeStack();
}

static void NotifyXbeeApiFrameResponse(u8 *pdata, u16 len)
{
    u16 count = 0x00;

    LOG_INFO0(("\n<< %s >>", __func__));

    if (pdata == '\0')      // reset previous incomplete packet on receive failure
    {
        sXbeePushPointer = 0x00;
        sXbeeFrameLen = 0x00;
        return ;
    }

    for(count = 0x00; count < len; count++)
    {
        if((pdata[count] == XBEE_API_FRAME_START_DELIMITER) && (sXbeePushPointer == 0x00))
        {
            memset(sXbeeRxBuffer, 0x00, sizeof(sXbeeRxBuffer));
            sXbeeRxBuffer[sXbeePushPointer++] = pdata[count];
        }
        else if(sXbeePushPointer > 0x00)
        {
            sXbeeRxBuffer[sXbeePushPointer] = pdata[count];

            if(sXbeePushPointer == 0x01)
            {
                sXbeeFrameLen |= (pdata[count] << 8) & 0xFF00;
            }
            else if(sXbeePushPointer == 0x02)
            {
                sXbeeFrameLen |= pdata[count] & 0x00FF;
            }
            if(sXbeePushPointer > 2)
            {
                if(((sXbeeFrameLen + 4) - 1) == sXbeePushPointer)     // start delimiter(1) + len(2) + data bytes(n) + checksum(1)
                {
                    XbeeProcessApiFrameResponse(sXbeeRxBuffer, sXbeePushPointer);
                    sXbeePushPointer = 0x00;
                    sXbeeFrameLen = 0x00;
                }
                else
                {
                    sXbeePushPointer++;
                }
            }
            else
            {
                sXbeePushPointer++;
            }
        }
    }
}

s16 XbeeSendApiFrameRequest(u8* pdata, u16 len)
{
    LOG_INFO0(("\n<< %s >>", __func__));

    return sXbeePlatformOperations.pTxFunc(pdata, len);
}

void XbeeAtCommandEventHandler(AppXbeeAtCommandResponse *at_cmd_response)
{
    u16 count = 0x00;
    LOG_INFO(("\n<< %s >>", __func__));

    LOG_INFO(("\nRx AT Command:: "));
    LOG_INFO((" %x", at_cmd_response->atCmdResponse.frameId));
    LOG_INFO((" %x", at_cmd_response->atCmdResponse.atCommand >> 8));
    LOG_INFO((" %x", at_cmd_response->atCmdResponse.atCommand & 0xFF));

    if(at_cmd_response->commandDataLen > 0)
    {
        for(count = 0x00; count < at_cmd_response->commandDataLen; count++)
        {
            LOG_INFO((" %x", at_cmd_response->atCmdResponse.commandData[count]));
        }
    }
}

void XbeeZigbeeReceiveEventHandler(AppXbeeZigbeeReceivePacket *zigbee_response)
{
    u16 count = 0x00;
    LOG_INFO(("\n<< %s >>", __func__));

    LOG_INFO(("\nRx ZigbeePacket::"));
    for(count = 0x00; count < ZIGBEE_RECEIVE_PACKET_SOURCE_ADDRESS_LEN; count++)
    {
        LOG_INFO((" %x", zigbee_response->rfPacketResponse.sourceAdress[count]));
    }
    LOG_INFO((" %x", (zigbee_response->rfPacketResponse.sourceNetworkAddress >> 8) & 0xFF));
    LOG_INFO((" %x", (zigbee_response->rfPacketResponse.sourceNetworkAddress) & 0xFF));
    LOG_INFO((" %x", zigbee_response->rfPacketResponse.receiveOption));

    if(zigbee_response->receiveDataLen > 0)
    {
        for(count = 0x00; count < zigbee_response->receiveDataLen; count++)
        {
            LOG_INFO((" %x", zigbee_response->rfPacketResponse.receiveData[count]));
        }
    }
}
