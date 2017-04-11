#ifndef XBEE_AT_COMMAND_H
#define XBEE_AT_COMMAND_H

#include <xbee_stack.h>

typedef enum
{
    EXBEE_AT_CMD_OK= 0x00,
    EXBEE_AT_CMD_INVALID,
    EXBEE_AT_CMD_EXPECTED_RESPONSE_TIMEOUT,
    EXBEE_AT_CMD_EXPECTED_IN_USE,
    EXBEE_AT_CMD_SEND_ERROR,

    ErrXbeeAtCommand_MAX,
}ErrXbeeAtCommand;


typedef struct
{
    AtCommandResponse atCmdResponse;
    u8 commandDataLen;
}AppXbeeAtCommandResponse;

typedef struct
{
    u8 validFlag;
    AppXbeeAtCommandResponse appXbeeAtCmdResponse;
}ResisgerExpectedXbeeAtCmdResponse;

typedef struct
{
    AtCommandFrame atCmdFrame;
    u8 parameterLen;
}AppXbeeAtCommandFrame;

s16 XbeeSendAtCommandRequest(AppXbeeAtCommandFrame *at_cmd);
s16 XbeeSendAtCommandRequestExpectedResponse(AppXbeeAtCommandFrame *at_cmd, AppXbeeAtCommandResponse *expected_response);

s16 XbeeAtCommandExpectedResponse(AppXbeeAtCommandResponse *expected_response);
void XbeeAtCommandEventCallBack(AppXbeeAtCommandResponse *at_cmd_response);     // should be defined in application layer

#endif
