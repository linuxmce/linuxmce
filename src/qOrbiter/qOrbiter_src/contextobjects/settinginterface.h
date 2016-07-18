#ifndef SETTINGINTERFACE_H
#define SETTINGINTERFACE_H

#include <QObject>
#include <QSettings>
#include "qdebug.h"

class SettingsInterfaceType : public QObject
{
    Q_OBJECT

public:
    SettingsInterfaceType() {}
    virtual ~SettingsInterfaceType() {}

    enum SettingsType{
        Settings_Network,
        Settings_UI,
        Settings_Media,
        Settings_Text,
        Settings_Custom
    };
    Q_ENUMS(SettingsType)

};

class SettingsKeyType : public QObject
{
    Q_OBJECT
public:
    SettingsKeyType() {}
    virtual ~SettingsKeyType() {}

    enum SettingKey{
        Setting_Network_Router,
        Setting_Network_Device_ID,
        Setting_Network_Last_Used,
        Setting_Network_Hostname,
        Setting_Network_ExternalHostname,
        Setting_Network_WebPort,
        Setting_Network_DeviceName,
        Setting_Ui_Skin,
        Setting_Ui_NetworkLoading,
        Setting_Ui_PrefSize,
        Setting_Ui_ScreenSaver,
        Setting_Text_sizemod,
        Setting_Text_font,
        Setting_Text_language,
        Setting_Media_AudioSort,
        Setting_Media_AudioSubTypeSort,
        Setting_Media_VideoSort,
        Setting_Media_VideSubTypeSort
    };
    Q_ENUMS(SettingKey)

};

class SettingInterface : public QObject
{
    Q_OBJECT
      Q_PROPERTY(bool ready READ getReady NOTIFY readyChanged)
      Q_PROPERTY(int fontSizeMod READ fontSizeMod  NOTIFY fontSizeModChanged)
public:
    explicit SettingInterface(QObject *parent = 0);
    bool ready;

    void setSimpleOption(SettingsKeyType::SettingKey sk, QVariant sval);
    void setOption(SettingsInterfaceType::SettingsType st,  SettingsKeyType::SettingKey sk, QVariant sval);
    QVariant getOption(SettingsInterfaceType::SettingsType st, SettingsKeyType::SettingKey sk);

    bool getReady() const;
    void setReady(bool value);
    int fontSizeMod() { return m_fontSizeMod;}

signals:
    void newLogMessage(QString msg);
    void settingsDataCleared();
    void writeError(QString error);
    void readyChanged();

signals://for 'live properties'
    void fontSizeModChanged();

public slots:

    void setLastLocation(int room, int entArea);
    int getLastRoom();
    int getLastEa();

    void log(QString message);

    void destroySettingsData();

    Q_INVOKABLE void setOption(int settingType, int settingKey, QVariant val){
        setOption( (SettingsInterfaceType::SettingsType)settingType, (SettingsKeyType::SettingKey)settingKey, val  );
    }

    Q_INVOKABLE  QVariant getOption(int settingType, int keyType){
        return getOption((SettingsInterfaceType::SettingsType)settingType, (SettingsKeyType::SettingKey)keyType);
    }

    Q_INVOKABLE void setFontSizeMod(int m){
        setSimpleOption(SettingsKeyType::Setting_Text_sizemod, m);
        m_fontSizeMod = m;
        emit fontSizeModChanged();
        qDebug() << Q_FUNC_INFO << m;
    }
    Q_INVOKABLE bool isScreenSaverEnabled() {
        return getOption(SettingsInterfaceType::Settings_UI, SettingsKeyType::Setting_Ui_ScreenSaver).toBool();
    }

private slots:
    void initializeSettings();


private:
    QString m_persistentName;
    QSettings m_settings;
    QMap <SettingsKeyType::SettingKey, QString> m_lookup;

    int m_fontSizeMod;

};

#endif // SETTINGINTERFACE_H
