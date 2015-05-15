#ifndef EXISTINGORBITER_H
#define EXISTINGORBITER_H

#include <QObject>
#include <QVariant>

class ExistingOrbiter : public QObject
{
    Q_OBJECT

    enum Roles {
        LabelRole = Qt::UserRole+1,
        LocationRole =Qt::UserRole+2,
        DeviceRole= Qt::UserRole+3
    };
public:

    explicit ExistingOrbiter(int deviceno, QString title, QObject *parent = 0);
    ExistingOrbiter() {}
    ExistingOrbiter( const ExistingOrbiter& other);
    QVariant data(int role) const;
    QHash<int, QByteArray> roleNames() const;



    inline QString id() const {  return label; }
    inline QString locationData() const { return location; }
    inline int deviceData() const { return i_device_number; }

    
signals:
    void deviceChanged();
    void titleChanged();
    void locationChanged();
    void dataChanged();
public slots:
    void set_title(QString t);
    QString get_title();

    void set_device_number(int dev);
    int get_device_number();

    void set_label(QString lab);
    QString get_label();
    
private:
    QString label;
    QString location;
    int i_device_number;
};

Q_DECLARE_METATYPE(ExistingOrbiter)
#endif // EXISTINGORBITER_H
