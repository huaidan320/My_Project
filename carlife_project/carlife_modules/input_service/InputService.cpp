#include "InputService.h"

InputService* InputService::pInstance = NULL;

InputService::InputService()
{
    //TODO: new object
}

InputService::~InputService()
{
    //TODO: delete object
}

InputService *InputService::getInstance()
{
    if(NULL == pInstance)
    {
        pInstance = new InputService();
        debug("pInstance = new InputService()");
    }

    return pInstance;
}

void InputService::cmdCarDataSubscribe(S_VEHICLE_INFO_LIST *)
{
    debug("cmdCarDataSubscribe is invoked");
}

void InputService::cmdCarDataSubscribeStart(S_VEHICLE_INFO_LIST *)
{
    debug("cmdCarDataSubscribeStart is invoked");
}

void InputService::cmdCarDataSubscribeStop(S_VEHICLE_INFO_LIST *)
{
    debug("cmdCarDataSubscribeStop is invoked");
}
