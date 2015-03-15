#ifndef SETTINGINTERFACE_H
#define SETTINGINTERFACE_H

#include <QObject>

class SettingInterface : public QObject
{
    Q_OBJECT
public:
    explicit SettingInterface(QObject *parent = 0);

signals:

public slots:

};

#endif // SETTINGINTERFACE_H
