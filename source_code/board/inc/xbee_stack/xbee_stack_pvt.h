#ifndef XBEE_STACK_PVT_H
#define XBEE_STACK_PVT_H

#include <comman.h>

typedef struct
{
    u8 usageFlag;
    u8 buffer[API_PACKET_BACKUP_BUFFER_SIZE];
}ApiFramePacketBackup;

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
