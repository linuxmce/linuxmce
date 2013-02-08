#include "googleweatherplugin_plugin.h"
#include "googleweather.h"

#include <qdeclarative.h>

void GoogleWeatherPluginPlugin::registerTypes(const char *uri)
{
    // @uri GoogleWeatherPlugin
    qmlRegisterType<GoogleWeather>(uri, 1, 0, "GoogleWeather");
}

Q_EXPORT_PLUGIN2(GoogleWeatherPlugin, GoogleWeatherPluginPlugin)

