/**
 * Roku Device implmentation for port 8080 telnet socket.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 *
 */

#ifndef ROKUDEVICE_H
#define ROKUDEVICE_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <netdb.h>

using namespace std;

namespace DCE
{

  class RokuDevice
  {
  private:
    string m_sIpAddress;
    int m_iSockFd;
    struct sockaddr_in m_server;
    struct addrinfo *m_pServerInfo;
  protected:
  public:
    RokuDevice(string sIPAddress);
    ~RokuDevice();
    bool GetConfig();
    bool SendToRoku(string sCommand);
  };

}

#endif
