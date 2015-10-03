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
    : INotifyEmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
    m_pEmulatorModel->m_sEmulatorBinary="/usr/bin/mame";
    m_pEmulatorModel->m_sProcessName="mame";
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
    return INotifyEmulatorController::init();
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
    return (m_pEmulatorModel->m_sSystemName == "arcade" ? FileUtils::FileWithoutExtension(FileUtils::FilenameWithoutPath(sMedia)) : m_pEmulatorModel->m_sSystemName);
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

  string MAMEEmulatorController::getSlotsAndRoms()
  {

    string sRet = "";

    if (!m_pEmulatorModel)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MAMEEmulatorController::getSlotsAndRoms() - no m_pEmulatorModel, bailing!");
	return "";
      }

    for (map<string,string>::iterator it=m_pEmulatorModel->m_mapMedia.begin(); it!=m_pEmulatorModel->m_mapMedia.end(); ++it)
      {
	string sSlot = it->first;
	string sMediaFile = it->second;
	
	if (sSlot == "default")
	  {
	    // By default, cart is used.
	    sSlot = "cart";
	  }
	else
	  {
	    // do not change.
	  }
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Inserting %s into slot %s",sSlot.c_str(),sMediaFile.c_str());
	
	sRet += "-" + sSlot + "\t" +
	  sMediaFile;
      }

    return sRet;

  }

  bool MAMEEmulatorController::run()
  {
    
    if (!m_pEmulatorModel)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MAMEEmulatorController::run() - pEmulatorModel is null. Bailing.");
	return false;
      }

    if (m_pEmulatorModel->m_sSystemName == "arcade")
      {
	m_pEmulatorModel->m_sArgs = getRomFromSlot();
      }
    else
      {
	m_pEmulatorModel->m_sArgs = getRomFromSlot() + "\t" + getSlotsAndRoms();
      }

    // If a media position was set (i.e. as part of the CMD_Play_Media, then put it in
    // the mame configuration file to load. Otherwise, blank it out.
    if (!m_pEmulatorModel->m_sMediaPosition.empty() && 
	!m_pEmulatorModel->m_bIsStreamingSource && 
	m_pEmulatorModel->m_bCanSaveState)
      {
	m_pEmulatorModel->m_sState = "1";

	string sPath = "/home/mamedata/sta/"+getRomFromSlot();
	string sSource = sPath + "/" + m_pEmulatorModel->m_sMediaPosition;
	string sDest = "/run/Game_Player/state";
	
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
	m_pEmulatorModel->m_sEmulatorBinary="/usr/bin/csmame";
	m_pEmulatorModel->m_sState="";
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
	m_pEmulatorModel->m_sEmulatorBinary="/usr/bin/mame";
      }

    if (INotifyEmulatorController::run())
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
    
    return INotifyEmulatorController::gotoMenu(iMenu); // and up the chain...
  }

  bool MAMEEmulatorController::saveState(string& sPosition, string& sText, bool bAutoSave, string sAutoSaveName)
  {
    int iRetries=10;
    if (!m_pEmulatorModel->m_bIsStreaming && 
	m_pEmulatorModel->m_bCanSaveState)
      {
	doAction("SAVE_STATE");

	while (!FileUtils::FileExists("/run/Game_Player/state/"+getRomFromSlot()+"/state.sta"))
	  {
	    if (iRetries>0)
	      {
		iRetries--;
		Sleep(100);
	      }
	    else
	      {
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MAMEEmulatorController::saveState() - State file /run/Game_Player/state/%s/state.sta does not exist. Bailing!",getRomFromSlot().c_str());
		sPosition="";
		sText="";
		return false;
	      }
	  }
	
	// Autosave bookmark is always saved as AutoSave if bAutoSave is true
	// otherwise, generate an md5 from the state file and use that as the
	// saved filename on disk. 
	string sBookmarkName;
	if (!bAutoSave)
	  {
	    sBookmarkName = FileUtils::FileChecksum("/run/Game_Player/state/"+getRomFromSlot()+"/state.sta");
	  }
	else
	  {
	    sBookmarkName = "AutoSave";
	  }

	string sCmd00 = "mkdir -p /home/mamedata/sta/"+getRomFromSlot();
	system(sCmd00.c_str());

	string sCmd = "mv /run/Game_Player/state/"+getRomFromSlot()+"/state.sta /home/mamedata/sta/"+getRomFromSlot()+"/"+sBookmarkName;
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
	string sDest = "/run/Game_Player/state/"+getRomFromSlot();
	
	string sCmd0 = "mkdir -p "+sDest;
	system(sCmd0.c_str());

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
	string sCmd = "cp "+sSource+" "+sDest+"/state.sta";
	system(sCmd.c_str());
	
	if (!FileUtils::FileExists(sDest))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MAMEEmulatorController::loadState() - Could not load copied state file %s as it does not exist. Bailing!",sDest.c_str());
	  }
	
	// File moved into place, save state.
	doAction("LOAD_STATE");
	
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
