/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#include "CDDB_Identifier.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "pluto_main/Define_MediaType.h"

#ifndef WIN32
#include "sys/wait.h"
#endif

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
CDDB_Identifier::CDDB_Identifier(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: CDDB_Identifier_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}
//<-dceag-getconfig-b->
bool CDDB_Identifier::GetConfig()
{
	if( !CDDB_Identifier_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->
	return true;
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
CDDB_Identifier::CDDB_Identifier(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: CDDB_Identifier_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
CDDB_Identifier::~CDDB_Identifier()
//<-dceag-dest-e->
{
	
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool CDDB_Identifier::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
CDDB_Identifier_Command *Create_CDDB_Identifier(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new CDDB_Identifier(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void CDDB_Identifier::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void CDDB_Identifier::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
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

void CDDB_Identifier::CMD_Identify_Media(int iPK_Device,string sID,string sFilename,string &sCMD_Result,Message *pMessage)
//<-dceag-c314-e->
{
	cout << "Command #314 - Identify Media" << endl;
	cout << "Parm #10 - ID=" << sID << endl;
	cout << "Parm #13 - Filename=" << sFilename << endl;

	// sResult: Tab separated: CDID Artist Title Genre Track1_title [Track2_title ...]
	int child_pipe[2],pid;
#ifndef WIN32
	pipe(child_pipe);

	pid = fork();
	const int MAX = 4096;
#endif
	if (pid == -1)
	{
		close(child_pipe[1]);
		close(child_pipe[2]);
		cout << "fork failed" << endl;
		return;
	}

	if (pid == 0)
	{
		close(child_pipe[0]);
		dup2(child_pipe[1], 1);

		cerr << "/usr/pluto/bin/cddb_id.sh" << sFilename.c_str() << endl;
#ifndef WIN32
		execl("/usr/pluto/bin/cddb_id.sh", "cddb_id.sh", sFilename.c_str(), (char *) NULL);
#endif		
		cerr << "exec failed" << endl;
		exit(-1);
	}
	else
	{
		int retcode, bytes;
		string sResult;
		char buffer[MAX + 1];

		close(child_pipe[1]);

		while ((bytes = read(child_pipe[0], &buffer, MAX)) > 0)
		{
			buffer[bytes] = 0;
			sResult += buffer;
		}
		
		cout << "Result: " << sResult << endl;
#ifndef WIN32
		waitpid(pid, &retcode, 0);
#endif
		retcode = WEXITSTATUS(retcode);
		if (retcode == 0)
		{
			sCMD_Result = "OK";
			int iEK_Disc;
			DCE::CMD_Media_Identified CMD_Media_Identified(m_dwPK_Device,pMessage->m_dwPK_Device_From,iPK_Device,sResult,sID,NULL,0,"CDDB-TAB",MEDIATYPE_pluto_CD_CONST,sFilename,"",&iEK_Disc);
			SendCommand(CMD_Media_Identified);
		}

		close(child_pipe[0]);
	}

}

