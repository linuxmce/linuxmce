#include <datamodels/locationmodel.h>

LocationItem::LocationItem(const QString &name, const int &iRoom, const QString &sTitle, const int &ea, QObject *parent) :
    ListItem(parent), m_name(name), m_val(iRoom), m_title(sTitle)
{
}


QHash<int, QByteArray> LocationItem::roleNames() const
{
  QHash<int, QByteArray> names;
  names[NameRole] = "name";
  names[EaRole] = "entertain_area";
  names[intRole] = "intRoom";
  names[TitleRole] = "Title";


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
  default:
    return QVariant();
  }
}
