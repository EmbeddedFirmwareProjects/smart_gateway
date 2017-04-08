#ifndef C_BUFFER_H
#define C_BUFFER_H

#include <comman.h>

#define CBUFFER_UART_LEN 200

typedef enum
{
    CBUFFER_UART1,
}CbufferSelect;

typedef struct
{
    CbufferSelect select;
    u16 pushPointer;
    u16 getPointer;
    u8 buffer[CBUFFER_UART_LEN];
}Cbuffer;

void PushToBuffer(CbufferSelect buffer_select, void *data, u16 data_len);
u16 GetFromBuffer(CbufferSelect buffer_select, void *data);

#endif

