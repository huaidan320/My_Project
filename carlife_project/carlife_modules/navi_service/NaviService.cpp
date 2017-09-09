#include "NaviService.h"

NaviService* NaviService::pInstance = NULL;

NaviService::NaviService()
{
    //TODO: new object
}

NaviService::~NaviService()
{
    //TODO: delete object
}

NaviService *NaviService::getInstance()
{
    if(NULL == pInstance)
    {
        pInstance = new NaviService();
        debug("pInstance = new NaviService()");
    }

    return pInstance;
}

//Navi TTS cmd channel
void NaviService::cmdNaviNextTurnInfo(S_NAVI_NEXT_TURN_INFO *)
{
    debug("cmdNaviNextTurnInfo is invoked");
}

//Navi TTS channel
void NaviService::ttsInit(S_AUDIO_INIT_PARAMETER *initParam)
{
    debug("ttsInit() is invoked");
    debug("sampleRate: %d", sampleRate);
    debug("channelConfig: %d", channelConfig);
    debug("sampleFormat: %d", sampleFormat);
}

void NaviService::ttsNormalData(u8 *, u32)
{
    debug("ttsNormalData() is invoked");
}

void NaviService::ttsStop()
{
    debug("ttsStop() is invoked");
}
