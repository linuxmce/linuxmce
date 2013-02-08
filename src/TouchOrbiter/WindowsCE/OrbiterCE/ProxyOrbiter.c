#include "ProxyOrbiter.h"
#include <windows.h>
#include <string.h>

#define MAXBUFF 1024

static struct PollData
{
	const unsigned char *ServerIP;
	int ServerPort;
	HWND hWnd;
	unsigned int wMsg;
};

static void ReadTextLine(SOCKET Socket, unsigned char *buffer, int length)
{
	int bytes;
	int buffer_idx = 0;

	while ((bytes = recv(Socket, &buffer[buffer_idx], 1, 0)) > 0)
	{
		buffer_idx++;
		if (buffer[buffer_idx - 1] == '\n')
			break;
		if (buffer_idx == length - 1)
			break;
	}

	buffer[buffer_idx] = 0;
}

static void ReadBinaryData(SOCKET Socket, unsigned char *buffer, int length)
{
	int bytes;
	int buffer_idx = 0;

	while ((bytes = recv(Socket, &buffer[buffer_idx], length - buffer_idx, 0)) > 0)
	{
		buffer_idx += bytes;
		if (buffer_idx >= length)
			break;
	}
}

static int GetNews(SOCKET Socket)
{
	unsigned char buffer[MAXBUFF];

	memset(buffer, 0, MAXBUFF);
	snprintf(buffer, MAXBUFF - 1, "%s\n", "ANYNEWS?");
	if (send(Socket, buffer, strlen(buffer), 0) == SOCKET_ERROR)
		return SOCKET_ERROR;

	ReadTextLine(Socket, buffer, MAXBUFF); // NEWS length
	ReadTextLine(Socket, buffer, MAXBUFF); // yes/no

	if (strstr(buffer, "yes") == buffer)
		return TRUE;
	else
		return FALSE;
}

static DWORD PollThreadFunc(LPVOID lpParameter)
{
	struct PollData *Data = (struct PollData *) lpParameter;

	SOCKET Socket = ProxyOrbiter_Connect(Data->ServerIP, Data->ServerPort);

	BOOL done = FALSE;
	while (!done)
	{
		int Result = GetNews(Socket);
		if (Result == TRUE)
		{
			PostMessage(Data->hWnd, Data->wMsg, 0, 1);
		}
		else if (Result == SOCKET_ERROR)
		{
			close(Socket);
			PostMessage(Data->hWnd, Data->wMsg, 0, 2);
			Socket = ProxyOrbiter_Connect(Data->ServerIP, Data->ServerPort);
		}
		Sleep(1000);
	}

	return 0;
}

SOCKET ProxyOrbiter_Connect(const unsigned char *ServerIP, int Port)
{
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct sockaddr_in saddr;
	saddr.sin_family = PF_INET;
	saddr.sin_port = htons(Port);
	saddr.sin_addr.s_addr = inet_addr(ServerIP);

	connect(s, (struct sockaddr *) &saddr, sizeof(struct sockaddr_in));

	return s;
}

BOOL ProxyOrbiter_GetImage(SOCKET Socket, unsigned char **bufferbin, unsigned long int *length)
{
	if (bufferbin == NULL || *bufferbin != NULL || length == NULL || Socket == INVALID_SOCKET)
		return FALSE;
	
	unsigned char buffer[MAXBUFF];
	unsigned char response[MAXBUFF];

	memset(buffer, 0, MAXBUFF);
	snprintf(buffer, MAXBUFF - 1, "%s\n", "IMAGE");
	if (send(Socket, buffer, strlen(buffer), 0) == SOCKET_ERROR)
		return FALSE;

	ReadTextLine(Socket, buffer, MAXBUFF); // IMAGE length
	sscanf(buffer, "%s %lu", response, length);
	if (*length <= 0)
		return FALSE;

	*bufferbin = (unsigned char *) malloc(*length * sizeof(unsigned char));
	ReadBinaryData(Socket, *bufferbin, *length);
	
	return TRUE;
}

void ProxyOrbiter_Touch(SOCKET Socket, int X, int Y)
{
	unsigned char buffer[MAXBUFF];

	memset(buffer, 0, MAXBUFF);
	snprintf(buffer, MAXBUFF - 1, "%s %dx%d\n", "TOUCH", X, Y);
	send(Socket, buffer, strlen(buffer), 0);
	ReadTextLine(Socket, buffer, MAXBUFF); // OK
}

void ProxyOrbiter_PollNews(const char *ServerIP, int ServerPort, HWND hWnd, unsigned int wMsg)
{
	struct PollData *Data = (struct PollData *) malloc(sizeof(struct PollData));
	Data->ServerIP = strdup(ServerIP);
	Data->ServerPort = ServerPort;
	Data->hWnd = hWnd;
	Data->wMsg = wMsg;

	HANDLE ThreadHandle;
	if ((ThreadHandle = CreateThread(NULL, 0, PollThreadFunc, Data, 0, NULL)) != NULL)
		CloseHandle(ThreadHandle);
}
