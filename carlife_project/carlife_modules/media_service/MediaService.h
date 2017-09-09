#ifndef MEDIASERVICE_H
#define MEDIASERVICE_H

#include "CCarLifeLibWrapper.h"
#include "CarlifeCommon.h"

using namespace CCarLifeLibH;

class MediaService
{
public:
    ~MediaService();

    static MediaService *getInstance();

    //Media cmd channel callback
    static void cmdMediaInfo(S_MEDIA_INFO *); //0x00010035
    static void cmdMediaProgressBar(S_MEDIA_PROGRESS_BAR *); //0x00010036

    //Media channel callback
    static void mediaInit(S_AUDIO_INIT_PARAMETER *);
    static void mediaNormalData(u8 *, u32);
    static void mediaStop(void);
    static void mediaPause(void);
    static void mediaResume(void);
    static void mediaSeek(void);

    //TODO:updateMediaState

private:
    MediaService();
    static MediaService* pInstance;
};
#endif // MEDIASERVICE_H
