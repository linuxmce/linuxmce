#ifndef MEDIATYPEITEM_H
#define MEDIATYPEITEM_H
#include <QAbstractListModel>
#include <QVariant>
#include <QImage>
#include <QMap>

//for use with the fileformat table.
class MediaSubTypeItem: public QObject
{
 Q_OBJECT
    Q_PROPERTY (bool m_isSelected READ selectedStatus WRITE updateSelection NOTIFY filterChanged)

public:
  enum Roles {
    IndexRole = Qt::UserRole+1,
    NameRole =Qt::UserRole+2,
    PKRole= Qt::UserRole+3,
    ImageRole = Qt::DisplayRole+4,
    StateRole = Qt::DisplayRole+5
  };

public:
  MediaSubTypeItem() {}
  explicit MediaSubTypeItem( QString name,  QString pk_fileformat,  QImage img, bool selstat, QObject *parent = 0);
  QVariant data(int role) const;
  QHash<int, QByteArray> roleNames() const;

   QString id() const {  return m_description; }
    QString name() const {  return m_description; }
   QString mediatype() const { return m_qs_mediatype; }
   QImage cellImage() const {  return m_image; }
   bool selectedStatus() const { return m_isSelected; }
   bool setStatus(bool b) { m_isSelected = b; return m_isSelected; }
   void updateSelection (bool newBool) ;

private:

  bool m_isSelected;
  QString m_qs_mediatype;
  QString m_description;
   QImage m_image;

signals:
  void imageChanged();
  void dataChanged();
  void filterChanged();
};
#endif // MEDIATYPEITEM_H
