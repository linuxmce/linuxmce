/*
 PlutoVMCContainer
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

// INCLUDE FILES
#include "PlutoMO.hrh"
#include <avkon.hrh>
#include <aknnotewrappers.h> 
#include <aknutils.h>  // for Fonts. 
#include <e32keys.h>
#include <coemain.h>

#include <e32base.h>

#include "PlutoVMCContainer.h"

#include <aknviewappui.h>
#include <avkon.hrh>

#include "PlutoVMCView.h"
#include "PlutoVMCContainer.h" 
#include "PlutoVMCUtil.h"
#include "PlutoMOAppUi.h"

#include "pluto_main/Define_Button.h"
#include "VIPShared/BD_PC_SetImageQuality.h"

#include "Logger.h"

#define KEY_TIMER_INTERVAL 500000
//------------------------------------------------------------------------------------------------------------------
CPlutoVMCContainer::CPlutoVMCContainer()
{
	m_bRepeated = false;
	m_bNeedRefresh = false;
	m_bDeleteLastKey = false;
	m_bDataKeys = false;
}
//------------------------------------------------------------------------------------------------------------------
void CPlutoVMCContainer::ConstructL(const TRect& aRect)
{
	CreateWindowL();

	SetRect(aRect);
	ActivateL();

	iScrollTimer = CPeriodic::NewL(0/*EPriorityStandard*/);
	iKeyTimer = CPeriodic::NewL(0);
}
//------------------------------------------------------------------------------------------------------------------
CPlutoVMCContainer::~CPlutoVMCContainer()
{
	CPlutoVMCUtil *pVMCUtil = (CPlutoVMCUtil *)CCoeEnv::Static(KCPlutoVMCUtilId);

	delete pVMCUtil;
	pVMCUtil = NULL;

}
//------------------------------------------------------------------------------------------------------------------
void CPlutoVMCContainer::FocusTo(TInt aCommand)
{
}
//------------------------------------------------------------------------------------------------------------------
void CPlutoVMCContainer::SizeChanged()
{
}
//------------------------------------------------------------------------------------------------------------------
TInt CPlutoVMCContainer::CountComponentControls() const
{
	return 0;
}
//------------------------------------------------------------------------------------------------------------------
CCoeControl* CPlutoVMCContainer::ComponentControl(TInt aIndex) const
{
	return NULL;
}
//------------------------------------------------------------------------------------------------------------------
void CPlutoVMCContainer::Draw(const TRect& aRect) const
{
	if(((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_bVMCViewerVisible == false)
		return;

    CWindowGc& gc = SystemGc();

	CPlutoVMCUtil *pVMCUtil = (CPlutoVMCUtil *)CCoeEnv::Static(KCPlutoVMCUtilId);

	pVMCUtil->BeginPaint(&gc);
	pVMCUtil->LocalDoRender();
	pVMCUtil->EndPaint();

	((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_bMakeVisibleAllowed = true;
}
//------------------------------------------------------------------------------------------------------------------
TInt CPlutoVMCContainer::ScrollTimerCallBack(TAny* aContainer)
{
	CPlutoVMCContainer *pContainer = (CPlutoVMCContainer *)aContainer;
	CPlutoVMCUtil *pVMCUtil = (CPlutoVMCUtil *)CCoeEnv::Static(KCPlutoVMCUtilId);

	if(pContainer->m_bScrollUp)
	{
		if(pVMCUtil->ScrollListUp())
			((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_pVMCView->Refresh();
	}
	else
	{
		if(pVMCUtil->ScrollListDown())
			((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_pVMCView->Refresh();
	}

	return 0;
}
//------------------------------------------------------------------------------------------------------------------
TInt CPlutoVMCContainer::KeyTimerCallBack(TAny* aContainer)
{
	CPlutoVMCUtil *pVMCUtil = (CPlutoVMCUtil *)CCoeEnv::Static(KCPlutoVMCUtilId);
	CPlutoVMCContainer *pContainer = (CPlutoVMCContainer *)aContainer;

	pContainer->iKeyTimer->Cancel();
	pContainer->m_iRepeatStep = 0;
	pContainer->m_iLastTick = 0;
	pContainer->m_uLastKeyCode = 0;

	return 0;
}
//------------------------------------------------------------------------------------------------------------------
//negative values for KeyCode if the code is unknown
int CPlutoVMCContainer::GetKeyCode(const TKeyEvent& aKeyEvent, TEventCode aType)
{
	int KeyCode = 0;

	//LOGN("GetKeyCode: ");
	//LOGN(aKeyEvent.iScanCode);
	//LOGN("\n");

	if(aType == EEventKeyDown && aKeyEvent.iScanCode == 0x12)
		KeyCode=BUTTON_Phone_Pencil_CONST;

	if(aType == EEventKeyUp)
	{
		if(!m_bRepeated)
			switch(aKeyEvent.iScanCode)
			{
				case EStdKeyUpArrow:	 KeyCode=BUTTON_Up_Arrow_CONST;			break;
				case EStdKeyDownArrow:	 KeyCode=BUTTON_Down_Arrow_CONST;		break;
				case EStdKeyRightArrow:	 KeyCode=BUTTON_Right_Arrow_CONST;		break;
				case EStdKeyLeftArrow:	 KeyCode=BUTTON_Left_Arrow_CONST;		break;

				case EStdKeyYes:		 KeyCode=BUTTON_Phone_Talk_CONST;		break;
				case EStdKeyNo:			 KeyCode=BUTTON_Phone_End_CONST;		break;
				case EStdKeyEnter:		 KeyCode=BUTTON_Enter_CONST;			break;

				case EStdKeyEscape:		 KeyCode=BUTTON_Phone_C_CONST;			break;
				
				case 42: //for the phone
				case EStdKeyNkpAsterisk: KeyCode=BUTTON_Asterisk_CONST;			break;
				

				case EStdKeyHash:		 KeyCode=BUTTON_Pound_CONST;			break;

				case 0x30:				 KeyCode=BUTTON_0_CONST;				break;
				case 0x31:				 KeyCode=BUTTON_1_CONST;				break;
				case 0x32:				 KeyCode=BUTTON_2_CONST;				break;
				case 0x33:				 KeyCode=BUTTON_3_CONST;				break;
				case 0x34:				 KeyCode=BUTTON_4_CONST;				break;
				case 0x35:			 	 KeyCode=BUTTON_5_CONST;				break;
				case 0x36:			 	 KeyCode=BUTTON_6_CONST;				break;
				case 0x37:				 KeyCode=BUTTON_7_CONST;				break;
				case 0x38:				 KeyCode=BUTTON_8_CONST;				break;
				case 0x39:				 KeyCode=BUTTON_9_CONST;				break;

				case 0x01:				 KeyCode=BUTTON_Phone_C_CONST;			break; 

				case 0x12:               KeyCode=BUTTON_Phone_Pencil_CONST; 	break; 

				case 0xA4:				 KeyCode=BUTTON_Phone_Soft_left_CONST;  break;
				case 0xA5:				 KeyCode=BUTTON_Phone_Soft_right_CONST; break;
				case 0xA7:				 KeyCode=BUTTON_Enter_CONST;			break;
			}
	}

	if(aType == EEventKeyUp)
	{
		if(m_bRepeated)
			switch(aKeyEvent.iScanCode)
			{
				case EStdKeyYes:		 KeyCode=BUTTON_Rept_Phone_Talk_CONST;		break;
				case EStdKeyNo:			 KeyCode=BUTTON_Rept_Phone_End_CONST;		break;
				case EStdKeyEscape:		 KeyCode=BUTTON_Rept_Phone_C_CONST;			break;
				case 42:				 KeyCode=BUTTON_Rept_Asterisk_CONST;		break;
				case 127:				 KeyCode=BUTTON_Rept_Pound_CONST;			break;

				case 0x30:			     KeyCode=BUTTON_Rept_0_CONST;				break;
				case 0x31:				 KeyCode=BUTTON_Rept_1_CONST;				break;
				case 0x32:				 KeyCode=BUTTON_Rept_2_CONST;				break;
				case 0x33:				 KeyCode=BUTTON_Rept_3_CONST;				break;
				case 0x34:				 KeyCode=BUTTON_Rept_4_CONST;				break;
				case 0x35:				 KeyCode=BUTTON_Rept_5_CONST;				break;
				case 0x36:				 KeyCode=BUTTON_Rept_6_CONST;				break;
				case 0x37:				 KeyCode=BUTTON_Rept_7_CONST;				break;
				case 0x38:				 KeyCode=BUTTON_Rept_8_CONST;				break;
				case 0x39:				 KeyCode=BUTTON_Rept_9_CONST;				break;

				case 0x01:				 KeyCode=BUTTON_Rept_Phone_C_CONST;			break;
				case 0xA4:				 KeyCode=BUTTON_Phone_Soft_left_CONST;  break; //hack
				case 0xA5:				 KeyCode=BUTTON_Phone_Soft_right_CONST; break; //hack
			}
	}

	if(!KeyCode && aType == EEventKeyUp) //unknown key
		KeyCode = -aKeyEvent.iScanCode;

	return KeyCode;
}
//------------------------------------------------------------------------------------------------------------------
bool CPlutoVMCContainer::HandleCommonKeys(const TKeyEvent& aKeyEvent, TEventCode aType)
{
	//if(aType == EEventKeyUp)
	//	LOG("KEYUP");

	//LOG("LOCALKEY");

	CPlutoVMCUtil *pVMCUtil = (CPlutoVMCUtil *)CCoeEnv::Static(KCPlutoVMCUtilId);
	int KeyCode = GetKeyCode(aKeyEvent, aType);

	//build keypressed command
	if(KeyCode != 0) //known or unknown key
	{
		if(KeyCode==BUTTON_Rept_Phone_End_CONST)
		{
			((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->CloseVMC();
			return true;
		}

		if(((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_bImageQualityScreen)
		{
			bool bUpdateImageQuality = false;
			if(KeyCode==BUTTON_Phone_Soft_left_CONST)
			{
				if(((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_nImageQuality >= 10)
				{
					((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_nImageQuality -= 5;
					bUpdateImageQuality = true;
				}
			}

			if(KeyCode==BUTTON_Phone_Soft_right_CONST)
			{
				if(((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_nImageQuality <= 95)
					((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_nImageQuality += 5;
				else
					((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_nImageQuality = 100;

				bUpdateImageQuality = true;
			}

			if(bUpdateImageQuality)
			{
				//request new signal strength
				BDCommandProcessor_Symbian_Base* pBDCommandProcessor = 
					((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_pBDCommandProcessor;

				if(pBDCommandProcessor)
				{
					BDCommand *pCommand = new BD_PC_SetImageQuality(((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_nImageQuality);
					pBDCommandProcessor->AddCommand(pCommand);
				}
			}
		}

		pVMCUtil->LocalKeyPressed(KeyCode);

		return true;
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------------
bool CPlutoVMCContainer::HandleDataGridKeys(const TKeyEvent& aKeyEvent, TEventCode aType)
{
	LOG("HandleDataGridKeys");
	CPlutoVMCUtil *pVMCUtil = (CPlutoVMCUtil *)CCoeEnv::Static(KCPlutoVMCUtilId);
	
	if(pVMCUtil->GridExists() && aType == EEventKeyDown)
	{
		if(aKeyEvent.iScanCode == EStdKeyUpArrow)
		{
			if(pVMCUtil->ScrollListUp())
				m_bNeedRefresh = true;

			m_bDataKeys = true;
			
			return true;
		}

		if(aKeyEvent.iScanCode == EStdKeyDownArrow)
		{
			if(pVMCUtil->ScrollListDown())
				m_bNeedRefresh = true; 

			m_bDataKeys = true;

			return true;
		}

		if(aKeyEvent.iScanCode == /*EStdKeyEnter*/ 167)
		{
			LOG("ENTER-sending selected");
			if(pVMCUtil->SelectCurrentItem())
				m_bNeedRefresh = true; 

			//m_bDataKeys = true;

			return true;
		}
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------------
void CPlutoVMCContainer::AutomaticallyScrollDataGrid(bool bUp)
{
	const unsigned long iTimerInterval = KEY_TIMER_INTERVAL;

	if (iScrollTimer->IsActive())
	{
		iScrollTimer->Cancel();
	}

	m_bScrollUp = bUp;

	iScrollTimer->Start(iTimerInterval, iTimerInterval, TCallBack(ScrollTimerCallBack, this)); 
}
//------------------------------------------------------------------------------------------------------------------
bool CPlutoVMCContainer::HandleAutomaticDataGridScrolling(const TKeyEvent& aKeyEvent, TEventCode aType)
{
	CPlutoVMCUtil *pVMCUtil = (CPlutoVMCUtil *)CCoeEnv::Static(KCPlutoVMCUtilId);

	if(pVMCUtil->GridExists() && aType == EEventKeyDown)
	{
		if(aKeyEvent.iScanCode == EStdKeyUpArrow)
		{
			AutomaticallyScrollDataGrid(true);
			return true;
		}
		
		if(aKeyEvent.iScanCode == EStdKeyDownArrow)
		{
			AutomaticallyScrollDataGrid(false);
			return true;
		}
	}

	if ((aType == EEventKeyDown || aType == EEventKeyUp) && iScrollTimer->IsActive())
	{
		iScrollTimer->Cancel();
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------------
int CPlutoVMCContainer::NumberKeyIndex(int KeyCode)
{
	switch(KeyCode)
	{
		case BUTTON_0_CONST:	return 0;
		case BUTTON_1_CONST:	return 1;
		case BUTTON_2_CONST:	return 2;
		case BUTTON_3_CONST:	return 3;
		case BUTTON_4_CONST:	return 4;

		case BUTTON_5_CONST:	return 5;
		case BUTTON_6_CONST:	return 6;
		case BUTTON_7_CONST:	return 7;
		case BUTTON_8_CONST:	return 8;
		case BUTTON_9_CONST:	return 9;
	}

	return -1;
}
//------------------------------------------------------------------------------------------------------------------
bool CPlutoVMCContainer::IsNumberKey(int KeyCode)
{
	switch(KeyCode)
	{
		case BUTTON_0_CONST:
		case BUTTON_1_CONST:
		case BUTTON_2_CONST:
		case BUTTON_3_CONST:
		case BUTTON_4_CONST:
		case BUTTON_5_CONST:
		case BUTTON_6_CONST:
		case BUTTON_7_CONST:
		case BUTTON_8_CONST:
		case BUTTON_9_CONST:
			return true;
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------------
bool CPlutoVMCContainer::IsNumberKey(const TKeyEvent& aKeyEvent)
{
	switch(aKeyEvent.iScanCode)
	{
		case 0x30:
		case 0x31:
		case 0x32:
		case 0x33:
		case 0x34:
		case 0x35:
		case 0x36:
		case 0x37:
		case 0x38:
		case 0x39:
			return true;
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------------
char CPlutoVMCContainer::GetKeyChar(int KeyCode)
{
	CPlutoVMCUtil *pVMCUtil = (CPlutoVMCUtil *)CCoeEnv::Static(KCPlutoVMCUtilId);

	const char *KeysMap[]			 = {".0",".1","abc2","def3","ghi4","jkl5","mno6","pqrs7","tuv8","wxyz9"}; 
	int KeysMapIndex[]				 = {2,   2,   4,     4,     4,      4,    4,     5,      4,      5     }; 
	
	const int iDiff = 14 * KEY_TIMER_INTERVAL / 200000;

	char KeyChar = 0;
	int iNow = User::TickCount();

	m_bDeleteLastKey = false;
	pVMCUtil->m_bEdit_BackspacePressed = false;

	//numeric button? (0, 1, ... 9)
	if(IsNumberKey(KeyCode))
	{
		//only digits
		if(pVMCUtil->m_CaptureKeyboardParam.bTypePin || pVMCUtil->m_CaptureKeyboardParam.bNumbersOnly)
			return '0' + NumberKeyIndex(KeyCode);

		int KeyIndex = NumberKeyIndex(m_uLastKeyCode);
		if(KeyIndex == 0 || KeyIndex == 1)
		{
			KeyChar = KeysMap[KeyIndex][0];
		}

		//first key ? start the timer!
		if(m_uLastKeyCode == 0) 
		{
			m_iRepeatStep = 0;
			const unsigned long iTimerInterval = 2 * KEY_TIMER_INTERVAL;
			
			if (iKeyTimer->IsActive())
				iKeyTimer->Cancel();

			iKeyTimer->Start(iTimerInterval, iTimerInterval, TCallBack(KeyTimerCallBack, this)); 
			KeyChar = KeysMap[NumberKeyIndex(KeyCode)][m_iRepeatStep];
		}
		else
		{
			//different button then last one or time's up - stop the timer, we have a key
			if(m_uLastKeyCode != KeyCode || iNow - m_iLastTick > iDiff)
			{
				if(m_uLastKeyCode != KeyCode)
					m_iRepeatStep = 0;
				else
					m_iRepeatStep %= KeysMapIndex[NumberKeyIndex(KeyCode)];

				KeyChar = KeysMap[NumberKeyIndex(KeyCode)][m_iRepeatStep];

				iKeyTimer->Cancel();
				m_iRepeatStep = 0;
				m_iLastTick = 0;
				m_uLastKeyCode = 0;
			}
			else //same button, just shift keys and start again the timer
			{
				m_bDeleteLastKey = true;
				m_iRepeatStep++;
				m_iRepeatStep %= KeysMapIndex[NumberKeyIndex(KeyCode)];

				const unsigned long iTimerInterval = 2 * KEY_TIMER_INTERVAL;

				if (iKeyTimer->IsActive())
					iKeyTimer->Cancel();

				iKeyTimer->Start(iTimerInterval, iTimerInterval, TCallBack(KeyTimerCallBack, this)); 
				KeyChar = KeysMap[NumberKeyIndex(KeyCode)][m_iRepeatStep];//***
			}
		}
		
		m_uLastKeyCode = KeyCode;
		m_iLastTick = iNow;
	}
	else 
		if(m_uLastKeyCode)
		{
			KeyChar = KeysMap[NumberKeyIndex(m_uLastKeyCode)][m_iRepeatStep];

			m_uLastKeyCode = 0;
			m_iLastTick = 0;
		}

	return KeyChar;
}
//------------------------------------------------------------------------------------------------------------------
bool CPlutoVMCContainer::HandleCaptureKeyboardKeys(const TKeyEvent& aKeyEvent, TEventCode aType)
{
	CPlutoVMCUtil *pVMCUtil = (CPlutoVMCUtil *)CCoeEnv::Static(KCPlutoVMCUtilId);

	//is capture keyboard command on ?
	if(!pVMCUtil->m_CaptureKeyboardParam.bOnOff)
		return false;

	int KeyCode = GetKeyCode(aKeyEvent, aType);

	if(KeyCode <= 0)
		return false; //ignore this key

	//enter keys & text not empty
	if(BUTTON_Enter_CONST == KeyCode && pVMCUtil->m_CaptureKeyboardParam.sVariableValue.length() > 0)
	{
		pVMCUtil->SetVariable();
		pVMCUtil->LocalKeyPressed(BUTTON_Terminate_Text_CONST);
		pVMCUtil->m_bRedrawOnlyEdit = true;
		pVMCUtil->m_bRedrawOnlyGrid = true;
		m_bNeedRefresh = true;	
		return true;
	}

	//verify is there is an 'interesting' key
	if(
		!IsNumberKey(aKeyEvent)					&&
		BUTTON_Rept_Phone_C_CONST != KeyCode	&&
		BUTTON_Phone_C_CONST != KeyCode			
	)
	{
		m_uLastKeyCode = 0;
		pVMCUtil->SetVariable();
		return false;  
	}

	//if user pressed 'Cancel' button and the text buffer is empty
	if(
		(BUTTON_Phone_C_CONST == KeyCode || BUTTON_Rept_Phone_C_CONST == KeyCode)	&&
		pVMCUtil->m_CaptureKeyboardParam.sVariableValue.length() == 0
	)
	{
		m_uLastKeyCode = 0;
		pVMCUtil->SetVariable();
		return false; 
	}

	if(BUTTON_Phone_C_CONST == KeyCode)
		if(pVMCUtil->ClearEdit())
		{
			pVMCUtil->m_bRedrawOnlyEdit = true;
			pVMCUtil->m_bRedrawOnlyGrid = true;
			m_bNeedRefresh = true;	
			return true;
		}

	if(BUTTON_Rept_Phone_C_CONST == KeyCode)
		if(pVMCUtil->ClearAllEdit())
		{
			pVMCUtil->m_bRedrawOnlyEdit = true;
			pVMCUtil->m_bRedrawOnlyGrid = true;
			m_bNeedRefresh = true;		
			return true;
		}

	char KeyChar = GetKeyChar(KeyCode);

	if(BUTTON_Phone_C_CONST == KeyCode)
		pVMCUtil->m_bEdit_BackspacePressed = true;

	if(m_bDeleteLastKey)
	{
		string Temp;
		
		for(int i = 0; i < pVMCUtil->m_CaptureKeyboardParam.sVariableValue.length() - 1; i++)
			Temp.Append(pVMCUtil->m_CaptureKeyboardParam.sVariableValue[i]);

		pVMCUtil->m_CaptureKeyboardParam.sVariableValue = Temp;

		m_bDeleteLastKey = 0;
	}

	if(KeyChar)
	{
		pVMCUtil->m_CaptureKeyboardParam.sVariableValue += KeyChar;	
		
		if(pVMCUtil->m_CaptureKeyboardParam.bDataGrid)
		{
			if(pVMCUtil->ScrollListPartialMatch())
			{
				pVMCUtil->m_bRedrawOnlyGrid = true;
				m_bNeedRefresh = true;
			}
		}

		if(pVMCUtil->m_CaptureKeyboardParam.bTextBox)
		{
			pVMCUtil->m_bRedrawOnlyEdit = true;
			m_bNeedRefresh = true;
		}


	}
	
	return true;
}
//------------------------------------------------------------------------------------------------------------------
TKeyResponse CPlutoVMCContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
	return EKeyWasNotConsumed;
}
//------------------------------------------------------------------------------------------------------------------
TKeyResponse CPlutoVMCContainer::OfferKeyEvent(const TKeyEvent& aKeyEvent, TEventCode aType)
{
	CPlutoVMCUtil *pVMCUtil = (CPlutoVMCUtil *)CCoeEnv::Static(KCPlutoVMCUtilId);

	if(aType == EEventKeyDown)
		m_bDataKeys = false;

	if(aKeyEvent.iScanCode == 180) //alt+tab :D
	{
		((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->Hide();
		return EKeyWasNotConsumed;
	}

	//if the viewer is not visible on the screen, ignore the key pressed
	if(!((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_bVMCViewerVisible)
		return EKeyWasNotConsumed;

	if(((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_bPlutoEventVisible)
		return EKeyWasNotConsumed;

	if(!((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_bMakeVisibleAllowed)
		return EKeyWasNotConsumed;

	//any repeated keys?
	if(pVMCUtil->IsRepeatedKey(aKeyEvent.iScanCode))
	{
		pVMCUtil->SendKey(pVMCUtil->SymbianKey2PlutoKey(aKeyEvent.iScanCode), aType == EEventKeyUp);
		return EKeyWasConsumed;
	}

	TKeyResponse Response = EKeyWasNotConsumed;
	m_bNeedRefresh = false;

	//check if it was a repeat key (the key was press long time)
	if(aType != EEventKeyUp && aType != EEventKeyDown)
	{
		if(aKeyEvent.iRepeats == 1)
			m_bRepeated = true;
		else
			m_bRepeated = false;
	}

	//handles data grid keys
	bool bRes1 = HandleDataGridKeys(aKeyEvent, aType); //handles up, down and enter for data grid
	bool bRes2 = HandleAutomaticDataGridScrolling(aKeyEvent, aType); //handles data grid automatic scroll

	if(bRes1 || bRes2)
	{
		Response = EKeyWasConsumed;

		if(m_bNeedRefresh)
			DrawDeferred(); 

		return Response;
	}

	//handles capture keyboard keys
	if(HandleCaptureKeyboardKeys(aKeyEvent, aType))
	{
		Response = EKeyWasConsumed;

		if(m_bNeedRefresh)
			DrawDeferred(); 

		return Response; 
	}

	if(m_bDataKeys)
		return EKeyWasConsumed;

	//handles common keys
	if(HandleCommonKeys(aKeyEvent, aType))
		Response = EKeyWasConsumed;

	if(m_bNeedRefresh)
		DrawDeferred(); 

	//no key was consumed
    return Response;
}
//------------------------------------------------------------------------------------------------------------------
void CPlutoVMCContainer::HandleControlEventL(
    CCoeControl* aControl,TCoeEvent aEventType)
{
}
//------------------------------------------------------------------------------------------------------------------
int CPlutoVMCContainer::GetSymbianKeyEventFromKeyCode(TKeyEvent& aKeyEvent, TEventCode& aType, long key)
{
	CPlutoVMCUtil *pVMCUtil = (CPlutoVMCUtil *)CCoeEnv::Static(KCPlutoVMCUtilId);
	
	aKeyEvent.iRepeats = 0;
	m_bDataKeys = false;
	aType = EEventKeyUp;
	aKeyEvent.iScanCode = pVMCUtil->PlutoKey2SymbianKey(key);

	return 0;
}
//------------------------------------------------------------------------------------------------------------------
void CPlutoVMCContainer::SimulateEvent(long eventType, long key)
{
	CPlutoVMCUtil *pVMCUtil = (CPlutoVMCUtil *)CCoeEnv::Static(KCPlutoVMCUtilId);

	pVMCUtil->m_bSimulation = true;

	TKeyEvent aKeyEvent;
	TEventCode aType;

	GetSymbianKeyEventFromKeyCode(aKeyEvent, aType, key);
	OfferKeyEvent(aKeyEvent, aType);

	pVMCUtil->m_bSimulation = false;
}
//------------------------------------------------------------------------------------------------------------------
// End of File  

