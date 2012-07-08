#include <datamodels/gridItem.h>

gridItem::gridItem( QString &ident, QString &name,  QString &path,  int &index,  QImage &img) :
    m_fk_file(ident), m_name(name), m_path(path), m_index(index), m_image(img)
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
  names[FKRole]= "id";
  names[AspectRole] = "aspect";
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
  case FKRole:
    return id();
  case AspectRole:
  return imgAspect();
  default:
    return QVariant();
  }
}
