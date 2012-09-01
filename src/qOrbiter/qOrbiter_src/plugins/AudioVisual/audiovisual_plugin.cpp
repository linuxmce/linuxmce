#include "audiovisual_plugin.h"
#include "videoplayer.h"
#include "audioplayer.h"
#include "mediamanager.h"


#include <qdeclarative.h>

void AudioVisualPlugin::registerTypes(const char *uri)
{

    // @uri AudioVisual
    qmlRegisterType<VideoPlayer>(uri, 1, 0, "VideoPlayer");

    qmlRegisterType<AudioPlayer>(uri, 1, 0, "AudioPlayer");

    qmlRegisterType<MediaManager>(uri, 1, 0, "MediaManager");
}

Q_EXPORT_PLUGIN2(AudioVisual, AudioVisualPlugin)


