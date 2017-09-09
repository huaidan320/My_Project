#ifndef SCREENSERVICE_H
#define SCREENSERVICE_H

#include "CCarLifeLibWrapper.h"
#include "CarlifeCommon.h"

using namespace CCarLifeLibH;

typedef enum
{
    SCREEN_STATE_NULL,
    SCREEN_STATE_INIT,
    SCREEN_STATE_READY,
    SCREEN_STATE_FOREGROUND,
    SCREEN_STATE_BACKGROUND,
    SCREEN_STATE_ERROR,
}SCREEN_STATE;

class ScreenService
{
public:
    ~ScreenService();

    static ScreenService *getInstance();

    //Video channel callback
    static void videoDataReceive(u8 *, u32);
    static void videoHeartBeat(void);

    //CMD video channel callback
    static void cmdVideoEncoderInitDone(S_VIDEO_ENCODER_INIT_DONE *);
    static void cmdVideoEncoderFrameRateChangeDone(S_VIDEO_ENCODER_FRAME_RATE_CHANGE_DONE *);
    static void cmdRegisterVideoEncoderJPEGAck(void); //0x00010057
    static void cmdScreenOn(void);
    static void cmdScreenOff(void);
    static void cmdScreenUserPresent(void);

    STATE_CHANGE_RESULT updateScreenState(SCREEN_STATE nextState);

private:
    ScreenService();
    static ScreenService* pInstance;

    int screenServiceInit(void);
    int screenServiceStart(void);
    int screenServicePause(void);
    int screenServiceReset(void);
    int screenServiceFrameRateChange(u32 frameRate);

    static S_VIDEO_ENCODER_INIT initVideoParam;
    static SCREEN_STATE screenState;
};

#endif // SCREENSERVICE_H
