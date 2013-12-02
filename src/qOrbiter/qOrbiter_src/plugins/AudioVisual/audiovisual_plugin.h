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
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "AudioVisual")
#endif
public:
    void registerTypes(const char *uri);
};

#endif // AUDIOVISUAL_PLUGIN_H

