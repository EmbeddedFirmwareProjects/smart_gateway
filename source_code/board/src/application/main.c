#include <stdio.h>
#include <platform.h>
#include <xbee_application.h>


int main (void) 
{
    PlatformSystemInit();

    PlatformPeripheralInit();

    XbeeApplicationInit();

    while(1)
    {
        XbeeHouseKeeping();
        DelayMs(200);
    }

    printf("\n\n");
    return 0;
}
