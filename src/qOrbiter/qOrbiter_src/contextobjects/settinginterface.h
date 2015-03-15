#ifndef SETTINGINTERFACE_H
#define SETTINGINTERFACE_H

#include <QObject>
#include <QSettings>

class SettingInterface : public QObject
{
    Q_OBJECT
public:
    explicit SettingInterface(QObject *parent = 0);

signals:
    void newLogMessage(QString msg);
    void settingsDataCleared();
    void writeError(QString error);

public slots:
    void log(QString message);
    void setOption(QString grp, QString key, QVariant opt);
    void setNetworkOption(QString key, QVariant opt);
    void setTextOption(QString key, QVariant opt);
    void setMediaOption(QString key, QVariant opt);



private slots:
    void initializeSettings();
    void destroySettingsData();

private:
    QSettings *m_settings;

};

#endif // SETTINGINTERFACE_H
