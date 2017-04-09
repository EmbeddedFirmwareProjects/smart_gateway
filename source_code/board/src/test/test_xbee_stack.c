
#include <xbee_stack.h>

/*
s16 ValidateApiFrame(u8 *apapi_frame);
void ExtractFrameData(u8 *apapi_frame, InfoFrameData *apframe_data);
void ProcessInfoFrameData(InfoFrameData *apframe_data);
*/

static void TestValidateApiFrame();

void testXbeeStack(void)
{
    XbeeStackInit();
    TestValidateApiFrame();
}

static void TestValidateApiFrame()
{    
    LOG_INFO0(("\n\n----------------- %s -----------------", __func__));
    
    LOG_INFO(("\n\nTestValidFrame:: "));
    u8 api_frame[] = {0x7E, 0x00, 0x05, AT_COMMAND_RESPONSE, 0x01, 0x42, 0x44, 0x00, 0xF0};
    ProcessApiFrame(api_frame, (u16)sizeof(api_frame));

    LOG_INFO(("\n\nTestInvalidStartDelimter:: "));
    u8 api_frame1[] = {0x7B, 0x00, 0x05, AT_COMMAND_RESPONSE, 0x01, 0x42, 0x44, 0x00, 0xF0};
    ProcessApiFrame(api_frame1, (u16)sizeof(api_frame1));
    
    LOG_INFO(("\n\nTestInvalidCheckSum:: "));
    u8 api_frame2[] = {0x7E, 0x00, 0x05, AT_COMMAND_RESPONSE, 0x01, 0x42, 0x44, 0x00, 0xFF};
    ProcessApiFrame(api_frame2, (u16)sizeof(api_frame2));

}