#include "playlistitemclass.h"
#ifdef debug
#include <QDebug>
#endif

PlaylistItemClass::PlaylistItemClass(QString ident, QString path,  int index) :
    m_fk_file(path), m_path(path), m_index(index), m_name(ident)
{
isActive = false;

}

QHash<int, QByteArray> PlaylistItemClass::roleNames() const
{

  QHash<int, QByteArray> names;
  names[NameRole] = "name";
  names[IndexRole] = "dceindex";
  names[PathRole] = "path"; 
  names[FKRole]= "file";
  names[StateRole] = "state";
  return names;
}

void PlaylistItemClass::addedToModel()
{
    emit dataChanged();
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
    return path();
  default:
    return QVariant();
  }
}
