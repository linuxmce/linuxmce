#ifndef CLOUDSETTINGS_H
#define CLOUDSETTINGS_H

#include <QObject>
#include <QSettings>

class CloudSettings : public QObject
{
    Q_OBJECT
public:
    explicit CloudSettings(QObject *parent = 0);
    enum SettingsType{
        Setting_UserName,
        Setting_RealName,
        Setting_AccessToken,
        Setting_RefreshToken
    };

signals:
    void settingSaved();
    void settingsReady();

public slots:
    void setIdentity(QString appName, int deviceTemplate);

private:
    QSettings m_settings;
    QString m_appName;
    int m_deviceTemplate;
};

#endif // CLOUDSETTINGS_H
