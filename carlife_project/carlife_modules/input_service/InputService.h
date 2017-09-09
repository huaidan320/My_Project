#ifndef INPUTSERVICE_H
#define INPUTSERVICE_H

#include "CCarLifeLibWrapper.h"
#include "CarlifeCommon.h"

using namespace CCarLifeLibH;

class InputService
{
public:
    ~InputService();

    static InputService *getInstance();

    //Input cmd channel callback
    static void cmdCarDataSubscribe(S_VEHICLE_INFO_LIST *);
    static void cmdCarDataSubscribeStart(S_VEHICLE_INFO_LIST *);
    static void cmdCarDataSubscribeStop(S_VEHICLE_INFO_LIST *);

    //TODO:updateInputState

private:
    InputService();
    static InputService* pInstance;
};

#endif // INPUTSERVICE_H
