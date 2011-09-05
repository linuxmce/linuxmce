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
    IndexRole =Qt::UserRole+2,
    PathRole= Qt::DisplayRole+3,
      ImageRole = Qt::DisplayRole+4,
    AttributeRole = Qt::UserRole+5,
    FKRole = Qt::DisplayRole+6

  };

public:
  gridItem(QObject *parent = 0) {}
  explicit gridItem( QString &ident, QString &name,  QString &path,  int &index,  QImage &img, QObject *parent = 0);
  QVariant data(int role) const;
  QHash<int, QByteArray> roleNames() const;

  inline QString id() const {  return m_fk_file.toUtf8(); }
  inline QString name() const { return m_name.toLatin1(); }
  inline int index() const { return m_index; }
  inline QString path() const { return m_path; }
  inline QImage cellImage() const {  return m_image; } 
  inline QMap <QString*, int> attributes() const {return m_mapAttrib;}


private:
  QString m_name;
  int m_index;
  QString m_path;
  QImage m_image;
  QString m_fk_file;
  QMap <QString*, int> m_mapAttrib;

signals:
  void imageChanged();
  void dataChanged();
};
#endif // GRIDITEM_H
