#ifndef _PLATFORM_USART_H_
#define _PLATFORM_USART_H_


#include <platform.h>


typedef enum
{
    CHANNEL_XBEE,
    CHANNEL_USER_COMMN,

    UsartChannel_MAX
}UsartChannel;

typedef enum
{
    EUSART_OK = 0x00,
    EUSART_INVALID_PLATFORM_OPERATIONS,
    EUSART_INVALID_CHANNEL_OPERATIONS,
    EUSART_INVALID_INIT_PFUNC,
    EUSART_PLATFORM_INIT_FUNC,

    PlatformUsartError_MAX,
}PlatformUsartError;


s16 PlatformUsartInit(u8 usart_channel, PlatformOperations *platform_operations);


#endif
