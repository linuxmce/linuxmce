#include "datagriditem.h"

DataGridItem::DataGridItem( QString &ident, QString &name,  QString &path,  int &index,  QImage &img, QObject *parent) :
    m_fk_file(ident), m_name(name), m_path(path), m_index(index), m_image(img)
{

}

QHash<int, QByteArray> DataGridItem::roleNames() const
{

  QHash<int, QByteArray> names;
  names[NameRole] = "name";
  names[IndexRole] = "modelindex";
  names[PathRole] = "path";
  names[AttributeRole] = "attributes";
  names[ImageRole] = "cellImage";
  names[FKRole]= "fkfile";
  return names;
}

QVariant DataGridItem::data(int role) const
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
  default:
    return QVariant();
  }
}
