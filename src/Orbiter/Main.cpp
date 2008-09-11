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

//<-dceag-incl-b->!
#include "Main.h"
#include "Orbiter.h"
#include "SDL/StartOrbiterSDL.h"
#include "DCE/Logger.h"
#include "ServerLogger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"

#include "../include/version.cpp"

using namespace DCE;
//<-dceag-incl-e->

// For whatever reason, if I put: #include "OrbiterImageSDL.h" in this file,
// then Microsoft won't compile, reporting an unresolved symbol _main.  It is something
// to do with SDL.  Moving those references to another cpp file solves the problems.  See StartOrbiterSDL.cpp
bool StartOrbiter(int PK_Device,int PK_DeviceTemplate,string sRouter_IP,string sLocalDirectory,bool bLocalMode,
				  int Width,int Height, bool bFullScreen);



//<-dceag-plug-b->! never a plug-in

//<-dceag-main-b->!  **DON'T AUTOMATICALLY OVERWRITE THIS SECTIONS, IT'S CUSTOM
// int main(int argc, char* argv[])
bool ParseCommandLineParams(int argc, char* argv[], CommandLineParams &commandlineparams)
{
	g_sBinary = FileUtils::FilenameWithoutPath(argv[0]);
	g_sBinaryPath = FileUtils::BasePath(argv[0]);

	cout<<"Copyright (C) 2004 Pluto, Inc., a Florida Corporation"<<endl
		<<"www.plutohome.com"<<endl
		<<"Phone: +1 (877) 758-8648"<<endl
		<<"This program is distributed according to the terms of the Pluto Public License, available at: "<<endl
		<<"http://plutohome.com/index.php?section=public_license "<<endl
		<<"This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; "<<endl
		<<"without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. "<<endl
		<<"See the Pluto Public License for more details."<<endl << "-----" << endl << endl;
  
  bool bError=false; // An error parsing the command line
    char c;
    for(int optnum=1;optnum<argc;++optnum)
    {
        if( argv[optnum][0]!='-' )
        {
            cerr << "Unknown option " << argv[optnum] << endl;
            bError=true;
        }

        c=argv[optnum][1];
        switch (c)
        {
        case 'r':
          commandlineparams.sRouter_IP = argv[++optnum];
            break;
        case 'd':
          commandlineparams.PK_Device = atoi(argv[++optnum]);
            break;
        case 'l':
          commandlineparams.sLogger = argv[++optnum];
            break;
        case 'D':
          commandlineparams.sLocalDirectory = argv[++optnum];
            break;
        case 'L':
          commandlineparams.bLocalMode = true;
            break;
        case 'W':
          commandlineparams.Width = atoi(argv[++optnum]);
            break;
        case 'H':
          commandlineparams.Height = atoi(argv[++optnum]);
            break;
		case 'F':
			commandlineparams.bFullScreen = true;
			break;
		case 'T':
      commandlineparams.PK_DeviceTemplate = atoi(argv[++optnum]);
			break;
        default:
            bError=true;
            break;
        };
    }

    if (bError)
    {

//                123456789012345678901234567890123456789012345678901234567890
	cout << "Orbiter, v." << VERSION << endl
            << "A Pluto DCE Device.  See www.plutohome.com/dce for details." << endl
            << "Usage: Orbiter [-r Router's IP] [-d My Device ID] [-l dcerouter|stdout|null|filename]" << endl
	    << "[-D Directory] [-L] [-W Width] [-H Height]" << endl
            << "-r router's IP		-- the IP address of the DCE Router  Defaults to 'dcerouter'." << endl
            << "-d my device ID		-- This device's ID number.  If not specified, it will be requested" << endl
	    << " 	from the router based on our IP address." << endl
            << "-l dcerouter		-- Where to save the log files.  Specify 'dce_router' to have " << endl
	    << "	the messages logged to the DCE Router.  Defaults to stdout." << endl
            << "-D directory		-- If a directory is specified, it will look for it's image " << endl
	    << "	and config files here rather than requesting from the server." << endl
            << "-L 			-- Local mode only.  Do not connect to the server.  All messages will just loop back." << endl
            << "-F			-- Full screen."
			<< "-W/H			-- Width/Height default to full screen." << endl;

        return false;
    }

	

    try
    {
        if( commandlineparams.sLogger=="dcerouter" )
			LoggerWrapper::SetInstance(new ServerLogger(commandlineparams.PK_Device, Orbiter::PK_DeviceTemplate_get_static(), commandlineparams.sRouter_IP));
        else if( commandlineparams.sLogger=="null" )
            LoggerWrapper::SetType(LT_LOGGER_NULL);
        else if( commandlineparams.sLogger!="stdout" )
			LoggerWrapper::SetType(LT_LOGGER_FILE,commandlineparams.sLogger.c_str());
    }
    catch(...)
    {
        cerr << "Unable to create logger" << endl;
    }

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Device: %d starting",commandlineparams.PK_Device);

    if( commandlineparams.sLocalDirectory.length()>0 && commandlineparams.sLocalDirectory[ commandlineparams.sLocalDirectory.length()-1 ]!='/' )
        commandlineparams.sLocalDirectory += "/";

    return true;
}

//<-dceag-main-e->
