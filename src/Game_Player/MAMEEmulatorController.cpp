/**
 * MAMEEmulatorController - Support to control the MAME emulator.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "MAMEEmulatorController.h"
#include "PlutoUtils/FileUtils.h"

namespace DCE
{
  MAMEEmulatorController::MAMEEmulatorController(Game_Player *pGame_Player, MAMEEmulatorModel *pEmulatorModel)
    : X11EmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
    m_pEmulatorModel->m_sEmulatorBinary="/usr/bin/mame";
    m_pEmulatorModel->m_sProcessName="mame";
    m_pEmulatorModel->m_sWindowName="mame.mame"; // wmclass.wmname of window.
    m_pEmulatorModel->m_bChangeRequiresRestart=true;
    m_pEmulatorModel->m_bRunning=false;
    m_pEmulatorModel->m_bHasArgs=true;
    m_pEmulatorModel->m_sArgs="";
  }

  MAMEEmulatorController::~MAMEEmulatorController()
  {
   
  }

  bool MAMEEmulatorController::init()
  {
    return X11EmulatorController::init();
  }

  string MAMEEmulatorController::getRomFromSlot()
  {
    string sMedia;
    if (getMediaInSlot(sMedia))
      {
	if (!FileUtils::FileExists(sMedia))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MAMEEmulatorController::run() - could not find media in default slot: %s",sMedia.c_str());
	    return "";
	  }
      }
    else
      {
	return "";
      }
    return FileUtils::FileWithoutExtension(FileUtils::FilenameWithoutPath(sMedia));
  }

  string MAMEEmulatorController::getRomPathFromSlot()
  {
    string sMedia;
    if (!getMediaInSlot(sMedia))
      {
	return "";
      }
    else
      {
	return FileUtils::BasePath(sMedia);
      }
  }

  bool MAMEEmulatorController::run()
  {
    
    if (!m_pEmulatorModel)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MAMEEmulatorController::run() - pEmulatorModel is null. Bailing.");
	return false;
      }

    m_pEmulatorModel->m_sArgs = getRomFromSlot();
    // If a media position was set (i.e. as part of the CMD_Play_Media, then put it in
    // the mame configuration file to load. Otherwise, blank it out.
    if (!m_pEmulatorModel->m_sMediaPosition.empty() && 
	!m_pEmulatorModel->m_bIsStreamingSource && 
	m_pEmulatorModel->m_bCanSaveState)
      {
	m_pEmulatorModel->m_sState = "1";

	string sPath = "/home/mamedata/sta/"+getRomFromSlot();
	string sSource = sPath + "/" + m_pEmulatorModel->m_sMediaPosition;
	string sDest = sPath + "/1.sta";
	
	FileUtils::DelFile(sDest); // some basic house cleaning.
	
	if (m_pEmulatorModel->m_sMediaPosition.find("AutoSave") != string::npos)
	  {
	    // Autosave specified, just load the autosave file for this game.
	    sSource = sPath + "/AutoSave";
	  }
	
	if (!FileUtils::FileExists(sSource))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MAMEEmulatorController::loadState() - Could not load state file %s as it does not exist.",sSource.c_str());
	  }
	
	// move file into place.
	string sCmd = "cp "+sSource+" "+sDest;
	system(sCmd.c_str());
	
	if (!FileUtils::FileExists(sDest))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MAMEEmulatorController::loadState() - Could not load copied state file %s as it does not exist. Bailing!",sDest.c_str());
	  }
      }
    else
      {
	m_pEmulatorModel->m_sState = "";      
      }
    
    // Finally, let's take care of setting up streaming, if needed.
    if (m_pEmulatorModel->m_bIsStreaming)
      {
	m_pEmulatorModel->m_sProcessName="csmame";
	m_pEmulatorModel->m_sWindowName="csmame.csmame";
	m_pEmulatorModel->m_sEmulatorBinary="/usr/bin/csmame";
	if (m_pEmulatorModel->m_bIsStreamingSource)
	  {
	    // This is the server, set args as needed
	    m_pEmulatorModel->m_sArgs += "\t-server";
	  }
	else
	  {
	    m_pEmulatorModel->m_sArgs += "\t-client\t-hostname\t"+m_pEmulatorModel->m_sHostName;
	  }
      }
    else
      {
	// Regular non streaming instance.
	m_pEmulatorModel->m_sProcessName="mame";
	m_pEmulatorModel->m_sWindowName="mame.mame";
	m_pEmulatorModel->m_sEmulatorBinary="/usr/bin/mame";
      }

    if (X11EmulatorController::run())
      return true;

    return false;

  }

  bool MAMEEmulatorController::setSpeed(int iSpeed)
  {
    switch (iSpeed)
      {
      case 0:
	pause();
	break;
      case 1000:
	unpause();
	break;
      default:
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"MAMEEmulatorController:setSpeed(%d) not implemented. Ignoring.");
	break;
      }
    return EmulatorController::setSpeed(iSpeed);
  }

  bool MAMEEmulatorController::gotoMenu(int iMenu)
  {
    switch(iMenu)
      {
      case 0:
	if (m_pEmulatorModel->m_iActiveMenu > 1) // 2 and above is a UI of some sort.
	  doAction("UI_EXIT");
	break;
      case 1:
	doAction("MONITOR_MODE");
	break;
      case 2:
	doAction("UI_ENTER");
      }
    
    return X11EmulatorController::gotoMenu(iMenu); // and up the chain...
  }

  bool MAMEEmulatorController::saveState(string& sPosition, string& sText, bool bAutoSave, string sAutoSaveName)
  {

    if (!m_pEmulatorModel->m_bIsStreaming && 
	m_pEmulatorModel->m_bCanSaveState)
      {
	doAction("SAVE_STATE");
	doAction("1");
	
	// the save game is now in /home/mamedata/sta/<ROMNAME>/1.sta. We need to 
	// make a copy of this to reference later.
	
	if (!FileUtils::FileExists("/home/mamedata/sta/"+getRomFromSlot()+"/1.sta"))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MAMEEmulatorController::saveState() - State file /home/mamedata/sta/%s/1.sta does not exist. Bailing!",getRomFromSlot().c_str());
	    sPosition="";
	    sText="";
	    return false;
	  }
	
	// Autosave bookmark is always saved as AutoSave if bAutoSave is true
	// otherwise, generate an md5 from the state file and use that as the
	// saved filename on disk. 
	string sBookmarkName;
	if (!bAutoSave)
	  {
	    sBookmarkName = FileUtils::FileChecksum("/home/mamedata/sta/"+getRomFromSlot()+"/1.sta");
	  }
	else
	  {
	    sBookmarkName = "AutoSave";
	  }
	string sCmd = "mv /home/mamedata/sta/"+getRomFromSlot()+"/1.sta /home/mamedata/sta/"+getRomFromSlot()+"/"+sBookmarkName;
	system(sCmd.c_str());
	
	if (!FileUtils::FileExists("/home/mamedata/sta/"+getRomFromSlot()+"/"+sBookmarkName))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MAMEEmulatorController::saveState() - Moving /home/mamedata/sta/%s/1.sta to /home/mamedata/sta/%s/%s failed. Bailing!",getRomFromSlot().c_str(),getRomFromSlot().c_str(),sBookmarkName.c_str());
	    sPosition="";
	    sText="";
	    return false;
	  }
	
	// all is successsful, set sPosition and sText for return.
	sPosition = sBookmarkName;
	sText = "Saved bookmark on "+StringUtils::SQLDateTime(0)+" at "+StringUtils::HourMinute(0,false);
	
	return true;
      }
    else
      {
	// State ignored in streaming mode.
	return true;
      }
  }

  void MAMEEmulatorController::setMediaPosition(string sMediaPosition)
  {
    EmulatorController::setMediaPosition(sMediaPosition);
    return;
  }
  
  bool MAMEEmulatorController::loadState(string sPosition)
  {
    
    if (!m_pEmulatorModel->m_bIsStreaming && m_pEmulatorModel->m_bCanSaveState)
      {
	string sPath = "/home/mamedata/sta/"+getRomFromSlot();
	string sSource = sPath + "/" + sPosition;
	string sDest = sPath + "/1.sta";
	
	FileUtils::DelFile(sDest); // some basic house cleaning.
	
	if (sPosition.find("AutoSave") != string::npos)
	  {
	    // Autosave specified, just load the autosave file for this game.
	    sSource = sPath + "/AutoSave";
	  }
	
	if (!FileUtils::FileExists(sSource))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MAMEEmulatorController::loadState() - Could not load state file %s as it does not exist.",sSource.c_str());
	    return false;
	  }
	
	// move file into place.
	string sCmd = "cp "+sSource+" "+sDest;
	system(sCmd.c_str());
	
	if (!FileUtils::FileExists(sDest))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MAMEEmulatorController::loadState() - Could not load copied state file %s as it does not exist. Bailing!",sDest.c_str());
	  }
	
	// File moved into place, save state.
	doAction("LOAD_STATE");
	doAction("1");
	
	return true;
      }
    else
      {
	// No state pushed during streaming. csmame freaks.
	return true;
      }
  }
  
  /** 
   * given a path, iterate through files named 0000.avi, etc. until one does not
   * exist. Then return that path name. 
   * This was done to support multiple recordings being done of the same game.
   */
  bool MAMEEmulatorController::getRecordingFilename(string sPath, string &sFilename)
  {
    int i;
    for (i=0;i<100;i++)
      {
	char cFilename[9];
	snprintf(cFilename,sizeof(cFilename),"%04d.avi",i);
	string sCurrentIndex = cFilename;
	string sCurrentFilename = sPath + "/" + sCurrentIndex;
	if (!FileUtils::FileExists(sCurrentFilename))
	  {
	    // found one, set and break. This is what MAME will name the file. 
	    sFilename = sCurrentIndex;
	    return true;
	  }
      }
    return false; // Somehow we have 100 active game recordings for this game. this is probably bad.
  }

  bool MAMEEmulatorController::record()
  {
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MAMEEmulatorController::record()");
    string sPath = "/home/mamedata/shots/"+getRomFromSlot();
    if (!m_pEmulatorModel->m_bIsRecording)
      {
	// Not recording. Start a recording.
	// Our patched MAME records an AVI by default.
	if (!getRecordingFilename(sPath,m_pEmulatorModel->m_sRecordingFilename))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MAMEEmulatorController::record() - Could not get a filename for path %s. Bailing on record!",sPath.c_str());
	    return false;
	  }
	EmulatorController::record(); // set recording flags and trigger recording action.
      }
    else
      {
	EmulatorController::record();
	// We are recording, trigger a transcoding job.
	m_pGame_Player->TranscodeAfterRecord(sPath,
					     m_pEmulatorModel->m_sRecordingFilename,
					     m_pEmulatorModel->m_dwPK_Device_Orbiter);
      }
    return true;
  }

}
