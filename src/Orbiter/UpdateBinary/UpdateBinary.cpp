/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
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
// UpdateBinary.cpp : Defines the entry point for the console application.
//

#ifdef WINCE
	#include "stdafx.h"
	#include <commctrl.h>
#ifndef WINCE_x86
	#include <aygshell.h>
#endif
	#include <sipapi.h>
#else
	#include <iostream>
#endif


#include <tchar.h>

#include "DCE/Socket.h"
#include "DCE/Message.h"
#include "DCE/ClientSocket.h"
#include "DCE/Logger.h"

#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"

#include "PlutoUtils/FileUtils.h"

#include "../include/version.h"

using namespace DCE;

int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
{
	string sOrbiterUpdateFileName = "Orbiter_WinCE.dat";
	string sOrbiterFileName = "/InternalStorage/Orbiter.exe";
	string sCommFile = "/Orbiter.cmm";
	string sRouter_IP="dcerouter";
	int PK_Device=0;
	string sLogger="UpdateBinary.log";
	bool bError=false; // An error parsing the command line

	//parse command line
	string::size_type pos = 0;

#ifdef WINCE
	char pCmdLine[256];
	wcstombs(pCmdLine, lpCmdLine, 256);
#define CMD_LINE pCmdLine
#else
#define CMD_LINE lpCmdLine
#endif

	string command_line = CMD_LINE;
	string token;
	do
	{
		token = StringUtils::Tokenize(command_line, " ", pos);

		if(token != "" && token.size() >= 2)
			switch(token[1])
		{	
			case 'r':
				sRouter_IP = StringUtils::Tokenize(command_line, " ", pos);
				break;
			case 'd':
				PK_Device = atoi(StringUtils::Tokenize(command_line, " ", pos).c_str());
				break;
			case 'l':
				sLogger = StringUtils::Tokenize(command_line, " ", pos);
				break;
			case 'i':
				sOrbiterUpdateFileName = StringUtils::Tokenize(command_line, " ", pos);
				break;
			case 'o':
				{
					if(command_line[pos] == '\"')
					{
						pos++;
						sOrbiterFileName = StringUtils::Tokenize(command_line, "\"", pos);
						pos++;
					}
					else
						sOrbiterFileName = StringUtils::Tokenize(command_line, " ", pos);
				}
				break;
			case 'c':
				sCommFile = StringUtils::Tokenize(command_line, " ", pos);
				break;
			default:
				bError=true;
				break;
		}
	}
	while(token != "");

	if (bError)
	{
		printf("UpdateBinary, v. %s\n", VERSION);
		printf("A Pluto DCE Device.  See www.plutohome.com/dce for details.\n");
		printf("Usage: Orbiter [-r Router's IP] [-d My Device ID] [-l dcerouter|stdout|null|filename]\n"); 
		printf("[-f Orbiter File On The Server]\n");
		printf("-r router's IP	-- the IP address of the DCE Router  Defaults to 'dcerouter'.\n");
		printf("-d my device ID	-- This device's ID number.  If not specified, it will be requested\n");
		printf("	               from the router based on our IP address.\n");
		printf("-l dcerouter	-- Where to save the log files.  Specify 'dcerouter' to have \n"); 
		printf("	               the messages logged to the DCE Router.  Defaults to stdout.\n");
		printf("-i filename     -- The file name of the orbiter update file on the server.\n");
		printf("-o filename     -- The file path and name of the orbiter.\n");
		printf("-c filename     -- The file path and name of the communication file.\n");
		
		return 1;
	}

	WORD    wVersion;
	WSADATA wsaData;
	wVersion = MAKEWORD( 1, 1 );
	if (WSAStartup(wVersion, &wsaData)!=0)
	{
		int ec = WSAGetLastError();
		printf("WSAStartup err %d", ec);
		return 0;
	}

	try
	{
		if( sLogger=="null" )
			LoggerWrapper::SetType(LT_LOGGER_NULL);
		else 
			LoggerWrapper::SetType(LT_LOGGER_FILE,sLogger.c_str());
	}
	catch(...)
	{
		printf("Unable to create logger\n");
		return -1;
	}

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Started UpdateBinary with command line: %s", command_line.c_str());
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Communication file: %s", sCommFile.c_str());

	ClientSocket *pClientSocket = new ClientSocket( PK_Device, sRouter_IP, 
		string( "Event Dev #" ) + StringUtils::itos( PK_Device ) );

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Connecting to host...");    

	pClientSocket->Connect(0,"Event #" + StringUtils::itos(PK_Device));

	char *pUpdateFile = NULL;
	int iSizeUpdateFile = 0;	

	string sChecksum;
	bool bChecksumOnly = false;

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Creating message...");

	Message *pMessage = new Message(
		PK_Device,  //long dwDeviceIDFrom
		DEVICECATEGORY_General_Info_Plugins_CONST, //long dwDeviceCategory
		false, //bool bIncludeChildren
		BL_SameHouse, //eBroadcastLevel eBL
		PRIORITY_NORMAL, //long dwPriority
		MESSAGETYPE_COMMAND, //long dwMessageType
		COMMAND_Request_File_And_Checksum_CONST, //long dwID,

		4, //  unsigned long dwParameterCount

		COMMANDPARAMETER_Filename_CONST, //Filename (string) param ID
		sOrbiterUpdateFileName.c_str(), 

		COMMANDPARAMETER_Checksum_CONST,
		&sChecksum,

		COMMANDPARAMETER_Checksum_Only_CONST,
		&bChecksumOnly,
		
		COMMANDPARAMETER_Data_CONST,	//Data (Data) param 1			  
		&pUpdateFile,             
		&iSizeUpdateFile
	);		

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sending message...");
	Message *pReceivedMessage = pClientSocket->SendReceiveMessage( pMessage );

	pUpdateFile = pReceivedMessage->m_mapData_Parameters[COMMANDPARAMETER_Data_CONST]; 
	iSizeUpdateFile = pReceivedMessage->m_mapData_Lengths[COMMANDPARAMETER_Data_CONST];
	sChecksum = pReceivedMessage->m_mapParameters[COMMANDPARAMETER_Checksum_CONST];

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Message sent.");

	if ( !iSizeUpdateFile )
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL,  "The update file is missing on the server." );
		return 1; 
	}

	//save the Orbiter.MD5 file
	string sOrbiterMD5FileName = sOrbiterFileName;
	StringUtils::Replace(&sOrbiterMD5FileName, ".exe", ".MD5");
	StringUtils::Replace(&sOrbiterMD5FileName, ".EXE", ".MD5");

	LoggerWrapper::GetInstance()->Write( LV_CRITICAL,  "Ready to update MD5 file: %s", sOrbiterMD5FileName.c_str());

    bool bOrbiterMD5FilePathIsWrong = false;
    if(!FileUtils::FileExists(sOrbiterMD5FileName))
    {
        bOrbiterMD5FilePathIsWrong = true;
    }

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Saving MD5 file");
	if(!FileUtils::WriteBufferIntoFile(sOrbiterMD5FileName, const_cast<char *>(sChecksum.c_str()), sChecksum.length()))
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL,  "Failed to save MD5 file" );
		Sleep(1000); //we'll continue with this version
	}

#ifdef WINCE
	wchar_t OrbiterFileNameW[256];
	mbstowcs(OrbiterFileNameW, sOrbiterFileName.c_str(), 256);
#define ORBITER_FILE OrbiterFileNameW	
#else
#define ORBITER_FILE const_cast<char *>(sOrbiterFileName.c_str())
#endif		

	LoggerWrapper::GetInstance()->Write( LV_CRITICAL,  "Ready to delete the orbiter.exe file");
	::DeleteFile(ORBITER_FILE);

	while(!FileUtils::WriteBufferIntoFile(sOrbiterFileName, pUpdateFile, iSizeUpdateFile))
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL,  "Failed to save the update file" );
		Sleep(1000); //we'll continue with this version
	}

	delete pClientSocket;
	pClientSocket = NULL;

	WSACleanup();

	//read data from the comm file
	size_t iCommSize;
	char *pCommData = FileUtils::ReadFileIntoBuffer(sCommFile, iCommSize);

	string sOrbiterCommandLine(pCommData);
	
	delete pCommData;
	pCommData = NULL;

	//starting the new orbiter
	PROCESS_INFORMATION pi;
	::ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	STARTUPINFO si;
	::ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.lpReserved = 0;

	LoggerWrapper::GetInstance()->Write( LV_CRITICAL,  "Starting the updated orbiter..." );

#ifdef WINCE
	string sOrbiterPath = sOrbiterFileName;

	StringUtils::Replace(&sOrbiterCommandLine, sOrbiterFileName + " ", "");
	string sCmdLine = sOrbiterCommandLine;

	wchar_t CmdLineW[256];
	mbstowcs(CmdLineW, sCmdLine.c_str(), 256);
	wchar_t OrbiterPathW[256];
	mbstowcs(OrbiterPathW, sOrbiterPath.c_str(), 256);

	LoggerWrapper::GetInstance()->Write( LV_CRITICAL,  "Orbiter path: %s", sOrbiterPath.c_str() );
	LoggerWrapper::GetInstance()->Write( LV_CRITICAL,  "Orbiter cmd line: %s", sCmdLine.c_str() );

	wchar_t CommFileW[256];
	mbstowcs(CommFileW, sCommFile.c_str(), 256);
	#define COMM_FILE CommFileW	
#else
	#define COMM_FILE const_cast<char *>(sCommFile.c_str())
#endif		

    if(!bOrbiterMD5FilePathIsWrong)
    {
    	LoggerWrapper::GetInstance()->Write( LV_CRITICAL,  "Ready to delete the communcation file");
    	::DeleteFile(COMM_FILE);
    }
    else
    {
        LoggerWrapper::GetInstance()->Write( LV_CRITICAL,  "The communication file will not be removed because the OrbiterMD5 file path is not correct!");
    }

#ifdef WINCE
	if(!::CreateProcess(OrbiterPathW, CmdLineW, NULL, NULL, NULL, 0, NULL, NULL, &si, &pi))
#else
	if(!::CreateProcess(NULL, const_cast<char *>(sOrbiterCommandLine.c_str()), NULL, NULL, NULL, 0, NULL, NULL, &si, &pi))
#endif
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL,  "Failed to start Orbiter application '%s'", sOrbiterCommandLine.c_str() );
		return false; 
	}

	LoggerWrapper::GetInstance()->Write( LV_CRITICAL,  "Update done.");

	return 0;
}

