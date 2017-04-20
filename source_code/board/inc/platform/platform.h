#ifndef _PLATFORM_H_
#define _PLATFORM_H_


#include <comman.h>


typedef struct
{
    void (*pRxFunc)(u8 *, u16);
    s16 (*pTxFunc)(u8 *, u16);
}PlatformOperations;


s16 PlatformSystemInit(void);
s16 PlatformPeripheralInit(void);

s16 XbeePlatformInit(PlatformOperations *platform_operations_data);

#endif
