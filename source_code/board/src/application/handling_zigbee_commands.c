
#include <xbee_comman.h>
#include <smart_power_app.h>


void XbeeZigbeeReceiveEventHandler(AppXbeeZigbeeReceivePacket *zigbee_response)
{
    u16 count = 0x00;

    LOG_INFO0(("<< %s >>\n", __func__));

    LOG_INFO(("Rx ZigbeePacket:: "));
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
        LOG_INFO(("\n"));
    }

    // validate network address


    // Process Smart Power Packet
    SmartPowerAppProcessCommand(zigbee_response->rfPacketResponse.receiveData, zigbee_response->receiveDataLen);
}
