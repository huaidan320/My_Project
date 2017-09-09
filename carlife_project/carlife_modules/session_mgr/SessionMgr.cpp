#include "SessionMgr.h"

SessionMgr* SessionMgr::pInstance = NULL;

S_HU_INFO SessionMgr::huInfo = {
    "Linux",//os
    "board",//board
    "xxx",  //bootloader
    "xxx",  //brand
    "xxx",  //cpu_abi
    "xxx",  //cpu_abi2
    "xxx",  //device
    "xxx",  //dispaly
    "xxx",  //fingerprint
    "xxx",  //hardware
    "xxx",  //host
    "xxx",  //cid
    "xxx",  //manufacturer
    "xxx",  //model
    "xxx",  //product
    "xxx",  //serial
    "xxx",  //codename
    "xxx",  //incremental
    "xxx",  //release
    "xxx",  //sdk
    2,      //sdk_int
    "",     //token
    ""
};

S_STATISTICS_INFO SessionMgr::statisticsInfo = {
    "carlife", //cuid
    "RADIO",   //versionName
    1,         //versionCode
    "20012100",//channel ID
    3,         //connectCount
    3,         //connectSuccessCount
    0,         //ConnectTime
    "crash"    //carshLog
};

SESSION_STATE SessionMgr::sessionState = SESSION_STATE_NULL;

SessionMgr::SessionMgr()
{
    //TODO: new object
}

SessionMgr::~SessionMgr()
{
    //TODO: delete object
}

SessionMgr *SessionMgr::getInstance()
{
    if(NULL == pInstance)
    {
        pInstance = new SessionMgr();
        debug("pInstance = new SessionMgr()");
    }

    return pInstance;
}

//cmd channel callback
void SessionMgr::cmdProtocolVersionMatchStatus(S_PROTOCOL_VERSION_MATCH_SATUS *status)
{
    debug("cmdProtocolVersionMatchStatus() is invoked");
    debug("\tprotocol version match status: %d", status->matchStatus);

    if(-1 == CCarLifeLib::getInstance()->cmdStatisticInfo(&statisticsInfo))
    {
        log_err("cmdStatisticInfo send fail");
        return;
    }
    else
    {
        debug("cmdStatisticInfo send success");
    }

    if(-1 == CCarLifeLib::getInstance()->cmdHUInfro(&huInfo))
    {
        log_err("cmdHUInfo send fail");
        return;
    }
    else
    {
        debug("cmdHUInfo send success");
    }

    if(updateSessionState(SESSION_STATE_CONNECTED) != STATE_CHANGE_SUCCESS)
    {
        log_err("updateSessionState failed");
        return;
    }
}

void SessionMgr::cmdMDInfro(S_MD_INFO *)
{
    debug("cmdMDInfro() is invoked");
}

void SessionMgr::cmdForeground()
{
    debug("cmdForeground() is invoked");
}

void SessionMgr::cmdBackground()
{
    debug("cmdBackground() is invoked");
}

void SessionMgr::cmdGoToDeskTop()
{
    debug("cmdGoToDeskTop() is invoked");
}

void SessionMgr::cmdModuleStatus(S_MODULE_STATUS_LIST_MOBILE *)
{
    debug("cmdModuleStatus is invoked");
}

void SessionMgr::cmdRegisterConnectException(S_CONNECTION_EXCEPTION *)
{
    debug("cmdRegisterConnectException is invoked");
}

void SessionMgr::cmdRegisterRequestGoToForeground()
{
    debug("cmdRegisterRequestGoToForeground is invoked");
}

void SessionMgr::cmdRegisterUIActionSound()
{
    debug("cmdRegisterUIActionSound is invoked");
}

void SessionMgr::cmdRegisterMdAuthenResponse(S_AUTHEN_RESPONSE *response)
{
    debug("cmdRegisterMdAuthenResponse is invoked");
    debug("response->encryptValue = %d", response->encryptValue);
}

void SessionMgr::cmdRegisterFeatureConfigRequest()
{
    debug("cmdRegisterFeatureConfigRequest is invoked");
}

void SessionMgr::cmdRegisterMdExit()
{
    debug("cmdRegisterMdExit is invoked");
}

STATE_CHANGE_RESULT SessionMgr::updateSessionState(SESSION_STATE nextState)
{
    STATE_CHANGE_RESULT stateRet = STATE_CHANGE_SUCCESS;

    switch(nextState)
    {
    case SESSION_STATE_NULL:
        //Update the state variable so that subsequent read are all updated.
        //Since you are processing a state transition to NULL
        sessionState = nextState;

        //TODO Reset all variables

        //Cleanup sockets and WDT
        //cleanupCarlife();
        break;

    case SESSION_STATE_CONNECTING:
        if(sessionState == SESSION_STATE_NULL)
        {
            sessionState = nextState;
            //TODO Start timer for checking connection timeout
            //TODO start adapter.
            //TODO adapter aoa connect success

            //if(0 != startCarlifeConnection())
            //{
            //    log_err("Carlife Connection start failed");
            //}
            //TODO Start timer for checking md_adapter status
        }
        else
        {
            stateRet = STATE_CHANGE_FAILURE;
        }
        break;

    case SESSION_STATE_CONNECTED:
        if(sessionState == SESSION_STATE_CONNECTING)
        {
            sessionState = nextState;

            //TODO call start connected callback();

            //if(update_screen_state(SCREEN_STATE_INIT) != STATE_CHANGE_SUCCESS)
            //{
                //TODO stop carlife
            //    log_err("update_screen_state failed");
            //}

            //TODO Start timer for sending video heartbeat to MD

            //TODO stop Start timer
        }
        else
        {
            stateRet = STATE_CHANGE_FAILURE;
        }

        break;
    default:
        stateRet = STATE_CHANGE_INVALID;
        break;
    }

    //TODO UNLOCK();
    return stateRet;
}
