/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

#include "PlutoUtils/CommonIncludes.h"
#include <iostream>

#define HELLO_PORT 1900
#define HELLO_GROUP "239.255.255.250"
#define MSGBUFSIZE 256

#include "DCE/DCEConfig.h"
DCEConfig g_DCEConfig;

#ifdef WIN32
int _tmain(int argc, _TCHAR* argv[])
{
	int err;
	WSADATA wsaData;
	err = WSAStartup(MAKEWORD( 1, 1 ),(LPWSADATA)  &wsaData);
#else
int main(int argc, char *argv[])
{
#endif
	struct sockaddr_in addr;
	int fd, nbytes;
	socklen_t addrlen;
	struct ip_mreq mreq;
	char msgbuf[MSGBUFSIZE];

	u_int yes=1;            /*** MODIFICATION TO ORIGINAL */

	/* create what looks like an ordinary UDP socket */
	if ((fd=(int) socket(AF_INET,SOCK_DGRAM,0)) < 0) {
		perror("socket");
		exit(1);
	}


	/**** MODIFICATION TO ORIGINAL */
	/* allow multiple sockets to use the same PORT number */
	if (setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,(SOCKOPTTYPE) &yes,sizeof(yes)) < 0) {
		perror("Reusing ADDR failed");
		exit(1);
	}
	/*** END OF MODIFICATION TO ORIGINAL */

	/* set up destination address */
	memset(&addr,0,sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=htonl(INADDR_ANY); /* N.B.: differs from sender */
	addr.sin_port=htons(HELLO_PORT);

	/* bind to receive address */
	if (bind(fd,(struct sockaddr *) &addr,sizeof(addr)) < 0) {
		perror("bind");
		exit(1);
	}

	/* use setsockopt() to request that the kernel join a multicast group */
	mreq.imr_multiaddr.s_addr=inet_addr(HELLO_GROUP);
	mreq.imr_interface.s_addr=htonl(INADDR_ANY);
	if (setsockopt(fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,(SOCKOPTTYPE) &mreq,sizeof(mreq)) < 0) {
		perror("setsockopt");
		exit(1);
	}

	string sHillcrestUUID = g_DCEConfig.m_mapParameters_Find("Hillcrest_uuid");
	if( sHillcrestUUID.empty() )
		sHillcrestUUID = "8996cb1a-5f75-11dd-a2eb-00e0817409f9";

	string sHillcrestVersion = g_DCEConfig.m_mapParameters_Find("Hillcrest_version");
	if( sHillcrestVersion.empty() )
		sHillcrestVersion = "1.0.109.22";

	string sName = "MediaServer_" + g_DCEConfig.m_mapParameters_Find("PK_Installation");

	char cBuffer[2000];
	sprintf(cBuffer,"HTTP/1.1 200 OK\r\n"
		"Location: http://192.168.0.101:9876/hillcrest/descriptor.xml\r\n"
		"Ext: \r\n"
		"USN: uuid:%s::urn:hcrest-com:service:X_RaptorIPCService:1\r\n"
		"SERVER: Hillcrest AV Server/%s-%s UPnP/1.0\r\n"
		"CACHE-CONTROL: max-age=600\r\n"
		"ST: urn:hcrest-com:service:X_RaptorIPCService:1\r\n"
		"Content-Length: 0\r\n"
		"Name: %s\r\n"
		"Version: %s\r\n"
		"HC_SCHEMA: 22\r\n",sHillcrestUUID.c_str(), sHillcrestVersion.c_str(), sName.c_str(),  sName.c_str(), sHillcrestVersion.c_str() );

	printf("Entering Loop\n");

	/* now just enter a read-print loop */
	while (1) 
	{
		addrlen=sizeof(addr);
		if ((nbytes=recvfrom(fd,msgbuf,MSGBUFSIZE,0,
			(struct sockaddr *) &addr,&addrlen)) < 0) {
				perror("recvfrom");
				exit(1);
			}

			int i = sendto(fd,(const char *) cBuffer, (int) strlen(cBuffer),0,(struct sockaddr *) &addr, sizeof(addr));
			printf("Sent (%d)\n",i);
			//printf("Sent (%d) to %d.%d.%d.%d\n", 
			//	i, (int) addr.sin_addr.S_un.S_un_b.s_b1,(int) addr.sin_addr.S_un.S_un_b.s_b2,(int) addr.sin_addr.S_un.S_un_b.s_b3,(int) addr.sin_addr.S_un.S_un_b.s_b4);
	}


#ifdef _WIN32
	WSACleanup();
#endif
	return 0;
}
