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

  protected:
    virtual void DeviceName_set(string sDeviceName);   // Set Device Name
    virtual string DeviceName_get();                   // Get Device Name
    virtual bool SetDeviceCharacteristics();           // Called in init() to set uidev characteristics.
    virtual bool SetDeviceEventBits();                 // Called in init() to set uinput EV_BITs.
    virtual bool WriteDeviceCharacteristics();         // Write the device characteristics to uinput
    virtual bool CreateDevice();                       // Create the uinput device

  private:
    int fd;                                            // uinput file descriptor
    struct uinput_user_dev uidev;                      // uinput user device
    string m_sDeviceName;                              // Name of device to return via uinput.
  };
}
#endif UINPUT_H
