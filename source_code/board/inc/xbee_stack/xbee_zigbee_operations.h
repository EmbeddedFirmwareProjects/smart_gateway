#ifndef XBEE_ZIGBEE_OPERATIONS_H
#define XBEE_ZIGBEE_OPERATIONS_H

#include <xbee_stack.h>

typedef struct
{
    ZigbeeReceivePacket rfPacketResponse;
    u8 receiveDataLen;
}AppXbeeZigbeeReceivePacket;

typedef struct
{
    ZigbeeTransmitRequest rfTransmitRequest;
    u8 rfDataLen;
}AppXbeeZigbeeTransmitRequest;


void XbeeZigbeeInit(void);
s16 XbeeSendZigbeeTransmitRequest(AppXbeeZigbeeTransmitRequest *zigbee_tx_request);
void XbeeZigbeeReceiveEventHandler(AppXbeeZigbeeReceivePacket *zigbee_response);     // should be defined in application layer

#endif
