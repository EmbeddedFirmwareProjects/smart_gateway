
#include <xbee_comman.h>
#include <smart_power_stack.h>
#include <smart_power_app.h>

static void SmartPowerStackProcessSystemCommand(u8 *pdata);
static void SmartPowerStackProcessApplicationCommand(u8 *pdata);


static u8 SmartPowerStackCalculateCheckSum(u8 *apapi_frame, u16 len)
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

void SmartPowerStackProcessCommand(u8 *pdata, u8 data_len)
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
    cal_check_sum = SmartPowerStackCalculateCheckSum(&pdata[2], pdata[1]);
    if(cal_check_sum != pdata[data_len - 1])
    {
        LOG_ERR(("\nERR:: cal_check_sum:: %x, checkSum:: %x", cal_check_sum, pdata[data_len - 1]));
        return;
    }

    // process command
    if((pdata[2] > 0x00) && (pdata[2] <= 0x0F))     // check for system command
    {
        SmartPowerStackProcessSystemCommand(&pdata[2]);
    }
    else if((pdata[2] >= 0x10) && (pdata[2] <= 0x20))     // check for application command
    {
        SmartPowerStackProcessApplicationCommand(&pdata[2]);
    }
    else
    {
        LOG_ERR(("\nERR:: SmartPowerAppProcessCommand(): Invalid command"));
        return;
    }
}

void SmartPowerStackProcessSystemCommand(u8 *pdata)
{
    LOG_INFO(("\n<< %s >>", __func__));
}

void SmartPowerStackProcessApplicationCommand(u8 *pdata)
{
    SmartPowerStackAppCommand app_cmd = {0};

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

            SmartPowerAppSwitchControl(&app_cmd);
        }
        break;

        default:
        {
            LOG_ERR(("\nERR::SmartPowerStackProcessApplicationCommand() command %d not handled", app_cmd.cmd));
        }
        break;
    }
}

void SmartPowerStackSendApplicationCommand(SmartPowerStackAppCommand *papp_cmd)
{
    u8 send_response[SMART_POWER_APPLICATION_COMMAND_MAX_LEN] = {0};
    u8 byte_count = 0x00;
    s16 ret = 0x00;

    switch(papp_cmd->cmd)
    {
        case SMART_POWER_APPLICATION_COMMAND_SWITCH_CONTROL:
        {
            // send response frame to master
            send_response[byte_count++] = SMART_POWER_SYSTEM_COMMAND_START_DELIMITER;
            send_response[byte_count++] = 0x05;                  // Length
            send_response[byte_count++] = SMART_POWER_APPLICATION_COMMAND_SWITCH_CONTROL;
            send_response[byte_count++] = papp_cmd->commandType;
            send_response[byte_count++] = papp_cmd->data.SwitchControl.response.switchNumber;
            send_response[byte_count++] = papp_cmd->data.SwitchControl.response.responseStatus;
            send_response[byte_count] = papp_cmd->data.SwitchControl.response.SwitchStatus;
        }
        break;

        default:
        {
            LOG_ERR(("\nERR::SmartPowerStackSendApplicationCommand() command %d not handled", papp_cmd->cmd));
            return;
        }
    }

    ret = XbeeSendZigbeeTransmitRequest(send_response, byte_count);
    if(ret != 0x00)
    {
        LOG_ERR(("\nERR:: XbeeSendZigbeeTransmitRequest(): %d", ret));
    }
}
