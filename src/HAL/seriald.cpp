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
//
// C++ Implementation: seriald
//
// Description:
//
//
// Author: Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "seriald.h"
#include "HAL.h"

#include "DCE/Logger.h"
#include "Serial/SerialPort.h"
#include "PlutoUtils/StringUtils.h"
#include "Gen_Devices/AllCommandsRequests.h"

DCE::HAL * SerialD::halDevice = NULL;
bool SerialD::running = false;
bool SerialD::stoprunning = false;
std::vector<std::string> SerialD::serialDevices;

SerialD::SerialD()
{
}

SerialD::~SerialD()
{
}

void* SerialD::startUp(void * device)
{
	running = true;
	stoprunning = false;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "SerialD::startUp  Waiting 10 seconds to let GSD devices start first and disable any invalid ports");
	Sleep(10000);
	LoggerWrapper::GetInstance()->Write(LV_DEBUG, "SerialD::startUp pthread_create ############ SerialD Start ");

	if( device == NULL )
	{
		// error
		running = false;
		return NULL;
	}

	halDevice = (HAL *)device;
	if( halDevice->m_pData == NULL )
	{
		// error
		running = false;
		return NULL;
	}

	string sPort;
	unsigned baud = 9600;

	string val2assign, response;

	if( running )
	{
		CMD_Get_Unused_Serial_Ports_DT cmd(halDevice->m_dwPK_Device, DEVICETEMPLATE_General_Info_Plugin_CONST, BL_SameHouse, halDevice->m_dwPK_Device, &val2assign);
		halDevice->SendCommand(cmd, &response);

		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Serial ports %s -- %s\n", response.c_str(), val2assign.c_str());

		if( !val2assign.empty() )
		{
			StringUtils::Tokenize(val2assign, ",", serialDevices);
		}
	}


	// thread loop
	while(!stoprunning)
	{
		// test each available serial port
		std::vector<std::string>::iterator it=serialDevices.begin();
		while( it!=serialDevices.end() )
		{
			sPort = (*it);
			try
			{
				CSerialPort serialPort(sPort, baud, epbsN81, true);

				if( serialPort.IsBusy() )
				{
					LoggerWrapper::GetInstance()->Write(LV_DEBUG, "++++++++++++ SerialD Busy %s", sPort.c_str());
					// fire event
					halDevice->EVENT_Device_Detected("", "", "", 0, "", 2, 0, "", "37|" + sPort, "serial", halDevice->m_sSignature_get());
					// remove it from tests
					it = serialDevices.erase(it);
					continue;
				}
				else
				{
//					LoggerWrapper::GetInstance()->Write(LV_DEBUG, "------------ SerialD Not Busy %s", sPort.c_str());
				}
			}
			catch(string sError)
			{
				LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Exception: %s", sError.c_str());
			}

			++it;
		}

		usleep(3000000);
	}

	LoggerWrapper::GetInstance()->Write(LV_DEBUG, "############ SerialD END ");
	running = false;

	return NULL;
}

bool SerialD::shutDown()
{
	LoggerWrapper::GetInstance()->Write(LV_DEBUG, "############ SerialD shutDown waiting for thread");
	stoprunning = true;
	time_t timeout = time(NULL) + 15;
	while( running && time(NULL) < timeout )
	{
		usleep(10);
	}
	LoggerWrapper::GetInstance()->Write(LV_DEBUG, "############ SerialD shutDown thread exited or timed out: running=%d",(int) running);
	return running==false;
}
