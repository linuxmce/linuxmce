/* 
	ParseLogger
	
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU Lesser General Public License for more details.
*/

/*
 * $Id: DCEParseLogger.cpp,v 1.1 2004/10/12 16:21:56 aaron Exp $
 */

#include <time.h>
#ifndef WIN32
#include <sys/time.h>
#endif


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
