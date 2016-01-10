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
#ifndef VIPMenu_H
#define VIPMenu_H

#define MENU_ELEMENT_GRAPHIC	1
#define MENU_ELEMENT_TEXT		2
#define MENU_ELEMENT_LIST		3

#ifndef SYMBIAN 
#include <iostream>
#include <string>
#include <list>
#include <map>
using namespace std;

#else
#include "PlutoUtils/MyString.h"
#endif

#include "PlutoUtils/MyStl.h"
#include "SerializeClass/SerializeClass.h"

class VIPMenuElement
{
public:
	unsigned short m_iX,m_iY,m_iWidth,m_iHeight;
	long m_iElementNumber;
	class VIPMenu *m_pMenu;

	// If a variable number is specified here, then the visibility state is tied
	// To the value of the variable.  If the variable is 0, the item is hidden, if the variable is anything else
	// the item is visible.  Enter a negative number to invert the normal behavior
	long m_iVariableIDHidden;

	// If something is specified here, the item is visible only when the option 
	// is specified for the current basket item.  Add a ! to the beginning to invert
	// the normal behavior
	string m_sBasketOption;

	VIPMenuElement() { m_pMenu=NULL; m_iX=m_iY=m_iWidth=m_iHeight=0; m_iVariableIDHidden=0; }
	VIPMenuElement(unsigned short X, unsigned short Y, unsigned short Width, unsigned short Height, unsigned short iVariableIDHidden) { m_pMenu=NULL; m_iX=X; m_iY=Y; m_iWidth=Width; m_iHeight=Height; m_iVariableIDHidden=iVariableIDHidden; }
	virtual ~VIPMenuElement() {}
	virtual unsigned short MenuElementID()=0;
};

#define GRAPHICTYPE_UNKNOWN		0
#define GRAPHICTYPE_PNG			1
#define GRAPHICTYPE_JPG			2
#define GRAPHICTYPE_MNG			3
#define GRAPHICTYPE_AGIF		4

class VIPMenuElement_Graphic : public VIPMenuElement
{
public:
	unsigned short m_iGraphicType;
	unsigned long m_iGraphicSize;
	void *m_pGraphicData;
	string m_sFileName;

	VIPMenuElement_Graphic(string sFileName, unsigned short X, unsigned short Y, unsigned short Width, unsigned short Height,unsigned short VariableIDHidden=0) 
		: VIPMenuElement(X,Y,Width,Height,VariableIDHidden) { m_iGraphicType=GRAPHICTYPE_UNKNOWN; m_sFileName=sFileName; m_pGraphicData=NULL; m_iGraphicSize=0; }
	unsigned short MenuElementID() { return MENU_ELEMENT_GRAPHIC; };
};


// If the text is of the form <%=X%>, then substitiute for the variable, formatted as per the variable format
class VIPMenuElement_Text : public VIPMenuElement
{
public:
	unsigned short m_iSize;
	unsigned char m_R,m_G,m_B;
	string m_sFont,m_sText;

	VIPMenuElement_Text(unsigned short Size, string Font, unsigned short X, unsigned short Y, unsigned short Width, unsigned short Height, string Text, unsigned char R,unsigned char G,unsigned char B,unsigned short VariableIDHidden=0)
		: VIPMenuElement(X,Y,Width,Height,VariableIDHidden) { m_iSize=Size; m_sFont=Font; m_sText=Text; m_R=R; m_G=G; m_B=B;  }
	unsigned short MenuElementID() { return MENU_ELEMENT_TEXT; };
};

// If the text is of the form <%=X%>, then substitiute for the variable, formatted as per the variable format
class VIPMenuElement_List : public VIPMenuElement_Text
{
public:
	// If IsAlpha, then the sole input should be a text box for the user to type
	// alpha numeric characters, which cause the list to scroll.  The Resolution will likely be
	// Y or E.  The variable associated with the input will contain the Value (not the Text)
	// of the selected item.  It is assumed that at least 1 item must always be selected.
	// If !IsAlpha, then there will be [MaxItemsPerScreen] on the screen, and the digit 1-[MaxItemsPerScreen]
	// will select an item.  The sole input box will likely have a max of 1 characters, and the resolution
	// is F.  If !IsAlpha, the MaxItemsPerScreen must be <= 12 (1-9, 0, * , #).
	unsigned short m_iIsAlpha,m_iMaxItemsPerScreen;

	// m_sText will have each entry deliniated with a line.  With each line, the columns
	// are delineated with [tab].  The first column is always hidden and contains the value
	// There must be at least 2 columns
	// m_sTabStops should be in the format X,Y,Z where XYZ are the number of pixels.
	// The first TabStop should be >0, and the phone will add the corresponding digit at position 0
	// If the first TabStop is 0, the phone will not automatically add a column
	// The tokens <%=LIST_VALUE%> <%=LIST_TEXT%> and <%=LIST_SEARCH%> will always evaluate
	// to the currently selected item, where LIST_SEARCH is a single digit for !IsAlpha and
	// a string for IsAlpha
	string m_sTabStops;


	VIPMenuElement_List(unsigned short IsAlpha,unsigned short MaxItemsPerScreen,string TabStops,
		unsigned short Size, string Font, unsigned short X, unsigned short Y, unsigned short Width, unsigned short Height, string Text, unsigned char R,unsigned char G,unsigned char B,unsigned short VariableIDHidden=0)
		: VIPMenuElement_Text(Size,Font,X,Y,Width,Height,Text,R,G,B,VariableIDHidden)
		{ m_iIsAlpha=IsAlpha; m_iMaxItemsPerScreen=MaxItemsPerScreen; m_sText=Text; m_sTabStops=TabStops; }
	unsigned short MenuElementID() { return MENU_ELEMENT_LIST; };
};

#define VIPVAR_PAYMENT_AMOUNT_USER	1
#define VIPVAR_PAYMENT_METHOD	2
#define VIPVAR_PIN_CODE			3
#define VIPVAR_INVOICE_NUM		4
#define VIPVAR_ESTABLISHMENT	5
#define VIPVAR_TRANS_DESC		6
#define VIPVAR_INVOICE_DETAIL	7
#define VIPVAR_VIP_TRANS_NUM	8
#define VIPVAR_AVAIL_PAYMENT_METHODS	9
#define VIPVAR_PAYMENT_AMOUNT_REQUESTED	10

#define MENU_VARFORMAT_NUMERIC		1
#define MENU_VARFORMAT_2_DECIMAL	2
#define MENU_VARFORMAT_TEXT			3
#define MENU_VARFORMAT_NUMERIC_PIN	4

class VIPVariable : public SerializeClass
{
public:
	bool m_bEncryptBeforeSending;
	// Format means when the variable is evaluated for displaying to the user in 
	// a text object, format according to this method
	long m_iVariableID;
	unsigned long m_iFormat;
	char m_iPhoneSetsValue;
	string m_sInitialValue,m_sCurrentValue;
	string m_sDescription;
	class VIPMenuCollection *m_pCollection;

	// These lists are not serialized, 
	MYSTL_CREATE_LIST(m_iListElements_Visible,VIPMenuElement);
	MYSTL_CREATE_LIST(m_iListElements_Text,VIPMenuElement);

	VIPVariable(long iVariableID,unsigned char PhoneSetsValue,string sInitialValue,bool bEncryptBeforeSending,unsigned short iFormat)
	{
		m_iFormat = iFormat;
		m_bEncryptBeforeSending = bEncryptBeforeSending;
		m_iVariableID = iVariableID;
		m_iPhoneSetsValue=PhoneSetsValue;  m_sInitialValue=sInitialValue;
		m_sCurrentValue = ""; //m_sInitialValue;
		m_pCollection=NULL;
	}
	VIPVariable() {};

	virtual void SetupSerialization(int iSC_Version)
	{
		SerializeClass::SetupSerialization(iSC_Version);
		StartSerializeList() + m_iVariableID + m_iFormat + m_iPhoneSetsValue
			+ m_sInitialValue + m_sCurrentValue + m_sDescription;
	}
};

// VariableNumber means the variable to put the input into.  Clear it first if Clear is 1.  Terminate with an F if number of chars is reached and terminate = 1
class VIPMenuInput
{
public:
	unsigned short m_iNumberOfChars;
	long m_iVariableID;
	unsigned char m_iClearVariable,m_iTerminateWhenNumberOfChars;
	long m_iInputNumber;
	string m_sDescription;
	class VIPMenuElement_Text *m_pTextElement;
	class VIPMenu *m_pMenu;

	VIPMenuInput(unsigned short NumberOfChars, unsigned char TerminateWhenNumberOfChars, unsigned char ClearVariable, class VIPVariable *pVariable, class VIPMenuElement_Text *pTextElement)
	{
		m_iNumberOfChars=NumberOfChars;  m_iTerminateWhenNumberOfChars=TerminateWhenNumberOfChars;
		m_pMenu=NULL;
		m_pTextElement = pTextElement;
		if( pVariable )
			m_iVariableID = pVariable->m_iVariableID;
		else
			m_iVariableID = -1;
	}
};

class VIPMenuZone
{
public:
	unsigned short m_X,m_Y,m_W,m_H;
	long m_iZoneNumber;
	char m_Key;
	class VIPMenu *m_pMenu;
};

// criteria: terminating key (YNEFUDLR01234567890)   where F means any digit, it just filled the max # of characters
// variable x = value y

// action flags: assign variable x the value y, hide app, goto menu, report tokens (gprs), report tokens (close request)

// Report to server means report over a GPRS/WAP without closing the menu

// CloseRequest can be one of the following:
#define	DONT_CLOSE			0
#define CLOSE_WITH_CANCEL	1
#define	CLOSE_WITH_OK		2
#define CLOSE_WITH_CHECKOUT	3
#define CLOSE_WITH_SWITCH_1	4
#define CLOSE_WITH_SWITCH_2	5
#define CLOSE_WITH_SWITCH_3	6
#define CLOSE_WITH_SWITCH_4	7
#define CLOSE_WITH_SWITCH_5	8

// Basket Actions
#define	BASKET_ADD			1
#define	BASKET_REMOVE		2
#define	BASKET_ADD_OPTION	3
#define	BASKET_CLEAR		4

class VIPMenuResolution
{
public:
	class VIPMenu *m_pMenu;
	long m_iResolutionNumber;
	// Criteria
	string m_sTerminatingKey;
	long m_iCriteriaVariableID;
	string m_sCriteriaVariableValue;
	string m_sProgramName;

	// Action
	unsigned char m_iHideApp,m_iCloseRequest,m_iReportToServer;
	long m_iMenuNumber_Goto;
	long m_iActionVariableID;
	string m_sActionVariableValue;

	unsigned char m_iBasketToggle,m_iBasketAction;
	string m_sBasketID,m_sBasketDescription;
	long m_iBasketCost;

	// Note that if m_sActionVariableValue=="!T" that means toggle.  If the variable is 0, assign 1, otherwise assign 0
	// If an object's visibility is tied to this variable, that object needs to be updated when the variable changes

	VIPMenuResolution(string TerminatingKey,long CriteriaVariable, string CriteriaVariableValue,string ProgramName,
		unsigned char HideApp,unsigned char CloseRequest,unsigned char ReportToServer,
		long MenuNumberGoto,long ActionVariableID,string ActionVariableValue)
	{

		m_sTerminatingKey=TerminatingKey; m_iCriteriaVariableID=CriteriaVariable; m_sCriteriaVariableValue=CriteriaVariableValue; m_sProgramName=ProgramName;
		m_iHideApp=HideApp; m_iCloseRequest=CloseRequest; m_iReportToServer=ReportToServer;
		m_iMenuNumber_Goto=MenuNumberGoto; m_iActionVariableID=ActionVariableID; m_sActionVariableValue=ActionVariableValue;
		m_pMenu=NULL;
		m_iBasketToggle=m_iBasketAction=0;
		m_iBasketCost=0;
	}

	void SetGotoMenu(class VIPMenu *pGotoMenu);
};

#define VIPMENU_INVOICE		1
#define VIPMENU_PMTMETHODS	2
#define VIPMENU_PINNUMBER	3
#define VIPMENU_WAITPMTAUTH	4

class VIPMenu
{
public:
	// So that we can serialize the items in and out (Elements, Inputs, Zones & Resolution), 
	// each one needs a unique ID, which we'll count here.  When we read in the new menu, we'll
	// be sure to make m_iNextChildID be greater than the highestID
	long m_iNextChildID;
	long m_iMenuNumber;
	unsigned char m_iAllowEstablishmentToAddTokens;
	// ID is (Invoice Screen, Credit Card List, etc.), Number is used in the 'Goto' as part of the Input
	unsigned short m_iMenuID;

	// In ms, this is how often the phone should poll for updates (ie WhatDoYouHave)
	unsigned long m_iFrequencyToPoll;
	string m_sDescription;
	MYSTL_CREATE_LONG_MAP(m_mapMenuElements,VIPMenuElement);
	MYSTL_CREATE_LONG_MAP(m_mapMenuInputs,VIPMenuInput);
	MYSTL_CREATE_LONG_MAP(m_mapMenuZones,VIPMenuZone);
	MYSTL_CREATE_LONG_MAP(m_mapMenuResolutions,VIPMenuResolution);

	class VIPMenuCollection *m_pCollection;

public:
	VIPMenu(unsigned short MenuID)
	{ 
		m_iMenuID=MenuID; m_iMenuNumber=-1;
		m_pCollection=NULL;
		m_iNextChildID=0;
	}
	void AddElement(VIPMenuElement *MenuElement) 
	{ 
		MenuElement->m_pMenu=this; MenuElement->m_iElementNumber=m_iNextChildID++; 
		MYSTL_ADD_TO_LONG_MAP(m_mapMenuElements,MenuElement,MenuElement->m_iElementNumber); 
	}
	void AddElement(VIPMenuElement *MenuElement,long Number)
	{
		MenuElement->m_pMenu=this; 
		MenuElement->m_iElementNumber=Number; 
		if( Number>=m_iNextChildID )
			m_iNextChildID = Number+1;
		MYSTL_ADD_TO_LONG_MAP(m_mapMenuElements,MenuElement,MenuElement->m_iElementNumber); 
	}
	void AddInput(VIPMenuInput *MenuInput,long Number) 
	{ 
		MenuInput->m_pMenu=this; MenuInput->m_iInputNumber=Number; 
		if( Number>=m_iNextChildID )
			m_iNextChildID = Number+1;
		MYSTL_ADD_TO_LONG_MAP(m_mapMenuInputs,MenuInput,MenuInput->m_iInputNumber); 
	}
	void AddInput(VIPMenuInput *MenuInput) 
	{ 
		MenuInput->m_pMenu=this; MenuInput->m_iInputNumber=m_iNextChildID++; 
		MYSTL_ADD_TO_LONG_MAP(m_mapMenuInputs,MenuInput,MenuInput->m_iInputNumber); 
	}
	void AddZone(VIPMenuZone *MenuZone) 
	{ 
		MenuZone->m_pMenu=this; MenuZone->m_iZoneNumber=m_iNextChildID++; 
		MYSTL_ADD_TO_LONG_MAP(m_mapMenuZones,MenuZone,MenuZone->m_iZoneNumber); 
	}
	void AddZone(VIPMenuZone *MenuZone,long Number) 
	{ 
		MenuZone->m_pMenu=this; MenuZone->m_iZoneNumber=Number; 
		if( Number>=m_iNextChildID )
			m_iNextChildID = Number+1;
		MYSTL_ADD_TO_LONG_MAP(m_mapMenuZones,MenuZone,MenuZone->m_iZoneNumber); 
	}
	void AddResolution(VIPMenuResolution *MenuResolution) 
	{ 
		MenuResolution->m_pMenu=this; MenuResolution->m_iResolutionNumber=m_iNextChildID++;
		MYSTL_ADD_TO_LONG_MAP(m_mapMenuResolutions,MenuResolution,MenuResolution->m_iResolutionNumber); 
	}
	void AddResolution(VIPMenuResolution *MenuResolution,long Number) 
	{ 
		MenuResolution->m_pMenu=this; MenuResolution->m_iResolutionNumber=Number;
		if( Number>=m_iNextChildID )
			m_iNextChildID = Number+1;
		MYSTL_ADD_TO_LONG_MAP(m_mapMenuResolutions,MenuResolution,MenuResolution->m_iResolutionNumber); 
	}
	VIPMenuElement *m_mapMenuElements_Find(long ID)
	{
		/*
		map<long,class VIPMenuElement *>::iterator it = m_mapMenuElements.find(ID);
		return it==m_mapMenuElements.end() ? NULL : (*it).second;
		*/
		MYSTL_GET_AT_LONG_MAP(m_mapMenuElements, VIPMenuElement, res, ID);
		return res;
	}
	VIPMenuInput *m_mapMenuInputs_Find(long ID)
	{
		/*
		map<long,class VIPMenuInput *>::iterator it = m_mapMenuInputs.find(ID);
		return it==m_mapMenuInputs.end() ? NULL : (*it).second;
		*/
		MYSTL_GET_AT_LONG_MAP(m_mapMenuInputs, VIPMenuInput, res, ID);
		return res;
	}
	VIPMenuZone *m_mapMenuZones_Find(long ID)
	{
		/*
		map<long,class VIPMenuZone *>::iterator it = m_mapMenuZones.find(ID);
		return it==m_mapMenuZones.end() ? NULL : (*it).second;
		*/
		MYSTL_GET_AT_LONG_MAP(m_mapMenuZones, VIPMenuZone, res, ID);
		return res;
	
	}
	VIPMenuResolution *m_mapMenuResolutions_Find(long ID)
	{
		/*
		map<long,class VIPMenuResolution *>::iterator it = m_mapMenuResolutions.find(ID);
		return it==m_mapMenuResolutions.end() ? NULL : (*it).second;
		*/
		MYSTL_GET_AT_LONG_MAP(m_mapMenuResolutions, VIPMenuResolution, res, ID);
		return res;
		
	}
};


#define	VARIABLE_TEXT		1
#define	VARIABLE_NUMERIC	2

#define VIPMENUCOLL_SECURE_PAYMENT	1

class VIPMenuCollection : public SerializeClass
{
	// So that we can serialize the items in and out (Elements, Inputs, Zones & Resolution), 
	// each one needs a unique ID, which we'll count here.  When we read in the new menu, we'll
	// be sure to make m_iNextChildID be greater than the highestID
	long m_iNextChildID;

public:
	MYSTL_CREATE_LONG_MAP(m_mapMenus,VIPMenu);
	MYSTL_CREATE_LONG_MAP(m_mapVariables,VIPVariable);
	long m_iBinarySize;
	char *m_pBinary;
	
	class VIPMenu *m_pMenu_Starting;
	unsigned short m_iMenuCollectionID;
	unsigned short m_iWidth,m_iHeight;

	VIPMenuCollection(unsigned short MenuCollectionID) { 
		 m_iBinarySize=0; m_pBinary=NULL; m_iMenuCollectionID=MenuCollectionID; m_pMenu_Starting=NULL; m_iNextChildID=0; }
	VIPMenuCollection(long size,const char *data);
	bool ConvertToBinary();

	class VIPVariable *m_mapVariables_Find(int VariableID)
	{
		/*
		map<long,class VIPVariable *>::iterator it = m_mapVariables.find(VariableID);
		return it==m_mapVariables.end() ? NULL : (*it).second;
		*/

		MYSTL_GET_AT_LONG_MAP(m_mapVariables, VIPVariable, res, VariableID);
		return res;
	}

	void AddVariable(VIPVariable *pVariable)
	{
		pVariable->m_pCollection = this;
//		m_mapVariables[pVariable->m_iVariableID]=pVariable;
		MYSTL_ADD_TO_LONG_MAP(m_mapVariables, pVariable, pVariable->m_iVariableID);
	}

	void AddMenu(VIPMenu *pMenu)
	{
		pMenu->m_iMenuNumber = m_iNextChildID++;
		pMenu->m_pCollection = this;
		MYSTL_ADD_TO_LONG_MAP(m_mapMenus,pMenu,pMenu->m_iMenuNumber);
	}
	VIPMenu *m_mapMenus_Find(long ID)
	{
		/*
		map<long,class VIPMenu *>::iterator it = m_mapMenus.find(ID);
		return it==m_mapMenus.end() ? NULL : (*it).second;
		*/
		MYSTL_GET_AT_LONG_MAP(m_mapMenus, VIPMenu, res, ID);
		return res;
	}
	void AddMenu(VIPMenu *pMenu,long Number)
	{
		pMenu->m_iMenuNumber = Number;
		if( Number>=m_iNextChildID )
			m_iNextChildID = Number+1;
		pMenu->m_pCollection = this;
		MYSTL_ADD_TO_LONG_MAP(m_mapMenus,pMenu,pMenu->m_iMenuNumber);
	}
};

#endif

