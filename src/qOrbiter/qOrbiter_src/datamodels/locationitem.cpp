#include <datamodels/locationitem.h>

LocationItem::LocationItem(const QString &name, const int &iRoom,  const int &ea, const QString &ea_name, const int &roomType, const QUrl &rmimg, QObject *parent) :
    m_name(name), m_val(iRoom), m_iEA(ea), m_eaname(ea_name), m_iType(roomType), roomImage(rmimg)
{
}


QHash<int, QByteArray> LocationItem::roleNames() const
{
  QHash<int, QByteArray> names;
  names[NameRole] = "name";
  names[EaRole] = "entertain_area";
  names[intRole] = "intRoom"; 
  names[TypeRole] = "room_type";
  names[ImageRole] = "room_image";
  names[EaNameRole] = "ea_name";

  return names;
}

QVariant LocationItem::data(int role) const
{
  switch(role) {
  case NameRole:
    return id();
  case EaRole:
    return entertain_area(); 
  case intRole:
    return roomVal();
  case TypeRole:
      return room_type();
  case ImageRole:
      return room_image();
  case EaNameRole:
      return eaId();
  default:
    return QVariant();
  }
}
