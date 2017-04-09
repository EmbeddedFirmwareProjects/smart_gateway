
#include <xbee_stack.h>

static ApiFramePacketBackup sApiPacketBackup[API_PACKET_BACKUP_SIZE];


static void processAtCommandResponse(AtCommandResponse *pat_response)
{
    LOG_INFO0(("\n<< %s >>", __func__));
    
    switch(pat_response->atCommand)
    {
        case AtCommandList_MAX:
        default:
        {
            // command not hadled
        }
        break;
    }
}

static s16 processFrameData(u8 *data)
{
    LOG_INFO0(("\n<< %s >>", __func__));
    
    switch(data[3])                 // cmdID
    {
        case AT_COMMAND_RESPONSE:
        {
            AtCommandResponse at_response;
            processAtCommandResponse(&at_response);
        }
        break;

        default:
        {
            // command not hadled
            return -ECMDID;
        }
        break;
    }
    return EOK;
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
    
    // verify start deliminator
    if(data[0] != API_FRAME_START_DELIMITER) 
    {
        return -ESTART_DELIMITER;
    }
    
    len = (data[1] << 8) | data[2];
    
    // verify checkSumexclude 
    cal_check_sum = calculateCheckSum(&data[3], len);  // exclude checkSum
    LOG_DBG(("\ncal_check_sum:: %x, checkSum:: %x", cal_check_sum, data[(len + 3)]));
    if(cal_check_sum == data[(len + 3)])     // startDelimiter(1) + len(2)
    {
        return EOK;
    }
    else
    {
        return -ECHECKSUM;
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
            return EOK;
        }
    }
    return -EAPI_BACKUP;
}

void ProcessApiFrame(u8* pdata, u16 len)
{
    ApiFramePacketBackup *papiFrame_backup = 0x00;
    s16 ret = 0x00;
        
    LOG_INFO0(("\n<< %s >>", __func__));
    
    // backup api frame
    ret = storeApiFrame(pdata, len, &papiFrame_backup);
    if(ret != EOK)
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
    if(ret != EOK)
    {
        LOG_ERR(("\nERR:: validateApiFrame():: %d", ret));
        return;
    }
    
    // process frame data
    ret = processFrameData(papiFrame_backup->buffer);
    if(ret != EOK)
    {
        LOG_ERR(("\nERR:: processFrameData():: %d", ret));
        return;
    }
    papiFrame_backup->usageFlag = false;
}

void XbeeStackInit(void)
{
    LOG_INFO0(("\n<< %s >>", __func__));
    
    // init ApiFramePacketBackup
    initApiFramePacketBackup();
}