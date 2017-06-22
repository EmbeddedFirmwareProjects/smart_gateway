
#include <xbee_at_command.h>

static u8 sAtCommandRequestApiPacketBuffer[API_FRAME_REQUEST_BUFFER_SIZE] = {0};

void XbeeProcessAtCommandResponse(u8 *data)
{
    u16 len = 0x00;
    s16 cmd_data_len = 0x00;
    AppXbeeAtCommandResponse app_at_cmd_response = {{0}};

    LOG_INFO0(("\n<< %s >>", __func__));

    len = (data[1] << 8) | data[2];
    app_at_cmd_response.atCmdResponse.frameId = data[4];
    app_at_cmd_response.atCmdResponse.atCommand = (data[5] << 8) | data[6];
    app_at_cmd_response.atCmdResponse.commandStatus = data[7];

    // cmd + frameId + atCommand(2) + commandStatus
    cmd_data_len = (len - 5);

    if(cmd_data_len > 0)
    {
        app_at_cmd_response.atCmdResponse.commandData = &data[8];
        app_at_cmd_response.commandDataLen = cmd_data_len;
        XbeeAtCommandEventHandler(&app_at_cmd_response);
    }
    else
    {
        LOG_ERR(("\nERR:: XbeeProcessAtCommandResponse() Invalid rcv_data_len: %d", cmd_data_len));
    }
}

s16 XbeeSendAtCommandRequest(AppXbeeAtCommandFrame *at_cmd_request)
{
    // cmd + frameId + AtCommand(2)
    u16 len = (at_cmd_request->parameterLen + 4);

    LOG_INFO0(("\n<< %s >>", __func__));

    sAtCommandRequestApiPacketBuffer[3] = AT_COMMAND;
    sAtCommandRequestApiPacketBuffer[4] = FrameIdCounter++;
    sAtCommandRequestApiPacketBuffer[5] = (at_cmd_request->atCmdFrame.atCommand & 0xFF00) >> 8;
    sAtCommandRequestApiPacketBuffer[6] = (at_cmd_request->atCmdFrame.atCommand & 0xFF);
    if(at_cmd_request->parameterLen > 0)
    {
        memcpy(&sAtCommandRequestApiPacketBuffer[7], at_cmd_request->atCmdFrame.parameterValue, at_cmd_request->parameterLen);
    }
    return XbeeSendApiFrameRequest(sAtCommandRequestApiPacketBuffer, len);
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
