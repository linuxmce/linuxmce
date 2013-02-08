/* 
	BufferedLogger
	
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU Lesser General Public License for more details.
*/

/*
 * $Id: BufferedLogger.cpp,v 1.1 2004/10/12 16:21:56 aaron Exp $
 */

#include <time.h>
#ifndef WIN32
#include <sys/time.h>
#endif

using namespace DCE;

///////////////////////////////////////////////////////////////////////////////////
BufferedLogger::BufferedLogger()
{
	m_MaxEntries = 200;
	m_FlushDelay=0;
	m_AlarmManager = new AlarmManager();
	m_AlarmManager->Start(1);
}

BufferedLogger::~BufferedLogger()
{
	delete m_AlarmManager;
	m_AlarmManager = NULL;
}

void BufferedLogger::WriteEntry(class Logger::Entry& entry)
{
	PLUTO_SAFETY_LOCK_LOGGER(sSM,m_Lock);  // Don't log anything but failures
	if(m_Queue.size()==m_MaxEntries)
		m_Queue.pop();
	m_Queue.push(entry);

}

void BufferedLogger::Start()
{
	if(m_FlushDelay>0)
		m_AlarmManager->AddRelativeAlarm(m_FlushDelay, this, 0, NULL);
	MultiLogger::Start();
}

void BufferedLogger::Flush()
{
	PLUTO_SAFETY_LOCK_LOGGER(sSM,m_Lock);  // Don't log anything but failures
	while(!m_Queue.empty())
	{
		MultiLogger::WriteEntry(m_Queue.front());
		m_Queue.pop();
	}
}

void BufferedLogger::AlarmCallback(int id, void* param)
{
	Flush();
	m_AlarmManager->AddRelativeAlarm(m_FlushDelay, this, 0, NULL);
}

BufferedLogger* BufferedLogger::Parse(TokenStream& ts, NameLoggerMap& targets, StringStringMap& vars)
{
	string token = ts.NextToken();
	if(token != "{")
	{
		fprintf(stderr, "Parse Error: buffer sections need to start with a '{'\n");
		return NULL;
	}
	
	int Max = 200;
	int Flush = 5*60;
	
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
			logger = BufferedLogger::Parse(ts, targets,vars);
			loggers.push_back(logger);
		}
		else if(token == "max")
		{
			string max = ts.NextToken();
			Max = atoi(max.c_str());
		}
		else if(token == "flush")
		{
			string max = ts.NextToken();
			Flush = ParseTimeSpec(max);
		}
		else
		{
			fprintf(stderr, "Parse Error: Unknown directive '%s'\n", token.c_str());
			return NULL;
		}
	}

	BufferedLogger* NewLogger = new BufferedLogger();
	NewLogger->m_Loggers = loggers;
	NewLogger->m_MaxEntries = Max;
	NewLogger->m_FlushDelay = Flush;
	return NewLogger;
}


int BufferedLogger::ParseTimeSpec(::std::string str)
{
	int len = (int) str.length();
	char* ptr = (char*)str.c_str();
	int val = atoi(ptr);
	
	if(ptr[len-1] == 'd')		//They asked for days
		val = val*24*60*60;
	else if(ptr[len-1] == 'h')	// hours
		val = val*60*60;
	else if(ptr[len-1] == 'm')	// mins
		val = val*60;
	return val;
}

///////////////////////////////////////////////////////////////////////////////////

bool ParseTargets(TokenStream& ts, NameLoggerMap& targets, StringStringMap& vars)
{
	string token;
	targets.clear();

	token = ts.NextToken();
	if(token != "targets")
	{
		fprintf(stderr, "Parse Error: The targets section must come first\n");
		return false;
		
	}
	token = ts.NextToken();
	if(token != "{")
	{
		fprintf(stderr, "Parse Error: The targets section must start with a '{'\n");
		return false;
	}
	
	
	while(1)
	{
		token = ts.NextToken();
		if(token == "}")
			return true;
			
		Logger* logger = NULL;
		string name = "";
		if(token == "file")
		{
			name = ts.NextToken();
			logger = FileLogger::Parse(ts,vars);
		}
		else if(token == "server")
		{
			name = ts.NextToken();
			logger = ServerLogger::Parse(ts,vars);
		}
		else if(token == "email")
		{
			name = ts.NextToken();
			logger = EMailLogger::Parse(ts,vars);
		}
		else
		{
			fprintf(stderr, "Parse Error: unknown log type '%s'\n", token.c_str());
			return false;
		}
		if(logger == NULL)
			return false;
			
		targets[name] = logger;
	}
}

Logger* ParseComplexLogger(const char* name, StringStringMap& vars)
{
	fprintf(stderr, "Parsing: '%s'\n", name);
	FileTokenStream ts(name);

	NameLoggerMap targets;
	if(!ParseTargets(ts, targets,vars))
		return NULL;
		
	string token = ts.NextToken();
	if(token != "config")
	{
		fprintf(stderr, "Parse Error: The config section must come after the targets section\n");
		return false;
		
	}
	Logger* logger = MultiLogger::Parse(ts, targets, vars);
	
	return logger;
}


///////////////////////////////////////////////////////////////////////////////////
Logger* CreateLogger(int argc, char* argv[], bool start)
{
	// just grab the basename.  does windows have basename()? ( nope. -- rob.)
	char* name = strrchr(argv[0], '/');
	if(name == NULL)
	{
		name = strrchr(argv[0], '\\');
		if(name == NULL)
			name = argv[0];
		else
			name++;
	}
	else
		name++;

	//var bindings...
	StringStringMap vars;
	
	Logger* log = NULL;
	bool complex_logger = false;
	bool simple_logger = false;
	bool server_logger = false;
	string conf_file;
	
	for(int i=1;i<argc;i++)
	{
		if(strcmp(argv[i], "-L") == 0)
		{
			if(i+1 == argc)
				return NULL;
			if(simple_logger)			//we only allow a single 'top level' logger
				return NULL;
			complex_logger = true;
			conf_file = argv[++i];
		}
		if(strcmp(argv[i], "-l") == 0)
		{
			if(complex_logger)			//we only allow a single 'top level' logger
				return NULL;
			simple_logger = true;
			
			if(i+1 == argc)				//oops, no actual log type
				return NULL;

			string arg = argv[++i];
			if(arg == "stdout")
				log = new FileLogger(stdout);
			else if(arg == "stderr")
				log = new FileLogger(stderr);
			else if(arg.substr(0,5) == "file,")
				log = new FileLogger(arg.substr(5).c_str());
			else if(arg == "file")
			{
				// the default action is to make a log name based on the executable name
				string log_name = name;
				log_name+=".log";
				log = new FileLogger(log_name.c_str());
			}
			else if(arg == "none")
				log = new NullLogger();
			else if(arg == "null")
				log = new NullLogger();
			else if(arg == "server")
				server_logger = true;
		}
		else if(strcmp(argv[i], "-ocserver") == 0)
		{
			if(i+1 == argc)
				return NULL;
				
			string Val(argv[++i]);
			string::size_type pos=0;
			string temp = StringUtils::Tokenize(Val, ",", pos);
			vars["DEVICEID"] = StringUtils::Tokenize(Val, ",", pos);
			if(temp.find(':') != string::npos)
			{
				pos = 0;
				vars["OCSERVER"] = StringUtils::Tokenize(Val, ":", pos);
				vars["OCSERVER_PORT"] = StringUtils::Tokenize(Val, ":", pos);
			}
			else
			{
				vars["OCSERVER"] = temp;
				vars["OCSERVER_PORT"] = StringUtils::itos(OC_DEFAULT_PORT);
			}
		}
	}
	
	// Now that we got all the var bindings, make loggers that need 'em
	if(complex_logger)
		log = ParseComplexLogger(conf_file.c_str(), vars);
	else if(server_logger)	
		log =  new ServerLogger(atoi(vars["DEVICEID"].c_str()), vars["OCSERVER"]);

	if(log == NULL)
	{
		log = new FileLogger(stdout);	
		if(log == NULL)
			return NULL;
	}
	if(start)
		log->Start();
	log->SetName(name);

	return log;
}
