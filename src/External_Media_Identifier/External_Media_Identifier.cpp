/*
     Copyright (C) 2013 LinuxMCE

     www.linuxmce.org


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#include "External_Media_Identifier.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "IdentifierGenericCD.h"
#include "IdentifierGenericDVD.h"
#include "IdentifierCDMDR.h"
#include "IdentifierDVDMDR.h"
#include <iostream>
#include "pluto_main/Define_MediaType.h"
#include "Disk_Drive_Functions/Disk_Drive_Functions.h"
#include <fcntl.h>
#ifndef WIN32
#include <sys/ioctl.h>
#include <sys/mount.h>
#endif

#include <limits.h>

#include <sstream>

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
External_Media_Identifier::External_Media_Identifier(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: External_Media_Identifier_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
  m_pIdentifyDisc = new IdentifyDisc();
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
External_Media_Identifier::~External_Media_Identifier()
//<-dceag-dest-e->
{
  if (m_pIdentifyDisc)
    {
      delete m_pIdentifyDisc;
      m_pIdentifyDisc=NULL;
    }
}

//<-dceag-getconfig-b->
bool External_Media_Identifier::GetConfig()
{
	if( !External_Media_Identifier_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool External_Media_Identifier::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->!

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void External_Media_Identifier::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	sCMD_Result = "UNHANDLED CHILD";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void External_Media_Identifier::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c314-b->

	/** @brief COMMAND: #314 - Identify Media */
	/** New media has been inserted and needs to be identified.  Any third party Media Identifier device can provide a description of the media. */
		/** @param #2 PK_Device */
			/** The disk drive that has the media */
		/** @param #10 ID */
			/** The ID of the disk */
		/** @param #13 Filename */
			/** The media that needs to be identified, such as /dev/cdrom under Linux, or E: under Windows */
		/** @param #201 PK_Device_Related */
			/** If specified, the device to send the resulting 'media identified' command to.  Otherwise it's the from device */

void External_Media_Identifier::CMD_Identify_Media(int iPK_Device,string sID,string sFilename,int iPK_Device_Related,string &sCMD_Result,Message *pMessage)
//<-dceag-c314-e->
{    
  IdentifierBase* pIdentifier = NULL;
  int iPK_MediaType=0;
  string sIdentifiedData;
  const char* Data=NULL;
  size_t iData_Size=0;
  int iEK_Disc=0;

  IdentifyDisc::DiscType discType = m_pIdentifyDisc->Identify(sFilename);

  switch (discType)
    {
    case IdentifyDisc::DiscType::CD:
      pIdentifier = new IdentifierCDMDR(sFilename, sID);
      iPK_MediaType=MEDIATYPE_pluto_CD_CONST;
      break;
    case IdentifyDisc::DiscType::DVD:
      pIdentifier = new IdentifierDVDMDR(sFilename, sID);
      iPK_MediaType=MEDIATYPE_pluto_DVD_CONST;
      break;
    case IdentifyDisc::DiscType::BluRay:
      pIdentifier = NULL;    // One does not exist yet, but we still want to set the disc type!
      iPK_MediaType=MEDIATYPE_pluto_BD_CONST;
      break;
    case IdentifyDisc::DiscType::UNKNOWN:
    default:
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not identify disc. Aborting");
      sCMD_Result="ERROR";
      return;
    }

  if (pIdentifier!=NULL)
    {
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"Calling pIdentifier->Init()");
      if (!pIdentifier->Init())
	return;
      
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"Calling pIdentifier->Identify()");
      if (!pIdentifier->Identify())
	{
	  // If we couldn't identify, but we know the disc type instantiate the appropriate
	  // generic disc identifier.
	  switch (discType)
	    {
	    case IdentifyDisc::DiscType::CD:
	      pIdentifier = new IdentifierGenericCD(sFilename, sID);
	      break;
	    case IdentifyDisc::DiscType::DVD:
	    case IdentifyDisc::DiscType::BluRay:
	      pIdentifier = new IdentifierGenericDVD(sFilename, sID);
	      break;
	    }

	  if (!pIdentifier->Init())
	    {
	      sCMD_Result="ERROR";
	      return;
	    }

	  if (!pIdentifier->Identify())
	    {
	      sCMD_Result="ERROR";
	      return;
	    }
	  // Otherwise, we now have an appropriate generic disc.
	}
      
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"Calling pIdentifier->GetIdentifiedData()");
      sIdentifiedData = pIdentifier->GetIdentifiedData(); 
      Data = pIdentifier->GetPictureData().data();
      if (pIdentifier->GetPictureData().size() == 0)
	Data = NULL;
      iData_Size = pIdentifier->GetPictureData().size();
    }

  sCMD_Result = "OK";

  LoggerWrapper::GetInstance()->Write(LV_STATUS,"Calling CMD_Media_Identified();");

  DCE::CMD_Media_Identified CMD_Media_Identified( m_dwPK_Device, pMessage->m_dwPK_Device_From,
						  iPK_Device, sIdentifiedData, sID,

						  (char*)Data, iData_Size,
						  pIdentifier->GetIdentityType(), iPK_MediaType,
						  sFilename, "",
						  &iEK_Disc);
  SendCommand(CMD_Media_Identified);

  if (pIdentifier)
    {
      delete pIdentifier;
    }
}


