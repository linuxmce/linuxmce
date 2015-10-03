/**
 * SecondGenEmulatorModel - Support for Second Gen Game consoles (1979-1984)
 * 
 * These consoles all share some common characteristics, but namely, each of them had
 * a repurposed numeric keypad attached to each joystick to allow for more control
 * options. The idea was eventually dispensed with, but three consoles supported have
 * this characteristic:
 *
 * Intellivision (1979)
 * ColecoVision (1982)
 * Atari 5200 (1982)
 *
 * This model/controller pair is to help provide the numeric keypad characteristics
 * and to more easily compartmentalise those changes.
 *
 */

#include "SecondGenEmulatorModel.h"
#include <X11/keysym.h>
#include "pluto_main/Define_Button.h"

namespace DCE
{
  SecondGenEmulatorModel::SecondGenEmulatorModel() : MAMEEmulatorModel()
  {
    m_iDoneDelay=200;
    initializeActionstoKeysyms();
    initializeButtontoKeysyms();
  }

  SecondGenEmulatorModel::~SecondGenEmulatorModel()
  {
    
  }

  void SecondGenEmulatorModel::initializeActionstoKeysyms()
  {
  }
  
  void SecondGenEmulatorModel::initializeButtontoKeysyms()
  {

    // MAMEEmulatorModel::initializeButtontoKeysyms();

  }


}
