/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/
#include "BufferedIOConnection.h"

using namespace std;

#define CACHE_READBLOCK_SIZE	128
#define DEFAULT_DELIMITED_TIMEOUT 3000
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

namespace DCE {

BufferedIOConnection::BufferedIOConnection(IOConnection* pconn)
	: pconn_(pconn)
{
}


BufferedIOConnection::~BufferedIOConnection()
{
}

int 
BufferedIOConnection::RecvRaw(char* buff, unsigned int size, int timeout) {
	return (pconn_ ? pconn_->Recv(buff, size, timeout) : -1);
}

int 
BufferedIOConnection::Recv(char* buff, unsigned int size, int timeout) {
	streamsize sz = buff_.sgetn(buff, size);
	if(sz == 0) {
		return RecvRaw(buff, size, timeout);
	} else {
		if(sz == (streamsize)size) {
			return sz;
		} else {
			return sz + RecvRaw(buff + sz, size - sz, timeout);
		}
	}
}

void 
BufferedIOConnection::UndoRecv(const char* buff, unsigned int size) {
	buff_.sputn(buff, size);
}

int 
BufferedIOConnection::RecvCache(unsigned int size, int timeout) {
	char buff[size];
	int recvsize = RecvRaw(buff, size, timeout);
	if(recvsize > 0) {
		UndoRecv(buff, recvsize);
	}
	return recvsize;
}

unsigned int 
BufferedIOConnection::CacheSize() {
	return buff_.in_avail();
}


std::string
BufferedIOConnection::RecvDelimited(const char* delimbuff, unsigned int delimsize, int timeout) 
{
	string ret;
	struct timeval first;
	struct timeval last;

	if(delimsize <= 0 || !isDataAvailable(timeout)) {
		return string(""); //ret;
	}
	if(timeout<=0)
	{
		timeout=DEFAULT_DELIMITED_TIMEOUT;
	}
	char c=0;	
	unsigned delimindex = 0;
	int rest_timeout=timeout;
	gettimeofday(&first, NULL);

	while(delimindex < delimsize) {
		int rsize = Recv(&c, sizeof(char), rest_timeout);
		if(rsize > 0) {
			if(c == delimbuff[delimindex]) {
				delimindex++;
			} else {
				delimindex = 0;
			}
			ret += c;
		}
		gettimeofday(&last, NULL);
		rest_timeout = timeout-((last.tv_sec - first.tv_sec)*1000+(last.tv_usec - first.tv_usec)/1000);
		if(rest_timeout <= 0)
		{
			fprintf(stderr,"Timeout in RecvDelimited : received so far '%s', but will return empty\n",ret.c_str());
			return string("");
		}
	}
	fprintf(stderr,"RecvDelimited returns '%s'\n",ret.c_str());	
	return ret;
}

};
