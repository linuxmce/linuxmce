#ifndef LOCATIONITEM_H
#define LOCATIONITEM_H

#include <QAbstractListModel>
#include <QVariant>
#include <QImage>
#include <QUrl>
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
        EaRole = Qt::DisplayRole+2, /*!< 'entertain_area' - Integer entertain area value.*/
        intRole= Qt::DisplayRole+3, /*!< 'intRoom' - Integer room value */
        TypeRole= Qt::DisplayRole+5,/*!< 'room_type' - Integer Type of room */
        ImageRole= Qt::DisplayRole+6, /*!< 'room_image' - String / url to image in skin directory */
        EaNameRole= Qt::DisplayRole+7 /*! 'ea_name' - String entertain area's name */
    };

public:
    LocationItem() {}
    explicit LocationItem(const QString &name, const int &iRoom, const int &ea, const QString &ea_name, const int &roomType, const QUrl &rmimage, QObject *parent = 0);

    QVariant data(int role) const;
    QHash<int, QByteArray> roleNames() const;

    inline QString id() const { return m_name; }
    inline QString eaId()const {return m_eaname;}
    inline int roomVal() const { return m_val; }
    inline int entertain_area() const { return m_iEA; }
    inline int room_type() const {return m_iType;}
    inline QUrl room_image() const {return roomImage;}

private:
    QString m_name;
    int m_val;
    int m_iEA;
    QString m_eaname;
    int m_iType;
    QString m_title;
    QUrl roomImage;


signals:
    void imageChanged();

};
#endif // LOCATIONITEM_H
