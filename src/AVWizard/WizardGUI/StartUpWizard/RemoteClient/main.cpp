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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <iostream>
#include <string>

#define PORT 28949 // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

std::string GetSocketText(int Socket)
{
	int Len;
	recv(Socket, &Len, sizeof(Len), 0);
	char* Buffer = (char*) malloc(Len+1);
	Buffer[Len] = 0;
	recv(Socket, Buffer, Len, 0);
	free(Buffer);
	std::string Result = Buffer;
	return Buffer;	
}

void SendSocketText(int Socket, std::string Text)
{
	int Len = Text.length();
	if(write(Socket, &Len, sizeof(Len))<0)
	{
		std::cout<<"Error on write"<<std::endl;
		exit(2);
	}
	char* Buffer = (char*) malloc(Len+1);
	strcpy(Buffer, Text.c_str());
	std::cout<<"<"<<Text<<">";
	write(Socket, Buffer, Len);
	free(Buffer);
}

int main(int argc, char *argv[])
{
	int sockfd;
	struct hostent *he;
	struct sockaddr_in their_addr; // connector's address information 

	if(argc < 2)
	{
		std::cout<<"Error on remote command."<<std::endl;
		std::cout<<"AVWizardRemote [left|right|top|bottom|return|escape|plus|minus]"<<std::endl;
		exit(2);
	}
	std::string Command = argv[1];
	std::cout<<Command<<std::endl;

	if ((he=gethostbyname("127.0.0.1")) == NULL) {  // get the host info 
	    perror("gethostbyname");
	    exit(1);
	}

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	    perror("socket");
	    exit(1);
	}

	bzero(&their_addr, sizeof(their_addr));
	their_addr.sin_family = AF_INET;    // host byte order 
	their_addr.sin_port = htons(PORT);  // short, network byte order 
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);

	if (connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1) {
	    perror("connect");
	    exit(1);
	}

	std::string Text = GetSocketText(sockfd);
	std::cout<<Text<<std::endl;
	SendSocketText(sockfd, Command);
	close(sockfd);

	return 0;
}
