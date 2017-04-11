
#include <xbee_at_command.h>

static ResisgerExpectedXbeeAtCmdResponse sRegisteredExpectedCmd = {true};
static u8 sAtCommandRequestApiPacketBuffer[API_PACKET_REQUEST_BUFFER_SIZE] = {0};

extern u16 ProcessApiFrameRequest(u8* pdata, u16 len);

void XbeeProcessAtCommandResponse(void *apdata)
{
    u16 len = 0x00;
    s16 cmd_data_len = 0x00;
    u8 *data = (u8 *)apdata;
    AtCommandResponse at_response;

    LOG_INFO0(("\n<< %s >>", __func__));

    len = (data[1] << 8) | data[2];
    at_response.frameId = data[4];
    at_response.atCommand = (data[5] << 8) | data[6];
    at_response.commandStatus = data[7];

    cmd_data_len = (len - 5);

    if(cmd_data_len > 0)
    {
        memcpy(at_response.commandData, &data[8], cmd_data_len);
    }

    if((sRegisteredExpectedCmd.validFlag == false) && (sRegisteredExpectedCmd.appXbeeAtCmdResponse.atCmdResponse.atCommand == at_response.atCommand))
    {
        sRegisteredExpectedCmd.appXbeeAtCmdResponse.atCmdResponse.frameId = at_response.frameId;
        sRegisteredExpectedCmd.appXbeeAtCmdResponse.atCmdResponse.commandStatus = at_response.commandStatus;
        if(cmd_data_len > 0)
        {
            memcpy(sRegisteredExpectedCmd.appXbeeAtCmdResponse.atCmdResponse.commandData, at_response.commandData, cmd_data_len);
            sRegisteredExpectedCmd.appXbeeAtCmdResponse.commandDataLen = cmd_data_len;
        }
        sRegisteredExpectedCmd.validFlag = true;
    }
    else
    {
        AppXbeeAtCommandResponse app_at_cmd_response;

        app_at_cmd_response.atCmdResponse.frameId = at_response.frameId;
        app_at_cmd_response.atCmdResponse.commandStatus = at_response.commandStatus;
        if(cmd_data_len > 0)
        {
            memcpy(app_at_cmd_response.atCmdResponse.commandData, at_response.commandData, cmd_data_len);
        }
        XbeeAtCommandEventCallBack(&app_at_cmd_response);
    }
}

s16 XbeeSendAtCommandRequest(AppXbeeAtCommandFrame *at_cmd_request)
{
    u16 len = (at_cmd_request->parameterLen + 3);           // frameId + AtCommand(2)

    LOG_INFO0(("\n<< %s >>", __func__));

    sAtCommandRequestApiPacketBuffer[3] = AT_COMMAND;
    sAtCommandRequestApiPacketBuffer[4] = at_cmd_request->atCmdFrame.frameId;
    sAtCommandRequestApiPacketBuffer[5] = (at_cmd_request->atCmdFrame.atCommand & 0xFF00) >> 8;
    sAtCommandRequestApiPacketBuffer[6] = (at_cmd_request->atCmdFrame.atCommand & 0xFF);
    if(at_cmd_request->parameterLen > 0)
    {
        memcpy(&sAtCommandRequestApiPacketBuffer[7], at_cmd_request->atCmdFrame.parameterValue, at_cmd_request->parameterLen);
    }

    return ProcessApiFrameRequest(sAtCommandRequestApiPacketBuffer, len);
}

s16 XbeeSendAtCommandRequestExpectedResponse(AppXbeeAtCommandFrame *app_at_cmd, AppXbeeAtCommandResponse *expected_response)
{
    u16 count = 0x00;
    s16 ret = 0x00;

    LOG_INFO0(("\n<< %s >>", __func__));

    ret = XbeeSendAtCommandRequest(app_at_cmd);
    if(ret != EXBEE_AT_CMD_OK)
    {
        LOG_ERR(("\nERR:: XbeeProcessAtCommandRequest():: %d", ret));
        return ret;
    }

    // register expected response
    if(sRegisteredExpectedCmd.validFlag == false)
    {
        LOG_ERR(("\nERR:: sRegisteredExpectedCmd already in use"));
        return -EXBEE_AT_CMD_EXPECTED_IN_USE;
    }

    sRegisteredExpectedCmd.validFlag = false;
    sRegisteredExpectedCmd.appXbeeAtCmdResponse.atCmdResponse.atCommand = expected_response->atCmdResponse.atCommand;

    while((count < 10) && (sRegisteredExpectedCmd.validFlag != true))
    {
        DelayUs(100);
    }

    if(sRegisteredExpectedCmd.validFlag == true)
    {
        expected_response->atCmdResponse.frameId = sRegisteredExpectedCmd.appXbeeAtCmdResponse.atCmdResponse.frameId;
        expected_response->atCmdResponse.commandStatus = sRegisteredExpectedCmd.appXbeeAtCmdResponse.atCmdResponse.commandStatus;
        if(sRegisteredExpectedCmd.appXbeeAtCmdResponse.commandDataLen > 0)
        {
            memcpy(expected_response->atCmdResponse.commandData, sRegisteredExpectedCmd.appXbeeAtCmdResponse.atCmdResponse.commandData, sRegisteredExpectedCmd.appXbeeAtCmdResponse.commandDataLen);
            expected_response->commandDataLen = sRegisteredExpectedCmd.appXbeeAtCmdResponse.commandDataLen;
        }
        sRegisteredExpectedCmd.validFlag = false;
        return EXBEE_AT_CMD_OK;
    }
    else
    {
        LOG_ERR(("\nERR:: Expected Response timeout()"));
        return -EXBEE_AT_CMD_EXPECTED_RESPONSE_TIMEOUT;
    }
}

s16 XbeeSendAtCommandExpectedResponse(AppXbeeAtCommandResponse *expected_response)
{
    u16 count = 0x00;

    LOG_INFO0(("\n<< %s >>", __func__));

    // register expected response
    if(sRegisteredExpectedCmd.validFlag == false)
    {
        LOG_ERR(("\nERR:: sRegisteredExpectedCmd already in use"));
        return -EXBEE_AT_CMD_EXPECTED_IN_USE;
    }

    sRegisteredExpectedCmd.validFlag = false;
    sRegisteredExpectedCmd.appXbeeAtCmdResponse.atCmdResponse.atCommand = expected_response->atCmdResponse.atCommand;

    while((count < 10) && (sRegisteredExpectedCmd.validFlag != true))
    {
        DelayUs(100);
    }

    if(sRegisteredExpectedCmd.validFlag == true)
    {
        expected_response->atCmdResponse.frameId = sRegisteredExpectedCmd.appXbeeAtCmdResponse.atCmdResponse.frameId;
        expected_response->atCmdResponse.commandStatus = sRegisteredExpectedCmd.appXbeeAtCmdResponse.atCmdResponse.commandStatus;
        if(sRegisteredExpectedCmd.appXbeeAtCmdResponse.commandDataLen > 0)
        {
            memcpy(expected_response->atCmdResponse.commandData, sRegisteredExpectedCmd.appXbeeAtCmdResponse.atCmdResponse.commandData, sRegisteredExpectedCmd.appXbeeAtCmdResponse.commandDataLen);
            expected_response->commandDataLen = sRegisteredExpectedCmd.appXbeeAtCmdResponse.commandDataLen;
        }
        sRegisteredExpectedCmd.validFlag = false;
        return EXBEE_AT_CMD_OK;
    }
    else
    {
        LOG_ERR(("\nERR:: Expected Response timeout()"));
        return -EXBEE_AT_CMD_EXPECTED_RESPONSE_TIMEOUT;
    }
}

