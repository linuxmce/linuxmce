#include "mediatypeitem.h"




MediaSubTypeItem::MediaSubTypeItem( QString name,  QString pk_fileformat,  QImage img, bool selstat,  QObject *parent) :
    m_description(name), m_qs_mediatype(pk_fileformat), m_image(img), m_isSelected(selstat)
{

}

QHash<int, QByteArray> MediaSubTypeItem::roleNames() const
{

  QHash<int, QByteArray> names;
  names[NameRole] = "desc";
  names[IndexRole] = "name";
  names[PKRole] = "pkattribute";
  names[ImageRole] = "image";
  names[StateRole] = "status";

  return names;
}

QVariant MediaSubTypeItem::data(int role) const
{
  switch(role) {
  case NameRole:
    return name();
  case IndexRole:
    return id();
  case ImageRole:
    return cellImage();
  case PKRole:
    return mediatype();
  case StateRole:
    return selectedStatus();
  default:
    return QVariant();
  }
}

void MediaSubTypeItem::updateSelection(bool newbool)
{
    if (m_isSelected == true)
    {
        m_isSelected = false;
        //qDebug () << this->id() << "Button state is " << m_isSelected;
        emit filterChanged();
       // return m_isSelected;
    }
    else
    {
        m_isSelected = true;
        //qDebug () << this->id() << "Button state is " << m_isSelected;
       emit filterChanged();
       //return m_isSelected;
    }
}
