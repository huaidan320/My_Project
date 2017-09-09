#include "VrService.h"

VrService* VrService::pInstance = NULL;

VrService::VrService()
{
    //TODO: new object
}

VrService::~VrService()
{
    //TODO: delete object
}

VrService *VrService::getInstance()
{
    if(NULL == pInstance)
    {
        pInstance = new VrService();
        debug("pInstance = new VrService()");
    }

    return pInstance;
}

/* VR cmd channel callback */
void VrService::cmdMicRecordWakeupStart()
{
    debug("cmdMicRecordWakeupStart() is invoked");
}

void VrService::cmdMicRecordEnd()
{
    debug("cmdMicRecordEnd() is invoked");
}

void VrService::cmdMicRecordRecogStart()
{
    debug("cmdMicRecordRecogStart() is invoked");
}

/* VR channel callback */
void VrService::vrInit(S_AUDIO_INIT_PARAMETER *initParam)
{
    debug("vrInit() is invoked");
    debug("sampleRate: %d", initParam->sampleRate);
    debug("channelConfig: %d", initParam->channelConfig);
    debug("sampleFormat: %d", initParam->sampleFormat);
}

void VrService::vrNormalData(u8 *, u32)
{
    debug("vrNormalData() is invoked");
}

void VrService::vrStop()
{
    debug("vrStop() is invoked");
}
