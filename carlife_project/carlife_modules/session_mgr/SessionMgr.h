#ifndef SESSIONMGR_H
#define SESSIONMGR_H

#include "CCarLifeLibWrapper.h"
#include "CarlifeCommon.h"

using namespace CCarLifeLibH;

typedef enum
{
    SESSION_STATE_NULL,
    SESSION_STATE_CONNECTING,
    SESSION_STATE_CONNECTED
}SESSION_STATE;

class SessionMgr
{
public:
    ~SessionMgr();

    static SessionMgr *getInstance();

    //cmd channel callback
    static void cmdProtocolVersionMatchStatus(S_PROTOCOL_VERSION_MATCH_SATUS *);
    static void cmdMDInfro(S_MD_INFO *);
    static void cmdForeground(void);
    static void cmdBackground(void);
    static void cmdGoToDeskTop(void);
    static void cmdModuleStatus(S_MODULE_STATUS_LIST_MOBILE *);
    static void cmdRegisterConnectException(S_CONNECTION_EXCEPTION *);
    static void cmdRegisterRequestGoToForeground(void);
    static void cmdRegisterUIActionSound(void);
    static void cmdRegisterMdAuthenResponse(S_AUTHEN_RESPONSE *);
    static void cmdRegisterFeatureConfigRequest(void);
    static void cmdRegisterMdExit(void);

    STATE_CHANGE_RESULT updateSessionState(SESSION_STATE nextState);

private:
    SessionMgr();
    static SessionMgr* pInstance;
    static S_HU_INFO huInfo;
    static S_STATISTICS_INFO statisticsInfo;
    static SESSION_STATE sessionState;
};

#endif // SESSIONMGR_H
