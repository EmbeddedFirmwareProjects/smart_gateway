#include <stdio.h>
#include <xbee_comman.h>
#include <platform.h>
#include <test_xbee_stack.h>

int main (void) 
{
    PlatformSystemInit();

    PlatformPeripheralInit();

    testXbeeStack();

    printf("\n\n");
    return 0;
}

void XbeeAtCommandEventCallBack(AppXbeeAtCommandResponse *at_cmd_response)
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

void XbeeZigbeeReceiveEventCallBack(AppXbeeZigbeeReceivePacket *zigbee_response)
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
