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
#define DEFAULT_PREDELAY 100
#define DEFAULT_POSTDELAY 0

this_file_isnt_used

class IRQueue
{
public:
	IRQueue(string Port, string IRCode, int Delay=0, int DeviceNum=-1, int ActionNum=-1, int StepNumber=0, int PreDelay=0, int ChannelSequenceNumber=0)
	{		
		m_Port = Port;
		m_IRCode = IRCode;
		m_PreDelay = PreDelay;
		m_Delay = Delay;
		m_DeviceNum = DeviceNum;
		m_ActionNum = ActionNum;
		m_StepNumber = StepNumber;
		m_PreDelay = PreDelay;
		m_ChannelSequenceNumber = ChannelSequenceNumber;
	}
	string m_Port, m_IRCode;
	int m_PreDelay, m_Delay, m_DeviceNum, m_ActionNum, m_StepNumber, m_ChannelSequenceNumber;
};

using namespace ::std;

#define MS_TO_CLK(x) ((x * CLK_TCK) / 1000)

class DCECommand_Impl;
class IRBase
{
public:
	DCECommand_Impl *m_pCmd;

	IRBase() : m_IRMutex("ir")
	{
		// Grab the command so we can look at device parameters.
		m_pCmd = NULL;
	}
	virtual ~IRBase();

	void Init(DCECommand_Impl *pCommand);

protected:
	my_pthread_mutex_t m_IRMutex;

	virtual void SendIR(string Port, string IRCode)=0;
	
	bool AddChannelChangeToQueue(int ChannelNumber, int Device);
	void AddIRToQueue(string Port, string IRCode, int Delay=0, int DeviceNum=-1, int ActionNum=-1, int Count=1, int ChannelSequenceNumber = 0);
	bool ProcessQueue();
	void ParseDevices();
	bool ProcessDCEMessage(class DCEMessage *pMessage);

	typedef pair<int,int> IntPair;
	map <IntPair, string> m_CodeMap;		   //maps from pin # to event object
	map <int, string> m_DevicePortStringMap;   // map deviceIDs to their full port names
	map <int, clock_t> m_DeviceNextAvailableTime;
	map <string, clock_t> m_PortLastIRSent;
	map <int, clock_t> m_DevicePreDelays;
	map <int, clock_t> m_DeviceDefaultDelays;
	map <int, int> m_ExecutingChannelSequenceNumber;
	list<IRQueue *> m_IRQueue;
	int m_CurrentChannelSequenceNumber;
};

