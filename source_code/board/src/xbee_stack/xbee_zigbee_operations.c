
#include <xbee_stack.h>
#include <xbee_zigbee_operations_pvt.h>
#include <xbee_zigbee_operations.h>
#include <smart_power_stack.h>

static XbeeZigbeePacketStatus sXbeeZigbeePacketStatusList[XBEE_ZIGBEE_PACKET_STATUS_LIST_SIZE];

static u8 sZigbeeRfCommandRequestApiPacketBuffer[API_FRAME_REQUEST_BUFFER_SIZE] = {0};
static u8 sXbeeZigbeePacketProcessOffset = 0x00;
static void XbeeProcessZigbeeReceivePacket(void);

void XbeeZigbeeOperationsInit(void)
{
    s16 list_count = 0x00;

    for(list_count = 0x00;list_count < XBEE_ZIGBEE_PACKET_STATUS_LIST_SIZE; list_count++)
    {
        sXbeeZigbeePacketStatusList[list_count].isInUse = false;
        sXbeeZigbeePacketStatusList[list_count].isValid = false;
        sXbeeZigbeePacketStatusList[list_count].dataLen = 0x00;
        sXbeeZigbeePacketStatusList[list_count].bufferPacket = '\0';
    }
}

void XbeeZigbeeHouseKeeping(void)
{
    u8 tmp_xbee_packet_process_offset = sXbeeZigbeePacketProcessOffset;
    u8 list_count = sXbeeZigbeePacketProcessOffset;

    if((sXbeeZigbeePacketStatusList[list_count].isInUse == false) && (sXbeeZigbeePacketStatusList[list_count].isValid == true))
    {
        sXbeeZigbeePacketProcessOffset = list_count;
    }
    else
    {
        if((list_count + 1) >= XBEE_ZIGBEE_PACKET_STATUS_LIST_SIZE)
        {
            list_count = 0x00;
            for(;list_count < tmp_xbee_packet_process_offset; list_count++)
            {
                if(sXbeeZigbeePacketStatusList[list_count].isValid == true)
                {
                    sXbeeZigbeePacketProcessOffset = list_count;
                    break;
                }
            }
            if(list_count >= tmp_xbee_packet_process_offset)
            {
                //LOG_INFO(("\nERR:: XbeeHouseKeeping:: No Xbee packet to process"));
                return;
            }
        }
        else
        {
            list_count++;
            for(;list_count < XBEE_ZIGBEE_PACKET_STATUS_LIST_SIZE; list_count++)
            {
                if(sXbeeZigbeePacketStatusList[list_count].isValid == true)
                {
                    sXbeeZigbeePacketProcessOffset = list_count;
                    break;
                }
            }
            if(list_count >= XBEE_ZIGBEE_PACKET_STATUS_LIST_SIZE)
            {
                //LOG_INFO(("\nERR:: XbeeHouseKeeping:: No Xbee packet to process"));
                return;
            }
        }
    }
    sXbeeZigbeePacketStatusList[sXbeeZigbeePacketProcessOffset].isInUse = true;
    XbeeProcessZigbeeReceivePacket();
    sXbeeZigbeePacketStatusList[sXbeeZigbeePacketProcessOffset].isInUse = false;
    sXbeeZigbeePacketStatusList[sXbeeZigbeePacketProcessOffset].isValid = false;
}


void XbeeZigbeePushReceivePacket(u8 *data)
{
    static u8 sXbeeZigbeePacketPushOffset = 0x00;

    u8 list_count = 0x00;
    u8 tmp_xbee_zigbee_packet_push_offset = 0x00;

    //LOG_INFO0(("\n<< %s >>", __func__));

    tmp_xbee_zigbee_packet_push_offset = sXbeeZigbeePacketPushOffset;
    list_count = sXbeeZigbeePacketPushOffset;
    if((sXbeeZigbeePacketStatusList[list_count].isInUse == false) && (sXbeeZigbeePacketStatusList[list_count].isValid == false))
    {
        sXbeeZigbeePacketPushOffset = list_count;
    }
    else
    {
        if((list_count + 1) >= XBEE_ZIGBEE_PACKET_STATUS_LIST_SIZE)
        {
            list_count = 0x00;
            for(;list_count < tmp_xbee_zigbee_packet_push_offset; list_count++)
            {
                if(sXbeeZigbeePacketStatusList[list_count].isInUse == false)
                {
                    sXbeeZigbeePacketPushOffset = list_count;
                    break;
                }
            }
            if(list_count >= tmp_xbee_zigbee_packet_push_offset)
            {
                LOG_ERR(("\nERR:: XbeeZigbeePushPacket:: xbee zigbee packet"));
                return;
            }
        }
        else
        {
            list_count++;
            for(;list_count < XBEE_ZIGBEE_PACKET_STATUS_LIST_SIZE; list_count++)
            {
                if(sXbeeZigbeePacketStatusList[list_count].isInUse == false)
                {
                    sXbeeZigbeePacketPushOffset = list_count;
                    break;
                }
            }
            if(list_count >= XBEE_ZIGBEE_PACKET_STATUS_LIST_SIZE)
            {
                LOG_ERR(("\nERR:: XbeeZigbeePushPacket:: buffer full rejecting xbee zigbee packet"));
                return;
            }
        }
    }
    sXbeeZigbeePacketStatusList[sXbeeZigbeePacketPushOffset].bufferPacket = data;
    sXbeeZigbeePacketStatusList[sXbeeZigbeePacketPushOffset].isValid = true;
}

static void XbeeProcessZigbeeReceivePacket(void)
{
    u16 len = 0x00;
    s16 rcv_data_len = 0x00;
    AppXbeeZigbeeReceivePacket app_zigbee_response = {{{0}}};

    LOG_INFO0(("\n<< %s >>", __func__));

    len = (sXbeeZigbeePacketStatusList[sXbeeZigbeePacketProcessOffset].bufferPacket[1] << 8) | sXbeeZigbeePacketStatusList[sXbeeZigbeePacketProcessOffset].bufferPacket[2];
    app_zigbee_response.rfPacketResponse.sourceAdress[0] = sXbeeZigbeePacketStatusList[sXbeeZigbeePacketProcessOffset].bufferPacket[4];
    app_zigbee_response.rfPacketResponse.sourceAdress[1] = sXbeeZigbeePacketStatusList[sXbeeZigbeePacketProcessOffset].bufferPacket[5];
    app_zigbee_response.rfPacketResponse.sourceAdress[2] = sXbeeZigbeePacketStatusList[sXbeeZigbeePacketProcessOffset].bufferPacket[6];
    app_zigbee_response.rfPacketResponse.sourceAdress[3] = sXbeeZigbeePacketStatusList[sXbeeZigbeePacketProcessOffset].bufferPacket[7];
    app_zigbee_response.rfPacketResponse.sourceAdress[4] = sXbeeZigbeePacketStatusList[sXbeeZigbeePacketProcessOffset].bufferPacket[8];
    app_zigbee_response.rfPacketResponse.sourceAdress[5] = sXbeeZigbeePacketStatusList[sXbeeZigbeePacketProcessOffset].bufferPacket[9];
    app_zigbee_response.rfPacketResponse.sourceAdress[6] = sXbeeZigbeePacketStatusList[sXbeeZigbeePacketProcessOffset].bufferPacket[10];
    app_zigbee_response.rfPacketResponse.sourceAdress[7] = sXbeeZigbeePacketStatusList[sXbeeZigbeePacketProcessOffset].bufferPacket[11];
    app_zigbee_response.rfPacketResponse.sourceNetworkAddress = (sXbeeZigbeePacketStatusList[sXbeeZigbeePacketProcessOffset].bufferPacket[12] << 8) | sXbeeZigbeePacketStatusList[sXbeeZigbeePacketProcessOffset].bufferPacket[13];
    app_zigbee_response.rfPacketResponse.receiveOption = sXbeeZigbeePacketStatusList[sXbeeZigbeePacketProcessOffset].bufferPacket[14];

    // cmd + sourceAdress(8) + sourceNetworkAddress[2] + receiveOption
    rcv_data_len = (len - 12);

    if(rcv_data_len > 0)
    {
        app_zigbee_response.rfPacketResponse.receiveData = &sXbeeZigbeePacketStatusList[sXbeeZigbeePacketProcessOffset].bufferPacket[15];
        app_zigbee_response.receiveDataLen = rcv_data_len;
        XbeeZigbeeReceiveEventHandler(&app_zigbee_response);
    }
    else
    {
        LOG_ERR(("\nERR:: XbeeProcessZigbeeReceivePacket() Invalid rcv_data_len: %d", rcv_data_len));
    }
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

void XbeeProcessZigbeeTransmitStatus(u8 *data)
{
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


