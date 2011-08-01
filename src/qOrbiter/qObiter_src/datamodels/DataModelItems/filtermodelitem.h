#ifndef FILTERMODELITEM_H
#define FILTERMODELITEM_H

#include <QAbstractListModel>
#include <QVariant>
#include <QImage>
#include <QMap>

//for use with the fileformat table.
class FilterModelItem: public QObject
{
 Q_OBJECT
    Q_PROPERTY (bool m_isSelected READ selectedStatus NOTIFY filterChanged)

public:
  enum Roles {
    IndexRole = Qt::UserRole+1,
    NameRole =Qt::UserRole+2,
    PKRole= Qt::UserRole+3,
    ImageRole = Qt::DisplayRole+4,
    StateRole = Qt::DisplayRole+5
  };

public:
  FilterModelItem(QObject *parent = 0) {}
  explicit FilterModelItem( QString name,  QString pk_fileformat,  QImage img, bool selstat, QObject *parent = 0);
  QVariant data(int role) const;
  QHash<int, QByteArray> roleNames() const;

  inline QString id() const {  return m_description; }
   inline QString name() const {  return m_description; }
  inline QString fileformat() const { return m_fk_fileformat; }
  inline QImage cellImage() const {  return m_image; }
  inline bool selectedStatus() const {return m_isSelected; }
  void updateSelection();


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
#endif // FILTERMODELITEM_H
