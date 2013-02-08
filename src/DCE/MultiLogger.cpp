/* 
	MultiLogger
	
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU Lesser General Public License for more details.
*/

/*
 * $Id: MultiLogger.cpp,v 1.2 2004/10/28 05:11:28 mihaid Exp $
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
MultiLogger::MultiLogger()
{
}

MultiLogger::~MultiLogger()
{
	PLUTO_SAFETY_LOCK_LOGGER(sSM,m_Lock);  // Don't log anything but failures
	LoggerVector::iterator it;
	for(it=m_Loggers.begin();it!= m_Loggers.end();it++)
	{
		delete (*it);
		m_Loggers.erase(it);
	}
}

void MultiLogger::WriteEntry(class Logger::Entry& entry)
{
	PLUTO_SAFETY_LOCK_LOGGER(sSM,m_Lock);  // Don't log anything but failures
	LoggerVector::iterator it;
	for(it=m_Loggers.begin();it!= m_Loggers.end();it++)
		(*it)->WriteEntry(entry);
}

void MultiLogger::AddOutput(Logger* logger)
{
	PLUTO_SAFETY_LOCK_LOGGER(sSM,m_Lock);  // Don't log anything but failures
	m_Loggers.push_back(logger);
}

void MultiLogger::Start()
{
	PLUTO_SAFETY_LOCK_LOGGER(sSM,m_Lock);  // Don't log anything but failures
	LoggerVector::iterator it;
	for(it=m_Loggers.begin();it!= m_Loggers.end();it++)
		(*it)->Start();
}


Logger* MultiLogger::Parse(TokenStream &ts, NameLoggerMap& targets, StringStringMap& vars)
{
	string token;
	
	token = ts.NextToken();
	if(token != "{")
	{
		fprintf(stderr, "Parse Error: the config section need to start with a '{' not '%s'\n", token.c_str());
		return NULL;
	}
	
	string name;	
	vector<Logger*> loggers;
	Logger* logger;
	while(1)
	{
		token = ts.NextToken();
		if(token == "}")
			break;
			
		if(token == "output")
		{
			name = ts.NextToken();
			logger = targets[name];
			if(logger == NULL)
			{
				fprintf(stderr, "Error: Unknown target '%s'\n", name.c_str());
				return NULL;
			}
			loggers.push_back(logger);
		}
		else if(token == "filter")
		{
			logger = FilterLogger::Parse(ts, targets, vars);
			loggers.push_back(logger);
		}
		else if(token == "buffer")
		{
			logger = BufferedLogger::Parse(ts, targets, vars);
			loggers.push_back(logger);
		}
		else
		{
			fprintf(stderr, "Parse Error: Unknown directive '%s'\n", token.c_str());
			return NULL;
		}
	}
	
	if(loggers.size() == 1)
		return loggers[0];
	
	MultiLogger* NewLogger = new MultiLogger();
	NewLogger->m_Loggers = loggers;
	return NewLogger;
}

