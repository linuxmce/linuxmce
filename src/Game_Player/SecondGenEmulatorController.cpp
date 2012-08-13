/**
 * SecondGenEmulatorController - Support for Second Gen Game consoles (1979-1984)
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
#include "SecondGenEmulatorController.h"

namespace DCE
{

  SecondGenEmulatorController::SecondGenEmulatorController(Game_Player *pGame_Player, SecondGenEmulatorModel *pEmulatorModel)
    : MESSEmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
  }

  SecondGenEmulatorController::~SecondGenEmulatorController()
  {
  
  }

}
