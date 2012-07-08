#include "filedetailsitem.h"



FileDetailsItem::FileDetailsItem( QString title,  QString data,  QImage img, bool selstat,  QObject *parent) :
    m_title(title), m_attrib_data(data), m_image(img), m_isSelected(selstat)
{

}

QHash<int, QByteArray> FileDetailsItem::roleNames() const
{

  QHash<int, QByteArray> names;
  names[NameRole] = "title";
  names[IndexRole] = "id";
  names[DataRole] = "pkattribute";
  names[ImageRole] = "image";
  names[StateRole] = "status";

  return names;
}

QVariant FileDetailsItem::data(int role) const
{
  switch(role) {
  case NameRole:
    return title();
  case IndexRole:
    return id();
  case ImageRole:
    return cellImage();
  case DataRole:
    return attribute_data();
  case StateRole:
    return selectedStatus();
  default:
    return QVariant();
  }
}

void FileDetailsItem::updateSelection(bool newbool)
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
