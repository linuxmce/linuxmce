#include <iostream>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <cstring>

#include "omxplayer-client.h"

using namespace std;

OMXPlayerClient::OMXPlayerClient(DBus::Connection &connection, const char *path, const char *name)
  : DBus::ObjectProxy(connection, path, name)
{

}

OMXPropsClient::OMXPropsClient(DBus::Connection &connection, const char *path, const char *name)
  : DBus::ObjectProxy(connection, path, name)
{

}

