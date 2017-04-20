#include <stdio.h>
#include <platform.h>
#include <xbee_application.h>


int main (void) 
{
    PlatformSystemInit();

    PlatformPeripheralInit();

    XbeeApplicationInit();

    XbeeHouseKeeping();

    printf("\n\n");
    return 0;
}
