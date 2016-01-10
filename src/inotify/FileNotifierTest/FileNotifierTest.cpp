/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "DCE/Logger.h"
#include "inotify/FileNotifier.h"

using namespace DCE;

void OnCreateFiles(list<string> &listFiles)
{
	for(list<string>::iterator it = listFiles.begin(); it != listFiles.end(); it++)
	{
		string sItem = *it;
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "File/folder created: %s", sItem.c_str());	
	}
}

void OnDeleteFiles(list<string> &listFiles)
{
    for(list<string>::iterator it = listFiles.begin(); it != listFiles.end(); it++)
    {
        string sItem = *it;
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "File/folder deleted: %s", sItem.c_str());
    }
}

int main(int argc, char* argv[]) 
{
  LoggerWrapper::GetInstance()->Write(LV_WARNING, "Starting...");

  FileNotifier fileNotifier;
  fileNotifier.RegisterCallbacks(OnCreateFiles, OnDeleteFiles);
  fileNotifier.Watch("/home/Chris/Test");
  fileNotifier.Run();

//  while(1) //wait forever here. it's just a test.
//  {
//      Sleep(1000);
//  }

  return 0;
}

