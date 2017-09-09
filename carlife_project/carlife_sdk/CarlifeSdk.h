#ifndef CARLIFESDK_H
#define CARLIFESDK_H

typedef enum {
    RESULT_TYPE_SUCCESS,
    RESULT_TYPE_FAILED
}RESULT_TYPE;

typedef enum {
    ACTION_DOWN = 0x00,
    ACTION_UP = 0x01,
    ACTION_MOVE = 0x02
}TOUCH_TYPE;

typedef enum {
    SCREEN_FOCUS_NULL,
    SCREEN_FOCUS_FULL
}SCREEN_FOCUS_TYPE;

typedef struct {
    unsigned char *manufacturer;
    unsigned char *model;
    unsigned char *product;
    unsigned char *serial;
    unsigned char *cuid;
    unsigned char *versionName;
    unsigned char *channelID;
    unsigned int height;
    unsigned int width;
    unsigned int rate;
    bool focusUi;
    bool btInternalUi;
    bool btAutopair;
    bool voiceWakeup;
    bool voiceMic;
}S_CARLIFE_INFO;

typedef void (*CompletionCB)(RESULT_TYPE);
typedef void (*ScreenAcquireCB)(void);
typedef void (*ScreenReleaseCB)(void);

typedef struct {
    CompletionCB Completion_f;
    ScreenAcquireCB VideoAcquire_f;
    ScreenReleaseCB VideoRelease_f;
}S_CARLIFE_DELEGATE;

class CarlifeSDK
{
public:
    ~CarlifeSDK();

    static CarlifeSDK* getInstance();

    RESULT_TYPE carlifeInit(S_CARLIFE_INFO *info, S_CARLIFE_DELEGATE *delegate);
    RESULT_TYPE carlifeStart(void);
    RESULT_TYPE carlifeStop(void);

    //Unknow
    RESULT_TYPE carlifeDstory(void);

    RESULT_TYPE carlifeTouch(int x, int y, TOUCH_TYPE action);
    RESULT_TYPE updateVideoFocus(SCREEN_FOCUS_TYPE type);

private:
    CarlifeSDK();
    static CarlifeSDK* pInstance;
};

#endif // CARLIFESDK_H
