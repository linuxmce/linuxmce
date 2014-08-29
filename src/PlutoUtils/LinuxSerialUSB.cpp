/* 
	LinuxSerialUSB
	
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU General Public License for more details.
*/

#include "PlutoUtils/LinuxSerialUSB.h"	 
#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/StringUtils.h"

string TranslateSerialUSB(string sInput,string sIPAddress)
{
#ifndef WIN32
	
	if( sInput.size()<6 || ( sInput.substr(0,3)!="pci" && sInput.substr(0,8) != "platform" ) )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"TranslateSerialUSB %s isn't serial usb",sInput.c_str());
		return sInput;
	}

	string sCmd, sOutput, sStdErr;
	if ( sIPAddress.empty()==false )
	{
		// Remote address, use ssh.
		const char *args[] = {"/usr/pluto/bin/TranslateRemoteSerialPort.sh",sIPAddress.c_str(),sInput.c_str(),NULL};
		ProcessUtils::GetCommandOutput(args[0],args,sOutput,sStdErr);
	}
	else
	{
		// This is local, just execute it.
		const char *args[] = {"/usr/pluto/bin/TranslateSerialPort.sh",sInput.c_str(),NULL};
		ProcessUtils::GetCommandOutput(args[0],args,sOutput,sStdErr);
	}

	StringUtils::TrimSpaces(sOutput);
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TranslateSerialUSB %s result %s", sInput.c_str(), sOutput.c_str());
#endif
#ifdef WIN32
	string sOutput = "";  // fix so win32 will compile.
#endif
	return sOutput;
}
