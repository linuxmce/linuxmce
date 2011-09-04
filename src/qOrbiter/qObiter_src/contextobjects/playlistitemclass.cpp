#include "playlistitemclass.h"
PlaylistItemClass::PlaylistItemClass( QString &ident, QString &name,  QString &path,  int &index,  QObject *parent) :
    m_fk_file(ident), m_name(name), m_path(path), m_index(index)
{
isActive = false;
}

QHash<int, QByteArray> PlaylistItemClass::roleNames() const
{

  QHash<int, QByteArray> names;
  names[NameRole] = "name";
  names[IndexRole] = "dceindex";
  names[PathRole] = "path";
  names[AttributeRole] = "attributes";
  names[FKRole]= "id";
  names[StateRole] = "state";
  return names;
}

QVariant PlaylistItemClass::data(int role) const
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
  default:
    return QVariant();
  }
}
