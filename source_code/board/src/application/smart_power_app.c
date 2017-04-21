#include <smart_power_app.h>
#include <xbee_comman.h>


static void SmartPowerAppProcessSystemCommand(u8 *pdata);
static void SmartPowerAppProcessApplicationCommand(u8 *pdata);


static u8 SmartPowerApplicationCalculateCheckSum(u8 *apapi_frame, u16 len)
{
    long check_sum = 0x00;
    u16 count = 0x00;

    LOG_INFO0(("<< %s >>\n", __func__));

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

    LOG_INFO(("<< %s >>\n", __func__));

    {
        u16 count = 0x00;
        LOG_INFO(("SmartPowerCommand:: "));
        for(count = 0x00; count < data_len; count++)
        {
            LOG_INFO((" %x", pdata[count]));
        }
        LOG_INFO(("\n"));
    }

    // validate start delimiter
    if(pdata[0] != SMART_POWER_SYSTEM_COMMAND_START_DELIMITER)
    {
        LOG_ERR(("ERR:: Invalid start delimiter\n"));
        return ;
    }

    // validate checksum
    cal_check_sum = SmartPowerApplicationCalculateCheckSum(&pdata[2], pdata[1]);
    if(cal_check_sum != pdata[data_len - 1])
    {
        LOG_ERR(("ERR:: cal_check_sum:: %x, checkSum:: %x\n", cal_check_sum, pdata[data_len - 1]));
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
        LOG_ERR(("ERR:: SmartPowerAppProcessCommand(): Invalid command\n"));
        return;
    }
}

void SmartPowerAppProcessSystemCommand(u8 *pdata)
{
    LOG_INFO(("<< %s >>\n", __func__));
}

void SmartPowerAppProcessApplicationCommand(u8 *pdata)
{
    SmartPowerAppCommand app_cmd = {0};
    AppXbeeZigbeeTransmitRequest zigbee_request;

    LOG_INFO(("<< %s >>\n", __func__));

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
                    // update switch status
                    LOG_INFO(("Requested to update switch number %d with value %d\n", app_cmd.data.SwitchControl.request.switchNumber,app_cmd.data.SwitchControl.request.SwitchStatus));

                    // send response frame to master
                }
                else if(app_cmd.data.SwitchControl.request.requestType == SMART_POWER_APPLICATION_REQUEST_TYPE_REQUEST_STATUS)
                {

                }
            }
        }
        break;

        default:
        {
            LOG_ERR(("ERR::SmartPowerAppProcessApplicationCommand() command %d not handled\n", app_cmd.cmd));
        }
        break;
    }
}

s16 SmartPowerAppSendCommand()
{

}
