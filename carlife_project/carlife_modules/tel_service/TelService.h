#ifndef TELSERVICE_H
#define TELSERVICE_H

#include "CCarLifeLibWrapper.h"
#include "CarlifeCommon.h"

using namespace CCarLifeLibH;

class TelService
{
public:
    ~TelService();

    static TelService *getInstance();

    /* TEL cmd channel callback */
    static void cmdMDBTPairInfro(BTPairInfo *);
    static void cmdTelStateChangeIncoming(void);
    static void cmdTelStateChangeOutGoing(void);
    static void cmdTelStateChangeIdle(void);
    static void cmdTelStateChangeInCalling(void);

    //TODO:updateTelState

private:
    TelService();
    static TelService* pInstance;
};
#endif // TELSERVICE_H
