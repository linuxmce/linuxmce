/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

/** @file Generic_Serial_Device.h
A core piece of LinuxMCE.

Requires 2 other modules to be present:
<ul>
<li>MessageTranslation
<li>Serial
<li>DCE
<li>PlutoUtils
<li>pluto_main
</ul>
*/
//<-dceag-d-b->
#ifndef Generic_Serial_Device_h
#define Generic_Serial_Device_h

//	DCE Implemenation for #69 Generic Serial Device

#include "Gen_Devices/Generic_Serial_DeviceBase.h"
//<-dceag-d-e->

#include "MessageTranslation/AVMessageTranslation.h"
#include "Serial/GenericIODevice.h"

#include "GSDMessageProcessing.h"


/** @namespace DCE
The Data Commands and Events (DCE) namespace.

The "Generic Serial Device" module (GSD) is designed for the thousands
of devices that are controlled over RS232 and Ethernet.
It allows a LinuxMCE driver to be added using a point-and-click,
fill-in-the-blanks interface.
Basic protocols can be implemented with no programming at all.

For more complicated tasks GSD embeds the Ruby language.
Ruby is a high-level interpreted language with flexible string handling,
like Perl, but the syntax is simpler.
The protocol and embedded Ruby for GSD devices is stored in the database
and automatically synchronized with all other LinuxMCE systems as long
as you leave checked the "Share my IR codes and GSD device" box on
the A/V Equipment wizard.
So there's no compiling and no code to check in.

GSD devices that control a/v equipment, lighting, etc., can be done
in as little as an hour or two, almost never more than a day for
complex protocols.
*/

/** @class Generic_Serial_Device
A class for adding support for new devices.

The "Generic Serial Device" module (GSD) is designed for the thousands
of devices that are controlled over RS232 and Ethernet.
It allows a LinuxMCE driver to be added using a point-and-click,
fill-in-the-blanks interface.
Basic protocols can be implemented with no programming at all.

For more complicated tasks GSD embeds the Ruby language.
Ruby is a high-level interpreted language with flexible string handling,
like Perl, but the syntax is simpler.
The protocol and embedded Ruby for GSD devices is stored in the database
and automatically synchronized with all other LinuxMCE systems as long
as you leave checked the "Share my IR codes and GSD device" box on
the A/V Equipment wizard.
So there's no compiling and no code to check in.

GSD devices that control a/v equipment, lighting, etc., can be done
in as little as an hour or two, almost never more than a day for
complex protocols.
*/

//<-dceag-decl-b->!
namespace DCE
{
	class Generic_Serial_Device :
			public Generic_Serial_Device_Command,
			public GSDMessageProcessor
	{
//<-dceag-decl-e->
		// Private methods
public:
		// Public member variables
		/** Logging thread */
		static pthread_t m_LoggingThread;
		static bool m_bStopLogging;
		static int m_FdPipe[2];

		/** Constructors
        @param DeviceID is the device ident.
        @param ServerAddress is the IP address of the server.
        @param bConnectEventHandler is the flag to force connection.
        @param bLocalMode is the local flag.
        @param pRouter ia a pointer to the router instance.
        */

        /** Destructor */

        /** Get the config */

        /** Register the serial device.
        @returns false as it empty registration a present.
        */

        /** ReceivedCommandForChild.
        @param pDeviceData_Impl is a pointer to the DeviceData_Impl.
        @param sCMD_Result is the address where the result is stored.
        @param pMessage is a pointer to ehe message.

        When you receive commands that are destined to one of your children,
          then if that child implements DCE then there will already be a separate class
          created for the child that will get the message.
        If the child does not, then you will get all commands for your children
          in ReceivedCommandForChild, where pDeviceData_Base is the child device.
        If you handle the message, you should change the sCMD_Result to OK.
        */

        /** ReceivedUnknownCommand.
        @param sCMD_Result is the address where the result is stored.
        @param pMessage is a pointer to ehe message.

        When you received a valid command, but it wasn't for one of your children,
          then ReceivedUnknownCommand gets called.
        If you handle the message, you should change the sCMD_Result to OK.
        */

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Generic_Serial_Device(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Generic_Serial_Device();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

        /** Connect the serial device
        */
		virtual bool Connect(int iPK_DeviceTemplate );

        // Unused
//		virtual bool ReceivedMessage(class Message *pMessageOriginal);

        /** Transmit.

        Currently does nothing.
        */
		virtual void Transmit(const char *pData,int iSize);

        /** Split off a thread and run it.
        */
		virtual void RunThread();

        /** Parse
        */
		virtual void ParseDeviceHierarchy(DeviceData_Impl *pdevdata = NULL);

//<-dceag-const2-b->!

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_COM_Port_on_PC();
	int DATA_Get_TCP_Port();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


//<-dceag-h-e->

protected:
		virtual void DispatchMessage(Message* pmsg);
/*
public:
		DeviceData_Impl* RecursiveFindChildDevice(unsigned dwPK_Device, DeviceData_Impl* pDeviceData_Impl);
*/

	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
