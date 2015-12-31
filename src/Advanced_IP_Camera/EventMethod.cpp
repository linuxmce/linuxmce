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
	m_iInterval = 10; //10 seconds as a sensible default
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

	LoggerWrapper::GetInstance ()->Write (LV_WARNING, "InputStatusChanged: old status = %d, new status = %d", pInputDevice->status, newStatus);
	if ( pInputDevice->status != newStatus )
	{
		m_pCamera->InputStatusChanged(pInputDevice, newStatus);
		pInputDevice->status = newStatus;
	}

}
void EventMethod::EventThread() {

	if (StringUtils::CompareNoCase(m_sMethod, "url") == 0 || 
	    StringUtils::CompareNoCase(m_sMethod, "urlPoll") == 0) {
		MethodURL();
	} else if (StringUtils::CompareNoCase(m_sMethod, "httpserver") == 0) {
		MethodServer();
	}
}

void EventMethod::MethodURL()
{
	bool usePolling = StringUtils::CompareNoCase(m_sMethod, "urlPoll") == 0;
	while (m_bRunning)
	{
		// Set up connection
		string data;
		string sUrl = m_pCamera->GetBaseURL() + m_sURL;

		CURLM* eventCurl = curl_easy_init();

		LoggerWrapper::GetInstance ()->Write (LV_STATUS, "EventMethod::MethodURL(): sUrl: %s", sUrl.c_str ());
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
		LoggerWrapper::GetInstance ()->Write (LV_STATUS, "EventMethod::MethodURL(): curl_Easy_perform has returned");
		if (res != 0)
		{
			LoggerWrapper::GetInstance ()->Write (LV_CRITICAL, "EventMethod::MethodURL(): failed to get connection: curl error: %s", curl_easy_strerror(res));
			// Sleep and retry
			struct timespec req;
			struct timespec rem;
			req.tv_sec = 120;
			req.tv_nsec = 0;
			nanosleep(&req, &rem);
			
		} else {
			long code;
			curl_easy_getinfo(eventCurl, CURLINFO_RESPONSE_CODE, &code);
			if (code == 200)
			{
				// http OK
				LoggerWrapper::GetInstance ()->Write (LV_STATUS, "EventMethod::MethodURL(): http OK");
//				*pData = data.buffer;
//				*iData_Size = data.size;
			} else {
				LoggerWrapper::GetInstance ()->Write (LV_CRITICAL, "EventMethod::MethodURL(): Error? http code: %d",  code);

			}
			if (usePolling) {
				// polling, close connection and restart it after a short timeout
				curl_easy_cleanup(eventCurl);
				struct timespec req;
				struct timespec rem;
				req.tv_sec = m_iInterval;
				req.tv_nsec = 0;
				nanosleep(&req, &rem);
			} else {
				// Keep-alive connection and camera sends new data on same connection
				while (m_bRunning)
				{
					// http url client does nothing here
				}
			}
		}
	}
}

void EventMethod::MethodServer()
{
	string serverPort;
	if (atoi(m_sURL.c_str()) > 0) {
		serverPort = m_sURL;
	} else {
		LoggerWrapper::GetInstance ()->Write (LV_CRITICAL, "EventMethod::MethodServer(): unable to find server port, m_sURL = %s", m_sURL.c_str());
		return;
	}

	int serversockfd, newsockfd;
	struct addrinfo ai_setup;
	struct addrinfo *ai_server;
	struct sockaddr_storage remoteAddr;
	memset(&ai_setup, 0, sizeof(ai_setup));
	ai_setup.ai_family = AF_UNSPEC;
	ai_setup.ai_socktype = SOCK_STREAM;
	ai_setup.ai_flags = AI_PASSIVE;

	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "EventMethod::MethodServer(): Setting up listening socket.");
	int status = getaddrinfo(NULL, serverPort.c_str(), &ai_setup, &ai_server);
	if ( status != 0 )
	{
		LoggerWrapper::GetInstance ()->Write (LV_CRITICAL, "EventMethod::MethodServer(): Error getaddrinfo");
		return;
	}

	serversockfd = socket(ai_server->ai_family, ai_server->ai_socktype, 0);
	if ( serversockfd < 0 )
	{
		LoggerWrapper::GetInstance ()->Write (LV_CRITICAL, "EventMethod::MethodServer(): Error opening socket");
		return;
	}

	if ( bind(serversockfd, ai_server->ai_addr, ai_server->ai_addrlen) < 0 )
	{
		LoggerWrapper::GetInstance ()->Write (LV_CRITICAL, "EventMethod::MethodServer(): Error binding");
		return;
	}

	if ( listen(serversockfd, 20) < 0 )
	{
		LoggerWrapper::GetInstance ()->Write (LV_CRITICAL, "EventMethod::MethodServer(): Error listening");
		return;
	}

	while (m_bRunning)
	{
		LoggerWrapper::GetInstance ()->Write (LV_STATUS, "EventMethod::MethodServer(): waiting for incoming connection.");
		socklen_t remoteAddrSize = sizeof remoteAddr;
		newsockfd = accept(serversockfd, (struct sockaddr *)&remoteAddr, &remoteAddrSize);
		if ( newsockfd == -1 )
		{
			LoggerWrapper::GetInstance ()->Write (LV_CRITICAL, "EventMethod::MethodServer(): Error accepting incoming connection");
		} else {
			LoggerWrapper::GetInstance ()->Write (LV_STATUS, "EventMethod::MethodServer(): Got connection");
			HandleHttpCom(newsockfd);
		LoggerWrapper::GetInstance ()->Write (LV_STATUS, "EventMethod::MethodServer(): closing socket");
		}
		close(newsockfd);
	}
	freeaddrinfo(ai_server);
	close(serversockfd);
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "EventMethod::MethodServer(): end");
}

void EventMethod::HandleHttpCom(int socketfd)
{
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "EventMethod::HandleHttpCom(): start");
	// Handle http communications

	// receive data
	string data;
	char buffer[3000];
	int bytes = recv(socketfd, buffer, 3000, 0);
	data.append(buffer, bytes);
	while (bytes == 3000) {
		data.append(buffer, bytes);
		bytes = recv(socketfd, buffer, 3000, 0);
	}
	if (bytes == 0) {
		LoggerWrapper::GetInstance ()->Write (LV_CRITICAL, "EventMethod::HandleHttpCom(): Remote closed on us");
		return;
	}
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "EventMethod::HandleHttpCom(): data = %s", data.c_str());


	// Send response
	string response = "HTTP/1.1 200\n";
	response += "\n\r";
	const char* rc = response.c_str();
	int totalSent = 0;
	size_t toSend = strlen(rc);
	int sent = write(socketfd, rc, toSend);
	while (sent >= 0 && sent + totalSent < response.size()) 
	{
		totalSent += sent;
		sent = send(socketfd, response.substr(totalSent).c_str(), toSend - totalSent, 0);
	}
	if (sent < 0)
	{
		LoggerWrapper::GetInstance ()->Write (LV_CRITICAL, "EventMethod::HandleHttpCom(): error sending response");
	}
	
	for( vector<InputDevice*>::const_iterator it = m_vectInputDevices.begin();
	     it != m_vectInputDevices.end(); ++it )
	{
		if ((*it)->Matches(data))
			InputStatusChanged(*it, data);
	}

	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "EventMethod::HandleHttpCom(): end");
}
