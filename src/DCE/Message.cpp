/*
    Message

    Copyright (C) 2004 Pluto, Inc., a Florida Corporation

    www.plutohome.com

    Phone: +1 (877) 758-8648

    This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    See the GNU Lesser General Public License for more details.
*/

#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/uuencode.h"
#include "PlutoUtils/PlutoDefs.h"
#include "Message.h"

#include "DCE/Logger.h"

#ifdef WINCE
	#define stricmp _stricmp
#endif

using namespace DCE;

#ifdef LL_DEBUG
	// A counter used to mark messages so they can be traced in the low level logs
	int g_MessageID=0;
#endif

map<DataFormat, SerializeMessage> Message::m_mapSerializers;
map<DataFormat, DeserializeMessage> Message::m_mapDeserializers;

Message::Message()
{
    Clear();
}

Message::Message( unsigned long dwSize, char *pcData )
{
    Clear();
    FromData( dwSize, pcData );
    m_pcDataBlock = NULL; // Be sure the SerializeClass destructor doesn't also try to delete this
}

Message::Message( Message *pMessage_Clone )
{
    Clear();

    m_dwPK_Device_From = pMessage_Clone->m_dwPK_Device_From;
    m_dwPK_Device_To = pMessage_Clone->m_dwPK_Device_To;
    m_dwPK_Device_Group_ID_To = pMessage_Clone->m_dwPK_Device_Group_ID_To;
    m_dwID = pMessage_Clone->m_dwID;
    m_dwPriority = pMessage_Clone->m_dwPriority;
    m_dwMessage_Type = pMessage_Clone->m_dwMessage_Type;
    m_dwPK_Device_Category_To = pMessage_Clone->m_dwPK_Device_Category_To;
    m_dwPK_Device_Template = pMessage_Clone->m_dwPK_Device_Template;
    m_bIncludeChildren = pMessage_Clone->m_bIncludeChildren;
    m_bRelativeToSender = pMessage_Clone->m_bRelativeToSender;
    m_bRespondedToMessage = pMessage_Clone->m_bRespondedToMessage;
    m_eBroadcastLevel = pMessage_Clone->m_eBroadcastLevel;
    m_eRetry = pMessage_Clone->m_eRetry;
    m_eExpectedResponse = pMessage_Clone->m_eExpectedResponse;
    m_sPK_Device_List_To = pMessage_Clone->m_sPK_Device_List_To;

    map<long, string>::iterator itParms;
    for(itParms=pMessage_Clone->m_mapParameters.begin();itParms!=pMessage_Clone->m_mapParameters.end();++itParms)
    {
        m_mapParameters[(*itParms).first]=(*itParms).second;
    }

    map<long, char *>::iterator itDataParms;
    map<long, unsigned long>::iterator itDataLength;
    for(itDataLength=pMessage_Clone->m_mapData_Lengths.begin();itDataLength!=pMessage_Clone->m_mapData_Lengths.end();++itDataLength)
    {
        itDataParms = pMessage_Clone->m_mapData_Parameters.find( (*itDataLength).first );
        if( itDataParms != pMessage_Clone->m_mapData_Parameters.end() )
        {
            m_mapData_Lengths[(*itDataLength).first] = (*itDataLength).second;
            char *Buffer = new char[(*itDataLength).second];
            memcpy(Buffer,(*itDataParms).second,(*itDataLength).second);
            m_mapData_Parameters[(*itDataLength).first] = Buffer;
        }
    }

    vector<class Message *>::iterator itExtras;
    for(itExtras=pMessage_Clone->m_vectExtraMessages.begin();
        itExtras!=pMessage_Clone->m_vectExtraMessages.end();++itExtras)
    {
        Message *pMessage_Child_Clone = *itExtras;
        if( pMessage_Child_Clone )  // It could be NULL if this multi-part message was already broken into pieces because it had separate destinations
        {
            Message *pMessage_Child = new Message(pMessage_Child_Clone);
            m_vectExtraMessages.push_back(pMessage_Child);
        }
    }
}

Message::Message( int iNumArgs, char *cArguments[], int dwPK_DeviceFrom )
{
	BuildFromArgs( iNumArgs, cArguments, dwPK_DeviceFrom,NULL,false);
}

void Message::BuildFromArgs( int iNumArgs, char *cArguments[], int dwPK_DeviceFrom, Message *pMessage_Parent,bool bDeleteArray/*=true*/)
{
    Clear();

	m_eBroadcastLevel = BL_SameHouse;
	m_eRetry = MR_None;

	int baseMessageSpecPos = 0;
	int targetType = 0; // Device;
	bool bResponseRequired=false,bOutParams=false;
	while( iNumArgs>baseMessageSpecPos && cArguments[baseMessageSpecPos][0]=='-' )
	{
		if ( stricmp(cArguments[baseMessageSpecPos], "-targetType") == 0 )
		{
			if ( strcmp(cArguments[baseMessageSpecPos+1], "category") == 0 )
				targetType = 1;
			else if ( strcmp(cArguments[baseMessageSpecPos+1], "template") == 0 )
				targetType = 2;
			baseMessageSpecPos += 2;
		}
		else if ( strcmp(cArguments[baseMessageSpecPos], "-r") == 0 )
		{
			bResponseRequired=true;
			baseMessageSpecPos++;
		}
		else if ( strcmp(cArguments[baseMessageSpecPos], "-o") == 0 )
		{
			bOutParams=true;
			baseMessageSpecPos++;
		}
        else if ( strcmp(cArguments[baseMessageSpecPos], "-p") == 0 )
        {
            baseMessageSpecPos++;
            sOutputParametersPath = cArguments[baseMessageSpecPos];
            baseMessageSpecPos++;
        }
		else if ( strcmp(cArguments[baseMessageSpecPos], "-bl") == 0 )
		{
			baseMessageSpecPos++;
			if ( strcmp(cArguments[baseMessageSpecPos], "none") == 0)
				m_eBroadcastLevel = BL_None;
			else if ( strcmp(cArguments[baseMessageSpecPos], "direct_siblings") == 0 )
				m_eBroadcastLevel = BL_DirectSiblings;
			else if ( strcmp(cArguments[baseMessageSpecPos], "same_computer") == 0 )
				m_eBroadcastLevel = BL_SameComputer;
			else if ( strcmp(cArguments[baseMessageSpecPos], "same_room") == 0 )
				m_eBroadcastLevel = BL_SameRoom;
			else if ( strcmp(cArguments[baseMessageSpecPos], "same_house") == 0 )
				m_eBroadcastLevel = BL_SameHouse;
			else if ( strcmp(cArguments[baseMessageSpecPos], "all_houses") == 0 )
				m_eBroadcastLevel = BL_AllHouses;
			baseMessageSpecPos ++;
		}
		else if ( strcmp(cArguments[baseMessageSpecPos], "-retry") == 0 )
		{
			baseMessageSpecPos++;
			if ( strcmp(cArguments[baseMessageSpecPos], "none") == 0)
				m_eRetry = MR_None;
			else if ( strcmp(cArguments[baseMessageSpecPos], "retry") == 0 )
				m_eRetry = MR_Retry;
			else if ( strcmp(cArguments[baseMessageSpecPos], "persist") == 0 )
				m_eRetry = MR_Persist;
			baseMessageSpecPos ++;
		}
		else
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Create message Unknown argument: %s",cArguments[baseMessageSpecPos++]);
	}

	if( bResponseRequired )
		m_eExpectedResponse = ER_DeliveryConfirmation;  // i.e. just an "OK"
	if( bOutParams )
		m_eExpectedResponse = ER_ReplyMessage;

	if( baseMessageSpecPos+3 >= iNumArgs )
    {
        if(bDeleteArray)
            PLUTO_SAFE_DELETE_ARRAY_OF_ARRAYS(cArguments, iNumArgs);

		return; // Invalid message
    }

	m_dwPK_Device_From = dwPK_DeviceFrom ? dwPK_DeviceFrom : atoi(cArguments[baseMessageSpecPos]);
	string sDeviceTo=cArguments[baseMessageSpecPos + 1];
	m_dwMessage_Type=atoi(cArguments[baseMessageSpecPos + 2]);
	m_dwID=atoi(cArguments[baseMessageSpecPos + 3]);
	m_dwPriority = PRIORITY_NORMAL;
	m_bIncludeChildren = true;

	if( targetType==1 )
	{
	    m_dwPK_Device_To = DEVICEID_CATEGORY;
	    m_dwPK_Device_Category_To = atoi(sDeviceTo.c_str());
	}
	else if( targetType==2)
	{
	    m_dwPK_Device_To = DEVICEID_MASTERDEVICE;
		m_dwPK_Device_Template = atoi(sDeviceTo.c_str());
	}
	else if( sDeviceTo.find(",")!=string::npos )
	{
		m_dwPK_Device_To = DEVICEID_LIST;
		m_sPK_Device_List_To = sDeviceTo;
	}
	else
	    m_dwPK_Device_To = atoi(sDeviceTo.c_str());

    for(int i=baseMessageSpecPos + 4; i<iNumArgs; i+=2)
    {
		enum { ptNormal, ptData, ptBinary, ptText, ptUU, ptOut } eType = ptNormal;

        char *pParamID = cArguments[i];
		if( pParamID[0]=='&' )
		{
			Message *pMessage = new Message();
			pMessage->BuildFromArgs(iNumArgs-i-1, &cArguments[i+1], dwPK_DeviceFrom, this, false);
			if( pMessage_Parent )
				pMessage_Parent->m_vectExtraMessages.push_back(pMessage);
			else
				m_vectExtraMessages.push_back(pMessage);

            if(bDeleteArray)
                PLUTO_SAFE_DELETE_ARRAY_OF_ARRAYS(cArguments, iNumArgs);

			return;
		}
		else if( pParamID[0]=='D' )
		{
			pParamID++;
			eType=ptData;
		}
		else if( pParamID[0]=='B' )
		{
			pParamID++;
			eType=ptBinary;
		}
		else if( pParamID[0]=='T' )
		{
			pParamID++;
			eType=ptText;
		}
		else if( pParamID[0]=='U' )
		{
			pParamID++;
			eType=ptUU;
		}
		int ParamNum = atoi(pParamID);

		if(!ParamNum)
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Message constructor malformed with wrong param id '%s'", pParamID);
			Clear();
			return;
		}

		size_t tSizeParmValue = 0;
		if( i+1>=iNumArgs )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Message constructor malformed with missing argument from %d to %s type %d id %d",
				m_dwPK_Device_From,sDeviceTo.c_str(),m_dwMessage_Type,m_dwID);
			Clear();
			return;
		}
		char *pParmValue = cArguments[i+1];
		if( eType==ptBinary || eType==ptText )
		{
			char *pFileName = pParmValue;
			pParmValue = FileUtils::ReadFileIntoBuffer( pFileName, tSizeParmValue );
			if( !pParmValue )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Message Parm cannot read file: %s",pFileName);
				continue;
			}
		}
		else if( eType==ptUU )
		{
			int iUUSize = int(strlen(pParmValue));
			unsigned char *pBinaryValue = new unsigned char[MaxDecodedSize(iUUSize)];
			tSizeParmValue = Ns_HtuuDecode((unsigned char *) pParmValue, pBinaryValue, iUUSize);
			pParmValue=(char *) pBinaryValue;
		}
		else
        {
			if( eType==ptData )
				tSizeParmValue = strlen( pParmValue );
		}

		if( !pParmValue )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Bad value for parameter ID: %d",ParamNum);
            if(bDeleteArray)
                PLUTO_SAFE_DELETE_ARRAY_OF_ARRAYS(cArguments, iNumArgs);
			return;
		}
		if( eType==ptData || eType==ptBinary || eType==ptUU )
		{
			m_mapData_Parameters[ParamNum] = pParmValue;
			m_mapData_Lengths[ParamNum] = (unsigned long)tSizeParmValue;
		}
		else
			m_mapParameters[ParamNum] = pParmValue;
    }

	m_pcDataBlock = NULL; // Be sure the SerializeClass destructor doesn't also try to delete this

    if(bDeleteArray)
        PLUTO_SAFE_DELETE_ARRAY_OF_ARRAYS(cArguments, iNumArgs);
}

Message::Message( string sMessageInStringFormat )
{
	int iNumArgs;
	int iPosNext=0;
	char **pArgs = StringUtils::ConvertStringToArgs(sMessageInStringFormat,iNumArgs,&iPosNext);
	BuildFromArgs( iNumArgs, pArgs );

	int iPosNextCumulative=iPosNext;
	while( iPosNext )  // If there's not at least 5 characters it can't be a valid message
	{
		// There are more messages
		pArgs = StringUtils::ConvertStringToArgs(sMessageInStringFormat.substr(iPosNextCumulative),iNumArgs,&iPosNext);
		if( iNumArgs )
		{
			iPosNextCumulative+=iPosNext;
			Message *pMessage = new Message();
			pMessage->BuildFromArgs( iNumArgs, pArgs );
			m_vectExtraMessages.push_back(pMessage);
		}
		else
			PLUTO_SAFE_DELETE_ARRAY_OF_ARRAYS(pArgs, iNumArgs);
	}
}

Message::Message( long dwDeviceIDFrom, long dwDeviceIDTo, long dwPriority, long dwMessageType, long dwID, unsigned long dwParameterCount, ... )
{
    Clear();

    m_dwPK_Device_From = dwDeviceIDFrom;
    m_dwPK_Device_To = dwDeviceIDTo;
    m_dwID = dwID;
    m_dwMessage_Type = dwMessageType;
    m_dwPriority = dwPriority;

    va_list marker;
    va_start( marker, dwParameterCount );
    for( unsigned long i=0; i < dwParameterCount; i++ )
    {
        int Key = va_arg( marker, int );
        if ( Key >= 0 ) // if Key > 0 it is a normal string parameter, otherwise i's a data block
        {
            m_mapParameters[Key] = string( va_arg( marker, char* ) );
        }
        else
        {
            char *pcData = va_arg( marker, char* );
            unsigned int dwSize = va_arg( marker, unsigned int );
            m_mapData_Lengths[-Key] = dwSize;
            char *pcInData = new char[dwSize];
            memcpy( pcInData, pcData, dwSize );
            m_mapData_Parameters[-Key] = pcInData;
        }
    }
    va_end( marker );
}

Message::Message( long dwDeviceIDFrom, string dwDeviceIDTo, long dwPriority, long dwMessageType, long dwID, unsigned long dwParameterCount, ... )
{
    Clear();

    m_dwPK_Device_From = dwDeviceIDFrom;
    m_dwPK_Device_To = DEVICEID_LIST;
    m_sPK_Device_List_To = dwDeviceIDTo;
    m_dwID = dwID;
    m_dwMessage_Type = dwMessageType;
    m_dwPriority = dwPriority;

    va_list marker;
    va_start( marker, dwParameterCount );
    for( unsigned long i=0; i<dwParameterCount; i++ )
    {
        int Key = va_arg( marker, int );
        if ( Key >= 0 ) // if Key > 0 it is a normal string parameter, otherwise i's a data block
        {
            m_mapParameters[Key] = string( va_arg( marker, char* ));
        }
        else
        {
            char *pcData = va_arg( marker, char* );
            unsigned int dwSize = va_arg( marker, unsigned int );
            m_mapData_Lengths[-Key] = dwSize;
            char *pcInData = new char[dwSize];
            memcpy( pcInData, pcData, dwSize );
            m_mapData_Parameters[-Key] = pcInData;
        }
    }
    va_end( marker );
}

Message::Message( long dwDeviceIDFrom, long dwDeviceCategory, bool bIncludeChildren, eBroadcastLevel eBL, long dwPriority, long dwMessageType, long dwID, unsigned long dwParameterCount, ... )
{
    Clear();

    m_dwPK_Device_From = dwDeviceIDFrom;
    m_dwPK_Device_To = DEVICEID_CATEGORY;
    m_dwPK_Device_Category_To = dwDeviceCategory;
    m_bIncludeChildren = bIncludeChildren;
    m_eBroadcastLevel = eBL;
    m_dwID = dwID;
    m_dwMessage_Type = dwMessageType;
    m_dwPriority = dwPriority;

    va_list marker;
    va_start( marker, dwParameterCount );
    for( unsigned long i=0; i < dwParameterCount; i++ )
    {
        int Key = va_arg( marker, int );
        if ( Key >= 0 ) // if Key > 0 it is a normal string parameter, otherwise i's a data block
        {
            m_mapParameters[Key] = string( va_arg( marker, char* ) );
        }
        else
        {
            char *pcData = va_arg( marker, char* );
            unsigned int dwSize = va_arg( marker, unsigned int );
            m_mapData_Lengths[-Key] = dwSize;
            char *pcInData = new char[dwSize];
            memcpy( pcInData, pcData, dwSize );
            m_mapData_Parameters[-Key] = pcInData;
        }
    }
    va_end( marker );
}
// This allows for a message to be sent to a particular type of device
Message::Message(long dwDeviceIDFrom, long dwMasterDevice, eBroadcastLevel eBL, long dwPriority, long dwMessageType, long dwID, unsigned long dwParameterCount, ...)
{
    Clear();
    m_dwPK_Device_From = dwDeviceIDFrom;
    m_dwPK_Device_To = DEVICEID_MASTERDEVICE;
    m_dwPK_Device_Template = dwMasterDevice;
    m_eBroadcastLevel = eBL;
    m_dwID = dwID;
    m_dwMessage_Type = dwMessageType;
    m_dwPriority = dwPriority;

    va_list marker;
    va_start( marker, dwParameterCount );
    for( unsigned long i=0; i < dwParameterCount; i++ )
    {
        int Key = va_arg( marker, int );
        if ( Key >= 0 )
        {
            m_mapParameters[Key] = string( va_arg( marker, char* ) );
        }
        else
        {
            char *pcData = va_arg( marker, char* );
            unsigned int dwSize = va_arg( marker, unsigned int );
            m_mapData_Lengths[-Key] = dwSize;
            char *pcInData = new char[dwSize];
            memcpy( pcInData, pcData, dwSize );
            m_mapData_Parameters[-Key] = pcInData;
        }
    }
    va_end( marker );
}

Message::~Message()
{
    Clear();
}

void Message::Clear()
{
#ifdef LL_DEBUG
	m_MessageID = g_MessageID++;
#else
	m_MessageID = 0;
#endif

	m_bCanBuffer = true;
	m_bRelativeToSender = true;
    m_eExpectedResponse = ER_None;
    m_eBroadcastLevel = BL_SameHouse; // Default broadcast level
    m_eRetry = MR_None; // Default retry
    m_bRespondedToMessage = false;
    m_dwPriority = 0;
    m_dwPK_Device_From = -1;
    m_dwPK_Device_To = -1;
    m_dwPK_Device_Group_ID_To = 0;
    m_dwID = -1;
    m_dwMessage_Type = 0;
    m_dwPK_Device_Category_To = 0;
    m_dwPK_Device_Template = 0;
    m_bIncludeChildren = false;

    m_mapParameters.clear();

    for(vector<Message *>::iterator it = m_vectExtraMessages.begin(); it != m_vectExtraMessages.end(); ++it)
        delete *it;
    m_vectExtraMessages.clear();

	map<long, char *>::iterator i;
	for(i=m_mapData_Parameters.begin();i!=m_mapData_Parameters.end();++i)
	{
		if( (*i).second )
			delete [] (*i).second;
	}

	m_mapData_Parameters.clear();
    m_mapData_Lengths.clear();
}

void Message::ToData( unsigned long &dwSize, char* &pcData, bool bWithHeader )
{
    // See comments at end of this message to see why we don't delete the memory allocated in SerializeClass
    StartWriting();
    unsigned long dwHeaderPosition=0, dwStartOfMessagePosition=0;

    if ( bWithHeader )
    {
        dwHeaderPosition = static_cast<unsigned long>(m_pcCurrentPosition - m_pcDataBlock) + 8;
#ifndef WIN32
#ifdef LL_DEBUG
        string s("MESSAGE                      ");     //possibly this is a compiler bug  - gcc 3.3.3
#else
        string s("MESSAGE               ");     //possibly this is a compiler bug  - gcc 3.3.3
#endif
                                //complains about call to non-existing function
        Write_string(s);
#else

#ifdef LL_DEBUG
		Write_string( string( "MESSAGE                      " ) );
#else
		Write_string( string( "MESSAGE               " ) );
#endif

#endif
        // Save a pointer to store the header size
        dwStartOfMessagePosition = static_cast<unsigned long>(m_pcCurrentPosition - m_pcDataBlock);
    }

    Write_long( 1234 ); // Just a sanity check
    Write_long( m_dwPK_Device_From );
    Write_long( m_dwPK_Device_To );
    Write_long( m_dwPK_Device_Group_ID_To );
    Write_long( m_dwID );
    Write_long( m_dwPriority );
    Write_long( m_dwMessage_Type );
    Write_long( m_dwPK_Device_Category_To );
    Write_long( m_dwPK_Device_Template );
    Write_unsigned_char( m_bIncludeChildren );
    Write_long( m_eBroadcastLevel );
    Write_long( m_eRetry );
    Write_unsigned_char( m_bRelativeToSender );
    Write_long( m_eExpectedResponse );
    Write_string( m_sPK_Device_List_To );

    Write_unsigned_long( (unsigned long) m_mapParameters.size() );
    map<long, string>::iterator itParms;
    for( itParms=m_mapParameters.begin(); itParms != m_mapParameters.end(); ++itParms)
    {
        Write_long( (*itParms).first );
        Write_string( (*itParms).second );
    }

    map<long, char *>::iterator itDataParms;
    map<long, unsigned long>::iterator itDataLength;
    Write_unsigned_long((unsigned long) m_mapData_Lengths.size());
    for( itDataLength=m_mapData_Lengths.begin(); itDataLength != m_mapData_Lengths.end(); ++itDataLength )
    {
        itDataParms = m_mapData_Parameters.find( (*itDataLength).first );
        if( itDataParms != m_mapData_Parameters.end() )
        {
            char *Ptr = (*itDataParms).second;
            unsigned long Length = (*itDataLength).second;
            if( !Ptr ) Length=0;
            Write_long( (*itDataLength).first );
            Write_unsigned_long( Length );
            Write_block( Ptr, Length );
        }
        else
        {
            // We have to write something because we already counted this in the list
            Write_long( (*itDataLength).first );
            Write_unsigned_long(0);
        }
    }

    Write_unsigned_long( (unsigned long) m_vectExtraMessages.size() );
    vector<class Message *>::iterator itExtras;
    for( itExtras=m_vectExtraMessages.begin(); itExtras!=m_vectExtraMessages.end(); ++itExtras )
    {
        Message *pMessage_Child = *itExtras;
        if( !pMessage_Child )  // The embedded message could have been deleted
            Write_unsigned_long( 0 );
        else
        {
            char *Data;
            unsigned long Size;
            pMessage_Child->ToData( Size, Data, false );
            Write_unsigned_long( Size );
            Write_block( Data, Size );
            delete[] Data;
        }
    }

    Write_unsigned_long(6789); // Just a sanity check

    if (bWithHeader)
    {
        unsigned long MessageSize = (unsigned long) ((m_pcCurrentPosition - m_pcDataBlock) - dwStartOfMessagePosition );
        char *pcFreezePosition = m_pcCurrentPosition;
        m_pcCurrentPosition = m_pcDataBlock + dwHeaderPosition;
        string Size = StringUtils::itos( MessageSize );
#ifdef LL_DEBUG
		Size += "," + StringUtils::itos(m_MessageID);
#endif
        // Don't do a Write_String(StringUtils::itos(MessageSize)) because that will add a null term
        Write_block((char *)Size.c_str(), static_cast<unsigned long>(Size.length()));
        m_pcCurrentPosition = pcFreezePosition;
    }

    // Normally we would want to delete the memory in serialize class.  But here we assign the 'out' data pointer pcData
    // to the memory block.  Socket::SendMessage needs the data block since that's what it sends, and it will delete it there.
    pcData = m_pcDataBlock;
    dwSize = static_cast<unsigned long>(m_pcCurrentPosition - m_pcDataBlock);

    m_pcDataBlock=NULL; // Be sure the SerializeClass destructor doesn't also try to delete this

    return;
}

string Message::ToString( bool bWithHeader )
{
	string sOutput;

	if( m_dwPK_Device_To == DEVICEID_CATEGORY )
	{
		sOutput += "-targetType category ";
		m_dwPK_Device_To = m_dwPK_Device_Category_To;
	}
	else if( m_dwPK_Device_To == DEVICEID_MASTERDEVICE )
	{
		sOutput += "-targetType template ";
		m_dwPK_Device_To = m_dwPK_Device_Template;
	}

	if( m_eExpectedResponse == ER_DeliveryConfirmation )
		sOutput += "-o ";
	else if( m_eExpectedResponse == ER_ReplyString || m_eExpectedResponse == ER_DeliveryConfirmation )
		sOutput += "-r ";

	sOutput += StringUtils::itos(m_dwPK_Device_From) + " ";
	if( m_dwPK_Device_To == DEVICEID_LIST )
		sOutput += m_sPK_Device_List_To + " ";
	else
		sOutput += StringUtils::itos(m_dwPK_Device_To) + " ";

	sOutput += StringUtils::itos(m_dwMessage_Type) + " " + StringUtils::itos(m_dwID) + " ";

	for(map<long, string>::iterator itP=m_mapParameters.begin();itP!=m_mapParameters.end();++itP)
		sOutput += StringUtils::itos(itP->first) + " \"" + StringUtils::Replace(itP->second,"\"","\\\"") + "\" ";

	for(map<long, char *>::iterator itD=m_mapData_Parameters.begin();itD!=m_mapData_Parameters.end();++itD)
	{
		sOutput += "U" + StringUtils::itos(itD->first) + " \"";
		size_t SizeRaw = m_mapData_Lengths[itD->first];

		size_t SizeEncoded = MaxEncodedSize(SizeRaw);
		char *pDataEncoded = new char[SizeEncoded];
		int Bytes=Ns_HtuuEncode((unsigned char *) itD->second, static_cast<unsigned int>(SizeRaw), (unsigned char *) pDataEncoded);
		pDataEncoded[Bytes]=0;
		sOutput += pDataEncoded;
		delete[] pDataEncoded;
		sOutput += "\" ";
	}

    vector<class Message *>::iterator itExtras;
    for( itExtras=m_vectExtraMessages.begin(); itExtras!=m_vectExtraMessages.end(); ++itExtras )
    {
        Message *pMessage_Child = *itExtras;
        if( pMessage_Child )  // The embedded message could have been deleted
			sOutput += "\"&\" " + pMessage_Child->ToString(false) + " ";
    }

	if( bWithHeader )
	{
		string::size_type size = sOutput.size();
		sOutput="MESSAGET " + StringUtils::itos(static_cast<int>(size)) + "\n" + sOutput;
	}
	return sOutput;
}

void Message::FromData( unsigned long dwSize, char *pcData )
{
    StartReading( dwSize, pcData );

    // making sure that pcData contains what it should
    unsigned long dwStart = Read_unsigned_long(); // the first long should be 1234 by convention
    if( dwStart != 1234 )
    {
        LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Badly formed message" );
    }

    // reading the member data

    m_dwPK_Device_From = Read_long();
    m_dwPK_Device_To = Read_long();
    m_dwPK_Device_Group_ID_To = Read_long();
    m_dwID = Read_long();
    m_dwPriority = Read_long();
    m_dwMessage_Type = Read_long();
    m_dwPK_Device_Category_To = Read_long();
    m_dwPK_Device_Template = Read_long();
    m_bIncludeChildren=(Read_unsigned_char()==1);
    m_eBroadcastLevel=(eBroadcastLevel) Read_long();
    m_eRetry=(eRetry) Read_long();
    m_bRelativeToSender = Read_unsigned_char()==1;
    m_eExpectedResponse=(eExpectedResponse) Read_long();
    Read_string( m_sPK_Device_List_To );

    if( dwStart != 1234 ) {
	// dump debug info
	LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "**** dwStart: %d", dwStart );
	Dump( LV_CRITICAL );
    }

    unsigned long dwNumParms = Read_unsigned_long();
    for( unsigned long dwNP = 0; dwNP < dwNumParms; ++dwNP )
    {
        unsigned long dwID = Read_unsigned_long();
        string sValue;
        Read_string( sValue );
        m_mapParameters[dwID] = sValue;
    }

    unsigned long dwNumData = Read_unsigned_long();
    for( unsigned long dwND = 0; dwND < dwNumData; ++dwND )
    {
        unsigned long dwID = Read_unsigned_long();
        unsigned long Size = Read_unsigned_long();
        if( Size )
        {
            char *Data = Read_block( Size );
            m_mapData_Lengths[dwID] = Size;
            m_mapData_Parameters[dwID] = Data;
        }
    }

    unsigned long dwNumChildren = Read_unsigned_long();
    for( unsigned long dwNC = 0; dwNC < dwNumChildren; ++dwNC )
    {
        unsigned long Size = Read_unsigned_long();
        if( Size )
        {
            Message *pMessage_Child = new Message( Size, m_pcCurrentPosition );
            m_vectExtraMessages.push_back(pMessage_Child);
            m_pcCurrentPosition += Size;
        }
    }

    // checking again if the message is in propper form (it's ending how it should)
    unsigned long dwEnd = Read_unsigned_long();
    if( dwEnd!=6789 )
    {
        LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Badly formed message" );
	// dump debug info
	LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "**** dwEnd: %d", dwEnd );
	Dump( LV_CRITICAL );
    }

    return;
}

void Message::Dump( int iLogLevel /*=LV_DEBUG*/)
{
	LoggerWrapper::GetInstance()->Write( iLogLevel, "**** m_dwPK_Device_From: %d", m_dwPK_Device_From );
	LoggerWrapper::GetInstance()->Write( iLogLevel, "**** m_dwPK_Device_To: %d", m_dwPK_Device_To );
	LoggerWrapper::GetInstance()->Write( iLogLevel, "**** m_dwPK_Device_Group_ID_To: %d", m_dwPK_Device_Group_ID_To );
	LoggerWrapper::GetInstance()->Write( iLogLevel, "**** m_dwID: %d", m_dwID );
	LoggerWrapper::GetInstance()->Write( iLogLevel, "**** m_dwPriority: %d", m_dwPriority );
	LoggerWrapper::GetInstance()->Write( iLogLevel, "**** m_dwMessage_Type: %d", m_dwMessage_Type );
	LoggerWrapper::GetInstance()->Write( iLogLevel, "**** m_dwPK_Device_Category_To: %d", m_dwPK_Device_Category_To );
	LoggerWrapper::GetInstance()->Write( iLogLevel, "**** m_dwPK_Device_Template: %d", m_dwPK_Device_Template );
	LoggerWrapper::GetInstance()->Write( iLogLevel, "**** m_bIncludeChildren: %d", m_bIncludeChildren );
	LoggerWrapper::GetInstance()->Write( iLogLevel, "**** m_eBroadcastLevel: %d", m_eBroadcastLevel );
	LoggerWrapper::GetInstance()->Write( iLogLevel, "**** m_eRetry: %d", m_eRetry );
	LoggerWrapper::GetInstance()->Write( iLogLevel, "**** m_bRelativeToSender: %d", m_bRelativeToSender );
	LoggerWrapper::GetInstance()->Write( iLogLevel, "**** m_eExpectedResponse: %d", m_eExpectedResponse );
}

/*static*/ void Message::RegisterSerializer(DataFormat format, SerializeMessage pfSerializeMessage)
{
	m_mapSerializers[format] = pfSerializeMessage;
}

/*static*/ void Message::RegisterDeserializer(DataFormat format, DeserializeMessage pfDeserializeMessage)
{
	m_mapDeserializers[format] = pfDeserializeMessage;
}

/*virtual*/ bool Message::SerializeToData(DataFormat format, char *&pData, size_t& nSize)
{
	map<DataFormat, SerializeMessage>::iterator it = m_mapSerializers.find(format);
	if(it != m_mapSerializers.end())
	{
		SerializeMessage pfSerializeMessage = it->second;
		return (*pfSerializeMessage)(this, pData, nSize);
	}

	return false;
}

/*virtual*/ bool Message::DeserializeFromData(DataFormat format, char *pData, size_t nSize)
{
	map<DataFormat, DeserializeMessage>::iterator it = m_mapDeserializers.find(format);
	if(it != m_mapDeserializers.end())
	{
		DeserializeMessage pfDeserializeMessage = it->second;
		return (*pfDeserializeMessage)(this, pData, nSize);
	}

	return false;
}

