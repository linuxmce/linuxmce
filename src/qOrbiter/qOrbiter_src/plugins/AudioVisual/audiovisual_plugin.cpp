#include "audiovisual_plugin.h"
#include "../../../../qMediaPlayer/AudioVisual/audiovisual.h"

#include <qdeclarative.h>

void AudioVisualPlugin::registerTypes(const char *uri)
{
    // @uri AudioVisual
    qmlRegisterType<AudioVisual>(uri, 1, 0, "AudioVisual");
}

Q_EXPORT_PLUGIN2(AudioVisual, AudioVisualPlugin)

