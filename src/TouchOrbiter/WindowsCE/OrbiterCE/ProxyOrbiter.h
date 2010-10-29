#ifndef PROXYORBITER_H
#define PROXYORBITER_H

#include <winsock2.h>

SOCKET ProxyOrbiter_Connect(const unsigned char *ServerIP, int Port);
BOOL ProxyOrbiter_GetImage(SOCKET Socket, unsigned char **bufferbin, unsigned long int *length);
void ProxyOrbiter_PollNews(const char *ServerIP, int ServerPort, HWND hWnd, unsigned int wMsg);
void ProxyOrbiter_Touch(SOCKET Socket, int X, int Y);

#endif /*PROXYORBITER_H*/
