/**
 * Atari800EmulatorController - Support to control the Atari800 emulator.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "Atari800EmulatorController.h"
#include "PlutoUtils/FileUtils.h"

namespace DCE
{
  Atari800EmulatorController::Atari800EmulatorController(Game_Player *pGame_Player, Atari800EmulatorModel *pEmulatorModel)
    : X11EmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
    m_pEmulatorModel->m_sEmulatorBinary="/usr/bin/atari800";
    m_pEmulatorModel->m_sProcessName="atari800";
    m_pEmulatorModel->m_sConfigFileTemplate="/usr/pluto/templates/mame.ini.template";
    m_pEmulatorModel->m_sConfigFile="/root/.mame/mame.ini";
    m_pEmulatorModel->m_sWindowName="atari800.atari800"; // wmclass.wmname of window.
    m_pEmulatorModel->m_bChangeRequiresRestart=true;
    m_pEmulatorModel->m_bRunning=false;
    m_pEmulatorModel->m_bHasArgs=true;
    m_pEmulatorModel->m_sArgs="";
  }

  Atari800EmulatorController::~Atari800EmulatorController()
  {
   
  }

  bool Atari800EmulatorController::init()
  {
    return X11EmulatorController::init();
  }

  string Atari800EmulatorController::getRomFromSlot()
  {
    string sMedia;
    if (getMediaInSlot(sMedia))
      {
	if (!FileUtils::FileExists(sMedia))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Atari800EmulatorController::run() - could not find media in default slot: %s",sMedia.c_str());
	    return "";
	  }
      }
    else
      {
	return "";
      }
    return sMedia;
  }

  string Atari800EmulatorController::getRomPathFromSlot()
  {
    string sMedia;
    if (!getMediaInSlot(sMedia))
      {
	return "";
      }
    else
      {
	return sMedia;
      }
  }

  bool Atari800EmulatorController::run()
  {
    
    if (!m_pEmulatorModel)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Atari800EmulatorController::run() - pEmulatorModel is null. Bailing.");
	return false;
      }

    m_pEmulatorModel->m_sArgs = "-dsprate\t48000\t" + getRomFromSlot();

    // If a media position was set (i.e. as part of the CMD_Play_Media, then put it in
    // the mame configuration file to load. Otherwise, blank it out.

    if (X11EmulatorController::run())
      return true;

    return false;

  }

  bool Atari800EmulatorController::setSpeed(int iSpeed)
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
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Atari800EmulatorController:setSpeed(%d) not implemented. Ignoring.");
	break;
      }
    return EmulatorController::setSpeed(iSpeed);
  }

  bool Atari800EmulatorController::gotoMenu(int iMenu)
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

  bool Atari800EmulatorController::saveState(string& sPosition, string& sText, bool bAutoSave, string sAutoSaveName)
  {

  }

  void Atari800EmulatorController::setMediaPosition(string sMediaPosition)
  {
    EmulatorController::setMediaPosition(sMediaPosition);
    return;
  }
  
  bool Atari800EmulatorController::loadState(string sPosition)
  {
    
  }
  
  /** 
   * given a path, iterate through files named 0000.avi, etc. until one does not
   * exist. Then return that path name. 
   * This was done to support multiple recordings being done of the same game.
   */
  bool Atari800EmulatorController::getRecordingFilename(string sPath, string &sFilename)
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
	    // found one, set and break. This is what Atari800 will name the file. 
	    sFilename = sCurrentIndex;
	    return true;
	  }
      }
    return false; // Somehow we have 100 active game recordings for this game. this is probably bad.
  }

  bool Atari800EmulatorController::record()
  {
    string sPath = "/home/mamedata/shots/"+getRomFromSlot();
    if (!m_pEmulatorModel->m_bIsRecording)
      {
	// Not recording. Start a recording.
	// Our patched Atari800 records an AVI by default.
	if (!getRecordingFilename(sPath,m_pEmulatorModel->m_sRecordingFilename))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Atari800yEmulatorController::record() - Could not get a filename for path %s. Bailing on record!",sPath.c_str());
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
