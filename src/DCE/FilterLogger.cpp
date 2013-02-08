/* 
	FilterLogger
	
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU Lesser General Public License for more details.
*/

/*
 * $Id: FilterLogger.cpp,v 1.2 2004/10/28 05:11:28 mihaid Exp $
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

#ifndef WINCE
#include "PlutoUtils/TokenStream.h"
#endif

using namespace DCE;

///////////////////////////////////////////////////////////////////////////////////
FilterLogger::FilterLogger()
{
	m_GreaterThan = 1024;
	m_LessThan = -1024;
}

FilterLogger::~FilterLogger()
{
}

void FilterLogger::WriteEntry(class Logger::Entry& entry)
{
	PLUTO_SAFETY_LOCK_LOGGER(sSM,m_Lock);  // Don't log anything but failures
	bool ret = LevelMatch(entry.m_iLevel);
	if(m_Invert)
		ret = !ret;
	if(ret)
		MultiLogger::WriteEntry(entry);
}

FilterLogger* FilterLogger::Parse(TokenStream& ts, NameLoggerMap& targets, StringStringMap& vars)
{
	string token = ts.NextToken();
	if(token != "{")
	{
		fprintf(stderr, "Parse Error: filter sections need to start with a '{'\n");
		return NULL;
	}
	
	string Levels;
	LoggerVector loggers;
	Logger* logger;
	while(1)
	{
		token = ts.NextToken();
		if(token == "}")
			break;
			
		if(token == "output")
		{
			string name = ts.NextToken();
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
			logger = FilterLogger::Parse(ts, targets,vars);
			loggers.push_back(logger);
		}
		else if(token == "buffer")
		{
			logger = BufferedLogger::Parse(ts, targets, vars);
			loggers.push_back(logger);
		}
		else if(token == "level")
		{
			if(Levels.length() > 0)
				Levels +=string(",")+ts.NextToken();
			else
				Levels = ts.NextToken();
		}
		else
		{
			fprintf(stderr, "Parse Error: Unknown directive '%s'\n", token.c_str());
			return NULL;
		}
	}

	FilterLogger* NewLogger = new FilterLogger();
	NewLogger->m_Loggers = loggers;
	if(!NewLogger->ParseLevels(Levels))
	{
		fprintf(stderr, "Error parsing levels\n");
		delete NewLogger;
		return NULL;
	}
	return NewLogger;
}


bool FilterLogger::LevelMatch(int Level)
{
	if(Level > m_GreaterThan)
		return true;

	if(Level < m_LessThan)
		return true;
	if(m_Levels.count(Level) > 0)
		return true;
	return false;
}

bool FilterLogger::ParseLevels(::std::string str)
{
	string::size_type pos = 0;
	string token;
	while(1)
	{
		token = StringUtils::Tokenize(str, ",", pos);
		if(token == "")
			break;
		if(token[0] == '>')
		{
			int val = atoi(token.c_str()+1);
			if(val<m_GreaterThan)
				m_GreaterThan = val;
		}
		else if(token[0] == '<')
		{
			int val = atoi(token.c_str()+1);
			if(val>m_LessThan)
				m_LessThan = val;
		}
		else if (token.find('-'))
		{
			int loc = (int)token.find('-');
			int low_val = atoi(token.c_str());
			int high_val = atoi(token.c_str()+loc+1);
			if(high_val<low_val)
			{
				int temp = low_val;
				low_val = high_val;
				high_val = temp;
			}
			for(int i=low_val;i<=high_val;i++)
				m_Levels.insert(i);
		}
		else
		{
			int val = atoi(token.c_str());
			m_Levels.insert(val);
		}
		
	}
	return true;
}
