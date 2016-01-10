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
#include "CTestz.h"
#include "AppResources.h"

CTestz* CTestz::m_pInstance = NULL;

CTestz::CTestz()
{
	m_hScreenBuffer = NULL;
	m_pInstance = this;
}

CTestz::~CTestz()
{
	if ( m_hScreenBuffer ) {
		WinDeleteWindow( m_hScreenBuffer, false );
		m_hScreenBuffer = NULL;
	}
	WinSetCoordinateSystem( m_ui16CoordSys );
}


void CTestz::EventLoop()
{
	Err			error;
	EventType	event;
	do {
		EvtGetEvent(&event, evtWaitForever);

		if (SysHandleEvent(&event))
			continue;
			
		if (MenuHandleEvent(0, &event, &error))
			continue;
			
		if (AppHandleEvent(&event))
			continue;

		FrmDispatchEvent(&event);

	} while (event.eType != appStopEvent);
}


void CTestz::Configure()
{
	// this enables a 1-to-1 correspondence between coordinates and pixels.
	m_ui16CoordSys = WinGetCoordinateSystem();
	WinSetCoordinateSystem( kCoordinatesNative );
	
	//For Palm OS Garnet, only kCoordinatesNative, kCoordinatesStandard, and kCoordinatesDouble are supported.
	
	/*
	Err err= FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);
	if ( !err ) {
		//The romVersion parameter should be greater than or equal to 0x05403000
		//sysMakeROMVersion(5,4,0,sysROMStageRelease,0)
		//This feature set corresponds to version 5.4 of Palm OS Garnet.
	}
	*/
	Err err;
	err = WinScreenMode( winScreenModeGet, &m_ui32Width, &m_ui32Height, &m_ui32Depth, NULL );
	#ifdef _DOUBLEBUFFERING_
  	  UInt16 error;
	  m_hScreenBuffer = WinCreateOffscreenWindow( m_ui32Width, m_ui32Height, nativeFormat, &error );
	  WinGetDrawWindowBounds( &m_rBufferBounds );	  
	#endif.
	WinSetCoordinateSystem( m_ui16CoordSys );
}

void CTestz::erase( void )
{
  if ( m_hScreenBuffer ) {
  	m_hScreen = WinGetDrawWindow();
  	WinSetDrawWindow( m_hScreenBuffer );
  	WinEraseRectangle(&m_rBufferBounds, 0);
  	WinSetDrawWindow( m_hScreen );
  	DrawBufferToScreen();
  }  
}

void CTestz::repaint( void )
{
  if ( m_hScreenBuffer ) {
  	DrawBufferToScreen();
  }  
}

void CTestz::SetScreenBuffer( bool bActive )
{
	if ( !m_hScreenBuffer ) return;						
	if ( bActive ) {
		WinSetCoordinateSystem( kCoordinatesNative );
		m_hScreen = WinGetDrawWindow();
		WinGetDrawWindowBounds( &m_rScreenBounds );
		WinSetDrawWindow( m_hScreenBuffer );				
	}
	else {
		WinSetDrawWindow( m_hScreen );
		WinSetCoordinateSystem( m_ui16CoordSys );
	}						
}

void CTestz::DrawBufferToScreen( void )
{
    if ( !m_hScreenBuffer ) return;	
    WinSetCoordinateSystem( kCoordinatesNative );
    
    if ( !m_hScreen ) {
    	m_hScreen = WinGetDrawWindow( );
    	WinGetDrawWindowBounds( &m_rScreenBounds );
    }
	WinCopyRectangle( m_hScreenBuffer, m_hScreen, &m_rBufferBounds, 
	                  m_rScreenBounds.topLeft.x, m_rScreenBounds.topLeft.y, winPaint );
	WinSetCoordinateSystem( m_ui16CoordSys );
}

void CTestz::SolidRectangle(int iX, int iY, int iWidth, int iHeight, PlutoColor color )
{
	SetScreenBuffer();
	
	WinPushDrawState();
	RectangleType rP;	
	rP.topLeft.x = iX;
	rP.topLeft.y = iY;
	rP.extent.x = iWidth;
	rP.extent.y = iHeight;

	RGBColorType prevRgb, rgbColor;
	PlutoColorToRGBColorType( rgbColor, color );
	WinSetForeColorRGB( &rgbColor, &prevRgb);

//  WinDrawRectangle always uses winPaint mode
//    for other modes use WinPaintRectangle ( which uses current drawing state )
	WinDrawRectangle( &rP, 0 );	// for round corners != 0
	
	WinPopDrawState();
	
	SetScreenBuffer( false );
	DrawBufferToScreen();
}

void CTestz::HollowRectangle(int X, int Y, int Width, int Height, PlutoColor color )
{
	SetScreenBuffer();
	
	WinPushDrawState();
	
	RectangleType rP;	
	rP.topLeft.x = X;
	rP.topLeft.y = Y;
	rP.extent.x = Width;
	rP.extent.y = Height;

	RGBColorType prevRgb, rgbColor;
	PlutoColorToRGBColorType( rgbColor, color );
	WinSetForeColorRGB( &rgbColor, &prevRgb);

//  WinDrawRectangleFrame always uses winPaint mode
//    for other modes use WinPaintRectangleFrame ( which uses current drawing state )
//  FrameType = ( noFrame, simpleFrame, rectangleFrame, simple3DFrame, roundFrame, 
//                boldRoundFrame, popupFrame, dialogFrame, menuFrame  )
	WinDrawRectangleFrame( simpleFrame, &rP );	// for round corners != 0
	
	WinPopDrawState();

	SetScreenBuffer( false );
	DrawBufferToScreen();
}


void CTestz::DrawLine(int iX, int iY, int iWidth, int iHeight, PlutoColor color )
{	
	SetScreenBuffer();
	
	WinPushDrawState();

	RGBColorType prevRgb, rgbColor;
	PlutoColorToRGBColorType( rgbColor, color );
	WinSetForeColorRGB( &rgbColor, &prevRgb);

//  WinDrawLine always uses winPaint mode
//    for other modes use WinPaintLine ( which uses current drawing state )
	WinDrawLine( iX, iY, iWidth, iHeight );	
	
	WinPopDrawState();	

	SetScreenBuffer( false );
	DrawBufferToScreen();
}

void CTestz::RenderText( string &sTextToDisplay, class TextStyle *pTextStyle, PlutoPoint point )
{
	SetScreenBuffer();
	
	WinPushDrawState();

	//RGBColorType prevRgb;
	//WinSetForeColorRGB( &rgbColor, &prevRgb);

    if ( pTextStyle ) {
    	if ( pTextStyle->m_bUnderline ) {
    		// UnderlineModeType = ( noUnderline, grayUnderline, solidUnderline, colorUnderline )
    		WinSetUnderlineMode( solidUnderline );
    	}
    	if ( pTextStyle->m_bBold ) {
    		FntSetFont( boldFont );
    	}
    	RGBColorType prevRgb, rgbColor;
    	PlutoColorToRGBColorType( rgbColor, pTextStyle->m_ForeColor );    	
    	WinSetForeColorRGB( &rgbColor, &prevRgb);
    	WinSetTextColorRGB( &rgbColor, &prevRgb);
    	PlutoColorToRGBColorType( rgbColor, pTextStyle->m_BackColor );    	
    	WinSetBackColorRGB( &rgbColor, &prevRgb);
    }
//  WinDrawChars always uses winPaint mode
//    for other modes use WinPaintChars ( which uses current drawing state )
	WinDrawChars( sTextToDisplay.c_str(), sTextToDisplay.length(), point.X, point.Y );	
	
	WinPopDrawState();	

	SetScreenBuffer( false );
	DrawBufferToScreen();
}


Boolean CTestz::MainFormDoCommand(UInt16 command)
{
	Boolean handled = false;
	FormType * pForm;

	switch (command) {
		case MainOptionsAboutStarterApp:
			pForm = FrmInitForm(AboutForm);
			FrmDoDialog(pForm);					// Display the About Box.
			FrmDeleteForm(pForm);
			handled = true;
			break;

	}
	
	return handled;
}




Boolean CTestz::MainFormHandleEvent(EventType* pEvent)
{
	if ( !m_pInstance ) return false;
	Boolean 	handled = false;
	FormType* 	pForm;
	
	string s = "xxx", s1="abcdef";
	TextStyle ts;
	ts.m_bBold = true;
	ts.m_bUnderline = true;
	ts.m_ForeColor = PlutoColor::Blue();
	ts.m_BackColor = PlutoColor::Gray();
	

	switch (pEvent->eType) {
		case menuEvent:
			return m_pInstance->MainFormDoCommand(pEvent->data.menu.itemID);

		case frmOpenEvent:
			pForm = FrmGetActiveForm();
			FrmDrawForm(pForm);			
			m_pInstance->Configure();
			//test.erase( );
						

			m_pInstance->SolidRectangle( 0, 0, 40, 40, PlutoColor::Red() );
			
			m_pInstance->SolidRectangle( 0, 40, 40, 40, PlutoColor::Green() );
			m_pInstance->SolidRectangle( 0, 80, 40, 40, PlutoColor::Blue() );
			m_pInstance->DrawLine( 40, 0, 80, 40, PlutoColor(255,255,0) );
			m_pInstance->DrawLine( 40, 40, 80, 0, PlutoColor::Gray() );
			m_pInstance->HollowRectangle( 80, 80, 40, 40, PlutoColor::Black() );
			m_pInstance->RenderText( s, &ts, PlutoPoint( 120, 120 ) );
			ts.m_ForeColor = PlutoColor::Red();
			ts.m_bBold = false;
			ts.m_bUnderline = false;
			m_pInstance->RenderText( s1, &ts, PlutoPoint( 200, 240 ) );
			m_pInstance->test_bmp();
			
			handled = true;
			break;
			
		case frmUpdateEvent:
			pForm = FrmGetActiveForm();
			FrmDrawForm(pForm);			
		    m_pInstance->repaint();
		    handled = true;
		    break;
			
		default:
			break;
	}
	
	return handled;
}

Boolean CTestz::AppHandleEvent(EventType* pEvent)
{
	UInt16 		formId;
	FormType* 	pForm;
	Boolean		handled = false;

	if (pEvent->eType == frmLoadEvent) {
		// Load the form resource.
		formId = pEvent->data.frmLoad.formID;
		
		pForm = FrmInitForm(formId);
		FrmSetActiveForm(pForm);

		// Set the event handler for the form.  The handler of the currently
		// active form is called by FrmHandleEvent each time is receives an
		// event.
		switch (formId) {
			case MainForm:
				FrmSetEventHandler(pForm, MainFormHandleEvent);
				break;

			default:
				break;
		}
		handled = true;
	}
	
	return handled;
}



void CTestz::test_bmp( void )
{
	SetScreenBuffer();
	UInt16 error;
	BitmapType *bmp = BmpCreate( 20, 20, 16, NULL, &error );
	if ( bmp ) {
		BitmapTypeV3 *bmp3 = BmpCreateBitmapV3( bmp, kDensityDouble, BmpGetBits(bmp), NULL );
		if ( bmp3 ) {
			UInt16 *pBmpData = (UInt16*)BmpGetBits( bmp );
			UInt32 datasize;
			BmpGetSizes ( bmp, &datasize, NULL );
			UInt8 ver = BmpGetVersion( bmp );
			for ( UInt32 i=0; i<datasize/4; i++ ) {				
				if ( ver==BitmapVersionThree ) pBmpData[i] = PlutoColorToUInt16( PlutoColor(255,255,0) );				
				else if ( ver==BitmapVersionTwo ) pBmpData[i] = PlutoColorToUInt16( PlutoColor(255,0,0) );
				else if ( ver==BitmapVersionOne ) pBmpData[i] = PlutoColorToUInt16( PlutoColor(0,255,0) );
				else if ( ver==BitmapVersionZero ) pBmpData[i] = PlutoColorToUInt16( PlutoColor(0,0,255) );
				else pBmpData[i] = PlutoColorToUInt16( PlutoColor(255,255,255) );
			}
			for ( UInt32 i=datasize/4; i<datasize/2; i++ ) {				
				pBmpData[i] = PlutoColorToUInt16( PlutoColor(255,255,0) );				
			}
			WinDrawBitmap ( bmp, 220, 220 );
		}
		BmpDelete( bmp );
	}
	SetScreenBuffer( false );
	DrawBufferToScreen();
}