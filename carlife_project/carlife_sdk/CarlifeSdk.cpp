#include "CCarLifeLibWrapper.h"
#include "CarlifeCommon.h"
#include "CarlifeSdk.h"

using namespace CCarLifeLibH;

CarlifeSDK* CarlifeSDK::pInstance = NULL;

CarlifeSDK::CCarLifeLib()
{
    //TODO: new object
}

CarlifeSDK::~CarlifeSDK()
{
    //TODO: delete object
}

CarlifeSDK *CarlifeSDK::getInstance()
{
    if(NULL == pInstance)
    {
        pInstance = new CarlifeSDK();
        debug("pInstance = new CarlifeSDK()");
    }

    return pInstance;
}

RESULT_TYPE CarlifeSDK::carlifeInit(S_CARLIFE_INFO *info, S_CARLIFE_DELEGATE *delegate)
{
    if(info != NULL)
    {
        //TODO: register carlife info
    }
    else
    {
        //Ignore, using default parameters
    }

    if((delegate->VideoAcquire_f == NULL) || (delegate->VideoRelease_f == NULL) ||
            delegate->Completion_f == NULL)
    {
        log_err("Carlife init Delegate failed");
        return RESULT_TYPE_FAILED;
    }
    else
    {
        //TODO: register modules callback
    }

    return RESULT_TYPE_SUCCESS;
}

RESULT_TYPE CarlifeSDK::carlifeStart(void)
{
    //TODO: updateSessionState

    return RESULT_TYPE_SUCCESS;
}

RESULT_TYPE CarlifeSDK::carlifeStop(void)
{
    //TODO: updateSessionState

    return RESULT_TYPE_SUCCESS;
}

RESULT_TYPE CarlifeSDK::carlifeDstory(void)
{
    //TODO: Unkown
    return RESULT_TYPE_SUCCESS;
}

RESULT_TYPE CarlifeSDK::carlifeTouch(int x, int y, TOUCH_TYPE action)
{
    S_TOUCH_SIGNAL_CLICK click;

    if(action == ACTION_DOWN)
    {
        //TODO:callback acquire carlife size
        click.x = x * 768 / 1600;
        click.y = y;

        if(CCarLifeLib::getInstance()->ctrlTouchSigleClick(&click) != 0)
        {
            log_err("Touch sigle Click failed");
            return RESULT_TYPE_FAILED;
        }
    }
    else if(action == ACTION_UP)
    {
        //TODO:
    }
    else if(action == ACTION_MOVE)
    {
        //TODO:
    }
    else
    {
        log_err("Touch sigle Input invalid");
        return RESULT_TYPE_FAILED;
    }

    return RESULT_TYPE_SUCCESS;
}

RESULT_TYPE CarlifeSDK::updateVideoFocus(SCREEN_FOCUS_TYPE type)
{
    //TODO:Unknow

    return RESULT_TYPE_SUCCESS;
}
