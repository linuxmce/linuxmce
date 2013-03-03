#include <datamodels/locationitem.h>
/*!
 * \brief LocationItem::LocationItem
 * \param name String name of the room
 * \param iRoom int value to identify the room to the LinuxMCE DCERouter
 * \param ea the int value of the ea
 * \param ea_name The string name of the current EA
 * \param roomType They Type of room
 * \param rmimg Image: url not image data
 * \param parent
 *
 *
 */
LocationItem::LocationItem(const QString &name, const int &intRoom,  const int &ea, const QString &ea_name, const int &roomType, const QUrl &rmimg, QObject *parent) :
    m_name(name), m_val(intRoom), m_iEA(ea), m_eaname(ea_name), m_iType(roomType), roomImage(rmimg)
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
  names[EaRole] = "entertain_area";
  names[intRole] = "intRoom"; 
  names[TypeRole] = "room_type";
  names[ImageRole] = "room_image";
  names[EaNameRole] = "ea_name";
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
