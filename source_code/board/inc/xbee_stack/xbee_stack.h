
#ifndef XBEE_STACK_H
#define XBEE_STACK_H

#include <comman.h>

#define START_DELIMITER 0x7E

// ZIGBEE_TRANSMIT_REQUEST_OPTIONS
#define OPTIONS_DISABLE_RETRIES 0x01
#define OPTIONS_ENABLE_APS_ENCRYPTION 0x20
#define OPTIONS_USE_EXTENDED_TRANSMISSION_TIMEOUT 0x40
#define OPTIONS_ALL_UNUSED_AND_UNSUPPORTED 0x00

// ZIGBEE_RECEIVE_PACKET_RECEIVE_OPTION
#define OPTION_PACKET_ACKNOWLEDGED 0x01
#define OPTION_PACKET_BROADCAST_PACKET 0x02
#define OPTION_PACKET_ENCRYPTED_APS_ENCRYPTION 0x20

typedef enum 
{
    AT_COMMAND = 0X08,
    AT_COMMAND_QUEUE_PARAMETER_VALUE = 0X09,
    ZIGBEE_TRANSMIT_REQUEST = 0X10,
    EXPLICIT_ADDRESSING_ZIGBEE_COMMAND_FRAME = 0X11,
    REMOTE_COMMAND_REQUEST = 0X17,
    CREATE_SOURCE_ROUTE = 0X21,
    AT_COMMAND_RESPONSE = 0X88,
    MODEM_STATUS = 0X8A,
    ZIGBEE_TRANSMIT_STATUS = 0X8B,
    ZIGBEE_RECEIVE_PACKET = 0X90,
    ZIGBEE_EXPLICIT_RX_INDICATOR = 0X91,
    ZIGBEE_IO_DATA_SAMPLE_RX_INDICATOR = 0X92,
    XBEE_SENSOR_READ_INDICATOR = 0X94,
    NODE_IDENTIFICATION_INDICATOR = 0X95,
    REMOTE_COMMAND_RESPONSE = 0X97,
    EXTENDED_MODEM_STATUS = 0X98,
    OVER_THE_AIR_FIRMWARE_UPDATE_STATUS = 0XA0,
    ROUTE_RECORD_INDICATOR = 0XA1,
    MANY_TO_ONE_ROUTE_REQUEST_INDICATOR = 0XA3,
    
    ApiIdentifier_MAX,
}ApiIdentifier;


typedef enum 
{
    DH,
    AtCommandList_MAX,
}AtCommandList;

typedef enum
{
    AtCommandStatus_MAX,
}AtCommandStatus;

typedef enum 
{
    ZigbeeTransmitDeliveryStatus_MAX,
}ZigbeeTransmitDeliveryStatus;

typedef enum 
{
    ZigbeeTransmitDiscoveryStatus_MAX,
}ZigbeeTransmitDiscoveryStatus;

typedef union  
{

    struct 
    {
        u8 frameId;
        AtCommandList atCommand;
        u8 *parameterValue;
    }AtCommandFrame;

    struct 
    {
        u8 frameId;
        AtCommandList atCommand;
        u8 *parameterValue;
    }AtCommandQueueParameterValue;

    struct 
    {
        u8 frameId;
        u8 destinationAdress[8];            // plz note msb address should be 1st
        u8 destinationNetworkAddress[2];    // plz note msb address should be 1st
        u8 broadcastRadius;
        u8 options;
        u8 *rfData;
    }ZigbeeTransmitRequest;

    struct 
    {
        u8 frameId;
        AtCommandList atCommand;
        AtCommandStatus commandStatus;
    }AtCommandResponse;

    struct 
    {
        u8 frameId;
        u8 destinationAddress[2];    // plz note msb address should be 1st
        u8 transmitRetryCount;
        ZigbeeTransmitDeliveryStatus deliveryStatus;
        ZigbeeTransmitDiscoveryStatus discoveryStatus;
    }ZigbeeTransmitStatus;

    struct 
    {
        u8 frameId;
        u8 sourceAdress[8];            // plz note msb address should be 1st
        u8 sourceNetworkAddress[2];    // plz note msb address should be 1st
        u8 receiveOption;
        u8 *receiveData;
    }ZigbeeReceivePacket;
}IdentifierSpecificData;

typedef struct 
{
    ApiIdentifier cmdId;
    IdentifierSpecificData cmdData;
}FrameData;

typedef struct 
{
    u8 startDelimiter;
    u16 len;
    FrameData farmeData;
    u8 checkSum;
}ApiFrame;

void GenerateApiFrame(u8 *apdata);

#endif