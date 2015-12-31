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
/**
 *
 * @file Notification.h
 * @brief header file for the Notification class
 *
 */
 
//<-dceag-d-b->
#ifndef Notification_h
#define Notification_h

#include "PlutoUtils/MultiThreadIncludes.h"

class Row_Alert;

namespace DCE
{
	class NotificationInfo
	{
	public:
		class Notification *m_pNotification;
		int m_Type;
		string m_sPhoneNumber;
		NotificationInfo(Notification *pNotification,int Type,string sPhoneNumber) {
			m_pNotification=pNotification; m_Type=Type; m_sPhoneNumber=sPhoneNumber; }
		~NotificationInfo();
	};

	class Notification 
	{
		friend class NotificationInfo;

		// Private member variables 
	    pluto_pthread_mutex_t m_AlertMutex;
		pthread_mutexattr_t m_MutexAttr; /** < make it recursive */
		Row_Alert *m_pRow_Alert;
		class Security_Plugin *m_pSecurity_Plugin;
        class Telecom_Plugin *m_pTelecom_Plugin;
		class Router *m_pRouter;

		string m_sOther_Phone_Notifications,m_sOrbiterNotifications;
		list<NotificationInfo *> m_listNotificationInfo;

	public:
		Notification(Security_Plugin *pSecurity_Plugin,Telecom_Plugin *pTelecom_Plugin,Router *pRouter,Row_Alert *pRow_Alert);
		~Notification();
		void DoIt();

		bool NotifyLoop(int iType,bool bProcessInBackground);

		// These functions do the actualy notification.  They will not return until
		// the user acknowledges the alert, or until it times out in iDelay or MAX_TIMEOUT_FOR_PHONES seconds.
		// It returns true if the user decided to cancel the event (ie reset the Alert)
		bool NotifyOrbiter(string sPhoneNumber,int iDelay);
		bool NotifyOther(string sPhoneNumber,int iDelay);

    private: 
        string GenerateWavFile(long nAlertType, long nDeviceID);
        string GetAlertInfo(long AlertType);

        bool ExecuteNotification(string sPhoneNumber, int iDelay, bool bNotifyOrbiter);
	};
}
#endif
