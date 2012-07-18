/*
     Copyright (C) 2012 LinuxMCE
     www.linuxmce.org

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#include "Advanced_IP_Camera.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;


EventMethod::EventMethod(Advanced_IP_Camera* pCamera)
{
        m_pCamera = pCamera;
	m_eventThread = NULL;
}

EventMethod::~EventMethod()
{
	// wait for the thread
	m_bRunning = false;
	if (m_eventThread != NULL)
		pthread_join (m_eventThread, NULL);

	for( vector<InputDevice*>::const_iterator it = m_vectInputDevices.begin();
	     it != m_vectInputDevices.end(); ++it )
		delete (*it);
}

void *
eventThread (void *param)
{
	EventMethod* pEventMethod = (EventMethod*)param;
	pEventMethod->EventThread();
	pthread_exit (NULL);
}

void EventMethod::Start()
{
	m_bRunning = true;
	pthread_create (&m_eventThread, NULL, eventThread, (void*)this);
}

void EventMethod::Stop()
{
	m_bRunning = false;
}

size_t EventMethod::StaticEventWriteCallback(void *ptr, size_t size, size_t nmemb, void *ourpointer) 
{
	return ((EventMethod*)ourpointer)->EventWriteCallback(ptr, size, nmemb);
}

size_t EventMethod::EventWriteCallback(void *ptr, size_t size, size_t nmemb)  {
	string separatorToken = "\n";

	if (m_bRunning) {
		m_sEventBuffer.append((char*)ptr, size*nmemb);
//		LoggerWrapper::GetInstance ()->Write (LV_WARNING, "EventWriteCallback: size*nmemb = %d", size*nmemb);

		// Check what we have got, are there any recognized events
		// split string on line endings
		size_t lastPos = 0;
		string::size_type loc = m_sEventBuffer.find( separatorToken, lastPos );
		while (loc < m_sEventBuffer.length()) {
			if( loc != string::npos ) {
				string s = m_sEventBuffer.substr(lastPos, loc);
				LoggerWrapper::GetInstance ()->Write (LV_STATUS, "EventWriteCallback: s = %s", s.c_str());
				for( vector<InputDevice*>::const_iterator it = m_vectInputDevices.begin();
				     it != m_vectInputDevices.end(); ++it )
				{
					if ((*it)->Matches(s))
						InputStatusChanged(*it, s);
				}
				lastPos = loc+1;
			}
			loc = m_sEventBuffer.find( separatorToken, lastPos );
		}
		// discard checked data
		if (lastPos != string::npos)
		{
			m_sEventBuffer = m_sEventBuffer.substr(lastPos);
			LoggerWrapper::GetInstance ()->Write (LV_STATUS, "EventWriteCallback: compacting m_sEventBuffer, new length = %d", m_sEventBuffer.length());
		}
		
		return size*nmemb;
	} else {
		return -1;
	}
}

void EventMethod::InputStatusChanged(InputDevice* pInputDevice, string trigger)
{
	LoggerWrapper::GetInstance ()->Write (LV_WARNING, "InputStatusChanged: trigger = %s", trigger.c_str());
	int newStatus = pInputDevice->GetNewStatus(trigger);

	m_pCamera->InputStatusChanged(pInputDevice, newStatus);
	pInputDevice->status = newStatus;

}
void EventMethod::EventThread() {

	// Set up connection
	string data;
	string sUrl = m_pCamera->GetBaseURL() + "/" + m_sURL;

	CURLM* eventCurl = curl_easy_init();

	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "EventThread(): sUrl: %s", sUrl.c_str ());
	curl_easy_setopt(eventCurl, CURLOPT_URL, sUrl.c_str());

	/* send all data to this function  */ 
	curl_easy_setopt(eventCurl, CURLOPT_WRITEFUNCTION, StaticEventWriteCallback);
	curl_easy_setopt(eventCurl, CURLOPT_WRITEDATA, (void *)this);
	curl_easy_setopt(eventCurl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
	if (!m_pCamera->GetUser().empty())
	{
		curl_easy_setopt(eventCurl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
		curl_easy_setopt(eventCurl, CURLOPT_USERNAME, m_pCamera->GetUser().c_str());
		if (!m_pCamera->GetPassword().empty()) {
			curl_easy_setopt(eventCurl, CURLOPT_PASSWORD, m_pCamera->GetPassword().c_str());
		}
	}
	CURLcode res = curl_easy_perform(eventCurl);
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "EventThread(): curl_Easy_perform has returned");
	if (res != 0)
	{
		LoggerWrapper::GetInstance ()->Write (LV_STATUS, "EventThread(): failed to get connection: curl error: %s", curl_easy_strerror(res));
		
	} else {
		long code;
		curl_easy_getinfo(eventCurl, CURLINFO_RESPONSE_CODE, &code);
		if (code == 200)
		{
			// http OK
			LoggerWrapper::GetInstance ()->Write (LV_STATUS, "EventThread(): http OK");
//			*pData = data.buffer;
//			*iData_Size = data.size;
		} else {
			LoggerWrapper::GetInstance ()->Write (LV_STATUS, "EventThread(): Error? http code: %d",  code);

		}
	}

	while (m_bRunning)
	{
		// http url client does nothing here
	}



}

