#include <smart_power_app.h>

void SmartPowerAppSwitchControl(SmartPowerStackAppCommand *papp_cmd)
{
    if(papp_cmd->commandType == SMART_POWER_COMMAND_TYPE_REQUEST)
    {
        if(papp_cmd->data.SwitchControl.request.requestType == SMART_POWER_APPLICATION_REQUEST_TYPE_UPDATE)
        {
            SmartPowerStackAppCommand switch_cntl;

            // update switch status
            LOG_INFO(("\nRequested to update switch number %d with value %d", papp_cmd->data.SwitchControl.request.switchNumber,papp_cmd->data.SwitchControl.request.SwitchStatus));

            switch_cntl.cmd = SMART_POWER_APPLICATION_COMMAND_SWITCH_CONTROL;
            switch_cntl.commandType = SMART_POWER_COMMAND_TYPE_RESPONSE;
            switch_cntl.data.SwitchControl.response.switchNumber = papp_cmd->data.SwitchControl.response.switchNumber;
            switch_cntl.data.SwitchControl.response.responseStatus = 0x00;   // only testing purpose
            switch_cntl.data.SwitchControl.response.SwitchStatus = papp_cmd->data.SwitchControl.response.SwitchStatus;

            SmartPowerStackSendApplicationCommand(&switch_cntl);
        }
        else if(papp_cmd->data.SwitchControl.request.requestType == SMART_POWER_APPLICATION_REQUEST_TYPE_REQUEST_STATUS)
        {
            SmartPowerStackAppCommand switch_cntl;

            //Request for switch status

            switch_cntl.cmd = SMART_POWER_APPLICATION_COMMAND_SWITCH_CONTROL;
            switch_cntl.commandType = SMART_POWER_COMMAND_TYPE_RESPONSE;
            switch_cntl.data.SwitchControl.response.switchNumber = papp_cmd->data.SwitchControl.response.switchNumber;
            switch_cntl.data.SwitchControl.response.responseStatus = 0x00;   // only testing purpose
            switch_cntl.data.SwitchControl.response.SwitchStatus = papp_cmd->data.SwitchControl.response.SwitchStatus;

            SmartPowerStackSendApplicationCommand(&switch_cntl);
        }
    }
}

