#include <datamodels/locationitem.h>
/*!
 * \brief LocationItem::LocationItem
 * \param name String name of the room
 * \param iRoom int value to identify the room to the LinuxMCE DCERouter
 * \param ea the int value of the ea
 * \param ea_name The string name of the current EA
 * \param roomType They Type of room
 * \param rmimg Image: url not image data
 * \param hideFromOrbiter - if the room should be hidden.
 * \param parent
 *
 *
 */
LocationItem::LocationItem(QString &name, int &intRoom, int &roomType, QUrl &rmimg, bool isHidden, QObject *parent) :
    m_name(name), m_val(intRoom), m_iType(roomType), roomImage(rmimg), hideRoom(isHidden)
{
}

/*!
 * \brief LocationItem::roleNames
 * \return Hash of the rolenames as used in the qml Application by the listmodel
 *
 */
QHash<int, QByteArray> LocationItem::roleNames() const
{

  QHash<int, QByteArray> names; 
  names[NameRole] = "name"; 
  names[intRole] = "intRoom"; 
  names[TypeRole] = "room_type";
  names[ImageRole] = "room_image";
  names[EaListRole] = "ea_list";
  names[HideRole] = "hideFromOrbiter";
  return names;
}

/*!
 * \brief LocationItem::data
 * \param role
 * \return the value correlating to the role name.
 */
QVariant LocationItem::data(int role) const
{
  switch(role) {
  case NameRole:
    return id();
  case intRole:
    return roomVal();
  case TypeRole:
      return room_type();
  case ImageRole:
      return room_image();
  case EaListRole:
      return entertainAreas();
  case HideRole:
      return hideFromOrbiter();
  default:
    return QVariant();
  }
}
