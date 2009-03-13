/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
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
#include "OrbiterApp.h"

#include "DCE/Logger.h"
#include "BD/BDCommandProcessor_Smartphone_Bluetooth.h"
#include "VIPShared/BD_PC_KeyWasPressed.h"
#include "VIPShared/BD_PC_GetSignalStrength.h"
#include "VIPShared/BD_PC_SelectedFromList.h"
#include "VIPShared/BD_PC_SetVariable.h"
#include "VIPShared/BD_PC_SetImageQuality.h"
#include "BD/BD_WhatDoYouHave.h"
#include "pluto_main/Define_Button.h"
#include "SerializeClass/ShapesColors.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "DCE/Logger.h"
#include "IncomingCallNotifier.h"
#include "VIPShared/BD_PC_Configure.h"
#include "VIPShared/BD_PC_SelectedItem.h"
#include "VIPShared/BD_PC_MouseEvent.h"
#include "VIPShared/BD_PC_GetMenuImages.h"
#include "Win32/DrawTextExUTF8.h"
#include "PlutoUtils/PlutoDefs.h"

using namespace DCE;

#include <string>
#include <algorithm>
using namespace std;

#include "LRMenu.h"
#include "LRPhoneMenu.h"

#include <aygshell.h>

#if defined(SMARTPHONE2005) || defined(_VC80_UPGRADE)
	#define clock		GetTickCount
#endif

#if defined(SMARTPHONE2005) || defined(_VC80_UPGRADE)		//--- CHANGED4WM5 ----//	
	#define Surface_GetWidth pSurface->GetWidth()
	#define Surface_GetHeight pSurface->GetHeight()
	#define pf_uint8_t PHAL::uint8_t
#else
	#define Surface_GetWidth pSurface->m_width
	#define Surface_GetHeight pSurface->m_height
	#define pf_uint8_t uint8_t
#endif

#if defined(SMARTPHONE2005) // Palm Treo 700w: 240x240
	#define APP_WIDTH  240
	#define APP_HEIGHT 240
#else	// Motorola MPx 220
	#define APP_WIDTH  176
	#define APP_HEIGHT 220
#endif
//---------------------------------------------------------------------------------------------------------
#define RED_MASK_16		(0x1F << 11)
#define GREEN_MASK_16	(0x3F << 5)
#define BLUE_MASK_16	0x1F

#define RED(color)		((0x00FF0000 & color) >> 16)
#define GREEN(color)	((0x0000FF00 & color) >> 8)
#define BLUE(color)		(0x000000FF & color)

#define ScrollTimerId 1
#define KeyTimerId    2
//---------------------------------------------------------------------------------------------------------
inline Pixel GetColor16(COLORREF color)
{
	return 
		(Pixel)(
			((GetRValue(color) << 8) & RED_MASK_16) | 
			((GetGValue(color) << 3) & GREEN_MASK_16) | 
			(GetBValue(color) >> 3)
		);		
}
//---------------------------------------------------------------------------------------------------------
static COLORREF black = RGB(0,0,0);
static COLORREF darkGray = RGB(85,85,85);
static COLORREF liteGray = RGB(140,140,200);
static COLORREF white = RGB(255,255,255); 
static COLORREF blue = RGB(101,123,179); 
static COLORREF blue_lite = RGB(205,214,237); 
//---------------------------------------------------------------------------------------------------------
CComModule _Module; //ATL - pocket frog
//-----------------------------------------------------------------------------------------------------
#define CHECK_STATUS() { if(m_bQuit_get()) return; }
//-----------------------------------------------------------------------------------------------------
OrbiterApp *OrbiterApp::m_pInstance = NULL; //the one and only
//---------------------------------------------------------------------------------------------------------
string sMsg;
HWND hWndMB = NULL;
HMENU hMenu;
RECT globalrc;

OrbiterApp::OrbiterApp(HINSTANCE hInstance) : m_ScreenMutex("rendering"), m_hInstance(hInstance)
{
	Reset();
	m_bQuit_set(false);

	m_nImageWidth = APP_WIDTH;
	m_nImageHeight = APP_HEIGHT;

	pthread_mutexattr_init( &m_MutexAttr );
    pthread_mutexattr_settype( &m_MutexAttr,  PTHREAD_MUTEX_RECURSIVE_NP );
	m_ScreenMutex.Init( &m_MutexAttr );

	m_pInstance = this;
	m_pBDCommandProcessor = new BDCommandProcessor_Smartphone_Bluetooth();

	m_nBkgndImageType = 0;
	m_pBkgndImage_Size = 0;
	m_pBkgndImage_Data = NULL;
	m_pBkgndImage_Repaint = false;

//#define TEST_DATAGRID
#ifdef TEST_DATAGRID
	m_bGridExists = true;

	m_ulGridX = 0;
	m_ulGridY = 40;
	m_ulGridWidth = 176;
	m_ulGridHeight = 160;

	m_vectDataGrid.push_back("17:30 ProTV\nStirile ProTV");
	m_vectDataGrid.push_back("17:35 ProTV\nStirile ProTV");
	m_vectDataGrid.push_back("20:00 HBO - Braveheart");
	m_vectDataGrid.push_back("17:40 ProTV\nStirile ProTV");
	m_vectDataGrid.push_back("17:45 ProTV\nStirile ProTV");
	m_vectDataGrid.push_back("17:50 ProTV\nStirile ProTV");
	m_vectDataGrid.push_back("17:55 ProTV\nStirile ProTV");
	m_vectDataGrid.push_back("18:30 ProTV - film");
	m_vectDataGrid.push_back("19:30 ProTV\nStirile ProTV");
	m_vectDataGrid.push_back("19:50 ProTV\nStirile ProTV");

	m_bGridSendSelectedOnMove = 0;
	m_ulGridSelectedItem = 2;

	m_CaptureKeyboardParam.bOnOff = true;
	m_CaptureKeyboardParam.bDataGrid = false;
	m_CaptureKeyboardParam.bReset = true;
	m_CaptureKeyboardParam.bTypePin = false;
	m_CaptureKeyboardParam.bNumbersOnly = false;
	m_CaptureKeyboardParam.bTextBox = true;
	m_CaptureKeyboardParam.iVariable = 17;
	m_CaptureKeyboardParam.sVariableValue = "";
	m_CaptureKeyboardParam.TextX = 0;
	m_CaptureKeyboardParam.TextY = 0;
	m_CaptureKeyboardParam.TextWidth = 175;
	m_CaptureKeyboardParam.TextHeight = 25;
	m_CaptureKeyboardParam.Reset();	
#endif

	// initialization for LRMenu
	m_pMainMenu = NULL;	
	m_bMainMenuRepaint = false;

	m_pIncomingCallNotifier = new IncomingCallNotifier();


}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ OrbiterApp::~OrbiterApp()
{
	if(NULL != m_pBkgndImage_Data)
	{
		delete[] m_pBkgndImage_Data;
		m_pBkgndImage_Data = NULL;
		m_pBkgndImage_Size = 0;
		m_nBkgndImageType = 0;
	}

	pthread_mutexattr_destroy(&m_MutexAttr);
	pthread_mutex_destroy(&m_ScreenMutex.mutex);

	delete m_pBDCommandProcessor;
	delete m_pIncomingCallNotifier;
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterApp::Reset()
{
	m_nImageType = 0;

	m_ulGridSelectedItem = 0;
	m_bRedrawOnlyGrid = false;
	m_bRedrawOnlyEdit = false;
	m_bGridExists = false;
	m_ulGridX = 0;
	m_ulGridY = 0;
	m_ulGridWidth = 0;
	m_ulGridHeight = 0;
	m_ulGridTopItem = 0;
	m_vectDataGrid.clear();

	m_CaptureKeyboardParam.bTextBox = false;
	m_CaptureKeyboardParam.bOnOff = false;

	m_nSignalStrength = 0;
	m_bSignalStrengthScreen = false;
	m_bImageQualityScreen = false;

	m_bRepeated = false;
	m_bNeedRefresh = false;
	m_bDeleteLastKey = false;
	m_bDataKeys = false;

	m_nLastTick = 0;
	m_nLastKeyCode = 0;
	m_nRepeatStep = 0;
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterApp::GameSuspend()
{
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterApp::GameResume()
{
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ bool OrbiterApp::IsFullScreen()
{
	return true;
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ int OrbiterApp::GetWindowWidth()
{
	return m_nImageWidth;
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ int OrbiterApp::GetWindowHeight()
{
	return m_nImageHeight;
}
//---------------------------------------------------------------------------------------------------------

/*virtual*/ bool OrbiterApp::GameInit()
{		
	
	ShowDisconnected();
	RefreshScreen();
	
	m_bTimerUp = false;
	if ( SetTimer( SLEEP_TIMER_ID, SLEEP_TIMER_TIMEOUT ) )
		m_bTimerUp = true;

	return true;
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterApp::GameEnd()
{
	// Clearing LRMenu
	PLUTO_SAFE_DELETE( m_pMainMenu )
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterApp::GameLoop()
{
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterApp::StylusDown( Point stylus )
{
	////// Send only Sylus Down	
	#if defined(SMARTPHONE2005) //rescale coords to Smartphone 2003
		if ( m_pMainMenu && m_pMainMenu->IsShowing() ) {
			if ( m_pMainMenu->HandleStylus( stylus.x, stylus.y ) ) return;
		}
		if( m_bGridExists ){
			if ( HandleDataGridStylus( stylus.x, stylus.y ) ) {
				RefreshScreen();
				return;
			}
		}
		double dVal = ((double)stylus.x)*(176./(double)APP_WIDTH);
		if ( dVal-(int)dVal>=0.5) ceil(dVal);
		else floor(dVal);
		stylus.x = (int)dVal;
		dVal = ((double)stylus.y)*(220./(double)APP_HEIGHT);
		if ( dVal-(int)dVal>=0.5) ceil(dVal);
		else floor(dVal);		
		stylus.y = (int)dVal;
	#endif
	SendMouseEvent( stylus.x, stylus.y, BD_PC_MouseEvent::meStylusDown );
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterApp::StylusUp( Point stylus )
{
	////// Send only Sylus Down
	// SendMouseEvent( stylus.x, stylus.y, BD_PC_MouseEvent::meStylusUp );
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterApp::StylusMove( Point stylus )
{
	////// Send only Sylus Down
	// SendMouseEvent( stylus.x, stylus.y, BD_PC_MouseEvent::meMove );
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ bool OrbiterApp::PocketFrogButtonDown(int nButton)
{
	HandleKeyEvents(WM_KEYDOWN, nButton, 0L);
	return true;
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ bool OrbiterApp::PocketFrogButtonUp(int nButton)
{
	HandleKeyEvents(WM_KEYUP, nButton, 0L);
	return true;
}
//---------------------------------------------------------------------------------------------------------
void OrbiterApp::SendKey(int nKeyCode, int nEventType)
{ 
	if(!m_pBDCommandProcessor->m_bClientConnected || !m_bSendKeyStrokes )
		return;
	// If local rendered menu is active don't send keys
	if ( m_pMainMenu && m_pMainMenu->IsShowing() ) return;

	// Disable sending keys for unknown codes;
	if ( nKeyCode<0 ) return; 

	BDCommand *pCommand = new BD_PC_KeyWasPressed(nKeyCode, nEventType);
	m_pBDCommandProcessor->AddCommand(pCommand);

//	BD_WhatDoYouHave *pBD_WhatDoYouHave = new BD_WhatDoYouHave();
//	m_pBDCommandProcessor->AddCommand(pBD_WhatDoYouHave);
}
//---------------------------------------------------------------------------------------------------------
void OrbiterApp::LocalKeyPressed(int nKeyCode)
{
	RenderMenu::KeyPressed(nKeyCode);
/*
	if(!m_pBDCommandProcessor->m_bClientConnected)
	{
		BD_WhatDoYouHave *pBD_WhatDoYouHave = new BD_WhatDoYouHave();
		m_pBDCommandProcessor->AddCommand(pBD_WhatDoYouHave);
	}
*/
}
//---------------------------------------------------------------------------------------------------------
#if defined(SMARTPHONE2005)		//--- CHANGED4WM5 ----//
void OrbiterApp::PreTranslateVirtualKey( UINT uMsg, WPARAM* wParam, bool *bLongKey )
{
	static bool bLastKeyShift = false;
	if ( *wParam==0x10 ) {
		// for numeric keys it sends a WM_KEYUP(0x10)+WM_KEYDOWN(key)+WM_KEYUP(key)+WM_KEYUP(0x10)
		// for other keys( i.e. '*','#' )it sends a WM_KEYDOWN(0x10)+WM_KEYDOWN(key)+WM_KEYUP(key)+WM_KEYUP(0x10)
		if(uMsg == WM_KEYDOWN) bLastKeyShift = true;
		else bLastKeyShift = false;
		return;
	}	
	switch(*wParam){		
		case VK_LWIN: *wParam = 0; break; // sends VK_LWIN most the time -> not a real key
		case 0xC2: *wParam = VK_ESCAPE; *bLongKey = false; break; // <Ok> Short
		case 0xC1: *wParam = VK_LWIN; *bLongKey = false; break; // <LWin>	Short	
		case 228:
		case VK_F4: *wParam = VK_F4; *bLongKey = true; break; // <EndCall> Long

		case 0xC5: *wParam = VK_ESCAPE; *bLongKey = true; break; // <Ok> Long

		case 0xC8: *wParam = VK_F4; *bLongKey = false; break; // <EndCall> Short

		case '8': if ( bLastKeyShift ) *wParam = VK_NUMPAD8; break;// *
		case '3': if ( bLastKeyShift ) *wParam = VK_NUMPAD9; break;// #	
		
	}	
}
#endif

//---------------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterApp::HandleKeyEvents(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static int nTimeDown = 0;
	static bool bIsLongKey = false;

    if(uMsg == WM_KEYDOWN)
	{
		::MessageBeep(MB_ICONASTERISK);
		m_bDataKeys = false;
		nTimeDown = clock();				
		bIsLongKey = false;
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Key down %d, time %d", wParam, nTimeDown);
#endif
	}
	else
	{
		int nTimeUp = clock();
		bIsLongKey = (nTimeUp - nTimeDown) > 300;
	}

	#if defined(SMARTPHONE2005)		//--- CHANGED4WM5 ----//
		//Translate virtual keys for Treo
		PreTranslateVirtualKey( uMsg, &wParam, &bIsLongKey );
	#endif
	int nPK_Button = VirtualKey2PlutoKey(wParam, bIsLongKey);
	if(IsRepeatedKey(wParam))
	{
		SendKey(nPK_Button ? nPK_Button : - wParam, uMsg == WM_KEYUP);
		return;
	}

	// Handle keys on local rendered menus
	if ( m_pMainMenu ) {
		//PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);
		if ( m_pMainMenu->HandleKeys( nPK_Button, uMsg == WM_KEYUP ) ) return;
	}

	m_bNeedRefresh = false;

	//handles data grid keys
	bool bRes1 = HandleDataGridKeys(nPK_Button, uMsg == WM_KEYUP); //handles up, down and enter for data grid
	bool bRes2 = HandleAutomaticDataGridScrolling(nPK_Button, uMsg == WM_KEYUP); //handles data grid automatic scroll
	
	if(bRes1 || bRes2)
	{
		if(m_bNeedRefresh)
			RefreshScreen(); 

		return;
	}

	if(m_bDataKeys)
		return;

	//handles capture keyboard keys
	if(HandleCaptureKeyboardKeys(nPK_Button, uMsg == WM_KEYUP, bIsLongKey))
	{
		if(m_bNeedRefresh)
			RefreshScreen(); 

		return; 
	}


	if(uMsg == WM_KEYUP)
	{
		if(BUTTON_Rept_Phone_End_CONST == nPK_Button)
		{
			OnQuit();
			return;
		}

		if(BUTTON_Rept_Phone_Pencil_CONST == nPK_Button)
		{
			Hide();
			return;
		}

		if(m_bImageQualityScreen)
		{
			bool bUpdateImageQuality = false;
			if(nPK_Button == BUTTON_Phone_Soft_left_CONST)
			{
				if(m_ulImageQuality >= 10)
				{
					m_ulImageQuality -= 5;
					bUpdateImageQuality = true;
				}
			}

			if(nPK_Button == BUTTON_Phone_Soft_right_CONST)
			{
				if(m_ulImageQuality <= 95)
					m_ulImageQuality += 5;
				else
					m_ulImageQuality = 100;

				bUpdateImageQuality = true;
			}

			if(bUpdateImageQuality)
			{
				if(m_pBDCommandProcessor->m_bClientConnected)
				{
					BDCommand *pCommand = new BD_PC_SetImageQuality(m_ulImageQuality);
					m_pBDCommandProcessor->AddCommand(pCommand);
				}
			}
		}

		SendKey(nPK_Button ? nPK_Button : - wParam, 2);
	}
}
//---------------------------------------------------------------------------------------------------------
void OrbiterApp::OnQuit()
{
	m_bQuit_set(true);
	::PostQuitMessage(0);
	Shutdown();
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterApp::TryToUpdate()
{
	if(m_bQuit_get())
		return;

	PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);
	Rect r( 0, 0, APP_WIDTH, APP_HEIGHT );
	GetDisplay()->Update( &r );
}
//---------------------------------------------------------------------------------------------------------
void OrbiterApp::RenderImage(int nImageType, int nSize, char *pData, int nX, int nY, int nWidth, int nHeight)
{
	if(m_bQuit_get())
		return;

	if(nWidth == 0 && nHeight == 0)
	{
		nWidth = m_nImageWidth;
		nHeight = m_nImageHeight;
	}
	
	PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);
	Surface *pSurface = LoadImage(GetDisplay(), (pf_uint8_t*)pData, (pf_uint8_t*)(pData + nSize));

	if(pSurface)
	{
		if(Surface_GetWidth == nWidth && Surface_GetHeight == nHeight)
			GetDisplay()->Blit( nX, nY, pSurface );
		else //zoom
		{
			Rect dest;	
			dest.Set(nX, nY, nWidth - 1, nHeight - 1);

			double ZoomX = nWidth / double(Surface_GetWidth);
			double ZoomY = nHeight / double(Surface_GetHeight);

			dest.right = dest.left + int(Surface_GetWidth * ZoomX);
			dest.bottom = dest.top + int(Surface_GetHeight * ZoomY);     
        
			if( dest.right-dest.left>0 && dest.bottom-dest.top>0 ) 
				GetDisplay()->BlitStretch(dest, pSurface); 
		}

		delete pSurface;
	}
}
//---------------------------------------------------------------------------------------------------------
void OrbiterApp::ShowImage(int nImageType, int nSize, char *pData)
{
	m_bRender_SignalStrengthOnly = false;

	if(NULL != m_pImageStatic_Data)
	{
		delete[] m_pImageStatic_Data;
		m_pImageStatic_Data = NULL;
	}

	if(NULL != m_pMenuCollection)
	{
		delete m_pMenuCollection;
		m_pMenuCollection = NULL;
	}

	m_nImageType = nImageType; 
	m_pImageStatic_Size = nSize;

	m_pImageStatic_Data = new char[nSize];
	memcpy(m_pImageStatic_Data, pData, nSize);

	// on receiving ShowImage hide local rendered menu
	if ( m_pMainMenu ) m_pMainMenu->Hide();
	RefreshScreen();
}
//---------------------------------------------------------------------------------------------------------
void OrbiterApp::SetSignalStrength(int nSignalStrength)
{
	m_nSignalStrength = nSignalStrength;
}
//---------------------------------------------------------------------------------------------------------
void OrbiterApp::SetSignalStrengthScreen(bool bSignalStrengthScreen)
{
	m_bSignalStrengthScreen = bSignalStrengthScreen;
}
//---------------------------------------------------------------------------------------------------------
void OrbiterApp::SetImageQualityScreen(bool bImageQualityScreen)
{
	m_bImageQualityScreen = bImageQualityScreen;
}
//---------------------------------------------------------------------------------------------------------
void OrbiterApp::InterceptRepeatedKeys(int nKeysListSize, char *pRepeatedKeysList)
{
	m_vectRepeatedKeys.clear();

	if(nKeysListSize)
	{
		unsigned int msgpos = 0;
		string token;
		string sList = pRepeatedKeysList;

		while((token = StringUtils::Tokenize(sList, ",", msgpos)) != "") 	
		{
			m_vectRepeatedKeys.push_back(PlutoKey2VirtualKey(atoi(token.c_str())));
		}
	}
}
//---------------------------------------------------------------------------------------------------------
int OrbiterApp::PlutoKey2VirtualKey(int nPlutoKey)
{
	int nVirtualKey = 0;

	switch (nVirtualKey)
    {
        case BUTTON_0_CONST:  nVirtualKey = '0';    break;
        case BUTTON_1_CONST:  nVirtualKey = '1';    break;
        case BUTTON_2_CONST:  nVirtualKey = '2';    break;
        case BUTTON_3_CONST:  nVirtualKey = '3';    break;
        case BUTTON_4_CONST:  nVirtualKey = '4';    break;
        case BUTTON_5_CONST:  nVirtualKey = '5';    break;
        case BUTTON_6_CONST:  nVirtualKey = '6';    break;
        case BUTTON_7_CONST:  nVirtualKey = '7';    break;
        case BUTTON_8_CONST:  nVirtualKey = '8';    break;
        case BUTTON_9_CONST:  nVirtualKey = '9';    break;

		case BUTTON_Phone_C_CONST: 			nVirtualKey = VK_ESCAPE;	break;
		case BUTTON_Phone_Pencil_CONST:		nVirtualKey = VK_LWIN;		break;
		case BUTTON_Phone_Talk_CONST: 		nVirtualKey = VK_F3;		break;
		case BUTTON_Phone_End_CONST: 		nVirtualKey = VK_F4;		break;
		case BUTTON_Phone_Soft_left_CONST:	nVirtualKey = VK_F1;		break;
		case BUTTON_Phone_Soft_right_CONST: nVirtualKey = VK_F2; 		break;
		case BUTTON_Asterisk_CONST:			nVirtualKey = VK_NUMPAD8;	break;
		case BUTTON_Pound_CONST:			nVirtualKey = VK_NUMPAD9;	break;

		case BUTTON_Up_Arrow_CONST:			nVirtualKey = VK_UP; 		break;
		case BUTTON_Down_Arrow_CONST:		nVirtualKey = VK_DOWN;		break;
		case BUTTON_Left_Arrow_CONST:		nVirtualKey = VK_LEFT;		break;
		case BUTTON_Right_Arrow_CONST:		nVirtualKey = VK_RIGHT;		break;
		case BUTTON_Enter_CONST:			nVirtualKey = VK_RETURN;	break;
	}

	return nPlutoKey;
}
//---------------------------------------------------------------------------------------------------------
int OrbiterApp::VirtualKey2PlutoKey(int nVirtualKey, bool bLongKey)
{
	int nPK_Button = -nVirtualKey;

	if(!bLongKey)
	{
		switch (nVirtualKey)
		{
			case '0':       nPK_Button = BUTTON_0_CONST;     break;
			case '1':       nPK_Button = BUTTON_1_CONST;     break;
			case '2':       nPK_Button = BUTTON_2_CONST;     break;
			case '3':       nPK_Button = BUTTON_3_CONST;     break;
			case '4':       nPK_Button = BUTTON_4_CONST;     break;
			case '5':       nPK_Button = BUTTON_5_CONST;     break;
			case '6':       nPK_Button = BUTTON_6_CONST;     break;
			case '7':       nPK_Button = BUTTON_7_CONST;     break;
			case '8':       nPK_Button = BUTTON_8_CONST;     break;
			case '9':       nPK_Button = BUTTON_9_CONST;     break;

			case VK_ESCAPE:		nPK_Button = BUTTON_Phone_C_CONST;			break;
			case VK_LWIN:		nPK_Button = BUTTON_Phone_Pencil_CONST;		break;
			case VK_F3:			nPK_Button = BUTTON_Phone_Talk_CONST;		break;
			case VK_F4:			nPK_Button = BUTTON_Phone_End_CONST;		break;
			case VK_F1:			nPK_Button = BUTTON_Phone_Soft_left_CONST;	break;
			case VK_F2:			nPK_Button = BUTTON_Phone_Soft_right_CONST;	break;
			case 119:
			case VK_NUMPAD8:    nPK_Button = BUTTON_Asterisk_CONST;			break;
			case 120:
			case VK_NUMPAD9:    nPK_Button = BUTTON_Pound_CONST;			break;

			case VK_UP:			nPK_Button = BUTTON_Up_Arrow_CONST;			break;
			case VK_DOWN:		nPK_Button = BUTTON_Down_Arrow_CONST;		break;
			case VK_LEFT:		nPK_Button = BUTTON_Left_Arrow_CONST;		break;
			case VK_RIGHT:		nPK_Button = BUTTON_Right_Arrow_CONST;		break;
			case VK_RETURN:		nPK_Button = BUTTON_Enter_CONST;			break;

			//case 117: //Smartphone's VOL_UP
			//case 118: //Smartphone's VOL_DOWN
		}
	}
	else
	{
		switch (nVirtualKey)
		{
			case '0':       nPK_Button = BUTTON_Rept_0_CONST;     break;
			case '1':       nPK_Button = BUTTON_Rept_1_CONST;     break;
			case '2':       nPK_Button = BUTTON_Rept_2_CONST;     break;
			case '3':       nPK_Button = BUTTON_Rept_3_CONST;     break;
			case '4':       nPK_Button = BUTTON_Rept_4_CONST;     break;
			case '5':       nPK_Button = BUTTON_Rept_5_CONST;     break;
			case '6':       nPK_Button = BUTTON_Rept_6_CONST;     break;
			case '7':       nPK_Button = BUTTON_Rept_7_CONST;     break;
			case '8':       nPK_Button = BUTTON_Rept_8_CONST;     break;
			case '9':       nPK_Button = BUTTON_Rept_9_CONST;     break;

			case VK_ESCAPE:		nPK_Button = BUTTON_Rept_Phone_C_CONST;			break;
			case 196:
			case VK_LWIN:		nPK_Button = BUTTON_Rept_Phone_Pencil_CONST;	break;
			case VK_F3:			nPK_Button = BUTTON_Rept_Phone_Talk_CONST;		break;
			case VK_F4:
			case 198:			nPK_Button = BUTTON_Rept_Phone_End_CONST;		break;
			case VK_F1:			nPK_Button = BUTTON_Rept_Phone_Soft_left_CONST;	break;
			case VK_F2:			nPK_Button = BUTTON_Rept_Phone_Soft_right_CONST;break;
			case VK_NUMPAD8:    nPK_Button = BUTTON_Rept_Asterisk_CONST;		break;
			case VK_NUMPAD9:    nPK_Button = BUTTON_Rept_Pound_CONST;			break;

			case VK_UP:			nPK_Button = BUTTON_Up_Arrow_CONST;			break;
			case VK_DOWN:		nPK_Button = BUTTON_Down_Arrow_CONST;		break;
			case VK_LEFT:		nPK_Button = BUTTON_Left_Arrow_CONST;		break;
			case VK_RIGHT:		nPK_Button = BUTTON_Right_Arrow_CONST;		break;
			case VK_RETURN:		nPK_Button = BUTTON_Enter_CONST;			break;
		}
	}

	return nPK_Button;
}
//---------------------------------------------------------------------------------------------------------
bool OrbiterApp::IsRepeatedKey(int nVirtualKey)
{
	vector<int>::iterator it;
	for(it = m_vectRepeatedKeys.begin(); it != m_vectRepeatedKeys.end(); it++)
	{
		if(*it == nVirtualKey)
			return true;
	}
	return false;
}
//---------------------------------------------------------------------------------------------------------
void OrbiterApp::RenderDataGrid(unsigned long ulX, unsigned long ulY, unsigned long ulWidth, 
	unsigned long ulHeight, vector<string> &vectDataGrid)
{
	PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);

	int nRowHeight = DEFAULT_DATAGRID_ROWHEIGHT;
	const int cnSmallOffsetX = 5;
	const int cnSmallOffsetY = 14;
	const int cnShadowLen = 3;


	
	int nVisibleItems = ulHeight / nRowHeight;
	nRowHeight = ulHeight / nVisibleItems;

	GetDisplay()->FillRect(ulX, ulY, ulX + ulWidth, ulY + nRowHeight * nVisibleItems, GetColor16(darkGray));

	int i = 0;

/*
	//let's see how many items are visible right now
	for(i = m_ulGridTopItem; i < m_ulGridTopItem + nVisibleItems; i++)
	{
		if(i >= vectDataGrid.size()) 
			break;

		string s = vectDataGrid[i];
		if(s.find('\n') != string::npos)
			nVisibleItems--;
	}

	//scroll the grid if need it
	while(m_ulGridTopItem + nVisibleItems - 1 < m_ulGridSelectedItem)
		m_ulGridTopItem++;
	while(m_ulGridTopItem > m_ulGridSelectedItem)
		m_ulGridTopItem--;

	//let's see now how many items are visible
	nVisibleItems = ulHeight / nRowHeight;
	for(i = m_ulGridTopItem; i < m_ulGridTopItem + nVisibleItems; i++)
	{
		if(i >= vectDataGrid.size()) 
			break;

		string s = vectDataGrid[i];
		if(s.find('\n') != string::npos)
			nVisibleItems--;
	}

	unsigned long ulGridBottomItem = m_ulGridTopItem + nVisibleItems;

	if(ulGridBottomItem > vectDataGrid.size())
		ulGridBottomItem = vectDataGrid.size();

*/  
	unsigned long ulGridBottomItem = 0;
	CalcDataGridItemsLayout( ulGridBottomItem, nRowHeight );

#if defined(SMARTPHONE2005)	
	bool bHasScroll = true;
	//if ( m_vectDataGrid.size()==ulGridBottomItem ) bHasScroll = false;
	

	if ( bHasScroll ) { //Draw scroll bar
		ulWidth -= DEFAULT_DATAGRID_SCROLLWIDTH;
		GetDisplay()->FillRect( ulX + ulWidth,
								ulY,
								ulX + ulWidth + DEFAULT_DATAGRID_SCROLLWIDTH,
								ulY + ulHeight,
								GetColor16(blue_lite)
								);		
		GetDisplay()->DrawRect( ulX + ulWidth,
								ulY,
								ulX + ulWidth + DEFAULT_DATAGRID_SCROLLWIDTH,
								ulY + ulHeight,
								GetColor16(darkGray)
								);		
		GetDisplay()->DrawRect( ulX + ulWidth,
								ulY,
								ulX + ulWidth + DEFAULT_DATAGRID_SCROLLWIDTH,
								ulY + DEFAULT_DATAGRID_SCROLLWIDTH,
								GetColor16(darkGray)
								);
		GetDisplay()->DrawRect( ulX + ulWidth,
								ulY + ulHeight - DEFAULT_DATAGRID_SCROLLWIDTH,
								ulX + ulWidth + DEFAULT_DATAGRID_SCROLLWIDTH,
								ulY + ulHeight,
								GetColor16(darkGray)
								);
		Point UpperTrg[3] = {
							 Point(ulX + ulWidth+2, ulY+DEFAULT_DATAGRID_SCROLLWIDTH-2), 
							 Point(ulX + ulWidth+DEFAULT_DATAGRID_SCROLLWIDTH-1, ulY+DEFAULT_DATAGRID_SCROLLWIDTH-2), 
							 Point(ulX + ulWidth+DEFAULT_DATAGRID_SCROLLWIDTH/2, ulY+1 ) 
							};
		Point LowerTrg[3] = {
							 Point(ulX + ulWidth+2, ulY+ulHeight-DEFAULT_DATAGRID_SCROLLWIDTH+1), 
							 Point(ulX + ulWidth+DEFAULT_DATAGRID_SCROLLWIDTH-1, ulY+ulHeight-DEFAULT_DATAGRID_SCROLLWIDTH+1), 
							 Point(ulX + ulWidth+DEFAULT_DATAGRID_SCROLLWIDTH/2, ulY+ulHeight-2 ) 
							};
		GetDisplay()->FillPoly( UpperTrg, 3, GetColor16(liteGray) );
		GetDisplay()->FillPoly( LowerTrg, 3, GetColor16(liteGray) );
	}
#endif
	int nExpandOffset = 0;
	string s1, s2;

	//the rendering
	for(i = m_ulGridTopItem; i < ulGridBottomItem; i++)
	{
		string s = vectDataGrid[i];
		int nBNpos = s.find('\n');
		int nStrLen =  s.length();
		int nItemSizeOffset = (nBNpos != string::npos) * nRowHeight;

		if(nBNpos == string::npos)
		{
			s1 = s;
		}
		else
		{
			s1 = s.substr(0, nBNpos);
			s2 = s.substr(nBNpos + 1, nStrLen - nBNpos);
		}
		
		if(m_ulGridSelectedItem == i)
		{
			//selected item shadow : darkGray
			GetDisplay()->FillRect(
				ulX + cnShadowLen, 
				ulY + (i - m_ulGridTopItem) * nRowHeight + nExpandOffset + cnShadowLen, 
				ulX + ulWidth, 
				ulY + (i - m_ulGridTopItem + 1) * nRowHeight + nExpandOffset + nItemSizeOffset, 
				GetColor16(darkGray)
			);

			//selected background color : bluelite
			GetDisplay()->FillRect(
				ulX, 
				ulY + (i - m_ulGridTopItem) * nRowHeight + nExpandOffset, 
				ulX + ulWidth - cnShadowLen, 
				ulY + (i - m_ulGridTopItem + 1) * nRowHeight + nExpandOffset - cnShadowLen + nItemSizeOffset, 
				GetColor16(blue_lite)
			);
		}
		else
		{
			//item shadow : darkGray
			GetDisplay()->FillRect(
				ulX + cnShadowLen, 
				ulY + (i - m_ulGridTopItem) * nRowHeight + nExpandOffset + 1, 
				ulX + ulWidth, 
				ulY + (i - m_ulGridTopItem + 1) * nRowHeight + nExpandOffset + nItemSizeOffset, 
				GetColor16(darkGray)
			);

			//background color : liteGray
			GetDisplay()->FillRect(
				ulX, 
				ulY + (i - m_ulGridTopItem) * nRowHeight + nExpandOffset, 
				ulX + ulWidth - 1, 
				ulY + (i - m_ulGridTopItem + 1) * nRowHeight + nExpandOffset - 1 + nItemSizeOffset, 
				GetColor16(blue)
			);
		}

		COLORREF ItemColor = m_ulGridSelectedItem == i ? black : white;

		RenderText(s1, ulX + cnSmallOffsetX, ulY +  (i - m_ulGridTopItem) * nRowHeight + nExpandOffset, ulWidth - cnSmallOffsetX, 
				   nRowHeight, ItemColor);
		if(nBNpos != string::npos)
		{
			RenderText(s2, ulX + cnSmallOffsetX, ulY + (i - m_ulGridTopItem) * nRowHeight + nExpandOffset + nItemSizeOffset, 
				ulWidth - cnSmallOffsetX, nRowHeight, ItemColor);

			nExpandOffset += nRowHeight;
		}
	}
}
//---------------------------------------------------------------------------------------------------------
void OrbiterApp::RenderText(string sText, unsigned long ulX, unsigned long ulY, unsigned long ulWidth, 
	unsigned long ulHeight, COLORREF color)
{
	PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);

	HDC hdc = GetDisplay()->GetBackBuffer()->GetDC(false);

 	::SetTextColor(hdc, color);
	::SetBkMode(hdc, TRANSPARENT);

    wchar_t wText[1024];
    mbstowcs(wText, sText.c_str(), 1024);

	RECT rectLocation = { ulX, ulY, ulX + ulWidth, ulY + ulHeight };
    ::DrawText(hdc, wText, sText.length(), &rectLocation, DT_WORDBREAK | DT_NOPREFIX); 

	GetDisplay()->GetBackBuffer()->ReleaseDC(hdc);
}
//---------------------------------------------------------------------------------------------------------
void OrbiterApp::ShowList(unsigned long ulX, unsigned long ulY, unsigned long ulWidth, 
	unsigned long ulHeight, vector<string> vectDataGrid, bool bSendSelectedOnMove, 
	bool bTurnOn, int nSelectedIndex)
{
	m_bRedrawOnlyGrid = false;
	m_bGridExists = bTurnOn;
	m_bGridSendSelectedOnMove = bSendSelectedOnMove;

	if(bTurnOn)
	{
		m_ulGridX		= ulX;
		m_ulGridY		= ulY;
		m_ulGridWidth	= ulWidth;
		m_ulGridHeight	= ulHeight;

		m_vectDataGrid = vectDataGrid;

		if(nSelectedIndex >= 0 && nSelectedIndex < m_vectDataGrid.size())
			m_ulGridSelectedItem = nSelectedIndex;
		else
			m_ulGridSelectedItem = 0;
	}
}
//---------------------------------------------------------------------------------------------------------
void OrbiterApp::RefreshScreen()
{
	PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);


	if ( m_pMainMenu && m_pMainMenu->IsShowing())	{
		// If local rendered menu created and showing 
		// try updating background and repainting manu
			if ( NULL != m_pBkgndImage_Data ) {
				DrawImage( m_nBkgndImageType, m_pBkgndImage_Data, m_pBkgndImage_Size, 0, 0, 0, 0 );		
				m_pBkgndImage_Repaint = false;
			}
		m_pMainMenu->Paint( m_bMainMenuRepaint );
		m_bMainMenuRepaint = false;
	}
	else { // there's no local rendered menu active
		if ( m_pBkgndImage_Repaint && NULL != m_pBkgndImage_Data ) {
			DrawImage( m_nBkgndImageType, m_pBkgndImage_Data, m_pBkgndImage_Size, 0, 0, 0, 0 );		
			m_pBkgndImage_Repaint = false;
		}
		else {
			if(NULL != m_pMenu || (m_pImageStatic_Size && m_pImageStatic_Data))
			{
				if(!m_bRender_SignalStrengthOnly)
				{
					if(!m_bRedrawOnlyGrid && !m_bRedrawOnlyEdit)
					{
						DoRender();
					}

					if(m_bGridExists)
					{
						RenderDataGrid(m_ulGridX, m_ulGridY, m_ulGridWidth, m_ulGridHeight, m_vectDataGrid);
					}			

					if(m_CaptureKeyboardParam.bTextBox)
						RenderEditBox();
				}

				if(m_bSignalStrengthScreen)
				{
					RenderSignalStrength(m_nSignalStrength);

					//request new signal strength
					if(m_pBDCommandProcessor->m_bClientConnected)
					{
						BDCommand *pCommand = new BD_PC_GetSignalStrength();
						m_pBDCommandProcessor->AddCommand(pCommand);
					}

					m_bRedrawOnlyGrid = false;
					m_bRedrawOnlyEdit = false;

					return;
				}

				if(m_bImageQualityScreen)			
				{
					RenderImageQuality();			
				}
			}
		}
	}

#ifdef TEST_DATAGRID	
	RenderDataGrid(m_ulGridX, m_ulGridY, m_ulGridWidth, m_ulGridHeight, m_vectDataGrid);

	if(m_CaptureKeyboardParam.bTextBox)
		RenderEditBox();
#endif

	TryToUpdate();

	m_bRedrawOnlyGrid = false;
	m_bRedrawOnlyEdit = false;
}
//---------------------------------------------------------------------------------------------------------
void OrbiterApp::RenderSignalStrength(int nSignalStrength)
{
	PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);

	PlutoRectangle rect(m_nImageWidth - 31, m_nImageHeight - 48, 30, 16);

	GetDisplay()->FillRect(rect.Left(), rect.Top(), rect.Right(), rect.Bottom(), white);
	RenderText(StringUtils::ltos(nSignalStrength), rect.Left(), rect.Top(), rect.Right(), rect.Bottom(), black);

	Rect rectUpdate;
	rectUpdate.Set(rect.Left(), rect.Top(), rect.Right(), rect.Bottom());
	GetDisplay()->Update(&rectUpdate);
}
//---------------------------------------------------------------------------------------------------------
void OrbiterApp::RenderImageQuality()
{
	PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);
	PlutoRectangle rect(0, m_nImageHeight - 20, m_nImageWidth, 20);

	GetDisplay()->FillRect(rect.Left(), rect.Top(), rect.Right(), rect.Bottom(), white);

	string sMessage = " -    Image Quality: " + StringUtils::ltos(m_ulImageQuality) + "%     +";
	RenderText(sMessage, rect.Left(), rect.Top(), rect.Right(), rect.Bottom(), black);
}
//---------------------------------------------------------------------------------------------------------
bool OrbiterApp::HandleDataGridKeys(int nPlutoKey, bool bKeyUp)
{
	if ( m_bGridExists && bKeyUp ) {
		if( nPlutoKey == BUTTON_Up_Arrow_CONST || nPlutoKey == BUTTON_Down_Arrow_CONST ){
			m_bDataKeys = true;
			return false;
		}
	}
	if(m_bGridExists && !bKeyUp)
	{
		if(nPlutoKey == BUTTON_Up_Arrow_CONST)
		{
			if(ScrollListUp())
				m_bNeedRefresh = true;

			m_bDataKeys = true;
			return true;
		}

		if(nPlutoKey == BUTTON_Down_Arrow_CONST)
		{
			if(ScrollListDown())
				m_bNeedRefresh = true; 

			m_bDataKeys = true;
			return true;
		}

		if(nPlutoKey == BUTTON_Enter_CONST)
		{
			if(SelectCurrentItem())
				m_bNeedRefresh = true; 

			return true;
		}
	}

	return false;
}
//---------------------------------------------------------------------------------------------------------
bool OrbiterApp::HandleAutomaticDataGridScrolling(int nPlutoKey, bool bKeyUp)
{
	if(m_bGridExists && !bKeyUp)
	{
		if(nPlutoKey == BUTTON_Up_Arrow_CONST)
		{
			AutomaticallyScrollDataGrid(true);
			return true;
		}
		
		if(nPlutoKey == BUTTON_Down_Arrow_CONST)
		{
			AutomaticallyScrollDataGrid(false);
			return true;
		}
	}

	::KillTimer(m_hWnd, ScrollTimerId);
	return false;
}
//---------------------------------------------------------------------------------------------------------
bool OrbiterApp::HandleCaptureKeyboardKeys(int KeyCode, bool bKeyUp, bool bLongKey)
{
	//is capture keyboard command on ?
	if(!m_CaptureKeyboardParam.bOnOff || !bKeyUp || KeyCode <= 0)
		return false;

	//enter keys & text not empty
	if(BUTTON_Enter_CONST == KeyCode && m_CaptureKeyboardParam.sVariableValue.length() > 0)
	{
		SetVariable();
		LocalKeyPressed(BUTTON_Terminate_Text_CONST);
		m_bRedrawOnlyEdit = true;
		m_bRedrawOnlyGrid = true;
		m_bNeedRefresh = true;	
		return true;
	}

	//verify is there is an 'interesting' key
	if(
		!IsNumberKey(KeyCode)					&&
		BUTTON_Rept_Phone_C_CONST != KeyCode	&&
		BUTTON_Phone_C_CONST != KeyCode			
	)
	{
		m_nLastKeyCode = 0;
		SetVariable();
		return false;  
	}

	//if user pressed 'Cancel' button and the text buffer is empty
	if(
		(BUTTON_Phone_C_CONST == KeyCode || BUTTON_Rept_Phone_C_CONST == KeyCode)	&&
		m_CaptureKeyboardParam.sVariableValue.length() == 0
	)
	{
		m_nLastKeyCode = 0;
		SetVariable();
		return false; 
	}

	if(BUTTON_Phone_C_CONST == KeyCode)
		if(ClearEdit())
		{
			m_bRedrawOnlyEdit = true;
			m_bRedrawOnlyGrid = true;
			m_bNeedRefresh = true;	
			return true;
		}

	if(BUTTON_Rept_Phone_C_CONST == KeyCode)
		if(ClearAllEdit())
		{
			m_bRedrawOnlyEdit = true;
			m_bRedrawOnlyGrid = true;
			m_bNeedRefresh = true;		
			return true;
		}

	char KeyChar = GetKeyChar(KeyCode);

	if(m_bDeleteLastKey)
	{
		m_CaptureKeyboardParam.sVariableValue = m_CaptureKeyboardParam.sVariableValue.substr(0, m_CaptureKeyboardParam.sVariableValue.length() - 1);
		m_bDeleteLastKey = 0;
	}

	if(KeyChar)
	{
		m_CaptureKeyboardParam.sVariableValue += KeyChar;	
		if(m_CaptureKeyboardParam.bDataGrid)
		{
			if(ScrollListPartialMatch())
			{
				m_bRedrawOnlyGrid = true;
				m_bNeedRefresh = true;
			}
		}

		if(m_CaptureKeyboardParam.bTextBox)
		{
			m_bRedrawOnlyEdit = true;
			m_bNeedRefresh = true;
		}
	}
	
	return true;
}
//------------------------------------------------------------------------------------------------------------------
bool OrbiterApp::ScrollListPartialMatch()
{
	bool bResult = false;
	string s = m_CaptureKeyboardParam.sVariableValue;	

	for(int i = 0; i < m_vectDataGrid.size(); i++)
	{
		string item = m_vectDataGrid[i];
		if(s.length() > item.length())
			continue;

		bool bMatch = true;
		for(int j = 0; j < s.length(); j++)
		{
			char c = item[j];

			if('A' <= c && c <= 'Z') 
				c = c - 'A' + 'a';

			if(s[j] != c)
			{
				bMatch = false;
				break;
			}
		}

		if(bMatch)
		{
			if(m_ulGridSelectedItem != i)
				bResult = true;

			m_ulGridSelectedItem = i;
			return bResult;
		}
	}	

	if(m_ulGridSelectedItem != 0)
		bResult = true;

	m_ulGridSelectedItem = 0;
	return bResult;
}
//------------------------------------------------------------------------------------------------------------------
void CALLBACK KeyTimerCallBack(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	OrbiterApp::GetInstance()->m_nRepeatStep = 0;
	OrbiterApp::GetInstance()->m_nLastTick = 0;
	OrbiterApp::GetInstance()->m_nLastKeyCode = 0;

	::KillTimer(OrbiterApp::GetInstance()->m_hWnd, KeyTimerId);
}
//------------------------------------------------------------------------------------------------------------------
char OrbiterApp::GetKeyChar(int KeyCode)
{
	const char *KeysMap[]			 = {".0",".1","abc2","def3","ghi4","jkl5","mno6","pqrs7","tuv8","wxyz9"}; 
	int KeysMapIndex[]				 = {2,   2,   4,     4,     4,      4,    4,     5,      4,      5     }; 

	const int nDiff = 800;
	char KeyChar = 0;
	int nNow = ::GetTickCount();

	m_bDeleteLastKey = false;

	//numeric button? (0, 1, ... 9)
	if(IsNumberKey(KeyCode))
	{
		//only digits
		if(m_CaptureKeyboardParam.bTypePin || m_CaptureKeyboardParam.bNumbersOnly)
			return '0' + NumberKeyIndex(KeyCode);

		int KeyIndex = NumberKeyIndex(m_nLastKeyCode);
		if(KeyIndex == 0 || KeyIndex == 1)
		{
			KeyChar = KeysMap[KeyIndex][0];
		}

		//first key ? start the timer!
		if(m_nLastKeyCode == 0) 
		{
			m_nRepeatStep = 0;
			::KillTimer(m_hWnd, KeyTimerId);
			::SetTimer(m_hWnd, KeyTimerId, nDiff, KeyTimerCallBack);
			KeyChar = KeysMap[NumberKeyIndex(KeyCode)][m_nRepeatStep];
		}
		else
		{
			//different button then last one or time's up - stop the timer, we have a key
			if(m_nLastKeyCode != KeyCode || nNow - m_nLastTick > nDiff)
			{
				if(m_nLastKeyCode != KeyCode)
					m_nRepeatStep = 0;
				else
					m_nRepeatStep %= KeysMapIndex[NumberKeyIndex(KeyCode)];

				KeyChar = KeysMap[NumberKeyIndex(KeyCode)][m_nRepeatStep];

				::KillTimer(m_hWnd, KeyTimerId);
				m_nRepeatStep = 0;
				m_nLastTick = 0;
				m_nLastKeyCode = 0;
			}
			else //same button, just shift keys and start again the timer
			{
				m_bDeleteLastKey = true;
				m_nRepeatStep++;
				m_nRepeatStep %= KeysMapIndex[NumberKeyIndex(KeyCode)];
				::KillTimer(m_hWnd, KeyTimerId);
				::SetTimer(m_hWnd, KeyTimerId, nDiff, KeyTimerCallBack);
				KeyChar = KeysMap[NumberKeyIndex(KeyCode)][m_nRepeatStep];
			}
		}
		
		m_nLastKeyCode = KeyCode;
		m_nLastTick = nNow;
	}
	else 
		if(m_nLastKeyCode)
		{
			KeyChar = KeysMap[NumberKeyIndex(m_nLastKeyCode)][m_nRepeatStep];

			m_nLastKeyCode = 0;
			m_nLastTick = 0;
		}

	return KeyChar;
}
//---------------------------------------------------------------------------------------------------------
int OrbiterApp::NumberKeyIndex(int KeyCode)
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
//---------------------------------------------------------------------------------------------------------
bool OrbiterApp::IsNumberKey(int KeyCode)
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
//---------------------------------------------------------------------------------------------------------
bool OrbiterApp::SetVariable()
{
	if(m_CaptureKeyboardParam.iVariable != 0)
	{
		if(m_pBDCommandProcessor->m_bClientConnected)
		{
			BDCommand *pCommand = new BD_PC_SetVariable(m_CaptureKeyboardParam.iVariable, m_CaptureKeyboardParam.sVariableValue);
			m_pBDCommandProcessor->AddCommand(pCommand);
		}
	}

	return false;
}
//---------------------------------------------------------------------------------------------------------
bool OrbiterApp::ClearEdit()
{
	if(m_CaptureKeyboardParam.sVariableValue.length() == 0)
		return false;

	string s = m_CaptureKeyboardParam.sVariableValue;
	m_CaptureKeyboardParam.sVariableValue = "";

	for(int i = 0; i < s.length() - 1; i++)
		m_CaptureKeyboardParam.sVariableValue += s[i];

	ScrollListPartialMatch();

	return true;
}
//------------------------------------------------------------------------------------------------------------------
bool OrbiterApp::ClearAllEdit()
{
	if(m_CaptureKeyboardParam.sVariableValue.length() == 0)
		return false;

	m_CaptureKeyboardParam.sVariableValue = "";
	ScrollListPartialMatch();

	return true;
}
//------------------------------------------------------------------------------------------------------------------
bool OrbiterApp::ScrollListUp()
{
	if(m_ulGridSelectedItem > 0)
	{
		m_bRedrawOnlyGrid = true;
		m_ulGridSelectedItem--;
		if(m_bGridSendSelectedOnMove)
			SelectCurrentItem();

		return true;
	}
	else
		return false;
}
//---------------------------------------------------------------------------------------------------------
bool OrbiterApp::ScrollListDown()
{
	if(m_ulGridSelectedItem < m_vectDataGrid.size() - 1)
	{
		m_bRedrawOnlyGrid = true;
		m_ulGridSelectedItem++;
		if(m_bGridSendSelectedOnMove)
			SelectCurrentItem(); 
		
		return true;
	}
	else
		return false;
}
//---------------------------------------------------------------------------------------------------------
bool OrbiterApp::SelectCurrentItem()
{
	if(m_pBDCommandProcessor->m_bClientConnected)
	{
		BDCommand *pCommand = new BD_PC_SelectedFromList(m_ulGridSelectedItem); 
		m_pBDCommandProcessor->AddCommand(pCommand);
	}

	return false; //don't redraw
}
//---------------------------------------------------------------------------------------------------------
void CALLBACK ScrollTimerCallBack(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	OrbiterApp *pOrbiter = OrbiterApp::GetInstance();

	if(pOrbiter->IsScrollingUp())
	{
		if(pOrbiter->ScrollListUp())
			pOrbiter->RefreshScreen();
	}
	else
	{
		if(pOrbiter->ScrollListDown())
			pOrbiter->RefreshScreen();
	}
}
//---------------------------------------------------------------------------------------------------------
void OrbiterApp::AutomaticallyScrollDataGrid(bool bKeyUp)
{
	::KillTimer(m_hWnd, ScrollTimerId);

	m_bScrollUp = bKeyUp;
	::SetTimer(m_hWnd, ScrollTimerId, 250, ScrollTimerCallBack);
	
}
//------------------------------------------------------------------------------------------------------------------
void OrbiterApp::SetCurrentSignalStrength(int nSignalStrength) 
{ 
	m_nSignalStrength = nSignalStrength; 

	if(m_nSignalStrength)
	{
		m_bRender_SignalStrengthOnly = true;
		RefreshScreen();
	}
}
//------------------------------------------------------------------------------------------------------------------
void OrbiterApp::SendKeyStrokes(bool bValue)
{
	m_bSendKeyStrokes = bValue;
}
//------------------------------------------------------------------------------------------------------------------
void OrbiterApp::SaveFile(unsigned long ulFileNameSize, char *pFileName, unsigned long ulFileDataSize, char *pFileData)
{
	string sFileName = g_sBinaryPath + pFileName;
	if(!FileUtils::WriteBufferIntoFile(sFileName, pFileData, ulFileDataSize))
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Failed to save %s file", sFileName);
}
//------------------------------------------------------------------------------------------------------------------
void OrbiterApp::ShowDisconnected()
{
	PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);

	#ifndef TEST_DATAGRID
	Reset();
	#endif

    string sLogoPath = g_sBinaryPath + "logo.gif";
    sLogoPath = StringUtils::Replace(sLogoPath, "/", "\\");
	DrawImage(sLogoPath.c_str(), 0, 0, m_nImageWidth, m_nImageHeight);

	TryToUpdate();
}
//------------------------------------------------------------------------------------------------------------------
void OrbiterApp::SetCaptureKeyboard(bool bOnOff, bool bDataGrid, bool bReset, int nEditType, int nVariable, string sText)
{
	m_CaptureKeyboardParam.bOnOff = bOnOff;

	if(bOnOff)
	{
		m_CaptureKeyboardParam.bDataGrid = bDataGrid;
		m_CaptureKeyboardParam.bReset = bReset;

		m_CaptureKeyboardParam.bTypePin = 2 == nEditType; 
		m_CaptureKeyboardParam.bNumbersOnly = 1 == nEditType;

		m_CaptureKeyboardParam.iVariable = nVariable;

		unsigned int msgpos = 0;
		string token;

		m_CaptureKeyboardParam.bTextBox = 0 != sText.length();

		if((token = StringUtils::Tokenize(sText, ",", msgpos)) != "") 	
			m_CaptureKeyboardParam.TextX = atoi(token.c_str());

		if((token = StringUtils::Tokenize(sText, ",", msgpos)) != "") 	
			m_CaptureKeyboardParam.TextY = atoi(token.c_str());

		if((token = StringUtils::Tokenize(sText, ",", msgpos)) != "") 	
			m_CaptureKeyboardParam.TextWidth = atoi(token.c_str());

		if((token = StringUtils::Tokenize(sText, ",", msgpos)) != "") 	
			m_CaptureKeyboardParam.TextHeight = atoi(token.c_str());
	}
	else
	{
		m_CaptureKeyboardParam.bDataGrid = false;
		m_CaptureKeyboardParam.bReset = false;
		m_CaptureKeyboardParam.bTypePin = false;
		m_CaptureKeyboardParam.bTextBox = false;

		m_CaptureKeyboardParam.iVariable = 0;
	}

	m_CaptureKeyboardParam.Reset();
}
//------------------------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterApp::Redraw()
{
	RefreshScreen();
}
//------------------------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterApp::MessageBox(const char *Message,const char *Title)
{
//	::MessageBox(0, Title, Message, 0);
}
//------------------------------------------------------------------------------------------------------------------
/*virtual*/ bool OrbiterApp::DrawImage(const char *Filename,int X,int Y,int Width,int Height)
{
	size_t nSize = 0;
	char *pData = FileUtils::ReadFileIntoBuffer(Filename, nSize);

	if(pData)
	{
		DrawImage(0, pData, nSize, X, Y, Width, Height);
		delete pData;
	}
	else 
		return false;

	return true;
}
//------------------------------------------------------------------------------------------------------------------
/*virtual*/ bool OrbiterApp::DrawImage(unsigned char ImageType,void *pGraphic,int GraphicSize,int X,int Y,int Width,int Height)
{
	RenderImage(ImageType, GraphicSize, (char *)pGraphic, X, Y, Width, Height);

	return true;
}
//------------------------------------------------------------------------------------------------------------------
/*virtual*/ bool OrbiterApp::Draw3dRect(MyRect &r)
{
	return true;
}
//------------------------------------------------------------------------------------------------------------------
/*virtual*/ bool OrbiterApp::DrawText(const char *Text, MyRect &r)
{
	PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);

	HDC hdc = GetDisplay()->GetBackBuffer()->GetDC(false);

	RECT rectLocation = { r.left, r.top, r.right, r.bottom };
	//DrawTextExUTF8(hdc, Text, -1, &rectLocation, 
	//	DT_WORDBREAK | DT_NOPREFIX, NULL); 

	GetDisplay()->GetBackBuffer()->ReleaseDC(hdc);
	return true;
}
//------------------------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterApp::SetTextProperties(int Size,const char *Font,int R, int G, int B)
{
	PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);

	HDC hdc = GetDisplay()->GetBackBuffer()->GetDC(false);

 	::SetTextColor(hdc, RGB(R, G, B));
	::SetBkMode(hdc, TRANSPARENT);

	GetDisplay()->GetBackBuffer()->ReleaseDC(hdc);
}
//------------------------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterApp::SwitchToMenu(VIPMenu *pMenu)
{
}
//------------------------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterApp::OpenProgram(string ProgramName)
{
}
//------------------------------------------------------------------------------------------------------------------
void OrbiterApp::RenderEditBox()
{
	int EditX = m_CaptureKeyboardParam.TextX;
	int EditY = m_CaptureKeyboardParam.TextY;
	int EditWidth = m_CaptureKeyboardParam.TextWidth;
	int EditHeight = m_CaptureKeyboardParam.TextHeight;

	GetDisplay()->FillRect(EditX, EditY, EditX + EditWidth, EditY + EditHeight, GetColor16(black));
	GetDisplay()->FillRect(EditX + 1, EditY + 1, EditX + EditWidth - 1, EditY + EditHeight - 1, GetColor16(blue_lite));

	string sTextToRender;
	if(m_CaptureKeyboardParam.bTypePin)
	{
		int nLen = m_CaptureKeyboardParam.sVariableValue.length();
		while(nLen--)
			sTextToRender += "*";
	}
	else
		sTextToRender = m_CaptureKeyboardParam.sVariableValue;

	RenderText(sTextToRender, EditX, EditY, EditX + EditWidth, EditY + EditHeight, black);
}
//------------------------------------------------------------------------------------------------------------------
void OrbiterApp::Hide()
{	
	LocalRenderer::Enable( false );

	::MessageBeep(MB_ICONQUESTION);
	Sleep(50);
	::MessageBeep(MB_ICONHAND);
	Sleep(50);
	::MessageBeep(MB_ICONASTERISK);

	::ShowWindow(m_hWnd, SW_HIDE);

	Sleep(300);
	::SetForegroundWindow(::GetDesktopWindow());
	::ShowWindow(::GetDesktopWindow(), SW_SHOW);
	

	#if defined(SMARTPHONE2005) && defined(_TEST_KEYBOARD_HOOK) 	//--- CHANGED4WM5 ----//
		SetKeybdHook( true );
	#endif
}
//------------------------------------------------------------------------------------------------------------------
void OrbiterApp::Show()
{	
	Sleep(50);
	::ShowWindow(::GetDesktopWindow(), SW_HIDE);
	::SetForegroundWindow(m_hWnd);
	::ShowWindow(m_hWnd, SW_SHOW);

	::MessageBeep(MB_ICONASTERISK);
	Sleep(50);
	::MessageBeep(MB_ICONHAND);
	Sleep(50);
	::MessageBeep(MB_ICONQUESTION);

	LocalRenderer::Enable( );

	#if defined(SMARTPHONE2005) && defined(_TEST_KEYBOARD_HOOK) 	//--- CHANGED4WM5 ----//
		SetKeybdHook( );
	#endif
}
//------------------------------------------------------------------------------------------------------------------
void OrbiterApp::SetImageQuality(unsigned long ulImageQuality)
{
	m_ulImageQuality = ulImageQuality;
}
//------------------------------------------------------------------------------------------------------------------
void OrbiterApp::SetBkgndImage( unsigned char nImageType, int nSize, char *pData )
{
	if(NULL != m_pBkgndImage_Data)
	{
		delete[] m_pBkgndImage_Data;
		m_pBkgndImage_Data = NULL;
	}

	m_nBkgndImageType = nImageType; 
	m_pBkgndImage_Size = nSize;

	m_pBkgndImage_Data = new char[nSize];
	memcpy(m_pBkgndImage_Data, pData, nSize);

	m_bMainMenuRepaint = true;
	m_pBkgndImage_Repaint = true;

	RefreshScreen();
}
//------------------------------------------------------------------------------------------------------------------

void OrbiterApp::CheckBookmarks( void )
{
	
	string sFileName = g_sBinaryPath + CONFIG_FILENAME;
	FILE *hFile = fopen( sFileName.c_str(), "rt" );
	if ( !hFile ) return;
	TCHAR *szURL = new TCHAR[MAX_URL_LENGTH];
	if (  szURL ) {
		if ( _fgetts( szURL, MAX_URL_LENGTH-2, hFile ) ) {
			if ( szURL[_tcslen(szURL)-1]==_T('\n') ) szURL[_tcslen(szURL)-1] = 0;
			//Pluto - http://34.34.23.34/lmce-admin/check.wml?
			CSmartphone2003Favorites::AddLinkToFavorites( _T(DEFAULT_LINK_NAME), szURL, FALSE );
			//Pluto Alerts - http://34.34.23.34/lmce-admin/check.wml?security=0		
			_tcscat(szURL, _T(ALERTS_LINK_SUBSTR));		
			CSmartphone2003Favorites::AddLinkToFavorites( _T(ALERTS_LINK_NAME), szURL, FALSE );
		}
		delete [MAX_URL_LENGTH] szURL;
	}
	fclose( hFile ); 	
	
}

#if defined(SMARTPHONE2005) && defined(_TEST_KEYBOARD_HOOK) 	//--- CHANGED4WM5 ----//


const int WH_KEYBOARD_LL = 20;

typedef LRESULT (CALLBACK* HookProc)(int nCode, WPARAM wParam, LPARAM lParam);

extern "C" __declspec(dllimport) 
	HHOOK WINAPI SetWindowsHookExW( int idHook, HookProc hookProc, HINSTANCE hMod, DWORD dwThreadId );
extern "C" __declspec(dllimport) 
	LRESULT WINAPI CallNextHookEx( HHOOK hhk, int nCode, WPARAM wParam, LPARAM lParam );
extern "C" __declspec(dllimport) 
	BOOL WINAPI UnhookWindowsHookEx(HHOOK hhk);


HHOOK hHook;
OrbiterApp *pInst;

LRESULT CALLBACK HookProcedure(int nCode, WPARAM wParam, LPARAM lParam)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"HookProcedure( %d, %d, %d )", nCode, wParam, lParam );
	if ( nCode>0 ) return CallNextHookEx(hHook, nCode, wParam, lParam);

	return CallNextHookEx(hHook, nCode, wParam, lParam);
}

void OrbiterApp::SetKeybdHook( bool bClear )
{
	if ( bClear ) {
		if ( hHook ) {
			UnhookWindowsHookEx( hHook );			
			hHook = NULL;
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"OrbiterApp::SetkeybdHook - Hook removed!");
		}
	}
	else {
		pInst = this;
		//hHook = SetWindowsHookExW(WH_KEYBOARD_LL, HookProcedure, GetModuleHandle(NULL), 0);
		if ( !hHook )
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"OrbiterApp::SetkeybdHook - Hooking err!");
		else 
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"OrbiterApp::SetkeybdHook - Hook installed!");
	}
	
}
#endif


/*
 

 


       

 

       private struct KBDLLHOOKSTRUCT

       {

           public int vkCode;

           public int scanCode;

           public int flags;

           public int time;

           public IntPtr dwExtraInfo;

       }

 

       
LRESULT OrbiterApp::OnCancelMode( UINT msg, WPARAM wparam, LPARAM lparam, BOOL& bHandled )
{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"OrbiterApp::OnCancelMode");
#endif
	if ( !m_bQuit_get()) {
		bHandled = TRUE;
		return 0;
	}
	bHandled = FALSE;
	return 1;
}
*/
//------------------------------------------------------------------------------------------------------------------
TCHAR CSmartphone2003Favorites::m_szBuffer[MAX_PATH];

bool CSmartphone2003Favorites::MakeLinkFileName(LPCTSTR pszName, LPTSTR pszBuf)
{
	// Get path to Favorites folder.
	if(!SHGetSpecialFolderPath(NULL, pszBuf, CSIDL_FAVORITES, TRUE))
		return false;

	// Make file name.
	_tcscat(pszBuf, _T("\\"));
	_tcscat(pszBuf, pszName);
	_tcscat(pszBuf, _T(".url"));

	return true;
}

bool CSmartphone2003Favorites::CheckURLName( LPCTSTR pszURLName )
{	
	if ( !MakeLinkFileName( pszURLName, m_szBuffer ) ) return FALSE;

	WIN32_FIND_DATA fData;	
	HANDLE hSearch = FindFirstFile( m_szBuffer, &fData );
	if ( hSearch!=INVALID_HANDLE_VALUE ) return true;
	return false;
}

bool CSmartphone2003Favorites::AddLinkToFavorites(LPCTSTR pszName, LPCTSTR pszURL, BOOL bFailIfExists)
{
	if ( CheckURLName( pszName ) ) {
		if ( bFailIfExists ) return false;
		DeleteFile(m_szBuffer);
	}

	// Get link file name.	
	if(!MakeLinkFileName(pszName, m_szBuffer)) return FALSE;
	
	// Create *.url file.
	FILE *pFile = _tfopen(m_szBuffer, _T("wt"));
	if(pFile == NULL)
		return false;

	_fputts(_T("[InternetShortcut]\n"), pFile);
	TCHAR *pBuf = new TCHAR[_tcslen(pszURL)+10];
	if ( pBuf ) {
		_stprintf( pBuf, _T("URL=%s"), pszURL);		
		_fputts( pBuf, pFile );			
		delete [_tcslen(pszURL)+10] pBuf;
		fclose(pFile);
	}
	else {
		fclose(pFile);
		return false;
	}	

	return true;
}

bool CSmartphone2003Favorites::DelLinkFromFavorites(LPCTSTR pszName)
{
	if ( CheckURLName( pszName ) ) {
		// Get link file name.
		if(!MakeLinkFileName(pszName, m_szBuffer))
			return false;

		DeleteFile(m_szBuffer);
		return true;
	}
	return true;
}

//------------------------------------------------------------------------------------------------------------------

void OrbiterApp::SetMenuData( MenuData& data )
{
	#ifdef _LOCAL_RENDERED_OBJECTS_
		PLUTO_SAFE_DELETE( m_pMainMenu )
		m_pMainMenu = data.CreateMainMenu();
		if ( m_pMainMenu ) {
			RECT rr = {0, 0, APP_WIDTH, APP_HEIGHT};
			RECT r = {5,5,APP_WIDTH-5,APP_HEIGHT-5};
			LocalRenderer::SetAppViewport( rr );
			m_pMainMenu->SetViewport( r );
			m_pMainMenu->Show(0,0);
		}

	#endif
}

//------------------------------------------------------------------------------------------------------------------

void OrbiterApp::SendConfiguration( void )
{
	
	#ifdef _LOCAL_RENDERED_OBJECTS_
		if(!m_pBDCommandProcessor->m_bClientConnected )
			return;

		PhoneConfig cfg;
		cfg.SetMenuMode( PhoneConfig::mmMenu );
		BDCommand *pCommand = new BD_PC_Configure( cfg );
		m_pBDCommandProcessor->AddCommand(pCommand);
	#endif
	
}

//------------------------------------------------------------------------------------------------------------------

void OrbiterApp::SendSelectedItem( string sItemId )
{
	if(!m_pBDCommandProcessor->m_bClientConnected )
		return;

	BDCommand *pCommand = new BD_PC_SelectedItem( sItemId );
	m_pBDCommandProcessor->AddCommand(pCommand);
}

//------------------------------------------------------------------------------------------------------------------
void OrbiterApp::SendMouseEvent( int iX, int iY, int EventType )
{
	if(!m_pBDCommandProcessor->m_bClientConnected )
		return;

	BDCommand *pCommand = new BD_PC_MouseEvent( iX, iY, (BD_PC_MouseEvent::MouseEventType)EventType );
	m_pBDCommandProcessor->AddCommand(pCommand);
}
//------------------------------------------------------------------------------------------------------------------
bool OrbiterApp::HandleDataGridStylus( int iX, int iY )
{
	if ( iX<=m_ulGridX || iX>=m_ulGridX+m_ulGridWidth ) return false;
	if ( iY<=m_ulGridY || iY>=m_ulGridY+m_ulGridHeight ) return false;

	unsigned long ulGridBottomItem = 0;
	int nRowHeight = 0;
	CalcDataGridItemsLayout( ulGridBottomItem, nRowHeight );

	// Check for stylus inside scroll buttons
	if ( iY > m_ulGridY && 
		 iY < m_ulGridY+DEFAULT_DATAGRID_SCROLLWIDTH &&
		 iX > m_ulGridX + m_ulGridWidth - DEFAULT_DATAGRID_SCROLLWIDTH && 
		 iX < m_ulGridX + m_ulGridWidth ) 
	{	
		if ( m_ulGridSelectedItem > 0 ) {
			m_ulGridSelectedItem--;
			SelectCurrentItem();
		}
		return true;
	}
	if ( iY > m_ulGridY + m_ulGridHeight - DEFAULT_DATAGRID_SCROLLWIDTH && 
		 iY < m_ulGridY + m_ulGridHeight &&
		 iX > m_ulGridX + m_ulGridWidth - DEFAULT_DATAGRID_SCROLLWIDTH && 
		 iX < m_ulGridX + m_ulGridWidth ) 
	{	
		if ( m_ulGridSelectedItem < m_vectDataGrid.size()-1 ) {
			m_ulGridSelectedItem++;
			SelectCurrentItem();
		}
		return true;
	}

	// Check for stylus inside items
	int nCrtPos = m_ulGridY;
	for(int i = m_ulGridTopItem; i < ulGridBottomItem; i++){
		if ( iY>nCrtPos && iY<nCrtPos+nRowHeight &&
			iX<m_ulGridX+m_ulGridWidth-DEFAULT_DATAGRID_SCROLLWIDTH ) {	
			m_ulGridSelectedItem = i;
			SelectCurrentItem();
			SendKey( BUTTON_Enter_CONST, 2 );
			return true;
		}
		nCrtPos += nRowHeight;
	}
	return false;
}
//------------------------------------------------------------------------------------------------------------------
/*
 *	Copied from RenderDataGrid
 */ 
void OrbiterApp::CalcDataGridItemsLayout( unsigned long &ulBottomItem, int &nRowHeight )
{
	nRowHeight = DEFAULT_DATAGRID_ROWHEIGHT;
	int nVisibleItems = m_ulGridHeight / nRowHeight;
	nRowHeight = m_ulGridHeight / nVisibleItems;

	int i = 0;
	//let's see how many items are visible right now
	for(i = m_ulGridTopItem; i < m_ulGridTopItem + nVisibleItems; i++)
	{
		if(i >= m_vectDataGrid.size()) 
			break;

		string s = m_vectDataGrid[i];
		if(s.find('\n') != string::npos)
			nVisibleItems--;
	}

	//scroll the grid if need it
	while(m_ulGridTopItem + nVisibleItems - 1 < m_ulGridSelectedItem)
		m_ulGridTopItem++;
	while(m_ulGridTopItem > m_ulGridSelectedItem)
		m_ulGridTopItem--;

	//let's see now how many items are visible
	nVisibleItems = m_ulGridHeight / nRowHeight;
	for(i = m_ulGridTopItem; i < m_ulGridTopItem + nVisibleItems; i++)
	{
		if(i >= m_vectDataGrid.size()) 
			break;

		string s = m_vectDataGrid[i];
		if(s.find('\n') != string::npos)
			nVisibleItems--;
	}
	
	ulBottomItem = m_ulGridTopItem + nVisibleItems;

	if( ulBottomItem > m_vectDataGrid.size() )	ulBottomItem = m_vectDataGrid.size();	
}
//------------------------------------------------------------------------------------------------------------------
LRESULT OrbiterApp::OnTimer( UINT msg, WPARAM wparam, LPARAM lparam, BOOL& bHandled )
{
	if ( wparam==SLEEP_TIMER_ID ) {
		bHandled = TRUE;
		m_bTimerUp = false;
		KillTimer( SLEEP_TIMER_ID );
		if(!m_pBDCommandProcessor->m_bClientConnected ) 
			Hide();
	}
	return 0;
}
//------------------------------------------------------------------------------------------------------------------
void OrbiterApp::Connected( void )
{
	if ( m_bTimerUp ) {
		m_bTimerUp = false;
		KillTimer( SLEEP_TIMER_ID );		
	}
}
//-----------------------------------------------------------------------------------------------------------------
void OrbiterApp::ShowMenu( long nCrtRoom )
{
	if ( m_pMainMenu ) 
		if ( !m_pMainMenu ->IsShowing() ) {
			Reset();
			if ( m_pBkgndImage_Data ) {
				m_pBkgndImage_Repaint = true;
				RefreshScreen();
			}
			LocalRenderer::Enable( );

			LRPhoneMenu* pPhoneMenu = dynamic_cast<LRPhoneMenu*>(m_pMainMenu);
			if ( pPhoneMenu ) {
				pPhoneMenu->SetCrtRoom( nCrtRoom );
			}
		}
	
}
//-----------------------------------------------------------------------------------------------------------------
void OrbiterApp::GetMenuImages( MenuItemInfo::ItemType nItemsType, vector<MenuItemInfo>& vItems )
{
	
	if(!m_pBDCommandProcessor->m_bClientConnected )
		return;

	BDCommand *pCommand = new BD_PC_GetMenuImages( nItemsType, vItems );
	m_pBDCommandProcessor->AddCommand(pCommand);		
}
//-----------------------------------------------------------------------------------------------------------------
void OrbiterApp::SetMenuImages( MenuItemInfo::ItemType nItemsType, vector<MenuItemInfo*>& vItems )
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"SetMenuImages %d %d",nItemsType, vItems.size() );
	if ( m_pMainMenu ) {
		//PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);
		((LRPhoneMenu*)m_pMainMenu)->SetMenuImages( nItemsType, vItems );
	}

}
//-----------------------------------------------------------------------------------------------------------------

LRESULT OrbiterApp::OnActivate( UINT msg, WPARAM wparam, LPARAM lparam, BOOL& bHandled )
{
	BOOL fActive = LOWORD(wparam);
	LocalRenderer::Enable( fActive );
	if ( fActive ) {
		if ( m_pMainMenu && m_pMainMenu->IsShowing() ) {
			m_pMainMenu->SetDirty();
			m_pMainMenu->Paint();
		}
	}

	return 0;
}
