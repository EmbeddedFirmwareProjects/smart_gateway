
#include <xbee_stack.h>
#include <xbee_zigbee_operations.h>
#include <smart_power_stack.h>


static u8 sZigbeeRfCommandRequestApiPacketBuffer[API_FRAME_REQUEST_BUFFER_SIZE] = {0};


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
        app_zigbee_response.receiveDataLen = rcv_data_len;
        XbeeZigbeeReceiveEventHandler(&app_zigbee_response);
    }
    else
    {
        LOG_ERR(("\nERR:: XbeeProcessZigbeeReceivePacket() Invalid rcv_data_len: %d", rcv_data_len));
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

    if(transmit_status.deliveryStatus != TX_DELIVERY_STATUS_SUCCESS)
    {
        LOG_ERR(("\nERR:: XbeeProcessZigbeeTransmitStatus() failed to transmit: %x %x %x %x %x %x", data[4], data[5], data[6], data[7], data[8], data[9]));
    }
    else
    {
        LOG_INFO(("\nXbeeProcessZigbeeTransmitStatus() transmitted successfully: %x %x %x %x %x %x", data[4], data[5], data[6], data[7], data[8], data[9]));
    }
}

s16 XbeeSendZigbeeTransmitRequest(AppXbeeZigbeeTransmitRequest *zigbee_tx_request)
{
    u16 len = 0x00;
    s16 ret = 0x00;

    LOG_INFO0(("\n<< %s >>", __func__));

    // cmd + frameId + destinationAddr(8) + destinationNetworkAddress[2] + broadcastRadius + options
    len = (zigbee_tx_request->rfDataLen + 14);

    sZigbeeRfCommandRequestApiPacketBuffer[3]   = ZIGBEE_TRANSMIT_REQUEST;
    sZigbeeRfCommandRequestApiPacketBuffer[4]   = FrameIdCounter++;
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
        LOG_ERR(("\nERR:: ProcessApiFrameRequest(): %d", ret));
        return -EXBEE_ZIGBEE_TX_REQUEST;
    }
    return EXBEE_OK;
}

void XbeeZigbeeReceiveEventHandler(AppXbeeZigbeeReceivePacket *zigbee_response)
{
    u16 count = 0x00;

    LOG_INFO0(("\n<< %s >>", __func__));

    LOG_INFO(("\nRx ZigbeePacket:: "));
    for(count = 0x00; count < ZIGBEE_RECEIVE_PACKET_SOURCE_ADDRESS_LEN; count++)
    {
        LOG_INFO((" %x", zigbee_response->rfPacketResponse.sourceAdress[count]));
    }
    LOG_INFO((" %x", (zigbee_response->rfPacketResponse.sourceNetworkAddress >> 8) & 0xFF));
    LOG_INFO((" %x", (zigbee_response->rfPacketResponse.sourceNetworkAddress) & 0xFF));
    LOG_INFO((" %x", zigbee_response->rfPacketResponse.receiveOption));

    if(zigbee_response->receiveDataLen > 0)
    {
        for(count = 0x00; count < zigbee_response->receiveDataLen; count++)
        {
            LOG_INFO((" %x", zigbee_response->rfPacketResponse.receiveData[count]));
        }
    }

    // validate network address


    // Process Smart Power Packet
    SmartPowerStackProcessCommand(zigbee_response->rfPacketResponse.receiveData, zigbee_response->receiveDataLen);
}

void XbeeZigbeeInit(void)
{
    //revert network address from memory
}

