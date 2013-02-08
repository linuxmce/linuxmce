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
//
// C++ Implementation: SocketRemoteClient
//
// Description: 
//
//
// Author: Ciprian Mustiata <ciprian.m@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "SocketRemoteClient.h"

#include <iostream>

#include "SafetyLock.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <netinet/in.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>

#include "Wizard.h"
#include <SDL.h>

SocketRemoteClient::SocketRemoteClient(SocketRemoteServer* Server, int Socket)
{
	this->Socket = Socket;
	this->Server = Server;

	std::cout<<"Create remote client! "<<std::endl;

	pthread_mutex_init(&SafeMutex, NULL);
	pthread_create(&ClientThread, NULL, SocketClientCallBack, this);
}

SocketRemoteClient::~SocketRemoteClient()
{
	pthread_mutex_destroy(&SafeMutex);
}

std::string SocketRemoteClient::ReceiveString()
{
	SafetyLock Lock(&SafeMutex);
	std::string Message="";
	int Len = 0;
	if(recv(Socket, &Len, sizeof(Len), 0) < 0)
	{
		close(Socket);
		Socket = 0;
		return Message;
	}

	if(Len <= 0 || Len > 1024)
	{
		close(Socket);
		Socket = 0;
		return Message;
	}

	char* Buffer = (char*) malloc(Len+1);
	Buffer[Len] = 0;
	if(recv(Socket, Buffer, Len, 0)<0)
	{
		close(Socket);
		Socket = 0;
		free(Buffer);
		return Message;
	}
	Message = Buffer;
	free(Buffer);

	return Message;
}

void SocketRemoteClient::SendString(std::string Message)
{
	SafetyLock Lock(&SafeMutex);
	int Len = Message.length();
	write(Socket, &Len, sizeof(Len));
	write(Socket, Message.c_str(), Len);
}

void SocketRemoteClient::Disconnect()
{
	SafetyLock Lock(&SafeMutex);
	close(Socket);
}

void* SocketClientCallBack(void* SocketClientCallBackPtr)
{
	SocketRemoteClient* Client = (SocketRemoteClient*)SocketClientCallBackPtr;
	std::string Message = "OPEN";
	Client->SendString(Message);
	
	SDL_Event Event;
	std::cout<<"Start getting information from client"<<std::endl;
	Message = Client->ReceiveString();
	Client->Disconnect();
	std::cout<<"Client disconnected!"<<std::endl;

	if(Message == "left")
	{
		Event.type = SDL_KEYUP;
		Event.key.state = SDL_RELEASED;
		Event.key.keysym.sym = SDLK_LEFT;
	}
	if(Message == "right")
	{
		Event.type = SDL_KEYUP;
		Event.key.state = SDL_RELEASED;
		Event.key.keysym.sym = SDLK_RIGHT;
	}
	if(Message == "up")
	{
		Event.type = SDL_KEYUP;
		Event.key.state = SDL_RELEASED;
		Event.key.keysym.sym = SDLK_UP;
	}
	if(Message == "down")
	{
		Event.type = SDL_KEYUP;
		Event.key.state = SDL_RELEASED;
		Event.key.keysym.sym = SDLK_DOWN;
	}
	if(Message == "return")
	{
		Event.type = SDL_KEYUP;
		Event.key.state = SDL_RELEASED;
		Event.key.keysym.sym = SDLK_RETURN;
	}
	if(Message == "escape")
	{
		Event.type = SDL_KEYUP;
		Event.key.state = SDL_RELEASED;
		Event.key.keysym.sym = SDLK_ESCAPE;
	}
	if(Message == "plus")
	{
		Event.type = SDL_KEYUP;
		Event.key.state = SDL_RELEASED;
		Event.key.keysym.sym = SDLK_PLUS;
	}
	if(Message == "minus")
	{
		Event.type = SDL_KEYUP;
		Event.key.state = SDL_RELEASED;
		Event.key.keysym.sym = SDLK_MINUS;
	}
	if(Message == "0") {
		Event.type = SDL_KEYUP;
		Event.key.state = SDL_RELEASED;
		Event.key.keysym.sym = SDLK_0;
	}
	if(Message == "1") {
		Event.type = SDL_KEYUP;
		Event.key.state = SDL_RELEASED;
		Event.key.keysym.sym = SDLK_1;
	}
	if(Message == "2") {
		Event.type = SDL_KEYUP;
		Event.key.state = SDL_RELEASED;
		Event.key.keysym.sym = SDLK_2;
	}
	if(Message == "3") {
		Event.type = SDL_KEYUP;
		Event.key.state = SDL_RELEASED;
		Event.key.keysym.sym = SDLK_3;
	}
	if(Message == "4") {
		Event.type = SDL_KEYUP;
		Event.key.state = SDL_RELEASED;
		Event.key.keysym.sym = SDLK_4;
	}
	if(Message == "5") {
		Event.type = SDL_KEYUP;
		Event.key.state = SDL_RELEASED;
		Event.key.keysym.sym = SDLK_5;
	}
	if(Message == "6") {
		Event.type = SDL_KEYUP;
		Event.key.state = SDL_RELEASED;
		Event.key.keysym.sym = SDLK_6;
	}
	if(Message == "7") {
		Event.type = SDL_KEYUP;
		Event.key.state = SDL_RELEASED;
		Event.key.keysym.sym = SDLK_7;
	}
	if(Message == "8") {
		Event.type = SDL_KEYUP;
		Event.key.state = SDL_RELEASED;
		Event.key.keysym.sym = SDLK_8;
	}
	if(Message == "9") {
		Event.type = SDL_KEYUP;
		Event.key.state = SDL_RELEASED;
		Event.key.keysym.sym = SDLK_9;
	}
	if(Message == "q" || Message == "Q") {
		Event.type = SDL_KEYUP;
		Event.key.state = SDL_RELEASED;
		Event.key.keysym.sym = SDLK_q;
	}
	if(Message == "w" || Message == "W") {
		Event.type = SDL_KEYUP;
		Event.key.state = SDL_RELEASED;
		Event.key.keysym.sym = SDLK_w;
	}
	if(Message == "a" || Message == "A") {
		Event.type = SDL_KEYUP;
		Event.key.state = SDL_RELEASED;
		Event.key.keysym.sym = SDLK_a;
	}
	if(Message == "l" || Message == "L") {
		Event.type = SDL_KEYUP;
		Event.key.state = SDL_RELEASED;
		Event.key.keysym.sym = SDLK_l;
	}


	SafetyLock Lock(&Client->Server->SafeMutex);
	
	std::cout<<"Command get:"<<Message<<std::endl;
	SDL_PushEvent(&Event);
	std::cout<<"Command executed:"<<Message<<std::endl;

	return NULL;
}

