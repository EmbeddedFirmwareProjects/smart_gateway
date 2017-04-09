
#include <xbee_stack.h>

static void processAtCommandResponse(InfoFrameData *apframe_frame)
{
    switch(apframe_frame->frameData.IdentifierSpecificData.AtCommandResponse.atCommand)
    {
        case AtCommandList_MAX:
        default:
        {
            // command not hadled
        }
        break;
    }
}

u8 calculateCheckSum(u8 *apapi_frame, u16 len)
{
    long check_sum = 0x00;
    
    while(len >= 0)
    {
        check_sum = check_sum + apapi_frame[len];
        --len;
    }
    check_sum = 0XFF - (check_sum & 0xFF);
    
    return (check_sum & 0xFF);
}

s16 ValidateApiFrame(u8 *apapi_frame)
{
    u16 len = 0x00;
    u8 cal_check_sum = 0x00;
    
    // verify start deliminator
    if(apapi_frame[0] != API_FRAME_START_DELIMITER) 
    {
        return -1;
    }
    
    // verify checkSum
    len = (apapi_frame[1] << 8) | apapi_frame[2];
    cal_check_sum = calculateCheckSum(&apapi_frame[3], (len - 1));  // exclude checkSum
    
    if(cal_check_sum == apapi_frame[(len + 3) - 1])     // 
    {
        return 0;
    }
    else
    {
        return -2;
    }
}


void ExtractFrameData(u8 *apdata, InfoFrameData *apframe_data)
{
    
}

void ProcessInfoFrameData(InfoFrameData *apframe_data)
{
    switch(apframe_data->frameData.cmdId)
    {
        case AT_COMMAND_RESPONSE:
        {
            processAtCommandResponse(apframe_data);
        }
        break;
        
        default:
        {
            // command not hadled
        }
        break;
    }
}
