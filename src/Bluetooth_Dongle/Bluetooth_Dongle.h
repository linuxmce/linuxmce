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
 * @file Bluetooth_Dongle.h
 * @brief header file for the BD_Orbiter and Bluetooth_Dongle classes
 * @author
 * @todo notcommented
 *
 */

 
//<-dceag-d-b->
#ifndef Bluetooth_Dongle_h
#define Bluetooth_Dongle_h

//	DCE Implemenation for #13 Bluetooth Dongle

#include "Gen_Devices/Bluetooth_DongleBase.h"
//<-dceag-d-e->

#ifdef SIMULATE_DETECTION
	#include "VIPShared/PhoneDetection_Simulate.h"
#else
	#ifndef WIN32
	#include <bluetooth/bluetooth.h>
	#include <bluetooth/rfcomm.h>
	#include <bluetooth/hci.h>
	#include <bluetooth/hci_lib.h>
	#include "VIPShared/PhoneDetection_Bluetooth_Linux.h"
	#else
	#include "VIPShared/PhoneDetection_Bluetooth_Windows.h"
	#endif
#endif

namespace DCE
{
	class Orbiter; /**< to be able to use it in declarations */
}

using namespace DCE;

#include "BD/BDCommandProcessor.h"

/**
 * @brief this class incapsulates what we need to assure the communication between a phone device and an orbiter instance
 * @todo ask
 */
class BD_Orbiter
{

public:

	class BDCommandProcessor *m_pBDCommandProcessor; /**< points to a BD command processor used for communicaion between the phone and the orbiter @todo ask */
	class PhoneDevice *m_pPhoneDevice; /**< the phone device */
	
	Orbiter *m_pOrbiter; /**< points to the Orbiter object */

	/**
	 * @brief constructor that assignes NULL to the member data ( witch are all pointers )
	 */
	BD_Orbiter() { m_pBDCommandProcessor=NULL; m_pOrbiter=NULL; m_pPhoneDevice=NULL; }
	
};

namespace DCE
{
	class Bluetooth_Dongle;
}

class BD_Orbiter_Plus_DongleHandle
{
public:
	BD_Orbiter* m_pBD_Orbiter;
	Bluetooth_Dongle* m_pBluetooth_Dongle;
	string m_sMacAddress;
	u_int64_t m_iMacAddress;
    string m_sVMCFile;
    string m_sConfigFile;

	BD_Orbiter_Plus_DongleHandle(class BD_Orbiter* pBD_Orbiter, class Bluetooth_Dongle* pBluetooth_Dongle, 
			string sMacAddress, u_int64_t iMacAddress, string sVMCFile, string sConfigFile)
	{
		m_pBD_Orbiter = pBD_Orbiter;
		m_pBluetooth_Dongle = pBluetooth_Dongle;
		m_sMacAddress = sMacAddress;
		m_iMacAddress = iMacAddress;
        m_sVMCFile = sVMCFile;
        m_sConfigFile = sConfigFile;
	}
};

class BD_ReconnectInfo
{
public:
    Bluetooth_Dongle* m_pBluetooth_Dongle;
    string m_sPhoneMacAddress;
    int m_iDeviceToLink;
    string m_sVMCFile;
    string m_sConfig_File;

    BD_ReconnectInfo(class Bluetooth_Dongle* pBluetooth_Dongle, string sPhoneMacAddress, int iDeviceToLink, string sVMCFile,
        string sConfig_File)
    {
        m_pBluetooth_Dongle = pBluetooth_Dongle;
        m_sPhoneMacAddress = sPhoneMacAddress;
        m_iDeviceToLink = iDeviceToLink;
        m_sVMCFile = sVMCFile;
        m_sConfig_File = sConfig_File;
    }
};

//<-dceag-decl-b->! this is custom
namespace DCE
{
	/** we use different definitions depending on the environment */
	
	/**
	 * @brief used to allow comunication between the router and the phone
	 */
#ifdef SIMULATE_DETECTION
	class Bluetooth_Dongle : public Bluetooth_Dongle_Command, public PhoneDetection_Simulate
#else
#ifdef WIN32
	class Bluetooth_Dongle : public Bluetooth_Dongle_Command, public PhoneDetection_Bluetooth_Windows
#else

	class Bluetooth_Dongle : public Bluetooth_Dongle_Command, public PhoneDetection_Bluetooth_Linux
#endif
#endif
	{
	//<-dceag-decl-e->
	
	//<-dceag-const-b->
public:
		// Constructors/Destructor
		Bluetooth_Dongle(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Bluetooth_Dongle();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
	
		pthread_mutexattr_t m_MutexAttr; /** < make it recursive */
		pluto_pthread_mutex_t m_BTMutex; /**< to control access to the shared memory */
		int m_dwPK_EntertainArea; /**< Our EntertainArea */

        pthread_cond_t m_MessageQueueCond; /** < @todo ask */
        pluto_pthread_mutex_t m_ScreenMutex; /** < Anything that should not be done during a screen render, change, etc. Blocking this will prevent screen changes */

	private:
		// Create a bug reports
		static void BugReport( void );
	
	public:
		// Public member variables
		map<string, BD_Orbiter *> m_mapOrbiterSockets;
		
		/**
		 * @brief finds the BD_Orbiter object searching it by mac address
		 */
		BD_Orbiter *m_mapOrbiterSockets_Find( string sMacAddress ) 
		{
			PLUTO_SAFETY_LOCK(bm, m_BTMutex);
			map<string, class BD_Orbiter *>::iterator it = m_mapOrbiterSockets.find( sMacAddress );
			return it == m_mapOrbiterSockets.end() ? NULL : ( *it ).second;
		}
	
		bool Connect(int iPK_DeviceTemplate); // Special stuff after connection

        /** From PhoneDetectionEngine class */
		
		/** 
		 * @brief This is the actual scan loop that gets called by the worker thread
		 * The worker thread will set m_bInScanLoop to true, and keep calling it until
		 * the thread returns false. This loop will add new devices to the map, and call
		 * Intern_ methods
		 */
		virtual bool ScanningLoop();
	
		/**
		 * The derived class must decide how to handle when new devices are detected or lost, or 
		 * when the signal strength changes by more than 10 in either direction
		 */
		
		/**
		 * @brief used to register a new device by the PhoneDetection base class
		 * calls SignalStrengthChanged to register the new device 
		 * @see SignalStrengthChanged
		 */ 
		virtual void NewDeviceDetected( class PhoneDevice *pDevice );

		/**
		 * @brief checks for different situations in which the phone device might be lost and deletes the allocated data if it is really lost
		 * @todo ask
		 */
		virtual void LostDevice( class PhoneDevice *pDevice );
		
		virtual void Intern_NewDeviceDetected(class PhoneDevice *pDevice);

		virtual void Intern_LostDevice(class PhoneDevice *pDevice);

		/**
		 * @brief called if the signal strength varies by moe then 10 to refresh the connection
		 * @todo ask
		 */
		virtual void SignalStrengthChanged( class PhoneDevice *pDevice );
		
		/**
		 * @brief gets the private member data
		 */
		string MacAddress() { return GetData()->m_sMacAddress; }
	//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class

			*****EVENT***** accessors inherited from base class
	void EVENT_Mobile_orbiter_detected(string sMac_Address,int iSignal_Strength,string sID);
	void EVENT_Mobile_orbiter_linked(string sMac_Address,string sVersion);
	void EVENT_Mobile_orbiter_lost(string sMac_Address,bool bConnectionFailed);

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #60 - Link with mobile orbiter */
	/** After the dongle has detected the device, then the orbiter plugin will send this command to tell the dongle to link to the phone. */
		/** @param #47 Mac address */
			/** The mac address of the phone */
		/** @param #118 VMC File */
			/** If VMC File is not empty, BluetoothDongle will have to send the file to PlutoMO */
		/** @param #130 Config File */
			/** Path to Config File the be sent to PlutoMO */

	virtual void CMD_Link_with_mobile_orbiter(string sMac_address,string sVMC_File,string sConfig_File) { string sCMD_Result; CMD_Link_with_mobile_orbiter(sMac_address.c_str(),sVMC_File.c_str(),sConfig_File.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Link_with_mobile_orbiter(string sMac_address,string sVMC_File,string sConfig_File,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #61 - Get Signal Strength */
	/** Returns the actual signal strength of the phone for this dongle. */
		/** @param #47 Mac address */
			/** The mac address of the phone */
		/** @param #48 Value */
			/** The strength of the connection
(255 is the maximum strength) */

	virtual void CMD_Get_Signal_Strength(string sMac_address,int *iValue) { string sCMD_Result; CMD_Get_Signal_Strength(sMac_address.c_str(),iValue,sCMD_Result,NULL);};
	virtual void CMD_Get_Signal_Strength(string sMac_address,int *iValue,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #62 - Create Mobile Orbiter */
	/** The bluetooth dongle spawns an internal mobile orbiter which communicates with the phone. */
		/** @param #2 PK_Device */
			/** The ID of the controller to spawn. */
		/** @param #45 PK_EntertainArea */
			/** If not empty, the Orbiter will be created in this entertainarea rather than the one for the dongle.  This will take precendence over PK_Room */
		/** @param #47 Mac address */
			/** The Mac Address of the phone. */
		/** @param #57 PK_Room */
			/** If not 0, the device will be created in this room instead of the default room for the dongle. */

	virtual void CMD_Create_Mobile_Orbiter(int iPK_Device,string sPK_EntertainArea,string sMac_address,int iPK_Room) { string sCMD_Result; CMD_Create_Mobile_Orbiter(iPK_Device,sPK_EntertainArea.c_str(),sMac_address.c_str(),iPK_Room,sCMD_Result,NULL);};
	virtual void CMD_Create_Mobile_Orbiter(int iPK_Device,string sPK_EntertainArea,string sMac_address,int iPK_Room,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #332 - Ignore MAC Address */
	/** Causes the dongle to ignore any detections of this MAC Address. */
		/** @param #47 Mac address */
			/** The Mac Address */

	virtual void CMD_Ignore_MAC_Address(string sMac_address) { string sCMD_Result; CMD_Ignore_MAC_Address(sMac_address.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Ignore_MAC_Address(string sMac_address,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #333 - Disconnect From Mobile Orbiter */
	/** Disconnects Mobile Orbiter from this BluetoothDongle. */
		/** @param #47 Mac address */
			/** The mac address of the phone */
		/** @param #118 VMC File */
			/** Path the VMC file to send */
		/** @param #124 DeviceToLink */
			/** Send CMD_Link_with_mobile_orbiter command to DeviceToLink */
		/** @param #130 Config File */
			/** Path to Config File to send */

	virtual void CMD_Disconnect_From_Mobile_Orbiter(string sMac_address,string sVMC_File,int iDeviceToLink,string sConfig_File) { string sCMD_Result; CMD_Disconnect_From_Mobile_Orbiter(sMac_address.c_str(),sVMC_File.c_str(),iDeviceToLink,sConfig_File.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Disconnect_From_Mobile_Orbiter(string sMac_address,string sVMC_File,int iDeviceToLink,string sConfig_File,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
	};
	
	//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
