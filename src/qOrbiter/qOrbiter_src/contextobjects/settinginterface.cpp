#include "settinginterface.h"
#include "qdebug.h"
#include "qdatetime.h"
#include "defineObjects/mediatypehelper.h"

SettingInterface::SettingInterface(QObject *parent) :
    QObject(parent),
    m_settings(0)
{
    /** Note that application name, org, and domain are set via QCoreApplication in main.cpp */
    m_settings = new QSettings(this);

    if(m_settings){
        connect(this, SIGNAL(settingsDataCleared()), this,SLOT(initializeSettings()));
        initializeSettings();
    }

}


void SettingInterface::initializeSettings()
{
    if(!m_settings->contains("network")){
        log(tr("Initializing newtwork Settings"));
        m_settings->beginGroup("network");
        m_settings->setValue("router","192.168.80.1");
        m_settings->setValue("hostname", "dcerouter.linuxmce");
        m_settings->setValue("externalhostname", "");
        m_settings->setValue("webaccess", "80");
        m_settings->endGroup();
        log(tr("Network Settings Written"));
    }

    if(!m_settings->contains("text")){
        log(tr("Initializing text settings"));
        m_settings->beginGroup("textoptions");
        m_settings->setValue("sizemodifier", 0);
        m_settings->setValue("font", "");
        m_settings->endGroup();
    }

    if(!m_settings->contains("media")){
        log(tr("Initializing Media Settings"));
        m_settings->beginGroup("media");
        m_settings->setValue("audiosorting",AttributeTypeHelper::Performer  );
        m_settings->setValue("audiosubtypesort", -1);
        m_settings->setValue("videosorting", AttributeTypeHelper::Title);
        m_settings->setValue("videosubtypesort", SubTypesHelper::MOVIES );
        log(tr("Finished Initializing Media settings"));
    }

    log(tr("Settings are ready"));

}

void SettingInterface::destroySettingsData()
{
    m_settings->clear();

}

void SettingInterface::log(QString message)
{
    QString logMsg;
    logMsg=QDateTime::currentDateTime().toLocalTime().toString()+"::"+message;
    emit newLogMessage(logMsg);
    qDebug() << logMsg;
}

void SettingInterface::setOption(QString grp, QString key, QVariant opt)
{
    if(!m_settings){
        emit writeError(tr("No settings object to write to!"));
        return;
    } else if(grp =="network" || grp=="media" || grp == "text"){
        emit writeError(tr("Not Allowed to change default settings through this interface."));
        return;
    }

   m_settings->beginGroup(grp);
   m_settings->setValue(key, opt);
   m_settings->endGroup();

}

void SettingInterface::setNetworkOption(QString key, QVariant opt)
{
    if(!m_settings){
        emit writeError(tr("No settings object to write to!"));
        return;
    }

    m_settings->beginGroup("network");
    if(!m_settings->contains(key)){
        emit writeError(tr("Invalid settings option"));
    } else {
        m_settings->setValue(key, opt);
    }
    m_settings->endGroup();

}

void SettingInterface::setTextOption(QString key, QVariant opt)
{
    if(!m_settings){
        emit writeError(tr("No settings object to write to!"));
        return;
    }

    m_settings->beginGroup("text");
    if(!m_settings->contains(key)){
        emit writeError(tr("Invalid settings option"));
    } else {
        m_settings->setValue(key, opt);
    }
    m_settings->endGroup();
}

void SettingInterface::setMediaOption(QString key, QVariant opt)
{
    if(!m_settings){
        emit writeError(tr("No settings object to write to!"));
        return;
    }

    m_settings->beginGroup("media");
    if(!m_settings->contains(key)){
        emit writeError(tr("Invalid settings option"));
    } else {
        m_settings->setValue(key, opt);
    }
    m_settings->endGroup();
}
