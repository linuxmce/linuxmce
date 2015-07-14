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

#include <cstdlib>

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
    : EmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
  }

  INotifyEmulatorController::~INotifyEmulatorController()
  {
  
  }

  // ---------------------------------------------------------------------
  bool INotifyEmulatorController::init()
  {
    return EmulatorController::init();
  }

  bool INotifyEmulatorController::run()
  {
    if (pthread_create(&m_inotifyThread, NULL, StartINotifyThread, (void *) this))
      {
	// failed
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Failed to create Game Player Receive iNotify thread.");
	return false;
      }
    else
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"XXXX Inotify Thread started.");
      }
    
    m_pEmulatorModel->m_sArgs = getRomFromSlot();

    // If a media position is specified, load it. 
    if (!m_pEmulatorModel->m_sMediaPosition.empty())
      {
	FileUtils::DelFile("/run/Game_Player/state");
	
	if (!loadState(m_pEmulatorModel->m_sMediaPosition))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_WARNING,"Could not load save state %s. Continuing",m_pEmulatorModel->m_sMediaPosition.c_str());
	  }
	
      }

    return EmulatorController::run();
  }

  bool INotifyEmulatorController::stop()
  {
    m_pEmulatorModel->m_bRunning=false;
    pthread_join(m_inotifyThread,NULL);
    return EmulatorController::stop();
  }
  // ---------------------------------------------------------------------

  bool INotifyEmulatorController::doAction(string sAction)
  {
    if (!m_pEmulatorModel->m_mapActionstoCommandBytes_Exists(sAction))
      return false;

    pair<char, bool> command = m_pEmulatorModel->m_mapActionsToCommandBytes_Find(sAction);

    SendCommand(command.first);

    Sleep(200);

    if (command.second)
      {
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
    return false;
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
    SendCommand(0x0C);

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
    SendCommand(0x20);

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

    string sCmd = "mv /run/Game_Player/state \"/home/mamedata/stella.sta/"+sBookmarkName + "\"";

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
  {
    string sSource = "/home/mamedata/stella.sta/"+sPosition;
    FileUtils::DelFile("/run/Game_Player/state");
    
    if (sPosition.find("AutoSave") != string::npos)
      {
	sSource = "/home/mamedata/stella.sta/"+FileUtils::FileWithoutExtension(FileUtils::FilenameWithoutPath(getRomFromSlot()))+"-"+"AutoSave";
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

    SendCommand(0x21); // load state

    return true;
  }



  // ---------------------------------------------------------------------

  void INotifyEmulatorController::INotifyRunloop()
  {
    inotify notify;

    int fd_notify = notify.watch("/run/Game_Player",IN_ALL_EVENTS);

    if (fd_notify < 0)
      return;

    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"XXXX Starting INotify Runloop.");

    while (!m_pEmulatorModel->m_bRunning)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"XXXX Emulator not running yet...waiting...");
	Sleep(100);
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

    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"XXXX Byebye.");
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
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"XXXX Emulator is up.");
	m_pGame_Player->EVENT_Menu_Onscreen(m_pEmulatorModel->m_iStreamID,false);
	break;
      }

    fclose(fp);

    unlink("/run/Game_Player/recv");
    
  }

  // ---------------------------------------------------------------------

  bool INotifyEmulatorController::SendCommand(char command)
  {
    char cBuf[2];
    FILE *fp;
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"INotifyEmulatorController::SendCommand() - Sending command 0x%0x",command);
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
  }\

}
