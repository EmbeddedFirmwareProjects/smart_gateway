#ifndef XBEE_STACK_PVT_H
#define XBEE_STACK_PVT_H

#include <comman.h>
#include <xbee_stack.h>

#define XBEE_PACKET_STATUS_LIST_SIZE 3

typedef struct
{
    s8 isInUse;
    s8 isValid;
    s8 dataLen;
    u8 bufferPacket[API_FRAME_MAX_RECEIVE_PACKET_SIZE];
}XbeePacketStatus;

typedef struct
{
    u8 apiIdentifier;
    void(*pFunc)(void*);
}ApiIdentifierList;

void XbeeProcessAtCommandResponse(void *apdata);
void XbeeProcessZigbeeTransmitStatus(void *apdata);
void XbeeProcessZigbeeReceivePacket(void *apdata);

static ApiIdentifierList ProcessApiIdentifierList[] = {
    {AT_COMMAND_RESPONSE, XbeeProcessAtCommandResponse},
    {ZIGBEE_TRANSMIT_STATUS, XbeeProcessZigbeeTransmitStatus},
    {ZIGBEE_RECEIVE_PACKET, XbeeProcessZigbeeReceivePacket},
};

#endif 
