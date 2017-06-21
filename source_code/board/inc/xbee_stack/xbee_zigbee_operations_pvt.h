#ifndef __xbee_zigbee_operations_pvt_h__
#define __xbee_zigbee_operations_pvt_h__

#include <comman.h>

#define XBEE_ZIGBEE_PACKET_STATUS_LIST_SIZE     4


typedef struct __tag_XbeeZigbeeOperations XbeeZigbeeOperations;
typedef struct __tag_StateXbeeZigbeeOperations StateXbeeZigbeeOperations;

typedef struct
{
    s8 isInUse;
    s8 isValid;
    s8 dataLen;
    u8 *bufferPacket;
}XbeeZigbeePacketStatus;

struct __tag_state_xbee_operations
{
    XbeeZigbeeOperations *Xbee_Operations_state_machine;
    void (*house_keeping) (StateXbeeZigbeeOperations *thiz);
    void (*handle_evnt_timeout) (StateXbeeZigbeeOperations *thiz);
    void (*handle_evnt_cmd) (StateXbeeZigbeeOperations *thiz);
};

struct __tag_XbeeZigbeeOperations {

};

#endif
