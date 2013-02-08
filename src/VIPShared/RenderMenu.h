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
#ifndef RenderMenu_h
#define RenderMenu_h

#ifdef SYMBIAN
#include "PlutoUtils/MyString.h"
#include "PlutoUtils/MyStl.h"
#include "PlutoUtils/MyMap.h"
#endif

#include "pluto_main/Define_Button.h"
#include "VIPMenu.h"

struct MyRect
{
	MyRect(int t,int l,int r,int b) { top=t; left=l; right=r; bottom=b; }
	int top,left,right,bottom;
};

class VIPList
{
public:
	string m_sValue,m_sText;
	class BasketItem *m_pBasketItem;

	VIPList() { m_pBasketItem=NULL; }
};

struct CaptureKeyboardParam
{
	bool bOnOff; 
	bool bDataGrid;
	bool bReset;
	bool bTypePin;
	bool bNumbersOnly;

	int    iVariable;
	string sVariableValue;
	
	bool bTextBox;
	unsigned long TextX;
	unsigned long TextY;
	unsigned long TextWidth;
	unsigned long TextHeight;

	void Reset() { sVariableValue = ""; }
};

class RenderMenu
{
protected:
#ifndef SYMBIAN
	clock_t m_ctLastKey;
#endif

	// For managing an on-screen list

	MYSTL_CREATE_LIST(m_listVIPList,VIPList);
	VIPMenuElement_List *m_pListOnScreen;
	int m_iListOffset;
	bool m_bListIsMore;
	string m_sListValue,m_sListText,m_sListSearch;
	class BasketItem *m_pBasketItem;
	MYSTL_CREATE_LIST(m_listBasket,BasketItem);
	

	// The current collection
	class VIPMenu *m_pMenu;
	class VIPMenuCollection *m_pMenuCollection;
	class VIPMenuInput *m_pInput;

	// The current static image (which we will show instead of the collection if we have one)
	unsigned char m_pImageStatic_Type;
	unsigned long m_pImageStatic_Size;
	char *m_pImageStatic_Data;

	// For scrolling through a basket, here are the first and last items on the screen
	class BasketItem *m_pBasketItem_First,*m_pBasketItem_Last;

	RenderMenu();
	virtual ~RenderMenu() {}

	void DoRender();
	void KeyPressed(int KeyCode);
	char NumericToText(int Key,const char *Value);
	int  ButtonToNumeric(int Key);
	string SubstituteValues(string Input);
	virtual void SetCurrent(class VIPMenu *pMenu,class VIPMenuCollection *pMenuCollection,class VIPMenuInput *pInput)
	{
		m_pMenu=pMenu; m_pMenuCollection=pMenuCollection; m_pInput=pInput;
	}

	virtual void SetStatic(unsigned char pImageStatic_Type,unsigned long pImageStatic_Size,char *pImageStatic_Data)
	{
		m_pImageStatic_Type=pImageStatic_Type;
		m_pImageStatic_Size=pImageStatic_Size;
		m_pImageStatic_Data=pImageStatic_Data;
	}

	virtual void Redraw()=0;
	virtual void MessageBox(const char *Message,const char *Title)=0;
	virtual bool DrawImage(const char *Filename,int X,int Y,int Width,int Height)=0;
	virtual bool DrawImage(unsigned char ImageType,void *pGraphic,int GraphicSize,int X,int Y,int Width,int Height)=0;
	virtual bool Draw3dRect(MyRect &r)=0;
	virtual bool DrawText(const char *Text, MyRect &r)=0;
	virtual void SetTextProperties(int Size,const char *Font,int R, int G, int B)=0;
	virtual void SwitchToMenu(VIPMenu *pMenu)=0;

	virtual void OpenProgram(string ProgramName) {};


public:
	// The capture keyboard parameters
	CaptureKeyboardParam m_CaptureKeyboardParam;
};

#endif
