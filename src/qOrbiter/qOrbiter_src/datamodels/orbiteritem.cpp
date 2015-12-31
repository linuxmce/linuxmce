#include "orbiteritem.h"

OrbiterItem::OrbiterItem(const QString &name, const QString &size, const QImage &img, QObject *parent) :
    OrbiterListItem(parent), m_name(name), m_size(size), m_command("command"), m_image(img)
{
}

void OrbiterItem::setCommand(QString command)
{
  if(m_command != command) {
    m_command = command;
    emit dataChanged();
  }
}

QHash<int, QByteArray> OrbiterItem::roleNames() const
{
  QHash<int, QByteArray> names;
  names[NameRole] = "name";
  names[SizeRole] = "size";
  names[CommandRole] = "command";
  names[TitleRole] = "title";
  names[ImageRole] = "image";

  return names;
}

QVariant OrbiterItem::data(int role) const
{
  switch(role) {
  case NameRole:
    return name();
  case SizeRole:
    return size();
  case TitleRole:
    return title();
  case ImageRole:
    return image();
  case CommandRole:
    return command();
  default:
    return QVariant();
  }
}
