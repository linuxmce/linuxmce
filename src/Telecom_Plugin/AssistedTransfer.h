//
// C++ Interface: AssistedTransfer
//
// Description:
//
//
// Author: Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ASSISTEDTRANSFER_H
#define ASSISTEDTRANSFER_H

#include "TelecomTask.h"

namespace DCE
{
	/**
	The telecom task which performs an assisted transfer.
	
		@author Eugen Constantinescu <eugen.c@plutohome.com>
	*/
	class AssistedTransfer : public TelecomTask
	{
		public:
			
			enum StepType
			{
				Step_Zero=0,
				Init_MyCall2Conference, Init_DestCall2Conference, Init_MyChannel2DestCall, Init_MyChannel2DestConf,
				Transfer_MyChannel,
				MergeCalls, JoinDestination,
				Cancel_MyChannel
			};
			
			/***/
			AssistedTransfer(	const string & ext,
								const string & callid,
								const string & channelid,
								const string & mycallid );
			/***/
			virtual ~AssistedTransfer();
			
			/***/
			virtual bool ProcessJob(const string &);
			/***/
			virtual bool ProcessEvent(class Message *);
			/***/
			virtual TelecomTask::TaskType RTTI() const { return TelecomTask::Assisted_Transfer; }
			
			string GetDebug() const;
			
		private:
			
			bool PrivateProcessJob(const string &);
			
			string sExtDest;
			string sCallID_Dest;
			string sMyChannelID;
			string sMyChannel2_ID;
			string sMyCallID;
			string sNextJob;
			string sChannel1_Dest;
			string sChannel2_Dest;
			StepType step;
	};
};
#endif
