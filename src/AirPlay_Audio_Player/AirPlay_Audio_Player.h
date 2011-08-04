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
#ifndef AirPlay_Audio_Player_h
#define AirPlay_Audio_Player_h

//	DCE Implemenation for #2189 AirPlay Audio Player

#include "Gen_Devices/AirPlay_Audio_PlayerBase.h"
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define HWID_SIZE 6

//<-dceag-d-e->

//<-dceag-decl-b->
namespace DCE
{
	class AirPlay_Audio_Player : public AirPlay_Audio_Player_Command
	{
//<-dceag-decl-e->
		// Private member variables
protected:
		
		struct shairbuffer
		{
  			char *data;
  			int   current;
  			int   maxsize;
  			int   marker;
		};

		struct keyring
		{
  			char *aeskey;
  			char *aesiv;
  			char *fmt;
		};

		struct comms
		{
  			int  in[2];
  			int  out[2];
		};

		
		struct connection
		{
  			struct shairbuffer  recv;
  			struct shairbuffer  resp;
  			struct keyring      *keys; // Does not point to malloc'd memory.
  			struct comms        *hairtunes;
  			int                 clientSocket;
  			char                *password;
		};

		int startAvahi(const char *pHWStr, const char *pServerName, int pPort);
		void handleClient(int pSock, char *pPassword, char *pHWADDR);

		void initBuffer(struct shairbuffer *pBuf, int pNumChars);
		void cleanupBuffers(struct connection *pConn);

		void initConnection(struct connection *pConn, struct keyring *pKeys, struct comms *pComms, int pSocket, char *pPassword);
		void closePipe(int *pPipe);
		void cleanup(struct connection *pConn);

		int readDataFromClient(int pSock, struct shairbuffer *pClientBuffer);
		int findEnd(char *tReadBuf);
		void addToShairBuffer(struct shairbuffer *pBuf, char *pNewBuf);
		void addNToShairBuffer(struct shairbuffer *pBuf, char *pNewBuf, int pNofNewBuf);
		int getAvailChars(struct shairbuffer *pBuf);

		void writeDataToClient(int pSock, struct shairbuffer *pResponse);
		
		int buildAppleResponse(struct connection *pConn, unsigned char *pIpBin, unsigned int pIpBinLen, char *pHWID);
		int parseMessage(struct connection *pConn, unsigned char *pIpBin, unsigned int pIpBinLen, char *pHWID);
		void propogateCSeq(struct connection *pConn);
		char *getTrimmed(char *pChar, int pSize, int pEndStr, int pAddNL, char *pTrimDest);
		char *getTrimmedMalloc(char *pChar, int pSize, int pEndStr, int pAddNL);
		char *getFromHeader(char *pHeaderPtr, const char *pField, int *pReturnSize);
		char *getFromContent(char *pContentPtr, const char* pField, int *pReturnSize);
		char *getFromSetup(char *pContentPtr, const char* pField, int *pReturnSize);
		char *getFromBuffer(char *pBufferPtr, const char *pField, int pLenAfterField, int *pReturnSize, char *pDelims);

		void setKeys(struct keyring *pKeys, char *pIV, char* pAESKey, char *pFmtp);
		RSA *loadKey();

		// Private methods
private:
		DeviceData_Base *m_pDevice_MD;
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		AirPlay_Audio_Player(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~AirPlay_Audio_Player();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		AirPlay_Audio_Player(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class

			*****EVENT***** accessors inherited from base class
	void EVENT_Playback_Completed(string sMRL,int iStream_ID,bool bWith_Errors);
	void EVENT_Playback_Started(string sMRL,int iStream_ID,string sSectionDescription,string sAudio,string sVideo);

			*****COMMANDS***** we need to implement
	*/

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
