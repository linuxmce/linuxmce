#include "ProxyOrbiter.h"

#define MAXBUFF 1024


struct PollData
{
	const char *ServerIP;
	int ServerPort;
	struct MessageQueue *hWnd;
};

pthread_t ProxyOrbiterThreadHandle;

extern pthread_mutex_t comm_mutex;


static void ReadTextLine(int Socket, char *buffer, int length)
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

static void ReadBinaryData(int Socket, unsigned char *buffer, int length)
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

int GetNews(int Socket)
{
	char buffer[MAXBUFF];

	if (Socket == 0) return SOCKET_ERROR;

	memset(buffer, 0, MAXBUFF);
	snprintf( buffer, MAXBUFF - 1, "%s\n", "ANYNEWS?");
	if (send(Socket, buffer, strlen(buffer), 0) == SOCKET_ERROR)
		return SOCKET_ERROR;

	ReadTextLine(Socket, buffer, MAXBUFF); // NEWS length
	ReadTextLine(Socket, buffer, MAXBUFF); // yes/no

	if (strstr(buffer, "yes") == buffer)
		return TRUE;
	else
		return FALSE;
}

char ProxyDone = FALSE;

void *PollThreadFunc(void *lpParameter)
{
	struct PollData *Data = (struct PollData *) lpParameter;
	struct MessageQueue *hwnd = Data->hWnd;

	while (ProxyDone==FALSE)
	{
		pthread_mutex_lock(&comm_mutex);
		int Result = GetNews(hwnd->CommSocket);
		pthread_mutex_unlock(&comm_mutex);

		if (Result == TRUE)
		{
#ifdef DEBUG
			fprintf(stdout,"PoolNews: Received\n");
#endif
			pthread_mutex_lock(&hwnd->mu_queue);
			hwnd->msg_queue=1;
			pthread_mutex_unlock(&hwnd->mu_queue);
			pthread_cond_signal(&hwnd->cond);
		}
		else if (Result == SOCKET_ERROR)
		{
#ifdef DEBUG
			fprintf(stdout,"PoolNews: SocketError\n");
#endif
			pthread_mutex_lock(&hwnd->mu_queue);
			hwnd->msg_queue=2;
			pthread_mutex_unlock(&hwnd->mu_queue);
			pthread_cond_signal(&hwnd->cond);
		}
		usleep(100000);
	}

	return 0;
}


void ProxyOrbiter_Stop(void)
{
	ProxyDone=TRUE;
	pthread_join(ProxyOrbiterThreadHandle,NULL);
}


int ProxyOrbiter_Connect(const char *ServerIP, int Port)
{
	int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	int err = 0;

	struct sockaddr_in saddr;
	saddr.sin_family = PF_INET;
	saddr.sin_port = htons(Port);
	saddr.sin_addr.s_addr = inet_addr(ServerIP);
	err = connect(s, (struct sockaddr *) &saddr, sizeof(struct sockaddr_in));
	if (err != 0)
	{
		fprintf(stderr,"Connection failed: %s:%d. Error %d\n", ServerIP,Port,err);
		return 0;
	}
	return s;
}

char ProxyOrbiter_GetImage(int Socket, unsigned char **bufferbin, unsigned long int *length)
{
	if (bufferbin == NULL || *bufferbin != NULL || length == NULL || Socket == 0)
		return FALSE;
	
	char buffer[MAXBUFF];
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

void ProxyOrbiter_Touch(int Socket, int X, int Y)
{
	char buffer[MAXBUFF];
	
	if (Socket == 0) return;

	memset(buffer, 0, MAXBUFF);
	snprintf(buffer, MAXBUFF - 1, "%s %dx%d\n", "TOUCH", X, Y);
	if (send(Socket, buffer, strlen(buffer), 0) == SOCKET_ERROR) return;
	ReadTextLine(Socket, buffer, MAXBUFF); // OK
}

void ProxyOrbiter_PollNews(const char *ServerIP, int ServerPort, struct MessageQueue *hWnd)
{
	struct PollData *Data = (struct PollData *) malloc(sizeof(struct PollData));
	Data->ServerIP = strdup(ServerIP);
	Data->ServerPort = ServerPort;
	Data->hWnd = hWnd;

	// create and start thread 
	if ( pthread_create( &ProxyOrbiterThreadHandle, NULL, PollThreadFunc, Data) ) 
	{
  		fprintf(stderr,"ProxyOrbiter... error creating PollNews thread\n");
		exit(0);
	}
}
