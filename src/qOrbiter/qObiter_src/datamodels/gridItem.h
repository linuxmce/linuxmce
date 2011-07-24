#ifndef GRIDITEM_H
#define GRIDITEM_H

#include <QAbstractListModel>
#include <QVariant>
#include <QImage>

class gridItem: public QObject
{
 Q_OBJECT


public:
  enum Roles {
    NameRole = Qt::UserRole+1,
    SizeRole,
    PriceRole,
    TitleRole,
    ImageRole
  };

public:
  gridItem(QObject *parent = 0) {}
  explicit gridItem(const QString &name, const QString &size, const QImage &img, QObject *parent = 0);
  QVariant data(int role) const;
  QHash<int, QByteArray> roleNames() const;
  void setPrice(qreal price);
  inline QString id() const {  return m_name; }
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
  void dataChanged();
};
#endif // GRIDITEM_H
