
#include <xbee_stack.h>
#include <xbee_stack_pvt.h>

static ApiFramePacketBackup sApiPacketBackup[API_PACKET_BACKUP_SIZE];

static s16 processFrameData(u8 *data)
{
    u32 api_identifier_list_len = 0x00;
    u32 count = 0x00;

    LOG_INFO0(("\n<< %s >>", __func__));

    api_identifier_list_len = sizeof(ProcessApiIdentifierList)/sizeof(ApiIdentifierList);

    for(count = 0x00; count < api_identifier_list_len; count++)
    {
        if(ProcessApiIdentifierList[count].apiIdentifier == data[3])
        {
            ProcessApiIdentifierList[count].pFunc(data);
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
    if(data[0] != API_FRAME_START_DELIMITER) 
    {
        return -EXBEE_START_DELIMITER;
    }
    
    len = (data[1] << 8) | data[2];
    
    // verify checkSumexclude 
    cal_check_sum = calculateCheckSum(&data[3], len);  // exclude start delemiter, len (2) and checkSum
    LOG_DBG(("\ncal_check_sum:: %x, checkSum:: %x", cal_check_sum, data[(len + 3)]));   // startDelimiter(1) + len(2)
    if(cal_check_sum == data[(len + 3)])     // startDelimiter(1) + len(2)
    {
        return EXBEE_OK;
    }
    else
    {
        return -EXBEE_CHECKSUM;
    }
}

static void initApiFramePacketBackup(void)
{
    u16 packet_number = 0x00;
    u8 count = 0x00;
    
    LOG_INFO0(("\n<< %s >>", __func__));
    
    packet_number = sizeof(sApiPacketBackup)/sizeof(ApiFramePacketBackup);
    
    for(count = 0x00; count < packet_number; count++)
    {
        sApiPacketBackup[count].usageFlag = false;
        memset(sApiPacketBackup[count].buffer, 0x00, API_PACKET_BACKUP_BUFFER_SIZE);
    }
}

static s16 storeApiFrame(u8* pdata, u16 len, ApiFramePacketBackup **papiFrame_backup)
{
    u16 packet_number = 0x00;
    u8 count = 0x00;
    
    LOG_INFO0(("\n<< %s >>", __func__));
    
    packet_number = sizeof(sApiPacketBackup)/sizeof(ApiFramePacketBackup);
    
    for(count = 0x00; count < packet_number; count++)
    {
        if(sApiPacketBackup[count].usageFlag == false)
        {
            memcpy(sApiPacketBackup[count].buffer, pdata, len);
            sApiPacketBackup[count].usageFlag = true;
            *papiFrame_backup = &sApiPacketBackup[count];
            return EXBEE_OK;
        }
    }
    return -EXBEE_API_BACKUP;
}

void ProcessApiFrameResponse(u8* pdata, u16 len)
{
    ApiFramePacketBackup *papiFrame_backup = 0x00;
    s16 ret = 0x00;
        
    LOG_INFO0(("\n<< %s >>", __func__));
    
    // backup api frame
    ret = storeApiFrame(pdata, len, &papiFrame_backup);
    if(ret != EXBEE_OK)
    {
        LOG_ERR(("\nERR:: storeApiFrame():: %d", ret));
        return;
    }
    
#ifdef DEBUG_ENABLED
    {
        u16 count = 0x00;
        u16 len = (papiFrame_backup->buffer[1] << 8) | papiFrame_backup->buffer[2];
        LOG_DBG(("\nRx Packet:: "));
        for(count = 0x00; count < (len + 4); count++)
        {
            LOG_DBG((" %x", papiFrame_backup->buffer[count]));
        }
    }
#endif
    
    // validate api frame
    ret = validateApiFrame(papiFrame_backup->buffer);
    if(ret != EXBEE_OK)
    {
        LOG_ERR(("\nERR:: validateApiFrame():: %d", ret));
        papiFrame_backup->usageFlag = false;
        return;
    }
    
    // process frame data
    ret = processFrameData(papiFrame_backup->buffer);
    if(ret != EXBEE_OK)
    {
        LOG_ERR(("\nERR:: processFrameData():: %d", ret));
        papiFrame_backup->usageFlag = false;
        return;
    }
}

s16 ProcessApiFrameRequest(u8* pdata, u16 len)
{
    LOG_INFO0(("\n<< %s >>", __func__));

    pdata[0] = API_FRAME_START_DELIMITER;

    pdata[1] = (len & 0xFF00) >> 8;
    pdata[2] = len & 0xFF;

    pdata[len + 3] = calculateCheckSum(&pdata[3], len); // exclude start delemiter + len (2)

    return SendApiFrameRequest(pdata, (len + 4));   // including start delimiter + len(2) + checksum
}

void XbeeStackInit(void)
{
    LOG_INFO0(("\n<< %s >>", __func__));
    
    // init ApiFramePacketBackup
    initApiFramePacketBackup();
}
