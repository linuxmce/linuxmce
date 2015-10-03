/**
 * EmulatorController - A base class to implement a controller
 * to control emulation engines.
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "EmulatorController.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"

namespace DCE
{

  EmulatorController::EmulatorController(Game_Player *pGame_Player, EmulatorModel *pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
  }

  EmulatorController::~EmulatorController()
  {
    stop();
    m_pGame_Player = NULL;	// DO NOT DELETE THE GAME PLAYER PARENT!
    delete m_pEmulatorModel;
    m_pEmulatorModel = NULL;
  }

  /**
   * Initialization routine called when the factory creates
   * the instance of this controller object. In the base class
   * it merely checks for the existance of an emulator
   * and then kills its process, if it does exist.
   */
  bool EmulatorController::init()
  {
    if (!m_pEmulatorModel->m_sProcessName.empty())
      {
	string sCmd = "killall -9 "+m_pEmulatorModel->m_sProcessName;
	  system(sCmd.c_str());
      }
    return true;
  }

  /**
   * Update the Disks device data, with any other matching media
   */
  void EmulatorController::updateDisks(string sMediaFile)
  {
    m_pEmulatorModel->m_dequeDisks.clear();
    m_pEmulatorModel->m_dequeDisks.push_back(Disk(sMediaFile,"default"));
    int i=0;
    string sDiskDDItems=StringUtils::itos(m_pEmulatorModel->m_iCurrentDisk) + "\n" + FileUtils::FilenameWithoutPath(sMediaFile) +"\n";

    for (i=1;i<=256;i++)
      {
	string sAdditionalDiskPath = sMediaFile + ".disk" + StringUtils::itos(i);
	if (FileUtils::FileExists(sAdditionalDiskPath))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_STATUS,"Adding additional disk: %s",sAdditionalDiskPath.c_str());
	    m_pEmulatorModel->m_dequeDisks.push_back(Disk(sAdditionalDiskPath,""));
	    sDiskDDItems += FileUtils::FilenameWithoutPath(sAdditionalDiskPath) + "\n";
	  }
      }
    m_pGame_Player->DATA_Set_Disks(sDiskDDItems);
  }

  /**
   * Insert media into the emulator into a given slot.
   * If not specified, the 'default' slot is used.
   */
  void EmulatorController::insertMediaNamed(string sMediaFile, string sSlot)
  {
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"Inserting media %s into slot %s",sMediaFile.c_str(),sSlot.c_str());
    m_pEmulatorModel->m_mapMedia[sSlot] = sMediaFile;
    updateDisks(sMediaFile);
  }

  /**
   * Eject media from specified slot. If no slot specified,
   * the default slot is used.
   */
  void EmulatorController::ejectMediaFromSlot(string sSlot)
  {
    map<string,string>::iterator it = m_pEmulatorModel->m_mapMedia.find(sSlot);
    if (it != m_pEmulatorModel->m_mapMedia.end())
      {
	m_pEmulatorModel->m_mapMedia.erase(it);
      }
  }

  /**
   * Eject all media from all slots. Called typically from CMD_Play_Media
   */
  void EmulatorController::ejectAllMedia()
  {
    m_pEmulatorModel->m_mapMedia.clear();
  }

  /**
   * return the media in the specified slot.
   */
  bool EmulatorController::getMediaInSlot(string& sMediaFile,string sSlot)
  {
    if (m_pEmulatorModel->m_mapMedia_Exists(sSlot))
      {
	sMediaFile=m_pEmulatorModel->m_mapMedia_Find(sSlot);
	return true;
      }
    else
      {
	sMediaFile="";
	return false;
      }
  }

  /**
   * Set whether streaming should be used.
   */
  void EmulatorController::setStreaming(bool bStreaming)
  {
    if (m_pEmulatorModel)
      {
	m_pEmulatorModel->m_bIsStreaming=bStreaming;
      }
  }

  /**
   * Set whether this is the master backend
   */
  void EmulatorController::setStreamingMaster(bool bStreamingMaster)
  {
    if (m_pEmulatorModel)
      m_pEmulatorModel->m_bIsStreamingSource = bStreamingMaster;
  }

  /**
   * Set stream ID used by this emulator
   */
  void EmulatorController::setStreamID(int iStreamID)
  {
    if (m_pEmulatorModel)
      m_pEmulatorModel->m_iStreamID=iStreamID;
    return;
  }

  /**
   * Set the media position for this emulator - Used when a CMD_Play_Media is issued
   * with a set positon for automatic loading of a save state.
   */
  void EmulatorController::setMediaPosition(string sMediaPosition)
  {
    if (m_pEmulatorModel && m_pEmulatorModel->m_bCanSaveState)
      m_pEmulatorModel->m_sMediaPosition=sMediaPosition;
    return;
  }

  /**
   * Set the emulator master hostname.
   */
  void EmulatorController::setHostName(string sHostName)
  {
    if (m_pEmulatorModel)
      m_pEmulatorModel->m_sHostName=sHostName;
  }

  /**
   * Basic sanity checks run before run() or stop().
   */
  bool EmulatorController::sanityCheck(string sFuncName)
  {
    if (!m_pEmulatorModel)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"EmulatorController::%s() - m_pEmulatorModel is NULL. Bailing.",sFuncName.c_str());
	return false;
      }
    
    if (!m_pGame_Player)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"EmulatorController::%s() - m_pGame_Player is NULL. Bailing.",sFuncName.c_str());
	return false;
      }
    return true; // sanity checks passed.
  }

  /** This just sets state to running. */
  bool EmulatorController::run()
  {
    if (!sanityCheck("run"))
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"EmulatorController::sanityCheck() - failed. Bailing.");
	return false;
      }
    if (m_pEmulatorModel->m_sEmulatorBinary.empty())
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"EmulatorController::run() - m_sEmulatorBinary is empty. Bailing.");
	return false;
      }

    if (!FileUtils::FileExists(m_pEmulatorModel->m_sEmulatorBinary))
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"EmulatorController::run() - Could not find emulator binary. %s Bailing.",m_pEmulatorModel->m_sEmulatorBinary.c_str());
	return false;
      }

    if (m_pEmulatorModel->m_bHasArgs && m_pEmulatorModel->m_sArgs.empty())
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"EmulatorController::run() - Emulator Specifies that args are required and arguments are empty. Bailing.");
	return false;
      }

    LoggerWrapper::GetInstance()->Write(LV_STATUS,"EmulatorController::run() - Launching %s with args %s",m_pEmulatorModel->m_sEmulatorBinary.c_str(),m_pEmulatorModel->m_sArgs.c_str());

    m_pEmulatorModel->coldReset();

    if (m_pEmulatorModel->m_bRunning && m_pEmulatorModel->m_bChangeRequiresRestart)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Emulator is running, and we need to restart.");
	EmulatorController::stop();
      }
    // Update the configuration file for the emulator ///////////////////////////////////////////////////

    m_pEmulatorModel->updateConfig();

    // Send the command back to the Game Player to launch the app ///////////////////////////////////////

    if (m_pEmulatorModel->m_bIsStreaming && !m_pEmulatorModel->m_bIsStreamingSource)
      {
	// incur a small delay so that the master may have time to properly
	// initialize.

	// TODO: replace with proper API call like isEmulatorReady()

	usleep(m_pEmulatorModel->m_tStreamingClientLaunchDelay);
      }

    m_pGame_Player->LaunchEmulator();

    LoggerWrapper::GetInstance()
      ->Write(LV_STATUS,"EmulatorController::run() - sent command back to game player to spawn application");

    ////////////////////////////////////////////////////////////////////////////////////////////////////

    // Any further processing should be taken care of by subclasses.

    m_pEmulatorModel->m_iExit_Code=-1;
    m_pEmulatorModel->m_bRunning_set(true);
    return true;
  }

  /** Just sets the state to not running. */
  bool EmulatorController::stop()
  {
    if (!sanityCheck("stop"))
      {
	LoggerWrapper::GetInstance()
	  ->Write(LV_CRITICAL,"EmulatorController::stop() - Sanity checks failed.");
      }
   
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"EmulatorController::stop() called.");

    /* if (!m_pEmulatorModel->emulatorHasCrashed() && m_pEmulatorModel->m_iActiveMenu > 0)
      {
	gotoMenu(0);
      } */

    /* if (m_pEmulatorModel->emulatorHasCrashed())
      gracefulExit(); // Attempt a Graceful exit. If it doesn't, KillEmulator() will try other means. */
                                          
    m_pGame_Player->KillEmulator();
    setMediaPosition(""); // clear it.

    m_pEmulatorModel->m_bRunning_set(false);
    // Clear all media from slots.
    // m_pEmulatorModel->m_mapMedia.clear();

    // any further deinitialization should be handled in the subclasses.
    m_pEmulatorModel->m_bQuitting_set(false);
    return true;
  }

  void EmulatorController::EmulatorHasExited(int iExit_Code)
  {
    m_pEmulatorModel->m_iExit_Code = iExit_Code;
    stop();
  }

  bool EmulatorController::P1Start()
  {
    return doAction("P1_START");
  }

  bool EmulatorController::P2Start()
  {
    return doAction("P2_START");
  }

  bool EmulatorController::P3Start()
  {
    return doAction("P3_START");
  }

  bool EmulatorController::P4Start()
  {
    return doAction("P4_START");
  }

  bool EmulatorController::coin1()
  {
    return doAction("COIN1");
  }

  bool EmulatorController::coin2()
  {
    return doAction("COIN2");
  }

  bool EmulatorController::pause()
  {
    string sAction;
    if (!m_pEmulatorModel->m_bIsPaused)
      {
	m_pEmulatorModel->m_bIsPaused=!m_pEmulatorModel->m_bIsPaused;
	sAction="PAUSE";
      }
    else
      {
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"EmulatorController::pause() - called while already paused.");
        sAction="NOP";
      }
    return doAction(sAction);
  }

  bool EmulatorController::unpause()
  {
    string sAction;
    if (m_pEmulatorModel->m_bIsPaused)
      {
	m_pEmulatorModel->m_bIsPaused=!m_pEmulatorModel->m_bIsPaused;
	sAction="UNPAUSE";
      }
    else
      {
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"EmulatorController::unpause() - called while already resumed.");
	sAction="NOP";
      }
    return doAction(sAction);
  }

  bool EmulatorController::setSpeed(int iSpeed)
  {
    // This is ultimately overridden by subclasses, but here, we just pass
    // the current speed to the model.
    m_pEmulatorModel->m_iSpeed=iSpeed;
    return doAction("SPEED_"+StringUtils::itos(iSpeed));
  }

  bool EmulatorController::uiUp()
  {
    return doAction("UI_UP");
  }

  bool EmulatorController::uiDown()
  {
    return doAction("UI_DOWN");
  }

  bool EmulatorController::uiLeft()
  {
    return doAction("UI_LEFT");
  }

  bool EmulatorController::uiRight()
  {
    return doAction("UI_RIGHT");
  }

  bool EmulatorController::uiOK()
  {
    return doAction("UI_OK");
  }

  bool EmulatorController::press0(Message *pMessage)
  {
    return doAction("0");
  }

  bool EmulatorController::press1(Message *pMessage)
  {
    return doAction("1");
  }

  bool EmulatorController::press2(Message *pMessage)
  {
    return doAction("2");
  }

  bool EmulatorController::press3(Message *pMessage)
  {
    return doAction("3");
  }

  bool EmulatorController::press4(Message *pMessage)
  {
    return doAction("4");
  }

  bool EmulatorController::press5(Message *pMessage)
  {
    return doAction("5");
  }

  bool EmulatorController::press6(Message *pMessage)
  {
    return doAction("6");
  }

  bool EmulatorController::press7(Message *pMessage)
  {
    return doAction("7");
  }

  bool EmulatorController::press8(Message *pMessage)
  {
    return doAction("8");
  }

  bool EmulatorController::press9(Message *pMessage)
  {
    return doAction("9");
  }

  bool EmulatorController::pressAsterisk()
  {
    return doAction("*");
  }

  bool EmulatorController::pressPound()
  {
    return doAction("#");
  }

  bool EmulatorController::uiBack()
  {
    return doAction("UI_BACK");
  }

  bool EmulatorController::emulatorExited()
  {
    m_pEmulatorModel->m_bRunning_set(false);
    return true;
  }

  bool EmulatorController::service1()
  {
    return doAction("SERVICE1");
  }
  
  bool EmulatorController::service2()
  {
    return doAction("SERVICE2");
  }

  bool EmulatorController::start()
  {
    return doAction("START");
  }
  
  bool EmulatorController::select()
  {
    return doAction("SELECT");
  }

  bool EmulatorController::option()
  {
    return doAction("OPTION");
  }

  bool EmulatorController::reset()
  {
    return doAction("RESET");
  }

  bool EmulatorController::gotoMenu(int iMenu)
  {
    m_pEmulatorModel->m_iActiveMenu = iMenu;
    return true;
  }

  bool EmulatorController::record()
  {
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"EmulatorController::record()");
    m_pEmulatorModel->m_bIsRecording=!m_pEmulatorModel->m_bIsRecording;
    return doAction("RECORD");
  }
  
  bool EmulatorController::gracefulExit()
  {
    return doAction("EXIT");
  }

  void EmulatorController::setOrbiter(long int dwPK_Device_Orbiter)
  {
    m_pEmulatorModel->m_dwPK_Device_Orbiter=dwPK_Device_Orbiter;
  }

  void EmulatorController::setSystemConfiguration(string sSystemConfiguration)
  {
    m_pEmulatorModel->m_sSystemConfiguration=sSystemConfiguration;
  }

  bool EmulatorController::canSaveState()
  {
    return m_pEmulatorModel->m_bCanSaveState;
  }

  bool EmulatorController::selectA()
  {
    return doAction("selectA");
  }
  
  bool EmulatorController::selectB()
  {
    return doAction("selectB");
  }

  void EmulatorController::getOption(string sPath, string &sValue_To_Assign, Message *pMessage)
  {
    // pMessage not used in base class
    sValue_To_Assign=m_pEmulatorModel->m_mapOptions_Find(sPath);
  }
  
  void EmulatorController::setOption(string sPath, string sValue_To_Assign, Message *pMessage)
  {
    // pMessage not used in base class
    m_pEmulatorModel->m_mapOptions[sPath]=sValue_To_Assign;
  }

  void EmulatorController::waitForEmulatorExit()
  {
    // Unimplemented here.
  }

}
