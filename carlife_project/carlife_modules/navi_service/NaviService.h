#ifndef NAVISERVICE_H
#define NAVISERVICE_H

#include "CCarLifeLibWrapper.h"
#include "CarlifeCommon.h"

using namespace CCarLifeLibH;

class NaviService
{
public:
    ~NaviService();

    static NaviService *getInstance();

    //Navi TTS cmd channel
    static void cmdNaviNextTurnInfo(S_NAVI_NEXT_TURN_INFO *); //0x00010030

    //Navi TTS channel
    static void ttsInit(S_AUDIO_INIT_PARAMETER *);
    static void ttsNormalData(u8 *, u32);
    static void ttsStop(void);

    //TODO:updateNaviState

private:
    NaviService();
    static NaviService* pInstance;
};

#endif // NAVISERVICE_H
