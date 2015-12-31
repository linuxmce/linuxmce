/*
     Copyright (C) 2012 LinucMCE
     www.linuxmce.org

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef EventMethod_h
#define EventMethod_h


#include "Advanced_IP_Camera.h"
//<-dceag-d-e->

#include <curl/curl.h>
#include <curl/easy.h>

#include <pthread.h>

namespace DCE {

	class Advanced_IP_Camera;

  class InputDevice {
       public:
	  InputDevice(int PK_Device, int FK_DeviceTemplate)
	  {
		  m_dwPK_Device = PK_Device;
		  m_dwFK_DeviceTemplate = FK_DeviceTemplate;
	  }

               int m_dwPK_Device;
	       int m_dwFK_DeviceTemplate;
               int status;
	       string m_sTriggerMethod;
               string m_sPatternOn;
               string m_sPatternOff;

               bool Matches(string s) {
                       return s.find(m_sPatternOn) != string::npos || s.find(m_sPatternOff) != string::npos;
               }

               int GetNewStatus(string s) {
                       if (s.find(m_sPatternOn) != string::npos) {
                               return 1;
                       } else if (s.find(m_sPatternOff) != string::npos) {
                               return 0;
                       }
                       return 0;
               }
	       string ToString() 
	       {
		       return StringUtils::Format("m_dwPK_Device = %d, m_dwFK_DeviceTemplate = %d, status = %s, m_sTriggerMethod = %s, m_sPatternOn = %s, m_sPatternOff = %s", m_dwPK_Device, m_dwFK_DeviceTemplate, status, m_sTriggerMethod.c_str(), m_sPatternOn.c_str(), m_sPatternOff.c_str());
	       }
  };

        /**
	 * Defines an method of accessing the event data of the camera device.
	 * This can be httpclient, httpserver, ftpserver, smtpserver
	 * It also holds a reference to the devices that are accessible using this method.
	 */
  class EventMethod {
  private:
	  Advanced_IP_Camera* m_pCamera;
	  bool m_bRunning;
	  vector<InputDevice*> m_vectInputDevices;

	  pthread_t m_eventThread;
	  string m_sEventBuffer;

  public:
	  string m_sMethod;
	  int m_iInterval;
	  string m_sURL;
	  EventMethod(Advanced_IP_Camera* pCamera);
	  ~EventMethod();

	  void Start();
	  void Stop();

	  void Add(InputDevice* pInputDevice)
	  {
		  m_vectInputDevices.push_back(pInputDevice);
	  }
	  static size_t StaticEventWriteCallback(void *ptr, size_t size, size_t nmemb, void *ourpointer);
	  size_t EventWriteCallback(void *ptr, size_t size, size_t nmemb);
	  void EventThread();
	  void MethodURL();
	  void MethodServer();
	  void HandleHttpCom(int socketfd);
	  void InputStatusChanged(InputDevice* pInputDevice, string trigger);

	  string ToString()
	  {
		  string s = "";
		  for( vector<InputDevice*>::const_iterator it = m_vectInputDevices.begin(); it != m_vectInputDevices.end(); ++it)
		  {
			  s += (*it)->ToString() + ", ";
		  }
		  return StringUtils::Format("m_bRunning = %d, m_sMethod = %s, m_sURL = %s, m_iInterval = %d, m_vectInputDevices = [ %s ]", m_bRunning, m_sMethod.c_str(), m_sURL.c_str(), m_iInterval, s.c_str());
	  }
  };

class CallbackData {
public:
	char* buffer;
	size_t size;

	CallbackData()
	{
		buffer = (char*)malloc(1);
		size = 0;
	}
	void Clear()
	{
		free(buffer);
	}
};

  
}
#endif
