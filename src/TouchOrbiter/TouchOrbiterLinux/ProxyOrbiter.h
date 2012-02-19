#ifndef PROXYORBITER_H
#define PROXYORBITER_H

#include "custom.h"

int ProxyOrbiter_Connect(const char *ServerIP, int Port);
char ProxyOrbiter_GetImage(int Socket, unsigned char **bufferbin, unsigned long int *length);
void ProxyOrbiter_PollNews(const char *ServerIP, int ServerPort, struct MessageQueue *hWnd);
void ProxyOrbiter_Touch(int Socket, int X, int Y);
void ProxyOrbiter_Stop(void);

#endif /*PROXYORBITER_H*/
