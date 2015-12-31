//
// C++ Interface: CallStatus
//
// Description:
//
//
// Author: Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CALLSTATUS_H
#define CALLSTATUS_H

#include <string>
#include <map>

#include "PlutoUtils/StringUtils.h"

using namespace std;

/**
The class which contains all the information about a telecom call.

	@author Eugen Constantinescu <eugen.c@plutohome.com>
*/
namespace DCE
{
	class CallStatus
	{
		public:
			
			/***/
			enum CallType { DirectCall=0, MultipleCall, Conference };

			/***/
			enum CallsStatus
			{
				csChannel = 0,
				csContext,
				csExten,
				csPriority,
				csStats,
				csApplication,
				csData,
				csCallerID,
				csAccoutcode,
				csAmaflags,
				csDuration,
				csBridged,
				NUM_FIELDS
			};
			
			/***/
			static CallType String2CallType(string);
			/***/
			static string CallType2String(CallType);
			
			/***/
			static string GetStringConferenceID(unsigned);
			
			/***/
			static bool IsConferenceCallID(string callid);

			/***/
			static unsigned int ExtractConferenceCallID(string callid);

			/***/
			static const char * conferencePrefix;
			
			/***/
			CallStatus();
			/***/
			~CallStatus();
			
			/***/
			void SetCallType(CallType ct) { type = ct; }
			/***/
			CallType GetCallType() const { return type; }
			
			/***/
			string GetID() const { return id; }
			
			/***/
			void SetConferenceID(unsigned cid) { conferenceID = cid; }
			/***/
			unsigned GetConferenceID() const { return conferenceID; }
			/***/
			bool IsConference() const { return type == Conference; }

			/***/
			void AddChannel(string channelid, string callerid)
			{
				size_t nPos = channelid.rfind(",");
				// if it's a linked temporary channel
				if( nPos != string::npos && nPos > 0 )
				{
					linkedChannels[ channelid.substr(0, nPos) ] = callerid;
				}
				else
				{
					channels[channelid] = callerid;
					if( !IsConference() && 2 < channels.size() )
					{
						SetCallType( CallStatus::MultipleCall );
					}
				}
			}
			
			/***/
			void RemoveChannel(string channelid)
			{
				channels.erase(channelid);
			}
			
			/***/
			bool HasChannel(string channelid) const
			{
				map<string, string>::const_iterator itFound = channels.find(channelid);
				
				return itFound != channels.end();
			}
			
			/***/
			bool Closed()
			{
				return channels.empty();
			}

			const map<string, string>& GetChannels() const
			{
				return channels;
			}
			
			/***/
			void AddLinkedChannel(string channelid, string callerid)
			{
				linkedChannels[channelid] = callerid;
			}
			
			/***/
			void RemoveLinkedChannel(string channelid)
			{
				linkedChannels.erase(channelid);
			}
			
			/***/
			bool HasLinkedChannel(string channelid) const
			{
				map<string, string>::const_iterator itFound = linkedChannels.find(channelid);
				
				return itFound != linkedChannels.end();
			}
			
			const map<string, string>& GetLinkedChannels() const
			{
				return linkedChannels;
			}
			
			/***/
			void SetDeviceOrbiter(int iDev)
			{
				iDeviceOrbiter = iDev;
			}
			/***/
			int GetDeviceOrbiter() const
			{
				return iDeviceOrbiter;
			}
			
			/***/
			void SetDeviceOwner(int iDev)
			{
				iDeviceOwner = iDev;
			}
			/***/
			int GetDeviceOwner() const
			{
				return iDeviceOwner;
			}
			
			/***/
			string GetDebugInfo() const;
			
		private:
			
			static unsigned autoId;
			CallType type;
			string id;
			unsigned conferenceID;
			map<string, string> channels;  //channelid <-> callerid
            
            // see dialplan, temporary channels from trusted context
            map<string, string> linkedChannels;
			
			int iDeviceOrbiter;
			int iDeviceOwner;
	};
}

#endif
