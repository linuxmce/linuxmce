#include "cloudsettings.h"
#include "QCoreApplication"
#include "qdebug.h"
CloudSettings::CloudSettings(QObject *parent) : QObject(parent)

{

}

void CloudSettings::setIdentity(QString appName, int deviceTemplate)
{
    m_appName=appName;
    m_deviceTemplate=deviceTemplate;

    QCoreApplication::setOrganizationName("LinuxMCE");
    QCoreApplication::setOrganizationDomain("www.linuxmce.org");
    QCoreApplication::setApplicationName(m_appName);

    emit settingsReady();
    qDebug() << QCoreApplication::organizationName();
    qDebug() << QCoreApplication::organizationDomain();
    qDebug() << QCoreApplication::applicationName();
}

