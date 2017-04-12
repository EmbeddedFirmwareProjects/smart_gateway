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
    u8 availableFlag;
    u8 validFlag;
    AppXbeeZigbeeReceivePacket appXbeeZigbeeReceivePacket;
}ResisgerExpectedZigbeeRfCommandResponse;

typedef struct
{
    u8 availableFlag;
    u8 validFlag;
    ZigbeeTransmitStatus zigbeeTransmitStatus;
}ResisgerZigbeeTransmitRequestStatus;

typedef struct
{
    ZigbeeTransmitRequest rfTransmitRequest;
    u8 rfDataLen;
}AppXbeeZigbeeTransmitRequest;


s16 XbeeSendZigbeeTransmitRequest(AppXbeeZigbeeTransmitRequest *zigbee_tx_request, u16 timeout_ms);
s16 XbeeSendZigbeeTransmitRequestExpectedResponse(AppXbeeZigbeeTransmitRequest *zigbee_tx_request, AppXbeeZigbeeReceivePacket *zigbee_response, u16 timeout_ms);

s16 XbeeZigbeeExpectedResponse(AppXbeeZigbeeReceivePacket *zigbee_response, u16 timeout_ms);
void XbeeZigbeeReceiveEventCallBack(AppXbeeZigbeeReceivePacket *zigbee_response);     // should be defined in application layer

#endif
