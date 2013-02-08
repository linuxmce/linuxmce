#include "uielements_plugin.h"
#include "button.h"

#include <qdeclarative.h>

void UIElementsPlugin::registerTypes(const char *uri)
{
    // @uri Linuxmce.UIElements
    qmlRegisterType<QtButton>(uri, 1, 0, "QtButton");
}

Q_EXPORT_PLUGIN2(UIElements, UIElementsPlugin)

