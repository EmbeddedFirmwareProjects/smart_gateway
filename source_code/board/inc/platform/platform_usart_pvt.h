#ifndef _PLATFORM_USART_PVT_H_
#define _PLATFORM_USART_PVT_H_

#define USART_RX_BUFFER_LEN     200

typedef enum
{
    CHANNEL_XBEE,
    CHANNEL_USER_COMMN,

    UsartChannel_MAX
}UsartChannel;

typedef enum
{
    USART_BAUDRATE_9600,
    UsartBaudrate_MAX
}UsartBaudrate;

typedef enum
{
    USART_PARITY_ODD,
    USART_PARITY_EVEN,
    USART_PARITY_SPACE,
    USART_PARITY_NO,
    USART_PARITY_MULTIDROP,
    UsartParity_MAX
}UsartParity;

typedef enum
{
    USART_STOPBIT_1,
    USART_STOPBIT_2,
    USART_STOPBIT_1_5,
    UsartStopBit_MAX
}UsartStopBit;

typedef struct
{
    UsartChannel channel;
    UsartBaudrate baudRate;
    UsartParity parity;
    UsartStopBit stopBit;
    void (*pRxFunc)(u8 *, u16);
}UsartConfigSetting;

#endif
