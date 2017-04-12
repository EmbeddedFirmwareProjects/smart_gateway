
#include <timer.h>

#include <unistd.h>


void DelayUs(u32 us)
{
    usleep(us);
}

void DelayMs(u32 ms)
{
    usleep(ms*1000);
}

void DelaySec(u32 sec)
{
    sleep(sec);
}



