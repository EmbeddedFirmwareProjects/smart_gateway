
#include <xbee_stack.h>

static void processAtCommandResponse(ApiFrame *apapi_frame)
{
    switch(apapi_frame->farmeData.cmdData.AtCommandResponse.atCommand)
    {
        case AtCommandList_MAX:
        default:
        {
            // command not hadled
        }
        break;
    }
}

static void ProcessFrameData(ApiFrame *apapi_frame)
{
    switch(apapi_frame->farmeData.cmdId)
    {
        case AT_COMMAND_RESPONSE:
        {
            processAtCommandResponse(apapi_frame);
        }
        break;
        
        default:
        {
            // command not hadled
        }
        break;
    }
}

void GenerateApiFrame(u8 *apdata)
{

}