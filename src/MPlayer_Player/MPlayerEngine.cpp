#include <iostream>
#include <vector>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <poll.h>

#include "MPlayerEngine.h"
#include "Log.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"

#define MPLAYER_BINARY "/usr/bin/mplayer"
#define MPLAYER_BINARY_SHORT "mplayer"

using namespace std;

//MPlayerEngine::MPlayerEngine(bool bOverrideAudioSettings, string sALSADevice, bool bALSAPassthrough) : m_engineStateMutex("mplayer-engine-state-mutex"),
//	m_engineReadPipeMutex("mplayer-engine-read-pipe-mutex"),
//	m_engineWritePipeMutex("mplayer-engine-write-pipe-mutex")
MPlayerEngine::MPlayerEngine(bool bOverrideAudioSettings, string sALSADevice, bool bALSAPassthrough, string sGpuAccel, string sGpuDeInt) : m_engineStateMutex("mplayer-engine-state-mutex"),
	m_engineReadPipeMutex("mplayer-engine-read-pipe-mutex"),
	m_engineWritePipeMutex("mplayer-engine-write-pipe-mutex")	
{
	// initializing mutexes
	pthread_mutexattr_t mutexAttr1;
	pthread_mutexattr_init( &mutexAttr1 );
	pthread_mutexattr_settype( &mutexAttr1,  PTHREAD_MUTEX_RECURSIVE_NP );
	m_engineStateMutex.Init( &mutexAttr1 );
	
	pthread_mutexattr_t mutexAttr2;
	pthread_mutexattr_init( &mutexAttr2 );
	pthread_mutexattr_settype( &mutexAttr2,  PTHREAD_MUTEX_RECURSIVE_NP );
	m_engineReadPipeMutex.Init( &mutexAttr2 );
	
	pthread_mutexattr_t mutexAttr3;
	pthread_mutexattr_init( &mutexAttr3 );
	pthread_mutexattr_settype( &mutexAttr3,  PTHREAD_MUTEX_RECURSIVE_NP );
	m_engineWritePipeMutex.Init( &mutexAttr3 );
	
	m_iChildPID = 0;
	m_bEngineIsRunning = false;
	m_bInPipe[0] = m_bInPipe[1] = false;
	m_bOutPipe[0] = m_bOutPipe[1] = false;
	m_bRunEngineOutputReader = false;
	
	m_sALSADevice = sALSADevice;
	m_bALSAPassthrough = bALSAPassthrough;
	m_bOverrideAudioSettings = bOverrideAudioSettings;
	
	m_eEngineState = PLAYBACK_STARTED;
	m_vCurrentPlaylist.push_back(BLACK_MPEG_FILE);

	
	// Assign GPU settings fetched from MD configuration in webadmin
	sHwAccelSetting = sGpuAccel;
	sDeInterlaceSetting = sGpuDeInt;
	Log("Using gpu acceleration setting: " + sHwAccelSetting + "\n");
	Log("Using deInterlace setting:      " + sDeInterlaceSetting + "\n");	

	StartEngine(); // Start MPlayer

}

MPlayerEngine::~MPlayerEngine() {
	StopEngine();
	ClosePipes();
}

void MPlayerEngine::ClosePipes() {
	Log("Closing pipes");
	for (int i=0; i<2; i++) {
		if (m_bInPipe[i])
			close(m_iInPipe[i]);
		if (m_bOutPipe[i])
			close(m_iOutPipe[i]);
		m_bInPipe[i] = m_bOutPipe[i] = false;
	}
	Log("Pipes are closed");
}

bool MPlayerEngine::StartEngine() {
	// TODO add error reporting

	// initializing in pipe
	if ( pipe(m_iInPipe) == -1 ) {
		Log("pipe() failed for in pipe");
		return false;
	}
	else {
		m_bInPipe[0] = m_bInPipe[1] = true;
	}

	// initializing out pipe
	if ( pipe(m_iOutPipe) == -1 ) {
		Log("pipe() failed for out pipe");
		return false;
	}
	else {
		m_bOutPipe[0] = m_bOutPipe[1] = true;
	}

	int pid = fork();
	if (pid == -1) {
		Log("Fork failed!");
		return false;
	}

	if (pid == 0) {
		Log("I'm a child");

		// connecting child stdin to InPipe, stdout & stderr to OutPipe
		if ( dup2(m_iInPipe[0], STDIN_FILENO)<0 || dup2(m_iOutPipe[1], STDOUT_FILENO)<0 ||
				dup2(m_iOutPipe[1], STDERR_FILENO)<0 ) {
			Log("dup2() failed in child");
			printf("%d : %s\n", errno, strerror(errno));
			exit(127);
		}

		Log("Starting mplayer");
		
		if (m_bOverrideAudioSettings) {
			// device dependant, replacing ":" with "=" to keep mplayer parser happy
			string sALSADevice = "alsa:device="+StringUtils::Replace(m_sALSADevice,":", "=");
			
			// depending on the passthrough setting
			string sALSACodec = (m_bALSAPassthrough?"hwac3,hwdts,":"") + string("a52,");

			if (sHwAccelSetting == "vdpau" || sHwAccelSetting == "auto") { // Try to use VDPAU with MPlayer
    				// For VDPAU with fallback to autodetect
				// vo = vdpau,xv
				// vc = ffh264vdpau,ffmpeg12vdpau,ffvc1vdpau,ffwmv3vdpau
				execle(MPLAYER_BINARY, MPLAYER_BINARY_SHORT, "-slave", "-idle", "-msglevel", "all=4", "-noborder",
				  "-fixed-vo", "-fs", "-vo", "vdpau,xv",
				  "-vc", "ffh264vdpau,ffmpeg12vdpau,ffvc1vdpau,ffwmv3vdpau",
				  "-ao", sALSADevice.c_str(),
				  "-ac", sALSACodec.c_str(),
				  "-lavdopts", "fast:threads=2", BLACK_MPEG_FILE, (char *) 0, environ);
			}
			else {
			    execle(MPLAYER_BINARY, MPLAYER_BINARY_SHORT, "-slave", "-idle", "-msglevel", "all=4", "-noborder",
			      "-fixed-vo", "-fs", "-vo", "xv,x11", "-vf", "screenshot",
			      "-ao", sALSADevice.c_str(),
			      "-ac", sALSACodec.c_str(),
			      "-lavdopts", "fast:threads=2", BLACK_MPEG_FILE, (char *) 0, environ);
			}
		}
		else {
			if (sHwAccelSetting == "vdpau" || sHwAccelSetting == "auto") { // Try to use VDPAU with MPlayer			
			    execle(MPLAYER_BINARY, MPLAYER_BINARY_SHORT, "-slave", "-idle", "-msglevel", "all=4", "-noborder",
			      "-fixed-vo", "-fs", "-vo", "vdpau,xv",
			      "-vc", "ffh264vdpau,ffmpeg12vdpau,ffvc1vdpau,ffwmv3vdpau",
	  		      "-lavdopts", "fast:threads=2", BLACK_MPEG_FILE, (char *) 0, environ);
	  		}
	  		else {
	  	    	    execle(MPLAYER_BINARY, MPLAYER_BINARY_SHORT, "-slave", "-idle", "-msglevel", "all=4", "-noborder",
			      "-fixed-vo", "-fs", "-vo", "xv,x11", "-vf", "screenshot",
	  		      "-lavdopts", "fast:threads=2", BLACK_MPEG_FILE, (char *) 0, environ);
	  		}
	  		  
		}

		Log("execle() failed");
		exit(127);
	}
	else {
		Log("I'm a parent");
		m_iChildPID = pid;
		
		// closing child's end of pipe 
		close(m_iInPipe[0]);
		m_bInPipe[0] = false;
		close(m_iOutPipe[1]);
		m_bOutPipe[1] = false;
		fcntl(m_iOutPipe[0], F_SETFL, O_NONBLOCK);
		
		m_bEngineIsRunning = true;

		m_bRunEngineOutputReader = true;
		// TODO error reporting here
		pthread_create(&m_tEngineOutputReaderThread, NULL, &EngineOutputReader, this);
	}

	return true;
}

void MPlayerEngine::StopEngine() {
	if (m_bEngineIsRunning) {
		// quitting engine and giving it time to actually quit
		ExecuteCommand("quit");
		sleep(1);
	
		// if it is still running, let's kill it
		kill(m_iChildPID, SIGKILL);
		m_iChildPID = 0;
	
		// exiting output reader thread
		if (m_bRunEngineOutputReader)
		{
			m_bRunEngineOutputReader = false;
			pthread_join(m_tEngineOutputReaderThread, NULL);
		}
		
		m_bEngineIsRunning = false;
	}
}

string MPlayerEngine::ReadLine() {
	PLUTO_SAFETY_LOCK(readPipeLock, m_engineReadPipeMutex);
	string sResult;
	char buf[10240];
	int iSize = 0;
	do {
		iSize = read(m_iOutPipe[0], buf, 10239);
		if (iSize>0)
		{
			buf[iSize] = 0;
			sResult += buf;
		}
	} while (iSize > 0);
	
	return sResult;
}

void Tokenize(const string& str,
                      vector<string>& tokens,
                      const string& delimiters/* = " "*/)
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

void* EngineOutputReader(void *pInstance) {
	Log("[EngineOutputReader] EngineOutputReader - started");

	MPlayerEngine* pThis = (MPlayerEngine*) pInstance;

	pollfd pfd[1];
	pfd[0].fd = pThis->m_iOutPipe[0];
	pfd[0].events = POLLIN;
	
	string sLastFilename;
	
	while (pThis->m_bRunEngineOutputReader) {
		poll(pfd, 1, 100);
		if (pfd[0].revents & POLLIN)
		{
			string sLine = pThis->ReadLine();
			//Log("Read lines: {\n" + sLine + "\n}");
			vector<string> vLines;
			Tokenize(sLine, vLines, "\n");
			for (vector<string>::iterator i=vLines.begin(); i!=vLines.end(); ++i) {
				Log("[EngineOutputReader] Analyzing line: |" + *i + "|");
				const string sAnsPrefix = "ANS_";
				const string sPlaybackPreInit = "Playing ";
				const string sPlaybackStarted = "Starting playback...";
				const string sPlaybackFinished = "Playback finished: ";
				const string sNoStreamFound = "No stream found.";
				const string sScreenshot = "*** screenshot '";
						
				if ( i->compare(0, sAnsPrefix.length(), sAnsPrefix) == 0 ) {
					if (sLastFilename == BLACK_MPEG_FILE)
					{
						Log("[EngineOutputReader] Found answer: " + *i + ", but ignoring it as it is related to empty mpeg file");
					}
					else
					{
						Log("[EngineOutputReader] Found answer: " + *i);
						vector<string> vAnswer;
						Tokenize(*i, vAnswer, "=");
						string sValue;
						if ( vAnswer.size()>=2 ) {
							sValue = vAnswer[1];
						}
	
						pThis->SetEngineAnswers(vAnswer[0], sValue);
					}
				}
				else if ( i->compare(0, sPlaybackPreInit.length(), sPlaybackPreInit) == 0 ) {
					sLastFilename = i->substr(sPlaybackPreInit.length(), i->length() - sPlaybackPreInit.length() - 1);
					Log("[EngineOutputReader] Playback preinit for file: " + sLastFilename);
				}
				else if ( i->compare(0, sPlaybackStarted.length(), sPlaybackStarted) == 0 ) {
					Log("[EngineOutputReader] Playback started: " + sLastFilename);
					pThis->SetCurrentFile(sLastFilename);
					
				}
				else if ( (i->compare(0, sPlaybackFinished.length(), sPlaybackFinished) == 0)||(i->compare(0, sNoStreamFound.length(), sNoStreamFound) == 0) ) {
					Log("[EngineOutputReader] Playback finished: " + sLastFilename);
					string sPlaylistLastFile = pThis->GetLastFile();
					
					
					if ( sPlaylistLastFile!="" )
					{
						if ( sPlaylistLastFile == sLastFilename )
						{
							pThis->ClearPlaylist();
							pThis->SetEngineState(MPlayerEngine::PLAYBACK_FINISHED);
							Log("[EngineOutputReader] Playlist is empty now");
						}
						
					}
					pThis->SetCurrentFile("");
				}
				// if screenshot found
				else if ( (i->compare(0, sScreenshot.length(), sScreenshot) == 0) )
				{
					string sScreenshotName = i->substr(sScreenshot.length(), i->find_last_of("'") - sScreenshot.length());
					Log("[EngineOutputReader] Detected new screenshot file: " + sScreenshotName);
					pThis->SetCurrentScreenshot(sScreenshotName);
				}
			}
		}
	}

	Log("[EngineOutputReader] EngineOutputReader - stopped");

	return NULL;
}

void MPlayerEngine::ExecuteCommand(string sCommand) {
	PLUTO_SAFETY_LOCK(writePipeLock, m_engineWritePipeMutex);

	char buf[1024];
	int iLen = snprintf(buf, 1024, "%s\n", sCommand.c_str());
	int iSize = write(m_iInPipe[1], buf, iLen);
	cout << "Wrote " << iSize << " bytes" << endl;
}

bool MPlayerEngine::ExecuteCommand(string sCommand, string sResponseName, string &sResponseValue) {
	sResponseName = "ANS_" + sResponseName;
	
	SetEngineAnswers(sResponseName, sResponseName);
	ExecuteCommand(sCommand);
	// giving MPlayer time to reply
	usleep(300000);
	
	string sAnswer = GetEngineAnswers(sResponseName);
	
	if ( sAnswer == sResponseName ) {
		sResponseValue = "";
		return false;
	}
	else {
		sResponseValue = sAnswer;
		return true;
	}
}

void MPlayerEngine::StopPlayback() {
	string sCommand = "loadfile ";
	sCommand += BLACK_MPEG_FILE;
	ExecuteCommand(sCommand);
}

bool MPlayerEngine::StartPlayback(string sMediaFile) {
	// TODO add error handling
	// TODO escape " in sMediaFile
	string sCommand = "loadfile ";
	sCommand += "\"" + sMediaFile + "\"";
	ExecuteCommand(sCommand);
	return true;
}

bool MPlayerEngine::StartPlaylist(vector<string> &vFiles)
{
	PLUTO_SAFETY_LOCK(stateLock, m_engineStateMutex);
	
	if (!vFiles.empty())
	{
		const string sPlaylist = "/tmp/MPlayer_Player.playlist";
		m_vCurrentPlaylist = vFiles;
		FileUtils::WriteVectorToFile(sPlaylist, vFiles);
		
		// early starting to keep watching threads happy
		
		// HACK current file should be already set, otherwise watchers
		// can detected false playback list transition
		// TODO redesign this logic to make the MPlayerEngine
		// 'current_status' a true representative of mplayer status
		// i.e. full slaves
		SetCurrentFile(vFiles[0]);
		SetEngineState(PLAYBACK_STARTED);
		ExecuteCommand("loadlist "+sPlaylist);
		return true;
	}
	else
	{
		m_vCurrentPlaylist.clear();
		SetEngineState(PLAYBACK_FINISHED);
		StopPlayback();
		return false;
	}
}

float MPlayerEngine::GetCurrentPosition() {
	string sCommand = "get_time_pos";
	string sTimePos;
	if ( ExecuteCommand(sCommand, "TIME_POSITION", sTimePos) ) {
		Log("Current stream time position: " + sTimePos);
		return atof(sTimePos.c_str());
	}
	else {
		Log("Failed to get stream time position");
		return 0.0;
	}
}

float MPlayerEngine::GetFileLength() {
	string sCommand = "get_time_length";
	string sTimeLen;
	if ( ExecuteCommand(sCommand, "LENGTH", sTimeLen) ) {
		Log("Current stream time length: " + sTimeLen);
		return atof(sTimeLen.c_str());
	}
	else {
		Log("Failed to get stream time length");
		return 0.0;
	}
}

string MPlayerEngine::GetAudioCodec() {
	string sCommand = "get_audio_codec";
	string sAudioCodec;
	if ( ExecuteCommand(sCommand, "AUDIO_CODEC", sAudioCodec) ) {
		Log("Current stream audio codec: " + sAudioCodec);
		return sAudioCodec;
	}
	else {
		Log("Failed to get stream audio codec");
		return "";
	}
}

string MPlayerEngine::GetVideoCodec() {
	string sCommand = "get_video_codec";
	string sVideoCodec;
	if ( ExecuteCommand(sCommand, "VIDEO_CODEC", sVideoCodec) ) {
		Log("Current stream video codec: " + sVideoCodec);
		return sVideoCodec;
	}
	else {
		Log("Failed to get stream video codec");
		return "";
	}
}

void MPlayerEngine::Seek(float fValue, SeekType eType) {
	string sCommand = "seek ";
	char buf[32];
	snprintf(buf, 32, "%.1f %i", fValue, eType);
	sCommand += buf;
	ExecuteCommand(sCommand);
}

MPlayerEngine::EngineState MPlayerEngine::GetEngineState() {
	PLUTO_SAFETY_LOCK(stateLock, m_engineStateMutex);
	return m_eEngineState;
}

void MPlayerEngine::SetEngineState(const MPlayerEngine::EngineState &eValue) {
	PLUTO_SAFETY_LOCK(stateLock, m_engineStateMutex);
	m_eEngineState = eValue;
}

string MPlayerEngine::GetCurrentFile() {
	PLUTO_SAFETY_LOCK(stateLock, m_engineStateMutex);
	return m_sCurrentFile;
}

void MPlayerEngine::SetCurrentFile(string sName) {
	PLUTO_SAFETY_LOCK(stateLock, m_engineStateMutex);
	m_sCurrentFile = sName;
}

void MPlayerEngine::GetScreenshot(int iWidth, int iHeight, char *&pData, int &iDataSize, string &sFormat, string &sCMD_Result)
{
	Log("MPlayerEngine::GetScreenshot - called");
	
	if (GetEngineState()!=PLAYBACK_STARTED)
	{
		Log("MPlayerEngine::GetScreenshot - player is not in playback state, aborting call");
		return;
	}
	
	char *currDir = get_current_dir_name();
	string sCurrDir = currDir;
	free(currDir);
	
	string sFile = sCurrDir+"/shot0001.png";
	
	// cleaning old file if present
	if (FileUtils::FileExists(sFile))
		FileUtils::DelFile(sFile);
		
	SetCurrentScreenshot("");
	ExecuteCommand("screenshot 0");
	
	//waiting for screenshot
	int iCounter=30;
	while (GetCurrentScreenshot()=="" && iCounter>0)
	{
		Sleep(100);
		iCounter--;
	}
	
	if (GetCurrentScreenshot()=="")
	{
		Log("MPlayerEngine::GetScreenshot - timeout waiting for screenshot maker, aborting");
		return;
	}
	
	sFile = sCurrDir+"/" + GetCurrentScreenshot();
	
	iCounter=30;
	while (!FileUtils::FileExists(sFile) && iCounter>0)
	{
		Sleep(100);
		iCounter--;
	}
	
	if (!FileUtils::FileExists(sFile))
	{
		Log("MPlayerEngine::GetScreenshot - timeout waiting for screenshot writer, aborting");
		return;
	}
	
	// HACK: let it finally write
	Sleep(1000);
	
	string sTargetFile = "/tmp/file_mp.jpg";
	string sCommand = "/usr/bin/convert -geometry " + StringUtils::itos(iWidth)+"x"+StringUtils::itos(iHeight) + " " + sFile + " " + sTargetFile;
	Log("Converting "+sFile+" => " + sTargetFile + " with command \""+sCommand+"\"");
	system(sCommand.c_str());
	if ( FileUtils::FileExists(sTargetFile) )
	{
		Log("Converting succeeded, reading target file");
		size_t iSize=0;
		pData = FileUtils::ReadFileIntoBuffer(sTargetFile, iSize);
		iDataSize = iSize;
		sFormat = "JPG";
	}
	else
	{
		Log("Converting failed, no target file found?");

	}
}

void MPlayerEngine::SetCurrentScreenshot(string sName)
{
	PLUTO_SAFETY_LOCK(stateLock, m_engineStateMutex);
	m_sCurrentScreenshot = sName;
}

string MPlayerEngine::GetCurrentScreenshot()
{
	PLUTO_SAFETY_LOCK(stateLock, m_engineStateMutex);
	return m_sCurrentScreenshot;
}

void MPlayerEngine::SetEngineAnswers(string key, string value)
{
	PLUTO_SAFETY_LOCK(stateLock, m_engineStateMutex);
	m_mEngineAnswers[key] = value;
}

string MPlayerEngine::GetEngineAnswers(string key)
{
	PLUTO_SAFETY_LOCK(stateLock, m_engineStateMutex);
	return m_mEngineAnswers[key];
}

string MPlayerEngine::GetLastFile()
{
	PLUTO_SAFETY_LOCK(stateLock, m_engineStateMutex);
	vector<string>::reverse_iterator ri = m_vCurrentPlaylist.rbegin();
	if (ri == m_vCurrentPlaylist.rend())
		return "";
	else
		return *ri;
}

void MPlayerEngine::ClearPlaylist()
{
	PLUTO_SAFETY_LOCK(stateLock, m_engineStateMutex);
	m_vCurrentPlaylist.clear();
}

int MPlayerEngine::GetChildPID()
{
	return m_iChildPID;
}
