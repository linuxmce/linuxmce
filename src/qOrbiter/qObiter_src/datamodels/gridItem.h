#ifndef GRIDITEM_H
#define GRIDITEM_H

#include <QAbstractListModel>
#include <QVariant>
#include <QImage>
#include <QMap>

class gridItem: public QObject
{
 Q_OBJECT


public:
  enum Roles {
    NameRole = Qt::UserRole+1,
    IndexRole,
    PathRole,
    ImageRole,
    AttributeRole

  };

public:
  gridItem(QObject *parent = 0) {}
  explicit gridItem( QString &name,  QString &path,  int &index,  QImage &img, QObject *parent = 0);
  QVariant data(int role) const;
  QHash<int, QByteArray> roleNames() const;

  inline QString id() const {  return m_name; }
  inline QString name() const { return m_name; }
  inline int index() const { return m_index; }
  inline QString path() const { return m_path; }
  inline QImage cellImage() const {return m_image; }
  inline QMap <QString*, int> attributes() const {return m_mapAttrib;}


private:
  QString m_name;
  int m_index;
  QString m_path;
  QImage m_image;
  QMap <QString*, int> m_mapAttrib;

signals:
  void imageChanged();
  void dataChanged();
};
#endif // GRIDITEM_H
