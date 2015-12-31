/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#ifndef __MAIN_H__
#define __MAIN_H__

#include <string>
using namespace std;

struct CommandLineParams
{
  CommandLineParams()
      : sRouter_IP("dcerouter")
      , PK_Device(0)
      , PK_DeviceTemplate(0)
      , sLogger("stdout")
      , Width(800)
      , Height(600)
      , bFullScreen(false)
	  , bUseOpenGL(false)
      , sLocalDirectory("")
      , sNestedDisplay("")
      , bLocalMode(false)
    {
    }
  string sRouter_IP;
  int PK_Device;
  int PK_DeviceTemplate;
  string sLogger;
  int Width;
  int Height;
	bool bFullScreen;
	bool bUseOpenGL;
  string sLocalDirectory;
  string sNestedDisplay;
  bool bLocalMode;
};

bool ParseCommandLineParams(int argc, char* argv[], CommandLineParams &commandlineparams);

#endif
