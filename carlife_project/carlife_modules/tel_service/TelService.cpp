#include "TelService.h"

TelService* TelService::pInstance = NULL;

TelService::TelService()
{
    //TODO: new object
}

TelService::~TelService()
{
    //TODO: delete object
}

TelService *TelService::getInstance()
{
    if(NULL == pInstance)
    {
        pInstance = new TelService();
        debug("pInstance = new TelService()");
    }

    return pInstance;
}

/* TEL cmd channel callback */
void TelService::cmdMDBTPairInfro(BTPairInfo *info)
{
    debug("cmdMDBTPairInfro() is invoked");
    debug("addrss: %s", info->address.data());
    debug("passKey: %s", info->passKey.data());
    debug("hash: %s", info->hash.data());
    debug("randomizer: %s", info->randomizer.data());
    debug("uuid: %s", info->uuid.data());
    debug("name: %s", info->name.data());
    debug("status: %d", info->status);
}

void TelService::cmdTelStateChangeIncoming()
{
    debug("cmdTelStateChangeIncoming() is invoked");
}

void TelService::cmdTelStateChangeOutGoing()
{
    debug("cmdTelStateChangeOutGoing() is invoked");
}

void TelService::cmdTelStateChangeIdle()
{
    debug("cmdTelStateChangeIdle() is invoked");
}

void TelService::cmdTelStateChangeInCalling()
{
    debug("cmdTelStateChangeInCalling() is invoked");
}
