#ifndef LOCATIONITEM_H
#define LOCATIONITEM_H


#include <datamodels/listModel.h>
#include <QImage>

class LocationItem: public ListItem {
Q_OBJECT


  public:
    enum Roles {
      NameRole = Qt::UserRole+1,
      EaRole,
      intRole,
      TitleRole

    };

  public:
    LocationItem(QObject *parent = 0): ListItem(parent) {}
    explicit LocationItem(const QString &name, const int &iRoom, const QString &sTitle, const int &ea, QObject *parent = 0);
    QVariant data(int role) const;
    QHash<int, QByteArray> roleNames() const;
    void setPrice(qreal price);
    inline QString id() const { return m_name; }
    inline int roomVal() const { return m_val; }
    inline QString title() const { return m_title; }
    inline int entertain_area() const { return m_iEA; }

  private:
    QString m_name;
    int m_val;
    int m_iEA;
    QString m_title;


  signals:
    void imageChanged();

};
#endif // LOCATIONITEM_H
