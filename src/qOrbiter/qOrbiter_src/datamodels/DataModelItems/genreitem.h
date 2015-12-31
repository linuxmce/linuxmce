#ifndef GENREITEM_H
#define GENREITEM_H

#include <QAbstractListModel>
#include <QVariant>
#include <QImage>
#include <QMap>

//for use with the fileformat table.
class GenreItem: public QObject
{
 Q_OBJECT
    Q_PROPERTY (bool m_isSelected READ selectedStatus WRITE setStatus NOTIFY filterChanged)

public:
  enum Roles {
    IndexRole = Qt::UserRole+1,
    NameRole =Qt::UserRole+2,
    PKRole= Qt::UserRole+3,
    ImageRole = Qt::DisplayRole+4,
    StateRole = Qt::DisplayRole+5
  };

public:
  GenreItem() {}
  explicit GenreItem( QString name,  QString pk_fileformat,  QImage img, bool selstat, QObject *parent = 0);
  QVariant data(int role) const;
  QHash<int, QByteArray> roleNames() const;

  inline QString id() const {  return m_description; }
   inline QString name() const {  return m_description; }
  inline QString fileformat() const { return m_fk_fileformat; }
  inline QImage cellImage() const {  return m_image; }

  bool selectedStatus() const {return m_isSelected; }
  void setStatus(bool b) { m_isSelected = b; }
  bool updateSelection (bool newBool) ;

public slots:
  void destruct() {
      //qDebug() << "calling destructor for genre sort item.";
     this->deleteLater();
  }

private:
  bool m_isSelected;
  QString m_fk_fileformat;
  QString m_description;
   QImage m_image;

signals:
  void imageChanged();
  void dataChanged();
  void filterChanged(bool m_isSelected);
};

#endif // GENREITEM_H
