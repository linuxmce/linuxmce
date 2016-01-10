/***************************************************************************
 *   Copyright (C) 2005 by igor                                            *
 *   igor@dexx                                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "proxypeerthread.h"

#include <iostream>
#include <sys/socket.h>

#include "DCE/Logger.h"

using namespace std;
using namespace DCE;

namespace MYTHTV {

	ProxyPeerThread::ProxyPeerThread(ProxyServer *pserver, int srcsockfd, int destsockfd)
		: ProxyPeer(srcsockfd, destsockfd), pserver_(pserver)
	{
	}

	ProxyPeerThread::~ProxyPeerThread()
	{

	}

	void ProxyPeerThread::handleTerminate()
	{
		close(getSrcSock());
		close(getDestSock());
	}

	void* ProxyPeerThread::_Run()
	{
		fd_set rfds;
		int maxfd =
			((getSrcSock() > getDestSock()) ? getSrcSock() : getDestSock()) + 1;

		while( ! isStopRequested() )
		{
			//sleep(1);
			// wait for data to be available on sockets
			FD_ZERO(&rfds);
			FD_SET(getSrcSock(), &rfds); FD_SET(getDestSock(), &rfds);

			struct timeval tv;
			tv.tv_sec = 0;
			tv.tv_usec = 500000;

			int retval = select(maxfd, &rfds, NULL, NULL, &tv);

			if(retval <= 0) {
	//			cout << "No Traffic on Peers. Waiting..." << endl;
				continue;
			}

			if(FD_ISSET(getSrcSock(), &rfds)) {
	//			cout << "Performing Source --> Destination." << endl;
				if(replData(true) < 0) {
					// cout << "Source disconnected" << endl;
					LoggerWrapper::GetInstance()->Write(LV_STATUS, "Source disconnected");
					break;
				}
			}

			if(FD_ISSET(getDestSock(), &rfds)) {
	//			cout << "Performing Destination --> Source." << endl;
				if(replData(false) < 0) {
	//				cout << "Destination disconnected" << endl;
					LoggerWrapper::GetInstance()->Write(LV_STATUS, "Destination disconnected");
					break;
				}
			}
		}

		return 0;
	}

	#define SIZEBUFF_SIZE	8
	#define DATABUFF_SIZE	1024

	int ProxyPeerThread::replData(bool fromsrc)
	{
		/* read fromsockfd */
		int
			fromsockfd =  (fromsrc) ? getSrcSock() : getDestSock(),
			tosockfd =  (fromsrc) ? getDestSock() : getSrcSock();

		string datastr;
		if(readData(fromsockfd, datastr) < 0) {
			return -1;
		}

	//	cout << "Received data: " << endl << datastr << endl;
	// 	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Received data: %s", datastr.c_str());

		/*process data with interceptors*/
		bool processed = false;
		if(processData(datastr.c_str(), fromsrc))
		{
			processed = true;
		}

		if(!processed)
		{
			/* sending data */
	//		cout << "Redirecting." << endl ;
			if(writeData(tosockfd, datastr) < 0) {
				return -1;
			}
		}

		return 0;
	}

	int ProxyPeerThread::readData(int sockfd, std::string& data)
	{
		/* read data size */
		char sizebuff[SIZEBUFF_SIZE + 1];

		int recvsize = recv(sockfd, sizebuff, SIZEBUFF_SIZE, 0);
		if(recvsize <= 0) {
			return -1;
		}

		string sizestr(sizebuff, recvsize);
	//	cout << "Received size: " << sizestr << endl;

		/*TO DO: place trimming here*/
		int size = atoi(sizestr.c_str());
	//	cout << "Expecting to read a string of " << size << " bytes" << endl;

		/* read data */

		int read = 0;
		unsigned int zerocnt = 0;

		char databuff[DATABUFF_SIZE];
		while (size > 0)
		{
			recvsize = recv(sockfd, databuff, (size < DATABUFF_SIZE) ? size : DATABUFF_SIZE, 0);

			read += recvsize;
			size -= recvsize;
			if (recvsize == 0)
			{
				if (++zerocnt >= 100) {
	//				cout << "Abandon waiting data." << endl;
					break;
				}
				usleep(50);

				if (zerocnt == 5) {
	//				cout << "Waiting for data: " << read << " " << size << endl;
				}
			} else {
				data.append(databuff, recvsize);
			}
		}

		return 0;
	}

	int ProxyPeerThread::writeData(int sockfd, const std::string& data)
	{
		char tmpbuff[12];
		sprintf(tmpbuff, "%d", data.length());

		string sizestr = tmpbuff;
		sizestr.resize(SIZEBUFF_SIZE, '\t');

		string outstr = sizestr + data;
	//	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sending data: %s", outstr.c_str());
	//	cout << "Sending data: " << outstr << endl;
		if((send(sockfd, outstr.c_str(), outstr.length(), 0) <= 0)) {
		//	cout << "Error sending data to destination." << endl;
			return -1;
		}

		return 0;
	}
};
