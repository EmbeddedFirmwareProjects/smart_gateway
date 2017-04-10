#ifndef XBEE_AT_COMMAND_H
#define XBEE_AT_COMMAND_H

#include <xbee_stack.h>

typedef enum
{
	AT_CMD_OK 							= 0x00,
	EAT_CMD_INVALID,
	EAT_CMD_EXPECTED_RESPONSE_TIMEOUT,
	EAT_CMD_EXPECTED_IN_USE,
	EAT_CMD_SEND_ERROR,

	ErrAtCommand_MAX,
}ErrAtCommand;

typedef struct
{
	AtCommandResponse atCmdResponse;
	u8 commandDataLen;
}AppAtCommandResponse;

typedef struct
{
	u8 validFlag;
	AppAtCommandResponse appAtCmdResponse;
}ResisgerExpectedAtCmdResponse;

typedef struct
{
	AtCommandFrame atCmdFrame;
    u8 parameterLen;
}AppAtCommandFrame;

s16 SendAtCommand(AppAtCommandFrame *at_cmd);
s16 SendAtCommandWithExpectedResponse(AppAtCommandFrame *at_cmd, AppAtCommandResponse *expected_response);
s16 AtCommandExpectedResponse(AppAtCommandResponse *expected_response);

void ProcessAtCommandResponse(void *apdata);

#endif
