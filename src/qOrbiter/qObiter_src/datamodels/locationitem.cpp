#include <datamodels/locationmodel.h>

LocationItem::LocationItem(const QString &name, const int &iRoom, const QString &sTitle, const int &ea,const int &roomType, QObject *parent) :
    m_name(name), m_val(iRoom), m_title(sTitle), m_iEA(ea), m_iType(roomType)
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
  default:
    return QVariant();
  }
}
