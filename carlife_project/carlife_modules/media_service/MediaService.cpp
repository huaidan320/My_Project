#include "MediaService.h"

MediaService* MediaService::pInstance = NULL;

MediaService::MediaService()
{
    //TODO: new object
}

MediaService::~MediaService()
{
    //TODO: delete object
}

MediaService *MediaService::getInstance()
{
    if(NULL == pInstance)
    {
        pInstance = new MediaService();
        debug("pInstance = new MediaService()");
    }

    return pInstance;
}

//Media cmd channel callback
void MediaService::cmdMediaInfo(S_MEDIA_INFO *info)
{
    debug("cmdMediaInfo is invoked");
    debug("source: %s", info->source.data());
    debug("song: %s", info->song.data());
    debug("artist: %s", info->artist.data());
    debug("album: %s", info->album.data());
    debug("duration: %d", info->duration);
    debug("playlistNum: %d", info->playlistNum);
    debug("songId: %s", info->songId.data());
    debug("mode: %d", info->mode);
}

void MediaService::cmdMediaProgressBar(S_MEDIA_PROGRESS_BAR *)
{
    debug("cmdMediaProgressBar is invoked");
}

//Media channel callback
void MediaService::mediaInit(S_AUDIO_INIT_PARAMETER *initParam)
{
    debug("mediaInit() is invoked");
    debug("sampleRate: %d", initParam->sampleRate);
    debug("channelConfig: %d", initParam->channelConfig);
    debug("sampleFormat: %d", initParam->sampleFormat);
}

void MediaService::mediaNormalData(u8 *, u32)
{
    debug("mediaNormalData() is invoked");
}

void MediaService::mediaStop()
{
    debug("mediaStop() is invoked");
}

void MediaService::mediaPause()
{
    debug("mediaPause() is invoked");
}

void MediaService::mediaResume()
{
    debug("mediaResume() is invoked");
}

void MediaService::mediaSeek()
{
    debug("mediaSeek() is invoked");
}
