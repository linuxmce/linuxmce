#ifndef GOOGLEWEATHERPLUGIN_PLUGIN_H
#define GOOGLEWEATHERPLUGIN_PLUGIN_H

#include <QDeclarativeExtensionPlugin>

class GoogleWeatherPluginPlugin : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
    
public:
    void registerTypes(const char *uri);
};

#endif // GOOGLEWEATHERPLUGIN_PLUGIN_H

