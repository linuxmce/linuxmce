#include <datamodels/locationitem.h>

LocationItem::LocationItem(const QString &name, const int &iRoom, const QString &sTitle, const int &ea,const int &roomType, const QUrl &rmimg, QObject *parent) :
    m_name(name), m_val(iRoom), m_title(sTitle), m_iEA(ea), m_iType(roomType), roomImage(rmimg)
{
}


QHash<int, QByteArray> LocationItem::roleNames() const
{
  QHash<int, QByteArray> names;
  names[NameRole] = "name";
  names[EaRole] = "entertain_area";
  names[intRole] = "intRoom";
  names[TitleRole] = "title";
  names[TypeRole] = "room_type";
  names[ImageRole] = "room_image";


  return names;
}

QVariant LocationItem::data(int role) const
{
  switch(role) {
  case NameRole:
    return id();
  case EaRole:
    return entertain_area();
  case TitleRole:
    return title();
  case intRole:
    return roomVal();
  case TypeRole:
      return room_type();
  case ImageRole:
      return room_image();
  default:
    return QVariant();
  }
}
