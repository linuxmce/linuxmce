/**
 * MESSEmulatorController - Support to control the MESS emulator.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "MESSEmulatorController.h"
#include "PlutoUtils/FileUtils.h"

namespace DCE
{
  MESSEmulatorController::MESSEmulatorController(Game_Player *pGame_Player, MESSEmulatorModel *pEmulatorModel)
    : X11EmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
    m_pEmulatorModel->m_sEmulatorBinary="/usr/bin/mess";
    m_pEmulatorModel->m_sProcessName="mess";
    m_pEmulatorModel->m_sWindowName="mess.mess"; // wmclass.wmname of window.
    m_pEmulatorModel->m_bChangeRequiresRestart=true;
    m_pEmulatorModel->m_bRunning=false;
    m_pEmulatorModel->m_bHasArgs=true;
    m_pEmulatorModel->m_sArgs="";
  }

  MESSEmulatorController::~MESSEmulatorController()
  {
   
  }

  void MESSEmulatorController::setSystemName(string sSystemName)
  {
    if (m_pEmulatorModel)
      {
	m_pEmulatorModel->m_sSystemName=sSystemName;
      }
  }

  string MESSEmulatorController::getSystemName()
  {
    if (m_pEmulatorModel)
      {
	return m_pEmulatorModel->m_sSystemName;
      }
    else
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MESSEmulatorController::getSystemName - no Emulator model, returning empty value.");
	return "";
      }
  }

  bool MESSEmulatorController::init()
  {
    return X11EmulatorController::init();
  }

  string MESSEmulatorController::getSystemFromSlot()
  {
    return (!m_pEmulatorModel->m_sSystemName.empty() ? m_pEmulatorModel->m_sSystemName : "ERROR_NOT_DEFINED");
  }

  string MESSEmulatorController::getSlotTypeForFilename(string sFilename)
  {
    // By default, we simply return -cart, as this is the usual for game systems.
    // Subclasses should override this with logic to return the appropriate slot type
    // given a filename.

    return "-cart";
  }
  
  string MESSEmulatorController::getRomFromSlot()
  {
    string sMedia;
    if (this->getMediaInSlot(sMedia))
      {
	if (!FileUtils::FileExists(sMedia))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MESSEmulatorController::run() - could not find media in default slot: %s",sMedia.c_str());
	    return "";
	  }
      }
    else
      {
	return "";
      }
    return sMedia;
  }

  string MESSEmulatorController::getRomPathFromSlot()
  {
    string sMedia;
    if (!this->getMediaInSlot(sMedia))
      {
	return "";
      }
    else
      {
	return FileUtils::BasePath(sMedia);
      }
  }

  string MESSEmulatorController::getSlotsAndRoms()
  {

    string sRet = "";

    if (!m_pEmulatorModel)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MESSEmulatorController::getSlotsAndRoms() - no m_pEmulatorModel, bailing!");
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

  string MESSEmulatorController::getAdditionalOptions()
  {
    return ""; // This is overridden as needed.
  }

  bool MESSEmulatorController::run()
  {
    
    if (!m_pEmulatorModel)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MESSEmulatorController::run() - pEmulatorModel is null. Bailing.");
	return false;
      }

    m_pEmulatorModel->m_sArgs = getSystemFromSlot() + "\t" +
      getSlotsAndRoms() + getAdditionalOptions();

    //m_pEmulatorModel->m_sArgs = this->getSystemFromSlot() + "\t" + 
    //  this->getSlotTypeForFilename(getRomFromSlot()) + "\t" + 
    //  this->getRomFromSlot();

    m_pEmulatorModel->m_sRomPath = this->getRomPathFromSlot();

    // If a media position was set (i.e. as part of the CMD_Play_Media, then put it in
    // the mess configuration file to load. Otherwise, blank it out.
    if (!m_pEmulatorModel->m_sMediaPosition.empty() && !m_pEmulatorModel->m_bIsStreamingSource)
      {
	m_pEmulatorModel->m_sState = "1";

	string sPath = "/home/mamedata/sta/"+getSystemFromSlot();
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
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MESSEmulatorController::loadState() - Could not load state file %s as it does not exist.",sSource.c_str());
	  }
	
	// move file into place.
	string sCmd = "cp "+sSource+" "+sDest;
	system(sCmd.c_str());
	
	if (!FileUtils::FileExists(sDest))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MESSEmulatorController::loadState() - Could not load copied state file %s as it does not exist. Bailing!",sDest.c_str());
	  }
      }
    else
      {
	m_pEmulatorModel->m_sState = "";      
      }
    
    // Finally, let's take care of setting up streaming, if needed.
    if (m_pEmulatorModel->m_bIsStreaming)
      {
	m_pEmulatorModel->m_sProcessName="csmess";
	m_pEmulatorModel->m_sWindowName="csmess.csmess";
	m_pEmulatorModel->m_sEmulatorBinary="/usr/bin/csmess";
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
	m_pEmulatorModel->m_sProcessName="mess";
	m_pEmulatorModel->m_sWindowName="mess.mess";
	m_pEmulatorModel->m_sEmulatorBinary="/usr/bin/mess";
      }

    if (X11EmulatorController::run())
      return true;

    return false;

  }

  bool MESSEmulatorController::reset()
  {
    if (m_pEmulatorModel->m_bIsComputer)
      doAction("PARTIALKBD");
    usleep(2000);
    bool bRet = doAction("RESET");
    usleep(2000);
    if (m_pEmulatorModel->m_bIsComputer)
      doAction("PARTIALKBD");
    return bRet;
  }

  bool MESSEmulatorController::pause()
  {
    if (m_pEmulatorModel->m_bIsComputer)
      doAction("PARTIALKBD");
    usleep(2000);
    bool bRet = doAction("PAUSE");
    usleep(2000);
    if (m_pEmulatorModel->m_bIsComputer)
      doAction("PARTIALKBD");
    return bRet;
  }

  bool MESSEmulatorController::unpause()
  {
    if (m_pEmulatorModel->m_bIsComputer)
      doAction("PARTIALKBD");
    usleep(2000);
    bool bRet = doAction("UNPAUSE");
    usleep(2000);
    if (m_pEmulatorModel->m_bIsComputer)
      doAction("PARTIALKBD");
    return bRet;
  }
  
  bool MESSEmulatorController::setSpeed(int iSpeed)
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
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"MESSEmulatorController:setSpeed(%d) not implemented. Ignoring.");
	break;
      }
    return EmulatorController::setSpeed(iSpeed);
  }

  bool MESSEmulatorController::gotoMenu(int iMenu)
  {
    switch(iMenu)
      {
      case 0:
	if (m_pEmulatorModel->m_iActiveMenu > 1 && m_pEmulatorModel->m_iActiveMenu != 3) // 2 and above is a UI of some sort.
	  {
	    if (m_pEmulatorModel->m_bIsComputer)
	      doAction("PARTIALKBD");
	    doAction("UI_EXIT");
	    if (m_pEmulatorModel->m_bIsComputer)
	      doAction("PARTIALKBD");
	  }
	break;
      case 1:
	if (m_pEmulatorModel->m_bIsComputer)
	  doAction("PARTIALKBD");
	doAction("MONITOR_MODE");
	if (m_pEmulatorModel->m_bIsComputer)
	  doAction("PARTIALKBD");
	break;
      case 2:
	if (m_pEmulatorModel->m_bIsComputer)
	  doAction("PARTIALKBD");
	doAction("UI_ENTER");
	if (m_pEmulatorModel->m_bIsComputer)
	  doAction("PARTIALKBD");
	break;
      case 3:
	usleep(2000);
	break;
      }
    
    return X11EmulatorController::gotoMenu(iMenu); // and up the chain...
  }

  bool MESSEmulatorController::saveState(string& sPosition, string& sText, bool bAutoSave, string sAutoSaveName)
  {

    if (!m_pEmulatorModel->m_bIsStreaming && 
	m_pEmulatorModel->m_bCanSaveState)
      {
	if (m_pEmulatorModel->m_bIsComputer)
	  doAction("PARTIALKBD");
	doAction("SAVE_STATE");
	doAction("1");
	if (m_pEmulatorModel->m_bIsComputer)
	  doAction("PARTIALKBD");

	// the save game is now in /home/mamedata/sta/<ROMNAME>/1.sta. We need to 
	// make a copy of this to reference later.
	
	if (!FileUtils::FileExists("/home/mamedata/sta/"+getSystemName()+"/1.sta"))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MESSEmulatorController::saveState() - State file /home/mamedata/sta/%s/1.sta does not exist. Bailing!",getRomFromSlot().c_str());
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
	    sBookmarkName = FileUtils::FileChecksum("/home/mamedata/sta/"+getSystemName()+"/1.sta");
	  }
	else
	  {
	    sBookmarkName = "AutoSave";
	  }
	string sCmd = "mv /home/mamedata/sta/"+getSystemName()+"/1.sta /home/mamedata/sta/"+getSystemName()+"/"+sBookmarkName;
	system(sCmd.c_str());
	
	if (!FileUtils::FileExists("/home/mamedata/sta/"+getSystemName()+"/"+sBookmarkName))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MESSEmulatorController::saveState() - Moving /home/mamedata/sta/%s/1.sta to /home/mamedata/sta/%s/%s failed. Bailing!",getSystemName().c_str(),getSystemName().c_str(),sBookmarkName.c_str());
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

  void MESSEmulatorController::setMediaPosition(string sMediaPosition)
  {
    EmulatorController::setMediaPosition(sMediaPosition);
    return;
  }
  
  bool MESSEmulatorController::loadState(string sPosition)
  {
    
    if (!m_pEmulatorModel->m_bIsStreaming && 
	m_pEmulatorModel->m_bCanSaveState)
      {
	string sPath = "/home/mamedata/sta/"+getSystemName();
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
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MESSEmulatorController::loadState() - Could not load state file %s as it does not exist.",sSource.c_str());
	    return false;
	  }
	
	// move file into place.
	string sCmd = "cp "+sSource+" "+sDest;
	system(sCmd.c_str());
	
	if (!FileUtils::FileExists(sDest))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MESSEmulatorController::loadState() - Could not load copied state file %s as it does not exist. Bailing!",sDest.c_str());
	  }
	
	// File moved into place, save state.
	if (m_pEmulatorModel->m_bIsComputer)
	  doAction("PARTIALKBD");
	doAction("LOAD_STATE");
	doAction("1");
	if (m_pEmulatorModel->m_bIsComputer)
	  doAction("PARTIALKBD");
	
	return true;
      }
    else
      {
	// No state pushed during streaming. csmess freaks.
	return true;
      }
  }
  
  /** 
   * given a path, iterate through files named 0000.avi, etc. until one does not
   * exist. Then return that path name. 
   * This was done to support multiple recordings being done of the same game.
   */
  bool MESSEmulatorController::getRecordingFilename(string sPath, string &sFilename)
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
	    // found one, set and break. This is what MESS will name the file. 
	    sFilename = sCurrentIndex;
	    return true;
	  }
      }
    return false; // Somehow we have 100 active game recordings for this game. this is probably bad.
  }

  bool MESSEmulatorController::record()
  {
    if (!m_pEmulatorModel)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MESSEmulatorController::record() - called with no emulator model. Aborting.");
      return false;
    }

    string sPath = "/home/mamedata/shots/"+m_pEmulatorModel->m_sSystemName;
    if (!m_pEmulatorModel->m_bIsRecording)
      {
	// Not recording. Start a recording.
	// Our patched MESS records an AVI by default.
	if (!getRecordingFilename(sPath,m_pEmulatorModel->m_sRecordingFilename))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MESSEmulatorController::record() - Could not get a filename for path %s. Bailing on record!",sPath.c_str());
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
