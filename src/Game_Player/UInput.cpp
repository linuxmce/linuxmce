/*
     Copyright (C) 2017 LinuxMCE

     www.linuxmce.org


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/**
 * UInput - a C++ object encapsulating the Linux uinput device
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#define DEFAULT_KEYPRESS_DELAY 100     // default keypress delay

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "UInput.h"

using namespace DCE;

/**
 * ctor
 */
UInput::UInput()
{
  fd=0;
  DeviceName_set("Game Player Virtual Device");
  Initialized_set(false);
  KeyPress_Delay_set(DEFAULT_KEYPRESS_DELAY);
  memset(&uidev, 0, sizeof(uidev));
}

/**
 * dtor
 */
UInput::~UInput()
{
  fd=0;
  Initialized_set(false);
  KeyPress_Delay_set(0);
  DeviceName_set("");
  memset(&uidev, 0, sizeof(uidev));
}

/**
 * Init() - Open uinput device, initialize for use.
 */
bool UInput::Init()
{
  if (IsInitialized())
    return false;;

  snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "%s", DeviceName_get().c_str());
  SetDeviceCharacteristics();
  fd=open("/dev/input/uinput", O_WRONLY | O_NONBLOCK);

  if (fd<0)
    return false;

  SetDeviceEventBits();
  WriteDeviceCharacteristics();
  CreateDevice();
  Initialized_set(true);
  return true;
}

/**
 * Done() - Close uinput device, sew things up.
 */
bool UInput::Done()
{
  if (!IsInitialized())
    return false;

  if (ioctl(fd, UI_DEV_DESTROY) < 0)
    return false;

  return true;
}

/** 
 * DeviceName_set() - Set the virtual device name to report to kernel
 */
void UInput::DeviceName_set(string sDeviceName)
{
  m_sDeviceName=sDeviceName;
}

/**
 * DeviceName_get() - Get the virtual device name
 */
string UInput::DeviceName_get()
{
  return m_sDeviceName;
}

/**
 * SetDeviceCharacteristics() - Called to populate uidev struct
 * with the apporpriate virtual device characteristics.
 */
void UInput::SetDeviceCharacteristics()
{
  uidev.id.bustype=BUS_USB;
  uidev.id.vendor=0x1234;
  uidev.id.product=0xfedc;
  uidev.id.version=1;
}

/**
 * SetDeviceEventBits() - Set the virtual device event bits
 */
bool UInput::SetDeviceEventBits()
{
  // By default, we are a keyboard. 
  if (ioctl(fd, UI_SET_EVBIT, EV_KEY) < 0)
    {
      return false;
    }

  // We want to be able to emit any key event.
  for (int i=0; i<KEY_MAX; i++)
    {
      if (ioctl(fd, UI_SET_KEYBIT, i) < 0)
	{
	  return false;
	}
    }
  return true;
}

/**
 * WriteDeviceCharacteristics() - Write Device Characteristics to uinput
 */
int UInput::WriteDeviceCharacteristics()
{
  return write(fd, &uidev, sizeof(uidev));
}

/**
 * CreateDevice() - Create the virtual device.
 */
int UInput::CreateDevice()
{
  return ioctl(fd, UI_DEV_CREATE);
}

/**
 * Initialized_set() - Set initialized flag
 */
void UInput::Initialized_set(bool bInitialized)
{
  m_bInitialized=true;
}

/**
 * IsInitialized() - Get Initialized flag
 */
bool UInput::IsInitialized()
{
  return m_bInitialized;
}

/**
 * KeyDown(int iEvent) - Press key down
*/
bool UInput::KeyDown(int iEvent)
{
  if (!IsInitialized())
    return false;

  struct input_event ev[2];
  memset(&ev,0,sizeof(ev));
  ev[0].type = EV_KEY;
  ev[0].code = iEvent;
  ev[0].value=1;
  ev[1].type = EV_SYN;
  ev[1].code = SYN_REPORT;
  ev[1].value=0;

  if (write(fd,ev,sizeof(ev)) < 2)
    return false;

  return true;
}

/**
 * KeyUp(int iEvent) - Press key up
 */
bool UInput::KeyUp(int iEvent)
{
  if (!IsInitialized())
    return false;

  struct input_event ev[2];
  memset(&ev,0,sizeof(ev));
  ev[0].type = EV_KEY;
  ev[0].code = iEvent;
  ev[0].value=0;
  ev[1].type = EV_SYN;
  ev[1].code = SYN_REPORT;
  ev[1].value=0;

  if (write(fd,ev,sizeof(ev)) < 2)
    return false;

  return true;
}

/**
 * KeyPress(int iEvent) - Press the requested key down, delay by the Keypress delay, then Press the requested key up.
 */
bool UInput::KeyPress(int iEvent)
{
  if (!KeyDown(iEvent))
    {
      // This failed, but we will try a key up, anyway.
    }

  usleep(KeyPress_Delay_get()*1000);

  if (!KeyUp(iEvent))
    {
      // This failed, we'll return false.
      return false;
    }

  return true;

}

/** 
 * KeyPress_Delay_set(int iDelay) - Set the wanted Keypress delay
 */
void UInput::KeyPress_Delay_set(int iDelay)
{
  m_iKeypressDelay=iDelay;
}

/**
 * KeyPress_Delay_get() - Get the wanted Keypress delay.
 */
int UInput::KeyPress_Delay_get()
{
  return m_iKeypressDelay;
}
