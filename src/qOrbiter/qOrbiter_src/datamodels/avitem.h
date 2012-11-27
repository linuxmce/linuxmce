#ifndef AVITEM_H
#define AVITEM_H
/*
 the purpose of this class is to be an abstact api for interacting with
    the av items in the context of sending them commands and other functionality
   from the previous 'resend av codes' screen.
   Each item will be represented by a simple qObject that can called by any qml anywhere.
   At the top level, it will consist of device details relevant to extracting further information
   about the device (like codes and commands) to resend codes or other activity.
  */

#include <QObject>
#include <QVariant>
#include <QImage>
#include <QMap>
class AvItem : public QObject
{
    Q_OBJECT

    enum Roles {
        NameRole = Qt::UserRole+1,
        NumberRole =Qt::UserRole+2,
        TemplateRole= Qt::DisplayRole+3,
        ImageRole = Qt::DisplayRole+4,
        AttributeRole = Qt::UserRole+5
    };

public:
    AvItem(QObject *parent = 0) {}
    explicit AvItem( QString &name, int &num,  int &templateNo, QImage &img, QString &attrib, QObject *parent = 0);
    QVariant data(int role) const;
    QHash<int, QByteArray> roleNames() const;

    inline QString name() const { return m_name.toLatin1(); }               //returns name of device
    inline int device_number() const { return m_deviceNo; }                 //returns device number
    inline int device_template() const { return m_deviceTemplate; }         //returns device template
    inline QImage cellImage() const {  return m_image; }                    //returns icon associated with device
    inline QMap <QString*, int> attributes() const {return m_mapAttrib;}    //returns attribute map, unused in this case for now


private:                                //internal class variables
    QString m_name;         //name of device
    int m_deviceNo;         //device number
    int m_deviceTemplate;   //device template
    QImage m_image;         //image slot to use for icon
    QString attribute;      //misc attribute

    QMap <QString*, int> m_mapAttrib;   //potentially used to populate the commands for fast reference

signals:    //signals unused at the moment
    void imageChanged();
    void dataChanged();

};

#endif // AVITEM_H
