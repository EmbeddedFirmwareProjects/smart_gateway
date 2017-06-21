
#include <xbee_comman.h>
#include <xbee_stack_pvt.h>
#include <platform.h>

static void Notify_Xbee_Api_Frame_Response(u8 *pdata, u16 len);
static s16 processFrameData(void);
static s16 validateApiFrame(u8 *data);

u8 FrameIdCounter = 0x01;
static XbeePacketStatus sXbeePacketStatusList[XBEE_PACKET_STATUS_LIST_SIZE];
static u8 sXbeePacketProcessOffset = 0x00;

static PlatformOperations sXbeePlatformOperations = {
        .pRxFunc = Notify_Xbee_Api_Frame_Response,
};


s16 XbeeStackInit(void)
{
    s16 list_count = 0x00;

    for(list_count = 0x00;list_count < XBEE_PACKET_STATUS_LIST_SIZE; list_count++)
    {
        sXbeePacketStatusList[list_count].isInUse = false;
        sXbeePacketStatusList[list_count].isValid = false;
        sXbeePacketStatusList[list_count].dataLen = 0x00;
        memset(sXbeePacketStatusList[list_count].bufferPacket, 0x00, API_FRAME_MAX_RECEIVE_PACKET_SIZE);
    }

    return XbeePlatformInit(&sXbeePlatformOperations);

    return 0;
}

void XbeeHouseKeeping(void)
{
    u8 tmp_xbee_packet_process_offset = sXbeePacketProcessOffset;
    u8 list_count = sXbeePacketProcessOffset;

    if((sXbeePacketStatusList[list_count].isInUse == false) && (sXbeePacketStatusList[list_count].isValid == true))
    {
        sXbeePacketProcessOffset = list_count;
    }
    else
    {
        if((list_count + 1) >= XBEE_PACKET_STATUS_LIST_SIZE)
        {
            list_count = 0x00;
            for(;list_count < tmp_xbee_packet_process_offset; list_count++)
            {
                if(sXbeePacketStatusList[list_count].isValid == true)
                {
                    sXbeePacketProcessOffset = list_count;
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
            for(;list_count < XBEE_PACKET_STATUS_LIST_SIZE; list_count++)
            {
                if(sXbeePacketStatusList[list_count].isValid == true)
                {
                    sXbeePacketProcessOffset = list_count;
                    break;
                }
            }
            if(list_count >= XBEE_PACKET_STATUS_LIST_SIZE)
            {
                //LOG_INFO(("\nERR:: XbeeHouseKeeping:: No Xbee packet to process"));
                return;
            }
        }
    }
    sXbeePacketStatusList[sXbeePacketProcessOffset].isInUse = true;
    XbeeProcessApiFrameResponse();
    sXbeePacketStatusList[sXbeePacketProcessOffset].isInUse = false;
    sXbeePacketStatusList[sXbeePacketProcessOffset].isValid = false;
}

static void Notify_Xbee_Api_Frame_Response(u8 *pdata, u16 len)
{
    static u8 sXbeePushPointer = 0x00;
    static u8 sXbeeFrameLen = 0x00;
    static u8 sXbeePacketPushOffset = 0x00;

    u16 byte_count = 0x00;
    u8 list_count = 0x00;
    u8 tmp_xbee_packet_push_offset = 0x00;

    LOG_INFO0(("\n<< %s >>", __func__));

    if (pdata == '\0')      // reset previous incomplete packet on receive failure
    {
        sXbeePushPointer = 0x00;
        sXbeeFrameLen = 0x00;
        return ;
    }

    for(byte_count = 0x00; byte_count < len; byte_count++)
    {
        if((pdata[byte_count] == XBEE_API_FRAME_START_DELIMITER) && (sXbeePushPointer == 0x00))
        {
            tmp_xbee_packet_push_offset = sXbeePacketPushOffset;
            list_count = sXbeePacketPushOffset;
            if((sXbeePacketStatusList[list_count].isInUse == false) && (sXbeePacketStatusList[list_count].isValid == false))
            {
                sXbeePacketPushOffset = list_count;
            }
            else
            {
                if((list_count + 1) >= XBEE_PACKET_STATUS_LIST_SIZE)
                {
                    list_count = 0x00;
                    for(;list_count < tmp_xbee_packet_push_offset; list_count++)
                    {
                        if(sXbeePacketStatusList[list_count].isInUse == false)
                        {
                            sXbeePacketPushOffset = list_count;
                            break;
                        }
                    }
                    if(list_count >= tmp_xbee_packet_push_offset)
                    {
                        LOG_ERR(("\nERR:: Notify_Xbee_Api_Frame_Response:: buffer full rejecting response data"));
                        return;
                    }
                }
                else
                {
                    list_count++;
                    for(;list_count < XBEE_PACKET_STATUS_LIST_SIZE; list_count++)
                    {
                        if(sXbeePacketStatusList[list_count].isInUse == false)
                        {
                            sXbeePacketPushOffset = list_count;
                            break;
                        }
                    }
                    if(list_count >= XBEE_PACKET_STATUS_LIST_SIZE)
                    {
                        LOG_ERR(("\nERR:: Notify_Xbee_Api_Frame_Response:: buffer full rejecting response data"));
                        return;
                    }
                }
            }
            memset(sXbeePacketStatusList[sXbeePacketPushOffset].bufferPacket, 0x00, API_FRAME_MAX_RECEIVE_PACKET_SIZE);
            sXbeePacketStatusList[sXbeePacketPushOffset].isValid = false;
            sXbeePacketStatusList[sXbeePacketPushOffset].bufferPacket[sXbeePushPointer++] = pdata[byte_count];
        }
        else if(sXbeePushPointer > 0x00)
        {
            sXbeePacketStatusList[sXbeePacketPushOffset].bufferPacket[sXbeePushPointer] = pdata[byte_count];

            if(sXbeePushPointer == 0x01)
            {
                sXbeeFrameLen |= (pdata[byte_count] << 8) & 0xFF00;
            }
            else if(sXbeePushPointer == 0x02)
            {
                sXbeeFrameLen |= pdata[byte_count] & 0x00FF;
            }
            if(sXbeePushPointer > 2)
            {
                if(((sXbeeFrameLen + 4) - 1) == sXbeePushPointer)     // start delimiter(1) + len(2) + data bytes(n) + checksum(1)
                {
                    sXbeePacketStatusList[sXbeePacketPushOffset].isValid = true;
                    sXbeePacketStatusList[list_count].dataLen = sXbeePushPointer;
                    sXbeePushPointer = 0x00;
                    sXbeeFrameLen = 0x00;
                    LOG_INFO(("\n"));
                }
                else
                {
                    sXbeePushPointer++;
                }
            }
            else
            {
                sXbeePushPointer++;
            }
        }
    }
}

void XbeeProcessApiFrameResponse(void)
{
    s16 ret = 0x00;

    LOG_INFO0(("\n<< %s >>", __func__));

#ifdef DEBUG_ENABLED
    {
        u16 count = 0x00;
        u16 len = (pdata[1] << 8) | pdata[2];
        LOG_DBG(("\nRx Packet:: "));
        for(count = 0x00; count < (len + 4); count++)
        {
            LOG_DBG((" %x", pdata[count]));
        }
    }
#endif

    // validate api frame
    ret = validateApiFrame(sXbeePacketStatusList[sXbeePacketProcessOffset].bufferPacket);
    if(ret != EXBEE_OK)
    {
        LOG_ERR(("\nERR:: validateApiFrame():: %d", ret));
        return;
    }

    // process frame for application
    ret = processFrameData();
    if(ret != EXBEE_OK)
    {
        LOG_ERR(("ERR:: processFrameData():: %d\n", ret));
        return;
    }
}

static u8 calculateCheckSum(u8 *apapi_frame, u16 len)
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

static s16 validateApiFrame(u8 *data)
{
    u16 len = 0x00;
    u8 cal_check_sum = 0x00;

    LOG_INFO0(("\n<< %s >>", __func__));

    // verify start delimiter
    if(data[0] != XBEE_API_FRAME_START_DELIMITER)
    {
        return -EXBEE_START_DELIMITER;
    }

    len = (data[1] << 8) | data[2];

    // verify checkSumexclude
    cal_check_sum = calculateCheckSum(&data[3], len);  // exclude start delimiter, len (2) and checkSum
    if(cal_check_sum == data[(len + 3)])     // startDelimiter(1) + len(2)
    {
        return EXBEE_OK;
    }
    else
    {
        LOG_ERR(("\nERR:: cal_check_sum:: %x, checkSum:: %x", cal_check_sum, data[(len + 3)]));
        return -EXBEE_CHECKSUM;
    }
}

static s16 processFrameData(void)
{
    u32 api_identifier_list_len = 0x00;
    u32 count = 0x00;

    LOG_INFO0(("\n<< %s >>", __func__));

    api_identifier_list_len = sizeof(ProcessApiIdentifierList)/sizeof(ApiIdentifierList);

    for(count = 0x00; count < api_identifier_list_len; count++)
    {
        if(ProcessApiIdentifierList[count].apiIdentifier == sXbeePacketStatusList[sXbeePacketProcessOffset].bufferPacket[3])
        {
            ProcessApiIdentifierList[count].pFunc(sXbeePacketStatusList[sXbeePacketProcessOffset].bufferPacket);
        }
    }

    if(count > api_identifier_list_len)
    {
        return -EXBEE_CMDID;
    }
    else
    {
        return EXBEE_OK;
    }
}


s16 XbeeSendApiFrameRequest(u8* pdata, u16 len)
{
    LOG_INFO0(("\n<< %s >>", __func__));

    return sXbeePlatformOperations.pTxFunc(pdata, len);
}

s16 XbeeProcessApiFrameRequest(u8* pdata, u16 len)
{
    LOG_INFO0(("\n<< %s >>", __func__));

    pdata[0] = XBEE_API_FRAME_START_DELIMITER;

    pdata[1] = (len & 0xFF00) >> 8;
    pdata[2] = len & 0xFF;

    pdata[len + 3] = calculateCheckSum(&pdata[3], len); // exclude start delemiter + len (2)

    return XbeeSendApiFrameRequest(pdata, (len + 4));   // including start delimiter + len(2) + checksum
}
