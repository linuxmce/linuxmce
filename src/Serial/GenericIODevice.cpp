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
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "Logger.h"
#include "GenericIODevice.h"

using namespace std;
using namespace DCE;

int GenericIODevice::TransmitCommand(string sCommand,Message *pMessage)
{
	StartWriting();
	const char *pCommand = sCommand.c_str();  // The string
	const char *pPos = pCommand;  // The character in the string we are parsing
	char cReading=0; // A flag T ; : ?  indicating if we're reading in a token, parm, format or translation, or nothing
	string sToken,sParms,sFormat,sTranslation;
	while( *pPos )
	{
		if( *pPos=='<' )
		{
			cReading='T';  // We're reading in the token
			pPos++;
			continue;
		}
		else if( cReading && (*pPos==';' || *pPos==':' || *pPos=='?') )
		{
			cReading=*pPos;
			pPos++;
			continue;
		}
		else if( cReading && *pPos=='>' )
		{
			ProcessToken(sToken,sParms,sFormat,sTranslation,pMessage);
			sToken="";  sParms="";  sFormat="";  sTranslation="";
			cReading=0;
			pPos++;
			continue;
		}
		else
		{
			char c = *pPos;

			if( *pPos=='\\' )
			{
				pPos++;
				switch( *pPos )
				{
				case 'n':
					c = '\n';
					break;
				case 'r':
					c = '\r';
					break;
				case 't':
					c = '\t';
					break;
				case '\\':
					c = '\\';
					break;
					// TODO -- handle numbers
				case 0:
					pPos--;
					break;  // Shouldn't happen.  The last character was a trailing \.  Just let it through
				}
			}

			if( cReading=='T' )
				sToken += c;
			else if( cReading==';' )
				sParms += c;
			else if( cReading==':' )
				sFormat += c;
			else if( cReading=='?' )
				sTranslation += c;
			else
				Write_unsigned_char(c);
		}

		pPos++;
	}
	return 0;
}

void GenericIODevice::ProcessToken(string sToken,string sParms,string sFormat,string sTranslation,Message *pMessage)
{
	if( sToken=="STX" )
		Write_unsigned_char('\002'); // confirm this
	else if( sToken=="ETX" )
		Write_unsigned_char('\003'); // confirm this
	else if( sToken=="NULL" )
		Write_unsigned_char('\003'); // confirm this
	else if( sToken=="C" )
	{
		string s=FormatAndTranslate(StringUtils::itos(pMessage->m_dwID),sFormat,sTranslation);
		Write_string(s);
	}
	else if( sToken=="CP" && sParms.length() )
	{
		int PK_CommandParameter = atoi( sParms.c_str() );
		string s=FormatAndTranslate(pMessage->m_mapParameters[PK_CommandParameter],sFormat,sTranslation);
		Write_string(s);
	}
}

string GenericIODevice::FormatAndTranslate(string sRawValue,string sFormat,string sTranslation)
{
	return "";
}


void GenericIODevice::ReceiveData(const char *pData,int iSize)
{
}

