//
// C++ Implementation: ExtensionStatus
//
// Description: 
//
//
// Author: Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ExtensionStatus.h"
#include "DCE/Logger.h"

using namespace DCE;

string ExtensionStatus::Availability2String( ExtensionStatus::Availability ava )
{
	string sOutAvailability;
	
	switch( ava )
	{
		case ExtensionStatus::Unregistered :
			sOutAvailability = "Unregistered";
			break;
			
		case ExtensionStatus::Registered :
			sOutAvailability = "Registered";
			break;
			
		case ExtensionStatus::UnknownAvailability :
		default :
			sOutAvailability = "Unknown";
			break;
	}
	
	return sOutAvailability;
}

string ExtensionStatus::Activity2String( ExtensionStatus::Activity act )
{
	string sOutActivity;
	
	switch( act )
	{
		case ExtensionStatus::Ready :
			sOutActivity = "Ready";
			break;
		
		case ExtensionStatus::OnThePhone :
			sOutActivity = "OnThePhone";
			break;
		
		case ExtensionStatus::Ringing :
			sOutActivity = "Ringing";
			break;
		
		case ExtensionStatus::Dialing :
			sOutActivity = "Dialing";
			break;
			
		case ExtensionStatus::UnknownActivity :
		default:
			sOutActivity = "Unknown";
			break;
	}

	return sOutActivity;
}

ExtensionStatus::Availability ExtensionStatus::String2Availability(const string & sAva)
{
	if( sAva == "Registered" )
	{
		return ExtensionStatus::Registered;
	}
	else if( "Unregistered" )
	{
		return ExtensionStatus::Unregistered;
	}
	
	return ExtensionStatus::UnknownAvailability;
}

ExtensionStatus::Activity ExtensionStatus::String2Activity(const string & sAct)
{
	if( sAct == "Ready" )
	{
		return ExtensionStatus::Ready;
	}
	else if( sAct == "OnThePhone" )
	{
		return ExtensionStatus::OnThePhone;
	}
	else if( sAct == "Ringing" )
	{
		return ExtensionStatus::Ringing;
	}
	else if( sAct == "Dialing" )
	{
		return ExtensionStatus::Dialing;
	}
	
	return ExtensionStatus::UnknownActivity;
}

ExtensionStatus::ExtensionType ExtensionStatus::String2Type(const string & sType)
{
	if( sType == "SIP" )
	{
		return ExtensionStatus::SIP;
	}
	else if("IAX")
	{
		return ExtensionStatus::IAX;
	}
	else if("SCCP")
	{
		return ExtensionStatus::SCCP;
	}
	
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"ExtensionStatus::String2Type unknown: %s", sType.c_str());
	return ExtensionStatus::OTHER;
}

string ExtensionStatus::Type2String( ExtensionStatus::ExtensionType type )
{
	string sOutType;
	switch( type )
	{
		case ExtensionStatus::SCCP :
			sOutType = "SCCP";
			break;
			
		case ExtensionStatus::IAX :
			sOutType = "IAX";
			break;
			
		case ExtensionStatus::SIP :
			sOutType = "SIP";
			break;

		default:
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Unknown extension type! Using 'Local' as type");
			sOutType = "Local";
			break;
	}
	
	return sOutType;
}

ExtensionStatus::ExtensionStatus()
	: availability(ExtensionStatus::Unregistered),
	  activity(ExtensionStatus::Ready),
	  extType(ExtensionStatus::SIP)
{
}

string ExtensionStatus::GetDebugInfo() const
{
	string sDebug = "Extension status ";
	
	sDebug += GetID() + " : ";
	sDebug += Type2String( GetExtensionType() ) + " : ";
	sDebug += Availability2String( GetAvailability() ) + " : ";
	sDebug += Activity2String( GetActivity() );
	
	return sDebug;
}
