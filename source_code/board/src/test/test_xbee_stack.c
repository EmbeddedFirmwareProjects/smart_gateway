
#include <xbee_stack.h>
#include <xbee_stack.h>
#include <xbee_at_command.h>

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
            LOG_DBG((" %x", at_cmd_response->atCmdResponse.commandData[count]));
        }
    }
}

static void TestProcessApiFrameResponse()
{
    LOG_INFO(("\n\n----------------- %s -----------------", __func__));

    LOG_INFO(("\nTestValidFrame:: "));
    u8 api_frame[] = {0x7E, 0x00, 0x07, AT_COMMAND_RESPONSE, 0x01, 'D', 'H', 0x00, 0x02, 0x01, 0xe7};
    ProcessApiFrameResponse(api_frame, (u16)sizeof(api_frame));
}

static void TestXbeeSendAtCommandRequest()
{
    u8 param[] = {0x02, 0x01};
    AppXbeeAtCommandFrame at_cmd;

    LOG_INFO(("\n\n----------------- %s -----------------", __func__));

    at_cmd.atCmdFrame.frameId = 0x01;
    at_cmd.atCmdFrame.atCommand = 0x4448;
    at_cmd.atCmdFrame.parameterValue = &param;
    at_cmd.parameterLen = 2;
    XbeeSendAtCommandRequest(&at_cmd);
}

void testXbeeStack(void)
{
    XbeeStackInit();

    TestProcessApiFrameResponse();
    TestXbeeSendAtCommandRequest();

}
