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
#ifdef QT5
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionPlugin")
#endif

public:
    void registerTypes(const char *uri);
};

#endif // DCESCREENSAVER_PLUGIN_H

