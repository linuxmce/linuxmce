#ifndef CUSTOM_H
#define CUSTOM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <malloc.h>


#define MAX_PATH FILENAME_MAX

#define NO_GRAPHICS
#define DEBUG

struct MessageQueue
{
	int CommSocket;
	int msg_queue;
	pthread_mutex_t mu_queue;
	pthread_cond_t cond;
};


#define RGB_COLOR_GREY		0xC0C0C0
#define RGB_COLOR_BROWN		0xA62A2A
#define RGB_COLOR_WHITE		0xFFFFFF
#define RGB_COLOR_RED		0xFF0000
#define RGB_COLOR_GREEN		0x00FF00
#define RGB_COLOR_BLUE		0x0000FF
#define RGB_COLOR_MAGNETA	0xFF00FF
#define RGB_COLOR_CYAN		0x00FFFF
#define RGB_COLOR_YELLOW	0xFFFF00
#define RGB_COLOR_BLACK		0x000000

#define SOCKET_ERROR	(-1)

#define TRUE	1
#define FALSE	0

#endif /*CUSTOM_H*/
