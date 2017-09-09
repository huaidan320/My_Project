#ifndef VRSERVICE_H
#define VRSERVICE_H

#include "CCarLifeLibWrapper.h"
#include "CarlifeCommon.h"

using namespace CCarLifeLibH;

class VrService
{
public:
    ~VrService();

    static VrService *getInstance();

    /* VR cmd channel callback */
    static void cmdMicRecordWakeupStart(void);
    static void cmdMicRecordEnd(void);
    static void cmdMicRecordRecogStart(void);

    /* VR channel callback */
    static void vrInit(S_AUDIO_INIT_PARAMETER *);
    static void vrNormalData(u8 *, u32);
    static void vrStop(void);

    //TODO:updateVrState

private:
    VrService();
    static VrService* pInstance;
};

#endif // VRSERVICE_H
