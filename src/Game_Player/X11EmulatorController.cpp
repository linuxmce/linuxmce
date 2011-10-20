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

namespace DCE
{
  X11EmulatorController::X11EmulatorController(Game_Player *pGame_Player, X11EmulatorModel *pEmulatorModel) 
    : EmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
  }

  X11EmulatorController::~X11EmulatorController()
  {
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

    return true;

  }

  bool X11EmulatorController::stop()
  {
    EmulatorController::stop(); // superclass, unsets running flag.
    
    // Close display
    XCloseDisplay(m_pEmulatorModel->m_pDisplay);
    return true;
  }

  bool X11EmulatorController::doAction(string sAction) // from EmulatorController
  {

    int iKeysym, iKeysym_modifier;
    pair<int, int> pairKeysyms = m_pEmulatorModel->m_mapActionsToKeysyms_Find(sAction);

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
