/**
 * UInput - a C++ object encapsulating the Linux uinput device
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef UINPUT_H
#define UINPUT_H

#include <linux/input.h>
#include <linux/uinput.h>

#include <string>

using namespace std;

namespace DCE
{
  class UInput
  {
  public:
    UInput(); 
    virtual ~UInput();

    // Initialization and done routines
    virtual bool Init();
    virtual bool Done();

    virtual void Initialized_set(bool bInitialized);
    virtual bool IsInitialized();

    virtual bool KeyUp(int iEvent);                    // send a key up event to the UInput device
    virtual bool KeyDown(int iEvent);                  // send a key down event to the UInput device
    virtual bool KeyPress(int iEvent);                 // Press a key down, then release up, to UInput device, after keypress delay.
    virtual void KeyPress_Delay_set(int iDelay);       // Set the wanted keypress delay
    virtual int  KeyPress_Delay_get();                 // Get the current keypress delay.

  protected:
    virtual void DeviceName_set(string sDeviceName);   // Set Device Name
    virtual string DeviceName_get();                   // Get Device Name
    virtual void SetDeviceCharacteristics();           // Called in init() to set uidev characteristics.
    virtual bool SetDeviceEventBits();                 // Called in init() to set uinput EV_BITs.
    virtual int  WriteDeviceCharacteristics();         // Write the device characteristics to uinput
    virtual int  CreateDevice();                       // Create the uinput device
   
  private:
    int fd;                                            // uinput file descriptor
    struct uinput_user_dev uidev;                      // uinput user device
    string m_sDeviceName;                              // Name of device to return via uinput.
    bool m_bInitialized;                               // FLAG: is UInput initialized?
    int  m_iKeypressDelay;                             // the keypress delay used in Keypress(int iEvent)
  };
}
#endif /* UINPUT_H */
