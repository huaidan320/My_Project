#include "ScreenService.h"

ScreenService* ScreenService::pInstance = NULL;

S_VIDEO_ENCODER_INIT ScreenService::initVideoParam = {768, 480, 30};

SCREEN_STATE ScreenService::screenState = SCREEN_STATE_NULL;

ScreenService::ScreenService()
{
    //TODO: new object
}

ScreenService::~ScreenService()
{
    //TODO: delete object
}

ScreenService *ScreenService::getInstance()
{
    if(NULL == pInstance)
    {
        pInstance = new ScreenService();
        debug("pInstance = new ScreenService()");
    }

    return pInstance;
}

//Video channel callback
void ScreenService::videoDataReceive(u8 *, u32)
{
    //TODO: output video data
    debug("videoDataReceive() is invoked");
}

void ScreenService::videoHeartBeat()
{
    debug("videoHeartBeat() is invoked");
}

//CMD video channel callback
void ScreenService::cmdVideoEncoderInitDone(S_VIDEO_ENCODER_INIT_DONE *)
{
    debug("cmdVideoEncoderInitDone() is invoked");
    //TODO: callback size
    //TODO: update_screen_state
}

void ScreenService::cmdVideoEncoderFrameRateChangeDone(S_VIDEO_ENCODER_FRAME_RATE_CHANGE_DONE *)
{
    debug("cmdVideoEncoderFrameRateChangeDone() is invoked");
}

void ScreenService::cmdRegisterVideoEncoderJPEGAck()
{
    debug("cmdRegisterVideoEncoderJPEGAck is invoked");
}

void ScreenService::cmdScreenOn()
{
    debug("cmdScreenOn() is invoked");
}

void ScreenService::cmdScreenOff()
{
    debug("cmdScreenOff() is invoked");
}

void ScreenService::cmdScreenUserPresent()
{
    debug("cmdScreenUserPresent() is invoked");
}

STATE_CHANGE_RESULT ScreenService::updateScreenState(SCREEN_STATE nextState)
{
    STATE_CHANGE_RESULT stateRet = STATE_CHANGE_SUCCESS;

    switch (nextState)
    {
    case SCREEN_STATE_NULL:

        break;

    case SCREEN_STATE_INIT:
        if(screenState == SCREEN_STATE_NULL)
        {
            screenState = nextState;

            if(screenServiceInit() != 0)
            {
                //TODO: stop carlife
                log_err("screenServiceInit failed");
            }
        }
        break;

    case SCREEN_STATE_READY:
        if(screen_state == SCREEN_STATE_INIT)
        {
            screenState = nextState;

            if(ScreenServiceStart() != 0)
            {
                //TODO: stop carlife
                log_err("ScreenServiceStart failed");
            }

            //TODO: Notify the decoding
        }
        break;

    case SCREEN_STATE_FOREGROUND:

        break;

    case SCREEN_STATE_BACKGROUND:

        break;

    case SCREEN_STATE_ERROR:

        break;

    default:
        stateRet = STATE_CHANGE_INVALID;
        break;
    }

    return stateRet;
}

int ScreenService::screenServiceInit()
{
    int ret = 0;

    ret = CCarLifeLib::getInstance()->cmdVideoEncoderInit(&initVideoParam);
    if(ret == -1)
    {
        log_err("cmdVideoEncoderInit Send failed");
        return ret;
    }
    else
    {
        debug("cmdVideoEncoderInit Send success");
    }

    return ret;
}

int ScreenService::screenServiceStart()
{
    int ret = 0;

    ret = CCarLifeLib::getInstance()->cmdVideoEncoderStart();
    if(ret == -1)
    {
        log_err("cmdVideoEncoderStart Send failed");
        return ret;
    }
    else
    {
        debug("cmdVideoEncoderStart Send success");
    }

    //TODO set update_video_state(VIDEO_STATE_INIT)

    return ret;
}

int ScreenService::screenServicePause()
{
    int ret = 0;

    ret = CCarLifeLib::getInstance()->cmdVideoEncoderPause();
    if(ret == -1)
    {
        log_err("cmdVideoEncoderPause Send failed");
        return ret;
    }
    else
    {
        debug("cmdVideoEncoderPause Send success");
    }

    return ret;
}

int ScreenService::screenServiceReset()
{
    int ret = 0;

    ret = CCarLifeLib::getInstance()->cmdVideoEncoderReset();
    if(ret == -1)
    {
        log_err("cmdVideoEncoderReset Send failed");
        return ret;
    }
    else
    {
        debug("cmdVideoEncoderReset Send success");
    }

    return ret;
}

int ScreenService::screenServiceFrameRateChange(u32 frameRate)
{
    int ret = 0;
    S_VIDEO_ENCODER_FRAME_RATE_CHANGE videoParam;

    videoParam.frameRate = frameRate;

    ret = CCarLifeLib::getInstance()->cmdVideoEncoderFrameRateChange(&videoParam);
    if(ret == -1)
    {
        log_err("cmdVideoEncoderFrameRateChange Send failed");
        return ret;
    }
    else
    {
        debug("cmdVideoEncoderFrameRateChange Send success");
    }

    return ret;
}
