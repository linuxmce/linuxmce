#ifndef SETTINGINTERFACE_H
#define SETTINGINTERFACE_H

#include <QObject>
#include <QSettings>

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
        Setting_Network_Hostname,
        Setting_Network_ExternalHostname,
        Setting_Network_WebPort,
        Setting_Ui_Skin,
        Setting_Ui_NetworkLoading,
        Setting_Ui_PrefSize,
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
public:
    explicit SettingInterface(QObject *parent = 0);
    bool ready;

    void setOption(SettingsInterfaceType::SettingsType st,  SettingsKeyType::SettingKey sk, QVariant sval);
    QVariant getOption(SettingsInterfaceType::SettingsType st, SettingsKeyType::SettingKey sk);

signals:
    void newLogMessage(QString msg);
    void settingsDataCleared();
    void writeError(QString error);

public slots:
    void log(QString message);

    void destroySettingsData();

    Q_INVOKABLE void setOption(int settingType, int settingKey, QVariant val){
        setOption( (SettingsInterfaceType::SettingsType)settingType, (SettingsKeyType::SettingKey)settingKey, val  );
    }

    Q_INVOKABLE  QVariant getOption(int settingType, int keyType){
        return getOption((SettingsInterfaceType::SettingsType)settingType, (SettingsKeyType::SettingKey)keyType);
    }
private slots:
    void initializeSettings();


private:
    QSettings m_settings;
    QMap <SettingsKeyType::SettingKey, QString> m_lookup;

};

#endif // SETTINGINTERFACE_H
