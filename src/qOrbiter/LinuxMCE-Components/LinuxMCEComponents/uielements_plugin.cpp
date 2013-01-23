#include "uielements_plugin.h"
#include "button.h"

#include <qdeclarative.h>

void UIElementsPlugin::registerTypes(const char *uri)
{
    // @uri Linuxmce.UIElements
    qmlRegisterType<Button>(uri, 1, 0, "Button");
}

Q_EXPORT_PLUGIN2(UIElements, UIElementsPlugin)

