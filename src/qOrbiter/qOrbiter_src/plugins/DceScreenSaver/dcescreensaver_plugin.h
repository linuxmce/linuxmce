#ifndef DCESCREENSAVER_PLUGIN_H
#define DCESCREENSAVER_PLUGIN_H

#ifdef QT5
#include <QQmlExtensionPlugin>
#else
#include <QDeclarativeExtensionPlugin>
#endif

class DceScreenSaverPlugin :
        #ifdef QT5
        public QQmlExtensionPlugin
        #else
        public QDeclarativeExtensionPlugin
#endif
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "DceScreenSaver")
#endif
    
public:
    void registerTypes(const char *uri);
};

#endif // DCESCREENSAVER_PLUGIN_H

