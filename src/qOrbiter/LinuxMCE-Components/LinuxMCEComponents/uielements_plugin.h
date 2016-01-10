#ifndef UIELEMENTS_PLUGIN_H
#define UIELEMENTS_PLUGIN_H

#include <QDeclarativeExtensionPlugin>

class UIElementsPlugin : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
    
public:
    void registerTypes(const char *uri);
};

#endif // UIELEMENTS_PLUGIN_H

