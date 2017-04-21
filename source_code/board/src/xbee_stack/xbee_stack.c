
#include <xbee_stack.h>
#include <xbee_stack_pvt.h>

static s16 processFrameData(u8 *data)
{
    u32 api_identifier_list_len = 0x00;
    u32 count = 0x00;

    LOG_INFO0(("<< %s >>\n", __func__));

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
    
    LOG_INFO0(("<< %s >>\n", __func__));
    
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
    
    LOG_INFO0(("<< %s >>\n", __func__));
    
    // verify start delimiter
    if(data[0] != XBEE_API_FRAME_START_DELIMITER)
    {
        return -EXBEE_START_DELIMITER;
    }
    
    len = (data[1] << 8) | data[2];
    
    // verify checkSumexclude 
    cal_check_sum = calculateCheckSum(&data[3], len);  // exclude start delemiter, len (2) and checkSum
    if(cal_check_sum == data[(len + 3)])     // startDelimiter(1) + len(2)
    {
        return EXBEE_OK;
    }
    else
    {
        LOG_ERR(("ERR:: cal_check_sum:: %x, checkSum:: %x\n", cal_check_sum, data[(len + 3)]));
        return -EXBEE_CHECKSUM;
    }
}

void XbeeProcessApiFrameResponse(u8* pdata, u16 len)
{
    s16 ret = 0x00;
        
    LOG_INFO0(("<< %s >>\n", __func__));
    
#ifdef DEBUG_ENABLED
    {
        u16 count = 0x00;
        u16 len = (pdata[1] << 8) | pdata[2];
        LOG_DBG(("Rx Packet:: "));
        for(count = 0x00; count < (len + 4); count++)
        {
            LOG_DBG((" %x", pdata[count]));
        }
    }
#endif
    
    // validate api frame
    ret = validateApiFrame(pdata);
    if(ret != EXBEE_OK)
    {
        LOG_ERR(("ERR:: validateApiFrame():: %d\n", ret));
        return;
    }
    
    // process frame data
    ret = processFrameData(pdata);
    if(ret != EXBEE_OK)
    {
        LOG_ERR(("ERR:: processFrameData():: %d\n", ret));
        return;
    }
}

s16 XbeeProcessApiFrameRequest(u8* pdata, u16 len)
{
    LOG_INFO0(("<< %s >>\n", __func__));

    pdata[0] = XBEE_API_FRAME_START_DELIMITER;

    pdata[1] = (len & 0xFF00) >> 8;
    pdata[2] = len & 0xFF;

    pdata[len + 3] = calculateCheckSum(&pdata[3], len); // exclude start delemiter + len (2)

    return XbeeSendApiFrameRequest(pdata, (len + 4));   // including start delimiter + len(2) + checksum
}

