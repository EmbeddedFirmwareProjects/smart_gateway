
#ifndef XBEE_STACK_H
#define XBEE_STACK_H

#include <comman.h>

#define API_FRAME_START_DELIMITER 0x7E

// ZIGBEE_TRANSMIT_REQUEST_OPTIONS
#define ZIGBEE_OPTIONS_DISABLE_RETRIES 0x01
#define ZIGBEE_OPTIONS_ENABLE_APS_ENCRYPTION 0x20
#define ZIGBEE_OPTIONS_USE_EXTENDED_TRANSMISSION_TIMEOUT 0x40
#define ZIGBEE_OPTIONS_ALL_UNUSED_AND_UNSUPPORTED 0x00

// ZIGBEE_RECEIVE_PACKET_RECEIVE_OPTION
#define ZIGBEE_OPTION_PACKET_ACKNOWLEDGED 0x01
#define ZIGBEE_OPTION_PACKET_BROADCAST_PACKET 0x02
#define ZIGBEE_OPTION_PACKET_ENCRYPTED_APS_ENCRYPTION 0x20

// Api frame packet process
#define API_PACKET_BACKUP_BUFFER_SIZE 200
#define API_PACKET_REQUEST_BUFFER_SIZE 200
#define API_PACKET_BACKUP_SIZE 3
#define ZIGBEE_RECEIVE_PACKET_SOURCE_ADDRESS_LEN 8

// AT COMMAND SETTINGS

typedef enum 
{
    AT_COMMAND                                  = 0X08,
    AT_COMMAND_QUEUE_PARAMETER_VALUE            = 0X09,
    ZIGBEE_TRANSMIT_REQUEST                     = 0X10,
    EXPLICIT_ADDRESSING_ZIGBEE_COMMAND_FRAME    = 0X11,
    REMOTE_COMMAND_REQUEST                      = 0X17,
    CREATE_SOURCE_ROUTE                         = 0X21,
    AT_COMMAND_RESPONSE                         = 0X88,
    MODEM_STATUS                                = 0X8A,
    ZIGBEE_TRANSMIT_STATUS                      = 0X8B,
    ZIGBEE_RECEIVE_PACKET                       = 0X90,
    ZIGBEE_EXPLICIT_RX_INDICATOR                = 0X91,
    ZIGBEE_IO_DATA_SAMPLE_RX_INDICATOR          = 0X92,
    XBEE_SENSOR_READ_INDICATOR                  = 0X94,
    NODE_IDENTIFICATION_INDICATOR               = 0X95,
    REMOTE_COMMAND_RESPONSE                     = 0X97,
    EXTENDED_MODEM_STATUS                       = 0X98,
    OVER_THE_AIR_FIRMWARE_UPDATE_STATUS         = 0XA0,
    ROUTE_RECORD_INDICATOR                      = 0XA1,
    MANY_TO_ONE_ROUTE_REQUEST_INDICATOR         = 0XA3,
    
    ApiIdentifier_MAX,
}ApiIdentifier;

typedef enum 
{
    DH  = ('D' << 8 | 'H'),

    AtCommandList_MAX,
}AtCommandList;

typedef enum
{
    AtCommandStatus_MAX,
}AtCommandStatus;

typedef enum 
{
    TX_DELIVERY_STATUS_SUCCESS                      = 0x00,
    TX_DELIVERY_STATUS_MAC_ACK_FAILURE              = 0x01,
    TX_DELIVERY_STATUS_CCA_FAILURE                  = 0x02,
    TX_DELIVERY_STATUS_INVALID_DESTINATION_ENDPOINT = 0x15,
    TX_DELIVERY_STATUS_NETWORK_ACK_FAILURE          = 0x21,
    TX_DELIVERY_STATUS_NOT_JOINED_TO_NETWORK        = 0x22,
    TX_DELIVERY_STATUS_SELF_ADDRESSED               = 0x23,
    TX_DELIVERY_STATUS_ADDRESS_NOT_FOUND            = 0x24,
    TX_DELIVERY_STATUS_ROUTE_NOT_FOUND              = 0x25,
    TX_DELIVERY_STATUS_BROADCAST_SOURCE_FAILED      = 0x26,
    TX_DELIVERY_STATUS_INVALID_BINDING_TABLE_INDEX  = 0x2B,
    TX_DELIVERY_STATUS_RESOURCE_ERROR               = 0x2C,
    TX_DELIVERY_STATUS_ATTEMPTED_BROADCAST          = 0x2D,
    TX_DELIVERY_STATUS_ATTEMPTED_UNICAST            = 0x2E,
    TX_DELIVERY_STATUS_RESOURCE_ERROR_LACK          = 0x32,
    TX_DELIVERY_STATUS_DATA_PAYLOAD_TOO_LARGE       = 0x74,
    TX_DELIVERY_STATUS_INDIRECT_MESSAGE_UNREQUESTED = 0x75,

    ZigbeeTransmitDeliveryStatus_MAX,
}ZigbeeTransmitDeliveryStatus;

typedef enum
{
    TX_DISCOVERY_STATUS_NO_DISCOVERY_OVERHEAD       = 0x00,
    TX_DISCOVERY_STATUS_ADDRESS_DISCOVERY           = 0x01,
    TX_DISCOVERY_STATUS_ROUTE_DISCOVERY             = 0x02,
    TX_DISCOVERY_STATUS_ADDRESS_AND_ROUTE           = 0x03,
    TX_DISCOVERY_STATUS_EXTENDED_TIMEOUT_DISCOVERY  = 0x40,

    ZigbeeTransmitDiscoveryStatus_MAX,
}ZigbeeTransmitDiscoveryStatus;

typedef enum 
{
    EXBEE_OK            = 0x00,
    EXBEE_API_BACKUP,
    EXBEE_START_DELIMITER,
    EXBEE_CHECKSUM,
    EXBEE_CMDID,
    EXBEE_AT_CMD_INVALID,
    EXBEE_AT_CMD_EXPECTED_RESPONSE_TIMEOUT,
    EXBEE_AT_CMD_EXPECTED_IN_USE,
    EXBEE_AT_CMD_SEND,
    EXBEE_ZIGBEE_TX_REQUEST_IN_USE,
    EXBEE_ZIGBEE_TX_REQUEST_STATUS,
    EXBEE_ZIGBEE_TX_REQUEST_STATUS_TIMEOUT,
    EXBEE_ZIGBEE_TX_REQUEST,
    EXBEE_ZIGBEE_EXPECTED_RESPONSE_TIMEOUT,

    XbeeStackError_MAX,
}XbeeStackError;


typedef struct
{
    u8 frameId;
    u16 atCommand;
    u8 *parameterValue;
}AtCommandFrame;
        
typedef struct
{
    u8 frameId;
    AtCommandList atCommand;
    u8 *parameterValue;
}AtCommandQueueParameterValue;
        
typedef struct
{
    u8 frameId;
    u8 destinationAdress[8];            // plz note msb address should be 1st
    u8 destinationNetworkAddress[2];    // plz note msb address should be 1st
    u8 broadcastRadius;
    u8 options;
    u8 *rfData;
}ZigbeeTransmitRequest;
        
typedef struct 
{
    u8 frameId;
    AtCommandList atCommand;
    AtCommandStatus commandStatus;
    u8 *commandData;
}AtCommandResponse;
        
typedef struct 
{
    u8 frameId;
    u16 destinationAddress;             // plz note msb address should be 1st
    u8 transmitRetryCount;
    u8 deliveryStatus;
    u8 discoveryStatus;
}ZigbeeTransmitStatus;
        
typedef struct 
{
    u8 sourceAdress[ZIGBEE_RECEIVE_PACKET_SOURCE_ADDRESS_LEN];      // note:: msb address should be 1st
    u16 sourceNetworkAddress;                                   // note:: msb address should be 1st
    u8 receiveOption;
    u8 *receiveData;
}ZigbeeReceivePacket;


void ProcessApiFrameResponse(u8* pdata, u16 len);
s16 SendApiFrameRequest(u8* pdata, u16 len);   // should be defined in application layer
void XbeeStackInit();

#endif
