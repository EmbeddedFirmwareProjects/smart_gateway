#ifndef XBEE_STACK_PVT_H
#define XBEE_STACK_PVT_H

#include <comman.h>
#include <xbee_at_command.h>

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

static ApiFramePacketBackup sApiPacketBackup[API_PACKET_BACKUP_SIZE];
static ApiIdentifierList ProcessApiIdentifierList[] = {
    {AT_COMMAND_RESPONSE, ProcessAtCommandResponse},
	//{ZIGBEE_TRANSMIT_STATUS, processZigbeeTransmitStatus},
	//{ZIGBEE_RECEIVE_PACKET, processZigbeeReceivePacket},
};

void processZigbeeTransmitStatus(void *apdata);
void processZigbeeReceivePacket(void *apdata);

#endif 
