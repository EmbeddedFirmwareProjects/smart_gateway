#include <stdio.h>
#include <xbee_at_command.h>
#include <xbee_zigbee_operations.h>
#include <test_xbee_stack.h>


int main (void) 
{
    testXbeeStack();

    printf("\n\n");
    return 0;
}

s16 SendApiFrameRequest(u8* pdata, u16 len)
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

}
