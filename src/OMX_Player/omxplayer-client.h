#ifndef __PLAYER_CLIENT_H
#define __PLAYER_CLIENT_H

#include <dbus-c++/dbus.h>
#include "omxplayer-client-glue.h"

class OMXPlayerClient
  : public org::mpris::MediaPlayer2::Player_proxy,
  public DBus::IntrospectableProxy,
  public DBus::PropertiesProxy,
  public DBus::ObjectProxy
{
public:

  OMXPlayerClient(DBus::Connection &connection, const char *path, const char *name);

};

class OMXPropsClient
  : public org::freedesktop::DBus::Properties_proxy,
  public DBus::IntrospectableProxy,
  public DBus::PropertiesProxy,
  public DBus::ObjectProxy
{
public:

  OMXPropsClient(DBus::Connection &connection, const char *path, const char *name);

};

#endif//__PLAYER_CLIENT_H




