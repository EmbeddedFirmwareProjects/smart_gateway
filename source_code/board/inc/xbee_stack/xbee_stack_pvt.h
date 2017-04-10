#ifndef XBEE_STACK_PVT_H
#define XBEE_STACK_PVT_H

#include <xbee_stack.h>
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


static void processZigbeeReceivePacket(void *received_packet);

static ApiFramePacketBackup sApiPacketBackup[API_PACKET_BACKUP_SIZE];
static ApiIdentifierList ProcessApiIdentifierList[] = {
    {AT_COMMAND_RESPONSE, processZigbeeReceivePacket},
};

#endif 
