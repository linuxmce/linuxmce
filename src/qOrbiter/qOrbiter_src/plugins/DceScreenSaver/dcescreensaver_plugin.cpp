#include "dcescreensaver_plugin.h"
#include "dcescreensaver.h"

#ifdef QT5

#else
#include <qdeclarative.h>
#endif

void DceScreenSaverPlugin::registerTypes(const char *uri)
{
    // @uri DceScreenSaver
    qmlRegisterType<DceScreenSaver>(uri, 1, 0, "DceScreenSaver");
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(DceScreenSaver, DceScreenSaverPlugin)
#endif

