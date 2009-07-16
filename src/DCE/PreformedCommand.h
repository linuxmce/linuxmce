/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU Lesser General Public License for more details.

*/

/** @file PreformedCommand.h
 Header file for the PreformedCommandResponse class.
 @todo notcommented
 */
#ifndef PreformedCommand_h
#define PreformedCommand_h
#include "Message.h"

namespace DCE
{

    /**
    @class PreformedCommandResponse
    This class ???
     */
	class PreformedCommandResponse {
	public:
		virtual ~PreformedCommandResponse() {};
		virtual void ParseResponse(Message *pMessage)=0;
	};

    /**
    @class PreformedCommand
    This class ???
     */
	class PreformedCommand	{
	public:
		PreformedCommandResponse *m_pcResponse; Message *m_pMessage;
		PreformedCommand() : m_pcResponse(NULL), m_pMessage(NULL) {}
		~PreformedCommand()
		{
			if( m_pcResponse )
			{
				delete m_pcResponse;
				m_pcResponse = NULL;
			}
		}

		void ParseResponse(Message *pMessage)
		{
			if( m_pcResponse )
				m_pcResponse->ParseResponse(pMessage);
		}
	};
}

#endif
