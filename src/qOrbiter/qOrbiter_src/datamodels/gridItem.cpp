#include <datamodels/gridItem.h>

gridItem::gridItem(QString ident, QString name,  QString path,  int index, QObject *parent) :
    m_fk_file(ident), m_name(name), m_path(path), m_index(index)
{
setPing(false);

}

QHash<int, QByteArray> gridItem::roleNames() const
{

  QHash<int, QByteArray> names;
  names[NameRole] = "name";
  names[IndexRole] = "dceindex";
  names[PathRole] = "path";
  names[AttributeRole] = "attributes";
  names[FKRole]= "id";
  names[AspectRole] = "aspect";
  names[TrackerRole]= "ping";
  return names;
}

QVariant gridItem::data(int role) const
{
  switch(role) {
  case NameRole:
    return name();
  case IndexRole:
    return index();
  case PathRole:
    return path();
  case FKRole:
    return id();
  case AspectRole:
  return imgAspect();
  case TrackerRole:
      return pinged();
  default:
    return QVariant();
  }
}
