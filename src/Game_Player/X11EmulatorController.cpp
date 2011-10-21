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

#include <X11/keysym.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

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
    
    LoggerWrapper::GetInstance()
      ->Write(LV_STATUS,"X11EmulatorController::findWindow() thread - Window %s found as %x",m_pEmulatorModel->m_sWindowName.c_str(),m_pEmulatorModel->m_iWindowId);
  }

  bool X11EmulatorController::run()
  {
    EmulatorController::run(); // superclass, sets running flag.

    // grab display.
    m_pEmulatorModel->m_pDisplay = XOpenDisplay(getenv("DISPLAY"));
    if (!m_pEmulatorModel->m_pDisplay)
      {
	EmulatorController::stop();
	return false;
      }

    // at this point our emulator application is running.
    // now we need to listen for the window.
    // for this, we spawn a thread to watch for the new
    // window and set the window ID appropriately.
    
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
    EmulatorController::stop(); // superclass, unsets running flag.

    m_pEmulatorModel->m_iWindowId = 0; // reset Window ID as it is no longer valid.

    // Close display
    XCloseDisplay(m_pEmulatorModel->m_pDisplay);
    return true;
  }

  bool X11EmulatorController::doAction(string sAction) // from EmulatorController
  {

    int iKeysym, iKeysym_modifier;
    pair<int, int> pairKeysyms = m_pEmulatorModel->m_mapActionsToKeysyms_Find(sAction);

    if (!m_pEmulatorModel->m_pDisplay)
      {
	// we do not have a pointer to a Display. Bail.
	LoggerWrapper::GetInstance()
	  ->Write(LV_CRITICAL,"X11EmulatorController::doAction() - No X Display set while trying to do action %s. Please do a run() first, and make sure your DISPLAY environment variable is set properly, and that you have sufficient access rights to the display.");
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

    WindowUtils::SendKeyToWindow(m_pEmulatorModel->m_pDisplay,
				 m_pEmulatorModel->m_iWindowId,
				 iKeysym,
				 iKeysym_modifier);

    return true; // no way to tell if a key was sent successfully.
    
  }

  bool X11EmulatorController::pressButton(int iPK_Button)
  {
    // TODO: Come back here and implement this fucker.
    return true;
  }

  bool X11EmulatorController::pressClick(int iPositionX, int iPositionY, int iButtons)
  {
    // TODO: Come back here.
    return true;
  }

  bool X11EmulatorController::getSnap(char **pData, int iData_Size)
  {
    // TODO: come back here.
    return true;
  }

  bool X11EmulatorController::setSpeed(int iSpeed)
  {
    // todo: come back here.
    return true;
  }

  bool X11EmulatorController::saveState()
  {
    // not implemented
    return true;
  }

  bool X11EmulatorController::loadState()
  {
    // not implemented
    return true;
  }
  
  bool X11EmulatorController::gotoMenu(int iMenu)
  {
    // not implemented
    return true;
  }

}
