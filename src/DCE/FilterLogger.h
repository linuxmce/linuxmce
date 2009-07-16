/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU Lesser General Public License for more details.

*/

/**
 * @file FilterLogger.h
 Header file for the FilterLogger class.
*/
#ifndef FilterLogger_H
#define FilterLogger_H

    /**
    @class FilterLogger
    This derived class ???
     */
class FilterLogger : public MultiLogger
{
public:
	FilterLogger();
	virtual ~FilterLogger();

	virtual void WriteEntry(class Logger::Entry& entry);

	static FilterLogger* Parse(TokenStream& ts, NameLoggerMap& targets, StringStringMap& vars);

protected:
	bool LevelMatch(int Level);
	bool ParseLevels(::std::string str);

	::std::set<int> m_Levels;
	int m_GreaterThan;
	int m_LessThan;
	bool m_Invert;
};

#endif
