
#include <c_buffer.h>

static Cbuffer sCbufferUart1 = {CBUFFER_UART1, 0x00, 0x00};


void PushToBuffer(CbufferSelect buffer_select, void *data, u16 data_len)
{
    Cbuffer *pcbuffer = '\0';

    if (data == '\0')
    {
        return;
    }

    switch(buffer_select) 
    {
        case CBUFFER_UART1:
        {
            pcbuffer = &sCbufferUart1;
        }
        break;
        
        default:
        {
            pcbuffer = '\0';
        }
        break;
    }
    
    if(pcbuffer == '\0')
    {
        return;
    }
    
    if((pcbuffer->pushPointer + data_len) < CBUFFER_UART_LEN)
    {
        if((pcbuffer->pushPointer + data_len) < pcbuffer->getPointer)
        {
            memcpy(&pcbuffer->buffer[pcbuffer->pushPointer], data, data_len);
            pcbuffer->pushPointer += data_len;
            if((pcbuffer->pushPointer == pcbuffer->getPointer))
            {
                pcbuffer->getPointer = pcbuffer->pushPointer + 1;
            }
        }
        else 
        {
            memcpy(&pcbuffer->buffer[pcbuffer->pushPointer], data, data_len);
            pcbuffer->pushPointer += data_len;
            pcbuffer->getPointer = pcbuffer->pushPointer + 1;
        }
    }
    else if((pcbuffer->pushPointer + data_len) > CBUFFER_UART_LEN)
    {
        u16 first_push = CBUFFER_UART_LEN - pcbuffer->pushPointer;
        u16 second_push = data_len - first_push;
        
        memcpy(&pcbuffer->buffer[pcbuffer->pushPointer], data, first_push);
        memcpy(&pcbuffer->buffer[0], &((u8 *)&data)[first_push], second_push);
        
        pcbuffer->pushPointer = second_push - 1;
        
        if((pcbuffer->getPointer <= pcbuffer->pushPointer))
        {
            pcbuffer->getPointer = pcbuffer->pushPointer + 1;
        }
    }
    else
    {
        memcpy(&pcbuffer->buffer[pcbuffer->pushPointer], data, data_len);
        pcbuffer->pushPointer += data_len;
        pcbuffer->getPointer = 0;
    }
}

u16 GetFromBuffer(CbufferSelect buffer_select, void *data)
{
    
    return 0;
}



