#ifndef __xbee_operations_pvt_h__
#define __xbee_operations_pvt_h__

#include <datatypes.h>

typedef struct __tag_XbeeOperations XbeeOperations;
typedef struct __tag_StateXbeeOperations StateXbeeOperations;

struct __tag_state_xbee_operations
{
    XbeeOperations *Xbee_Operations_state_machine;
    void (*house_keeping) (StateXbeeOperations *thiz);
    void (*handle_evnt_timeout) (StateXbeeOperations *thiz);
    void (*handle_evnt_cmd) (StateXbeeOperations *thiz);
};

struct __tag_XbeeOperations {

};

#endif
