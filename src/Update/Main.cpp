#include "DCE/Logger.h"
#include "ServerLogger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCERouter.h"

#include "UpdatesManager.h"

#define VERSION 1.0

#define SCRIPT_NAME "/usr/pluto/bin/LMCEUpdate_Helper.sh"
#define SCRIPT_IN 100
#define SCRIPT_OUT 101

int main(int argc, char* argv[]) 
{
	g_sBinary = FileUtils::FilenameWithoutPath(argv[0]);
	g_sBinaryPath = FileUtils::BasePath(argv[0]);

	cout << "LMCEUpdate, v." << VERSION << endl
		<< "Visit www.linuxmce.org for source code and license information" << endl << endl;

	string sRouter_IP="dcerouter";
	string sLogger="stdout";
	string sXmlPath = "updates.xml";
	string sUpdatesPath = "/home/updates";
	string sOutput="updates.out";

	bool bError=false; // An error parsing the command line
	bool bDownload = true;
	bool bFullDownload = true;
	char c=0;
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
				sRouter_IP = argv[++optnum];
				break;
				
			case 'x':
				sXmlPath = argv[++optnum];
				break;
				
			case 'p':
				sUpdatesPath = argv[++optnum];
				break;
				
			case 'u':
				// process the uploads for this system
				bDownload = false;
				break;
				
			case 'd':
				sOutput = argv[++optnum];
				break;
			
			case 'l':
				sLogger = argv[++optnum];
				break;
			
			// small download
			case 's':
				bFullDownload = false;
				break;
				
			default:
				bError=true;
				break;
		};
	}

	if (bError)
	{
		cout << "A Pluto DCE Device.  See www.plutohome.com/dce for details." << endl
				<< "-d -- Search the updates description and save them into the file name." << endl
				<< "-r -- the IP address of the DCE Router  Defaults to 'dcerouter'." << endl
				<< "-u -- Apply the updates." << endl
				<< "-x -- The optional XML file name used for reading the updates information." << endl
				<< "-p -- The optional path for updates directory." << endl
				<< "-s -- Download only the needed updates." << endl
				<< "-l -- Where to save the log files. Defaults to stdout." << endl;
		exit(0);
	}

	try
	{
		if( sLogger=="dcerouter" )
			LoggerWrapper::SetInstance(new ServerLogger(1, 1, sRouter_IP));
		else if( sLogger=="null" )
			LoggerWrapper::SetType(LT_LOGGER_NULL);
		else if( sLogger!="stdout" )
			LoggerWrapper::SetType(LT_LOGGER_FILE,sLogger);
	}
	catch(...)
	{
		cerr << "Unable to create logger" << endl;
	}

//	LoggerWrapper::GetInstance()->Write(LV_STATUS, "LMCEUpdate starting.  Connecting to: %s",sRouter_IP.c_str());

    // todo
	int manager2script[2];
	int script2manager[2];

	// Init write pipe
	if (pipe(manager2script) == -1) {
		perror("pipe write");
		return(1);
	}

	// Init read pipe
	if (pipe(script2manager) == -1) {
		perror("pipe read");
		return(1);
	}

	// Fork
	int script_pid = fork();
	if (script_pid < 0 ) {
		perror("fork");
		return 1;
	}

	// Run child script
	if (script_pid == 0) {
		close(manager2script[1]);  // Close input pipe fd
		dup2(manager2script[0],SCRIPT_IN); // Make stdin of this process be the same thing as the output of this pipe
		close(manager2script[0]);  // Close output pipe fd

		close(script2manager[0]);  // Close ouput pipe fd
		dup2(script2manager[1],SCRIPT_OUT); // Make stdout of the process be the same thing as the input of the pipe
		close(script2manager[1]);  // Close input pipe fd

		execl(SCRIPT_NAME, SCRIPT_NAME, NULL);

		_exit(0);
	}

	// Run Application
	close(manager2script[0]);
	close(script2manager[1]);
	int sin, sout;
	sin = script2manager[0];
	sout = manager2script[1];
	
	UpdatesManager updManager(sXmlPath.c_str(), sUpdatesPath.c_str(), sin, sout);
	if( c == 'd' )
	{
		if( !updManager.Init(false) || !updManager.GetUpdatesDescription(sOutput.c_str()) )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Updates manager failure.");
			return 1;
		}
	}
	else
	{
		updManager.SetFullDownload(bFullDownload);
		if( !updManager.Init(bDownload) || !updManager.Run() )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Updates manager failure.");
			return 1;
		}
	}
//		updManager.Debug("debug.xml");
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "LMCEUpdate ending");

	return 0;
}
