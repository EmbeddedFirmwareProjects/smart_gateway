#include <smart_power_app.h>
#include <xbee_comman.h>


static void SmartPowerAppProcessSystemCommand(u8 *pdata);
static void SmartPowerAppProcessApplicationCommand(u8 *pdata);
void SmartPowerAppSendCommand(u8 *pdata, u8 len);

static u8 SmartPowerApplicationCalculateCheckSum(u8 *apapi_frame, u16 len)
{
    long check_sum = 0x00;
    u16 count = 0x00;

    LOG_INFO0(("\n<< %s >>", __func__));

    while(count < len)
    {
        check_sum = check_sum + apapi_frame[count];
        ++count;
    }
    check_sum = 0XFF - (check_sum & 0xFF);

    return (check_sum & 0xFF);
}

void SmartPowerAppProcessCommand(u8 *pdata, u8 data_len)
{
    u8 cal_check_sum = 0x00;

    LOG_INFO(("\n<< %s >>", __func__));

    {
        u16 count = 0x00;
        LOG_INFO(("\nSmartPowerCommand:: "));
        for(count = 0x00; count < data_len; count++)
        {
            LOG_INFO((" %x", pdata[count]));
        }
    }

    // validate start delimiter
    if(pdata[0] != SMART_POWER_SYSTEM_COMMAND_START_DELIMITER)
    {
        LOG_ERR(("\nERR:: Invalid start delimiter"));
        return ;
    }

    // validate checksum
    cal_check_sum = SmartPowerApplicationCalculateCheckSum(&pdata[2], pdata[1]);
    if(cal_check_sum != pdata[data_len - 1])
    {
        LOG_ERR(("\nERR:: cal_check_sum:: %x, checkSum:: %x", cal_check_sum, pdata[data_len - 1]));
        return;
    }

    // process command
    if((pdata[2] > 0x00) && (pdata[2] <= 0x0F))     // check for system command
    {
        SmartPowerAppProcessSystemCommand(&pdata[2]);
    }
    else if((pdata[2] >= 0x10) && (pdata[2] <= 0x20))     // check for application command
    {
        SmartPowerAppProcessApplicationCommand(&pdata[2]);
    }
    else
    {
        LOG_ERR(("\nERR:: SmartPowerAppProcessCommand(): Invalid command"));
        return;
    }
}

void SmartPowerAppProcessSystemCommand(u8 *pdata)
{
    LOG_INFO(("\n<< %s >>", __func__));
}

void SmartPowerAppProcessApplicationCommand(u8 *pdata)
{
    SmartPowerAppCommand app_cmd = {0};

    LOG_INFO(("\n<< %s >>", __func__));

    app_cmd.cmd = pdata[0];

    switch(app_cmd.cmd)
    {
        case SMART_POWER_APPLICATION_COMMAND_SWITCH_CONTROL:
        {
            app_cmd.commandType = pdata[1];
            app_cmd.data.SwitchControl.request.switchNumber = pdata[2];
            app_cmd.data.SwitchControl.request.requestType = pdata[3];
            app_cmd.data.SwitchControl.request.SwitchStatus = pdata[4];

            if(app_cmd.commandType == SMART_POWER_COMMAND_TYPE_REQUEST)
            {
                if(app_cmd.data.SwitchControl.request.requestType == SMART_POWER_APPLICATION_REQUEST_TYPE_UPDATE)
                {
                    u8 send_response[SMART_POWER_APPLICATION_COMMAND_MAX_LEN] = {0};
                    u8 count = 0x00;

                    // update switch status
                    LOG_INFO(("\nRequested to update switch number %d with value %d", app_cmd.data.SwitchControl.request.switchNumber,app_cmd.data.SwitchControl.request.SwitchStatus));

                    // send response frame to master
                    send_response[count++] = SMART_POWER_SYSTEM_COMMAND_START_DELIMITER;
                    send_response[count++] = 0x05;
                    send_response[count++] = SMART_POWER_APPLICATION_COMMAND_SWITCH_CONTROL;
                    send_response[count++] = SMART_POWER_COMMAND_TYPE_RESPONSE;
                    send_response[count++] = app_cmd.data.SwitchControl.response.switchNumber;
                    send_response[count++] = 0x00;  // only testing purpose
                    send_response[count] = app_cmd.data.SwitchControl.response.SwitchStatus;

                    SmartPowerAppSendCommand(send_response, count);
                }
                else if(app_cmd.data.SwitchControl.request.requestType == SMART_POWER_APPLICATION_REQUEST_TYPE_REQUEST_STATUS)
                {

                }
            }
        }
        break;

        default:
        {
            LOG_ERR(("\nERR::SmartPowerAppProcessApplicationCommand() command %d not handled", app_cmd.cmd));
        }
        break;
    }
}

void SmartPowerAppSendCommand(u8 *pdata, u8 len)
{
    s16 ret = 0x00;
    AppXbeeZigbeeTransmitRequest zigbee_request;

    LOG_INFO(("\n<< %s >>", __func__));

    zigbee_request.rfTransmitRequest.frameId = 0x01;
    zigbee_request.rfTransmitRequest.destinationAdress[0] = 0xFF;
    zigbee_request.rfTransmitRequest.destinationAdress[1] = 0xFF;
    zigbee_request.rfTransmitRequest.destinationAdress[2] = 0xFF;
    zigbee_request.rfTransmitRequest.destinationAdress[3] = 0xFF;
    zigbee_request.rfTransmitRequest.destinationAdress[4] = 0xFF;
    zigbee_request.rfTransmitRequest.destinationAdress[5] = 0xFF;
    zigbee_request.rfTransmitRequest.destinationAdress[6] = 0xFF;
    zigbee_request.rfTransmitRequest.destinationAdress[7] = 0xFF;
    zigbee_request.rfTransmitRequest.destinationNetworkAddress[0] = 0x01;
    zigbee_request.rfTransmitRequest.destinationNetworkAddress[1] = 0x11;
    zigbee_request.rfTransmitRequest.broadcastRadius = 0x00;
    zigbee_request.rfTransmitRequest.options = ZIGBEE_OPTIONS_DISABLE_RETRIES | ZIGBEE_OPTIONS_ENABLE_APS_ENCRYPTION;
    zigbee_request.rfDataLen = len;
    zigbee_request.rfTransmitRequest.rfData = pdata;

    ret = XbeeSendZigbeeTransmitRequest(&zigbee_request, 200);
    if(ret != 0x00)
    {
        LOG_ERR(("\nERR:: XbeeSendZigbeeTransmitRequest(): %d", ret));
    }


}
