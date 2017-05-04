#include <stdio.h>
#include <platform.h>
#include <xbee_comman.h>


int main (void) 
{
    PlatformSystemInit();

    PlatformPeripheralInit();

    XbeeStackInit();

    while(1)
    {
        DelayMs(200);
    }

    printf("\n\n");
    return 0;
}
