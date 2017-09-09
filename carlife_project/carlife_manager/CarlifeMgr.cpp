#include "CarlifeCommon.h"
#include "CarlifeMgr.h"
#include "SessionMgr.h"
#include "ScreenService.h"
#include "MediaService.h"
#include "NaviService.h"
#include "VrService.h"
#include "TelService.h"
#include "InputService.h"

enum CHANNEL_ID {
    CHANNEL_CMD = 1,
    CHANNEL_VIDEO,
    CHANNEL_MEDIA,
    CHANNEL_TTS,
    CHANNEL_VR
};

S_HU_PROTOCOL_VERSION CarlifeMgr::huProtocolVersion = {1, 0};

CarlifeMgr* CarlifeMgr::pInstance = NULL;

CarlifeMgr::CarlifeMgr()
{
    //TODO: new object
}

CarlifeMgr::~CarlifeMgr()
{
    //TODO: delete object
}

CarlifeMgr *CarlifeMgr::getInstance()
{
    if(NULL == pInstance)
    {
        pInstance = new CarlifeMgr();
        debug("pInstance = new CarlifeMgr()");
    }

    return pInstance;
}

int CarlifeMgr::startCarlifeConnection()
{
    int ret = 0;

    pthread_attr_t channel_thread_attr;

    pthread_attr_init(&channel_thread_attr);
    pthread_attr_setdetachstate(&channel_thread_attr, PTHREAD_CREATE_DETACHED);

    //Initialize CarLife library
    CCarLifeLib::getInstance()->carLifeLibInit();

    if(0 == CCarLifeLib::getInstance()->connectionSetup("127.0.0.1"))
    {
        debug("command/video/media/tts/vr/control channel connection have been set up!");
    }
    else
    {
        log_err("Connection set up failed!");
        return -1;
    }

    //Start Cmd channel thread
    ret = pthread_create(&channelThreadId[CHANNEL_CMD], &channel_thread_attr, cmdReceiveThread, NULL);
    if(ret != 0)
    {
        log_err("cmdReceiveThread create failed!");
        return -1;
    }
    else
    {
        debug("cmdReceiveThread create succcess!");
    }

    //Start Video channel thread
    ret = pthread_create(&channelThreadId[CHANNEL_VIDEO], &channel_thread_attr, videoReceiveThread, NULL);
    if(ret != 0)
    {
        log_err("videoReceivThread create failed!");
        return -1;
    }
    else
    {
        debug("videoReceivThread create succcess!");
    }

    //Start Media channel thread
    ret = pthread_create(&channelThreadId[CHANNEL_MEDIA], &channel_thread_attr, mediaReceiveThread, NULL);
    if(ret != 0)
    {
        log_err("mediaReceivThread create failed!");
        return -1;
    }
    else
    {
        debug("mediaReceivThread create succcess!");
    }

    //Start TTS channel thread
    ret = pthread_create(&channelThreadId[CHANNEL_TTS], &channel_thread_attr, ttsReceiveThread, NULL);
    if(ret != 0)
    {
        log_err("ttsReceivThread create failed!");
        return -1;
    }
    else
    {
        debug("ttsReceivThread create succcess!");
    }

    //Start VR channel thread
    ret = pthread_create(&channelThreadId[CHANNEL_VR], &channel_thread_attr, vrReceiveThread, NULL);
    if(ret != 0)
    {
        log_err("vrReceivThread create failed!");
        return -1;
    }
    else
    {
        debug("vrReceivThread create succcess!");
    }

    ret = CCarLifeLib::getInstance()->cmdHUProtoclVersion(&huProtocolVersion);
    if(ret != 0)
    {
        log_err("Send cmdHUProtoclVersion failed");
        return -1;
    }
    else
    {
        debug("Send cmdHUProtoclVersion success");
    }

    return ret;
}

int CarlifeMgr::cleanupCarlife()
{
    //kill adapter

    //cancel thread
    if(pthread_cancel(channelThreadId[CHANNEL_VR]) != 0 ||
            pthread_cancel(channelThreadId[CHANNEL_TTS]) != 0 ||
            pthread_cancel(channelThreadId[CHANNEL_MEDIA]) != 0 ||
            pthread_cancel(channelThreadId[CHANNEL_VIDEO]) != 0 ||
            pthread_cancel(channelThreadId[CHANNEL_CMD]) != 0)
    {
        log_warn("pthread_cancel failed");
    }

    //Close all sockets
    CCarLifeLib::getInstance()->disconnect();

    CCarLifeLib::getInstance()->carLifeLibDestory();

    return 0;
}

/* CMD channel receive thread */
void *CarlifeMgr::cmdReceiveThread(void *)
{
    //Register cmd channel callback

    /* SessionMgr */
    CCarLifeLib::getInstance()->cmdRegisterProtocolVersionMatchStatus(SessionMgr::cmdProtocolVersionMatchStatus);
    CCarLifeLib::getInstance()->cmdRegisterMDInfro(SessionMgr::cmdMDInfro);
    CCarLifeLib::getInstance()->cmdRegisterForeground(SessionMgr::cmdForeground);
    CCarLifeLib::getInstance()->cmdRegisterBackground(SessionMgr::cmdBackground);
    CCarLifeLib::getInstance()->cmdRegisterGoToDeskTop(SessionMgr::cmdGoToDeskTop);
    CCarLifeLib::getInstance()->cmdRegisterModuleStatus(SessionMgr::cmdModuleStatus);
    CCarLifeLib::getInstance()->cmdRegisterConnectException(SessionMgr::cmdRegisterConnectException);
    CCarLifeLib::getInstance()->cmdRegisterRequestGoToForeground(SessionMgr::cmdRegisterRequestGoToForeground);
    CCarLifeLib::getInstance()->cmdRegisterUIActionSound(SessionMgr::cmdRegisterUIActionSound);
    CCarLifeLib::getInstance()->cmdRegisterMdAuthenResponse(SessionMgr::cmdRegisterMdAuthenResponse);
    CCarLifeLib::getInstance()->cmdRegisterFeatureConfigRequest(SessionMgr::cmdRegisterFeatureConfigRequest);
    CCarLifeLib::getInstance()->cmdRegisterMdExit(SessionMgr::cmdRegisterMdExit);

    /* ScreenService */
    CCarLifeLib::getInstance()->cmdRegisterVideoEncoderInitDone(ScreenService::cmdVideoEncoderInitDone);
    CCarLifeLib::getInstance()->cmdRegisterVideoEncoderFrameRateChangeDone(ScreenService::cmdVideoEncoderFrameRateChangeDone);
    CCarLifeLib::getInstance()->cmdRegisterVideoEncoderJPEGAck(ScreenService::cmdRegisterVideoEncoderJPEGAck);
    CCarLifeLib::getInstance()->cmdRegisterScreenOn(ScreenService::cmdScreenOn);
    CCarLifeLib::getInstance()->cmdRegisterScreenOff(ScreenService::cmdScreenOff);
    CCarLifeLib::getInstance()->cmdRegisterScreenUserPresent(ScreenService::cmdScreenUserPresent);

    /* MediaService */
    CCarLifeLib::getInstance()->cmdRegisterMediaInfo(MediaService::cmdMediaInfo);
    CCarLifeLib::getInstance()->cmdRegisterMediaProgressBar(MediaService::cmdMediaProgressBar);

    /* NaviService */
    CCarLifeLib::getInstance()->cmdRegisterNaviNextTurnInfo(NaviService::cmdNaviNextTurnInfo);

    /* VrService */
    CCarLifeLib::getInstance()->cmdRegisterMicRecordWakeupStart(VrService::cmdMicRecordWakeupStart);
    CCarLifeLib::getInstance()->cmdRegisterMicRecordEnd(VrService::cmdMicRecordEnd);
    CCarLifeLib::getInstance()->cmdRegisterMicRecordRecogStart(VrService::cmdMicRecordRecogStart);

    /* TelService */
    CCarLifeLib::getInstance()->cmdRegisterMDBTPairInfro(TelService::cmdMDBTPairInfro);
    CCarLifeLib::getInstance()->cmdRegisterTelStateChangeIncoming(TelService::cmdTelStateChangeIncoming);
    CCarLifeLib::getInstance()->cmdRegisterTelStateChangeOutGoing(TelService::cmdTelStateChangeOutGoing);
    CCarLifeLib::getInstance()->cmdRegisterTelStateChangeIdle(TelService::cmdTelStateChangeIdle);
    CCarLifeLib::getInstance()->cmdRegisterTelStateChangeInCalling(TelService::cmdTelStateChangeInCalling);

    /* InputService */
    CCarLifeLib::getInstance()->cmdRegisterCarDataSubscribe(InputService::cmdCarDataSubscribe);
    CCarLifeLib::getInstance()->cmdRegisterCarDataSubscribeStart(InputService::cmdCarDataSubscribeStart);
    CCarLifeLib::getInstance()->cmdRegisterCarDataSubscribeStop(InputService::cmdCarDataSubscribeStop);

    while(1)
    {
        //Receive data from command channel
        if(-1 == CCarLifeLib::getInstance()->cmdReceiveOperation())
        {
            log_err("Command channel receive thread exit!");
            break;
        }
    }

    return NULL;
}

/* Video channel receive thread */
void *CarlifeMgr::videoReceiveThread(void *)
{
    //Register video callback
    CCarLifeLib::getInstance()->videoRegisterDataReceive(ScreenService::videoDataReceive);
    CCarLifeLib::getInstance()->videoRegisterHeartBeat(ScreenService::videoHeartBeat);

    while(1)
    {
        if(-1 == CCarLifeLib::getInstance()->videoReceiveOperation())
        {
            log_err("Video channel receive thread exit!");
            break;
        }
    }

    return NULL;
}

/* Media channel receive thread */
void *CarlifeMgr::mediaReceiveThread(void *)
{
    //Register meida callback
    CCarLifeLib::getInstance()->mediaRegisterInit(MediaService::mediaInit);
    CCarLifeLib::getInstance()->mediaRegisterNormalData(MediaService::mediaNormalData);
    CCarLifeLib::getInstance()->mediaRegisterStop(MediaService::mediaStop);
    CCarLifeLib::getInstance()->mediaRegisterPause(MediaService::mediaPause);
    CCarLifeLib::getInstance()->mediaRegisterResume(MediaService::mediaResume);
    CCarLifeLib::getInstance()->mediaRegisterSeek(MediaService::mediaSeek);

    while(1)
    {
        if(-1==CCarLifeLib::getInstance()->mediaReceiveOperation())
        {
            log_err("Media channel thread exit!");
            break;
        }
    }

    return NULL;
}

/* TTS channel receive thread */
void *CarlifeMgr::ttsReceiveThread(void *)
{
    //Register tts callback
    CCarLifeLib::getInstance()->ttsRegisterInit(NaviService::ttsInit);
    CCarLifeLib::getInstance()->ttsRegisterNormalData(NaviService::ttsNormalData);
    CCarLifeLib::getInstance()->ttsRegisterStop(NaviService::ttsStop);

    while(1)
    {
        if(-1==CCarLifeLib::getInstance()->ttsReceiveOperation())
        {
            log_err("TTS channel thread exit!");
            break;
        }
    }

    return NULL;
}

/* VR channel receive thread */
void *CarlifeMgr::vrReceiveThread(void *)
{
    //Register vr callback
    CCarLifeLib::getInstance()->vrRegisterInit(VrService::vrInit);
    CCarLifeLib::getInstance()->vrRegisterNormalData(VrService::vrNormalData);
    CCarLifeLib::getInstance()->vrRegisterStop(VrService::vrStop);

    while(1)
    {
        if(-1==CCarLifeLib::getInstance()->vrReceiveOperation())
        {
            log_err("VR channel thread exit!");
            break;
        }
    }

    return NULL;
}
