
#include <xbee_zigbee_operations.h>

static ResisgerExpectedZigbeeRfCommandResponse sRegisteredExpectedCmd = {true, false};
static ResisgerZigbeeTransmitRequestStatus sResisgerZigbeeTransmitRequestStatus = {true, false};
static u8 sZigbeeRfCommandRequestApiPacketBuffer[API_PACKET_REQUEST_BUFFER_SIZE] = {0};

extern u16 XbeeProcessApiFrameRequest(u8* pdata, u16 len);

void XbeeProcessZigbeeReceivePacket(void *apdata)
{
    u16 len = 0x00;
    s16 rcv_data_len = 0x00;
    u8 *data = (u8 *)apdata;
    AppXbeeZigbeeReceivePacket app_zigbee_response = {{{0}}};

    LOG_INFO0(("\n<< %s >>", __func__));

    len = (data[1] << 8) | data[2];
    app_zigbee_response.rfPacketResponse.sourceAdress[0] = data[4];
    app_zigbee_response.rfPacketResponse.sourceAdress[1] = data[5];
    app_zigbee_response.rfPacketResponse.sourceAdress[2] = data[6];
    app_zigbee_response.rfPacketResponse.sourceAdress[3] = data[7];
    app_zigbee_response.rfPacketResponse.sourceAdress[4] = data[8];
    app_zigbee_response.rfPacketResponse.sourceAdress[5] = data[9];
    app_zigbee_response.rfPacketResponse.sourceAdress[6] = data[10];
    app_zigbee_response.rfPacketResponse.sourceAdress[7] = data[11];
    app_zigbee_response.rfPacketResponse.sourceNetworkAddress = (data[12] << 8) | data[13];
    app_zigbee_response.rfPacketResponse.receiveOption = data[14];

    // cmd + sourceAdress(8) + sourceNetworkAddress[2] + receiveOption
    rcv_data_len = (len - 12);

    if(rcv_data_len > 0)
    {
        app_zigbee_response.rfPacketResponse.receiveData = &data[15];
    }

    if(sRegisteredExpectedCmd.availableFlag == false)
    {
        sRegisteredExpectedCmd.appXbeeZigbeeReceivePacket.rfPacketResponse.sourceAdress[0] = app_zigbee_response.rfPacketResponse.sourceAdress[0];
        sRegisteredExpectedCmd.appXbeeZigbeeReceivePacket.rfPacketResponse.sourceAdress[1] = app_zigbee_response.rfPacketResponse.sourceAdress[1];
        sRegisteredExpectedCmd.appXbeeZigbeeReceivePacket.rfPacketResponse.sourceAdress[2] = app_zigbee_response.rfPacketResponse.sourceAdress[2];
        sRegisteredExpectedCmd.appXbeeZigbeeReceivePacket.rfPacketResponse.sourceAdress[3] = app_zigbee_response.rfPacketResponse.sourceAdress[3];
        sRegisteredExpectedCmd.appXbeeZigbeeReceivePacket.rfPacketResponse.sourceAdress[4] = app_zigbee_response.rfPacketResponse.sourceAdress[4];
        sRegisteredExpectedCmd.appXbeeZigbeeReceivePacket.rfPacketResponse.sourceAdress[5] = app_zigbee_response.rfPacketResponse.sourceAdress[5];
        sRegisteredExpectedCmd.appXbeeZigbeeReceivePacket.rfPacketResponse.sourceAdress[6] = app_zigbee_response.rfPacketResponse.sourceAdress[6];
        sRegisteredExpectedCmd.appXbeeZigbeeReceivePacket.rfPacketResponse.sourceAdress[7] = app_zigbee_response.rfPacketResponse.sourceAdress[7];
        sRegisteredExpectedCmd.appXbeeZigbeeReceivePacket.rfPacketResponse.sourceNetworkAddress = app_zigbee_response.rfPacketResponse.sourceNetworkAddress;
        sRegisteredExpectedCmd.appXbeeZigbeeReceivePacket.rfPacketResponse.receiveOption = app_zigbee_response.rfPacketResponse.receiveOption;

        if(rcv_data_len > 0)
        {
            sRegisteredExpectedCmd.appXbeeZigbeeReceivePacket.rfPacketResponse.receiveData = app_zigbee_response.rfPacketResponse.receiveData;
            sRegisteredExpectedCmd.appXbeeZigbeeReceivePacket.receiveDataLen = rcv_data_len;
        }
        sRegisteredExpectedCmd.validFlag = true;
    }
    else
    {
        if(rcv_data_len > 0)
        {
            app_zigbee_response.receiveDataLen = rcv_data_len;
        }
        XbeeZigbeeReceiveEventCallBack(&app_zigbee_response);
    }
}

void XbeeProcessZigbeeTransmitStatus(void *apdata)
{
    u8 *data = (u8 *)apdata;
    ZigbeeTransmitStatus transmit_status;

    LOG_INFO0(("\n<< %s >>", __func__));

    transmit_status.frameId = data[4];
    transmit_status.destinationAddress = (data[5] << 8) | data[6];
    transmit_status.transmitRetryCount = data[7];
    transmit_status.deliveryStatus = data[8];
    transmit_status.discoveryStatus = data[9];

    if(sResisgerZigbeeTransmitRequestStatus.availableFlag == false)
    {
        if(sResisgerZigbeeTransmitRequestStatus.zigbeeTransmitStatus.frameId == transmit_status.frameId)
        {
            sResisgerZigbeeTransmitRequestStatus.zigbeeTransmitStatus.frameId = transmit_status.frameId;
            sResisgerZigbeeTransmitRequestStatus.zigbeeTransmitStatus.destinationAddress = transmit_status.destinationAddress;
            sResisgerZigbeeTransmitRequestStatus.zigbeeTransmitStatus.deliveryStatus = transmit_status.deliveryStatus;
            sResisgerZigbeeTransmitRequestStatus.zigbeeTransmitStatus.discoveryStatus = transmit_status.discoveryStatus;
            sResisgerZigbeeTransmitRequestStatus.validFlag = true;
        }
    }
}

s16 XbeeSendZigbeeTransmitRequest(AppXbeeZigbeeTransmitRequest *zigbee_tx_request, u16 timeout_ms)
{
    u16 len = 0x00;
    s16 ret = 0x00;
    s16 count = 0x00;

    LOG_INFO0(("\n<< %s >>", __func__));

    // check Zigbee Transmit Request Status
    if(sResisgerZigbeeTransmitRequestStatus.availableFlag == false)
    {
        LOG_ERR(("\nERR:: sResisgerZigbeeTransmitRequestStatus already in use"));
        return -EXBEE_ZIGBEE_TX_REQUEST_IN_USE;
    }

    // register Zigbee Transmit Request Status
    sResisgerZigbeeTransmitRequestStatus.availableFlag = false;
    sResisgerZigbeeTransmitRequestStatus.validFlag = false;

    sResisgerZigbeeTransmitRequestStatus.zigbeeTransmitStatus.frameId = zigbee_tx_request->rfTransmitRequest.frameId;

    // cmd + frameId + destinationAddr(8) + destinationNetworkAddress[2] + broadcastRadius + options
    len = (zigbee_tx_request->rfDataLen + 14);

    sZigbeeRfCommandRequestApiPacketBuffer[3]   = ZIGBEE_TRANSMIT_REQUEST;
    sZigbeeRfCommandRequestApiPacketBuffer[4]   = zigbee_tx_request->rfTransmitRequest.frameId;
    sZigbeeRfCommandRequestApiPacketBuffer[5]   = zigbee_tx_request->rfTransmitRequest.destinationAdress[0];
    sZigbeeRfCommandRequestApiPacketBuffer[6]   = zigbee_tx_request->rfTransmitRequest.destinationAdress[1];
    sZigbeeRfCommandRequestApiPacketBuffer[7]   = zigbee_tx_request->rfTransmitRequest.destinationAdress[2];
    sZigbeeRfCommandRequestApiPacketBuffer[8]   = zigbee_tx_request->rfTransmitRequest.destinationAdress[3];
    sZigbeeRfCommandRequestApiPacketBuffer[9]   = zigbee_tx_request->rfTransmitRequest.destinationAdress[4];
    sZigbeeRfCommandRequestApiPacketBuffer[10]  = zigbee_tx_request->rfTransmitRequest.destinationAdress[5];
    sZigbeeRfCommandRequestApiPacketBuffer[11]  = zigbee_tx_request->rfTransmitRequest.destinationAdress[6];
    sZigbeeRfCommandRequestApiPacketBuffer[12]  = zigbee_tx_request->rfTransmitRequest.destinationAdress[7];
    sZigbeeRfCommandRequestApiPacketBuffer[13]  = zigbee_tx_request->rfTransmitRequest.destinationNetworkAddress[0];
    sZigbeeRfCommandRequestApiPacketBuffer[14]  = zigbee_tx_request->rfTransmitRequest.destinationNetworkAddress[1];
    sZigbeeRfCommandRequestApiPacketBuffer[15]  = zigbee_tx_request->rfTransmitRequest.broadcastRadius;
    sZigbeeRfCommandRequestApiPacketBuffer[16]  = zigbee_tx_request->rfTransmitRequest.options;

    if(zigbee_tx_request->rfDataLen > 0)
    {
        memcpy(&sZigbeeRfCommandRequestApiPacketBuffer[17], zigbee_tx_request->rfTransmitRequest.rfData, zigbee_tx_request->rfDataLen);
    }

    ret = XbeeProcessApiFrameRequest(sZigbeeRfCommandRequestApiPacketBuffer, len);
    if(ret != 0x00)
    {
        sResisgerZigbeeTransmitRequestStatus.validFlag = false;
        sResisgerZigbeeTransmitRequestStatus.availableFlag = true;

        LOG_ERR(("\nERR:: ProcessApiFrameRequest(): %d", ret));
        return -EXBEE_ZIGBEE_TX_REQUEST;
    }

    while((count < timeout_ms) && (sResisgerZigbeeTransmitRequestStatus.validFlag != true))
    {
        DelayMs(1);
        count++;
    }

    if(sResisgerZigbeeTransmitRequestStatus.validFlag == true)
    {
        if(sResisgerZigbeeTransmitRequestStatus.zigbeeTransmitStatus.deliveryStatus != TX_DELIVERY_STATUS_SUCCESS)
        {
            LOG_ERR(("\nERR:: zigbeeTransmitStatus.deliveryStatus: %d", sResisgerZigbeeTransmitRequestStatus.zigbeeTransmitStatus.deliveryStatus));
            sResisgerZigbeeTransmitRequestStatus.validFlag = false;
            sResisgerZigbeeTransmitRequestStatus.availableFlag = true;
            return -EXBEE_ZIGBEE_TX_REQUEST_STATUS;
        }
        sResisgerZigbeeTransmitRequestStatus.validFlag = false;
        sResisgerZigbeeTransmitRequestStatus.availableFlag = true;
        return EXBEE_OK;
    }
    else
    {
        sResisgerZigbeeTransmitRequestStatus.validFlag = false;
        sResisgerZigbeeTransmitRequestStatus.availableFlag = true;

        LOG_ERR(("\nERR:: ZigbeeTransmitRequestStatus timeout()"));
        return -EXBEE_ZIGBEE_TX_REQUEST_STATUS_TIMEOUT;
    }
}

s16 XbeeSendZigbeeTransmitRequestExpectedResponse(AppXbeeZigbeeTransmitRequest *zigbee_tx_request, AppXbeeZigbeeReceivePacket *zigbee_response, u16 timeout_ms)
{
    s16 ret = 0x00;

    LOG_INFO0(("\n<< %s >>", __func__));

    ret = XbeeSendZigbeeTransmitRequest(zigbee_tx_request, timeout_ms);
    if(ret != EXBEE_OK)
    {
        LOG_ERR(("\nERR:: XbeeSendZigbeeTransmitRequest():: %d", ret));
        return ret;
    }

    ret = XbeeZigbeeExpectedResponse(zigbee_response, timeout_ms);

    return ret;
}

s16 XbeeZigbeeExpectedResponse(AppXbeeZigbeeReceivePacket *zigbee_response, u16 timeout_ms)
{
    s16 count = 0x00;

    LOG_INFO0(("\n<< %s >>", __func__));

    // register expected response
    if(sRegisteredExpectedCmd.availableFlag == false)
    {
        LOG_ERR(("\nERR:: sRegisteredExpectedCmd already in use"));
        return -EXBEE_AT_CMD_EXPECTED_IN_USE;
    }

    sRegisteredExpectedCmd.availableFlag = false;
    sRegisteredExpectedCmd.validFlag = false;

    while((count < timeout_ms) && (sRegisteredExpectedCmd.validFlag != true))
    {
        DelayMs(1);
        count++;
    }

    if(sRegisteredExpectedCmd.validFlag == true)
    {
        zigbee_response->rfPacketResponse.sourceAdress[0] = sRegisteredExpectedCmd.appXbeeZigbeeReceivePacket.rfPacketResponse.sourceAdress[0];
        zigbee_response->rfPacketResponse.sourceAdress[1] = sRegisteredExpectedCmd.appXbeeZigbeeReceivePacket.rfPacketResponse.sourceAdress[1];
        zigbee_response->rfPacketResponse.sourceAdress[2] = sRegisteredExpectedCmd.appXbeeZigbeeReceivePacket.rfPacketResponse.sourceAdress[2];
        zigbee_response->rfPacketResponse.sourceAdress[3] = sRegisteredExpectedCmd.appXbeeZigbeeReceivePacket.rfPacketResponse.sourceAdress[3];
        zigbee_response->rfPacketResponse.sourceAdress[4] = sRegisteredExpectedCmd.appXbeeZigbeeReceivePacket.rfPacketResponse.sourceAdress[4];
        zigbee_response->rfPacketResponse.sourceAdress[5] = sRegisteredExpectedCmd.appXbeeZigbeeReceivePacket.rfPacketResponse.sourceAdress[5];
        zigbee_response->rfPacketResponse.sourceAdress[6] = sRegisteredExpectedCmd.appXbeeZigbeeReceivePacket.rfPacketResponse.sourceAdress[6];
        zigbee_response->rfPacketResponse.sourceAdress[7] = sRegisteredExpectedCmd.appXbeeZigbeeReceivePacket.rfPacketResponse.sourceAdress[7];
        zigbee_response->rfPacketResponse.sourceNetworkAddress = sRegisteredExpectedCmd.appXbeeZigbeeReceivePacket.rfPacketResponse.sourceNetworkAddress;
        zigbee_response->rfPacketResponse.receiveOption = sRegisteredExpectedCmd.appXbeeZigbeeReceivePacket.rfPacketResponse.receiveOption;

        if(sRegisteredExpectedCmd.appXbeeZigbeeReceivePacket.receiveDataLen > 0)
        {
            zigbee_response->rfPacketResponse.receiveData = sRegisteredExpectedCmd.appXbeeZigbeeReceivePacket.rfPacketResponse.receiveData;
            zigbee_response->receiveDataLen = sRegisteredExpectedCmd.appXbeeZigbeeReceivePacket.receiveDataLen;
        }
        sRegisteredExpectedCmd.validFlag = false;
        sRegisteredExpectedCmd.availableFlag = true;
        return EXBEE_OK;
    }
    else
    {
        sRegisteredExpectedCmd.validFlag = false;
        sRegisteredExpectedCmd.availableFlag = true;

        LOG_ERR(("\nERR:: Zigbee Expected Response timeout()"));
        return -EXBEE_ZIGBEE_EXPECTED_RESPONSE_TIMEOUT;
    }
}
