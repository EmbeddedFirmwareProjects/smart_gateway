
#include <xbee_at_command.h>
#include <xbee_zigbee_operations.h>

#ifdef __linux__
#include<pthread.h>
static pthread_t tid;
static pthread_t tid1;
#endif

static u8 *spApiFrame;
static u16 sApiFrameLen;
static u32 sApiFrameDelay;
static u8 *spApiFrame1;
static u16 sApiFrameLen1;
static u32 sApiFrameDelay1;

static void * ThreadSendApiFrame(void *data)
{
    DelayMs(sApiFrameDelay);
    ProcessApiFrameResponse(spApiFrame, sApiFrameLen);
    pthread_exit(0);
}
static void * ThreadSendApiFrame1(void *data)
{
    DelayMs(sApiFrameDelay1);
    ProcessApiFrameResponse(spApiFrame1, sApiFrameLen1);
    pthread_exit(0);
}

static void TestSendThread(u32 delayed_time)
{
    int err = 0x00;
    sApiFrameDelay = delayed_time;

#ifdef __linux__
    err = pthread_create(&tid, NULL, &ThreadSendApiFrame, NULL);
#endif
    if (err != 0)
    {
        LOG_INFO(("\ncan't create thread :[%s]", strerror(err)));
    }
}
static void TestSendThread1(u16 delayed_time)
{
    int err = 0x00;
    sApiFrameDelay1 = delayed_time;

#ifdef __linux__
    err = pthread_create(&tid1, NULL, &ThreadSendApiFrame1, NULL);
#endif
    if (err != 0)
    {
        LOG_INFO(("\ncan't create thread :[%s]", strerror(err)));
    }
}


static void TestAtCmdProcessApiFrameResponse()
{
    LOG_INFO(("\n\n----------------- %s -----------------", __func__));

    LOG_INFO(("\nTestValidFrame:: "));
    u8 api_frame[] = {0x7E, 0x00, 0x07, AT_COMMAND_RESPONSE, 0x01, 'D', 'H', 0x00, 0x02, 0x01, 0xe7};
    ProcessApiFrameResponse(api_frame, (u16)sizeof(api_frame));
}

static void TestXbeeSendAtCommandRequest()
{
    u8 param[] = {0x02, 0x01};
    AppXbeeAtCommandFrame at_cmd;

    LOG_INFO(("\n\n----------------- %s -----------------", __func__));

    at_cmd.atCmdFrame.frameId = 0x01;
    at_cmd.atCmdFrame.atCommand = 0x4448;
    at_cmd.atCmdFrame.parameterValue = (u8 *)&param;
    at_cmd.parameterLen = sizeof(param);
    XbeeSendAtCommandRequest(&at_cmd);
}

static void TestXbeeAtCommandExpectedResponse()
{
    s16 ret = 0x00;
    AppXbeeAtCommandResponse expected_response = {{0}};

    LOG_INFO(("\n\n----------------- %s -----------------", __func__));

    LOG_INFO(("\nTest on reception of expected at cmd"));
    expected_response.atCmdResponse.atCommand = ('D' << 8) | 'H';
    {// send expected cmd
        u8 api_frame[] = {0x7E, 0x00, 0x07, AT_COMMAND_RESPONSE, 0x01, 'D', 'H', 0x00, 0x02, 0x01, 0xe7};
        spApiFrame = (u8 *)api_frame;
        sApiFrameLen = sizeof(api_frame);
        TestSendThread(500);
    }
    ret = XbeeAtCommandExpectedResponse(&expected_response, 2000);
    if(ret == 0x00)
    {
        u16 count = 0x00;
        LOG_INFO(("\nRx AT Command(%d):: ", expected_response.commandDataLen));
        LOG_INFO((" %02x", expected_response.atCmdResponse.frameId));
        LOG_INFO((" %02x", expected_response.atCmdResponse.atCommand >> 8));
        LOG_INFO((" %02x", expected_response.atCmdResponse.atCommand & 0xFF));
        LOG_INFO((" %02x", expected_response.atCmdResponse.commandStatus));

        if(expected_response.commandDataLen > 0)
        {
            for(count = 0x00; count < 2; count++)
            {
                LOG_INFO((" %02x", expected_response.atCmdResponse.commandData[count]));
            }
        }
    }
    else
    {
        LOG_INFO(("\nret:: %d", ret));
    }

    LOG_INFO(("\n\nTest Time out"));
    ret = XbeeAtCommandExpectedResponse(&expected_response, 2000);
    LOG_INFO(("\nret:: %d", ret));
}

static void TestXbeeSendAtCommandRequestExpectedResponse()
{
    s16 ret = 0x00;
    AppXbeeAtCommandResponse expected_response = {{0}};
    u8 param[] = {0x03, 0x04};
    AppXbeeAtCommandFrame at_cmd;

    LOG_INFO(("\n\n----------------- %s -----------------", __func__));

    LOG_INFO(("\nTest send and expected at cmd"));
    {
        // send expected cmd
        u8 api_frame[] = {0x7E, 0x00, 0x07, AT_COMMAND_RESPONSE, 0x01, 'D', 'H', 0x00, 0x02, 0x01, 0xe7};
        spApiFrame = (u8 *)api_frame;
        sApiFrameLen = sizeof(api_frame);
        TestSendThread(500);
    }
    at_cmd.atCmdFrame.frameId = 0x01;
    at_cmd.atCmdFrame.atCommand = 0x4448;
    at_cmd.atCmdFrame.parameterValue = (u8 *)&param;
    at_cmd.parameterLen = sizeof(param);

    expected_response.atCmdResponse.atCommand = ('D' << 8) | 'H';

    ret = XbeeSendAtCommandRequestExpectedResponse(&at_cmd, &expected_response, 2000);
    if(ret == 0x00)
    {
        u16 count = 0x00;
        LOG_INFO(("\nRx AT Command(%d):: ", expected_response.commandDataLen));
        LOG_INFO((" %02x", expected_response.atCmdResponse.frameId));
        LOG_INFO((" %02x", expected_response.atCmdResponse.atCommand >> 8));
        LOG_INFO((" %02x", expected_response.atCmdResponse.atCommand & 0xFF));
        LOG_INFO((" %02x", expected_response.atCmdResponse.commandStatus));

        if(expected_response.commandDataLen > 0)
        {
            for(count = 0x00; count < 2; count++)
            {
                LOG_INFO((" %02x", expected_response.atCmdResponse.commandData[count]));
            }
        }
    }
    else
    {
        LOG_INFO(("\nret:: %d", ret));
    }
}

void TestAtCommandOperations()
{
    TestAtCmdProcessApiFrameResponse();
    TestXbeeSendAtCommandRequest();
    TestXbeeAtCommandExpectedResponse();
    TestXbeeSendAtCommandRequestExpectedResponse();
}


static void TestZigbeeProcessApiFrameResponse()
{
    s16 count = 0x00;
    u8 api_frame[] = {
            0x7E,
            0x00, 0x11,
            ZIGBEE_RECEIVE_PACKET,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0x12, 0x23,
            0x21,
            'D', 'H', 0x00, 0x02, 0x01,
            0x92
    };

    LOG_INFO(("\n\n----------------- %s -----------------", __func__));

    LOG_INFO(("\nTestValidFrame:: "));

    LOG_INFO(("\nProcessing Packet:: "));
    for(count = 0x00; count < sizeof(api_frame); count++)
    {
        LOG_INFO((" %x", api_frame[count]));
    }
    ProcessApiFrameResponse(api_frame, (u16)sizeof(api_frame));
}

static void TestXbeeSendZigbeeTransmitRequest()
{
    s16 ret = 0x00;
    u8 param[] = {0x02, 0x01, 0x00};
    AppXbeeZigbeeTransmitRequest zigbee_request;

    LOG_INFO(("\n\n----------------- %s -----------------", __func__));
    LOG_INFO(("\nTest Timeout Tx status:: "));
    zigbee_request.rfTransmitRequest.frameId = 0x01;
    zigbee_request.rfTransmitRequest.destinationAdress[0] = 0xFF;
    zigbee_request.rfTransmitRequest.destinationAdress[1] = 0xFF;
    zigbee_request.rfTransmitRequest.destinationAdress[2] = 0xFF;
    zigbee_request.rfTransmitRequest.destinationAdress[3] = 0xFF;
    zigbee_request.rfTransmitRequest.destinationAdress[4] = 0xFF;
    zigbee_request.rfTransmitRequest.destinationAdress[5] = 0xFF;
    zigbee_request.rfTransmitRequest.destinationAdress[6] = 0xFF;
    zigbee_request.rfTransmitRequest.destinationAdress[7] = 0xFF;
    zigbee_request.rfTransmitRequest.destinationNetworkAddress[0] = 0x01;
    zigbee_request.rfTransmitRequest.destinationNetworkAddress[1] = 0x11;
    zigbee_request.rfTransmitRequest.broadcastRadius = 0x00;
    zigbee_request.rfTransmitRequest.options = ZIGBEE_OPTIONS_DISABLE_RETRIES | ZIGBEE_OPTIONS_ENABLE_APS_ENCRYPTION;
    zigbee_request.rfTransmitRequest.rfData = &param[0];
    zigbee_request.rfDataLen = sizeof(param);
    ret = XbeeSendZigbeeTransmitRequest(&zigbee_request, 200);
    LOG_INFO(("\nret:: %d", ret));

    LOG_INFO(("\n\nTest Valid Tx status:: "));
    {// send transmit status packet
        u8 api_frame[] = {0x7E, 0x00, 0x07, ZIGBEE_TRANSMIT_STATUS, 0x01, 0x11, 0x11, 0x02, TX_DELIVERY_STATUS_SUCCESS, TX_DISCOVERY_STATUS_ADDRESS_AND_ROUTE, 0x4C};
        spApiFrame = (u8 *)api_frame;
        sApiFrameLen = sizeof(api_frame);
        TestSendThread(500);
    }
    ret = XbeeSendZigbeeTransmitRequest(&zigbee_request, 2000);
    LOG_INFO(("\nret:: %d", ret));

    LOG_INFO(("\n\nTest invalid Tx status:: "));
    {// send transmit status packet
        u8 api_frame[] = {0x7E, 0x00, 0x07, ZIGBEE_TRANSMIT_STATUS, 0x01, 0x11, 0x11, 0x02, TX_DELIVERY_STATUS_MAC_ACK_FAILURE, TX_DISCOVERY_STATUS_ADDRESS_AND_ROUTE, 0x4B};
        spApiFrame = (u8 *)api_frame;
        sApiFrameLen = sizeof(api_frame);
        TestSendThread(500);
    }
    ret = XbeeSendZigbeeTransmitRequest(&zigbee_request, 2000);
    LOG_INFO(("\nret:: %d", ret));
}

static void TestXbeeSendZigbeeTransmitRequestExpectedResponse()
{
    s16 ret = 0x00;
    u8 param[] = {0x02, 0x01, 0x00};
    AppXbeeZigbeeTransmitRequest zigbee_request;
    AppXbeeZigbeeReceivePacket zigbee_response;
    u8 api_frame[] = {0x7E, 0x00, 0x07, ZIGBEE_TRANSMIT_STATUS, 0x01, 0x11, 0x11, 0x02, TX_DELIVERY_STATUS_SUCCESS, TX_DISCOVERY_STATUS_ADDRESS_AND_ROUTE, 0x4C};
    u8 api_frame1[] = {
        0x7E,
        0x00, 0x11,
        ZIGBEE_RECEIVE_PACKET,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x12, 0x23,
        0x21,
        'D', 'H', 0x00, 0x02, 0x01,
        0x92
    };

    LOG_INFO(("\n\n----------------- %s -----------------", __func__));
    LOG_INFO(("\nTest Timeout response:: "));
    zigbee_request.rfTransmitRequest.frameId = 0x01;
    zigbee_request.rfTransmitRequest.destinationAdress[0] = 0xFF;
    zigbee_request.rfTransmitRequest.destinationAdress[1] = 0xFF;
    zigbee_request.rfTransmitRequest.destinationAdress[2] = 0xFF;
    zigbee_request.rfTransmitRequest.destinationAdress[3] = 0xFF;
    zigbee_request.rfTransmitRequest.destinationAdress[4] = 0xFF;
    zigbee_request.rfTransmitRequest.destinationAdress[5] = 0xFF;
    zigbee_request.rfTransmitRequest.destinationAdress[6] = 0xFF;
    zigbee_request.rfTransmitRequest.destinationAdress[7] = 0xFF;
    zigbee_request.rfTransmitRequest.destinationNetworkAddress[0] = 0x01;
    zigbee_request.rfTransmitRequest.destinationNetworkAddress[1] = 0x11;
    zigbee_request.rfTransmitRequest.broadcastRadius = 0x00;
    zigbee_request.rfTransmitRequest.options = ZIGBEE_OPTIONS_DISABLE_RETRIES | ZIGBEE_OPTIONS_ENABLE_APS_ENCRYPTION;
    zigbee_request.rfTransmitRequest.rfData = &param[0];
    zigbee_request.rfDataLen = sizeof(param);

    {// send transmit status packet
        spApiFrame = (u8 *)api_frame;
        sApiFrameLen = sizeof(api_frame);
        TestSendThread(500);
    }
    ret = XbeeSendZigbeeTransmitRequestExpectedResponse(&zigbee_request, &zigbee_response, 2000);
    LOG_INFO(("\nret:: %d", ret));

    LOG_INFO(("\n\nTest Valid response:: "));
    {// send transmit status packet
        spApiFrame = (u8 *)api_frame;
        sApiFrameLen = sizeof(api_frame);
        TestSendThread(500);
    }
    {// send valid packet
        spApiFrame1 = (u8 *)api_frame1;
        sApiFrameLen1 = sizeof(api_frame1);
        TestSendThread1(1000);
    }
    ret = XbeeSendZigbeeTransmitRequestExpectedResponse(&zigbee_request, &zigbee_response, 2000);
    LOG_INFO(("\nret:: %d", ret));
}

static void TestXbeeZigbeeExpectedResponse()
{
    s16 ret = 0x00;
    u8 api_frame1[] = {
        0x7E,
        0x00, 0x11,
        ZIGBEE_RECEIVE_PACKET,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x12, 0x23,
        0x21,
        'D', 'H', 0x00, 0x02, 0x01,
        0x92
    };
    AppXbeeZigbeeReceivePacket zigbee_response;

    LOG_INFO(("\n\n----------------- %s -----------------", __func__));
    LOG_INFO(("\nTest Timeout response:: "));
    ret = XbeeZigbeeExpectedResponse(&zigbee_response, 2000);
    LOG_INFO(("\nret:: %d", ret));

    LOG_INFO(("\n\nTest Valid response:: "));
    {// send valid packet
        spApiFrame1 = (u8 *)api_frame1;
        sApiFrameLen1 = sizeof(api_frame1);
        TestSendThread1(500);
    }
    ret = XbeeZigbeeExpectedResponse(&zigbee_response, 2000);
    LOG_INFO(("\nret:: %d", ret));
}

void TestZigbeeOperations()
{
    TestZigbeeProcessApiFrameResponse();
    TestXbeeSendZigbeeTransmitRequest();
    TestXbeeSendZigbeeTransmitRequestExpectedResponse();
    TestXbeeZigbeeExpectedResponse();
}

void testXbeeStack(void)
{
    XbeeStackInit();

    TestAtCommandOperations();

    TestZigbeeOperations();
}
