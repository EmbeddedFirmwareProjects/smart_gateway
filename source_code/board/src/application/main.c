#include <stdio.h>
#include <platform.h>
#include <xbee_comman.h>


int main (void) 
{
    PlatformSystemInit();

    PlatformPeripheralInit();

    XbeeStackInit();
    XbeeZigbeeOperationsInit();

    while(1)
    {
        XbeeHouseKeeping();
        XbeeZigbeeHouseKeeping();
        DelaySec(1);
    }

    printf("\n\n");
    return 0;
}
