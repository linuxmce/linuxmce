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

#ifndef SECONDGENEMULATORCONTROLLER_H
#define SECONDGENEMULATORCONTROLLER_H

#include "MAMEEmulatorController.h"
#include "SecondGenEmulatorModel.h"

using namespace std;

namespace DCE
{
  class SecondGenEmulatorController : public MAMEEmulatorController
  {
  public:
    SecondGenEmulatorController(Game_Player *pGame_Player, SecondGenEmulatorModel *pEmulatorModel); // ctor
    virtual ~SecondGenEmulatorController(); // dtor

    SecondGenEmulatorModel *m_pEmulatorModel;

    virtual bool pressButton(int iPK_Button, Message *pMessage);
    virtual void getOption(string sPath, string &sValue_To_Assign, Message *pMessage);
    virtual void setOption(string sPath, string sValue_To_Assign, Message *pMessage);

    virtual bool press0(Message *pMessage);
    virtual bool press1(Message *pMessage);
    virtual bool press2(Message *pMessage);
    virtual bool press3(Message *pMessage);
    virtual bool press4(Message *pMessage);
    virtual bool press5(Message *pMessage);
    virtual bool press6(Message *pMessage);
    virtual bool press7(Message *pMessage);
    virtual bool press8(Message *pMessage);
    virtual bool press9(Message *pMessage);

  };

}


#endif
