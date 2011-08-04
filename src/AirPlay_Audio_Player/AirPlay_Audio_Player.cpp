/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#include "AirPlay_Audio_Player.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include "DCE/DeviceData_Base.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"

extern "C" {
	#include "socketlib.h"
	#include "hairtunes.h"
}

#ifndef TRUE
#define TRUE (-1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif


//<-dceag-d-e->

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
AirPlay_Audio_Player::AirPlay_Audio_Player(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: AirPlay_Audio_Player_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
  	char tHWID[HWID_SIZE] = {0,51,52,53,54,55};
  	char tHWID_Hex[HWID_SIZE * 2 + 1];
  	memset(tHWID_Hex, 0, sizeof(tHWID_Hex));

  	char tServerName[56] = "Media Director";
  	char tPassword[56] = "";

  	struct addrinfo *tAddrInfo;
  	//int  tSimLevel = 0;
  	int  tUseKnownHWID = FALSE;
  	int  tPort = PORT;
  	
  	m_pDevice_MD = NULL;

    //LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "AirPlay_Audio_Player::Player starting up on %L", m_pData->m_dwPK_Device);
   	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "AirPlay_Audio_Player::Player starting up");
	srandom ( time(NULL) );

  	int tIdx = 0;
  	for(tIdx=0;tIdx<HWID_SIZE;tIdx++)
  	{
		if(tIdx > 0)
    	{
      		if(!tUseKnownHWID)
      		{
        		int tVal = ((random() % 80) + 33);
        		tHWID[tIdx] = tVal;
      		}
    	}
   		sprintf(tHWID_Hex+(tIdx*2), "%02X",tHWID[tIdx]);
  	}
	
	AirPlay_Audio_Player::startAvahi(tHWID_Hex, tServerName, tPort);
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Starting connection server: specified server port: %d\n", tPort);
    int tServerSock = setupListenServer(&tAddrInfo, tPort);
    if(tServerSock < 0)
    {
      	//freeaddrinfo(tAddrInfo);
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "AirPlay_Audio_Player::Error setting up server socket on port %d, try specifying a different port\n", tPort);
      	//exit(1);
   	}

    int tClientSock = 0;
    while(1)
    {
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Waiting for clients to connect\n");
      	tClientSock = acceptClient(tServerSock, tAddrInfo);
      	if(tClientSock > 0)
      	{
        	int tPid = fork();
        	if(tPid == 0)
       		{
          		freeaddrinfo(tAddrInfo);
          		tAddrInfo = NULL;
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Accepted Client Connection..\n");
          		close(tServerSock);
          		handleClient(tClientSock, tPassword, tHWID);
          		//close(tClientSock);
          		//return 0;
        	}
        	else
        	{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Child now busy handling new client\n");
          		close(tClientSock);
        	}
      	}
      	else
      	{
        	LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Failed to initialize server socket, retrying...");
        	sleep(2);
     	}
    }
 
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
AirPlay_Audio_Player::AirPlay_Audio_Player(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: AirPlay_Audio_Player_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
AirPlay_Audio_Player::~AirPlay_Audio_Player()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool AirPlay_Audio_Player::GetConfig()
{
	if( !AirPlay_Audio_Player_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool AirPlay_Audio_Player::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
AirPlay_Audio_Player_Command *Create_AirPlay_Audio_Player(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new AirPlay_Audio_Player(pPrimaryDeviceCommand, pData, pEvent, pRouter);
}
//<-dceag-createinst-e->

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void AirPlay_Audio_Player::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	sCMD_Result = "UNHANDLED CHILD";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void AirPlay_Audio_Player::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here

void AirPlay_Audio_Player::SomeFunction()
{
	// If this is going to be loaded into the router as a plug-in, you can implement: 	virtual bool Register();
	// to do all your registration, such as creating message interceptors

	// If you use an IDE with auto-complete, after you type DCE:: it should give you a list of all
	// commands and requests, including the parameters.  See "AllCommandsRequests.h"

	// Examples:
	
	// Send a specific the "CMD_Simulate_Mouse_Click" command, which takes an X and Y parameter.  We'll use 55,77 for X and Y.
	DCE::CMD_Simulate_Mouse_Click CMD_Simulate_Mouse_Click(m_dwPK_Device,OrbiterID,55,77);
	SendCommand(CMD_Simulate_Mouse_Click);

	// Send the message to orbiters 32898 and 27283 (ie a device list, hence the _DL)
	// And we want a response, which will be "OK" if the command was successfull
	string sResponse;
	DCE::CMD_Simulate_Mouse_Click_DL CMD_Simulate_Mouse_Click_DL(m_dwPK_Device,"32898,27283",55,77)
	SendCommand(CMD_Simulate_Mouse_Click_DL,&sResponse);

	// Send the message to all orbiters within the house, which is all devices with the category DEVICECATEGORY_Orbiter_CONST (see pluto_main/Define_DeviceCategory.h)
	// Note the _Cat for category
	DCE::CMD_Simulate_Mouse_Click_Cat CMD_Simulate_Mouse_Click_Cat(m_dwPK_Device,DEVICECATEGORY_Orbiter_CONST,true,BL_SameHouse,55,77)
    SendCommand(CMD_Simulate_Mouse_Click_Cat);

	// Send the message to all "DeviceTemplate_Orbiter_CONST" devices within the room (see pluto_main/Define_DeviceTemplate.h)
	// Note the _DT.
	DCE::CMD_Simulate_Mouse_Click_DT CMD_Simulate_Mouse_Click_DT(m_dwPK_Device,DeviceTemplate_Orbiter_CONST,true,BL_SameRoom,55,77);
	SendCommand(CMD_Simulate_Mouse_Click_DT);

	// This command has a normal string parameter, but also an int as an out parameter
	int iValue;
	DCE::CMD_Get_Signal_Strength CMD_Get_Signal_Strength(m_dwDeviceID, DestDevice, sMac_address,&iValue);
	// This send command will wait for the destination device to respond since there is
	// an out parameter
	SendCommand(CMD_Get_Signal_Strength);  

	// This time we don't care about the out parameter.  We just want the command to 
	// get through, and don't want to wait for the round trip.  The out parameter, iValue,
	// will not get set
	SendCommandNoResponse(CMD_Get_Signal_Strength);  

	// This command has an out parameter of a data block.  Any parameter that is a binary
	// data block is a pair of int and char *
	// We'll also want to see the response, so we'll pass a string for that too

	int iFileSize;
	char *pFileContents
	string sResponse;
	DCE::CMD_Request_File CMD_Request_File(m_dwDeviceID, DestDevice, "filename",&pFileContents,&iFileSize,&sResponse);
	SendCommand(CMD_Request_File);

	// If the device processed the command (in this case retrieved the file),
	// sResponse will be "OK", and iFileSize will be the size of the file
	// and pFileContents will be the file contents.  **NOTE**  We are responsible
	// free deleting pFileContents.


	// To access our data and events below, you can type this-> if your IDE supports auto complete to see all the data and events you can access

	// Get our IP address from our data
	string sIP = DATA_Get_IP_Address();

	// Set our data "Filename" to "myfile"
	DATA_Set_Filename("myfile");

	// Fire the "Finished with file" event, which takes no parameters
	EVENT_Finished_with_file();
	// Fire the "Touch or click" which takes an X and Y parameter
	EVENT_Touch_or_click(10,150);
}
*/
//<-dceag-sample-e->

/*

	COMMANDS TO IMPLEMENT

*/

int AirPlay_Audio_Player::startAvahi(const char *pHWStr, const char *pServerName, int pPort)
{
  	LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Starting avahi registration");
  	unsigned int tMaxServerName = 25; // Something reasonable?  iPad showed 21, iphone 25
  	int tPid = fork();
  	if(tPid == 0)
  	{
    	char tName[100 + HWID_SIZE + 3];
    	if(strlen(pServerName) > tMaxServerName)
    	{
      		LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Servername truncated to %d characters", tMaxServerName);
    	}

    	tName[0] = '\0';
    	char tPort[SERVLEN];
   	 	sprintf(tPort, "%d", pPort);
    	strcat(tName, pHWStr);
    	strcat(tName, "@");
    	strncat(tName, pServerName, tMaxServerName);
    	LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Registering with Avahi/DNS-SD Name: %s on port %d\n", tName, pPort);

    	execlp("avahi-publish-service", "avahi-publish-service", tName,
         "_raop._tcp", tPort, "tp=UDP","sm=false","sv=false","ek=1","et=0,1",
         "cn=0,1","ch=2","ss=16","sr=44100","pw=false","vn=3","txtvers=1", NULL);
    	execlp("dns-sd", "dns-sd", "-R", tName,
         "_raop._tcp", ".", tPort, "tp=UDP","sm=false","sv=false","ek=1","et=0,1",
         "cn=0,1","ch=2","ss=16","sr=44100","pw=false","vn=3","txtvers=1", NULL);
    	if(errno == -1) 
    	{
            perror("error");
    	}
    	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "AirPlay_Audio_Player::failed to run avahi-publish-server OR dns-sd");
    	exit(1);
  	}
  	else
  	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Avahi/DNS-SD started on PID: %d\n", tPid);
  	}
  	return tPid;
}

void AirPlay_Audio_Player::handleClient(int pSock, char *pPassword, char *pHWADDR)
{
  	LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::In Handle Client\n");
  	fflush(stdout);

  	socklen_t len;
  	struct sockaddr_storage addr;
  	#ifdef AF_INET6
  		unsigned char ipbin[INET6_ADDRSTRLEN];
  	#else
  		unsigned char ipbin[INET_ADDRSTRLEN];
  	#endif
  	unsigned int ipbinlen;
  	int port;
  	char ipstr[64];

  	len = sizeof addr;
  	getsockname(pSock, (struct sockaddr*)&addr, &len);

  	// deal with both IPv4 and IPv6:
  	if (addr.ss_family == AF_INET) 
  	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Constructing ipv4 address\n");
      	struct sockaddr_in *s = (struct sockaddr_in *)&addr;
      	port = ntohs(s->sin_port);
      	inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
      	memcpy(ipbin, &s->sin_addr, 4);
      	ipbinlen = 4;
  	} 
  	else 
  	{ // AF_INET6
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Constructing ipv6 address\n");
      	struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
      	port = ntohs(s->sin6_port);
      	inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
        union 
        {
            struct sockaddr_in6 s;
        	unsigned char bin[sizeof(struct sockaddr_in6)];
        } addr;
        memcpy(&addr.s, &s->sin6_addr, sizeof(struct sockaddr_in6));

        if(memcmp(&addr.bin[0], "\x00\x00\x00\x00" "\x00\x00\x00\x00" "\x00\x00\xff\xff", 12) == 0)
      	{
        	// its ipv4...
        	memcpy(ipbin, &addr.bin[12], 4);
        	ipbinlen = 4;
      	}
      	else
      	{
        	memcpy(ipbin, &s->sin6_addr, 16);
        	ipbinlen = 16;
      	}
  	}
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Peer IP %s @ port %ds\n", ipstr, port);

  	int tMoreDataNeeded = 1;
  	struct keyring     tKeys;
  	struct comms       tComms;
  	struct connection  tConn;
  	initConnection(&tConn, &tKeys, &tComms, pSock, pPassword);

  	while(1)
  	{
    	tMoreDataNeeded = 1;

    	initBuffer(&tConn.recv, 80); // Just a random, small size to seed the buffer with.
    	initBuffer(&tConn.resp, 80);

    	int tError = FALSE;
    	while(1 == tMoreDataNeeded)
    	{
      		tError = readDataFromClient(pSock, &(tConn.recv));
      		if(!tError && strlen(tConn.recv.data) > 0)
      		{
        		LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Finished Reading some data from client\n");
	
        		// parse client request
        		tMoreDataNeeded = parseMessage(&tConn, ipbin, ipbinlen, pHWADDR);
        		if(1 == tMoreDataNeeded)
        		{
          			LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Need to read more data\n");
        		}
        		else if(-1 == tMoreDataNeeded) // Forked process down below ended.
        		{
          			LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Forked Process ended...cleaning up\n");
          			cleanup(&tConn);
          			// pSock was already closed
          			return;
        		}
       	 	// if more data needed,
      		}
      		else
      		{
        		LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Error reading from socket, closing client\n");
        		// Error reading data....quit.
        		cleanup(&tConn);
        		return;
      		}
    	}
    	LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Writing: %d chars to socket\n", tConn.resp.current);
    	//tConn->resp.data[tConn->resp.current-1] = '\0';
    	writeDataToClient(pSock, &(tConn.resp));
   		// Finished reading one message...
    	cleanupBuffers(&tConn);
  	}
  	cleanup(&tConn);
  	fflush(stdout);
}

void AirPlay_Audio_Player::initConnection(struct connection *pConn, struct keyring *pKeys, struct comms *pComms, int pSocket, char *pPassword)
{
  	pConn->hairtunes = pComms;
  	if(pKeys != NULL)
  	{
    	pConn->keys = pKeys;
    	pConn->keys->aesiv = NULL;
    	pConn->keys->aeskey = NULL;
    	pConn->keys->fmt = NULL;
  	}
  	pConn->recv.data = NULL;  // Pre-init buffer expected to be NULL
  	pConn->resp.data = NULL;  // Pre-init buffer expected to be NULL
  	pConn->clientSocket = pSocket;
  	if(strlen(pPassword) >0)
  	{
    	pConn->password = pPassword;
  	}
  	else
  	{
    	pConn->password = NULL;
  	}
}

void AirPlay_Audio_Player::closePipe(int *pPipe)
{
  	if(*pPipe != -1)
  	{
    	close(*pPipe);
    	*pPipe = -1;
  	}
}

void AirPlay_Audio_Player::cleanup(struct connection *pConn)
{
  	cleanupBuffers(pConn);
  	if(pConn->hairtunes != NULL)
  	{
    	closePipe(&(pConn->hairtunes->in[0]));
    	closePipe(&(pConn->hairtunes->in[1]));
    	closePipe(&(pConn->hairtunes->out[0]));
    	closePipe(&(pConn->hairtunes->out[1]));
  	}
  	if(pConn->keys != NULL)
  	{
    	if(pConn->keys->aesiv != NULL)
    	{
      		free(pConn->keys->aesiv);
    	}
    	if(pConn->keys->aeskey != NULL)
    	{
      		free(pConn->keys->aeskey);
    	}
    	if(pConn->keys->fmt != NULL)
    	{
      		free(pConn->keys->fmt);
    	}
    	pConn->keys = NULL;
  	}
  	if(pConn->clientSocket != -1)
  	{
    	close(pConn->clientSocket);
    	pConn->clientSocket = -1;
  	}
}


void AirPlay_Audio_Player::initBuffer(struct shairbuffer *pBuf, int pNumChars)
{
  	if(pBuf->data != NULL)
  	{
    	LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Hrm, buffer wasn't cleaned up....trying to free\n");
    	free(pBuf->data);
    	LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Free didn't seem to seg fault....huzzah\n");
  	}
  	pBuf->current = 0;
  	pBuf->marker = 0;
  	pBuf->maxsize = sizeof(char) * pNumChars;
  	pBuf->data = (char*)malloc(pBuf->maxsize);
  	memset(pBuf->data, 0, pBuf->maxsize);
}


void AirPlay_Audio_Player::cleanupBuffers(struct connection *pConn)
{
  	if(pConn->recv.data != NULL)
  	{
    	free(pConn->recv.data);
    	pConn->recv.data = NULL;
  	}
  	if(pConn->resp.data != NULL)
  	{
    	free(pConn->resp.data);
    	pConn->resp.data = NULL;
  	}
}

int AirPlay_Audio_Player::readDataFromClient(int pSock, struct shairbuffer *pClientBuffer)
{
  	char tReadBuf[MAX_SIZE];
  	tReadBuf[0] = '\0';

  	int tRetval = 1;
  	int tEnd = -1;
  	while(tRetval > 0 && tEnd < 0)
  	{
     	// Read from socket until \n\n, \r\n\r\n, or \r\r is found
      	LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Waiting To Read...\n");
      	fflush(stdout);
      	tRetval = read(pSock, tReadBuf, MAX_SIZE);
      	// if new buffer contains the end of request string, only copy partial buffer?
      	tEnd = findEnd(tReadBuf);
      	if(tEnd >= 0)
     	{
        	if(pClientBuffer->marker == 0)
        	{
          		pClientBuffer->marker = tEnd+1; // Marks start of content
        	}
        	LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Found end of http request at: %d\n", tEnd);
        	fflush(stdout);        
      	}
      	else
      	{
        	tEnd = MAX_SIZE;
        	LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Read %d of data so far\n%s\n", tRetval, tReadBuf);
        	fflush(stdout);
      	}
      	if(tRetval > 0)
      	{
        	// Copy read data into tReceive;
        	LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Read %d data, using %d of it\n", tRetval, tEnd);
        	addNToShairBuffer(pClientBuffer, tReadBuf, tRetval);
        	LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Finished copying data\n");
      	}
      	else
      	{
        	LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::rror reading data from socket, got: %d bytes", tRetval);
        	return tRetval;
      	}
  	}
  	if(tEnd + 1 != tRetval)
  	{
    	LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Read more data after end of http request. %d instead of %d\n", tRetval, tEnd+1);
  	}
  	LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Finished Reading Data:\n%s\nEndOfData\n", pClientBuffer->data);
  	fflush(stdout);
  	return 0;
}

int AirPlay_Audio_Player::findEnd(char *tReadBuf)
{
  	// find \n\n, \r\n\r\n, or \r\r is found
  	int tIdx = 0;
  	int tLen = strlen(tReadBuf);
  	for(tIdx = 0; tIdx < tLen; tIdx++)
  	{
    	if(tReadBuf[tIdx] == '\r')
    	{
      		if(tIdx + 1 < tLen)
      		{
        		if(tReadBuf[tIdx+1] == '\r')
        		{
          			return (tIdx+1);
        		}
        		else if(tIdx+3 < tLen)
        		{
          			if(tReadBuf[tIdx+1] == '\n' && tReadBuf[tIdx+2] == '\r' && tReadBuf[tIdx+3] == '\n')
          			{
            			return (tIdx+3);
          			}
        		}
      		}
    	}
    	else if(tReadBuf[tIdx] == '\n')
    	{
      		if(tIdx + 1 < tLen && tReadBuf[tIdx+1] == '\n')
      		{
        		return (tIdx + 1);
      		}
    	}
  	}
  	// Found nothing
  	return -1;
}

void AirPlay_Audio_Player::addToShairBuffer(struct shairbuffer *pBuf, char *pNewBuf)
{
  	addNToShairBuffer(pBuf, pNewBuf, strlen(pNewBuf));
}

void AirPlay_Audio_Player::addNToShairBuffer(struct shairbuffer *pBuf, char *pNewBuf, int pNofNewBuf)
{
  	int tAvailChars = getAvailChars(pBuf);
  	if(pNofNewBuf > tAvailChars)
  	{
    	int tNewSize = pBuf->maxsize * 2 + MAX_SIZE + sizeof(char);
    	char *tTmpBuf = (char*)malloc(tNewSize);

    	tTmpBuf[0] = '\0';
    	memset(tTmpBuf, 0, tNewSize/sizeof(char));
    	memcpy(tTmpBuf, pBuf->data, pBuf->current);
    	free(pBuf->data);

    	pBuf->maxsize = tNewSize;
    	pBuf->data = tTmpBuf;
  	}
  	memcpy(pBuf->data + pBuf->current, pNewBuf, pNofNewBuf);
  	pBuf->current += pNofNewBuf;
  	if(getAvailChars(pBuf) > 1)
  	{
    	pBuf->data[pBuf->current] = '\0';
  	}
}

int AirPlay_Audio_Player::getAvailChars(struct shairbuffer *pBuf)
{
  	return (pBuf->maxsize / sizeof(char)) - pBuf->current;
}

void AirPlay_Audio_Player::writeDataToClient(int pSock, struct shairbuffer *pResponse)
{
  	LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::\n----Beg Send Response Header----\n%.*s\n", pResponse->current, pResponse->data);
  	send(pSock, pResponse->data, pResponse->current,0);
  	LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::----Send Response Header----\n");
}

// Handles compiling the Apple-Challenge, HWID, and Server IP Address
// Into the response the airplay client is expecting.
int AirPlay_Audio_Player::buildAppleResponse(struct connection *pConn, unsigned char *pIpBin, unsigned int pIpBinLen, char *pHWID)
{
  	// Find Apple-Challenge
 	char *tResponse = NULL;

  	int tFoundSize = 0;
  	char* tFound = getFromHeader(pConn->recv.data, "Apple-Challenge", &tFoundSize);
  	if(tFound != NULL)
  	{
    	char tTrim[tFoundSize + 2];
    	getTrimmed(tFound, tFoundSize, TRUE, TRUE, tTrim);
    	LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::HeaderChallenge:  [%s] len: %d  sizeFound: %d\n", tTrim, strlen(tTrim), tFoundSize);
    	int tChallengeDecodeSize = 16;
    	char *tChallenge = decode_base64((unsigned char *)tTrim, tFoundSize, &tChallengeDecodeSize);
    	LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Challenge Decode size: %d  expected 16\n", tChallengeDecodeSize);

    	int tCurSize = 0;
    	unsigned char tChalResp[38];

   	 	memcpy(tChalResp, tChallenge, tChallengeDecodeSize);
    	tCurSize += tChallengeDecodeSize;
    
    	memcpy(tChalResp+tCurSize, pIpBin, pIpBinLen);
    	tCurSize += pIpBinLen;

    	memcpy(tChalResp+tCurSize, pHWID, HWID_SIZE);
    	tCurSize += HWID_SIZE;

    	int tPad = 32 - tCurSize;
    	if (tPad > 0)
    	{
      		memset(tChalResp+tCurSize, 0, tPad);
      		tCurSize += tPad;
    	}

    	char *tTmp = encode_base64((unsigned char *)tChalResp, tCurSize);
    	LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Full sig: %s\n", tTmp);
    	free(tTmp);

    	// RSA Encrypt
    	RSA *rsa = loadKey();  // Free RSA
    	int tSize = RSA_size(rsa);
    	unsigned char tTo[tSize];
    	RSA_private_encrypt(tCurSize, (unsigned char *)tChalResp, tTo, rsa, RSA_PKCS1_PADDING);
    
    	// Wrap RSA Encrypted binary in Base64 encoding
    	tResponse = encode_base64(tTo, tSize);
    	int tLen = strlen(tResponse);
    	while(tLen > 1 && tResponse[tLen-1] == '=') tResponse[tLen-1] = '\0';
    	free(tChallenge);
    	RSA_free(rsa);
  	}

  	if(tResponse != NULL)
  	{
    	// Append to current response
    	addToShairBuffer(&(pConn->resp), (char *)"Apple-Response: ");
    	addToShairBuffer(&(pConn->resp), tResponse);
    	addToShairBuffer(&(pConn->resp), (char *)"\r\n");
    	free(tResponse);
    	return TRUE;
  	}
  	return FALSE;
}

int AirPlay_Audio_Player::parseMessage(struct connection *pConn, unsigned char *pIpBin, unsigned int pIpBinLen, char *pHWID)
{
  	int tReturn = 0; // 0 = good, 1 = Needs More Data, -1 = close client socket.
  	if(pConn->resp.data == NULL) initBuffer(&(pConn->resp), MAX_SIZE);

  	char *tContent = getFromHeader(pConn->recv.data, "Content-Length", NULL);
  	if(tContent != NULL)
  	{
    	int tContentSize = atoi(tContent);
    	if((unsigned)pConn->recv.marker == 0 || strlen(pConn->recv.data+pConn->recv.marker) != (unsigned)tContentSize)
    	{
      		LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Content-Length: %s value -> %d\n", tContent, tContentSize);
        	if((unsigned)pConn->recv.marker != 0)
        		{
          			LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::ContentPtr has %d, but needs %d\n", 
                  	strlen(pConn->recv.data+pConn->recv.marker), tContentSize);
        		}
      		// check if value in tContent > 2nd read from client.
      		return 1; // means more content-length needed
    	}
    }
  	else LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::No content, header only\n");

  	// "Creates" a new Response Header for our response message
  	addToShairBuffer(&(pConn->resp), (char *)"RTSP/1.0 200 OK\r\n");

  	if(buildAppleResponse(pConn, pIpBin, pIpBinLen, pHWID)) // need to free sig
  	{
    	LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Added AppleResponse to Apple-Challenge request\n");
  	}

  	// Find option, then based on option, do different actions.
  	if(strncmp(pConn->recv.data, "OPTIONS", 7) == 0)
  	{
    	propogateCSeq(pConn);
    	addToShairBuffer(&(pConn->resp), (char *)"Public: ANNOUNCE, SETUP, RECORD, PAUSE, FLUSH, TEARDOWN, OPTIONS, GET_PARAMETER, SET_PARAMETER\r\n");
  	}
  	else if(!strncmp(pConn->recv.data, "ANNOUNCE", 8))
  	{
    	char *tContent = pConn->recv.data + pConn->recv.marker;
    	int tSize = 0;
    	char *tHeaderVal = getFromContent(tContent, "a=aesiv", &tSize); // Not allocated memory, just pointing
    	if(tSize > 0)
    	{
      		int tKeySize = 0;
      		char tEncodedAesIV[tSize + 2];
      		getTrimmed(tHeaderVal, tSize, TRUE, TRUE, tEncodedAesIV);
      		LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::AESIV: [%.*s] Size: %d  Strlen: %d\n", tSize, tEncodedAesIV, tSize, strlen(tEncodedAesIV));
      		char *tDecodedIV =  decode_base64((unsigned char*) tEncodedAesIV, tSize, &tSize);

      		// grab the key, copy it out of the receive buffer
      		tHeaderVal = getFromContent(tContent, "a=rsaaeskey", &tKeySize);
      		char tEncodedAesKey[tKeySize + 2]; // +1 for nl, +1 for \0
      		getTrimmed(tHeaderVal, tKeySize, TRUE, TRUE, tEncodedAesKey);
      		LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::AES KEY: [%s] Size: %d  Strlen: %d\n", tEncodedAesKey, tKeySize, strlen(tEncodedAesKey));
     	 	// remove base64 coding from key
      		char *tDecodedAesKey = decode_base64((unsigned char*) tEncodedAesKey,tKeySize, &tKeySize);  // Need to free DecodedAesKey
      		// Grab the formats
      		int tFmtpSize = 0;
      		char *tFmtp = getFromContent(tContent, "a=fmtp", &tFmtpSize);  // Don't need to free
      		tFmtp = getTrimmedMalloc(tFmtp, tFmtpSize, TRUE, FALSE); // will need to free
      		LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Format: %s\n", tFmtp);

      		RSA *rsa = loadKey();
      		// Decrypt the binary aes key
      		char *tDecryptedKey = (char *)malloc(RSA_size(rsa) * sizeof(char)); // Need to Free Decrypted key
      		//char tDecryptedKey[RSA_size(rsa)];
      		if(RSA_private_decrypt(tKeySize, (unsigned char *)tDecodedAesKey, (unsigned char*) tDecryptedKey, rsa, RSA_PKCS1_OAEP_PADDING) >= 0)
      		{
        		LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Decrypted AES key from RSA Successfully\n");
      		}
      		else LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Error Decrypting AES key from RSA\n");
      		free(tDecodedAesKey);
      		RSA_free(rsa);
      		setKeys(pConn->keys, tDecodedIV, tDecryptedKey, tFmtp);
      		propogateCSeq(pConn);
    	}
  	}
  	else if(!strncmp(pConn->recv.data, "SETUP", 5))
  	{
    	// Setup pipes
    	struct comms *tComms = pConn->hairtunes;
    	if (! (pipe(tComms->in) == 0 && pipe(tComms->out) == 0))
    	{
      		LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Error setting up hairtunes communications...some things probably wont work very well.\n");
    	}
    
    	// Setup fork
    	char tPort[8] = "6000";  // get this from dup()'d stdout of child pid

    	int tPid = fork();
    	if(tPid == 0)
    	{
      		int tDataport=0;
      		char tCPortStr[8] = "59010";
      		char tTPortStr[8] = "59012";
      		int tSize = 0;

      		char *tFound  =getFromSetup(pConn->recv.data, "control_port", &tSize);
      		getTrimmed(tFound, tSize, 1, 0, tCPortStr);
      		tFound = getFromSetup(pConn->recv.data, "timing_port", &tSize);
      		getTrimmed(tFound, tSize, 1, 0, tTPortStr);

      		LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::converting %s and %s from str->int\n", tCPortStr, tTPortStr);
      		int tControlport = atoi(tCPortStr);
      		int tTimingport = atoi(tTPortStr);

      		LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Got %d for CPort and %d for TPort\n", tControlport, tTimingport);
      		char *tRtp = NULL;
      		char *tPipe = NULL;//(char *)"/var/run/airplay.raw";
      		char *tAoDriver = NULL;
      		char *tAoDeviceName = NULL;
      		char *tAoDeviceId = NULL;

      		// *************************************************
      		// ** Setting up Pipes, AKA no more debug/output  **
      		// *************************************************
      		dup2(tComms->in[0],0);   // Input to child
      		closePipe(&(tComms->in[0]));
      		closePipe(&(tComms->in[1]));

      		dup2(tComms->out[1], 1); // Output from child
      		closePipe(&(tComms->out[1]));
      		closePipe(&(tComms->out[0]));

      		struct keyring *tKeys = pConn->keys;
      		pConn->keys = NULL;
      		pConn->hairtunes = NULL;

      		// Free up any recv buffers, etc..
      		if(pConn->clientSocket != -1)
      		{
        		close(pConn->clientSocket);
        		pConn->clientSocket = -1;
      		}
      		cleanupBuffers(pConn);
      		hairtunes_init(tKeys->aeskey, tKeys->aesiv, tKeys->fmt, tControlport, tTimingport, tDataport, tRtp, tPipe, tAoDriver, tAoDeviceName, tAoDeviceId);

      		// Quit when finished.
      		LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Returned from hairtunes init....returning -1, should close out this whole side of the fork\n");
      		return -1;
    	}
    	else if(tPid >0)
    	{
      		// Ensure Connection has access to the pipe.
      		closePipe(&(tComms->in[0]));
      		closePipe(&(tComms->out[1]));

      		char tFromHairtunes[80];
      		int tRead = read(tComms->out[0], tFromHairtunes, 80);
      		if(tRead <= 0)
      		{
        		LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Error reading port from hairtunes function, assuming default port: %d\n", tPort);
      		}
      		else
      		{
        		int tSize = 0;
        		char *tPortStr = getFromHeader(tFromHairtunes, "port", &tSize);
        		if(tPortStr != NULL) getTrimmed(tPortStr, tSize, TRUE, FALSE, tPort);
         		else LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Read %d bytes, Error translating %s into a port\n", tRead, tFromHairtunes);
      		}
      		//  READ Ports from here?close(pConn->hairtunes_pipes[0]);
      		propogateCSeq(pConn);
      		int tSize = 0;
     		 char *tTransport = getFromHeader(pConn->recv.data, "Transport", &tSize);
      		addToShairBuffer(&(pConn->resp), (char *)"Transport: ");
      		addNToShairBuffer(&(pConn->resp), tTransport, tSize);
      		// Append server port:
      		addToShairBuffer(&(pConn->resp), (char *)";server_port=");
      		addToShairBuffer(&(pConn->resp), tPort);
      		addToShairBuffer(&(pConn->resp), (char *)"\r\nSession: DEADBEEF\r\n");
      	}
    	else
    	{
      		LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Error forking process....dere' be errors round here.\n");
      		return -1;
    	}
  	}
  	else if(!strncmp(pConn->recv.data, "TEARDOWN", 8))
  	{
    	// Be smart?  Do more finish up stuff...
    	addToShairBuffer(&(pConn->resp), (char *)"Connection: close\r\n");
    	propogateCSeq(pConn);
    	close(pConn->hairtunes->in[1]);
	    LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Tearing down connection, closing pipes\n");
	    //close(pConn->hairtunes->out[0]);
    	tReturn = -1;  // Close client socket, but sends an ACK/OK packet first
  	}
  	else if(!strncmp(pConn->recv.data, "FLUSH", 5))
  	{
    	write(pConn->hairtunes->in[1], "flush\n", 6);
    	propogateCSeq(pConn);
  	}
  	else if(!strncmp(pConn->recv.data, "SET_PARAMETER", 13))
  	{
    	propogateCSeq(pConn);
    	int tSize = 0;
    	char *tVol = getFromHeader(pConn->recv.data, "volume", &tSize);
    	LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::About to write [vol: %.*s] data to hairtunes\n", tSize, tVol);

    	write(pConn->hairtunes->in[1], "vol: ", 5);
    	write(pConn->hairtunes->in[1], tVol, tSize);
    	write(pConn->hairtunes->in[1], "\n", 1);
    	LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Finished writing data write data to hairtunes\n");
  	}
  	else
  	{
    	LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::\n\nUn-Handled recv: %s\n", pConn->recv.data);
    	propogateCSeq(pConn);
  	}
  	addToShairBuffer(&(pConn->resp), (char *)"\r\n");
  	return tReturn;
}

// Copies CSeq value from request, and adds standard header values in.
void AirPlay_Audio_Player::propogateCSeq(struct connection *pConn) //char *pRecvBuffer, struct shairbuffer *pConn->recp.data)
{
  	int tSize=0;
  	char *tRecPtr = getFromHeader(pConn->recv.data, "CSeq", &tSize);
  	addToShairBuffer(&(pConn->resp), (char *)"Audio-Jack-Status: connected; type=analog\r\n");
 	addToShairBuffer(&(pConn->resp), (char *)"CSeq: ");
  	addNToShairBuffer(&(pConn->resp), tRecPtr, tSize);
  	addToShairBuffer(&(pConn->resp), (char *)"\r\n");
}	

char *AirPlay_Audio_Player::getTrimmed(char *pChar, int pSize, int pEndStr, int pAddNL, char *pTrimDest)
{
  	int tSize = pSize;
  	if(pEndStr)tSize++;
  	if(pAddNL)tSize++;
  
  	memset(pTrimDest, 0, tSize);
  	memcpy(pTrimDest, pChar, pSize);
  	if(pAddNL) pTrimDest[pSize] = '\n';
  	if(pEndStr) pTrimDest[tSize-1] = '\0';
  	return pTrimDest;
}

char *AirPlay_Audio_Player::getTrimmedMalloc(char *pChar, int pSize, int pEndStr, int pAddNL)
{
  	int tAdditionalSize = 0;
  	if(pEndStr)
    tAdditionalSize++;
  	if(pAddNL)
    tAdditionalSize++;
  	char *tTrimDest = (char*)malloc(sizeof(char) * (pSize + tAdditionalSize));
  	return getTrimmed(pChar, pSize, pEndStr, pAddNL, tTrimDest);
}

char *AirPlay_Audio_Player::getFromHeader(char *pHeaderPtr, const char *pField, int *pReturnSize)
{
  	return getFromBuffer(pHeaderPtr, pField, 2, pReturnSize, (char *)"\r\n");
}

char *AirPlay_Audio_Player::getFromContent(char *pContentPtr, const char* pField, int *pReturnSize)
{
  	return getFromBuffer(pContentPtr, pField, 1, pReturnSize, (char *)"\r\n");
}

char *AirPlay_Audio_Player::getFromSetup(char *pContentPtr, const char* pField, int *pReturnSize)
{
  	return getFromBuffer(pContentPtr, pField, 1, pReturnSize, (char *)";\r\n");
}

char *AirPlay_Audio_Player::getFromBuffer(char *pBufferPtr, const char *pField, int pLenAfterField, int *pReturnSize, char *pDelims)
{
  	LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::GettingFromBuffer: %s\n", pField);
  	char* tFound = strstr(pBufferPtr, pField);
  	int tSize = 0;
  	if(tFound != NULL)
  	{
    	tFound += (strlen(pField) + pLenAfterField);
    	int tIdx = 0;
    	char tDelim = pDelims[tIdx];
    	char *tShortest = NULL;
    	char *tEnd = NULL;
    	while(tDelim != '\0')
    	{
      		tDelim = pDelims[tIdx++]; // Ensures that \0 is also searched.
      		tEnd = strchr(tFound, tDelim);
      		if(tEnd != NULL && (NULL == tShortest || tEnd < tShortest)) tShortest = tEnd;
    	}    
    	tSize = (int) (tShortest - tFound);
    	LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Found %.*s  length: %d\n", tSize, tFound, tSize);
    	if(pReturnSize != NULL) *pReturnSize = tSize;
  	}
  	else LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::Not Found\n");
  	return tFound;
}

void AirPlay_Audio_Player::setKeys(struct keyring *pKeys, char *pIV, char* pAESKey, char *pFmtp)
{
  	if(pKeys->aesiv != NULL) free(pKeys->aesiv);
  	if(pKeys->aeskey != NULL) free(pKeys->aeskey);
  	if(pKeys->fmt != NULL) free(pKeys->fmt);
  	pKeys->aesiv = pIV;
  	pKeys->aeskey = pAESKey;
 	pKeys->fmt = pFmtp;
}

#define AIRPORT_PRIVATE_KEY \
"-----BEGIN RSA PRIVATE KEY-----\n" \
"MIIEpQIBAAKCAQEA59dE8qLieItsH1WgjrcFRKj6eUWqi+bGLOX1HL3U3GhC/j0Qg90u3sG/1CUt\n" \
"wC5vOYvfDmFI6oSFXi5ELabWJmT2dKHzBJKa3k9ok+8t9ucRqMd6DZHJ2YCCLlDRKSKv6kDqnw4U\n" \
"wPdpOMXziC/AMj3Z/lUVX1G7WSHCAWKf1zNS1eLvqr+boEjXuBOitnZ/bDzPHrTOZz0Dew0uowxf\n" \
"/+sG+NCK3eQJVxqcaJ/vEHKIVd2M+5qL71yJQ+87X6oV3eaYvt3zWZYD6z5vYTcrtij2VZ9Zmni/\n" \
"UAaHqn9JdsBWLUEpVviYnhimNVvYFZeCXg/IdTQ+x4IRdiXNv5hEewIDAQABAoIBAQDl8Axy9XfW\n" \
"BLmkzkEiqoSwF0PsmVrPzH9KsnwLGH+QZlvjWd8SWYGN7u1507HvhF5N3drJoVU3O14nDY4TFQAa\n" \
"LlJ9VM35AApXaLyY1ERrN7u9ALKd2LUwYhM7Km539O4yUFYikE2nIPscEsA5ltpxOgUGCY7b7ez5\n" \
"NtD6nL1ZKauw7aNXmVAvmJTcuPxWmoktF3gDJKK2wxZuNGcJE0uFQEG4Z3BrWP7yoNuSK3dii2jm\n" \
"lpPHr0O/KnPQtzI3eguhe0TwUem/eYSdyzMyVx/YpwkzwtYL3sR5k0o9rKQLtvLzfAqdBxBurciz\n" \
"aaA/L0HIgAmOit1GJA2saMxTVPNhAoGBAPfgv1oeZxgxmotiCcMXFEQEWflzhWYTsXrhUIuz5jFu\n" \
"a39GLS99ZEErhLdrwj8rDDViRVJ5skOp9zFvlYAHs0xh92ji1E7V/ysnKBfsMrPkk5KSKPrnjndM\n" \
"oPdevWnVkgJ5jxFuNgxkOLMuG9i53B4yMvDTCRiIPMQ++N2iLDaRAoGBAO9v//mU8eVkQaoANf0Z\n" \
"oMjW8CN4xwWA2cSEIHkd9AfFkftuv8oyLDCG3ZAf0vrhrrtkrfa7ef+AUb69DNggq4mHQAYBp7L+\n" \
"k5DKzJrKuO0r+R0YbY9pZD1+/g9dVt91d6LQNepUE/yY2PP5CNoFmjedpLHMOPFdVgqDzDFxU8hL\n" \
"AoGBANDrr7xAJbqBjHVwIzQ4To9pb4BNeqDndk5Qe7fT3+/H1njGaC0/rXE0Qb7q5ySgnsCb3DvA\n" \
"cJyRM9SJ7OKlGt0FMSdJD5KG0XPIpAVNwgpXXH5MDJg09KHeh0kXo+QA6viFBi21y340NonnEfdf\n" \
"54PX4ZGS/Xac1UK+pLkBB+zRAoGAf0AY3H3qKS2lMEI4bzEFoHeK3G895pDaK3TFBVmD7fV0Zhov\n" \
"17fegFPMwOII8MisYm9ZfT2Z0s5Ro3s5rkt+nvLAdfC/PYPKzTLalpGSwomSNYJcB9HNMlmhkGzc\n" \
"1JnLYT4iyUyx6pcZBmCd8bD0iwY/FzcgNDaUmbX9+XDvRA0CgYEAkE7pIPlE71qvfJQgoA9em0gI\n" \
"LAuE4Pu13aKiJnfft7hIjbK+5kyb3TysZvoyDnb3HOKvInK7vXbKuU4ISgxB2bB3HcYzQMGsz1qJ\n" \
"2gG0N5hvJpzwwhbhXqFKA4zaaSrw622wDniAK5MlIE0tIAKKP4yxNGjoD2QYjhBGuhvkWKaXTyY=\n" \
"-----END RSA PRIVATE KEY-----"

RSA *AirPlay_Audio_Player::loadKey()
{
  	BIO *tBio = BIO_new_mem_buf((void *)AIRPORT_PRIVATE_KEY, -1);
  	RSA *rsa = PEM_read_bio_RSAPrivateKey(tBio, NULL, NULL, NULL); //NULL, NULL, NULL);
  	BIO_free(tBio);
  	LoggerWrapper::GetInstance()->Write(LV_WARNING, "AirPlay_Audio_Player::RSA Key: %d\n", RSA_check_key(rsa));
  	return rsa;
}