#include <stdio.h>
#include <xbee_stack.h>
#include <xbee_at_command.h>

#include <test_xbee_stack.h>

s16 SendApiFrameRequest(u8* pdata, u16 len)
{
    return 0;
}

void XbeeAtCommandEventCallBack(AppXbeeAtCommandResponse *at_cmd_response)
{

}

int main (void) 
{
    XbeeStackInit();

    printf("\n\n");
    return 0;
}
