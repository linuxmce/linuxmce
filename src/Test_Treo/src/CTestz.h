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
#ifndef CTESTZ_H
#define CTESTZ_H

#include <PalmOS.h>
#include <string>
#include "PlutoInc.h"

using namespace std;

#define _DOUBLEBUFFERING_





class CTestz {
private:
  void PlutoColorToRGBColorType( RGBColorType& dstColor, PlutoColor srcColor ){
  	dstColor.index = 0;
	dstColor.r = srcColor.R();
	dstColor.g = srcColor.G();
	dstColor.b = srcColor.B();
  }	
   UInt16 RGBColorTypeToUInt16( RGBColorType color ){
		UInt16 ret = 0;
		ret = color.b & 0x001F;
		ret |= ((UInt16)(color.g & 0x003F))<<5;
		ret |= ((UInt16)(color.r & 0x001F))<<11;
		return ret;
	}
	
	UInt16 CTestz::PlutoColorToUInt16( PlutoColor color ){
	  	UInt16 ret = 0;
		ret = color.B() & 0x001F;
		ret |= ((UInt16)(color.G() & 0x003F))<<5;
		ret |= ((UInt16)(color.R() & 0x001F))<<11;
		return ret;
	}
  
  WinHandle m_hScreenBuffer, m_hScreen;
  
  UInt32 m_ui32Width, m_ui32Height, m_ui32Depth;
  UInt16 m_ui16CoordSys;
  
  RectangleType m_rBufferBounds, m_rScreenBounds;
  void SetScreenBuffer( bool bActive=true );
  void DrawBufferToScreen( void );
  
  Boolean AppHandleEvent(EventType* pEvent);
  Boolean MainFormDoCommand(UInt16 command);
  static Boolean MainFormHandleEvent(EventType* pEvent);
  static CTestz* m_pInstance;
  
  
public:

	CTestz();
	virtual ~CTestz();
	
	void erase( void );
	void repaint( void );
	
	
	virtual void EventLoop();
	virtual void Configure();
	
	virtual void SolidRectangle(int iX, int iY, int iWidth, int iHeight, PlutoColor color );//, string ParentObjectID = "", string ObjectID = "");
	virtual void HollowRectangle(int X, int Y, int Width, int Height, PlutoColor color );//, string ParentObjectID = "", string ObjectID = "");
	virtual void DrawLine(int iX, int iY, int iWidth, int iHeight, PlutoColor color);
	
	//virtual void RenderText( string &sTextToDisplay, class DesignObjText *Text, class TextStyle *pTextStyle, PlutoPoint point = PlutoPoint(0, 0), string ObjectID = "") = 0;
	virtual void RenderText( string &sTextToDisplay, class TextStyle *pTextStyle, PlutoPoint point );
	
	void test_bmp( void );
	
	/*
	void test(void){
		char timeString[10]="Test";
		WinDrawChars(timeString, StrLen(timeString), 10, 10);
	}
	*/
};


#endif // CTESTZ_H
