#ifndef CARLIFEMGR_H
#define CARLIFEMGR_H

#include <pthread.h>
#include "CCarLifeLibWrapper.h"

using namespace CCarLifeLibH;

class CarlifeMgr
{
public:
    ~CarlifeMgr();

    static CarlifeMgr* getInstance();

    int startCarlifeConnection(void);
    int cleanupCarlife(void);

private:
    CarlifeMgr();
    pthread_t channelThreadId[5];

    static CarlifeMgr* pInstance;
    //Receive thread
    static void *cmdReceiveThread(void *);
    static void *videoReceiveThread(void *);
    static void *mediaReceiveThread(void *);
    static void *ttsReceiveThread(void *);
    static void *vrReceiveThread(void *);

    static S_HU_PROTOCOL_VERSION huProtocolVersion;
};

#endif // CARLIFEMGR_H
