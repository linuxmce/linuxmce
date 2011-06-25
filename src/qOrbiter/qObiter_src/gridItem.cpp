#include "gridItem.h"

gridItem::gridItem(const QString &name, const QString &size, const QImage &img, QObject *parent) :
    ListItem(parent), m_name(name), m_size(size), m_price(-1), m_image(img)
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
  names[ImageRole] = "image";

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
    return image();
  case PriceRole:
    return price();
  default:
    return QVariant();
  }
}
