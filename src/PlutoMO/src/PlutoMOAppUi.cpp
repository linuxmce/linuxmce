/*
 PlutoMOAppUI
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

//----------------------------------------------------------------------------------------------
#include <avkon.hrh>
#include <uri8.h>
#include <e32base.h>
#include <http.h>
#include <chttpformencoder.h>
#include <http/rhttpsession.h>
#include <http/mhttptransactioncallback.h>
#include "eikmenub.h"
#include <aknnotewrappers.h> 
#include <aknglobalconfirmationquery.h>
//----------------------------------------------------------------------------------------------
#include "PlutoMO.hrh"
//----------------------------------------------------------------------------------------------
#include "PlutoMOAppUi.h"
#include "PlutoMOContainer.h" 
#include "PlutoVMCContainer.h"
#include "PlutoVMCUtil.h"
#include "PlutoVMCView.h"
#include "Logger.h"
#include "PlutoDefs.h"
#include "PlutoVMCView.h"
#include "GetCallerID.h"
#include "NotifyIncomingCall.h"
#include "LineReader.h"
#include "PlutoEventView.h"
#include "PlutoEventContainer.h"
//----------------------------------------------------------------------------------------------
//test - not used
#include "HttpClient.h"
//----------------------------------------------------------------------------------------------
char CPlutoMOAppUi::GetCurrentDrive()
{
	bool bRes = false;
	char cDrive = 'c';

	TUint attr;
	RFs iFsSession;

	User::LeaveIfError(iFsSession.Connect()); 
	string sApplicationFilePath = "x:\\system\\Apps\\PlutoMO\\PlutoMO.app";

	//search for app folder
	for(cDrive = 'c'; cDrive <= 'z'; cDrive++)
	{
		attr = 0;
		sApplicationFilePath.SetAt(cDrive, 0);

		if(KErrNone == iFsSession.Att(sApplicationFilePath.Des(), attr))
			break;
	}

	iFsSession.Close();

	return cDrive;
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::SetupPaths()
{
	//initializing default paths
#ifdef __SERIES60_30__
	m_sAppFolder		= "\\private\\";
	m_sLoggerFileName	= "\\private\\\\PlutoMO.log";
	m_sVMCFolderFilter	= "\\private\\\\*.vmc";
	m_sPlutoVMC		= "\\private\\\\PlutoMO.vmc";
	m_sPlutoConfig		= "\\private\\\\PlutoMO.cfg";
	m_sPlutoEventPng   	= "\\private\\\\PlutoEvent.png";

#else
	m_sAppFolder		= "x:\\System\\Apps\\PlutoMO";
	m_sLoggerFileName	= "x:\\System\\apps\\PlutoMO\\PlutoMO.log";
	m_sVMCFolderFilter	= "x:\\System\\Apps\\PlutoMO\\*.vmc";
	m_sPlutoVMC		= "x:\\System\\Apps\\PlutoMO\\PlutoMO.vmc";
	m_sPlutoConfig		= "x:\\System\\Apps\\PlutoMO\\PlutoMO.cfg";
	m_sPlutoEventPng	= "x:\\System\\Apps\\PlutoMO\\PlutoEvent.png";
	//current drive
	char cDrive = GetCurrentDrive();
	
	//adjusting drive
	m_sAppFolder.SetAt(cDrive, 0); 
	m_sLoggerFileName.SetAt(cDrive, 0); 
	m_sVMCFolderFilter.SetAt(cDrive, 0);
	m_sPlutoVMC.SetAt(cDrive, 0);
	m_sPlutoConfig.SetAt(cDrive, 0);
	m_sPlutoEventPng.SetAt(cDrive, 0);
#endif
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::ConstructL()
{
	m_bImageQualityScreen = false;
	m_bSignalStrengthScreen = false;

	//find the real paths to app's misc files
	SetupPaths();

#ifdef __SERIES60_30__
	BaseConstructL(EAknEnableSkin);
#else
	BaseConstructL();
#endif
    iAppContainer = new (ELeave) CPlutoMOContainer;
    iAppContainer->SetMopParent(this);
    iAppContainer->ConstructL( ClientRect() );
    AddToStackL( iAppContainer );

	iPlutoEventView = NULL;
	m_pVMCView = NULL;

	SymbianLogger *pLogger = new SymbianLogger(m_sLoggerFileName, KCPlutoLoggerId, CCoeStatic::EApp);

	//members initialization
	m_bSendKeyStrokes = false;
	m_bMakeVisibleAllowed = false;

	m_pBDCommandProcessor = NULL;

#ifndef __WINS__ 
	m_pBDCommandProcessor = new BDCommandProcessor_Symbian_Bluetooth("", this);
	m_pBDCommandProcessor->Start();
	m_pBDCommandProcessor->Listen();
	m_pBDCommandProcessor->SetupSecurityManager();
	m_pBDCommandProcessor->AdvertiseThePlutoService();
#endif

	m_bPlutoEventVisible = false;
	m_iSignalStrength = 0;
	m_bSignalStrengthScreen = false;
	m_bRender_SignalStrengthOnly = false;

	LOG("Waiting for connections...\n");

	LOG("Reading configuration file\n");
	ReadConfigurationFile();

	LOG("Setup incoming call notifier\n");
	SetupIncomingCallNotifier(); // ARM only (it is crashing in WINS)
	
	iCurType = 0;
	m_bApplicationIsMinimized = false;
}
//----------------------------------------------------------------------------------------------
CPlutoMOAppUi::~CPlutoMOAppUi()
{
	Cleanup();
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::Cleanup()
{
	CancelCaptureSoftKeys();

	if(NULL != m_pBDCommandProcessor)
	{
		m_pBDCommandProcessor->Disconnect();

		delete m_pBDCommandProcessor;
		m_pBDCommandProcessor = NULL;
	}

	if (iAppContainer)
    {
	    RemoveFromStack( iAppContainer );
		delete iAppContainer;
		iAppContainer = NULL;
    }

	if(NULL != iGetCallerId)
	{
		delete iGetCallerId;
		iGetCallerId = NULL;
	}
}
//----------------------------------------------------------------------------------------------

void CPlutoMOAppUi::ReadConfigurationFile()
{
	CAsciiLineReader lr;
	lr.OpenL(m_sPlutoConfig.Des());
	TInt iCurrentLineOffset;
	TPtrC8 iCurrentLine;

	lr.ReadL(iCurrentLine, iCurrentLineOffset);
	iURL.Copy(iCurrentLine);
	iURLClone.Copy(iCurrentLine);

	//number of events
	lr.ReadL(iCurrentLine, iCurrentLineOffset);
	TLex8 lex_num_event(iCurrentLine);
	lex_num_event.Val(iNumEventTypes);

	TInt i;
	for(i = 0; i < iNumEventTypes; i++)
	{
		lr.ReadL(iCurrentLine, iCurrentLineOffset);
		iPlutoEventTypes[i].Copy(iCurrentLine);
	}
	
	//read number of phone numbers
	lr.ReadL(iCurrentLine, iCurrentLineOffset);
	TLex8 lex_num_phone(iCurrentLine);
	lex_num_phone.Val(iNumPhoneTypes);

	for(i = 0; i < iNumPhoneTypes; i++)
	{
		//wap event
		lr.ReadL(iCurrentLine, iCurrentLineOffset);
		TLex8 lex_wap(iCurrentLine);
		lex_wap.Val(iPhoneTypes[i].iWAP_EventType);
		
		//phone number
		lr.ReadL(iCurrentLine, iCurrentLineOffset);
		iPhoneTypes[i].iPhoneNumber.Copy(iCurrentLine);

		//message
		lr.ReadL(iCurrentLine, iCurrentLineOffset);
		iPhoneTypes[i].iMessage.Copy(iCurrentLine);

		//hang up: true/false 1/0
		lr.ReadL(iCurrentLine, iCurrentLineOffset);
		TLex8 lex_hangup(iCurrentLine);
		lex_hangup.Val(iPhoneTypes[i].iHangUp);
	}

	//user name
	lr.ReadL(iCurrentLine, iCurrentLineOffset);
	iUsername.Copy(iCurrentLine);
	//pin
	lr.ReadL(iCurrentLine, iCurrentLineOffset);
	iPinMD5.Copy(iCurrentLine);
	//phone number
	lr.ReadL(iCurrentLine, iCurrentLineOffset);
	iPhoneNo.Copy(iCurrentLine);

	lr.Close();
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::SetupIncomingCallNotifier()
{
#ifndef __WINS__ 
	iGetCallerId = new (ELeave) CGetCallerId(this);	

	User::LeaveIfError(iTelServer.Connect());

	_LIT(KTsyName, "phonetsy.tsy");
	RTelServer::TPhoneInfo info;
	RPhone::TLineInfo lineInfo;
	RPhone::TLineInfo lineDataInfo;

	User::LeaveIfError(iTelServer.LoadPhoneModule(KTsyName));

	// Get the details for the first (and only) phone.

	User::LeaveIfError(iTelServer.GetPhoneInfo(0, info));

	// Open the phone.
	User::LeaveIfError(iPhone.Open(iTelServer, info.iName));
	
	iPhone.Initialise();
	// Get the information for the voice line, line 0.
	User::LeaveIfError(iPhone.GetLineInfo(0, lineInfo));
	
	// Open the line. iName will now be "VoiceLine1".
	User::LeaveIfError(iLine.Open(iPhone, lineInfo.iName));

	iIncomingCallWatcher = new (ELeave) CIncomingCallWatcher(iLine, this);
	iIncomingCallWatcher->ConstructL(); 
#endif //__WINS__
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::DynInitMenuPaneL(
    TInt aResourceId,CEikMenuPane* aMenuPane)
{
	if(aResourceId == 511950854) //menu resource id
	{
		aMenuPane->SetItemDimmed(EAknCmdExit, m_bVMCViewerVisible);
		aMenuPane->SetItemDimmed(EPlutoMOCmdAppTest1, m_bVMCViewerVisible);
		aMenuPane->SetItemDimmed(EPlutoMOCmdAppTest2, m_bVMCViewerVisible);
		aMenuPane->SetItemDimmed(EPlutoMOCmdAppTest3, m_bVMCViewerVisible);
		aMenuPane->SetItemDimmed(EPlutoMOCmdAppTest4, m_bVMCViewerVisible);
	}
}
//----------------------------------------------------------------------------------------------
TKeyResponse CPlutoMOAppUi::HandleKeyEventL(
    const TKeyEvent& aKeyEvent,TEventCode aType)
{
	if(m_bVMCViewerVisible && m_pVMCView)
	{
		m_pVMCView->iContainer->OfferKeyEvent(aKeyEvent, aType);
	    return EKeyWasConsumed;
	}

	if(m_bPlutoEventVisible && iPlutoEventView)
	{
		iPlutoEventView->iContainer->OfferKeyEvent(aKeyEvent, aType);
	    return EKeyWasConsumed;
	}

	return EKeyWasNotConsumed;
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        case EEikCmdExit:
            {
				Cleanup();
				Exit();
	            break;
            }

		// Have to change what it does currently
		// Opens the file 
		case EPlutoMOCmdAppTest1:
            {
            iAppContainer->CallLaunch();
			break;
            }

		// Version
		case EPlutoMOCmdAppTest2:
            {
            CAknInformationNote* informationNote = new (ELeave) CAknInformationNote;
			informationNote->ExecuteLD(string(VERSION).Des());
			break;
            }
			
		// Deleting a VMC file
        case EPlutoMOCmdAppTest3:
            {
            iAppContainer->CallDelete();
			iAppContainer->SetPlutoMO(EPlutoMOPictures, EPlutoMOSizeDateNoChange);
			break;
            }
		
		// About
        case EPlutoMOCmdAppTest4:
            {
			CAknInformationNote* informationNote = new (ELeave) CAknInformationNote;
			informationNote->ExecuteLD(_L("Pluto\nPluto Mobile Orbiter"));
			break;
            }

        default:
            break;      
        }
    }
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::CreateVMCView()
{
	if(NULL == m_pVMCView)
	{
		m_pVMCView = new (ELeave) CPlutoVMCView();
		CleanupStack::PushL(m_pVMCView);
		m_pVMCView->ConstructL();
		CleanupStack::Pop();    // view1
		AddViewL(m_pVMCView);        // transfer ownership to CAknViewAppUi

		ActivateLocalViewL(m_pVMCView->Id());
	}
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::MakeViewerVisible(bool Value)
{
	if(!m_pVMCView)
	{
		// Display an information note
		CAknInformationNote* informationNote;
		informationNote = new (ELeave) CAknInformationNote;
    	informationNote->ExecuteLD(_L("PlutoMO is currently disconnected"));

		iAppContainer->SetPlutoMO(EPlutoMOPictures, EPlutoMODate);

		return;
	}

	if(Value)
	{
		m_pVMCView->iContainer->MakeVisible(Value);
	}

	CaptureSoftKeys();

	Show();
	m_bVMCViewerVisible = true;
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::OpenImage(unsigned char Type, unsigned long Size, char *Data) //fullscreen (!)
{
	CreateVMCView();

	CPlutoVMCUtil *pVCMUtil = (CPlutoVMCUtil *)CCoeEnv::Static(KCPlutoVMCUtilId);
	pVCMUtil->SetImage(Type, Size, Data);

	m_pVMCView->Refresh();
	MakeViewerVisible(m_bMakeVisibleAllowed);
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::InterceptRepeatedKeys(unsigned long  KeysListSize, const char *pRepeatedKeysList)
{
	CPlutoVMCUtil *pVCMUtil = (CPlutoVMCUtil *)CCoeEnv::Static(KCPlutoVMCUtilId);
	pVCMUtil->InterceptRepeatedKeys(KeysListSize, pRepeatedKeysList);	
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::OpenVMC(bool bParsed, TFileName& iFileName, 
								 VIPMenuCollection *pVMC)
{
	CreateVMCView();

	CPlutoVMCUtil *pVCMUtil = (CPlutoVMCUtil *)CCoeEnv::Static(KCPlutoVMCUtilId);
	if(bParsed)
	{
		pVCMUtil->SetAndSwitchToMenu(pVMC);
	}
	else
		pVCMUtil->Parse(iFileName);

	m_pVMCView->Refresh();

	MakeViewerVisible(m_bMakeVisibleAllowed);
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::ShowList(
	unsigned long x, 
	unsigned long y, 
	unsigned long Width, 
	unsigned long Height,
	RPointerArray<string> DatagridStringList,
	bool bSendSelectedOnMove,
	bool bTurnOn,
	unsigned long iSelectedItem
)
{
	CreateVMCView();

	CPlutoVMCUtil *pVCMUtil = (CPlutoVMCUtil *)CCoeEnv::Static(KCPlutoVMCUtilId);
	pVCMUtil->SetList(x, y, Width, Height, DatagridStringList, bSendSelectedOnMove, bTurnOn, iSelectedItem);
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::SetCaptureKeyboardCommand(
	bool bOnOff, 
	bool bDataGrid, 
	bool bReset, 
	int  iEditType, 
	int  iVariable, 
	string sText
)
{
	CreateVMCView();

	CPlutoVMCUtil *pVCMUtil = (CPlutoVMCUtil *)CCoeEnv::Static(KCPlutoVMCUtilId);
	pVCMUtil->SetCaptureKeyboardCommand(bOnOff, bDataGrid, bReset, iEditType, iVariable, sText);
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::SaveFile(
	long iFileNameSize, 
	const char *pFileName, 
	long iFileDataSize, 
	const char *pFileData
) 
{
	TFileName iFileName;
	
	RFile file;
	RFs   aFs;
	aFs.Connect();

	LOG("Saving file: ");
	LOG(pFileName);
	LOG(iFileNameSize);
	string sFullPathName = m_sAppFolder + "\\" + string(pFileName);

	aFs.Delete(sFullPathName.Des());
	file.Create(aFs, sFullPathName.Des(), EFileStream | EFileWrite);

	HBufC8 *base_str = HBufC8::NewL(iFileDataSize);
	TPtr8 pStr = base_str->Des();

	for (unsigned int i = 0; i < iFileDataSize; i++)
		pStr.Append(TChar(pFileData[i]));

	file.Write(pStr, iFileDataSize);
	PLUTO_SAFE_DELETE(base_str);

	file.Close();

	aFs.Close();

	if(string(pFileName) == string("PlutoMO.cfg"))
		ReadConfigurationFile();
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::UpdateScreen(
	bool bStore, 
	unsigned long iVMCSize, 
	const char* pVMC,
	unsigned long iVMCFileNameSize, 
	const char* pVMCFileName
)
{
	if(bStore)
	{
		HBufC16 *pPath = HBufC16::NewL(256);
		TPtr16 aPath = pPath->Des();
#ifdef __SERIES60_30__
		string FilePath("\\private\\\\");
#else
		string FilePath("c:\\Nokia\\Images\\");
#endif
		for (unsigned int i = 0; i < FilePath.length(); i++)
			aPath.Append(FilePath[i]);

		for (int i = 0; i < iVMCFileNameSize; i++)
			aPath.Append(pVMCFileName[i]);
		
		SaveFile(FilePath.length(), string(aPath).c_str(), iVMCSize, pVMC);
	}

	TFileName iDummy;
	VIPMenuCollection *pVIPMenuCollection = new VIPMenuCollection(iVMCSize, pVMC);
	OpenVMC(true, iDummy, pVIPMenuCollection);
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::UpdateScreen(bool bParsed, const TDes8& aVmc, unsigned int uSize, 
								 VIPMenuCollection *pVMC)
{
	TFileName iFileName;
	
	if(!bParsed)
	{
		RFile file;
		RFs   aFs;
		aFs.Connect();

		HBufC8 *base_str = HBufC8::NewL(uSize);
		TPtr8 pStr = base_str->Des();

		for (unsigned int i = 0; i < uSize; i++)
			pStr.Append(TChar(aVmc[i]));

		HBufC16 *pPath = HBufC16::NewL(256);
		TPtr16 aPath = pPath->Des();

		file.Temp(aFs, aPath, iFileName, EFileStream | EFileWrite);
		file.Write(pStr, uSize);
		file.Close();

		delete base_str;
		base_str = NULL;

		delete pPath;
		pPath = NULL;

		aFs.Close();
	}

	OpenVMC(bParsed, iFileName, pVMC);
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::Hide()
{
	LOG("In hide method\n");

	TApaTask task(CEikonEnv::Static()->WsSession());
	task.SetWgId(CEikonEnv::Static()->RootWin().Identifier());
	task.SendToBackground();

	//m_bVMCViewerVisible = false;
	m_bApplicationIsMinimized = true;

	CancelCaptureSoftKeys();
	
	LOG("Existing hide method\n");
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::Show()
{
	if(m_bApplicationIsMinimized)
		return;

	CaptureSoftKeys();

	TApaTask task(CEikonEnv::Static()->WsSession());
	task.SetWgId(CEikonEnv::Static()->RootWin().Identifier());
	task.BringToForeground();

	//m_bVMCViewerVisible = true;
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::ResetViewer()
{
	LOG("Hidding VMCView...\n");
	if(NULL != m_pVMCView && NULL != m_pVMCView->iContainer)
	{
		m_pVMCView->iContainer->MakeVisible(false);
		m_bVMCViewerVisible = false;
		m_bMakeVisibleAllowed = true;
	}

	CPlutoVMCUtil *pVMCUtil = (CPlutoVMCUtil *)CCoeEnv::Static(KCPlutoVMCUtilId);

	if(pVMCUtil)
		pVMCUtil->Reset();

	LOG("Hidding PlutoEventView...\n");
	if(NULL != iPlutoEventView && NULL != iPlutoEventView->iContainer)
		iPlutoEventView->iContainer->MakeVisible(false);

	LOG("Reseting list...\n");
	RPointerArray<string> Dummy;
	ShowList(0, 0, 0, 0, Dummy, false, false, 0);

	LOG("Reseting capture keyboard...\n");
	SetCaptureKeyboardCommand(false, false, true, false, 0, "");

	LOG("Reset Viewer complete!\n");

	m_bRender_SignalStrengthOnly = false;
	m_iSignalStrength = 0;
	m_bSignalStrengthScreen = false;
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::NotifyIncomingCall(TDesC& aCallName)
{
	iCall.OpenExistingCall(iLine, aCallName);
	iGetCallerId->GetLatest();
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::NotifyIncomingNumber(const TDesC& aTellNumber)
{
	TInt i = 0;

	LOG("NotifyIncomingNumber: Phone number:\n");
	LOG(string(aTellNumber));
	LOG("\n");

	for(i = 0; i < iNumPhoneTypes; ++i)
	{
		TInt PhoneLen = iPhoneTypes[i].iPhoneNumber.Length();
		if (aTellNumber.Length() >= PhoneLen && iPhoneTypes[i].iPhoneNumber == aTellNumber.Right(PhoneLen))
		{
			LOG("Found a record: this is our number\n");

			if(iPhoneTypes[i].iHangUp)
			{
				iCall.HangUp();
				LOG("Hang up call - ok\n");
			}

			iCurType = i;

			LOG("Ready to open the browser\n");
			if(!(iIdle))
			{
				iIdle = CIdle::NewL(CActive::EPriorityIdle);
			}

			iIdle->Start(TCallBack(DoIdleStatic,this));
			return;
		}
	}

	iCall.Close();
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::CloseVMC() 
{ 
	m_pVMCView->iContainer->MakeVisible(false);
	m_bVMCViewerVisible = false;
	
	CancelCaptureSoftKeys();

	Cleanup();
	Exit();
}
//----------------------------------------------------------------------------------------------
TInt CPlutoMOAppUi::DoIdleStatic(TAny *aAppUi)
{
	LOG("In DoIdleStatic\n");
	return ((CPlutoMOAppUi*)aAppUi)->DoIdle();
}
//----------------------------------------------------------------------------------------------
TInt CPlutoMOAppUi::DoIdle()
{
	TBool ret;
	RCall::TStatus iCallStatus;
	iCall.GetStatus(iCallStatus);

	// check for status of the call set ret to ETrue when hannging up
	// or idle. and bring app to front
	if (iCallStatus == RCall::EStatusHangingUp || iCallStatus == RCall::EStatusIdle)
	{
		ret = EFalse; //finished do not come back
		iCall.Close();
		LOG("Ready to open PlutoEventView\n");

		LOG(iCurType);

		TBuf<256> msg;
		msg.Copy(_L("Pluto: security breach!\nSee the event?"));
		msg.Append(_L(".\n"));

		LOG(msg);

		CAknGlobalConfirmationQuery* confQuery = CAknGlobalConfirmationQuery::NewLC();
		TRequestStatus iConfirmationStat;
		confQuery->ShowConfirmationQueryL(iConfirmationStat, msg,R_AVKON_SOFTKEYS_YES_NO);
		User::WaitForRequest(iConfirmationStat);
		TInt qResult = iConfirmationStat.Int();
		confQuery->CancelConfirmationQuery();
		CleanupStack::PopAndDestroy(); // confQuery

//		yes - 3005  // WHY is this not a constant somewhere?!
//		no - 3006 
		if (qResult == 3005)
		{
			LOG("O fost un ok... open the browser\n");
			
			//just open vali's wml page for security
			iURL.Copy(iURLClone);
			iURL.Append(string("security=0").Des());
			LaunchBrowser();
		}
	}
	else
		ret = ETrue; //not finish, do come back

	LOG("end of idle");

	return ret;
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::LaunchBrowser()
{
	LOG("LaunchBrowser start\n");

	HBufC* param = HBufC::NewLC( 1024 ); 

	string sSecurityInfo = "&user=" + string(iUsername) + "&pin=" + string(iPinMD5) + "&phone=" + string(iPhoneNo);

	param->Des().Copy( _L("4 "));
	param->Des().Append(iURL); 

	//add here pin and user
	param->Des().Append(sSecurityInfo.Des());

	// Wap Browser's constants UId 
	const TInt KWmlBrowserUid = 0x10008D39; 
	TUid id( TUid::Uid( KWmlBrowserUid ) ); 

	TApaTaskList taskList( CEikonEnv::Static()->WsSession() ); 
	TApaTask task = taskList.FindApp( id ); 

	Hide();
	LOG("if ( task.Exists() )\n");

	if ( task.Exists() ) 
	{ 
		HBufC8* param8 = HBufC8::NewLC( param->Length() ); 
		param8->Des().Append( *param ); 
		task.SendMessage( TUid::Uid( 0 ), *param8 ); // Uid is not used 
		CleanupStack::PopAndDestroy(); // param8 

		LOG("app exists\n");
	} 
	else 
	{ 
		RApaLsSession appArcSession; 
		User::LeaveIfError(appArcSession.Connect()); // connect to AppArc server 
		TThreadId id; 
		appArcSession.StartDocument( *param, TUid::Uid( KWmlBrowserUid ), id ); 
		appArcSession.Close(); 

		LOG("app doesn't exist\n");
	} 

	task.BringToForeground();

	CleanupStack::PopAndDestroy(); // param 

	LOG("LaunchBrowser end\n");
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::SimulateEvent(long eventType, long key)
{
	LOG("CPlutoMOAppUi::SimulateEvent");
	m_pVMCView->iContainer->SimulateEvent(eventType, key);
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::Disconnect()
{
	 LOG("Need to disconnect\n");
	m_pBDCommandProcessor->Disconnect();
}
//---------------------------------------------------------------------------------------------
void CPlutoMOAppUi::CaptureSoftKeys()
{
	if(!m_iCapturedKeyIdYes)
	{
		m_iCapturedKeyIdYes = CEikonEnv::Static()->RootWin().CaptureKeyUpAndDowns(EStdKeyYes, 0, 0);
	}

	if(!m_iCapturedKeyIdNo)
	{
		m_iCapturedKeyIdNo = CEikonEnv::Static()->RootWin().CaptureKeyUpAndDowns(EStdKeyNo, 0, 0);
	}
}
//---------------------------------------------------------------------------------------------
void CPlutoMOAppUi::CancelCaptureSoftKeys()
{
	if(m_iCapturedKeyIdNo)
	{
		CEikonEnv::Static()->RootWin().CancelCaptureKeyUpAndDowns(m_iCapturedKeyIdNo);	
		m_iCapturedKeyIdNo = 0;
	}

	if(m_iCapturedKeyIdYes)
	{
		CEikonEnv::Static()->RootWin().CancelCaptureKeyUpAndDowns(m_iCapturedKeyIdYes);	
		m_iCapturedKeyIdYes = 0;
	}
}
//---------------------------------------------------------------------------------------------
void CPlutoMOAppUi::SetCurrentSignalStrength(int iSignalStrength)
{
	m_iSignalStrength = iSignalStrength;

	if(m_iSignalStrength)
	{
		m_bRender_SignalStrengthOnly = true;
		m_pVMCView->Refresh();
	}
}
//---------------------------------------------------------------------------------------------
// End of File  


