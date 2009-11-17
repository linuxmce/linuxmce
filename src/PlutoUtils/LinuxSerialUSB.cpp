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


string TranslateSerialUSB(string sInput,string sIPAddress)
{
#ifndef WIN32
	
	if( sInput.size()<6 || sInput.substr(0,3)!="pci" )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"TranslateSerialUSB %s isn't serial usb",sInput.c_str());
		return sInput;
	}

	size_t pos = 0;
	string sPciId = StringUtils::Tokenize(sInput, "+", pos);
	string sUsbId = StringUtils::Tokenize(sInput, "+", pos);
	sUsbId = StringUtils::Replace(sUsbId,".","\\.");

	char tmpFile[40] = "/tmp/devusbXXXXXX";
	mktemp(tmpFile);
	string sCmd;

	if( sIPAddress.empty()==false )
		sCmd = "ssh " + sIPAddress + " ";

	sCmd += "find /sys/devices -name '*tty*' | egrep '/tty[:/]' | grep usb | grep '" + sPciId + ".*-" + sUsbId + ".*' | sed -r 's,tty[:/],,g' >" + tmpFile; 

	system(sCmd.c_str());

	vector<string> vectStr;
	FileUtils::ReadFileIntoVector(tmpFile,vectStr);
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"TranslateSerialUSB Cmd %s size %d",sCmd.c_str(),(int) vectStr.size());
#endif
	for(vector<string>::iterator it=vectStr.begin();it!=vectStr.end();++it)
	{
		if( (*it).find(sPciId)!=string::npos )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"TranslateSerialUSB found %s, returning %s",
				(*it).c_str(),("/dev/" + FileUtils::FilenameWithoutPath(*it)).c_str());
			return "/dev/" + FileUtils::FilenameWithoutPath(*it);
		}
	}
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TranslateSerialUSB %s couldn't find a match",sInput.c_str());
	
/*
	char tmpFile[40] = "/tmp/devusbXXXXXX";
	mktemp(tmpFile);
	system(("ls -l /sys/bus/usb-serial/devices/ > " + string(tmpFile)).c_str());
	
	vector<string> vectStr;
	FileUtils::ReadFileIntoVector(tmpFile,vectStr);
	for(vector<string>::iterator it=vectStr.begin();it!=vectStr.end();++it)
	{
		if( (*it).find(sInput)!=string::npos )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"TranslateSerialUSB found %s, returning %s",
				(*it).c_str(),("/dev/" + FileUtils::FilenameWithoutPath(*it)).c_str());
			return "/dev/" + FileUtils::FilenameWithoutPath(*it);
		}
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"TranslateSerialUSB %s couldn't find a match",sInput.c_str());
*/
#endif

	return sInput;
}
