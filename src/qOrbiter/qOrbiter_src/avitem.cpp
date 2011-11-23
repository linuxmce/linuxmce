#include "avitem.h"

AvItem::AvItem( QString &name, int &num,  int &templateNo, QImage &img, QString &attrib, QObject *parent) :
   m_name(name), m_deviceNo(num), m_deviceTemplate(templateNo), m_image(img), attribute(attrib)
{

}

QHash<int, QByteArray> AvItem::roleNames() const
{

  QHash<int, QByteArray> names;
  names[NameRole] = "name";
  names[NumberRole] = "deviceno";
  names[TemplateRole] = "devicetemplate";
  names[AttributeRole] = "attributes";
  names[ImageRole] = "icon";
  return names;
}

QVariant AvItem::data(int role) const
{
  switch(role) {
  case NameRole:
    return name();
  case NumberRole:
    return device_number();
  case ImageRole:
    return cellImage();
  case TemplateRole:
    return device_template();
   default:
    return QVariant();
  }
}
