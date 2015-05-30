#ifndef HUECOMMAND_H
#define HUECOMMAND_H

#include <QObject>
#include <QUrl>
#include <QVariant>

class HueCommand : public QObject
{
    Q_OBJECT
public:
    explicit HueCommand(QObject *parent = 0);
    HueCommand(QUrl target_url, QVariant target_params, QObject *parent=0);

    QVariant parameters;
    QUrl target;
signals:

public slots:
};

#endif // HUECOMMAND_H
