
#include <xbee_at_command.h>

#include<pthread.h>
static pthread_t tid;
static u8 *spApiFrame;
static u16 sApiFrameLen;

void * ThreadSendApiFrame(void *data)
{
    DelayMs(500);
    ProcessApiFrameResponse(spApiFrame, sApiFrameLen);
    pthread_exit(0);
}

static void TestSendThread()
{
    int err = 0x00;

    err = pthread_create(&tid, NULL, &ThreadSendApiFrame, NULL);
    if (err != 0)
    {
        LOG_INFO(("\ncan't create thread :[%s]", strerror(err)));
    }
}


static void TestProcessApiFrameResponse()
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
    {
        // send expected cmd
        u8 api_frame[] = {0x7E, 0x00, 0x07, AT_COMMAND_RESPONSE, 0x01, 'D', 'H', 0x00, 0x02, 0x01, 0xe7};
        spApiFrame = (u8 *)api_frame;
        sApiFrameLen = sizeof(api_frame);
        TestSendThread();
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
        TestSendThread();
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

void testXbeeStack(void)
{
    XbeeStackInit();

    TestProcessApiFrameResponse();
    TestXbeeSendAtCommandRequest();
    TestXbeeAtCommandExpectedResponse();
    TestXbeeSendAtCommandRequestExpectedResponse();
}
