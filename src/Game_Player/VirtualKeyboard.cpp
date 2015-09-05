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

  /**
   * dtor
   */
  VirtualKeyboard::~VirtualKeyboard()
  {
    Close();
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

  void VirtualKeyboard::Close()
  {
    if (m_bOpened && m_iInputfd > 0)
      {
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"VirtualKeyboard::Close() - Virtual Keyboard is open. Calling uinput UI_DEV_DESTROY ioctl.");
	if (ioctl(m_iInputfd,UI_DEV_DESTROY) < 0)
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VirtualKeyboard::Close() - Virtual Keyboard could not be destroyed.");
	  }
	close(m_iInputfd);
      }
    else
      {
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"VirtualKeyboard::Close() - Close called when not opened.");
	if (m_iInputfd > 0)
	  {
	    LoggerWrapper::GetInstance()->Write(LV_WARNING,"VirtualKeyboard::Close() - uinput fd was open. Closing it.");
	    close(m_iInputfd);
	  }
      }
    
    m_bOpened=false;

  }

  bool VirtualKeyboard::ClickKey(int iKey)
  {

    struct input_event ev;

    if (!m_bOpened)
      {
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"VirtualKeyboard::ClickKey(%d) - Virtual Keyboard not open. Opening it.");
	if (!Open())
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VirtualKeyboard::ClickKey(%d) - Could not open virtual keyboard.");
	    return false;
	  }
      }

    ev.type = EV_KEY;
    ev.code = iKey;
    ev.value = 1; // Key down.

    if (write(m_iInputfd,&ev,sizeof(ev)) < 0)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VirtualKeyboard::ClickKey(%d) - Could not write key down. Bailing!");
	return false;
      }

    ev.type = EV_SYN;
    ev.code = 0;
    ev.value = 0;

    if (write(m_iInputfd,&ev,sizeof(ev)) < 0)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VirtualKeyboard::ClickKey(%d) - Could not write SYN for key down. Bailing!");
	return false;
      }    

    usleep(1000);
    
    ev.type = EV_KEY;
    ev.code = iKey;
    // ev.value is already 0

    if (write(m_iInputfd,&ev,sizeof(ev)) < 0)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VirtualKeyboard::ClickKey(%d) - Could not write key up. Bailing!");
	return false;
      }

    ev.type = EV_SYN;
    ev.code = 0;
    ev.value = 0;

    if (write(m_iInputfd,&ev,sizeof(ev)) < 0)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VirtualKeyboard::ClickKey(%d) - Could not write SYN for key up. Bailing!");
	return false;
      }    

    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VirtualKeyboard::ClickKey(%d) - Wrote key press/depress successfully.");
    return true;

  }

}
