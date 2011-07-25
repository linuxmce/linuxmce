#include <datamodels/gridItem.h>

gridItem::gridItem( QString &name,  QString &path,  int &index,  QImage &img, QObject *parent) :
    m_name(name), m_path(path), m_index(index), m_image(img)
{

}

QHash<int, QByteArray> gridItem::roleNames() const
{

  QHash<int, QByteArray> names;
  names[NameRole] = "name";
  names[IndexRole] = "dceindex";
  names[PathRole] = "path";
  names[AttributeRole] = "attributes";
  names[ImageRole] = "cellImage";
  return names;


}

QVariant gridItem::data(int role) const
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
  default:
    return QVariant();
  }

}
