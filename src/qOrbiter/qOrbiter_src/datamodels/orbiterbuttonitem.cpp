#include "orbiterbuttonitem.h"

OrbiterButtonItem::OrbiterButtonItem(const QString &name, const QString &size, const QString &goto_screen, const QImage &img, QObject *parent) :
     m_name(name), m_size(size), m_goto(goto_screen), m_image(img)
{
}




QHash<int, QByteArray> OrbiterButtonItem::roleNames() const
{
  QHash<int, QByteArray> names;
  names[NameRole] = "name";
  names[SizeRole] = "size";
  names[GotoRole] = "goto";
  names[TitleRole] = "Title";
  names[ImageRole] = "cellImage";
  names[DisplayRole] = "display";
  names[AttributeRole] = "attribute";


  return names;
}

QVariant OrbiterButtonItem::data(int role) const
{
  switch(role) {
  case NameRole:
    return name();
  case SizeRole:
    return size();
  case TitleRole:
    return size();
  case ImageRole:
    return cellImage();
  case GotoRole:
    return goto_data();
  case DisplayRole:
    return display();
  case AttributeRole:
    return attribute();
  default:
    return QVariant();
  }
}
