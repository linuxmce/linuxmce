#include "audiovisual_plugin.h"
#include "mediamanager.h"

#ifndef QT5
#include <qdeclarative.h>
#endif

void AudioVisualPlugin::registerTypes(const char *uri)
{
    // @uri AudioVisual
    qmlRegisterType<MediaManager>(uri, 1, 0, "MediaManager");
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(AudioVisual, AudioVisualPlugin)
#endif

