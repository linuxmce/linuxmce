#ifndef USERITEM_H
#define USERITEM_H

#include <datamodels/listModel.h>
#include <QImage>

class UserItem : public ListItem
{
    Q_OBJECT

public:
  enum Roles {
    NameRole = Qt::UserRole+1, // username
    FirstNameRole,
    LastNamRole,
    NickNameRole,
    PKUserRole,
    UserModeRole,
    RequirePinRole,
    PhoneExtensionRole,
    ImageRole,
    DefaultRole

  };

public:
  UserItem(QObject *parent = 0): ListItem(parent) {}
  explicit UserItem( QString &userName ,  QString &firstName,  QString &lastName,  QString &nickName,   int &iPK_User,  int &sUserMode,  int &pinRequired,  int &phoneExtension ,  QImage &img,  int &defaultUser, QObject *parent);

  QVariant data(int role) const;
  QHash<int, QByteArray> roleNames() const;

  void setName(QString name);
  inline QString id() const { return m_userName; }
  inline QString first_name() const { return m_firstName; }
  inline QString last_name() const { return m_lastName; }
  inline QString nick_name() const { return m_nickName; }
  inline int pk_user_get() const { return m_pk_user; }
  inline int user_mode() const { return m_userMode; }
  inline int require_pin() const { return m_requirePin; }
  inline int phone_ext() const { return m_phoneExtension; }
  inline QImage image_user() const {return m_image;}
  inline int default_user() const {return m_defaultUser;}

private:
  QString m_userName;       //username
  QString m_firstName;
  QString m_lastName;
  QString m_nickName;
  int m_pk_user;
  int m_userMode;
  int m_requirePin;
  int m_phoneExtension;
  int m_defaultUser;
  QImage m_image;


signals:
  void imageChanged();

};

#endif // USERITEM_H
