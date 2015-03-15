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

public slots:
    void log(QString message);


private slots:
    void initializeSettings();
    void destroySettingsData();

private:
    QSettings *m_settings;

};

#endif // SETTINGINTERFACE_H
