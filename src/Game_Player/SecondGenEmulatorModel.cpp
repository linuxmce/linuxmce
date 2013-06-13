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
  SecondGenEmulatorModel::SecondGenEmulatorModel() : MESSEmulatorModel()
  {
    initializeActionstoKeysyms();
    initializeButtontoKeysyms();
  }

  SecondGenEmulatorModel::~SecondGenEmulatorModel()
  {
    
  }

  void SecondGenEmulatorModel::initializeActionstoKeysyms()
  {
    MESSEmulatorModel::initializeActionstoKeysyms();
    m_mapActionsToKeysyms["P2_1"] = make_pair(XK_1,0);
    m_mapActionsToKeysyms["P2_2"] = make_pair(XK_2,0);
    m_mapActionsToKeysyms["P2_3"] = make_pair(XK_3,0);
    m_mapActionsToKeysyms["P2_4"] = make_pair(XK_q,0);
    m_mapActionsToKeysyms["P2_5"] = make_pair(XK_w,0);
    m_mapActionsToKeysyms["P2_6"] = make_pair(XK_e,0);
    m_mapActionsToKeysyms["P2_7"] = make_pair(XK_a,0);
    m_mapActionsToKeysyms["P2_8"] = make_pair(XK_s,0);
    m_mapActionsToKeysyms["P2_9"] = make_pair(XK_d,0);
    m_mapActionsToKeysyms["P2_0"] = make_pair(XK_x,0);
  }
  
  void SecondGenEmulatorModel::initializeButtontoKeysyms()
  {

    X11EmulatorModel::initializeButtontoKeysyms();

    // P1 CLR and Enter
    m_mapButtonToKeysyms[BUTTON_Asterisk_CONST] = XK_KP_Delete;
    m_mapButtonToKeysyms[BUTTON_Pound_CONST] = XK_KP_Enter;

    // P2 CLR and Enter
    int P2Asterisk = SECONDGEN_P2+BUTTON_Asterisk_CONST;
    int P2Pound = SECONDGEN_P2+BUTTON_Pound_CONST;
    m_mapButtonToKeysyms[P2Asterisk] = XK_Z;
    m_mapButtonToKeysyms[P2Pound] = XK_C;

  }


}
