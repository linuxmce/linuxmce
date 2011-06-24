#include "gridItem.h"

gridItem::gridItem(const QString &name, const QString &size, QObject *parent) :
  ListItem(parent), m_name(name), m_size(size), m_price(-1)
{
}

void gridItem::setPrice(qreal price)
{
  if(m_price != price) {
    m_price = price;
    emit dataChanged();
  }
}

QHash<int, QByteArray> gridItem::roleNames() const
{
  QHash<int, QByteArray> names;
  names[NameRole] = "name";
  names[SizeRole] = "size";
  names[PriceRole] = "price";
  names[TitleRole] = "Title";
  names[ImageRole] = "Image";

  return names;
}

QVariant gridItem::data(int role) const
{
  switch(role) {
  case NameRole:
    return name();
  case SizeRole:
    return size();
  case TitleRole:
    return size();
  case ImageRole:
    return size();
  case PriceRole:
    return price();
  default:
    return QVariant();
  }
}
