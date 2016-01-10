#include "settinginterface.h"
#include "qdebug.h"
#include "qdatetime.h"
#include "defineObjects/mediatypehelper.h"
#include "qdebug.h"
#include "qhostinfo.h"
#include "QSysInfo"

SettingInterface::SettingInterface(QObject *parent) :
    QObject(parent),
    m_settings(0),
    m_persistentName(""),
    m_fontSizeMod(0),
    ready(false)
{
    /** Note that application name, org, and domain are set via QCoreApplication in main.cpp */

    m_settings.setFallbacksEnabled(false);
    m_lookup.insert(SettingsKeyType::Setting_Network_DeviceName, "deviceName");
    m_lookup.insert(SettingsKeyType::Setting_Network_Router, "router");
    m_lookup.insert(SettingsKeyType::Setting_Network_Device_ID, "device");
    m_lookup.insert(SettingsKeyType::Setting_Network_Hostname, "hostname");
    m_lookup.insert(SettingsKeyType::Setting_Network_ExternalHostname, "externalhostname");
    m_lookup.insert(SettingsKeyType::Setting_Network_WebPort, "webaccess");
    m_lookup.insert(SettingsKeyType::Setting_Ui_Skin, "skin");
    m_lookup.insert(SettingsKeyType::Setting_Ui_NetworkLoading, "usenetwork");
    m_lookup.insert(SettingsKeyType::Setting_Ui_PrefSize, "preferredsize");
    m_lookup.insert(SettingsKeyType::Setting_Text_sizemod, "sizemodifier");
    m_lookup.insert(SettingsKeyType::Setting_Text_font, "font");
    m_lookup.insert(SettingsKeyType::Setting_Text_language, "language");
    m_lookup.insert(SettingsKeyType::Setting_Media_AudioSort, "audiosorting");
    m_lookup.insert(SettingsKeyType::Setting_Media_AudioSubTypeSort, "audiosubtypesort");
    m_lookup.insert(SettingsKeyType::Setting_Media_VideoSort, "videosorting");
    m_lookup.insert(SettingsKeyType::Setting_Media_VideSubTypeSort, "videosubtypesort");
    connect(this, SIGNAL(writeError(QString)), this, SLOT(log(QString)));

    if(!m_settings.status()==QSettings::AccessError){
        connect(this, SIGNAL(settingsDataCleared()), this,SLOT(initializeSettings()));
        initializeSettings();
    }  else {
        log(tr("Could not access settings in constructor"));
    }


}

void SettingInterface::setSimpleOption(SettingsKeyType::SettingKey sk, QVariant sval)
{

    switch (sk) {
    case SettingsKeyType::Setting_Network_Router:
        setOption(SettingsInterfaceType::Settings_Network, sk, sval);
        break;
    case SettingsKeyType::Setting_Text_sizemod:
        setOption(SettingsInterfaceType::Settings_Text, sk, sval);
        break;
    default:
        break;
    }


}
bool SettingInterface::getReady() const
{
    return ready;
}

void SettingInterface::setReady(bool value)
{
    if(ready ==value )return;
    ready = value;
    emit readyChanged();
}



void SettingInterface::initializeSettings()
{
    //device name is tricky because it used as an Entertain area identifier. We want to ensure its always read correctly so we do our best to maintain it.
    //the problem case is when the user clears settings. the device name should remain the same but there is nowhere to place it in a simple manner, so we do it behind the scenes.
    setReady(false);

    if(!m_settings.childGroups().contains("network")){
        log(tr("Initializing network Settings"));
        m_settings.beginGroup("network");
        m_settings.setValue("deviceName", m_persistentName.isEmpty() ? "QOrbiter Device" : m_persistentName);
        m_settings.setValue("router","192.168.80.1");
        m_settings.setValue("hostname", "dcerouter.linuxmce");
        m_settings.setValue("externalhostname", "");
        m_settings.setValue("webaccess", "80");
        m_settings.setValue("device", -1);
        m_settings.endGroup();
        log(tr("Finished Initializing Network Settings"));
    }

    if(!m_settings.childGroups().contains("ui")){
        log(tr("Initializing UI Settings"));
        m_settings.beginGroup("ui");
        m_settings.setValue("skin","default");
        m_settings.setValue("usenetwork", false);
        m_settings.setValue("preferredsize", "small");
        m_settings.endGroup();
        log(tr("Finished Initializing UI Settings"));
    }

    if(!m_settings.childGroups().contains("textoptions")){
        log(tr("Initializing text settings"));
        m_settings.beginGroup("textoptions");
        m_settings.setValue("sizemodifier", 0);
        m_settings.setValue("font", "");
        m_settings.setValue("language", QLocale::system().name());
        m_settings.endGroup();
        log(tr("Finished Initializing text settings"));
    }

    if(!m_settings.childGroups().contains("media")){
        log(tr("Initializing Media Settings"));
        m_settings.beginGroup("media");
        m_settings.setValue("audiosorting",AttributeTypeHelper::Performer  );
        m_settings.setValue("audiosubtypesort", -1);
        m_settings.setValue("videosorting", AttributeTypeHelper::Title);
        m_settings.setValue("videosubtypesort", SubTypesHelper::MOVIES );
        log(tr("Finished Initializing Media settings"));
    }

    setFontSizeMod(getOption(SettingsInterfaceType::Settings_Text, SettingsKeyType::Setting_Text_sizemod).toInt());

    log(tr("Settings are ready"));
    setReady(true);
}

void SettingInterface::destroySettingsData()
{
    setReady(false);
    m_persistentName = getOption(SettingsInterfaceType::Settings_Network, SettingsKeyType::Setting_Network_DeviceName).toString();
    m_settings.clear();
    qDebug() << m_settings.allKeys();
    initializeSettings();
}

void SettingInterface::log(QString message)
{
    QString logMsg;
    logMsg=QDateTime::currentDateTime().toLocalTime().toString()+"::"+message;
    emit newLogMessage(logMsg);
    qDebug() << logMsg;
}

void SettingInterface::setOption(SettingsInterfaceType::SettingsType st, SettingsKeyType::SettingKey sk, QVariant sval)
{
    QString grp="";
    QString key = m_lookup.value(sk);
    switch (st) {
    case SettingsInterfaceType::Settings_Network:grp="network"; break;
    case SettingsInterfaceType::Settings_Media:grp="media"; break;
    case SettingsInterfaceType::Settings_Text: grp="textoptions"; break;
    case SettingsInterfaceType::Settings_UI: grp="ui"; break;
    default: grp="";
        break;
    }
    if(grp=="")
        return;

    log(QString("Setting option for GROUP: -=%1=- KEY: -=%2=- VALUE: %3").arg(grp).arg(m_lookup.value(sk)).arg(sval.toString()));

    if(m_settings.status()==QSettings::AccessError){
        emit writeError(tr("No settings object to write to!"));
        return;
    }

    m_settings.beginGroup(grp);
    if(!m_settings.contains(key)){
        emit writeError(tr("Invalid settings option"));
        log(QString("Options for this key are %1").arg(m_settings.childKeys().join("\n")));
    } else {
        if(sk==SettingsKeyType::Setting_Network_DeviceName){
            m_persistentName=sval.toString();
        }
        m_settings.setValue(key, sval);
    }
    log("Option set");
    m_settings.endGroup();

}

QVariant SettingInterface::getOption(SettingsInterfaceType::SettingsType st, SettingsKeyType::SettingKey sk)
{

    qDebug() << Q_FUNC_INFO << "Incoming value " << (int)st;
    QString grp="";
    QString key = m_lookup.value(sk);
    QVariant rtrn= QVariant("invalid");
    switch (st) {
    case SettingsInterfaceType::Settings_Network:grp="network"; break;
    case SettingsInterfaceType::Settings_Media:grp="media"; break;
    case SettingsInterfaceType::Settings_Text: grp="textoptions"; break;
    case SettingsInterfaceType::Settings_UI: grp="ui"; break;
    default: grp="";
        break;
    }
    log(QString("Getting option for group -=%1=-").arg(grp));

    if(grp=="")
        return rtrn;

    if(m_settings.status()==QSettings::AccessError){
        emit writeError(tr("No settings object to read!"));
        return rtrn;
    }


    m_settings.beginGroup(grp);

    if(!m_settings.contains(key)){

        if(key=="deviceName"){
            QString d = QHostInfo::localHostName();
            if(d=="localhost"){
                d= "QOrbiter Generic";
            }
            setOption(SettingsInterfaceType::Settings_Network, SettingsKeyType::Setting_Network_DeviceName, d);
            rtrn = d;
        } else {
            emit writeError(tr("No  settings option for -=%1=-").arg(key));
            log(QString("Options for this key are %1").arg(m_settings.childKeys().join("\n")));
            m_settings.endGroup();
        }

    } else {
        qDebug() << Q_FUNC_INFO << "Returning settings value for key" << key;
        rtrn=m_settings.value(key);
    }
    m_settings.endGroup();
    return rtrn;

}
