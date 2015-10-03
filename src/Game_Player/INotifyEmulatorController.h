/**
 * INotifyEmulatorController - A Class to define common elements for
 * emulators controlled by inotify events
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef INOTIFYEMULATORCONTROLLER_H
#define INOTIFYEMULATORCONTROLLER_H

#include "EmulatorController.h"
#include "INotifyEmulatorModel.h"
#include "../inotify/inotify_class.h"

using namespace std;

namespace DCE
{
  class INotifyEmulatorController : public EmulatorController
  {
  public:
    INotifyEmulatorController(Game_Player *pGame_Player, INotifyEmulatorModel *pEmulatorModel);
    virtual ~INotifyEmulatorController();

    void inotifyThread();
    void INotifyRunloop();

    /* Base API Calls */
    virtual bool run();
    virtual bool stop();
    virtual bool init();

    virtual bool doAction(string sAction);
    virtual void pleaseResend();

    /* TODO: Move these out of here and up above when done. */
    /* Because these must be implemented, garrr.. */
    virtual bool pressButton(int iPK_Button, Message *pMessage);
    virtual bool pressClick(int iPositionX, int iPositionY, Message *pMessage);
    virtual bool getSnap(long int iPK_Device, int iWidth, int iHeight, char **pData, int &iData_Size);
    virtual bool saveState(string &sPosition, string &sText, bool bAutoSave=false, string sAutoSaveName="");
    virtual bool loadState(string sPosition);

  protected:
    INotifyEmulatorModel *m_pEmulatorModel;
    bool SendCommand(unsigned char command);
    virtual string getRomFromSlot();
    virtual string getRomPathFromSlot();

  private:
    pthread_t m_inotifyThread;
    pluto_pthread_mutex_t m_INotifyCommandMutex;
    void ProcessINotifyEvent(cpp_inotify_event event);
    void doCommand(unsigned char command);
  };
}

#endif
