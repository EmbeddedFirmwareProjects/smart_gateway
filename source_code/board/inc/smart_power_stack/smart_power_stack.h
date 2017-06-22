#ifndef SMART_POWER_STACK_H
#define SMART_POWER_STACK_H

#include <comman.h>


#define SMART_POWER_SYSTEM_COMMAND_START_DELIMITER  0XAA

#define SMART_POWER_SYSTEM_COMMAND_LIST \
    ENUM_DETAILS(SMART_POWER_SYSTEM_COMMAND_HEART_BEAT = 0x01, "Heart beat message sent to master"),\
    ENUM_DETAILS(SMART_POWER_SYSTEM_COMMAND_HARDWARE_DIAGNOSTIC = 0x02, "Request sent from master"),\
    ENUM_DETAILS(SMART_POWER_SYSTEM_COMMAND_SYSTEM_CONTROL = 0x03, "Request sent from master"),

#define SMART_POWER_APPLICATION_COMMAND_LIST \
    ENUM_DETAILS(SMART_POWER_APPLICATION_COMMAND_SWITCH_CONTROL = 0x10, "Switch control command"),\
    ENUM_DETAILS(SMART_POWER_APPLICATION_COMMAND_REGULATOR_CONTROL = 0x11, "Regulator control"),\
    ENUM_DETAILS(SMART_POWER_APPLICATION_COMMAND_POWER_SOCKET_CONTROL = 0x03, "Power Socket Control"),\
    ENUM_DETAILS(SMART_POWER_APPLICATION_COMMAND_MOBILE_CHARGER = 0x03, "Mobile charger control"),\
    ENUM_DETAILS(SMART_POWER_APPLICATION_COMMAND_SENSOR = 0x03, "Sensor info request sent to slave"),

#define SMART_POWER_APPLICATION_COMMAND_MAX_LEN    10


typedef enum
{
    SMART_POWER_SYSTEM_COMMAND_LIST
    SMART_POWER_APPLICATION_COMMAND_LIST

    SmartPowerCommandList_MAX,
}SmartPowerCommandList;

typedef enum
{
    SMART_POWER_COMMAND_TYPE_RESPONSE = 0x00,
    SMART_POWER_COMMAND_TYPE_REQUEST = 0x01,

    SmartPowerCommandType_MAX
}SmartPowerCommandType;

typedef enum
{
    SMART_POWER_APPLICATION_REQUEST_TYPE_REQUEST_STATUS = 0x00,
    SMART_POWER_APPLICATION_REQUEST_TYPE_UPDATE = 0x01,

    SmartPowerApplicationRequestType_MAX
}SmartPowerApplicationRequestType;


typedef union
{
    union
    {
        struct
        {
            u8 capsenseCommunication;
        }response;
    }HardwareDiagnostic;

}SmartPowerSystemCommandData;

typedef struct
{
    SmartPowerCommandList cmd;
    SmartPowerCommandType commandType;
    SmartPowerSystemCommandData data;
}SmartPowerStackSystemCommand;


typedef union
{
    union
    {
        struct
        {
            u8 switchNumber;
            u8 requestType;
            u8 SwitchStatus;
        }request;
        struct
        {
            u8 switchNumber;
            u8 responseStatus;
            u8 SwitchStatus;
        }response;
    }SwitchControl;

    union
    {
        struct
        {
            u8 regulatorNumber;
            u8 requestType;
            u8 regulatorStatus;
        }request;
        struct
        {
            u8 regulatorNumber;
            u8 responseStatus;
            u8 regulatorStatus;
        }response;
    }RegulatorControl;

    union
    {
        struct
        {
            u8 powerSocketNumber;
            u8 requestType;
            u8 powerSocketStatus;
        }request;
        struct
        {
            u8 powerSocketNumber;
            u8 responseStatus;
            u8 powerSocketStatus;
        }response;
    }PowerSocketControl;

    union
    {
        struct
        {
            u8 mobileSocketNumber;
            u8 requestType;
            u8 mobileChargerStatus;
        }request;
        struct
        {
            u8 mobileChargerNumber;
            u8 responseStatus;
            u8 mobileChargerStatus;
        }response;
    }MobileChargerControl;

    union
    {
        struct
        {
            u8 sensorNumber;
            u8 requestType;
            u8 sensorStatus;
        }request;
        struct
        {
            u8 sensorNumber;
            u8 responseStatus;
            u8 sensorStatus;
        }response;
    }Sensor;

}SmartPowerAppCommandData;

typedef struct
{
    u8 cmd;
    SmartPowerCommandType commandType;
    SmartPowerAppCommandData data;
}SmartPowerStackAppCommand;

void SmartPowerStackProcessCommand(u8 *pdata, u8 len);
void SmartPowerStackSendApplicationCommand(SmartPowerStackAppCommand *papp_cmd);


#endif
