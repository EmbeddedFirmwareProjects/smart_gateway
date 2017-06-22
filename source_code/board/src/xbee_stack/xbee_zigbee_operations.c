
#include <xbee_stack.h>
#include <xbee_zigbee_operations_pvt.h>
#include <xbee_zigbee_operations.h>
#include <smart_power_stack.h>


static void XbeeProcessZigbeeReceivePacket(void);


static XbeeZigbeeReceivePacket sXbeeZigbeeReceivePacketList[XBEE_ZIGBEE_PACKET_STATUS_LIST_SIZE];
static u8 sXbeeZigbeePacketProcessOffset = 0x00;
static u8 sXbeeZigbeeServerSourceAddress[XBEE_ZIGBEE_SOURCE_ADDRESS_LEN] = {0};
static u16 sXbeeZigbeeServerNetworkAddress = 0x00;


void XbeeZigbeeOperationsInit(void)
{
    s16 list_count = 0x00;

    for(list_count = 0x00;list_count < XBEE_ZIGBEE_PACKET_STATUS_LIST_SIZE; list_count++)
    {
        sXbeeZigbeeReceivePacketList[list_count].isInUse = false;
        sXbeeZigbeeReceivePacketList[list_count].isValid = false;
        sXbeeZigbeeReceivePacketList[list_count].dataLen = 0x00;
        sXbeeZigbeeReceivePacketList[list_count].bufferPacket = '\0';
    }
}

void XbeeZigbeeHouseKeeping(void)
{
    u8 tmp_xbee_packet_process_offset = sXbeeZigbeePacketProcessOffset;
    u8 list_count = sXbeeZigbeePacketProcessOffset;

    if((sXbeeZigbeeReceivePacketList[list_count].isInUse == false) && (sXbeeZigbeeReceivePacketList[list_count].isValid == true))
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
                if(sXbeeZigbeeReceivePacketList[list_count].isValid == true)
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
                if(sXbeeZigbeeReceivePacketList[list_count].isValid == true)
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
    sXbeeZigbeeReceivePacketList[sXbeeZigbeePacketProcessOffset].isInUse = true;
    XbeeProcessZigbeeReceivePacket();
    sXbeeZigbeeReceivePacketList[sXbeeZigbeePacketProcessOffset].isInUse = false;
    sXbeeZigbeeReceivePacketList[sXbeeZigbeePacketProcessOffset].isValid = false;
}

void XbeeZigbeePushReceivePacket(u8 *data)
{
    static u8 sXbeeZigbeePacketPushOffset = 0x00;

    u8 list_count = 0x00;
    u8 tmp_xbee_zigbee_packet_push_offset = 0x00;

    //LOG_INFO0(("\n<< %s >>", __func__));

    tmp_xbee_zigbee_packet_push_offset = sXbeeZigbeePacketPushOffset;
    list_count = sXbeeZigbeePacketPushOffset;
    if((sXbeeZigbeeReceivePacketList[list_count].isInUse == false) && (sXbeeZigbeeReceivePacketList[list_count].isValid == false))
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
                if(sXbeeZigbeeReceivePacketList[list_count].isInUse == false)
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
                if(sXbeeZigbeeReceivePacketList[list_count].isInUse == false)
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
    sXbeeZigbeeReceivePacketList[sXbeeZigbeePacketPushOffset].bufferPacket = data;
    sXbeeZigbeeReceivePacketList[sXbeeZigbeePacketPushOffset].isValid = true;
}

static void XbeeProcessZigbeeReceivePacket(void)
{
    u16 len = 0x00;
    s16 rcv_data_len = 0x00;
    u8 count = 0x00;

    LOG_INFO0(("\n<< %s >>", __func__));

    len = (sXbeeZigbeeReceivePacketList[sXbeeZigbeePacketProcessOffset].bufferPacket[1] << 8) | sXbeeZigbeeReceivePacketList[sXbeeZigbeePacketProcessOffset].bufferPacket[2];
    // cmd + sourceAdress(8) + sourceNetworkAddress[2] + receiveOption
    rcv_data_len = (len - 12);

    if(rcv_data_len > 0)
    {
        {
            AppXbeeZigbeeReceivePacket app_zigbee_response = {{0}};

            app_zigbee_response.rfPacketResponse.sourceAdress = (u8 *) &sXbeeZigbeeReceivePacketList[sXbeeZigbeePacketProcessOffset].bufferPacket[4];
            app_zigbee_response.rfPacketResponse.sourceNetworkAddress = (sXbeeZigbeeReceivePacketList[sXbeeZigbeePacketProcessOffset].bufferPacket[12] << 8) | sXbeeZigbeeReceivePacketList[sXbeeZigbeePacketProcessOffset].bufferPacket[13];
            app_zigbee_response.rfPacketResponse.receiveOption = sXbeeZigbeeReceivePacketList[sXbeeZigbeePacketProcessOffset].bufferPacket[14];

            app_zigbee_response.rfPacketResponse.receiveData = &sXbeeZigbeeReceivePacketList[sXbeeZigbeePacketProcessOffset].bufferPacket[15];
            app_zigbee_response.receiveDataLen = rcv_data_len;

            LOG_INFO(("\nRx ZigbeePacket:: "));
            for(count = 0x00; count < XBEE_ZIGBEE_SOURCE_ADDRESS_LEN; count++)
            {
                LOG_INFO((" %x", (unsigned int) app_zigbee_response.rfPacketResponse.sourceAdress[count]));
            }
            LOG_INFO((" %x", (app_zigbee_response.rfPacketResponse.sourceNetworkAddress >> 8) & 0xFF));
            LOG_INFO((" %x", (app_zigbee_response.rfPacketResponse.sourceNetworkAddress) & 0xFF));
            LOG_INFO((" %x", app_zigbee_response.rfPacketResponse.receiveOption));

            if(app_zigbee_response.receiveDataLen > 0)
            {
                for(count = 0x00; count < app_zigbee_response.receiveDataLen; count++)
                {
                    LOG_INFO((" %x", app_zigbee_response.rfPacketResponse.receiveData[count]));
                }
            }
        }

        // validate zigbee source address and network address


        // Process Smart Power Packet
        SmartPowerStackProcessCommand(&sXbeeZigbeeReceivePacketList[sXbeeZigbeePacketProcessOffset].bufferPacket[15], rcv_data_len);
    }
    else
    {
        LOG_ERR(("\nERR:: XbeeProcessZigbeeReceivePacket() Invalid rcv_data_len: %d", rcv_data_len));
    }
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
        LOG_ERR(("\nERR:: XbeeProcessZigbeeTransmitStatus() failed to transmit: %x %x %x %x %x %x\n", data[4], data[5], data[6], data[7], data[8], data[9]));
    }
    else
    {
        LOG_INFO(("\nXbeeProcessZigbeeTransmitStatus() transmitted successfully: %x %x %x %x %x %x\n", data[4], data[5], data[6], data[7], data[8], data[9]));
    }
}

s16 XbeeSendZigbeeTransmitRequest(u8 *data, u8 data_len)
{
    u16 len = 0x00;
    s16 ret = 0x00;
    u8 xbee_zigbee_request_packet_buffer[API_FRAME_REQUEST_BUFFER_SIZE] = {0};

    LOG_INFO0(("\n<< %s >>", __func__));

    // cmd + frameId + destinationAddr(8) + destinationNetworkAddress[2] + broadcastRadius + options
    len = (data_len + 14);

    xbee_zigbee_request_packet_buffer[3]   = ZIGBEE_TRANSMIT_REQUEST;
    xbee_zigbee_request_packet_buffer[4]   = FrameIdCounter++;
    xbee_zigbee_request_packet_buffer[5]   = sXbeeZigbeeServerSourceAddress[0];
    xbee_zigbee_request_packet_buffer[6]   = sXbeeZigbeeServerSourceAddress[1];
    xbee_zigbee_request_packet_buffer[7]   = sXbeeZigbeeServerSourceAddress[2];
    xbee_zigbee_request_packet_buffer[8]   = sXbeeZigbeeServerSourceAddress[3];
    xbee_zigbee_request_packet_buffer[9]   = sXbeeZigbeeServerSourceAddress[4];
    xbee_zigbee_request_packet_buffer[10]  = sXbeeZigbeeServerSourceAddress[5];
    xbee_zigbee_request_packet_buffer[11]  = sXbeeZigbeeServerSourceAddress[6];
    xbee_zigbee_request_packet_buffer[12]  = sXbeeZigbeeServerSourceAddress[7];
    xbee_zigbee_request_packet_buffer[13]  = (sXbeeZigbeeServerNetworkAddress & 0xFF);
    xbee_zigbee_request_packet_buffer[14]  = (sXbeeZigbeeServerNetworkAddress & 0XFF00) >> 8;
    xbee_zigbee_request_packet_buffer[15]  = 0x00;
    xbee_zigbee_request_packet_buffer[16]  = ZIGBEE_OPTIONS_DISABLE_RETRIES | ZIGBEE_OPTIONS_ENABLE_APS_ENCRYPTION;

    if(data_len > 0)
    {
        memcpy(&xbee_zigbee_request_packet_buffer[17], data, data_len);
    }

    ret = XbeeSendApiFrameRequest(xbee_zigbee_request_packet_buffer, len);
    if(ret != 0x00)
    {
        LOG_ERR(("\nERR:: ProcessApiFrameRequest(): %d", ret));
        return -EXBEE_ZIGBEE_TX_REQUEST;
    }
    return EXBEE_OK;
}


