#ifndef AUDIOVISUAL_PLUGIN_H
#define AUDIOVISUAL_PLUGIN_H

#ifdef QT5
#include <QtQml/QQmlExtensionPlugin>
#else
#include <QtDeclarative/QDeclarativeExtensionPlugin>
#endif

#ifdef QT5
class AudioVisualPlugin : public QQmlExtensionPlugin
#else
class AudioVisualPlugin : public QDeclarativeExtensionPlugin
 #endif
{
    Q_OBJECT
#ifdef QT5
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionPlugin")
#endif

public:
    void registerTypes(const char *uri);
};

#endif // AUDIOVISUAL_PLUGIN_H

