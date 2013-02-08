#include "audiovisual_plugin.h"
#include "mediamanager.h"


#include <qdeclarative.h>

void AudioVisualPlugin::registerTypes(const char *uri)
{

#ifdef QT5

#else
    // @uri AudioVisual
    qmlRegisterType<MediaManager>(uri, 1, 0, "MediaManager");
#endif

}

Q_EXPORT_PLUGIN2(AudioVisual, AudioVisualPlugin)


