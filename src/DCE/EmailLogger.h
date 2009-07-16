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
 * @file EmailLogger.h
 Header file for the EmailLogger class.
*/
#ifndef EmailLogger_H
#define EmailLogger_H

namespace DCE
{

    /** @class EMailLogger
     The EmailLogger class.
     */
	class EMailLogger : public Logger
	{
	public:

		EMailLogger(string to, string from, string subject);
		virtual ~EMailLogger();

		static EMailLogger* Parse(TokenStream& ts, StringStringMap& vars);

		virtual void WriteBlock(const char *pBlock, size_t sBlockLen ) {}
		virtual void WriteEntry(class Logger::Entry& entry);

	protected:
		string m_To;
		string m_From;
		string m_Subject;
		string m_Server;
	};
}

#endif
