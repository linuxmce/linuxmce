//
// C++ Implementation: AssistedTransfer
//
// Description:
//
//
// Author: Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "AssistedTransfer.h"
#include "Telecom_Plugin.h"
#include "DCE/Logger.h"

using namespace DCE;

AssistedTransfer::AssistedTransfer(	const string & ext,
									const string & callid,
									const string & channelid,
									const string & mycallid )
	: TelecomTask(),
	  sExtDest(ext),
	  sCallID_Dest(callid),
	  sMyChannelID(channelid),
	  sMyCallID(mycallid),
	  step(AssistedTransfer::Step_Zero)
{
}

AssistedTransfer::~AssistedTransfer()
{
}

bool AssistedTransfer::ProcessJob(const string & job)
{
	bool bRun = PrivateProcessJob(job);
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "After PrivateProcessJob: %s", GetDebug().c_str());
	if( !bRun )
	{
		PrivateProcessJob("cancel");
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "After Cancel: %s", GetDebug().c_str());
	}
	
	return bRun;
}

bool AssistedTransfer::ProcessEvent(class Message * pMessage)
{
	if( pMessage->m_dwID == EVENT_PBX_Link_CONST )
	{
		string sSource_Channel = pMessage->m_mapParameters[EVENTPARAMETER_Source_Channel_CONST];
		string sDestination_Channel = pMessage->m_mapParameters[EVENTPARAMETER_Destination_Channel_CONST];
		string sSource_Caller_ID = pMessage->m_mapParameters[EVENTPARAMETER_Source_Caller_ID_CONST];
		string sDestination_Caller_ID = pMessage->m_mapParameters[EVENTPARAMETER_Destination_Caller_ID_CONST];
		
		LoggerWrapper::GetInstance()->Write
			(LV_STATUS, "AssistedTransfer::ProcessEvent Link \nSC=%s\nDC=%s\nSI=%s\nDI=%s\n%s",
			sSource_Channel.c_str(),
			sDestination_Channel.c_str(),
			sSource_Caller_ID.c_str(),
			sDestination_Caller_ID.c_str(),
			GetDebug().c_str());
		
		
		switch( step )
		{
			case AssistedTransfer::Init_MyCall2Conference :
				if( sSource_Channel == sMyChannelID || sSource_Channel == sMyChannel2_ID )
				{
					CallStatus * pCallStatus1 = telecom->FindCallStatusForChannel( sMyChannelID );
					CallStatus * pCallStatus2 = telecom->FindCallStatusForChannel( sMyChannel2_ID );
					if( pCallStatus1 != NULL && pCallStatus1 == pCallStatus2 )
					{
						sMyCallID = pCallStatus1->GetID();
						ProcessJob("initialize");
					}
					else if(NULL == pCallStatus1 || NULL == pCallStatus2)
					{
						LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "One of the channels is not into a call anymore!");
						SetJobState(TelecomTask::Done);
						SetState(TelecomTask::Failed);
					}
				}
				break;
				
			case AssistedTransfer::Init_DestCall2Conference :
				if( sSource_Channel == sChannel1_Dest || sSource_Channel == sChannel2_Dest )
				{
					CallStatus * pCallStatus1 = telecom->FindCallStatusForChannel( sChannel1_Dest );
					CallStatus * pCallStatus2 = telecom->FindCallStatusForChannel( sChannel2_Dest );
					if( pCallStatus1 != NULL && pCallStatus1 == pCallStatus2 )
					{
						sCallID_Dest = pCallStatus1->GetID();
						ProcessJob("initialize");
					}
					else if(NULL == pCallStatus1 || NULL == pCallStatus2)
					{
						LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "One of the channels is not into a call anymore!");
						SetJobState(TelecomTask::Done);
						SetState(TelecomTask::Failed);
					}
				}
				break;
				
			case AssistedTransfer::Init_MyChannel2DestCall :
				if( sSource_Channel == sMyChannelID )
				{
					// todo: do more checking, be paranoic
					CallStatus * pCallStatus = telecom->FindCallStatusForChannel( sMyChannelID );
					if( pCallStatus )
					{
						sCallID_Dest = pCallStatus->GetID();
						SetJobState( TelecomTask::Done );
						if( !sNextJob.empty() )
						{
							ProcessJob(sNextJob);
						}
					}
					else
					{
						LoggerWrapper::GetInstance()->Write
							(LV_CRITICAL, "Error: Init_MyChannel2DestCall: there isn't a call for my channel %s",
							 sMyChannelID.c_str());
						SetJobState(TelecomTask::Done);
						SetState(TelecomTask::Failed);
					}
				}
				break;
			
			case AssistedTransfer::Init_MyChannel2DestConf :
				if( sSource_Channel == sMyChannelID )
				{
					// todo: do more checking, be paranoic
					CallStatus * pCallStatus = telecom->FindCallStatusForChannel( sMyChannelID );
					if( pCallStatus && sCallID_Dest == pCallStatus->GetID() )
					{
						SetJobState( TelecomTask::Done );
						if( !sNextJob.empty() )
						{
							ProcessJob(sNextJob);
						}
					}
					else
					{
						if( !pCallStatus )
						{
							LoggerWrapper::GetInstance()->Write
								(LV_CRITICAL, "Error: Init_MyChannel2DestConf : there isn't a call for my channel %s",
								 sMyChannelID.c_str());
						}
						else
						{
							LoggerWrapper::GetInstance()->Write
								(LV_CRITICAL, "Error: Init_MyChannel2DestConf : my call %s != dest call %s",
								pCallStatus->GetID().c_str(), sCallID_Dest.c_str());
						}
						SetJobState(TelecomTask::Done);
						SetState(TelecomTask::Failed);
					}
				}
				break;
				
			case AssistedTransfer::Transfer_MyChannel :
				SetJobState(TelecomTask::Done);
				SetState(TelecomTask::Completed);
				break;
				
			case AssistedTransfer::JoinDestination :
			case AssistedTransfer::MergeCalls :
				if( sSource_Channel == sMyChannelID )
				{
					CallStatus * pCallStatus = telecom->FindCallStatusForChannel( sMyChannelID );
					if( pCallStatus )
					{
						SetJobState(TelecomTask::Done);
						SetState(TelecomTask::Completed);
					}
					else
					{
						LoggerWrapper::GetInstance()->Write
							(LV_CRITICAL, "Error: MergeCalls: there isn't a call for my channel %s", sMyChannelID.c_str());
						SetJobState(TelecomTask::Done);
						SetState(TelecomTask::Failed);
					}
				}
				break;
			
			default:
				// todo, debug info ?
				break;
		}
		
		return true;
	}
	else if( pMessage->m_dwID == EVENT_PBX_Hangup_CONST )
	{
		string sChannel_ID = pMessage->m_mapParameters[EVENTPARAMETER_Channel_ID_CONST];
		string sReason = pMessage->m_mapParameters[EVENTPARAMETER_Reason_CONST];
		
		LoggerWrapper::GetInstance()->Write
			(LV_STATUS, "AssistedTransfer::ProcessEvent Hang \nCh=%s\nEr=%s\n%s",
			sChannel_ID.c_str(),
			sReason.c_str(),
			GetDebug().c_str());
		
		switch( step )
		{
			case AssistedTransfer::Init_MyChannel2DestCall :
			case AssistedTransfer::Init_MyCall2Conference :
				if( sChannel_ID == sMyChannelID )
				{
					SetJobState(TelecomTask::Done);
					ProcessJob("cancel");
				}
				break;
				
			default:
				break;
		}
		
		return true;
	}
	
	return false;
}

bool AssistedTransfer::PrivateProcessJob(const string & job)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "AssistedTransfer::PrivateProcessJob : job %s\n%s", job.c_str(), GetDebug().c_str());
	
	if( job == "cancel" )
	{
		SetState(TelecomTask::Failed);
		SetJobState(TelecomTask::Done);
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "AssistedTransfer : cancel job debug: %s", GetDebug().c_str());
	}
	else
	{
		if( TelecomTask::Completed == GetState() || TelecomTask::Failed == GetState() )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "AssistedTransfer : job completed/failed !");
			return false;
		}
	
		if( TelecomTask::Running == GetJobState() && job != sJobID && !sJobID.empty() )
		{
			// try later
			sNextJob = job;
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "AssistedTransfer : next job = %s", sNextJob.c_str());
			return true;
		}
	
		SetState(TelecomTask::Processing);
		SetJobState(TelecomTask::Running);
	}
	
	CallStatus * pDestCallStatus = NULL;
	CallStatus * pMyCallStatus = NULL;
	if( !sCallID_Dest.empty() )
	{
		pDestCallStatus = telecom->FindCallStatusForID( sCallID_Dest );
		if( pDestCallStatus == NULL )
		{
			LoggerWrapper::GetInstance()->Write
				(LV_CRITICAL, "Error: PrivateProcessJob: No destination call with ID %s",
				sCallID_Dest.c_str());
			return false;
		}
	}
	if( !sMyCallID.empty() )
	{
		pMyCallStatus = telecom->FindCallStatusForID( sMyCallID );
		if( pMyCallStatus == NULL )
		{
			LoggerWrapper::GetInstance()->Write
				(LV_CRITICAL, "Error: PrivateProcessJob: No my call with ID %s",
				sMyCallID.c_str());
			return false;
		}
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Error: PrivateProcessJob: My call ID is empty!");
		return false;
	}
	
	sJobID = job;
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "AssistedTransfer : trying to do job %s", sJobID.c_str());
	
	//job "initialize" (internal)
	//1) {X,Y,A} & B => {X,Y} & {A,B}
	//or 
	//2) {X,Y,A} & {B,C} => {X,Y} & {A,B,C} 
	if( job == "initialize" )
	{
		if( step == AssistedTransfer::Step_Zero )
		{
			// be sure your current call is conference
			// you don't want the other channel to be hang up
			if( !pMyCallStatus->IsConference() )
			{
				const map<string, string> & channels = pMyCallStatus->GetChannels();
				if( 2 != channels.size() )
				{
					LoggerWrapper::GetInstance()->Write
						(LV_CRITICAL, "Error: initialize: my call isn't conference but it has %u channels!",
						 channels.size());
					return false;
				}
				
				// find the second channel from my call
				for(map<string, string>::const_iterator it=channels.begin(); it!=channels.end(); ++it)
				{
					if( sMyChannelID != (*it).first )
					{
						sMyChannel2_ID = (*it).first;
						break;
					}
				}
				
				// the new ID will be saved into sMyCallID
				string sNewConfID = telecom->DirectCall2Conference(pMyCallStatus);
				if( sNewConfID.empty() )
				{
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Error: initialize: coudn't change my call in conference!");
					return false;
				}
				
				step = AssistedTransfer::Init_MyCall2Conference;
				return true;
			}
			
			step = AssistedTransfer::Init_MyCall2Conference;
		}
		
		if( step == AssistedTransfer::Init_MyCall2Conference )
		{
			if( !sExtDest.empty() )
			{
				// the new ID will be saved into sCallID_Dest
				telecom->CMD_PL_Transfer(0, 0, sExtDest, sMyChannelID, "");
				step = AssistedTransfer::Init_MyChannel2DestCall;
			}
			else if( pDestCallStatus != NULL )
			{
				if( pDestCallStatus->IsConference() )
				{
					// transfer my channel to the {B,C} conference
					telecom->CMD_PL_Transfer
						(0, 0, CallStatus::GetStringConferenceID(pDestCallStatus->GetConferenceID()), sMyChannelID, "");
					step = AssistedTransfer::Init_MyChannel2DestConf;
				}
				else
				{
					const map<string, string> & channels = pDestCallStatus->GetChannels();
					if( 2 == channels.size() )
					{
						sChannel1_Dest = (*channels.begin()).first;
						sChannel2_Dest = (*(++channels.begin())).first;
						
						// transform the direct call {B,C} into the conference {B,C}
						string sNewConfID = telecom->DirectCall2Conference(pDestCallStatus);
						if( !sNewConfID.empty() )
						{
							step = AssistedTransfer::Init_DestCall2Conference;
						}
						else
						{
							LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Error: initialize: coudn't change dest call in conference!");
							return false;
						}
					}
					else
					{
						LoggerWrapper::GetInstance()->Write
							(LV_CRITICAL, "Error: initialize: dest call isn't conference but it has %u channels!",
							channels.size());
					}
				}
			}
			return true;
		}
		
		if( step == AssistedTransfer::Init_DestCall2Conference )
		{
			if(NULL != pDestCallStatus)
			{
				// transfer my channel to the new conference
				telecom->CMD_PL_Transfer(0, 0, 
					CallStatus::GetStringConferenceID(pDestCallStatus->GetConferenceID()), 
					sMyChannelID, "");

				step = AssistedTransfer::Init_MyChannel2DestConf;
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Error: Init_DestCall2Conference: no destination call!");
				return false;
			}
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Error: Init_DestCall2Conference: wrong step %d !", step);
			return false;
		}
	}
	//job "transfer"
	//1) {X,Y,A} & B => {X,Y} & {A,B} -> drop A, {X,Y,B}
	//or
	//2) {X,Y,A} & {B,C} => {X,Y} & {A,B,C} ->  drop A, {X,Y,B,C}
	else if( job == "transfer" )
	{
		if( pDestCallStatus == NULL )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Error: transfer: no dest call!");
			return false;
		}
		
		if( pDestCallStatus->IsConference() )
		{
			telecom->CMD_Merge_Calls(sMyCallID ,sCallID_Dest);
			step = AssistedTransfer::MergeCalls;
			telecom->CMD_PL_Cancel(0, sMyChannelID);
			step = AssistedTransfer::Cancel_MyChannel;
		}
		else
		{
			string sChannel = telecom->FindChannelForExt(pDestCallStatus, sExtDest);
			if( !sChannel.empty() )
			{
				telecom->CMD_PL_Transfer
					(0, 0, CallStatus::GetStringConferenceID( pMyCallStatus->GetConferenceID() ), sChannel, "");
				step = AssistedTransfer::JoinDestination;
			}
			else
			{
				LoggerWrapper::GetInstance()->Write
					(LV_CRITICAL, "Error: No channel for ext %s in dest call %s",
					sExtDest.c_str(), pDestCallStatus->GetID().c_str());
				return false;
			}
		}
	}
	//job "conference"
	//1) {X,Y,A} & B => {X,Y} & {A,B} -> {X,Y,A,B}
	else if( job == "conference" )
	{
		telecom->CMD_Merge_Calls(sMyCallID ,sCallID_Dest);
		step = AssistedTransfer::MergeCalls;
	}
	//job "merge calls"
	//2) {X,Y,A} & {B,C} => {X,Y} & {A,B,C} -> {X,Y,A,B,C}
	else if( job == "merge calls" )
	{
		telecom->CMD_Merge_Calls(sMyCallID ,sCallID_Dest);
		step = AssistedTransfer::MergeCalls;
	}
	//job "cancel"
	//1) {X,Y,A} & B => {X,Y} & {A,B} -> {X,Y,A} & B
	//or 
	//2) {X,Y,A} & {B,C} => {X,Y,A} & {B,C} 
	else if( job == "cancel" )
	{
		CallStatus * pMyChannel_CallStatus = telecom->FindCallStatusForChannel( sMyChannelID );
		CallStatus * pMyOld_CallStatus = telecom->FindCallStatusForID(sMyCallID);

		// the channel is still available, let's try to do the previous configuration
		if( pMyChannel_CallStatus != NULL && NULL != pMyOld_CallStatus)
		{
			// check if MY channel was transfered to another call
			if( pMyChannel_CallStatus != pMyCallStatus )
			{
				telecom->CMD_PL_Transfer(0, 0, 
					CallStatus::GetStringConferenceID(pMyOld_CallStatus->GetConferenceID()), 
					sMyChannelID, "");
			}
			else
			{
				LoggerWrapper::GetInstance()->Write
					(LV_CRITICAL, "Error: cancel: my current call is the same as the old one: %s",
					pMyOld_CallStatus->GetID().c_str());
				return false;
			}
		}
		// the old call is available but my channel was dropped
		// let's make a new channel to the old call
		else if( NULL != pMyOld_CallStatus )
		{
			string sMyExt;
			if( telecom->ParseChannel(sMyChannelID, &sMyExt) && !sMyExt.empty() )
			{
				telecom->CMD_PL_Join_Call(0, sMyExt, sMyCallID, 0);
			}
			else
			{
				LoggerWrapper::GetInstance()->Write
					(LV_CRITICAL, "Error: cancel: couldn't get the ext from channel %s",
					sMyChannelID.c_str());
			}
		}
		else
		{
			LoggerWrapper::GetInstance()->Write
				(LV_CRITICAL, "Error: cancel: neither my current call or my old call are available now! %s",
				GetDebug().c_str());
			return false;
		}
	}
	else
	{
		LoggerWrapper::GetInstance()->Write
			(LV_WARNING, "AssistedTransfer : No such job %s %s",
			job.c_str(),
			GetDebug().c_str());
		return false;
	}
	
	return true;
}

string AssistedTransfer::GetDebug() const
{
	string sDebug = "\nAssistedTransfer::GetDebug:\n";
	
	sDebug += "Destination Ext : " + sExtDest + "\n";
	sDebug += "Destination Call ID : " + sCallID_Dest + "\n";
	sDebug += "My Channel ID : " + sMyChannelID + "\n";
	sDebug += "My Channel2 ID : " + sMyChannel2_ID + "\n";
	sDebug += "My call ID : " + sMyCallID + "\n";
	sDebug += "Next Job : " + sNextJob + "\n";
	sDebug += "Dest Channel1 ID : " + sChannel1_Dest + "\n";
	sDebug += "Dest Channel2 ID : " + sChannel2_Dest + "\n";
	sDebug += "Step ID : " + StringUtils::itos(step) + "\n";
	
	sDebug += "UID : " + sID + "\n";
	sDebug += "TaskState : " + StringUtils::itos(state) + "\n";
	sDebug += "JobID : " + sJobID + "\n";
	sDebug += "JobState : " + StringUtils::itos(jobState) + "\n";
	
	return sDebug;
}
