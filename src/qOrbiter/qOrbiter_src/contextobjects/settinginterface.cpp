#include "settinginterface.h"
#include "qdebug.h"
#include "qdatetime.h"
#include "defineObjects/mediatypehelper.h"
#include "qdebug.h"

SettingInterface::SettingInterface(QObject *parent) :
    QObject(parent),
    m_settings(0),
    ready(false)
{
    /** Note that application name, org, and domain are set via QCoreApplication in main.cpp */
    m_settings = new QSettings(this);
    m_settings->setFallbacksEnabled(false);
    m_lookup.insert(Setting_Network_Router, "router");
    m_lookup.insert(Setting_Network_Device_ID, "device");
    m_lookup.insert(Setting_Network_Hostname, "hostname");
    m_lookup.insert(Setting_Network_ExternalHostname, "externalhostname");
    m_lookup.insert(Setting_Network_WebPort, "webaccess");
    m_lookup.insert(Setting_Ui_Skin, "skin");
    m_lookup.insert(Setting_Ui_NetworkLoading, "usenetwork");
    m_lookup.insert(Setting_Ui_PrefSize, "preferredsize");
    m_lookup.insert(Setting_Text_sizemod, "sizemodifier");
    m_lookup.insert(Setting_Text_font, "font");
    m_lookup.insert(Setting_Text_language, "language");
    m_lookup.insert(Setting_Media_AudioSort, "audiosorting");
    m_lookup.insert(Setting_Media_AudioSubTypeSort, "audiosubtypesort");
    m_lookup.insert(Setting_Media_VideoSort, "videosorting");
    m_lookup.insert(Setting_Media_VideSubTypeSort, "videosubtypesort");
    connect(this, SIGNAL(writeError(QString)), this, SLOT(log(QString)));
    if(m_settings){
        connect(this, SIGNAL(settingsDataCleared()), this,SLOT(initializeSettings()));

        initializeSettings();
    }
}


void SettingInterface::initializeSettings()
{
    if(!m_settings->childGroups().contains("network")){
        log(tr("Initializing network Settings"));
        m_settings->beginGroup("network");
        m_settings->setValue("router","192.168.80.1");
        m_settings->setValue("hostname", "dcerouter.linuxmce");
        m_settings->setValue("externalhostname", "");
        m_settings->setValue("webaccess", "80");
        m_settings->setValue("device", -1);
        m_settings->endGroup();
        log(tr("Finished Initializing Network Settings"));
    }

    if(!m_settings->childGroups().contains("ui")){
        log(tr("Initializing UI Settings"));
        m_settings->beginGroup("ui");
        m_settings->setValue("skin","default");
        m_settings->setValue("usenetwork", false);
        m_settings->setValue("preferredsize", "small");
        m_settings->endGroup();
        log(tr("Finished Initializing UI Settings"));
    }

    if(!m_settings->childGroups().contains("textoptions")){
        log(tr("Initializing text settings"));
        m_settings->beginGroup("textoptions");
        m_settings->setValue("sizemodifier", 0);
        m_settings->setValue("font", "");
        m_settings->setValue("language", QLocale::system().name());
        m_settings->endGroup();
        log(tr("Finished Initializing text settings"));
    }

    if(!m_settings->childGroups().contains("media")){
        log(tr("Initializing Media Settings"));
        m_settings->beginGroup("media");
        m_settings->setValue("audiosorting",AttributeTypeHelper::Performer  );
        m_settings->setValue("audiosubtypesort", -1);
        m_settings->setValue("videosorting", AttributeTypeHelper::Title);
        m_settings->setValue("videosubtypesort", SubTypesHelper::MOVIES );
        log(tr("Finished Initializing Media settings"));
    }

    log(tr("Settings are ready"));
    ready=true;
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
    } else if(grp.contains("network") || grp.contains("media") || grp.contains("textoptions")){
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

    m_settings->beginGroup("textoptions");
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

void SettingInterface::setUiOption(QString key, QVariant opt)
{
    if(!m_settings){
        emit writeError(tr("No settings object to write to!"));
        return;
    }

    m_settings->beginGroup("ui");
    if(!m_settings->contains(key)){
        emit writeError(tr("Invalid settings option"));
    } else {
        m_settings->setValue(key, opt);
    }
    m_settings->endGroup();
}

void SettingInterface::setOption(SettingInterface::SettingsType st, SettingInterface::SettingKey sk, QVariant sval)
{
    QString grp="";
    QString key = m_lookup.value(sk);
    switch (st) {
    case Settings_Network:grp="network"; break;
    case Settings_Media:grp="media"; break;
    case Settings_Text: grp="textoptions"; break;
    case Settings_UI: grp="ui"; break;
    default: grp="";
        break;
    }
    if(grp=="")
        return;

     log(QString("Setting option for GROUP: -=%1=- KEY: -=%2=- VALUE: %3").arg(grp).arg(m_lookup.value(sk)).arg(sval.toString()));

    if(!m_settings){
        emit writeError(tr("No settings object to write to!"));
        return;
    }

    m_settings->beginGroup(grp);
    if(!m_settings->contains(key)){
        emit writeError(tr("Invalid settings option"));
        log(QString("Options for this key are %1").arg(m_settings->childKeys().join("\n")));
    } else {
        m_settings->setValue(key, sval);
    }
    m_settings->endGroup();

}

QVariant SettingInterface::getOption(SettingInterface::SettingsType st, SettingInterface::SettingKey sk)
{
    QString grp="";
    QString key = m_lookup.value(sk);
    QVariant rtrn= QVariant();
    switch (st) {
    case Settings_Network:grp="network"; break;
    case Settings_Media:grp="media"; break;
    case Settings_Text: grp="textoptions"; break;
    case Settings_UI: grp="ui"; break;
    default: grp="";
        break;
    }
    log(QString("Getting option for group -=%1=-").arg(grp));

    if(grp=="")
        return rtrn;

    if(!m_settings){
        emit writeError(tr("No settings object to read!"));
        return rtrn;
    }


    m_settings->beginGroup(grp);

    if(!m_settings->contains(key)){
        emit writeError(tr("No  settings option for -=%1=-").arg(key));
        log(QString("Options for this key are %1").arg(m_settings->childKeys().join("\n")));
        m_settings->endGroup();

    } else {
        qDebug() << Q_FUNC_INFO << "Returning settings value for key" << key;
        rtrn=m_settings->value(key);
    }
     m_settings->endGroup();
     return rtrn;

}


QVariant SettingInterface::getDefaultOption(QString grp, QString opt)
{
    if(!m_settings){
        emit writeError(tr("No settings object to read from!"));
        return QVariant(tr("Nada"));
    }

    m_settings->beginGroup(grp);
    if(m_settings->contains(opt)){
        return m_settings->value(opt);
    }
}

QVariant SettingInterface::getCustomOption(QString grp, QString opt)
{
    if(!m_settings){
        emit writeError(tr("No settings object to read from!"));
        return QVariant(tr("Nada"));
    }
    if(!m_settings->childGroups().contains(grp)){
        emit writeError(tr("Custom Setting doesnt exist!"));
        return QVariant(tr("Nada"));
    }
    m_settings->beginGroup(grp);
    if(m_settings->contains(opt)){
        return m_settings->value(opt);
    }
}
