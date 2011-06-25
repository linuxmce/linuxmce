#ifndef GRIDITEM_H
#define GRIDITEM_H

#include <listModel.h>
#include <QImage>

class gridItem : public ListItem
{
  Q_OBJECT
 Q_PROPERTY(QImage cellImage READ cellImage NOTIFY imageChanged)

public:
  enum Roles {
    NameRole = Qt::UserRole+1,
    SizeRole,
    PriceRole,
    TitleRole,
    ImageRole

  };

public:
  gridItem(QObject *parent = 0): ListItem(parent) {}
  explicit gridItem(const QString &name, const QString &size, const QImage &img, QObject *parent = 0);
  QVariant data(int role) const;
  QHash<int, QByteArray> roleNames() const;
  void setPrice(qreal price);
  inline QString id() const { return m_name; }
  inline QString name() const { return m_name; }
  inline QString size() const { return m_size; }
  inline QImage cellImage() const {return m_image; }
  inline qreal price() const { return m_price; }

private:
  QString m_name;
  QString m_size;
  qreal m_price;
  QImage m_image;

signals:
  void imageChanged();
};
#endif // GRIDITEM_H
