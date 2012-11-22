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

#include <X11/keysym.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

/**
 * This is a castrated window handler so that the program will keep running.
 * This is okay, because any failed X requests will be retried until timeout.
 */
int _defaultWindowHandler(Display *disp, XErrorEvent *err)
{
  DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS,"_defaultWindowHandler - we got called.");
  return 0;
}

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
  }

  X11EmulatorController::~X11EmulatorController()
  {
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
    // If we're being called, we need to make double sure that we find the new window,
    // so zero out the old one.

    XSetErrorHandler(_defaultWindowHandler);

    bool bRunLoop=true;
    int iRetry=0; // number of times to retry despite a duplicate window returned.
    while (bRunLoop==true)
      {
	if (m_pEmulatorModel->m_iWindowId != 0)
	  {
	    LoggerWrapper::GetInstance()->Write(LV_STATUS,"Previous Window Id was %x setting to 0 to reacquire new window.",m_pEmulatorModel->m_iWindowId);
	    m_pEmulatorModel->m_iPreviousWindowId = m_pEmulatorModel->m_iWindowId;
	    m_pEmulatorModel->m_iWindowId=0;
	  }
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Finding a window named %s",m_pEmulatorModel->m_sWindowName.c_str());
	while (m_pEmulatorModel->m_iWindowId == 0)
	  {
	    char *cWindowName = strdup(m_pEmulatorModel->m_sWindowName.c_str());
	    if (!WindowUtils::FindWindowMatchingWMCLASS(m_pEmulatorModel->m_pDisplay,
							cWindowName,
							0,
							m_pEmulatorModel->m_iWindowId))
	      {
		// Wait just a bit.
		LoggerWrapper::GetInstance()
		  ->Write(LV_STATUS,"X11EmulatorController::findWindow() thread - Can't find window %s yet. Trying again.",m_pEmulatorModel->m_sWindowName.c_str());
		usleep(100000);
	      }
	    free(cWindowName);
	  }

	if (m_pEmulatorModel->m_iWindowId == m_pEmulatorModel->m_iPreviousWindowId)
	  {
	    LoggerWrapper::GetInstance()->Write(LV_STATUS,"Got Same Window ID %x as last time. Waiting a bit, and trying again!",m_pEmulatorModel->m_iWindowId);
	    if (iRetry>10)
	      {
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Apparently got the same Window # again, ok. Let's use it.");
		bRunLoop=false;
	      }
	    else
	      {
		iRetry++;
	      }
	    usleep(100000);
	  }
	else
	  {
	    // We got a new window, let the loop break;
	    LoggerWrapper::GetInstance()
	      ->Write(LV_STATUS,"X11EmulatorController::findWindow() thread - Window %s found as %x",m_pEmulatorModel->m_sWindowName.c_str(),m_pEmulatorModel->m_iWindowId);
	    bRunLoop=false;
	  }
      }
    // magic to make emulator windows always get swallowed by orbiter, but only
    // once the window is present and acquired.
    m_pGame_Player->EVENT_Menu_Onscreen(m_pEmulatorModel->m_iStreamID,false);
    XSetErrorHandler(NULL);  // put everything back.
  }

  bool X11EmulatorController::init()
  {
    // Not much going on here.
    return EmulatorController::init();
  }

  bool X11EmulatorController::run()
  {
    EmulatorController::run(); // superclass, sets running flag.

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
    
    return true;

  }

  bool X11EmulatorController::stop()
  {

    m_pEmulatorModel->m_iWindowId = 0; // reset Window ID as it is no longer valid.

    // Close display
    if (m_pEmulatorModel->m_pDisplay)
      {
	XCloseDisplay(m_pEmulatorModel->m_pDisplay);
      }

    return EmulatorController::stop(); // superclass, unsets running flag.

  }

  void X11EmulatorController::EmulatorHasExited(int iExit_Code)
  {
    m_pEmulatorModel->m_iExit_Code = iExit_Code;
    stop();
  }

  bool X11EmulatorController::doAction(string sAction) // from EmulatorController
  {

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

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"X11EmulatorController::doAction(%s) - Sending %d to window %x",sAction.c_str(),iKeysym,m_pEmulatorModel->m_iWindowId);

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
	return true; // no way to tell if a key was sent successfully.
      }
    else
      {
	return false; // The emulator was not running, so key was not sent.
      }
  }

  bool X11EmulatorController::pressButton(int iPK_Button)
  {
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
	if (m_pEmulatorModel->m_mapButtonToKeysyms_Exists(iPK_Button) && m_pEmulatorModel->m_bRunning)
	  {
	    WindowUtils::SendKeyToWindow(m_pEmulatorModel->m_pDisplay,
					 m_pEmulatorModel->m_iWindowId,
					 m_pEmulatorModel->m_mapButtonToKeysyms_Find(iPK_Button),
					 m_pEmulatorModel->m_iCurrentKeyModifier);
	    m_pEmulatorModel->m_iCurrentKeyModifier=0; // unpress modifier after use.
	  }
	else
	  {
	    return false; // Emulator was not running, do not send keysym.
	  }
      }
    return true;
  }

  bool X11EmulatorController::pressClick(int iPositionX, int iPositionY, int iButtons)
  {
    // TODO: Come back here.
    return true;
  }

  bool X11EmulatorController::getSnap(long int iPK_Device, int iWidth, int iHeight, char **pData, int& iData_Size)
  {
    size_t size; // length of image in bytes.
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

}
