
#include <xbee_stack.h>

/*
s16 ValidateApiFrame(u8 *apapi_frame);
void ExtractFrameData(u8 *apapi_frame, InfoFrameData *apframe_data);
void ProcessInfoFrameData(InfoFrameData *apframe_data);
*/


void testXbeeStack(void)
{
    XbeeStackInit('\0', '\0');
}
