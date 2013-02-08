#include "epgitemclass.h"

EPGItemClass::EPGItemClass( QString chanName, int chanIndex,  QString channel, QString program, int dceIndex, QString chanImage, QString progImag ) :
    m_channame(chanName), m_dceIndex(dceIndex), m_channel(channel), m_program(program), channel_image(chanImage), program_image(progImag), channel_number(chanIndex)
{

}

QHash<int, QByteArray> EPGItemClass::roleNames() const
{

  QHash<int, QByteArray> names;
  names[NameRole] = "name";
  names[IndexRole] = "index";
  names[ChannelRole] = "channelnumber";
  names[ProgramRole] = "program";
  names[IdRole]= "id";
  names[ChanImageRole] = "channelimage";
  names[ProgImageRole] = "programimage";
  names[ChannelIdRole] = "channelid";
  return names;
}

QVariant EPGItemClass::data(int role) const
{
  switch(role) {
  case NameRole:
    return name();
  case IndexRole:
    return index();
  case ChannelRole:
    return channel();
  case IdRole:
    return id();
  case ChanImageRole:
    return channelImage();
  case ProgImageRole:
    return programImage();
  case ChannelIdRole:
    return mythid();
  case ProgramRole:
  return program();
  default:
    return QVariant();
  }
}
