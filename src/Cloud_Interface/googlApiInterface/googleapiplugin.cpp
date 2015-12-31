#include "googleapiplugin.h"
#include "qdebug.h"
GoogleApiPlugin::GoogleApiPlugin()
{
settingsManager = new CloudSettings();
settingsManager->setIdentity(name(), deviceTemplate());
}

void GoogleApiPlugin::setUserList(QList<QObject *>l)
{
Q_UNUSED(l);
}

