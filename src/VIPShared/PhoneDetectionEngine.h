/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#ifndef PhoneDetectionEngine_h
#define PhoneDetectionEngine_h

#include "PlutoUtils/MultiThreadIncludes.h"
#include "Logger.h"

using namespace DCE;

class PhoneDetectionEngine
{
public:
	// This will always have the currently detected devices.  You must grab
	// the m_MapMutex before accessing it.  The Intern_ methods add and remove
	// to this map
	map<u_int64_t,class PhoneDevice *> m_mapPhoneDevice_Detected;
	map<u_int64_t,class PhoneDevice *> m_mapDevicesDetectedThisScan;
	map<u_int64_t,bool> m_mapIgnoreMacs; // Just a map to detect presence.  The bool isn't used

    map<u_int64_t,string> m_mapKnownNames;//If a phone was detected already, we'll find its name here

	bool m_bInScanLoop; // True if it's in the scanning loop
	bool m_bAbortScanLoop;  // True when we are in the process of Aborting the scan
    
    //We are not going to perform any scanning if this is true
    //the current scan loop will be interrupted right again if this one is set to true
    bool m_bScanningSuspended; 
	
    pthread_t m_ThreadID;
	pluto_pthread_mutex_t m_MapMutex,m_StartStopMutex,m_VariableMutex;
	pthread_mutexattr_t m_MutexAttr;

	PhoneDetectionEngine() 
		: 	m_MapMutex("detect map mutex"),m_StartStopMutex("detect ss mutex"),
            m_VariableMutex("other maps mutex")
    { 
		pthread_mutexattr_init( &m_MutexAttr );
		pthread_mutexattr_settype( &m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );
		m_MapMutex.Init(&m_MutexAttr);
		m_StartStopMutex.Init(NULL);
        m_VariableMutex.Init(NULL);
		m_bInScanLoop=false;
		m_bAbortScanLoop=false;
        m_bScanningSuspended = false;
		m_ThreadID=NULL;
	}
	virtual ~PhoneDetectionEngine() 
	{ 
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Phone Detection Engine terminating");
		StopScanning(); 

        pthread_mutex_destroy(&m_VariableMutex.mutex);
        pthread_mutex_destroy(&m_StartStopMutex.mutex);
        pthread_mutex_destroy(&m_MapMutex.mutex);
        pthread_mutexattr_destroy(&m_MutexAttr);
	}

	PhoneDevice *m_mapPhoneDevice_Detected_Find(u_int64_t MacAddress)
	{
		PLUTO_SAFETY_LOCK(mm,m_MapMutex);
		map<u_int64_t,class PhoneDevice *>::iterator it = m_mapPhoneDevice_Detected.find(MacAddress);
		return it==m_mapPhoneDevice_Detected.end() ? NULL : (*it).second;
	}

	PhoneDevice *m_mapDevicesDetectedThisScan_Find(u_int64_t MacAddress)
	{
		PLUTO_SAFETY_LOCK(mm,m_MapMutex);
		map<u_int64_t,class PhoneDevice *>::iterator it = m_mapDevicesDetectedThisScan.find(MacAddress);
		return it == m_mapDevicesDetectedThisScan.end() ? NULL : (*it).second;
	}

	// StartScanning should spawn a new worker thread that starts scanning
	// and return immediately
	virtual void StartScanning();

	// StopScanning will kill the thread, and return only when it has safely
	// stopped the scan
	virtual void StopScanning();

    //break the current scanning loop, ignore its results and do not start another one
    //untip ResumeScanning is called
    virtual void SuspendScanning();
    virtual void ResumeScanning();

	// RequestStopScanning will kill the thread, but will return immediately.
	// The calling class can later call StopScanning to be sure it has stopped
	virtual void RequestStopScanning() { m_bAbortScanLoop=true; };

	// This is the actual scan loop that gets called by the worker thread
	// The worker thread will set m_bInScanLoop to true, and keep calling it until
	// the thread returns false.  This loop will add new devices to the map, and call
	// Intern_ methods
	virtual bool ScanningLoop()=0;

	// The derived class must decide how to handle when new devices are detected or lost, or 
	// when the signal strength changes by more than 10 in either direction
	virtual void NewDeviceDetected(class PhoneDevice *pDevice)=0;
	virtual void LostDevice(class PhoneDevice *pDevice)=0;
	virtual void SignalStrengthChanged(class PhoneDevice *pDevice)=0;

	virtual void AddDeviceToDetectionList(class PhoneDevice *pDevice);
	virtual void RemoveDeviceFromDetectionList(u_int64_t iMacAddress);

	//this method should not be overrided in derived classes
	virtual void DetectionLogic(); 
    
	virtual int GetLinkQuality(const char *addr) { return 0; };


	
	// Since the scanning routine can be sensitive, rather than handling everything on the 
	// same thread, we'll just span a new thread to handle the events.  These Intern_
	// functions spawn a new thread which calls the event handlers above
protected:

	// This will also add the phone to the map
	virtual void Intern_NewDeviceDetected(class PhoneDevice *pDevice);

	// This will also remove the phone from the map
	virtual void Intern_LostDevice(class PhoneDevice *pDevice);
	virtual void Intern_SignalStrengthChanged(class PhoneDevice *pDevice);
};
#endif

