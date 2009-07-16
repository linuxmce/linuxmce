/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU Lesser General Public License for more details.

*/

/** @file BufferedLogger.h
 *@brief The BufferedLogger class.
 */

#ifndef BufferedLogger_H
#define BufferedLogger_H

namespace DCE
{

    /**
    @class BufferedLogger
     Defines log methods.
     */
	class BufferedLogger : public MultiLogger, public AlarmEvent
	{
	public:
		BufferedLogger();
		virtual ~BufferedLogger();

		virtual void WriteEntry(class Logger::Entry& entry);

		virtual void Flush();
		virtual void Start();

		static BufferedLogger* Parse(TokenStream& ts, NameLoggerMap& targets, StringStringMap& vars);

	protected:
		virtual void AlarmCallback(int id, void* param);
		static int ParseTimeSpec(::std::string str);

		typedef ::std::queue<Logger::Entry> LogEntryQueue;

		size_t m_MaxEntries;
		LogEntryQueue m_Queue;
		int m_FlushDelay;

		AlarmManager* m_AlarmManager;
	};
}

#endif
