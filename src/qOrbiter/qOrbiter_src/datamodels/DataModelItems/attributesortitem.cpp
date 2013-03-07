#include "attributesortitem.h"
#include <QDebug>




AttributeSortItem::AttributeSortItem(QString name,  QString pk_fileformat,  QString img, bool selstat,  QObject *parent) :
    m_description(name), m_fk_fileformat(pk_fileformat), m_image(img), m_isSelected(selstat)
{

}

QHash<int, QByteArray> AttributeSortItem::roleNames() const
{

  QHash<int, QByteArray> names;
  names[NameRole] = "desc";
  names[IndexRole] = "name";
  names[PKRole] = "pkattribute";
  names[ImageRole] = "image";
  names[StateRole] = "status";

  return names;
}

QVariant AttributeSortItem::data(int role) const
{
  switch(role) {
  case NameRole:
    return name();
  case IndexRole:
    return id();
  case ImageRole:
    return cellImage();
  case PKRole:
    return fileformat();
  case StateRole:
    return selectedStatus();
  default:
    return QVariant();
  }
}

bool AttributeSortItem::updateSelection(bool newbool)
{
    setStatus(newbool);
}

