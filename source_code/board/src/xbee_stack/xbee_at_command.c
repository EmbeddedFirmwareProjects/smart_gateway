
#include <xbee_at_command.h>

static ResisgerExpectedAtCmdResponse sRegisteredExpectedCmd = {true};

void ProcessAtCommandResponse(void *apdata)
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

    if((sRegisteredExpectedCmd.validFlag == false) && (sRegisteredExpectedCmd.appAtCmdResponse.atCmdResponse.atCommand == at_response.atCommand))
    {
		sRegisteredExpectedCmd.appAtCmdResponse.atCmdResponse.frameId = at_response.frameId;
		sRegisteredExpectedCmd.appAtCmdResponse.atCmdResponse.commandStatus = at_response.commandStatus;
		if(cmd_data_len > 0)
		{
			memcpy(sRegisteredExpectedCmd.appAtCmdResponse.atCmdResponse.commandData, at_response.commandData, cmd_data_len);
			sRegisteredExpectedCmd.appAtCmdResponse.commandDataLen = cmd_data_len;
		}
		sRegisteredExpectedCmd.validFlag = true;
    }
    else
    {
		switch(at_response.atCommand)
		{
			case DH:
			{

			}
			break;

			default:
			{

			}
			break;
		}
    }
}

s16 SendAtCommand(AppAtCommandFrame *at_cmd)
{
	return 0;
}

s16 SendAtCommandWithExpectedResponse(AppAtCommandFrame *at_cmd, AppAtCommandResponse *expected_response)
{
	u16 count = 0x00;

	//send at cmd


	// register expected response
	if(sRegisteredExpectedCmd.validFlag == false)
	{
		return -EAT_CMD_EXPECTED_IN_USE;
	}

	sRegisteredExpectedCmd.validFlag = false;
	sRegisteredExpectedCmd.appAtCmdResponse.atCmdResponse.atCommand = expected_response->atCmdResponse.atCommand;

	while((count < 10) && (sRegisteredExpectedCmd.validFlag != true))
	{
		DelayUs(100);
	}

	if(sRegisteredExpectedCmd.validFlag == true)
	{
		expected_response->atCmdResponse.frameId = sRegisteredExpectedCmd.appAtCmdResponse.atCmdResponse.frameId;
		expected_response->atCmdResponse.commandStatus = sRegisteredExpectedCmd.appAtCmdResponse.atCmdResponse.commandStatus;
		if(sRegisteredExpectedCmd.appAtCmdResponse.commandDataLen > 0)
		{
			memcpy(expected_response->atCmdResponse.commandData, sRegisteredExpectedCmd.appAtCmdResponse.atCmdResponse.commandData, sRegisteredExpectedCmd.appAtCmdResponse.commandDataLen);
			expected_response->commandDataLen = sRegisteredExpectedCmd.appAtCmdResponse.commandDataLen;
		}
		sRegisteredExpectedCmd.validFlag = false;
		return AT_CMD_OK;
	}
	else
	{
		return -EAT_CMD_EXPECTED_RESPONSE_TIMEOUT;
	}
}

s16 AtCommandExpectedResponse(AppAtCommandResponse *expected_response)
{
	u16 count = 0x00;

	LOG_INFO0(("\n<< %s >>", __func__));

	// register expected response
	if(sRegisteredExpectedCmd.validFlag == false)
	{
		return -EAT_CMD_EXPECTED_IN_USE;
	}

	sRegisteredExpectedCmd.validFlag = false;
	sRegisteredExpectedCmd.appAtCmdResponse.atCmdResponse.atCommand = expected_response->atCmdResponse.atCommand;

	while((count < 10) && (sRegisteredExpectedCmd.validFlag != true))
	{
		DelayUs(100);
	}

	if(sRegisteredExpectedCmd.validFlag == true)
	{
		expected_response->atCmdResponse.frameId = sRegisteredExpectedCmd.appAtCmdResponse.atCmdResponse.frameId;
		expected_response->atCmdResponse.commandStatus = sRegisteredExpectedCmd.appAtCmdResponse.atCmdResponse.commandStatus;
		if(sRegisteredExpectedCmd.appAtCmdResponse.commandDataLen > 0)
		{
			memcpy(expected_response->atCmdResponse.commandData, sRegisteredExpectedCmd.appAtCmdResponse.atCmdResponse.commandData, sRegisteredExpectedCmd.appAtCmdResponse.commandDataLen);
			expected_response->commandDataLen = sRegisteredExpectedCmd.appAtCmdResponse.commandDataLen;
		}
		sRegisteredExpectedCmd.validFlag = false;
		return AT_CMD_OK;
	}
	else
	{
		return -EAT_CMD_EXPECTED_RESPONSE_TIMEOUT;
	}
}
