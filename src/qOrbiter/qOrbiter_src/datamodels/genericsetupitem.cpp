#include "genericsetupitem.h"


GenericSetupItem::GenericSetupItem( QString &ident, QString &name,  QString &path,  int &index,  QImage &img, QObject *parent) :
    m_fk_file(ident), m_name(name), m_path(path), m_index(index), m_image(img)
{

}

QHash<int, QByteArray> GenericSetupItem::roleNames() const
{

  QHash<int, QByteArray> names;
  names[NameRole] = "type";
  names[IndexRole] = "var1";
  names[PathRole] = "var2";
  names[AttributeRole] = "var3";
  names[ImageRole] = "var3";
  names[FKRole]= "var5";
  names[AspectRole] = "var6";
  return names;
}

QVariant GenericSetupItem::data(int role) const
{
  switch(role) {
  case NameRole:
    return name();
  case IndexRole:
    return index();
  case ImageRole:
    return cellImage();
  case PathRole:
    return path();
  case FKRole:
    return id();
  case AspectRole:
  return imgAspect();
  default:
    return QVariant();
  }
}
