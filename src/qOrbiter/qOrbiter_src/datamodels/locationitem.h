#ifndef LOCATIONITEM_H
#define LOCATIONITEM_H

#include <QAbstractListModel>
#include <QVariant>
#include <QImage>
#include <QUrl>

class LocationItem: public QObject{
Q_OBJECT


  public:
    enum Roles {
      NameRole = Qt::DisplayRole+1,
      EaRole = Qt::DisplayRole+2,
      intRole= Qt::DisplayRole+3,
      TypeRole= Qt::DisplayRole+5,
        ImageRole= Qt::DisplayRole+6,
        EaNameRole= Qt::DisplayRole+7
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
