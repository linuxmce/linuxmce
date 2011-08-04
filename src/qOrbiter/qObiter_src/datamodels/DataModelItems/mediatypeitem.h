#ifndef MEDIATYPEITEM_H
#define MEDIATYPEITEM_H
#include <QAbstractListModel>
#include <QVariant>
#include <QImage>
#include <QMap>

//for use with the fileformat table.
class MediaTypeItem: public QObject
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
  MediaTypeItem(QObject *parent = 0) {}
  explicit MediaTypeItem( QString name,  QString pk_fileformat,  QImage img, bool selstat, QObject *parent = 0);
  QVariant data(int role) const;
  QHash<int, QByteArray> roleNames() const;

  inline QString id() const {  return m_description; }
   inline QString name() const {  return m_description; }
  inline QString fileformat() const { return m_fk_fileformat; }
  inline QImage cellImage() const {  return m_image; }
  inline bool selectedStatus() const { return m_isSelected; }
  inline bool setStatus(bool b) { m_isSelected = b; }
   void updateSelection (bool newBool) ;

private:

  bool m_isSelected;
  QString m_fk_fileformat;
  QString m_description;
   QImage m_image;

signals:
  void imageChanged();
  void dataChanged();
  void filterChanged();
};
#endif // MEDIATYPEITEM_H
