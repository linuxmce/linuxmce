#ifndef VDRCommon_h
#define VDRCommon_h

#include "DCE/PlainClientSocket.h"

#define VDR_SOCKET_TIMEOUT	3  // SECONDS

bool SendOnlyVDRCommand(string sIP, string sCommand)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"svdrPort send-only starting to connect");
	PlainClientSocket _PlainClientSocket(sIP + ":2001");
	if( !_PlainClientSocket.Connect() )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Unable to connect to VDR client");
		return false;
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"svdrPort send-only connected");
	string sResponse;
	if( !_PlainClientSocket.ReceiveString(sResponse,VDR_SOCKET_TIMEOUT) || sResponse.substr(0,3)!="220" )
	{
//		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDR not ready got %s",sResponse.c_str());
		return false;
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"svdrPort response received (%s)",sResponse.c_str());
	if( !_PlainClientSocket.SendString(sCommand) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"SendVDRCommand Could not send string");
		return false;
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"svdrPort send-only sending command done");
	return true;
}	

bool SendVDRCommand(string sIP, string sCommand,string &sVDRResponse)
{
//	LoggerWrapper::GetInstance()->Write(LV_WARNING,"SendVDRCommand Going to send command %s",sCommand.c_str());
	if ( sCommand == "HITK Up" || sCommand == "HITK Down" ) 
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"svdrPort using SendOnly for %s", sCommand.c_str());
		bool bResult;
		bResult = SendOnlyVDRCommand(sIP,sCommand);
		return bResult;
	}

	PlainClientSocket _PlainClientSocket(sIP + ":2001");
	if( !_PlainClientSocket.Connect() )
	{
//		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Unable to connect to VDR client");
		return false;
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"svdrPort connected");
	string sResponse;
	if( !_PlainClientSocket.ReceiveString(sResponse,VDR_SOCKET_TIMEOUT) || sResponse.substr(0,3)!="220" )
	{
//		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDR not ready got %s",sResponse.c_str());
		return false;
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"svdrPort response received (%s)",sResponse.c_str());
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"svdrPort sending command %s",sCommand.c_str());
	if( !_PlainClientSocket.SendString(sCommand) )
	{
//		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"SendVDRCommand Could not send string");
		return false;
	}

	for(int iLines=0;;++iLines)
	{
		char *pData = NULL;
		int nSize = 0;

		if(!_PlainClientSocket.ReceiveDataDelimited(nSize, pData, '\n') || NULL == pData)
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDR not ok with command got %d lines then failed: %s",iLines,sVDRResponse.c_str());
			return false;
		}

		//we want a string
		string sResponse(pData);

		//deallocated received data buffer
		delete [] pData;
		pData = NULL;

//		LoggerWrapper::GetInstance()->Write(LV_STATUS,"VDR line %d: %s", iLines,sResponse.c_str());
		
		if( sResponse.substr(0,3)!="250" && sResponse.substr(0,3)!="215" && sResponse.substr(0,3)!="550" )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDR not ok with command got %d lines then got: %s",iLines,sResponse.c_str());
			return false;
		}

		if( sResponse.size()>4 )
			sVDRResponse += sResponse.substr(4);

		if( sResponse[3]!='-' )
			break;
	}
//	LoggerWrapper::GetInstance()->Write(LV_WARNING,"VDR Responded %s",sResponse.c_str());
	if( !_PlainClientSocket.SendString("QUIT") )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"SendVDRCommand Could not send string");
		return false;
	}

	sResponse="";
	if( !_PlainClientSocket.ReceiveString(sResponse,VDR_SOCKET_TIMEOUT) || sResponse.substr(0,3)!="221" )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"SendVDRCommand VDR not ok with quit got %s",sResponse.c_str());
		return false;
	}
	
	_PlainClientSocket.Close();
	return true;
}

#endif
