#include "useritem.h"

UserItem::UserItem( QString &userName ,  QString &firstName,  QString &lastName,  QString &nickName,   int &iPK_User,  int &sUserMode,  int &pinRequired,  int &phoneExtension ,  QImage &img,  int &defaultUser ,QObject *parent = 0):
    m_userName(userName), m_firstName(firstName), m_lastName(lastName), m_nickName(nickName), m_pk_user(iPK_User), m_userMode(sUserMode), m_requirePin(pinRequired), m_phoneExtension(phoneExtension), m_image(img), m_defaultUser(defaultUser)
{
}


QHash<int, QByteArray> UserItem::roleNames() const
{
  QHash<int, QByteArray> names;
  names[NameRole] = "username";
  names[FirstNameRole] = "firstName";
  names[LastNamRole] = "lastName";
  names[PKUserRole] = "pkUser";
  names[UserModeRole] = "userMode";
  names[RequirePinRole] = "pinRequired";
  names[PhoneExtensionRole] = "phoneExtension";
  names[ImageRole] = "image";
  names[CompatRole] = "name";
  names[DefaultRole] = "default";
  return names;
}

QVariant UserItem::data(int role) const
{
  switch(role) {
  case NameRole:
    return id();
  case FirstNameRole:
    return first_name();
  case LastNamRole:
    return last_name();
  case PKUserRole:
    return pk_user_get();
  case UserModeRole:
    return user_mode();
  case RequirePinRole:
    return require_pin();
  case PhoneExtensionRole:
    return phone_ext();
  case ImageRole:
    return image_user();
  case DefaultRole:
    return default_user();
  case CompatRole:
      return pk_user_get();
  default:
    return QVariant();
  }
}
