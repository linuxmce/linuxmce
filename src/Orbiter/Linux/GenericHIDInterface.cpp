#include "DCE/Logger.h"
#include "../Orbiter.h"
#include "GenericHIDInterface.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "../PlutoUtils/ProcessUtils.h" // may not need this, revisit later.

#include <stdint.h>
#include <linux/input.h>

#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#include <dirent.h>
#include <sys/types.h>

// Taken from evtest.c - Thanks Vojtech!

#define EVENT_PATH "/dev/input"

// The pthread entry point.
void *ProcessGenericHIDEvents(void *p)
{
  GenericHIDInterface *pGenericHIDInterface = (GenericHIDInterface *) p;
  pGenericHIDInterface->ProcessGenericHIDEvents();
  return NULL;
}

GenericHIDInterface::GenericHIDInterface(Orbiter *pOrbiter) : m_GenericHIDMutex("HID")
{
  m_pOrbiter = pOrbiter;
  m_bRunning = false;
  m_GenericHIDMutex.Init(&m_pOrbiter->m_MutexAttr);
  m_iHoldingDownButton=m_iRepeat=m_tLastButtonPress=m_iLastButtonPress=0;
}

void GenericHIDInterface::UpdateEventDevices()
{
  // This is called to update the list of event devices to scan.
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"GenericHIDInterface::UpdateEventDevices() - Begin");
  DIR * dir;
  struct dirent *dir_ent;

  dir = opendir(EVENT_PATH);
  dir_ent = NULL;
  string event_path = EVENT_PATH;

  if (m_vEventFDs.size() > 0) 
    {
      CloseEventDevices();
    }

  while (NULL != dir && (dir_ent = readdir(dir)))
    {
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"GenericHIDInterface::UpdateEventDevices() - traversing file %s",dir_ent->d_name);
      if ((dir_ent->d_name[0] != '.') && (strstr(dir_ent->d_name,"event") != NULL))
	{
	  string filePath = event_path + "/" + dir_ent->d_name;
	  int fd = open(filePath.c_str(),O_RDONLY|O_NONBLOCK);
	  if (!fd)
	    {
	      // don't add it
	    } else
	    {
	      LoggerWrapper::GetInstance()->Write(LV_STATUS,"GenericHIDInterface::UpdateEventDevices() - Opened %s as fd %d",filePath.c_str(),fd);
	      m_vEventFDs.push_back(fd);
	    }
	}
    }

  LoggerWrapper::GetInstance()->Write(LV_STATUS,"GenericHIDInterface::UpdateEventDevices() - End");
}

void GenericHIDInterface::CloseEventDevices()
{

  LoggerWrapper::GetInstance()->Write(LV_STATUS,"GenericHIDInterface::CloseEventDevices() Begin");

  for (vector<int>::iterator it = m_vEventFDs.begin(); it!=m_vEventFDs.end(); it++) 
    {
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"GenericHIDInterface::CloseEventDevices() Closing FD #%d",*it);
      close(*it);
    }

  LoggerWrapper::GetInstance()->Write(LV_STATUS,"GenericHIDInterface::CloseEventDevices() End");

}

bool GenericHIDInterface::DecodeEventInFD(int fd)
{

  struct input_event ev[64];

  LoggerWrapper::GetInstance()->Write(LV_STATUS,"GenericHIDInterface::DecodeEventInFD() Begin");
  int rd;

  rd = read(fd, ev, sizeof(struct input_event) * 64);

  if ( rd < (int) sizeof(struct input_event))
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "GenericHIDInterface::DecodeEventInFD: Short read from fd %d");
    } else 
    {
      for ( unsigned int i = 0; i < rd / sizeof(struct input_event); i++ )
	{
	  if ((ev[i].type == EV_MSC && (ev[i].code == MSC_RAW || ev[i].code == MSC_SCAN)) || ev[i].type == EV_SYN)
	    {
	      /* Do nothing */
	    }
	  else 
	    {
	      if ((ev[i].type == EV_KEY))
		{
		  /* oOo, a keypress, let's deal with it. */
		  int iKeyCode = ev[i].code;
		  int iValue = ev[i].value;
		  Orbiter::Event *pEvent = new Orbiter::Event;
		  pEvent->type=((iValue > 0) ? Orbiter::Event::BUTTON_DOWN : Orbiter::Event::BUTTON_UP);
		  pEvent->data.button.m_bSimulated = false;
		  pEvent->data.button.m_iPK_Button = 0;
		  pEvent->data.button.m_iKeycode = iKeyCode + 20000;
		  LoggerWrapper::GetInstance()->Write(LV_STATUS,"GenericHIDInterface::DecodeEventInFD code %d value %d.",iKeyCode+20000,iValue);
		  m_pOrbiter->CallMaintenanceInMiliseconds(0, &Orbiter::QueueEventForProcessing, pEvent, pe_NO, false);
		}
	    }
	}
    }
  
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"GenericHIDInterface::DecodeEventInFD() End");
  return true;
}

void GenericHIDInterface::ProcessGenericHIDEvents()
{
  // Find each and every event device in /dev/input/by-name. 
  // Attempt to open them and put the ones successfully opened into a vector.
  // Loop through the vector, translating each EV_KEY press into a biased keycode # starting with 100000
  // and generating appropriate Orbiter::Events based on button up and button down input states.

  // TEMPORARY: Build list of event devices when thread starts. Replace this with an
  // inotify callback.
  
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"GenericHIDInterface::ProcessGenericHIDEvents() - Begin");

  UpdateEventDevices();

  while (!m_pOrbiter->m_bQuit_get()) // This thread will run as long as Orbiter is running.
    {
      //      LoggerWrapper::GetInstance()->Write(LV_STATUS,"GenericHIDInterface::ProcessGenericHIDEvents() - Running Top");
      fd_set rfds;
      struct timeval tv;
      FD_ZERO(&rfds);
      
      /* Wait up to 5 seconds to get I/O from an event device. */
      tv.tv_sec  = 5;
      tv.tv_usec = 0;
      
      int iLastFD;

      for (vector<int>::iterator it = m_vEventFDs.begin(); it != m_vEventFDs.end(); it++)
	{
	  //	  LoggerWrapper::GetInstance()->Write(LV_STATUS,"GenericHIDInterface::ProcessGenericHIDEvents() - Adding fd %d to rfds",*it);
	  FD_SET(*it,&rfds);
	  iLastFD = *it;
	}

      int retval = select(iLastFD+1,&rfds,NULL,NULL,&tv);
      //      LoggerWrapper::GetInstance()->Write(LV_STATUS,"GenericHIDInterface::ProcessGenericHIDEvents() - retval is %d",retval);

      if (retval == -1)
	{
	  // error.
	}
      else if (retval)
	{
	  // Data is here. Let's grab and decode.
	  for (vector<int>::iterator it = m_vEventFDs.begin(); it != m_vEventFDs.end(); it++)
	    {
	      if (FD_ISSET(*it,&rfds))
	      {
		//		LoggerWrapper::GetInstance()->Write(LV_STATUS,"GenericHIDInterface::ProcessGenericHIDEvents() we have data in FD %d",*it);
		if (!DecodeEventInFD(*it))
		  {
		    LoggerWrapper::GetInstance()->Write(LV_WARNING,"GenericHIDInterface::DecodeEventInFD Unable to read from FD %d",*it);
		  }
	      }
	    }
	}
    
      //      LoggerWrapper::GetInstance()->Write(LV_STATUS,"GenericHIDInterface::ProcessGenericHIDEvents() - Bottom of Thread.");
  
    }

  LoggerWrapper::GetInstance()->Write(LV_STATUS,"GenericHIDInterface::ProcessGenericHIDEvents() - m_bQuit recieved. Closing Event Devices. ");
  // Close out the file descriptors.
  CloseEventDevices();

    LoggerWrapper::GetInstance()->Write(LV_STATUS,"GenericHIDInterface::ProcessGenericHIDEvents() - End");

}
