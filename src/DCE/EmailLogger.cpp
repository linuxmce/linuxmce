/* 
	EmailLogger
	
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU Lesser General Public License for more details.
*/

/*
 * $Id: DCEEmailLogger.cpp,v 1.2 2004/10/28 05:11:28 mihaid Exp $
 */

#include <time.h>
#ifndef WIN32
#include <sys/time.h>
#endif

#include "PlutoUtils/CommonIncludes.h"	
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "Message.h"
#include "PlutoUtils/TokenStream.h"

using namespace DCE;

/////////////////////////////////////////////////////////////


EMailLogger::EMailLogger(string to, string from, string subject)
{
	m_To = to;
	m_From = from;
	m_Subject = subject;
}

EMailLogger::~EMailLogger()
{
}

EMailLogger* EMailLogger::Parse(TokenStream& ts, StringStringMap& vars)
{
	string token;
	token = ts.NextToken();
	if(token != "{")
	{
		fprintf(stderr, "Parse Error: server sections need to start with a '{'\n");
		return NULL;
	}

	string To;
	string From = "logger@1control.com";
	string Subject = "Logger Notification";
	string Server = "localhost";
	while(1)
	{
		token = ts.NextToken();
		if(token == "}")
			break;
			
		if(token == "to")
			To = ts.NextToken();
		else if(token == "from")
			From = ts.NextToken();
		else if(token == "subject")
			Subject = ts.NextToken();
//		else if(token == "server")
//			Server = ts.NextToken();
		else
		{
			fprintf(stderr, "Parse Error: Unknown parameter '%s'\n", token.c_str());
			return NULL;
		}
	}
	
	if(To.length() == 0)
	{
		fprintf(stderr, "Parse Error: You must specify an address for 'to'\n");
		return NULL;
	}
	return new EMailLogger(To, From, Subject);
}

void EMailLogger::WriteEntry(Logger::Entry& entry)
{
#ifndef WIN32	

	FILE* f = popen("/usr/sbin/sendmail -bm -t", "w");
	if(f == NULL)
		return;
		
	PLUTO_SAFETY_LOCK_LOGGER(sSM,m_Lock);  // Don't log anything but failures
	fprintf(f, "From: %s\n", m_From.c_str());
	fprintf(f, "To: %s\n", m_To.c_str());
	fprintf(f, "Subject: %s\n\n\n", m_Subject.c_str());
	fprintf(f, "Level:\t%d\n", entry.Level);
	fprintf(f, "Device ID:\t%d\n", entry.DeviceID);
	struct tm t;
	localtime_r((time_t *)&entry.m_dwTime,&t);
	double sec = (double)(entry.m_dwMicroseconds/1E6) + t.tm_sec;
	fprintf(f,"Time/Date: %02d/%02d/%02d %d:%02d:%06.3f",(int) t.tm_mon+1,(int) t.tm_mday,(int) t.tm_year-100,(int) t.tm_hour,(int) t.tm_min, sec);
	fprintf(f, "Name:\t%s\n", entry.Name.c_str());
	fprintf(f, "Data:\t%s\n", entry.Name.c_str());
	pclose(f);
#endif
}
