/**
 * X11EmulatorController - A class to define common elements for 
 * emulators controlled by synthesizing X11 events. 
 *
 * This is another abstract class. Do not instantiate.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "X11EmulatorController.h"
#include "WindowUtils/WindowUtils.h"
#include "pluto_main/Define_Button.h"
#include "pluto_main/Define_DeviceData.h"

#include <X11/keysym.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <math.h>
#include "utilities/linux/window_manager/WMController/WMController.h"
#include "utilities/linux/window_manager/wmctrl/wmctrl.h"

/**
 * The Window ID Thread. Called from X11EmulatorController::run().
 * expects a pointer back into X11EmulatorController.
 */
void * StartWindowIdThread(void * Arg)
{
  DCE::X11EmulatorController *pX11EmulatorController = (DCE::X11EmulatorController *) Arg;
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
  pX11EmulatorController->findWindow();
  return NULL;
}

namespace DCE
{
  X11EmulatorController::X11EmulatorController(Game_Player *pGame_Player, X11EmulatorModel *pEmulatorModel) 
    : EmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
    m_windowIdThread = 0;
    m_pAlarmManager=NULL;
    m_sLastAction = "NOP";
    m_bResend=false;
  }

  X11EmulatorController::~X11EmulatorController()
  {
  }

  /**
   * Set the m_bResend flag, so that we can try sending a keypress one more time, 
   * if it failed.
   */

  void X11EmulatorController::pleaseResend()
  {
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"XXXXXX PLEASERESEND!");
    pthread_join(m_windowIdThread,NULL);
//    PLUTO_SAFETY_LOCK (gm, m_pGame_Player->m_X11ControllerMutex);
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"Asking parent to please re-send the key.");
    m_bResend=true;
  }

  bool X11EmulatorController::getWindowCalled(string sWindowName,Window& windowId)
  {
    WmCtrl wmctrl;
    list<WinInfo> listWinInfo;
    wmctrl.ActionCommand('l',NULL,NULL,true,&listWinInfo);
    for (list<WinInfo>::iterator it = listWinInfo.begin(); it != listWinInfo.end();++it)
      {
	if (it->sClassName.find(sWindowName) != string::npos)
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Found Window! %x",static_cast<unsigned int>(it->ulWindowId));
	    windowId = it->ulWindowId;
	    return true;
	  }
      }
    return false;
  }

  /**
   * Thread for finding the window specified by m_sWindowName
   * and setting the Window Id.
   *
   * This thread is spawned in our run() and sleep loops until the 
   * window is found. Then exits.
   */
  void X11EmulatorController::findWindow()
  {
    PLUTO_SAFETY_LOCK (gm, m_pGame_Player->m_X11ControllerMutex);
    m_bWindowIdThreadIsRunning=true;

    int iRetry=0;

    Sleep(1000);

    // Try to fetch the window
    while ((iRetry<20) && (m_pEmulatorModel->m_iWindowId == 0))
      {
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Attempting to fetch window %s, Try #%d",m_pEmulatorModel->m_sWindowName.c_str(),iRetry);
	// LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"XXXXXXX Window ID IS %s",sWindowId.c_str());
	if (!getWindowCalled(m_pEmulatorModel->m_sWindowName,m_pEmulatorModel->m_iWindowId))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_STATUS,"X11EmulatorController::findWindow(%s) - could not find window yet, retrying.",m_pEmulatorModel->m_sWindowName.c_str());
	    usleep(100000);
	    ++iRetry;
	  }
	else
	  {
	    // Window has been acquired.
	    usleep(100000);
	    m_pGame_Player->EVENT_Menu_Onscreen(m_pEmulatorModel->m_iStreamID,false);
	    LoggerWrapper::GetInstance()->Write(LV_WARNING,"X11EmulatorController::findWindow(%s) - new window is %x",m_pEmulatorModel->m_sWindowName.c_str(),m_pEmulatorModel->m_iWindowId);
	    continue;
	  }
      }
    // thread exit.

    WMControllerImpl *pWMController = new WMControllerImpl();
    pWMController->SetMaximized(m_pEmulatorModel->m_sWindowName.c_str(),true);
    delete pWMController; 

    m_bWindowIdThreadIsRunning=false;
  }

  bool X11EmulatorController::init()
  {
    // Not much going on here. We set a global pointer to this class for our window handler
    // and then we call the base class.
    PLUTO_SAFETY_LOCK (gm, m_pGame_Player->m_X11ControllerMutex);
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
	      }
	  }
	else
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"X11EmulatorController::init - m_pData is NULL");
	  }
      }
    else
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"X11EmulatorController::init() - m_pGamePlayer is NULL, WTF?");
	return false;
      }

    return EmulatorController::init();
  }

  bool X11EmulatorController::run()
  {
    PLUTO_SAFETY_LOCK (gm, m_pGame_Player->m_X11ControllerMutex);
    EmulatorController::run(); // superclass, sets running flag.
    m_sLastAction="NOP";
    m_pEmulatorModel->m_iExit_Code=-1;
    m_bResendFired=false;
    m_pAlarmManager=new AlarmManager();
    m_pAlarmManager->Start(1);
    // grab display.
    m_pEmulatorModel->m_pDisplay = XOpenDisplay(getenv("DISPLAY"));
    if (!m_pEmulatorModel->m_pDisplay)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"X11EmulatorController::run() - Could not grab X11 display. Bailing!");
	EmulatorController::stop();
	return false;
      }

    if (m_pEmulatorModel->m_sWindowName.empty())
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"X11EmulatorController::run() - m_sWindowName NOT set, bailing!");
	return false;
      }

    // at this point our emulator application is running.
    // now we need to listen for the window.
    // for this, we spawn a thread to watch for the new
    // window and set the window ID appropriately.

    LoggerWrapper::GetInstance()->Write(LV_STATUS,"X11EmulatorController::run() - Spawning Window ID thread.");
    
    if (pthread_create(&m_windowIdThread, NULL, StartWindowIdThread, (void *) this))
      {
	// failed, bail.
	LoggerWrapper::GetInstance()
	  ->Write(LV_CRITICAL,"X11EmulatorController::run() - failed to create window id thread.");
	return false;
      }
   
    usleep(40000); 
    // m_pAlarmManager->AddRelativeAlarm(1,this,CHECK_RESEND,NULL);

    return true;

  }

  bool X11EmulatorController::stop()
  {
    pthread_join(m_windowIdThread,NULL); // Let's make sure that we close out the window ID thread cleanly, first.
    // PLUTO_SAFETY_LOCK (gm, m_pGame_Player->m_X11ControllerMutex);
    m_pEmulatorModel->m_iWindowId = 0; // reset Window ID as it is no longer valid.
 
    if (m_pEmulatorModel->m_iExit_Code > 0)
      {
	// There was an emulator crash. No need to do the rest of this, as it has already been fired.
	return EmulatorController::stop();
      }

    // Close display
    if (m_pEmulatorModel->m_pDisplay)
      { // Somehow this causes a crash. grrr..
	//XCloseDisplay(m_pEmulatorModel->m_pDisplay);
      }

    if (m_bResendFired == true)
      {
	m_pAlarmManager->CancelAlarmByType(CHECK_RESEND);
      }
    m_pAlarmManager->Stop();
    pthread_join(m_windowIdThread,NULL);
    delete m_pAlarmManager;
    m_pAlarmManager=NULL;
    return EmulatorController::stop(); // superclass, unsets running flag.

  }

  void X11EmulatorController::AlarmCallback(int id, void* param)
  {
    PLUTO_SAFETY_LOCK (gm, m_pGame_Player->m_X11ControllerMutex);
    m_bResendFired=true;
    checkResend();
  }

  void X11EmulatorController::checkResend()
  {
    if (m_bResend)
      {
	findWindow();
	doAction(m_sLastAction);
      }

    // m_pAlarmManager->AddRelativeAlarm(5, this, CHECK_RESEND, NULL);
  }

  void X11EmulatorController::EmulatorHasExited(int iExit_Code)
  {
    m_pEmulatorModel->m_iExit_Code = iExit_Code;
    stop();
  }

  bool X11EmulatorController::doAction(string sAction) // from EmulatorController
  {
    PLUTO_SAFETY_LOCK (gm, m_pGame_Player->m_X11ControllerMutex);
    m_bResend=false;

    // TODO: Do we rethink doAction as a virtual instead of a pure virtual?
    if (sAction=="NOP") // NOP = no operation for those who don't speak 6502 assembler :P
      {
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"X11EmulatorController::doAction(NOP)");
	m_sLastAction="NOP";
	return true;
      }

    int iKeysym, iKeysym_modifier;
    pair<int, int> pairKeysyms = m_pEmulatorModel->m_mapActionsToKeysyms_Find(sAction);

    if (!m_pEmulatorModel->m_pDisplay)
      {
	// we do not have a pointer to a Display. Bail.
	LoggerWrapper::GetInstance()
	  ->Write(LV_CRITICAL,"X11EmulatorController::doAction() - No X Display set while trying to do action %s. Please do a run() first, and make sure your DISPLAY environment variable is set properly, and that you have sufficient access rights to the display.",sAction.c_str());
	return false;
      }
    if (m_pEmulatorModel->m_iWindowId == 0)
      {
	// we do not have a window. Bail.
	LoggerWrapper::GetInstance()
	  ->Write(LV_CRITICAL,"X11EmulatorController::doAction() - No Window ID set while trying to do action %s. Please do a run() first.",sAction.c_str());
	return false;
      }

    if (pairKeysyms.first == 0 && pairKeysyms.second == 0)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"X11EmulatorController::doAction(%s) - Could not find matching keysym.",sAction.c_str());
	return false;
      }

    iKeysym          = pairKeysyms.first;
    iKeysym_modifier = pairKeysyms.second;

    if (m_pEmulatorModel->m_bRunning)
      {

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"X11EmulatorController::doAction(%s) - Sending %d to window %x",sAction.c_str(),iKeysym,m_pEmulatorModel->m_iWindowId);

	m_sLastAction=sAction; // For the resend handler.

	if (iKeysym_modifier != 0)
	  {
	    WindowUtils::SendKeyToWindow(m_pEmulatorModel->m_pDisplay,
					 m_pEmulatorModel->m_iWindowId,
					 iKeysym,
					 m_pEmulatorModel->m_iEventSerialNum++,
					 iKeysym_modifier);
	  }
	else
	  {
	    WindowUtils::SendKeyToWindow(m_pEmulatorModel->m_pDisplay,
					 m_pEmulatorModel->m_iWindowId,
					 iKeysym,
					 m_pEmulatorModel->m_iEventSerialNum++);      
	  }

	// At this point, if the window ID was invalid, our window handler would have caught it,
	// and attempted to re-acquire the target window (e.g. when a game is switched)
	// This attempts to see if a resend is required, and does so.

	return true; // no way to tell if a key was sent successfully.
      }
    else
      {
	return false; // The emulator was not running, so key was not sent.
      }
  }

  bool X11EmulatorController::pressButton(int iPK_Button, Message *pMessage)
  {
    PLUTO_SAFETY_LOCK (gm, m_pGame_Player->m_X11ControllerMutex);
    if (!m_pEmulatorModel->m_pDisplay)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"X11EmulatorModel::pressButton(%d) - No valid display pointer, bailing!",iPK_Button);
	return false;
      }

    if (!m_pEmulatorModel->m_iWindowId)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"X11EmulatorModel::pressButton(%d) - No valid window, bailing!",iPK_Button);
	return false;
      }

    switch (iPK_Button)
      {
      case BUTTON_left_shift_CONST:
	m_pEmulatorModel->m_iCurrentKeyModifier = XK_Shift_L;
	break;
      case BUTTON_right_shift_CONST:
	m_pEmulatorModel->m_iCurrentKeyModifier = XK_Shift_R;
	break;
      case BUTTON_Control_CONST:
	m_pEmulatorModel->m_iCurrentKeyModifier = XK_Control_L;
	break;
      case BUTTON_Shift_Left_CONST:
	m_pEmulatorModel->m_iCurrentKeyModifier = XK_Shift_L;
	break;
      case BUTTON_Shift_Right_CONST:
	m_pEmulatorModel->m_iCurrentKeyModifier = XK_Shift_R;
	break;
      default:
        LoggerWrapper::GetInstance()->Write(LV_STATUS,"Mapped to %d",m_pEmulatorModel->m_mapButtonToKeysyms_Find(iPK_Button));
	if (m_pEmulatorModel->m_mapButtonToKeysyms_Exists(iPK_Button) && m_pEmulatorModel->m_bRunning)
	  {
	    LoggerWrapper::GetInstance()->Write(LV_STATUS,"Sending key %d, modifier %d, to window %x",m_pEmulatorModel->m_mapButtonToKeysyms_Find(iPK_Button),
													m_pEmulatorModel->m_iCurrentKeyModifier,
													m_pEmulatorModel->m_iWindowId);
	    
	    XTestFakeKeyEvent( m_pEmulatorModel->m_pDisplay, XKeysymToKeycode( m_pEmulatorModel->m_pDisplay, m_pEmulatorModel->m_iCurrentKeyModifier),True, CurrentTime );
	    WindowUtils::SendKeyToWindow(m_pEmulatorModel->m_pDisplay,
					 m_pEmulatorModel->m_iWindowId,
					 m_pEmulatorModel->m_mapButtonToKeysyms_Find(iPK_Button),
					 m_pEmulatorModel->m_iCurrentKeyModifier);
	    XTestFakeKeyEvent( m_pEmulatorModel->m_pDisplay, XKeysymToKeycode( m_pEmulatorModel->m_pDisplay, m_pEmulatorModel->m_iCurrentKeyModifier),False, CurrentTime );

	    m_pEmulatorModel->m_iCurrentKeyModifier=0; // unpress modifier after use.
	  }
	else
	  {
	    return false; // Emulator was not running, do not send keysym.
	  }
      }
    return true;
  }

  bool X11EmulatorController::pressClick(int iPositionX, int iPositionY, Message *pMessage)
  {
    PLUTO_SAFETY_LOCK (gm, m_pGame_Player->m_X11ControllerMutex);
    if (!pMessage)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"X11EmulatorController::pressClick(%d,%d) - malformed Message sent. aborting. ");
	return false;
      }

    pair<int, int> currentGeometry = m_pEmulatorModel->getVideoFrameGeometry(pMessage->m_dwPK_Device_From);
    int iWidth = currentGeometry.first;
    int iHeight = currentGeometry.second;
    if (iWidth == -1 || iHeight == -1)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"X11EmulatorController::pressClick(%d,%d) - Could not fetch VideoFrameGeometry for device %d. Aborting.",iPositionX,iPositionY,pMessage->m_dwPK_Device_From);
	return false;
      }
    else
      {
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"X11EmulatorController::pressClick(%d,%d) - VideoFrameGeometry for orbiter %d is %dx%d",iPositionX,iPositionY,pMessage->m_dwPK_Device_From,iWidth,iHeight);
      }

    int iWindowX, iWindowY, iWindowW, iWindowH;
    if (WindowUtils::GetWindowGeometry(m_pEmulatorModel->m_pDisplay, m_pEmulatorModel->m_iWindowId, iWindowX, iWindowY, iWindowW, iWindowH))
      {
	double iScaleW, iScaleH = 0;
	int iScaledX, iScaledY = 0;
	iScaleW = (double)iWindowW / (double)iWidth;
	iScaleH = (double)iWindowH / (double)iHeight;
	iScaledX = floor(((double)iPositionX * iScaleW)) + floor(((double)iWindowX * iScaleW));
	iScaledY = floor(((double)iPositionY * iScaleH)) + floor(((double)iWindowY * iScaleH));
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"X11EmulatorController::pressClick(%d, %d) - iScaleW %d iScaleH %d iScaledX %d iScaledY %d",iPositionX,iPositionY,iScaleW,iScaleH,iScaledX,iScaledY);
	WindowUtils::SendClickToWindow(m_pEmulatorModel->m_pDisplay, m_pEmulatorModel->m_iWindowId, 1, iScaledX, iScaledY);
      }
    else
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"X11EmulatorController::pressClick(%d, %d) - Could not get window geometry to scale click to window 0x%x",iPositionX, iPositionY, m_pEmulatorModel->m_iWindowId);
	return false;
      }
    return true;
  }

  bool X11EmulatorController::getSnap(long int iPK_Device, int iWidth, int iHeight, char **pData, int& iData_Size)
  {
    PLUTO_SAFETY_LOCK (gm, m_pGame_Player->m_X11ControllerMutex);
    size_t size; // length of image in bytes.
    XSetInputFocus(m_pEmulatorModel->m_pDisplay, m_pEmulatorModel->m_iWindowId, RevertToNone, CurrentTime);
    string sGeometry = StringUtils::itos(iWidth) + "x" + StringUtils::itos(iHeight);
    m_pEmulatorModel->updateVideoFrameGeometry(iPK_Device,iWidth,iHeight);

    // Take shot with scrot, and move it over the original. 
    string cmd = 
      "scrot -u -t "+sGeometry+" /tmp/Game_Player_Get_Video_Frame_"+StringUtils::itos(iPK_Device)+".jpg";
    system(cmd.c_str());
    cmd = 
      "mv /tmp/Game_Player_Get_Video_Frame_"+StringUtils::itos(iPK_Device)+"-thumb.jpg /tmp/Game_Player_Get_Video_Frame_"+StringUtils::itos(iPK_Device)+".jpg";
    system(cmd.c_str());
    
    *pData = FileUtils::ReadFileIntoBuffer("/tmp/Game_Player_Get_Video_Frame_"+StringUtils::itos(iPK_Device)+".jpg",size);
    iData_Size = size;

    FileUtils::DelFile("/tmp/Game_Player_Get_Video_Frame_"+StringUtils::itos(iPK_Device)+".jpg");

    return true;
  }

  bool X11EmulatorController::setSpeed(int iSpeed)
  {
    // todo: come back here.
    return true;
  }

  bool X11EmulatorController::saveState(string& sPosition, string& sText, bool bAutoSave, string sAutoSaveName)
  {
    // not implemented, should never be called.
    return false;
  }

  bool X11EmulatorController::loadState(string sPosition)
  {
    // not implemented, should never be called.
    return false;
  }
  
  bool X11EmulatorController::gotoMenu(int iMenu)
  {
    // right now nothing, but bounce back to base class
    // to set current menu for tracking.
    return EmulatorController::gotoMenu(iMenu);
  }

  void X11EmulatorController::waitForEmulatorExit()
  {
    /* 
    PLUTO_SAFETY_LOCK (gm, m_pGame_Player->m_X11ControllerMutex);
    int iRetry=0;

    // Try to fetch the window
    while ((iRetry<20) && (m_pEmulatorModel->m_iWindowId != 0))
      {
	char *cWindowName=strdup(m_pEmulatorModel->m_sWindowName.c_str());
	
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Attempting to fetch window %s, Try #%d",cWindowName,iRetry);

	if (WindowUtils::FindWindowMatchingWMCLASS(m_pEmulatorModel->m_pDisplay,
						    cWindowName,
						    0,
						    m_pEmulatorModel->m_iWindowId))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_STATUS,"X11EmulatorController::waitForEmulatorExit(%s) - window still here. Retrying..",cWindowName);
	    usleep(100000);
	    ++iRetry;
	  }
	else
	  {
	    // Window is gone
	    usleep(100000);
	    LoggerWrapper::GetInstance()->Write(LV_WARNING,"X11EmulatorController::findWindow(%s) - window is gone.");
	    continue;
	  }
	free(cWindowName);
      }
    */
  }
}
