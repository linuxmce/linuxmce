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

#include "UInput.h"

/**
 * ctor
 */
UInput::UInput()
{
  fd=0;
  DeviceName_set("Game Player Virtual Device");
  memset(&uidev, 0, sizeof(uidev));
}

/**
 * dtor
 */
UInput::~UInput()
{
  fd=0;
  DeviceName_set("");
  memset(&uidev, 0, sizeof(uidev));
}

/**
 * Init() - Open uinput device, initialize for use.
 */
bool UInput::Init()
{
  snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, DeviceName_get().c_str());
  SetDeviceCharacteristics();
  fd=open("/dev/input/uinput", O_WRONLY | O_NONBLOCK);

  if (fd<0)
    return false;

  SetDeviceEventBits();

  WriteDeviceCharacteristics();

  CreateDevice();

  return true;
}

/**
 * Done() - Close uinput device, sew things up.
 */
bool UInput::Done()
{
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
bool UInput::SetDeviceCharacteristics()
{
  uidev.id.bustype=BUS_USB;
  uidev.id.vendor=0x1234;
  uidev.id.product=0xfedc;
  uidev.id.version=1;
  return true;
}

/**
 * SetDeviceEventBits() - Set the virtual device event bits
 */
void UInput::SetDeviceEventBits()
{
  // By default, we are a keyboard. 
  ioctl(fd, UI_SET_EVBIT, EV_KEY);

  // Set all keys.
  for (int i=0; i<KEY_MAX; i++)
    {
      if (ioctl(fd, UI_SET_KEYBIT, i) < 0)
	{
	  return false;
	}
    }
}

/**
 * WriteDeviceCharacteristics() - Write Device Characteristics to uinput
 */
void UInput::WriteDeviceCharacteristics()
{
  write(fd, &uidev, sizeof(uidev));
}

/**
 * CreateDevice() - Create the virtual device.
 */
void UInput::CreateDevice()
{
  ioctl(fd, UI_DEV_CREATE);
}
