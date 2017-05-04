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
    AtCommandFrame atCmdFrame;
    u8 parameterLen;
}AppXbeeAtCommandFrame;

s16 XbeeSendAtCommandRequest(AppXbeeAtCommandFrame *at_cmd);
void XbeeAtCommandEventHandler(AppXbeeAtCommandResponse *at_cmd_response);     // should be defined in application layer

#endif
