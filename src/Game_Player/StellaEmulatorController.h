/**
 * StellaEmulatorController - Support to control the Stella emulator.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef StellaEMULATORCONTROLLER_H
#define StellaEMULATORCONTROLLER_H

#include "INotifyEmulatorController.h"
#include "StellaEmulatorModel.h"

using namespace std;

namespace DCE
{
  class StellaEmulatorController : public INotifyEmulatorController
  {
  public:
    StellaEmulatorController(Game_Player *pGame_Player, StellaEmulatorModel *pEmulatorModel); // ctor
    virtual ~StellaEmulatorController(); // dtor

    StellaEmulatorModel *m_pEmulatorModel;
    virtual bool init();
    virtual bool run();

    virtual bool pause();
    virtual bool unpause();
    virtual bool setSpeed(int iSpeed);
    virtual bool pressButton(int iPK_Button, Message *pMessage);

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
    
  protected:
  private:
    string getSaveStateFromSlot();
    virtual string getRomFromSlot();
    bool doKeypadAction(string sKey, Message *pMessage);
  };
}

#endif
