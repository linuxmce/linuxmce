/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU Lesser General Public License for more details.

*/

/** @file MultiLogger.h
 Header file for the MultiLogger class.
 @todo notcommented
 */
#ifndef MultiLogger_H
#define MultiLogger_H

    /**
    @class MultiLogger
    This class ???
     */
class MultiLogger : public Logger
{
public:
	MultiLogger();
	virtual ~MultiLogger();

	virtual void WriteBlock(const char *pBlock, size_t sBlockLen ) {}
	virtual void WriteEntry(class Logger::Entry& entry);
	virtual void Start();
	static Logger* Parse(TokenStream& ts, NameLoggerMap& targets, StringStringMap& vars);

	void AddOutput(Logger* logger);

protected:
	typedef ::std::vector<Logger*> LoggerVector;
	LoggerVector m_Loggers;
};

#endif
