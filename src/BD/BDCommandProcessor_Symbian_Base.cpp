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
#include "BDCommandProcessor_Symbian_Base.h"
#include "PlutoUtils/MyString.h"

#include "BDCommand.h"
#include "PlutoPhoneCommands.h"
#include "BD_HaveNothing.h"
#include "BD_WhatDoYouHave.h"
#include "PlutoUtils/PlutoDefs.h"

#include "PlutoMOApp.h"
#include "PlutoMOAppUi.h"
#include "Logger.h"

#include "BD_PC_ReportMyVersion.h"

#ifdef __SERIES60_30__
_LIT(KPlutoMODirCurrConnection,"\\private\\A1ef0015\\0000.vmc");
#else
_LIT(KPlutoMODirCurrConnection,"c:\\system\\Apps\\PlutoMO\\0000.vmc");
#endif

//----------------------------------------------------------------------------------------------
BDCommandProcessor_Symbian_Base::BDCommandProcessor_Symbian_Base
	(
		const string& sMacAddressPhone,
		MBluetoothListener* aListener
	) 
	: 
		CActive(EPriorityStandard), 
		iListener(aListener)
{ 
	iPendingKey = false;

	m_bDead = false;

	m_ReceiveCmdData=NULL;
	m_ReceiveAckData=NULL;
	m_ReceiveCmdHeader=NULL;
	m_ReceiveAckHeader=NULL;

	m_HBuf_ReceiveCmdData=NULL;
	m_HBuf_ReceiveAckData=NULL;
	m_HBuf_ReceiveCmdHeader=NULL;
	m_HBuf_ReceiveAckHeader=NULL;

	m_pCommand = NULL;
	m_pCommand_Sent=NULL;
	m_sMacAddressPhone=sMacAddressPhone;

	m_Recv_iBuf = NULL;
	m_Send_iBuf = NULL;

	m_HBuf_SendBuffer = NULL;
	iCommandTimer = NULL;

	CActiveScheduler::Add(this);
}
//----------------------------------------------------------------------------------------------
BDCommandProcessor_Symbian_Base::~BDCommandProcessor_Symbian_Base()
{
	if (iCommandTimer)
	{
		iCommandTimer->Cancel();
	}

	PLUTO_SAFE_DELETE(iCommandTimer);

	iListeningSocket.CancelAccept();

	iState = EIdle;
	iConnected = EFalse;

	iSocket.CancelAll();
	iSocket.Close();

	iSdpDatabase.Close();
    iSdpSession.Close();

	iSocketServ.Close();

	MYSTL_CLEAR_LIST(m_listCommands);

	PLUTO_SAFE_DELETE_ARRAY(m_ReceiveCmdHeader);
	PLUTO_SAFE_DELETE_ARRAY(m_ReceiveCmdData);
	PLUTO_SAFE_DELETE_ARRAY(m_ReceiveAckHeader);
	PLUTO_SAFE_DELETE_ARRAY(m_ReceiveAckData);

	PLUTO_SAFE_DELETE(m_HBuf_ReceiveCmdHeader);
	PLUTO_SAFE_DELETE(m_HBuf_ReceiveCmdData);
	PLUTO_SAFE_DELETE(m_HBuf_ReceiveAckHeader);
	PLUTO_SAFE_DELETE(m_HBuf_ReceiveAckData);

	PLUTO_SAFE_DELETE(m_HBuf_SendBuffer);
	PLUTO_SAFE_DELETE(m_Send_iBuf);
	PLUTO_SAFE_DELETE(m_Recv_iBuf);

	PLUTO_SAFE_DELETE(m_pCommand_Sent); 
	PLUTO_SAFE_DELETE(m_pCommand);

	Cancel();
}
//----------------------------------------------------------------------------------------------
void BDCommandProcessor_Symbian_Base::GotoStage(TBluetoothState state)
{
	iState = state;

	GotoNextStage();
}
//----------------------------------------------------------------------------------------------
void BDCommandProcessor_Symbian_Base::GotoNextStage()
{
	//LOG("@GotoNext@:\n");
	TRequestStatus* s = &iStatus; 
	User::RequestComplete(s, KErrNone); 
}
//----------------------------------------------------------------------------------------------
bool  BDCommandProcessor_Symbian_Base::SendData(int size, const char *data) 
{
	if(size)
	{
		iSocket.CancelWrite();

		if(NULL != m_HBuf_SendBuffer)
		{
			delete m_HBuf_SendBuffer;
			m_HBuf_SendBuffer = NULL;
		}
		
		if(NULL != m_Send_iBuf)
		{
			delete m_Send_iBuf;
			m_Send_iBuf = NULL;
		}

		m_HBuf_SendBuffer = HBufC8::NewL(size);
		m_Send_iBuf = new TPtr8(NULL, 0);
		m_Send_iBuf->Set((TUint8*)m_HBuf_SendBuffer->Ptr(), 0, size);

		for (int i = 0; i < size; i++)
			m_Send_iBuf->Append(TUint8(data[i]));

		iSocket.Write(*m_Send_iBuf, iStatus);

		//LOG(">>> Sending: ");
		//LOGN(size);
		//LOGN(" bytes\n");
	}
	else //nothing to send ... just go to the next stage
	{
		GotoNextStage();
	}

	return true;
}
//----------------------------------------------------------------------------------------------
bool BDCommandProcessor_Symbian_Base::SendLong(long l)
{
	//LOG("@ About to send a long  with value : ");
	//LOGN(l);
	//LOGN(" \n");

	SendData(4,(const char *)&l);

	return true;
}
//----------------------------------------------------------------------------------------------
void  BDCommandProcessor_Symbian_Base::Listen() 
{
	iCommandTimer = CPeriodic::NewL(EPriorityStandard);

	User::LeaveIfError( iListeningSocket.Listen( KListeningQueSize ) );
	iListeningSocket.Accept( iSocket, iStatus );
	iState = EAccepting;

	SetActive();
}
//----------------------------------------------------------------------------------------------
void  BDCommandProcessor_Symbian_Base::SetupSecurityManager()
{
	// a connection to the security manager
	RBTMan secManager;

	// a security session
	TBTServiceSecurity secSettingsSession;

	// define the security on this port
	//User::LeaveIfError(secManager.Connect());
	//CleanupClosePushL(secManager);
	//User::LeaveIfError(secSettingsSession.Open(secManager));
	//CleanupClosePushL(secSettingsSession);

	// the security settings 
	// TBTServiceSecurity serviceSecurity( KUidPlutoMO, KSolBtRFCOMM, 0 );
	TBTServiceSecurity serviceSecurity;

	//Define security requirements
	serviceSecurity.SetUid(KUidPlutoMO);    
	serviceSecurity.SetAuthentication(false);    
	serviceSecurity.SetEncryption(false); 
	serviceSecurity.SetAuthorisation(false);

	// serviceSecurity.SetChannelID(KPlutoMOPort );
	TRequestStatus status;
	// secSettingsSession.RegisterService(serviceSecurity, status);

	//User::WaitForRequest( status ); // wait until the security settings are set
	//User::LeaveIfError( status.Int() );

	//CleanupStack::PopAndDestroy();  //  secManager
	//CleanupStack::PopAndDestroy();  //  secSettingsSession

}
//----------------------------------------------------------------------------------------------
void BDCommandProcessor_Symbian_Base::AdvertiseThePlutoService()
{
	 TSdpServRecordHandle recHandle;
	 CSdpAttrValue* attrVal = 0;
	 CSdpAttrValueDES* attrValDES = 0;
 
	 // Set Attr 1 (service class list) to list with UUID = 0x1101 (serial port)
	 TUUID sppUid = TUUID(TUint32(0x1101));
	 iSdpDatabase.CreateServiceRecordL(sppUid, recHandle);
 
	 // Set Attr 2 (service record state) to 0
	 TBuf8<4> recordState;
	 recordState.FillZ(4);
	 attrVal = CSdpAttrValueUint::NewUintL(recordState);
	 CleanupStack::PushL(attrVal);
	 iSdpDatabase.UpdateAttributeL(recHandle, KSdpAttrIdServiceRecordState, *attrVal);
	 CleanupStack::PopAndDestroy();
	 attrVal = 0;


	 // the port number in the protocol should be a TUint8, not a 4 byte UInt 
	 TBuf8<1> portNumber;
	 portNumber.SetLength(1);
	 portNumber[0] = (TUint8)KPlutoMOPort ;
	 attrValDES = CSdpAttrValueDES::NewDESL(0);
	 CleanupStack::PushL(attrValDES);
	 attrValDES
	  ->StartListL()
	   ->BuildDESL()
	   ->StartListL()
		->BuildUUIDL(TUUID(TUint16(0x0100))) // L2CAP
	   ->EndListL()
	   ->BuildDESL()
	   ->StartListL()
		->BuildUUIDL(TUUID(TUint16(0x0003))) // RFCOMM
		->BuildUintL(portNumber) // the port number
	   ->EndListL()
	  ->EndListL();
	 iSdpDatabase.UpdateAttributeL(recHandle, KSdpAttrIdProtocolDescriptorList, *attrValDES);
	 CleanupStack::PopAndDestroy();
	 attrValDES = 0;
 
	 // Set Attr 5 (browse group list) to list with one UUID 0x1002 (public browse group)
	 //
	 attrValDES = CSdpAttrValueDES::NewDESL(0);
	 CleanupStack::PushL(attrValDES);
	 attrValDES
	  ->StartListL()
	   ->BuildUUIDL(TUUID(TUint32(0x1002))) // Public browse group
	  ->EndListL();
	 iSdpDatabase.UpdateAttributeL(recHandle, KSdpAttrIdBrowseGroupList, *attrValDES);
	 CleanupStack::PopAndDestroy();
	 attrValDES = 0;
 

	 // Set Attr 0x006 (language base)
	 TBuf8<2> langId;
	 langId.FillZ(2);
	 langId[0] = 0x65;
	 langId[1] = 0x6e;
 
	 TBuf8<2> charEnc;
	 charEnc.FillZ(2);
	 charEnc[1] = 0x6a;
 
	 TBuf8<2> baseAttrId;
	 baseAttrId.FillZ(2);
	 baseAttrId[0] = 0x01;
 
	 attrValDES = CSdpAttrValueDES::NewDESL(0);
	 CleanupStack::PushL(attrValDES);
	 attrValDES
	  ->StartListL()
	   ->BuildUintL(langId) // english
	   ->BuildUintL(charEnc) // UTF-8
	   ->BuildUintL(baseAttrId) // language base
	  ->EndListL();
	 iSdpDatabase.UpdateAttributeL(recHandle, KSdpAttrIdLanguageBaseAttributeIDList, *attrValDES);
	 CleanupStack::PopAndDestroy();
	 attrValDES = 0;
 
	 // Set Attr 0x007 (time to live) to 1200 (0x4B0) seconds (20 minutes)
	 TBuf8<4> timeToLive;
	 timeToLive.FillZ(4);
	 timeToLive[2]=4;
	 timeToLive[3]=0xb0;
	 attrVal = CSdpAttrValueUint::NewUintL(timeToLive);
	 CleanupStack::PushL(attrVal);
	 iSdpDatabase.UpdateAttributeL(recHandle, KSdpAttrIdServiceInfoTimeToLive, *attrVal);
	 CleanupStack::PopAndDestroy();
	 attrVal = 0;
 
	 // Set Attr 0x008 (availability) to 0xff - fully available
	 TBuf8<1> avail;
	 avail.FillZ(1);
	 avail[0]=0xff;
	 attrVal = CSdpAttrValueUint::NewUintL(avail);
	 CleanupStack::PushL(attrVal);
	 iSdpDatabase.UpdateAttributeL(recHandle, KSdpAttrIdServiceAvailability, *attrVal);
	 CleanupStack::PopAndDestroy();
	 attrVal = 0;
 
	 // Set Attr 0x100 (default Name) to string
	 attrVal = CSdpAttrValueString::NewStringL(KServiceName);
	 CleanupStack::PushL(attrVal);
	 iSdpDatabase.UpdateAttributeL(recHandle, KSdpAttrIdBasePrimaryLanguage+KSdpAttrIdOffsetServiceName, *attrVal);
	 CleanupStack::PopAndDestroy();
	 attrVal = 0;
 
	 // Set Attr 0x101 (def. description) to string
	 attrVal = CSdpAttrValueString::NewStringL(KServiceName);
	 CleanupStack::PushL(attrVal);
	 iSdpDatabase.UpdateAttributeL(recHandle, KSdpAttrIdBasePrimaryLanguage+KSdpAttrIdOffsetServiceDescription, *attrVal);
	 CleanupStack::PopAndDestroy();
	 attrVal = 0;

	 
	 // Set Attr 0x102 (def. provider) to Intuwave
	 attrVal = CSdpAttrValueString::NewStringL(KServiceDescription);
	 CleanupStack::PushL(attrVal);
	 iSdpDatabase.UpdateAttributeL(recHandle, 0x0102, *attrVal);
	 CleanupStack::PopAndDestroy();
	 attrVal = 0;
}
//----------------------------------------------------------------------------------------------
void  BDCommandProcessor_Symbian_Base::RunL() 
{
	//LOGN("*");

	if (iStatus != KErrNone || iState == EConnectionLost)
	{
		//LOG("Disconnected!\n");
		//iSocket.CancelAll();
		iSocket.Close();

		/*
		LOG("About to delete timer\n");
		if (iCommandTimer->IsActive())
		{
			iCommandTimer->Cancel();

			delete iCommandTimer;
			iCommandTimer = NULL;

			LOG("timed deleted\n");
		}
		*/

		if(iStatus != KErrNone)
		{
			LOG(" status : error");
		}
		else
		{
			LOG(" or EConnectionLost");
		}

		((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_bMakeVisibleAllowed = false;
		((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->ResetViewer();

		MYSTL_CLEAR_LIST(m_listCommands);

		iConnected = false;
		User::LeaveIfError( iSocket.Open( iSocketServ ) );
		iListeningSocket.Accept( iSocket, iStatus );
		iState = EAccepting;
		SetActive();

		LOG("About to hide\n");
		iListener->Hide();

		LOG("Waiting for new connections...\n");
		//TODO: delete "connection.vmc"

		/*
		RFile file;
		RFs   aFs;
		aFs.Connect();
		CFileMan* fileMan = CFileMan::NewL(aFs);
		CleanupStack::PushL(fileMan); 
		string FilesPath(KPlutoMODirCurrConnection);
		fileMan->Delete(FilesPath.Des());
		CleanupStack::PopAndDestroy();
		aFs.Close();
		*/
		return;
	}

	
	//if(iState == EIdle)
	//	LOG("IDLE\n"); 

	//if(iPendingKey)
	//	LOG("KEYPENDING\n");

	if(
		iPendingKey							&& 
		(
			iState == ERecvCommand_End  ||
			iState == EIdle				||
			iState == ESendingCommand
		)
	)
	{
		//LOG("PENDINGKEY-OVER");
		iPendingKey = false;
		iState = ESendingCommand;
	}

	//LOGN("-");
	//LOGN(iState);
	//LOGN(",");

	if(m_iTimedOut)
	{
		LOG("Reseting timeout... state :");
		m_iTimedOut = 0; //no timed-out
		m_bStatusOk = true;
	}

	switch (iState)
	{
	case EAccepting:
		{
			m_iTimedOut = 0; //no timed-out
			m_bStatusOk = true;
			//LOG("EAccepting\n");
			iConnected = true;

			if (iCommandTimer->IsActive())
			{
				iCommandTimer->Cancel();

				delete iCommandTimer;
				iCommandTimer = NULL;

				iCommandTimer = CPeriodic::NewL(EPriorityIdle);
			}

			iCommandTimer->Start(100000, 1000000,
				TCallBack(CommandTimerCallBack, this)); 

			AddCommand(new BD_PC_ReportMyVersion(string(VERSION)));

			iState = EIdle;

			/*
			//create a dummy vmc file
			RFile file;
			RFs   aFs;
			aFs.Connect();
			string FilesPath(KPlutoMODirCurrConnection);
			if(KErrNotFound == file.Open(aFs, FilesPath.Des(), EFileStream | EFileWrite))
			{
				file.Create(aFs, FilesPath.Des(), EFileStream | EFileWrite);
			}
			file.Close();
			aFs.Close();
			*/

		}
		break;

	case ESendingCommand:
		{
			if(m_iTimedOut)
			{
				LOG("Reseting timeout... state :");
				m_iTimedOut = 0; //no timed-out
				m_bStatusOk = true;
			}

			//LOG("ESendingCommand\n");
			m_bImmediateCallback = false;

  			if( MYSTL_SIZEOF_LIST(m_listCommands)==0 )
			{
				//LOG("LIST IS EMPTY\n");
				m_pCommand_Sent = new BD_WhatDoYouHave();
			}
			else
			{
				//LOG("LIST IS NOT EMPTY\n");
				//int size = MYSTL_SIZEOF_LIST(m_listCommands);
				//LOG(size);
				MYSTL_EXTRACT_FIRST(m_listCommands, BDCommand, pC);
				m_pCommand_Sent = pC;
			}

			m_pCommand_Sent->ConvertCommandToBinary();
			SendLong(m_pCommand_Sent->ID());

			iState = ESendingCommandId;
			SetActive();
		}
		break;

	case ESendingCommandId:
		{
			if(m_iTimedOut)
			{
				LOG("Reseting timeout... state :");
				m_iTimedOut = 0; //no timed-out
				m_bStatusOk = true;
			}

			//LOG("ESendingCommandId\n");
			SendLong(m_pCommand_Sent->GetCommandOrAckSize());
			iState = ESendingCommandOrAckSize;
			SetActive();
		}
		break;

	case ESendingCommandOrAckSize:
		{
			if(m_iTimedOut)
			{
				LOG("Reseting timeout... state :");
				m_iTimedOut = 0; //no timed-out
				m_bStatusOk = true;
			}

			//LOG("ESendingCommandOrAckSize\n");
			SendData(m_pCommand_Sent->GetCommandOrAckSize(),m_pCommand_Sent->GetCommandOrAckData());
			iState = ESendingCommandOrAckData;
			SetActive();
		}
		break;

	case ESendingCommandOrAckData:
		{
			if(m_iTimedOut)
			{
				LOG("Reseting timeout... state :");
				m_iTimedOut = 0; //no timed-out
				m_bStatusOk = true;
			}

			//LOG("ESendingCommandOrAckData\n");
			int AckHeaderSize;
			if( m_pCommand_Sent->ID() == BD_PC_WHAT_DO_YOU_HAVE )
				AckHeaderSize = 8; 
			else
				AckHeaderSize = 4;

			PLUTO_SAFE_DELETE_ARRAY(m_ReceiveAckHeader);
			PLUTO_SAFE_DELETE(m_HBuf_ReceiveAckHeader);
			PLUTO_SAFE_DELETE(m_Recv_iBuf);

			m_iRecvSize = AckHeaderSize;
			m_HBuf_ReceiveAckHeader = HBufC8::NewL(m_iRecvSize);
			
			m_Recv_iBuf = new TPtr8(NULL, 0); 
			m_Recv_iBuf->Set((TUint8*)m_HBuf_ReceiveAckHeader->Ptr(), 0, m_iRecvSize); 
			iSocket.Read(*m_Recv_iBuf, iStatus);

			iState = EReceivingAckHeader;
			m_pCommand_Sent->FreeSerializeMemory();

			SetActive();
		}
		break;

	case EReceivingAckHeader:
		{
			if(m_iTimedOut)
			{
				LOG("Reseting timeout... state :");
				m_iTimedOut = 0; //no timed-out
				m_bStatusOk = true;
			}
			//LOG("EReceivingAckHeader\n");

			PLUTO_SAFE_DELETE_ARRAY(m_ReceiveAckHeader);
			m_ReceiveAckHeader = new char[m_iRecvSize];
			for (int i = 0; i < m_iRecvSize; i++) 
				m_ReceiveAckHeader[i] = (char)(*m_Recv_iBuf)[i];

			//LOG("<<< Received: ");
			//LOGN(m_iRecvSize);
			//LOGN(" bytes\n");

			long *lSize;
			if( m_pCommand_Sent->ID()==BD_PC_WHAT_DO_YOU_HAVE )
				lSize = (long *) (m_ReceiveAckHeader + 4);
			else
				lSize = (long *) (m_ReceiveAckHeader);

			if( *lSize )
			{
				PLUTO_SAFE_DELETE(m_HBuf_ReceiveAckData);
				PLUTO_SAFE_DELETE_ARRAY(m_ReceiveAckData);

				m_iRecvSize = *lSize;
				m_bStartRecv = true;

				GotoStage(EReceivingAckData_Loop);
			}
			else
				GotoStage(EReceivingAckData);

			SetActive();
		}
		break;


	case EReceivingAckData_Loop:
		{
			if(m_iTimedOut)
			{
				LOG("Reseting timeout... state :");
				m_iTimedOut = 0; //no timed-out
				m_bStatusOk = true;
			}

			//LOG("EReceivingAckData_Loop\n");
			if(m_bStartRecv)
			{
				m_bStartRecv = false;

				PLUTO_SAFE_DELETE(m_HBuf_ReceiveAckData);
				PLUTO_SAFE_DELETE_ARRAY(m_ReceiveAckData);
				PLUTO_SAFE_DELETE(m_Recv_iBuf);

				m_HBuf_ReceiveAckData = HBufC8::NewL(m_iRecvSize);
				m_ReceiveAckData = new char[m_iRecvSize];
				
				m_Recv_iBuf = new TPtr8(NULL, 0); 
				m_Recv_iBuf->Set((TUint8*)m_HBuf_ReceiveAckData->Ptr(), 0, m_iRecvSize); 
				iSocket.Read(*m_Recv_iBuf, iStatus);

				iState = EReceivingAckData_Loop;
			}
			else
			{
				for (int i = 0; i < m_iRecvSize; i++) 
					m_ReceiveAckData[i] = (char)(*m_Recv_iBuf)[i];

				//LOG("<<< Received: ");
				//LOGN(m_iRecvSize);
				//LOGN(" bytes\n");

				GotoStage(EReceivingAckData);
			}

			SetActive();
		}
		break;

	case EReceivingAckData:
		{
			if(m_iTimedOut)
			{
				LOG("Reseting timeout... state :");
				m_iTimedOut = 0; //no timed-out
				m_bStatusOk = true;
			}

			//LOG("EReceivingAckData\n");
			if( m_pCommand_Sent->ID() == BD_PC_WHAT_DO_YOU_HAVE )
			{
				iStatus = KErrNone;
				iState = EReceivingCommand;

				PLUTO_SAFE_DELETE(m_pCommand_Sent);
			}
			else
			{
				long *lSize = (long *) (m_ReceiveAckHeader);
				m_pCommand_Sent->ParseAck(*lSize, m_ReceiveAckData);

				PLUTO_SAFE_DELETE(m_pCommand_Sent);

				iState = EIdle;
				break;
			}
		}

	case EReceivingCommand:
		{
			if(m_iTimedOut)
			{
				LOG("Reseting timeout... state :");
				m_iTimedOut = 0; //no timed-out
				m_bStatusOk = true;
			}

			//LOG("EReceivingCommand\n");
			// The Ack will be a command that's parsed directly
			long *lType = (long *) m_ReceiveAckHeader;
			long *lSize = (long *) (m_ReceiveAckHeader + 4);

			if(*lType)
			{
				m_pCommand = BuildCommandFromData(*lType);

				//LOG("About to parse command. Type:");
				//LOG(*lType);
				m_pCommand->ParseCommand(*lSize, m_ReceiveAckData);
				m_pCommand->ProcessCommand(NULL);
				m_pCommand->ConvertAckToBinary();

				iState = ERecvCommand_ReadyToSendAck;
				//no break, continue
			}
			else
			{
				PLUTO_SAFE_DELETE_ARRAY(m_ReceiveCmdHeader);
				PLUTO_SAFE_DELETE(m_HBuf_ReceiveCmdHeader);
				PLUTO_SAFE_DELETE(m_Recv_iBuf);

				m_iRecvSize = 8;
				m_HBuf_ReceiveCmdHeader = HBufC8::NewL(m_iRecvSize);
				
				m_Recv_iBuf = new TPtr8(NULL, 0); 
				m_Recv_iBuf->Set((TUint8*)m_HBuf_ReceiveCmdHeader->Ptr(), 0, m_iRecvSize); 
				iSocket.Read(*m_Recv_iBuf, iStatus);

				iState = EReceivingCmdHeader;
				SetActive();
				break;
			}
		}

	case ERecvCommand_ReadyToSendAck:
		{
			if(m_iTimedOut)
			{
				LOG("Reseting timeout... state :");
				m_iTimedOut = 0; //no timed-out
				m_bStatusOk = true;
			}

			//LOG("ERecvCommand_ReadyToSendAck\n");
			SendLong(m_pCommand->GetCommandOrAckSize());
			iState = ERecvCommand_SendingCommandOrAckSize;
			SetActive();
		}
		break;

	case ERecvCommand_SendingCommandOrAckSize:
		{
			if(m_iTimedOut)
			{
				LOG("Reseting timeout... state :");
				m_iTimedOut = 0; //no timed-out
				m_bStatusOk = true;
			}

			//LOG("ERecvCommand_SendingCommandOrAckSize\n");
			SendData(m_pCommand->GetCommandOrAckSize(),m_pCommand->GetCommandOrAckData());
			iState = ERecvCommand_SendingCommandOrAckData;
			SetActive();
		}
		break;

	case ERecvCommand_SendingCommandOrAckData:
		{
			if(m_iTimedOut)
			{
				LOG("Reseting timeout... state :");
				m_iTimedOut = 0; //no timed-out
				m_bStatusOk = true;
			}

			//LOG("ERecvCommand_SendingCommandOrAckData\n");
			m_pCommand->FreeSerializeMemory();
			PLUTO_SAFE_DELETE(m_pCommand);

			iState = ERecvCommand_End;

			//no break, continue
		}

	case ERecvCommand_End:
		{
			if(m_iTimedOut)
			{
				LOG("Reseting timeout... state :");
				m_iTimedOut = 0; //no timed-out
				m_bStatusOk = true;
			}

			//LOG("<<<ERecvCommand_End>>>\n");

			long *lType = (long *) m_ReceiveAckHeader;

			if( *lType != BD_CP_HAVE_NOTHING )
			{
				GotoStage(ESendingCommand);
				SetActive();
			}
			else
				iState = EIdle;
		}
		break;

	case EReceivingCmdHeader:
		{
			if(m_iTimedOut)
			{
				LOG("Reseting timeout... state :");
				m_iTimedOut = 0; //no timed-out
				m_bStatusOk = true;
			}

			//LOG("EReceivingCmdHeader\n");

			PLUTO_SAFE_DELETE_ARRAY(m_ReceiveCmdHeader);
			m_ReceiveCmdHeader = new char[m_iRecvSize];
			for (int i = 0; i < m_iRecvSize; i++) 
				m_ReceiveCmdHeader[i] = (char)(*m_Recv_iBuf)[i];

			//LOG("<<< Received: ");
			//LOGN(m_iRecvSize);
			//LOGN(" bytes\n");
			
			long *lSize = (long *) (m_ReceiveCmdHeader + 4);

			if( *lSize )
			{

				PLUTO_SAFE_DELETE_ARRAY(m_ReceiveCmdData);
				PLUTO_SAFE_DELETE(m_HBuf_ReceiveCmdData);
				PLUTO_SAFE_DELETE(m_Recv_iBuf);

				m_iRecvSize = 8;
				m_HBuf_ReceiveCmdData = HBufC8::NewL(m_iRecvSize);
				
				m_Recv_iBuf = new TPtr8(NULL, 0); 
				m_Recv_iBuf->Set((TUint8*)m_HBuf_ReceiveCmdData->Ptr(), 0, m_iRecvSize); 
				iSocket.Read(*m_Recv_iBuf, iStatus);

				iState = EReceivingCmdData;
			}
			else
				GotoStage(EReceivingCommand_BuildCommand);

			SetActive();
		}
		break;

	case EReceivingCmdData:
		{
			if(m_iTimedOut)
			{
				LOG("Reseting timeout... state :");
				m_iTimedOut = 0; //no timed-out
				m_bStatusOk = true;
			}
			//LOG("EReceivingCmdData\n");

			PLUTO_SAFE_DELETE_ARRAY(m_ReceiveCmdData);
			m_ReceiveCmdData = new char[m_iRecvSize];
			for (int i = 0; i < m_iRecvSize; i++) 
				m_ReceiveCmdData[i] = (char)(*m_Recv_iBuf)[i];

			//LOG("<<< Received: ");
			//LOGN(m_iRecvSize);
			//LOGN(" bytes\n");

			GotoStage(EReceivingCommand_BuildCommand);
			SetActive();
		}
		break;

	case EReceivingCommand_BuildCommand:
		{
			if(m_iTimedOut)
			{
				LOG("Reseting timeout... state :");
				m_iTimedOut = 0; //no timed-out
				m_bStatusOk = true;
			}

			//LOG("EReceivingCommand_BuildCommand\n");
			long *lType = (long *) m_ReceiveCmdHeader;
			long *lSize = (long *) (m_ReceiveCmdHeader+4);
			m_pCommand = BuildCommandFromData(*lType);
			if( m_pCommand->ID() == BD_PC_WHAT_DO_YOU_HAVE )
			{
				GotoStage(ESendingCommand);
				//iState = ESendingCommand;
			}
			else
			{
				m_pCommand->ParseCommand(*lSize,m_ReceiveCmdData);
				m_pCommand->ProcessCommand(NULL);
				m_pCommand->ConvertAckToBinary();

				SendLong(m_pCommand->GetCommandOrAckSize());
				iState = ERecvCommand_SendingCommandOrAckSize_Step2;
				SetActive();
			}
		}
		break;

	case ERecvCommand_SendingCommandOrAckSize_Step2:
		{
			if(m_iTimedOut)
			{
				LOG("Reseting timeout... state :");
				m_iTimedOut = 0; //no timed-out
				m_bStatusOk = true;
			}

			//LOG("ERecvCommand_SendingCommandOrAckSize_Step2\n");
			SendData(m_pCommand->GetCommandOrAckSize(), m_pCommand->GetCommandOrAckData());
			iState = ERecvCommand_SendingCommandOrAckData_Step2;
			SetActive();
		}
		break;

	case ERecvCommand_SendingCommandOrAckData_Step2:
		{
			if(m_iTimedOut)
			{
				LOG("Reseting timeout... state :");
				m_iTimedOut = 0; //no timed-out
				m_bStatusOk = true;
			}

			//LOG("ERecvCommand_SendingCommandOrAckData_Step2\n");
			m_pCommand->FreeSerializeMemory();

			long *lType = (long *) m_ReceiveCmdHeader;

			if( *lType != BD_CP_HAVE_NOTHING )
			{
				GotoStage(ESendingCommand);
				SetActive();
			}
			else
				iState = EIdle;
		}

	default:
		{
		LOG("Unexpected Bluetooth state\n");
		TBuf<256> formatBuf;
		formatBuf.Format(_L("Unexpected Bluetooth state %d"), iState);
		CEikonEnv::Static()->InfoWinL(KNullDesC, formatBuf);
		}
		break;
		}

}
//----------------------------------------------------------------------------------------------
void  BDCommandProcessor_Symbian_Base::Close() 
{
}
//----------------------------------------------------------------------------------------------
void BDCommandProcessor_Symbian_Base::Disconnect()
{
	iSocket.CancelAll();
	iState = EConnectionLost;
}
//---------------------------------------------------------------------------------------------
void BDCommandProcessor_Symbian_Base::ProcessCommands(bool bCriticalRequest /*=true*/)
{
	if(iConnected)
	{
		if(!m_bStatusOk)
		{
			//it's seems that in last x seconds, we had only EIdle state
			//connection lost!
			LOG("Time's up!\n");

			iSocket.CancelAll();
			iState = EConnectionLost;
			return;
		}

		if(bCriticalRequest && IsActive()) //the object is already active.
		{
			//LOG("ACTIVE-KEY\n"); 
			//LOG("Key was press, but the object is still active. Will just set a flag\n"); 
			//GotoStage(ESendingCommand); //don't force object activation
			iPendingKey = true;
			return;
		}

		if(iState == EIdle || bCriticalRequest)
		{
			//if(bCriticalRequest)
			//	LOG("KEY\n");

			GotoStage(ESendingCommand);
			SetActive();
			return;
		}

		//on timer tick, we found an EIdle state. what if the connection was lost?
		m_iTimedOut++;

		LOG("@ Inactivity : ");
		LOGN(m_iTimedOut);
		LOGN(" \n");

		if(m_iTimedOut >= 20) //20 secs
		{
			m_bStatusOk = false;
			return;
		}
	}
}
//----------------------------------------------------------------------------------------------
TInt BDCommandProcessor_Symbian_Base::CommandTimerCallBack(TAny* aBluetoothHandler)
{
	//LOG("$$$\n");

	BDCommandProcessor_Symbian_Base* pProcessor = 
		(BDCommandProcessor_Symbian_Base*)aBluetoothHandler;

	pProcessor->ProcessCommands(false);

	return 0;
}
//----------------------------------------------------------------------------------------------
void BDCommandProcessor_Symbian_Base::Connect()
{
	SetActive();
}
//----------------------------------------------------------------------------------------------
TInt BDCommandProcessor_Symbian_Base::RunError(TInt aError)
{
	TBuf<256> formatBuf;
	if (aError != KErrNone)
	{
		formatBuf.Format(_L("Unexpected error %d in state %d"), aError, iState);
		CEikonEnv::Static()->InfoWinL(formatBuf, KNullDesC);
	}
	return KErrNone;
}
//----------------------------------------------------------------------------------------------
void BDCommandProcessor_Symbian_Base::DoCancel()
{
}
//----------------------------------------------------------------------------------------------
void BDCommandProcessor_Symbian_Base::AddCommand(class BDCommand *pCommand)
{
	MYSTL_ADDTO_LIST(m_listCommands,pCommand);
}
//----------------------------------------------------------------------------------------------
