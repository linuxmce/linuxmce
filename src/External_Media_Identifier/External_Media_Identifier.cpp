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


#include "IdentifierCDMDR.h"
#include "IdentifierDVDMDR.h"
#include <iostream>
#include "pluto_main/Define_MediaType.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
External_Media_Identifier::External_Media_Identifier(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: External_Media_Identifier_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
External_Media_Identifier::External_Media_Identifier(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: External_Media_Identifier_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
External_Media_Identifier::~External_Media_Identifier()
//<-dceag-dest-e->
{
	
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
//<-dceag-createinst-b->
External_Media_Identifier_Command *Create_External_Media_Identifier(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new External_Media_Identifier(pPrimaryDeviceCommand, pData, pEvent, pRouter);
}
//<-dceag-createinst-e->

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
  // cout << "Need to implement command #314 - Identify Media" << endl;
  // cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
  // cout << "Parm #10 - ID=" << sID << endl;
  // cout << "Parm #13 - Filename=" << sFilename << endl;
  // cout << "Parm #201 - PK_Device_Related=" << iPK_Device_Related << endl;

  string sText = "";
  int iPK_MediaType = 0;
  int iEK_Disc = 0;
  string sDisks = "";
  string sURL = "";
  string sBlock_Device = "";
  
  CMD_Get_Disk_Info CMD_Get_Disk_Info(m_dwPK_Device, 
				      (iPK_Device_Related == 0 ? pMessage->m_dwPK_Device_From : iPK_Device_Related), 
				      &sText, 
				      &iPK_MediaType, 
				      &iEK_Disc, 
				      &sDisks, 
				      &sURL, 
				      &sBlock_Device);
 
  SendCommand(CMD_Get_Disk_Info);

  if (iPK_MediaType == 0)
    {
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"CMD_Identify_Media() - Returned mediatype from CMD_Get_Disk_Info == 0, ignoring.");
      return;
    }
 
  IdentifierBase* pIdentifier = NULL;

  switch(iPK_MediaType)
    {
    case MEDIATYPE_pluto_CD_CONST:
      pIdentifier = new IdentifierCDMDR(sFilename, sID);
      break;
    case MEDIATYPE_pluto_DVD_CONST:
      pIdentifier = new IdentifierDVDMDR(sFilename,sID);
      break;
    default:
      LoggerWrapper::GetInstance()->Write(LV_WARNING,"CMD_Identify_Media() - I do not know how to identify disks of media type %d. Aborting identify.",iPK_MediaType);
      break;
    }
  
  if (pIdentifier == NULL)
    return;

  if (!pIdentifier->Init())
    return;

  if (!pIdentifier->Identify())
    return;
  
  string sIdentifiedData = pIdentifier->GetIdentifiedData();
  
  sCMD_Result = sIdentifiedData;
  const char *Data = pIdentifier->GetPictureData().data();
  if (pIdentifier->GetPictureData().size() == 0)
    Data = NULL;

  size_t iData_Size = pIdentifier->GetPictureData().size();
  DCE::CMD_Media_Identified CMD_Media_Identified( m_dwPK_Device, pMessage->m_dwPK_Device_From,
						  iPK_Device, sCMD_Result, sID,
						  (char*)Data, iData_Size,
						  pIdentifier->GetIdentityType(), iPK_MediaType,
						  sFilename, "",
						  &iEK_Disc);
  SendCommand(CMD_Media_Identified);

  // if (Data)
  //   {
  //     free(Data);
  //   }

  if (pIdentifier)
    {
      delete pIdentifier;
    }
}


