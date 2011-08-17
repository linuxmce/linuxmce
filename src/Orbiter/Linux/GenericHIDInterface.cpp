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
  m_pAlarmManager = new AlarmManager();
}

void GenericHIDInterface::AlarmCallback(int id, void* param)
{
  if (id==ALARM_CHECK_FOR_NEW_DEVICES) 
    {
            UpdateEventDevices();
    }
}

void GenericHIDInterface::LogFDs(vector<int> vectFDs)
{
  for (vector<int>::iterator it = vectFDs.begin(); it != vectFDs.end(); ++it)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"GenericHIDInterface::LogFDs() - %d is in FD vector.",*it);
    }
}

void GenericHIDInterface::UpdateEventDevices()
{
  // This is called to update the list of event devices to scan.
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"GenericHIDInterface::UpdateEventDevices() - Begin");

  if (m_vEventFDs.size() > 0) 
    {
      CloseEventDevices();
    }

  for (vector<string>::iterator it=m_pOrbiter->m_vectGyroDevices.begin(); it!=m_pOrbiter->m_vectGyroDevices.end(); ++it)
    {
      // iterator contains a device path.
      string filePath = *it;
      int fd = open(filePath.c_str(),O_RDONLY);
      if (!fd || fd < 0)
	{
	  // try once more, if it fails, do not add it.
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"GenericHIDInterface::UpdateEventDevices() - Could not open %s.. waiting a moment and retrying.",filePath.c_str());
	  Sleep(1000);
	  fd = open(filePath.c_str(),O_RDONLY);
	  if (!fd || fd < 0)
	    {
	      // Yup, ignore it.
	      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"GenericHIDInterface::UpdateEventDevices() - Could not open %s after trying a second time. Ignoring.",filePath.c_str());
	    }
	  else
	    {
	      // it opened after a second try.
	      LoggerWrapper::GetInstance()->Write(LV_STATUS,"GenericHIDInterface::UpdateEventDevices() - Successfully opened %s as fd %s after retry.",filePath.c_str(),fd);
	      m_vEventFDs.push_back(fd);
	      LogFDs(m_vEventFDs);
	    }
	} 
      else
	{
	  LoggerWrapper::GetInstance()->Write(LV_STATUS,"GenericHIDInterface::UpdateEventDevices() - Opened %s as fd %d",filePath.c_str(),fd);
	  m_vEventFDs.push_back(fd);
	  LogFDs(m_vEventFDs);
	}
    }

    m_pAlarmManager->CancelAlarmByType(ALARM_CHECK_FOR_NEW_DEVICES);		// cancel to provide sync
    m_pAlarmManager->AddRelativeAlarm(8,this,ALARM_CHECK_FOR_NEW_DEVICES,NULL); // schedule another device check in 8 secs.

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
  
  // empty out the vectors.
  while (!m_vEventFDs.empty())
    {
      m_vEventFDs.pop_back();
    }

  LoggerWrapper::GetInstance()->Write(LV_STATUS,"GenericHIDInterface::CloseEventDevices() End");

}

bool GenericHIDInterface::DecodeEventInFD(int fd)
{

  struct input_event ev[64];

  //  LoggerWrapper::GetInstance()->Write(LV_STATUS,"GenericHIDInterface::DecodeEventInFD() Begin");
  int rd;

  rd = read(fd, ev, sizeof(struct input_event) * 64);

  if ( rd < (int) sizeof(struct input_event))
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "GenericHIDInterface::DecodeEventInFD: Short read from fd %d read only %d bytes",fd,rd);
      // m_pAlarmManager->CancelAlarmByType(ALARM_CHECK_FOR_NEW_DEVICES);
      // m_pAlarmManager->AddRelativeAlarm(4,this,ALARM_CHECK_FOR_NEW_DEVICES,NULL);
      Sleep(4000);  // Stupid, i know, but it will keep things quiet for a moment.
    } else
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"GenericHIDInterface::DecodeEventInFD() Successful read of %d bytes from fd %d",rd,fd);
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
		  switch(iValue)
		    {
		    case 0:
		      pEvent->type=Orbiter::Event::BUTTON_UP;
		      break;
		    case 1:
		      pEvent->type=Orbiter::Event::BUTTON_DOWN;
		      break;
		    case 2:
		      pEvent->type=Orbiter::Event::BUTTON_DOWN;
		      break;
		    }
		  pEvent->data.button.m_bSimulated = false;
		  pEvent->data.button.m_iPK_Button = 0;
		  pEvent->data.button.m_iKeycode = iKeyCode + 120000;
		  LoggerWrapper::GetInstance()->Write(LV_STATUS,"GenericHIDInterface::DecodeEventInFD code %d value %d.",iKeyCode+120000,iValue);
		  m_pOrbiter->CallMaintenanceInMiliseconds(0, &Orbiter::QueueEventForProcessing, pEvent, pe_NO, false);
		}
	    }
	}
    }

  //  LoggerWrapper::GetInstance()->Write(LV_STATUS,"GenericHIDInterface::DecodeEventInFD() End");
  return true;
}

void GenericHIDInterface::ProcessGenericHIDEvents()
{
  // Get each and every event device data matching device.
  // Attempt to open them and put the ones successfully opened into a vector.
  // Loop through the vector, translating each EV_KEY press into a biased keycode # starting with 100000
  // and generating appropriate Orbiter::Events based on button up and button down input states.

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
	  	  LoggerWrapper::GetInstance()->Write(LV_STATUS,"GenericHIDInterface::ProcessGenericHIDEvents() - Adding fd %d to rfds",*it);
	  FD_SET(*it,&rfds);
	  iLastFD = *it;
	}

      int retval = select(iLastFD+1,&rfds,NULL,NULL,&tv);
            LoggerWrapper::GetInstance()->Write(LV_STATUS,"GenericHIDInterface::ProcessGenericHIDEvents() - retval is %d",retval);

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
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"GenericHIDInterface::ProcessGenericHIDEvents() we have data in FD %d",*it);
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
