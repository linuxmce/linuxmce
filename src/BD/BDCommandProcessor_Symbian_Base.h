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
#ifndef __BDCommandProcessor_Symbian_Base_h
#define __BDCommandProcessor_Symbian_Base_h

#include <es_sock.h>
#include <bt_sock.h>
#include <btextnotifiers.h>
#include <btmanclient.h>
#include <btsdp.h>
#include <eikprogi.h>
#include <aknwaitdialog.h>
#include <aknprogressdialog.h> 

#include "VIPShared/VIPIncludes.h"
#include "VIPMenu.h"

#define BD_PC_WHAT_DO_YOU_HAVE			1
#define BD_CP_HAVE_NOTHING				100
//----------------------------------------------------------------------------------------------
#define KServiceName _L8("Pluto")
#define KServiceDescription _L8("PlutoInc")

const TInt KPlutoMOPort = 19;
const TInt KSerialPortService = 0x1101;

_LIT( KServerTransportName, "RFCOMM" );
static const TInt KListeningQueSize = 4;
//----------------------------------------------------------------------------------------------
class MBluetoothListener
{
public:
	virtual void UpdateScreen(bool bParsed, const TDes8& aVmc, unsigned int uSize, 
								 VIPMenuCollection *pVMC) = 0;
	virtual void UpdateScreen(	
		bool bStore, 
		unsigned long iVMCSize, 
		const char* pVMC,
		unsigned long iVMCFileNameSize, 
		const char* pVMCFileName
	) = 0;

	virtual void Hide() = 0;
	virtual void Show() = 0;
	virtual CCoeEnv* CoeEnv() = 0;
};
//----------------------------------------------------------------------------------------------
class BDCommandProcessor_Symbian_Base: public CActive 
{

public:

	enum TBluetoothState 
	{
		EError, //0
		EIdle, //1
		EAccepting,//2

		ESendingCommand,//3

		ESendingCommandId,//4
		ESendingCommandOrAckSize,//5
		ESendingCommandOrAckData,//6
		ESendingCommandOrAckDataAndFinish,//7

		EReceivingAckHeader,//8
		EReceivingAckData_Loop,//9
		EReceivingAckData,//10

		EReceivingCommand,//11

		ERecvCommand_ReadyToSendAck,//12
		ERecvCommand_SendingCommandOrAckSize,//13
		ERecvCommand_SendingCommandOrAckData,//14

		EReceivingCmdHeader,//15
		EReceivingCmdData,//16

		EReceivingCommand_BuildCommand,//17

		ERecvCommand_SendingCommandOrAckSize_Step2,//18
		ERecvCommand_SendingCommandOrAckData_Step2,//19

		ERecvCommand_End,//20

		EConnectionLost//21
	};

	virtual void Connect();
	virtual void Disconnect();

private: // Inherited from CActive.
	
	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);

private: // New functions.

	static TInt CommandTimerCallBack(TAny* aBluetoothHandler);

	inline void GotoStage(TBluetoothState state);
	inline void GotoNextStage();

protected:

	MBluetoothListener* iListener;
	CPeriodic* iCommandTimer;

	RSocketServ iSocketServ;
	RSocket iListeningSocket;
	RSocket iSocket;

	TBool iConnected;
	TBluetoothState iState;

	RSdp iSdpSession;
	RSdpDatabase iSdpDatabase;
	RNotifier iNotifier;

	//bdcommandprocessor 
	MYSTL_CREATE_LIST(m_listCommands, BDCommand);

	TBool m_bImmediateCallback;
	TBool m_bDead;

	char  *m_ReceiveCmdHeader,*m_ReceiveCmdData;
	char  *m_ReceiveAckHeader,*m_ReceiveAckData;

	HBufC8  *m_HBuf_ReceiveCmdHeader,*m_HBuf_ReceiveCmdData;
	HBufC8  *m_HBuf_ReceiveAckHeader,*m_HBuf_ReceiveAckData;

	HBufC8  *m_HBuf_SendBuffer;
	TPtr8   *m_Send_iBuf;
	TPtr8   *m_Recv_iBuf;
	int		m_iRecvSize;

	bool	m_bStartRecv;

	string m_sMacAddressPhone;
	class BDCommand *m_pCommand_Sent;  // The command we're in the process of sending
	class BDCommand *m_pCommand;

	bool iPendingKey;

public:

	BDCommandProcessor_Symbian_Base(const string& sMacAddressPhone, MBluetoothListener* aListener);
	virtual ~BDCommandProcessor_Symbian_Base();

	virtual void Start() = 0;
	virtual void Listen();
	virtual void SetupSecurityManager();
	virtual void AdvertiseThePlutoService();

	virtual bool SendData(int size,const char *data);
	virtual bool SendLong(long l);
	virtual void Close();

	void AddCommand(class BDCommand *pCommand);

	void ProcessCommands(bool bCriticalRequest = true);

	bool m_bStatusOk;
	int  m_iTimedOut;
};
//----------------------------------------------------------------------------------------------
#endif
