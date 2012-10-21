#ifndef EXISTINGORBITER_H
#define EXISTINGORBITER_H

#include <QObject>

class ExistingOrbiter : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString label READ get_label WRITE set_label NOTIFY titleChanged)
    Q_PROPERTY(QString location READ get_title WRITE set_title NOTIFY locationChanged)
    Q_PROPERTY(int i_device_number READ get_device_number WRITE set_device_number NOTIFY deviceChanged)

public:
    explicit ExistingOrbiter(int deviceno, QString title, QObject *parent = 0);
    QString label;
    QString location;
    int i_device_number;
    
signals:
    void deviceChanged();
    void titleChanged();
    void locationChanged();
    
public slots:
    void set_title(QString t);
    QString get_title();

    void set_device_number(int dev);
    int get_device_number();

    void set_label(QString lab);
    QString get_label();
    
};

#endif // EXISTINGORBITER_H
