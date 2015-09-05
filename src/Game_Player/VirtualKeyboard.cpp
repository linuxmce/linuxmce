/**
 * VirtualKeyboard - a class that creates a uinput virtual keyboard
 * for emulators that can't handle synthetic X events. (e.g. MAME and MESS)
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 */

/** 
 * This class is a singleton, to be shared amongst any of the emulators that
 * need it. 
 */

#include "VirtualKeyboard.h"
#include <stddef.h>
#include "DCE/Logger.h"
#include <linux/input.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

namespace DCE
{
  /**
   * Global singleton pointer for the VirtualKeyboard.
   */
  VirtualKeyboard* VirtualKeyboard::m_pInstance = NULL;

  /**
   * Get the sole instance of this class allowed.
   */
  VirtualKeyboard* VirtualKeyboard::GetInstance()
  {
    if (!m_pInstance)
      m_pInstance = new VirtualKeyboard;

    return m_pInstance;
  }

  /**
   * ctor
   */
  VirtualKeyboard::VirtualKeyboard()
  {
    m_bOpened=false;
    m_iInputfd=-1;
    memset(&m_uidev,0,sizeof(m_uidev));
    snprintf(m_uidev.name,UINPUT_MAX_NAME_SIZE,"LinuxMCE Game Player Virtual Keyboard");
    m_uidev.id.bustype=BUS_USB;
    m_uidev.id.vendor=0x1234;
    m_uidev.id.product=0x0123;
    m_uidev.id.version=1;
  }

  bool VirtualKeyboard::Open()
  {
    int i=0;
    
    m_iInputfd = open("/dev/uinput",O_WRONLY|O_NONBLOCK);
    if (m_iInputfd<0)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VirtualKeyboard::Open() - Could not open /dev/uinput to register device. Bailing!");
	return false;
      }

    // Set the event bits.
    if (ioctl(m_iInputfd,UI_SET_EVBIT,EV_KEY)<0)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VirtualKeyboard::Open() - Could not set EV_KEY EVBIT. Bailing!");
	return false;
      }

    if (ioctl(m_iInputfd,UI_SET_EVBIT,EV_SYN)<0)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VirtualKeyboard::Open() - Could not set EV_SYN EVBIT. Bailing!");
	return false;
      }

    if (ioctl(m_iInputfd,UI_SET_EVBIT,EV_REP)<0)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VirtualKeyboard::Open() - Could not set EV_REP EVBIT. Bailing!");
	return false;
      }

    // Set the KEY bits.
    for (i=KEY_ESC;i<=KEY_MICMUTE;i++)
      {
	if (ioctl(m_iInputfd,UI_SET_KEYBIT,i)!=0)
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VirtualKeyboard::Open() - Could not set KEYBIT for Key ID %d. Bailing!",i);
	    return false;
	  }
      }

    // Finally, shove it all off with m_uidev to uinput, and create the device.
    if (write(m_iInputfd,&m_uidev,sizeof(m_uidev)) < 0)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VirtualKeyboard::Open() - Could not write capabilities to uinput. Bailing!");
	return false;
      }
    
    if (ioctl(m_iInputfd,UI_DEV_CREATE)<0)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VirtualKeyboard::Open() - Unable to tell uinput to do UI_DEV_CREATE. Bailing!");
      }
    
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VirtualKeyboard::Open() - Created and opened Virtual Keyboard.");
    m_bOpened=true;

    return true;
  }

}
