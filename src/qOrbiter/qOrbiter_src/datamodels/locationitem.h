#ifndef LOCATIONITEM_H
#define LOCATIONITEM_H

#include <QAbstractListModel>
#include <QVariant>
#include <QImage>
#include <QUrl>
#include <QDebug>
/*!
 * \brief The LocationItem class.
 * Represents a single item in the LocationModel Listmodel
 *\ingroup room_list
 */
class LocationItem: public QObject{
    Q_OBJECT


public:
    /*!
     * \brief The Location Roles enum.
     * Defines the properties of the roomsList model.
     * \ingroup data_model
     * \relates room_list
     */
    enum Roles {
        NameRole = Qt::DisplayRole+1, /*!< 'name' - string room name*/      
        intRole= Qt::DisplayRole+2, /*!< 'intRoom' - Integer room value */
        TypeRole= Qt::DisplayRole+3,/*!< 'room_type' - Integer Type of room */
        ImageRole= Qt::DisplayRole+4, /*!< 'room_image' - String / url to image in skin directory */
        EaListRole=Qt::DisplayRole+5, /*!< 'ea_list' QVariantMap which can be accessed as a javascript object. */
        HideRole=Qt::DisplayRole+6  /*!< hide from orbiters */
    };

public:
    LocationItem() {}
    explicit LocationItem( QString &name,  int &iRoom, int &roomType,  QUrl &rmimage, bool isHidden, QObject *parent = 0);

    QVariant data(int role) const;
    QHash<int, QByteArray> roleNames() const;

    inline QString id() const { return m_name; }    
    inline int roomVal() const { return m_val; } 
    inline int room_type() const {return m_iType;}
    inline QUrl room_image() const {return roomImage;}
    inline bool hideFromOrbiter() const {return hideRoom;}
    inline QVariantList entertainAreas() const {return ea_list;}
    void addEa(QString name, int no){
        QVariantMap p;

        p.insert("ea_number", no);
        p.insert("ea_name", name);
        p.insert("room",roomVal());
        ea_list.append(p);
        emit dataChanged();       
    }

private:
    QString m_name;
    int m_val;
    int m_iType;
    QString m_title;
    QUrl roomImage;
    QVariantList ea_list;
    bool hideRoom;

signals:
    void imageChanged();
    void dataChanged();

};
#endif // LOCATIONITEM_H
