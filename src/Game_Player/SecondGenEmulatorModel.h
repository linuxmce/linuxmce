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

#ifndef SECONDGENEMULATORMODEL_H
#define SECONDGENEMULATORMODEL_H

#define SECONDGEN_P1 0
#define SECONDGEN_P2 1024 // Second generation console button offset when P2 selected.

#include "MAMEEmulatorModel.h"
#include <map>

namespace DCE
{
  class SecondGenEmulatorModel : public MAMEEmulatorModel
  {

  protected:

  public:
    SecondGenEmulatorModel();
    virtual ~SecondGenEmulatorModel();

    map<long, int> m_mapOrbiterToKeypadPlayer;
    int m_mapOrbiterToKeypadPlayer_Find(int iPK_Orbiter)
    {
      map<long, int>::iterator it = m_mapOrbiterToKeypadPlayer.find(iPK_Orbiter);
      return it == m_mapOrbiterToKeypadPlayer.end() ? 1 : it->second;
    }

    virtual void initializeActionstoKeysyms();
    virtual void initializeButtontoKeysyms();

  };
}

#endif
