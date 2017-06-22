#ifndef XBEE_ZIGBEE_OPERATIONS_H
#define XBEE_ZIGBEE_OPERATIONS_H

#include <comman.h>
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


void XbeeZigbeeOperationsInit(void);
void XbeeZigbeeHouseKeeping(void);
void XbeeZigbeePushReceivePacket(u8 *data);
void XbeeProcessZigbeeTransmitStatus(u8 *data);

s16 XbeeSendZigbeeTransmitRequest(u8 *data, u8 data_len);
void XbeeZigbeeReceiveEventHandler(AppXbeeZigbeeReceivePacket *zigbee_response);     // should be defined in application layer

#endif
