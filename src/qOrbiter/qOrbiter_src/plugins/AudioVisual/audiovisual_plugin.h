#ifndef AUDIOVISUAL_PLUGIN_H
#define AUDIOVISUAL_PLUGIN_H

#include <QDeclarativeExtensionPlugin>

class AudioVisualPlugin : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
    
public:
    void registerTypes(const char *uri);
};

#endif // AUDIOVISUAL_PLUGIN_H

