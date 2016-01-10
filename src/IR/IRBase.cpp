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
#include "DCE/CommonIncludes.h"
#include "DCE/DCECommand_Impl.h"
#include "DCE/DCEDeviceData_Impl.h"
#include "DCE/StringUtils.h"
#include "DCE/DCELogger.h"
#include "common/Serial/SerialPort.h"

#include <math.h>

#include "DCE/Gen_Constants/ActionData.h"
#include "DCE/Gen_Constants/EventListData.h"
#include "DCE/Gen_Constants/C_ActionParameterData.h"
#include "DCE/Gen_Constants/C_EventParameterData.h"
#include "DCE/Gen_Constants/C_DeviceParameterData.h"

#include "IRBase.h"

#ifndef WIN32

#include <sys/types.h>
#include <sys/times.h>
#include <time.h>

#define clock() linux_ticks()

this_file_isnt_used

clock_t CLK_TCK;

clock_t linux_ticks()
{
	struct tms mytms;
	return times(&mytms);
}

#endif

void IRBase::Init(DCECommand_Impl *pCommand)
{
	m_CurrentChannelSequenceNumber = 1;
	m_pCmd = pCommand;
	pthread_mutex_init(&m_IRMutex.mutex, 0);
	ParseDevices();
#ifndef WIN32
	CLK_TCK = 1000;
	// No linux method seems to be able to give you this.. grr
#endif
}

IRBase::~IRBase()
{
	pthread_mutex_destroy(&m_IRMutex.mutex);
}

bool IRBase::ProcessQueue()
{
	DCESafetyLock sl(&m_IRMutex);
	list<IRQueue *>::iterator ipQueue;
	clock_t CurTime = clock();

	// printf("Clk = %d, CLK_TCK = %d\n", clock(), CLK_TCK);

	for(ipQueue = m_IRQueue.begin(); ipQueue!= m_IRQueue.end(); ++ipQueue)
	{
		// Is the last IR on this port + this pre delay still in the future?  If so continue on to the next queued item
		
		if ((*ipQueue)->m_PreDelay && MS_TO_CLK((*ipQueue)->m_PreDelay) + m_PortLastIRSent[(*ipQueue)->m_Port] > CurTime)
			continue;

		// Can this device accept IR yet?
		if ((*ipQueue)->m_DeviceNum > -1 && m_DeviceNextAvailableTime[(*ipQueue)->m_DeviceNum] > CurTime)
			continue;

		if ((*ipQueue)->m_DeviceNum > -1 && (*ipQueue)->m_ChannelSequenceNumber)
			m_ExecutingChannelSequenceNumber[(*ipQueue)->m_DeviceNum] = (*ipQueue)->m_ChannelSequenceNumber;

		// All above checks passed for this entry.  Fire it off, update the scheduler, remove, and return.

		if ((*ipQueue)->m_Port.substr(0,4)!="ttyS")
		{ 
			g_pDCELogger->Write(LV_STATUS, "IRQueue: Device %d, Action %d, Step %d", (*ipQueue)->m_DeviceNum, (*ipQueue)->m_ActionNum, (*ipQueue)->m_StepNumber);
			SendIR((*ipQueue)->m_Port, (*ipQueue)->m_IRCode);
			CurTime = clock();
		}
		else 
		{
			// If the target is a serial port, send it directly to the port

			if ((*ipQueue)->m_IRCode.substr(0,4)=="SER,")
			{
                string::size_type spos=0;
                string Port = StringUtils::Tokenize((*ipQueue)->m_Port, ",", spos);
                int Speed = atoi(StringUtils::Tokenize((*ipQueue)->m_Port, ",", spos).c_str());
                // todo string ParityBitStop = StringUtils::Tokenize(sPort, ",", spos);

                CSerialPort sp(Port, Speed, epbsN81);
				string OutStr = StringUtils::EscapeChars((*ipQueue)->m_IRCode.substr(4)); // will handle hex conversions
                sp.Write((char *)OutStr.c_str(), OutStr.length());
                printf("Writing %s to port %s speed %d\n",OutStr.c_str(), Port.c_str(), Speed);
			}
			else
			{
				g_pDCELogger->Write(LV_WARNING, "Trying to send code to a serial port that's not serial data (=%s)", (*ipQueue)->m_IRCode.c_str());
			}
		}
		// Now, update the port's last tick count and set the device's next readiness time
		if ((*ipQueue)->m_DeviceNum > -1)
		{
			(*ipQueue)->m_Delay = max((*ipQueue)->m_Delay, m_DeviceDefaultDelays[(*ipQueue)->m_DeviceNum]);
			m_DeviceNextAvailableTime[(*ipQueue)->m_DeviceNum] = CurTime + MS_TO_CLK((*ipQueue)->m_Delay);
		}
		m_PortLastIRSent[(*ipQueue)->m_Port] = CurTime;
		m_IRQueue.erase(ipQueue);
		return true;
	}
	return false;
}

void IRBase::ParseDevices()
{
	mapDCECommand_Impl::iterator iChild;
	for(iChild = m_pCmd->m_mapDCECommandImpl_Children.begin(); iChild != m_pCmd->m_mapDCECommandImpl_Children.end(); ++iChild)
	{
		int DeviceID = (*iChild).first;
		
		m_DevicePortStringMap[DeviceID]=(*iChild).second->m_pData->m_Parameters[C_DEVICEPARAMETER_IR_PORTNUM_CONST];
		string codes = (*iChild).second->m_pData->m_Parameters[C_DEVICEPARAMETER_IRDATA_CONST];
	
		int CodeID, count;
		string code;
		size_t pos = 0;

		count = atoi(StringUtils::Tokenize(codes, "|", pos).c_str());
		for(int i=0;i<count;i++)
		{
			CodeID = atoi(StringUtils::Tokenize(codes, "|", pos).c_str());
			code = StringUtils::Tokenize(codes, "|", pos);
			m_CodeMap[IntPair(DeviceID, CodeID)] = code;	
			g_pDCELogger->Write(LV_STATUS, "Loaded IR code for Device %d, Action %d", DeviceID, CodeID);
		}
		m_DevicePreDelays[DeviceID] = DEFAULT_PREDELAY;
		m_DeviceDefaultDelays[DeviceID] = DEFAULT_POSTDELAY;
		
		map<long,string>::iterator param;
       		param = (*iChild).second->m_pData->m_Parameters.find(C_DEVICEPARAMETER_IR_SLEEP_BEFORE_AFTER_CONST);
		if (param != m_pCmd->m_mapDCECommandImpl_Children[DeviceID]->m_pData->m_Parameters.end())
		{
			string::size_type pos = 0;
			string tok = StringUtils::Tokenize((*param).second, ",", pos);
			if (tok.length())
			{
				m_DevicePreDelays[DeviceID] = atoi(tok.c_str());
				tok = StringUtils::Tokenize((*param).second, ",", pos);
				if (tok.length())
				{
					m_DeviceDefaultDelays[DeviceID] = atoi(tok.c_str());
				}
			}
		}
	}
}

void IRBase::AddIRToQueue(string Port, string IRCode, int Delay, int DeviceNum, int ActionNum, int Count, int ChannelSequenceNumber)
{
	DCESafetyLock sl(&m_IRMutex);
	
	// If this is a volume command, check the outgoing queue and 
	// erase other volume requests. 

	if (ActionNum == ACTION_AV_VOLUME_UP_CONST || ActionNum == ACTION_AV_VOLUME_DOWN_CONST)
	{
		list<IRQueue *>::iterator ipQueue, ipNextQueue;
		for(ipNextQueue = m_IRQueue.begin();ipNextQueue != m_IRQueue.end(); )
		{
			ipQueue = ipNextQueue++;
			
			if (((*ipQueue)->m_ActionNum==ACTION_AV_VOLUME_UP_CONST || (*ipQueue)->m_ActionNum==ACTION_AV_VOLUME_DOWN_CONST) && 
				(*ipQueue)->m_DeviceNum==DeviceNum)
			{
				m_IRQueue.erase(ipQueue);
			}
		}
	}
	else if (ChannelSequenceNumber)
	{
		list<IRQueue *>::iterator ipQueue, ipNextQueue;
		for(ipNextQueue = m_IRQueue.begin();ipNextQueue != m_IRQueue.end(); )
		{
			ipQueue = ipNextQueue++;
			
			if (((*ipQueue)->m_ChannelSequenceNumber && DeviceNum > -1 &&
				(*ipQueue)->m_ChannelSequenceNumber != ChannelSequenceNumber &&
				(*ipQueue)->m_DeviceNum==DeviceNum) && 
				(*ipQueue)->m_ChannelSequenceNumber != m_ExecutingChannelSequenceNumber[DeviceNum])
			{
				printf("Removing redundant channel change digit (action %d, seq %d).\n", (*ipQueue)->m_ActionNum, (*ipQueue)->m_ChannelSequenceNumber);
				m_IRQueue.erase(ipQueue);
			}
		}
	}

	
	IntPair IRCodePair(DeviceNum, ActionNum);
	if (IRCode.length() == 0)
	{
		if (ActionNum == -1 || DeviceNum == -1)
		{
			g_pDCELogger->Write(LV_CRITICAL, "Tried to AddIRToQueue without either an IR code or a device/action pair.");
		}
		else
		{
			// No IR code.  We need to look it up
			map <IntPair, string>::iterator iCode = m_CodeMap.find(IRCodePair);
			if (iCode == m_CodeMap.end())
			{
				g_pDCELogger->Write(LV_WARNING, "Could not find a code for Device %d, Action %d", DeviceNum, ActionNum);
			}
			else
			{
				IRCode = (*iCode).second;
			}
		}
	}
	else
	{
		if (ActionNum != -1 && DeviceNum != -1)
		{
			// We have an IRCode and an action pair, this is usually the result of being sent a new code from 
			// the server, so, update our map.
			m_CodeMap[IRCodePair] = IRCode;
		}
	}
	if (Port.length() == 0)
	{
		map <int, string>::iterator iPort = m_DevicePortStringMap.find(DeviceNum);
		if (iPort != m_DevicePortStringMap.end())
		{
			Port = (*iPort).second;

		}
	}
	else
	{
		if (DeviceNum != -1)
		{
			m_DevicePortStringMap[DeviceNum] = Port;
		}
	}
	int Step = 1;
	IRQueue *pLastQueued=NULL;
	int i;
	for(i=0;i<Count;++i)
	{
		string::size_type pos=0;
		string tok;
		while((tok = StringUtils::Tokenize(IRCode, "&", pos))!="")
		{
			string::size_type pos2=0;
			string Code = StringUtils::Tokenize(tok, "@", pos2);
			int Delay = atoi(StringUtils::Tokenize(tok, "@", pos2).c_str());
			m_IRQueue.push_back((pLastQueued = new IRQueue(Port, Code, Delay, DeviceNum, ActionNum, Step++, m_DevicePreDelays[DeviceNum], ChannelSequenceNumber)));
		}
		if (pLastQueued)
		{
			pLastQueued->m_Delay+=Delay;
		}
	}
}

bool IRBase::AddChannelChangeToQueue(int ChannelNumber, int Device)
{
	if (m_pCmd->m_mapDCECommandImpl_Children.count(Device) == 0)
	{
		g_pDCELogger->Write(LV_CRITICAL, "Cannot send channel change to non-existent device %d!", Device);
		return false;
	}
	int NumDigits = 3;
	bool bSendEnter = false;
	map<long,string>::iterator param;
	param = m_pCmd->m_mapDCECommandImpl_Children[Device]->m_pData->m_Parameters.find(C_DEVICEPARAMETER_NUMBER_DIGITS_CONST);
	if (param != m_pCmd->m_mapDCECommandImpl_Children[Device]->m_pData->m_Parameters.end())
	{
		string::size_type pos = 0;

		NumDigits = atoi(StringUtils::Tokenize((*param).second, ",", pos).c_str());
		string tok = StringUtils::ToUpper(StringUtils::Tokenize((*param).second, ",", pos));

		bSendEnter = (tok=="E" || tok=="e");
	}
	else
	{
		printf("Device id %d has no number digits parameter.  Assuming 3 and no enter.\n", Device);
	}
	int DigitDelay = 0;
	param = m_pCmd->m_mapDCECommandImpl_Children[Device]->m_pData->m_Parameters.find(C_DEVICEPARAMETER_DIGIT_DELAY_CONST);
	if (param != m_pCmd->m_mapDCECommandImpl_Children[Device]->m_pData->m_Parameters.end())
	{
		DigitDelay = atoi((*param).second.c_str());
	}
	int TotalDigits = StringUtils::itos(ChannelNumber).length();
	if (NumDigits < TotalDigits)
	{
		if (NumDigits > 0)
		{
			g_pDCELogger->Write(LV_WARNING, "Warning, number of digits specified as %d but channel is %d!", NumDigits, ChannelNumber);
		}
		NumDigits = TotalDigits;
	}
	for(int i=NumDigits;i>0;i--)
	{
		unsigned char digit = (ChannelNumber%(int)pow((double)10,i))/(int)pow((double)10,i-1);
		g_pDCELogger->Write(LV_STATUS, "Sending digit %d...",digit);	
		AddIRToQueue("", "", DigitDelay, Device, digit+80, 1, m_CurrentChannelSequenceNumber);
	}
	if (bSendEnter)
	{
		g_pDCELogger->Write(LV_STATUS, "Sending <enter>...");			
		AddIRToQueue("", "", DigitDelay, Device, ACTION_GEN_ENTER_CONST, 1, m_CurrentChannelSequenceNumber);
	}
	m_CurrentChannelSequenceNumber++;
	return true;
}

bool IRBase::ProcessDCEMessage(DCEMessage *pMessage)
{
	int TargetDevice = pMessage->m_DeviceIDTo;

	// Is this a slave of another device?  If so, change the target

	mapDCECommand_Impl::iterator iTD = m_pCmd->m_mapDCECommandImpl_Children.find(TargetDevice);
	if (iTD != m_pCmd->m_mapDCECommandImpl_Children.end())
	{
		map<long,string>::iterator param = (*iTD).second->m_pData->m_Parameters.find(C_DEVICEPARAMETER_DEVICE_SLAVE_TO_CONST);
		if (param != m_pCmd->m_mapDCECommandImpl_Children[TargetDevice]->m_pData->m_Parameters.end())
		{
			TargetDevice = atoi((*param).second.c_str());
			g_pDCELogger->Write(LV_STATUS, "Destination is slave device, changing target ID from %d to %d.", pMessage->m_DeviceIDTo, TargetDevice);
		}
	}

	if (pMessage->m_ID == ACTION_SEND_CODE_CONST)
	{
		int ActionNum = -1, DeviceNum = -1;
		m_pCmd->SendString("OK");

		// Is an action ID specified? 
		if (pMessage->m_Parameters.count(C_ACTIONPARAMETER_ID_CONST)>0) 
		{
			ActionNum=atoi(pMessage->m_Parameters[C_ACTIONPARAMETER_ID_CONST].c_str());
			DeviceNum=TargetDevice;
		}
		AddIRToQueue(pMessage->m_Parameters[C_ACTIONPARAMETER_ABSOLUTE_CHANNEL_CONST],pMessage->m_Parameters[C_ACTIONPARAMETER_TOKENS_CONST], 0, DeviceNum, ActionNum);
		return true;
	}
	if( pMessage->m_ID == ACTION_AV_CHANNEL_NUMBER_CONST)
	{
		m_pCmd->SendString("OK");
		AddChannelChangeToQueue(atoi(pMessage->m_Parameters[C_ACTIONPARAMETER_ABSOLUTE_CHANNEL_CONST].c_str()), TargetDevice);
		return true;
	}

	// Ok, not a system-handled action.  

	//Check the device/code map to see if we have an IR code for it.
	if (m_CodeMap.count(IntPair(TargetDevice, pMessage->m_ID))>0)
	{
		m_pCmd->SendString("OK");
		int Delay=0, Count=1;
		map<long,string>::iterator param;

		if (pMessage->m_ID == ACTION_GEN_ON_CONST || pMessage->m_ID == ACTION_GEN_OFF_CONST || pMessage->m_ID == ACTION_POWER_CONST)
		{
			printf("%d: Got power command: ", TargetDevice);
			param = m_pCmd->m_mapDCECommandImpl_Children[TargetDevice]->m_pData->m_Parameters.find(C_DEVICEPARAMETER_IRPOWERDELAY_CONST);
            if (param != m_pCmd->m_mapDCECommandImpl_Children[TargetDevice]->m_pData->m_Parameters.end())
		    {
				Delay = atoi((*param).second.c_str());
				printf("using delay of %d.\n", Delay);
			}
			else
			{
				printf("no delay found.\n");
			}
		}
		else if (pMessage->m_Parameters.count(C_ACTIONPARAMETER_PKID_C_INPUT_CONST)>0)
		{
			param = m_pCmd->m_mapDCECommandImpl_Children[TargetDevice]->m_pData->m_Parameters.find(C_DEVICEPARAMETER_IRMODEDELAY_CONST);
            if (param != m_pCmd->m_mapDCECommandImpl_Children[TargetDevice]->m_pData->m_Parameters.end())
			{
				Delay = atoi((*param).second.c_str());
				g_pDCELogger->Write(LV_STATUS, "Found mode delay of %d", Delay);
			}
		}
        if(pMessage->m_Parameters.count(C_ACTIONPARAMETER_REPEAT_CONST) > 0 && pMessage->m_Parameters[C_ACTIONPARAMETER_REPEAT_CONST].length() > 0)
		{
			Count = atoi(pMessage->m_Parameters[C_ACTIONPARAMETER_REPEAT_CONST].c_str());
			if( Count==0 )
				Count=1;
		}
		AddIRToQueue("", "", Delay, TargetDevice, pMessage->m_ID, Count);
		return true;
	}
	return false;
}

