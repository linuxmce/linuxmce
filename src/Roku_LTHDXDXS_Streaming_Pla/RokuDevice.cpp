/**
 * Roku Device implmentation for port 8080 telnet socket.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 *
 */

#include "RokuDevice.h"
#include "DCE/Logger.h"

namespace DCE
{

  RokuDevice::RokuDevice(string sIPAddress)
  {
    m_sIpAddress=sIPAddress;
    m_iSockFd=0;
  }

  RokuDevice::~RokuDevice()
  {
    m_sIpAddress="";
    m_iSockFd=0;
  }

  bool RokuDevice::GetConfig()
  {

    if (0 != getaddrinfo(m_sIpAddress.c_str(), NULL, NULL, &m_pServerInfo)) 
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not get addrinfo for %s",m_sIpAddress.c_str());
	return false;
      }
    
    memcpy(&m_server, m_pServerInfo->ai_addr, sizeof(struct sockaddr_in));
    m_server.sin_family = AF_INET;
    m_server.sin_port = htons(8060);
    freeaddrinfo(m_pServerInfo);
    // At this point, the socket is ready for connect.

    return true;

  }

  bool RokuDevice::SendToRoku(string sCommand)
  {

    // Open the socket, send the command, and
    // immediately close the socket.
    
    m_iSockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == m_iSockFd) {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not create socket.");
      return false;
    }

    if (-1 == connect(m_iSockFd, (const struct sockaddr *) &m_server,
		      sizeof(struct sockaddr_in))) 
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could Not Connect");
	// Could not connect. 
	return false;
      }

    if (-1 == send(m_iSockFd, sCommand.c_str(), (size_t) strlen(sCommand.c_str()) + 1, 0)) {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not send %s",sCommand.c_str());
      return false;
    }

    usleep(2000);

    char buf[8192];
    
    if (-1 == recv(m_iSockFd, buf, sizeof(buf), 0))
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not receive Response after sending data.");
	return false;
      }
    else
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Received data %s",buf);
      }

    close(m_iSockFd);

    return true;

  }

}
