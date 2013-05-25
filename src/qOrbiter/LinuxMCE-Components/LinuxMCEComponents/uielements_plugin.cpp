#include "uielements_plugin.h"
#include "button.h"
#include <screensaver.h>

#include <qdeclarative.h>

void UIElementsPlugin::registerTypes(const char *uri)
{
    // @uri Linuxmce.UIElements
    qmlRegisterType<QtButton>(uri, 1, 0, "QtButton");
    qmlRegisterType<ScreenSaver>(uri, 1, 0, "GlScreenSaver");
}

Q_EXPORT_PLUGIN2(UIElements, UIElementsPlugin)

