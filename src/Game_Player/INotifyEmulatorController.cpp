/**
 * INotifyEmulatorController - A Class to define common elements for
 * emulators controlled by inotify events
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "EmulatorController.h"
#include "INotifyEmulatorController.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "DCE/Message.h"
#include "INotifyCodes.h"
#include "ImageUtils.h"

#include <cstdlib>
#include <string.h>

/**
 * The INotify Receive Thread - used to send data from
 * emulator to game player (e.g. when launch completes.)
 */
void * StartINotifyThread(void * Arg)
{
  DCE::INotifyEmulatorController *pINotifyEmulatorController = (DCE::INotifyEmulatorController *) Arg;
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
  pINotifyEmulatorController->INotifyRunloop();
  return NULL;
}

namespace DCE
{
  // ---------------------------------------------------------------------

  INotifyEmulatorController::INotifyEmulatorController(Game_Player *pGame_Player, INotifyEmulatorModel *pEmulatorModel)
    : EmulatorController(pGame_Player, pEmulatorModel),
      m_INotifyCommandMutex("inotify")
  {
    m_INotifyCommandMutex.Init(NULL);
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
  }

  INotifyEmulatorController::~INotifyEmulatorController()
  {
  
  }

  // ---------------------------------------------------------------------
  bool INotifyEmulatorController::init()
  {
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"INotifyEmulatorController::init() - Fetching Media Director screen resolution.");
    if (m_pGame_Player)
      {
	if (m_pGame_Player->m_pData)
	  {
 	    string::size_type pos=0;
	    int PK_MD = m_pGame_Player->m_pData->m_dwPK_Device_MD;
	    string sVideoSettings = m_pGame_Player->m_pData->m_pEvent_Impl->GetDeviceDataFromDatabase(PK_MD,DEVICEDATA_Video_settings_CONST); // 1920 1080/60
	    int iScreenWidth = atoi(StringUtils::Tokenize(sVideoSettings," ",pos).c_str());
	    int iScreenHeight = atoi(StringUtils::Tokenize(sVideoSettings,"/",pos).c_str());
	    int iRefreshRate = atoi(StringUtils::Tokenize(sVideoSettings,"/",pos).c_str()); // There shouldn't be a /, but this will get the rest of the string.
	    if (iScreenWidth!=0 && iScreenHeight!=0 && iRefreshRate!=0)
	      {
		m_pEmulatorModel->m_iScreenWidth=iScreenWidth;
		m_pEmulatorModel->m_iScreenHeight=iScreenHeight;
		m_pEmulatorModel->m_iRefreshRate=iRefreshRate;

		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media Director resolution detected as: %dx%d@%dHz",m_pEmulatorModel->m_iScreenWidth
						    ,m_pEmulatorModel->m_iScreenHeight
						    ,m_pEmulatorModel->m_iRefreshRate);

	      }
	  }
	else
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"INotifyEmulatorController::init() - Could not get m_pData from Game Player!");
	    return false;
	  }
      }
    else
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"INotifyEmulatorController::init() - Could not fetch Game Player parent. WTF?!");
	return false;
      }

    return EmulatorController::init();
  }

  bool INotifyEmulatorController::run()
  {
    if (pthread_create(&m_inotifyThread, NULL, StartINotifyThread, (void *) this))
      {
	// failed
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"INotifyEmulatorController::run() - Failed to create Game Player Receive iNotify thread.");
	return false;
      }
    else
      {
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"INotifyEmulatorController::run() - Inotify Thread started.");
      }
    
    return EmulatorController::run();
  }

  bool INotifyEmulatorController::stop()
  {
    doAction("STOP"); // Tell Emulator to stop.
    m_pEmulatorModel->m_bRunning=false;
    pthread_join(m_inotifyThread,NULL);
    return EmulatorController::stop();
  }
  // ---------------------------------------------------------------------
  
  void INotifyEmulatorController::doMediaSwap(string sMediaFilename, string sSlot)
  {
    char buf1[2];
    char buf2[1024];
    char buf[1024];
    FILE* fp;

    memset(buf1,0,sizeof(buf1));
    memset(buf2,0,sizeof(buf2));
    memset(buf,0,sizeof(buf));

    buf1[0] = 0xFE;
    buf1[1] = 0x00;

    strcpy(buf2,sMediaFilename.c_str());
    buf2[sMediaFilename.size()] = 0x00;
    strcat(buf,buf1);
    strcat(buf,buf2);

    PLUTO_SAFETY_LOCK(im,m_INotifyCommandMutex);
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"INotifyEmulatorController::doMediaSwap() - Sending media swap for %s into slot %s",buf,sSlot.c_str());
    fp = fopen("/run/Game_Player/send","wb");
    fwrite(buf,sizeof(char),strlen(buf)+1,fp);
    fclose(fp);
  }

  // ---------------------------------------------------------------------

  bool INotifyEmulatorController::doAction(string sAction)
  {
    if (!m_pEmulatorModel->m_mapActionstoCommandBytes_Exists(sAction))
      {
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"INotifyEmulatorController::doAction() - action %s does not exist.",sAction.c_str());
	return false;
      }

    pair<char, bool> command = m_pEmulatorModel->m_mapActionsToCommandBytes_Find(sAction);

    SendCommand(command.first);

    if (command.second)
      {
	Sleep(m_pEmulatorModel->m_iDoneDelay);
	if (!SendCommand(LMCE_INOTIFY_DONE))
	  return false;
      }
    else
      {
	return true;
      }

    return false;
  }

  // ---------------------------------------------------------------------

  bool INotifyEmulatorController::pressButton(int iPK_Button, Message *pMessage)
  {
    if (!m_pEmulatorModel->m_mapButtonsToActions_Exists(iPK_Button))
      {
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Could not find action mapping for requested PK_Button %d",iPK_Button);
	return false;
      }

    LoggerWrapper::GetInstance()->Write(LV_STATUS,"INotifyEmulatorController::pressButton(%d) - matched action: %s",iPK_Button,m_pEmulatorModel->m_mapButtonsToActions_Find(iPK_Button).c_str());

    return doAction(m_pEmulatorModel->m_mapButtonsToActions_Find(iPK_Button));
  }

  // ---------------------------------------------------------------------

  bool INotifyEmulatorController::pressClick(int iPositionX, int iPositionY, Message *pMessage)
  {
    return false;
  }

  // ---------------------------------------------------------------------

  bool INotifyEmulatorController::getSnap(long int iPK_Device, int iWidth, int iHeight, char **pData, int &iData_Size)
  {
    int iRetries=5;
    size_t size=0;
    doAction("GET_SNAPSHOT");

    while (iRetries>0)
      {
	if (FileUtils::FileExists("/run/Game_Player/snap.png"))
	  break;
	iRetries--;
	Sleep(50);
      }

    if (iRetries==0)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"INotifyEmulatorController::getSnap() - could not find /run/Game_Player/snap.png. Bailing!");
	return false;
      }

    ImageUtils::ResizeImage("/run/Game_Player/snap.png",iWidth,iHeight);

    Sleep(100);

    *pData = FileUtils::ReadFileIntoBuffer("/run/Game_Player/snap.png",size);
    iData_Size = size;

    if (size==0)
      return false;
      
    FileUtils::DelFile("/run/Game_Player/snap.png");

    return true;
  }

  // ---------------------------------------------------------------------

  bool INotifyEmulatorController::saveState(string &sPosition, string &sText, bool bAutoSave, string sAutoSavename)
  {
    int iRetries=5;
    string sBookmarkName;
    doAction("SAVE_STATE");

    while (iRetries>0)
      {
	if (FileUtils::FileExists("/run/Game_Player/state"))
	  break;
	iRetries--;
	Sleep(50);
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"INotifyEmulatorController::saveState() - Waiting on save state file... tries left: %d",iRetries);
      }

    if (iRetries==0)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"INotifyEmulatorController::saveState() - State file /run/Game_Player/state does not exist. Bailing!");
	sPosition="";
	sText="";
	return false;
      }

    if (!bAutoSave)
      {
	sBookmarkName = FileUtils::FileWithoutExtension(FileUtils::FilenameWithoutPath(getRomFromSlot()))+"-"+FileUtils::FileChecksum("/run/Game_Player/state");
      }
    else
      {
	sBookmarkName = FileUtils::FileWithoutExtension(FileUtils::FilenameWithoutPath(getRomFromSlot()))+"-"+"AutoSave";
      }

    string sCmd = "mv /run/Game_Player/state \""+m_pEmulatorModel->m_sStateDir+"/"+sBookmarkName + "\"";

    system(sCmd.c_str());

    // all is successsful, set sPosition and sText for return.
    sPosition = sBookmarkName;
    sText = "Saved bookmark on "+StringUtils::SQLDateTime(0)+" at "+StringUtils::HourMinute(0,false);

    return true;
  }

  // ---------------------------------------------------------------------

  void INotifyEmulatorController::pleaseResend()
  {
    // not used, but must be implemented.
  }

  // ---------------------------------------------------------------------

  bool INotifyEmulatorController::loadState(string sPosition)
 {    string sSource = m_pEmulatorModel->m_sStateDir+"/"+sPosition;
    FileUtils::DelFile("/run/Game_Player/state");
    
    if (sPosition.find("AutoSave") != string::npos)
      {
	sSource = m_pEmulatorModel->m_sStateDir+"/"+FileUtils::FileWithoutExtension(FileUtils::FilenameWithoutPath(getRomFromSlot()))+"-"+"AutoSave";
      }

    if (!FileUtils::FileExists(sSource))
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"INotifyEmulatorController::loadState() - Could not load state file %s, as it does not exist.",sSource.c_str());
	return false;
      }

    string sCmd = "cp \""+sSource+"\" /run/Game_Player/state";
    system(sCmd.c_str());

    if (!FileUtils::FileExists("/run/Game_Player/state"))
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"INotifyEmulatorController::loadState() - could not load copied state file /run/Game_Player/state , as it does not exist.");
	return false;
      }

    doAction("LOAD_STATE"); // load state

    return true;
  }

  // ---------------------------------------------------------------------

  void INotifyEmulatorController::INotifyRunloop()
  {
    inotify notify;
    int iRetries=20;

    int fd_notify = notify.watch("/run/Game_Player",IN_ALL_EVENTS);

    if (fd_notify < 0)
      return;

    LoggerWrapper::GetInstance()->Write(LV_STATUS,"INotifyEmulatorController::INotifyRunloop() - Starting INotify Runloop.");

    while (!m_pEmulatorModel->m_bRunning && iRetries > 0)
      {
	iRetries--;
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"INotifyEmulatorController::INotifyRunloop() - Emulator not running yet...waiting...");
	Sleep(100);
      }

    if (!m_pEmulatorModel->m_bRunning)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"INotifyEmulatorController::INotifyRunloop() - Could not contact emulator, gonna go byebye...");
	notify.unwatch(fd_notify);
	return;
      }

    while (m_pEmulatorModel->m_bRunning)
      {
	if (notify.pending_events())
	  {
	    ProcessINotifyEvent(notify.get_event());
	  }
	Sleep(50);
      }

    notify.unwatch(fd_notify);

    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"INotifyEmulatorController::INotifyRunloop() - Exiting runloop.");
    return;
  }

  // ---------------------------------------------------------------------

  void INotifyEmulatorController::ProcessINotifyEvent(cpp_inotify_event event)
  {
    char cBuf[1024];
    FILE *fp;

    if (event.name != "recv")
      return; // Not for us.

    if (!(event.mask & IN_CREATE))
      return;

    fp = fopen("/run/Game_Player/recv","rb");
    fread(&cBuf,1,1,fp);

    switch (cBuf[0])
      {
      case 0x00: // Heartbeat
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"INotifyEmulatorController::ProcessInotifyEvent() - Emulator is up.");
	m_pGame_Player->EVENT_Menu_Onscreen(m_pEmulatorModel->m_iStreamID,false);
	if (!m_pEmulatorModel->m_sMediaPosition.empty())
	  {
	    if (!loadState(m_pEmulatorModel->m_sMediaPosition))
	      {
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"INotifyEmulatorController::ProcessInotifyEvent() - Emulator up, position %s was specified, but was unable to load. Emulation will attempt to continue.",m_pEmulatorModel->m_sMediaPosition.c_str());
	      }
	    else
	      {
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"INotifyEmulatorController::ProcessInotifyEvent() - Emulator up, position %s was specified, and loaded successfully.",m_pEmulatorModel->m_sMediaPosition.c_str());
	      }
	  }
	break;
      }

    fclose(fp);

    unlink("/run/Game_Player/recv");
    
  }

  // ---------------------------------------------------------------------

  bool INotifyEmulatorController::SendCommand(unsigned char command)
  {
    PLUTO_SAFETY_LOCK(im,m_INotifyCommandMutex);
    unsigned char cBuf[2];
    FILE *fp;
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"INotifyEmulatorController::SendCommand() - Sending command 0x%0x",command);
    fp = fopen("/run/Game_Player/send","wb");
    cBuf[0]=command;
    fwrite(&cBuf,1,1,fp);
    fclose(fp);
    return true;
  }

  // ---------------------------------------------------------------------

  string INotifyEmulatorController::getRomFromSlot()
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
    return sMedia;
  }

  string INotifyEmulatorController::getRomPathFromSlot()
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

}
