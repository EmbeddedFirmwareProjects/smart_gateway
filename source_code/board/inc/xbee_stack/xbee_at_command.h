#ifndef XBEE_AT_COMMAND_H
#define XBEE_AT_COMMAND_H

#include <xbee_stack.h>

typedef struct
{
    AtCommandResponse atCmdResponse;
    u8 commandDataLen;
}AppXbeeAtCommandResponse;

typedef struct
{
    u8 availableFlag;
    u8 validFlag;
    AppXbeeAtCommandResponse appXbeeAtCmdResponse;
}ResisgerExpectedXbeeAtCmdResponse;

typedef struct
{
    AtCommandFrame atCmdFrame;
    u8 parameterLen;
}AppXbeeAtCommandFrame;

s16 XbeeSendAtCommandRequest(AppXbeeAtCommandFrame *at_cmd);
s16 XbeeSendAtCommandRequestExpectedResponse(AppXbeeAtCommandFrame *at_cmd, AppXbeeAtCommandResponse *expected_response, u16 timeout_ms);

s16 XbeeAtCommandExpectedResponse(AppXbeeAtCommandResponse *expected_response, u16 timeout_ms);
void XbeeAtCommandEventCallBack(AppXbeeAtCommandResponse *at_cmd_response);     // should be defined in application layer

#endif
