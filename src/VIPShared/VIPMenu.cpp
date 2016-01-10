/*
 VIPMenu
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#include "VIPIncludes.h"
#include "VIPMenu.h"

#ifdef VIPPHONE
#ifdef VIPDESIGN
#include "VIPShared/PlutoConfig.h"
#endif
#endif

#ifndef SYMBIAN
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#endif

/* The binary format of a menu collection is:

# of variables
	variable number
	phone sets variable
	long value
	text value

# of menus
	menu #
	m_iAllowEstablishmentToAddTokens
	m_iMenuID
	repeat 4 times resolution for Y, N, E, F:
		m_iHideApp
		m_iReportTokens
		m_iMenuNumber_Goto
	# of elements
		element type, x, y, w, h, extra information for element
	# of inputs
		m_iMenuInputID
		m_iNumberOfChars
		m_iVariableNumber
		m_iTerminateWhenNumberOfChars
		Element_Text
*/	
		
#define MAX_VARIABLE_SIZE	1000     // Each variable won't be more than 1k
#define MAX_MENU_SIZE		500000   // Each menu won't be more than 500k


VIPMenuCollection::VIPMenuCollection(long size, const char *data) 
{
	m_pBinary=NULL;
	m_iBinarySize=0;
    m_iNextChildID=0;
	
	StartReading(size,(char *)data);
	m_iMenuCollectionID = Read_unsigned_short();
	long StartingMenu = Read_long();
	m_iWidth = Read_unsigned_short();
	m_iHeight = Read_unsigned_short();
	unsigned short NumberOfVariables = Read_unsigned_short();

	for(long iCount=0;iCount<NumberOfVariables;++iCount)
	{
		long VariableID = Read_long();
		string sDescription;
		Read_string(sDescription);
		unsigned char PhoneSetsValue = Read_unsigned_char();
		bool bEncryptBeforeSending = (Read_unsigned_char()==1);
		unsigned short iFormat = Read_unsigned_short();
		string sInitialValue;
		Read_string(sInitialValue);

		VIPVariable *pVariable = new VIPVariable(VariableID,PhoneSetsValue,sInitialValue,bEncryptBeforeSending,iFormat);
		pVariable->m_sDescription=sDescription;
		AddVariable(pVariable);
	}

	unsigned short NumberOfMenus = Read_unsigned_short();
	for(long iCountMenus=0;iCountMenus<NumberOfMenus;++iCountMenus)
	{
		long MenuNumber = Read_long();
		string Description;
		Read_string(Description);
		unsigned char AllowEstablishmentToAddTokens = Read_unsigned_char();
		unsigned short MenuID = Read_unsigned_short();

		VIPMenu *pMenu = new VIPMenu(MenuID);
		AddMenu(pMenu,MenuNumber);
		pMenu->m_iFrequencyToPoll = Read_unsigned_long();
		pMenu->m_sDescription=Description;

		unsigned short NumberOfResolutions = Read_unsigned_short();
		for(long iRes=0;iRes<NumberOfResolutions;++iRes)
		{
			long ResolutionNumber = Read_long();
			string TerminatingKeys;
			Read_string(TerminatingKeys);
			long CriteriaVariable = Read_long();
			string CriteriaVariableValue;
			Read_string(CriteriaVariableValue);
			string ProgramName;
			Read_string(ProgramName);
			unsigned char HideApp = Read_unsigned_char();
			unsigned char CloseRequest = Read_unsigned_char();
			unsigned char ReportToServer = Read_unsigned_char();
			long MenuNumber = Read_long();
			long ActionVariableID = Read_long();
			string ActionVariableValue;
			Read_string(ActionVariableValue);

			VIPMenuResolution *pV = new VIPMenuResolution(TerminatingKeys,CriteriaVariable,CriteriaVariableValue,
				ProgramName,HideApp,CloseRequest,ReportToServer,
				MenuNumber,ActionVariableID,ActionVariableValue);

			pV->m_iBasketToggle=Read_unsigned_char();
			pV->m_iBasketAction=Read_unsigned_char();
			Read_string(pV->m_sBasketID);
			Read_string(pV->m_sBasketDescription);
			pV->m_iBasketCost=Read_long();

			pMenu->AddResolution(pV,ResolutionNumber);
		}

		unsigned short NumberOfElements = Read_unsigned_short();
		for(long iElement=0;iElement<NumberOfElements;++iElement)
		{
			long ElementNumber = Read_long();
			unsigned short ElementType = Read_unsigned_short();
			unsigned short X = Read_unsigned_short();
			unsigned short Y = Read_unsigned_short();
			unsigned short W = Read_unsigned_short();
			unsigned short H = Read_unsigned_short();
			long VariableIDHidden = Read_long();
			string BasketOption;
			Read_string(BasketOption);

			VIPMenuElement *pElement;

			if( ElementType==MENU_ELEMENT_GRAPHIC )
			{
				unsigned short Type = Read_unsigned_short();
				string sFileName;
				Read_string(sFileName);

				VIPMenuElement_Graphic *pGraphic = new VIPMenuElement_Graphic(sFileName,X,Y,W,H);
				pElement = pGraphic;
				pGraphic->m_iGraphicType = Type;

				pGraphic->m_iGraphicSize = Read_unsigned_long();
				pGraphic->m_pGraphicData = Read_block(pGraphic->m_iGraphicSize);
				pMenu->AddElement(pGraphic,ElementNumber);
			}
			else
			{
				unsigned short iIsAlpha,iMaxItemsPerScreen;
				string sTabStops;

				if( ElementType==MENU_ELEMENT_LIST )
				{
					iIsAlpha = Read_unsigned_short();
					iMaxItemsPerScreen = Read_unsigned_short();
					Read_string(sTabStops);
				}
				
				unsigned short Size = Read_unsigned_short();
				unsigned char R = Read_unsigned_char();
				unsigned char G = Read_unsigned_char();
				unsigned char B = Read_unsigned_char();
				string Font;
				Read_string(Font);
				string Text;
				Read_string(Text);

				if( ElementType==MENU_ELEMENT_LIST )
				{
					VIPMenuElement_List *pList = new VIPMenuElement_List(iIsAlpha,iMaxItemsPerScreen,
						sTabStops,Size,Font,X,Y,W,H,Text,R,G,B);
					pElement=pList;
					pMenu->AddElement(pList,ElementNumber);
				}
				else
				{
					VIPMenuElement_Text *pText = new VIPMenuElement_Text(Size,Font,X,Y,W,H,Text,R,G,B);
					pElement=pText;
					pMenu->AddElement(pText,ElementNumber);
				}
			}
			pElement->m_iVariableIDHidden = VariableIDHidden;
			pElement->m_sBasketOption = BasketOption;
		}

		unsigned short NumberOfInputs = Read_unsigned_short();
		for(long iInput=0;iInput<NumberOfInputs;++iInput)
		{
			long InputNumber = Read_long();
			unsigned short NumberOfChars = Read_unsigned_short();
			long VariableID = Read_long();
			unsigned char TerminateWhenNumChars = Read_unsigned_char();
			unsigned char ClearVariable = Read_unsigned_char();
			unsigned short ElementText = Read_unsigned_short();


			VIPMenuElement_Text *pElement=(VIPMenuElement_Text *)pMenu->m_mapMenuElements_Find(ElementText);

			VIPVariable *pVariable=NULL;
			if( VariableID!=-1 )
				pVariable = m_mapVariables_Find(VariableID);

			VIPMenuInput *pInput = new VIPMenuInput(NumberOfChars,TerminateWhenNumChars,ClearVariable,pVariable,pElement);
			pMenu->AddInput(pInput,InputNumber);
		}
	}

	m_pMenu_Starting = m_mapMenus_Find(StartingMenu);
}

bool VIPMenuCollection::ConvertToBinary()
{
#ifdef VIPDESIGN
	if( m_pMenu_Starting==NULL ) 
		return false;

	StartWriting();

	Write_unsigned_short(m_iMenuCollectionID);
	Write_long(m_pMenu_Starting->m_iMenuNumber);
	Write_unsigned_short(m_iWidth);
	Write_unsigned_short(m_iHeight);
	Write_unsigned_short((unsigned short) MYSTL_LONGMAP_SIZE(m_mapVariables));

	MYSTL_ITERATE_LONG_MAP(m_mapVariables,VIPVariable,pVariable,itVariables)
	{
		Write_long(pVariable->m_iVariableID);
		Write_string(pVariable->m_sDescription);
		Write_unsigned_char(pVariable->m_iPhoneSetsValue);
		Write_unsigned_char(pVariable->m_bEncryptBeforeSending);
		Write_unsigned_short(pVariable->m_iFormat);
		Write_string(pVariable->m_sInitialValue);
	}

	Write_unsigned_short((unsigned short) MYSTL_SIZEOF_LONG_MAP(m_mapMenus));
	MYSTL_ITERATE_LONG_MAP(m_mapMenus,VIPMenu,pMenu,iMenus)
	{
		Write_long(pMenu->m_iMenuNumber);
		Write_string(pMenu->m_sDescription);
		Write_unsigned_char(pMenu->m_iAllowEstablishmentToAddTokens);
		Write_unsigned_short(pMenu->m_iMenuID);
		Write_unsigned_long(pMenu->m_iFrequencyToPoll);
		Write_unsigned_short((unsigned short) MYSTL_SIZEOF_LONG_MAP(pMenu->m_mapMenuResolutions));
		MYSTL_ITERATE_LONG_MAP(pMenu->m_mapMenuResolutions,VIPMenuResolution,pRes,iRes)
		{
			Write_long(pRes->m_iResolutionNumber);
			Write_string(pRes->m_sTerminatingKey);
			Write_long(pRes->m_iCriteriaVariableID);
			Write_string(pRes->m_sCriteriaVariableValue);
			Write_string(pRes->m_sProgramName);
			Write_unsigned_char(pRes->m_iHideApp);
			Write_unsigned_char(pRes->m_iCloseRequest);
			Write_unsigned_char(pRes->m_iReportToServer);
			Write_long(pRes->m_iMenuNumber_Goto);
			Write_long(pRes->m_iActionVariableID);
			Write_string(pRes->m_sActionVariableValue);

			Write_unsigned_char(pRes->m_iBasketToggle);
			Write_unsigned_char(pRes->m_iBasketAction);
			Write_string(pRes->m_sBasketID);
			Write_string(pRes->m_sBasketDescription);
			Write_long(pRes->m_iBasketCost);
		}

		Write_unsigned_short((unsigned short) MYSTL_SIZEOF_LONG_MAP(pMenu->m_mapMenuElements));
		MYSTL_ITERATE_LONG_MAP(pMenu->m_mapMenuElements,VIPMenuElement,pElement,iElements)
		{
			Write_long(pElement->m_iElementNumber);
			Write_unsigned_short(pElement->MenuElementID());
			Write_unsigned_short(pElement->m_iX);
			Write_unsigned_short(pElement->m_iY);
			Write_unsigned_short(pElement->m_iWidth);
			Write_unsigned_short(pElement->m_iHeight);
			Write_long(pElement->m_iVariableIDHidden);
			Write_string(pElement->m_sBasketOption);

			if( pElement->MenuElementID()==MENU_ELEMENT_GRAPHIC )
			{
				VIPMenuElement_Graphic *pGraphic = (VIPMenuElement_Graphic *) pElement;
				if( pGraphic->m_iGraphicType==GRAPHICTYPE_UNKNOWN && pGraphic->m_sFileName.length()>4 )
				{
					string Extension=StringUtils::ToUpper(pGraphic->m_sFileName.substr( pGraphic->m_sFileName.length()-4));

					if( Extension==".PNG" )
						pGraphic->m_iGraphicType = GRAPHICTYPE_PNG;
					else if( Extension==".JPG" )
						pGraphic->m_iGraphicType = GRAPHICTYPE_JPG;
				}

				Write_unsigned_short(pGraphic->m_iGraphicType);
				Write_string(pGraphic->m_sFileName);

#ifndef ORBITER_PLUGIN
				//if( !pGraphic->m_iGraphicSize || !pGraphic->m_pGraphicData )
				{
					FILE *file = fopen( (g_pPlutoConfig->m_sMenuPath + pGraphic->m_sFileName).c_str(),"rb");
					if( !file )
						file = fopen( pGraphic->m_sFileName.c_str(),"rb");  // See if it's a fully qualified path
					if( file )
					{
						fseek(file, 0L, SEEK_END);
						pGraphic->m_iGraphicSize=ftell(file);
						if( pGraphic->m_iGraphicSize==0 )
							fclose(file);
						else
						{
							fseek(file, 0L, SEEK_SET);
							pGraphic->m_pGraphicData=(char *)malloc(pGraphic->m_iGraphicSize+1);
							size_t bytesread=fread(pGraphic->m_pGraphicData,1,pGraphic->m_iGraphicSize,file);
							fclose(file);
						}
					}
				}
#endif 

				Write_unsigned_long(pGraphic->m_iGraphicSize);
				Write_block((char *)pGraphic->m_pGraphicData,pGraphic->m_iGraphicSize);
			}
			else
			{
				if( pElement->MenuElementID()==MENU_ELEMENT_LIST )
				{
					VIPMenuElement_List *pList = (VIPMenuElement_List *) pElement;
					Write_unsigned_short(pList->m_iIsAlpha);
					Write_unsigned_short(pList->m_iMaxItemsPerScreen);
					Write_string(pList->m_sTabStops);
				}
				VIPMenuElement_Text *pText = (VIPMenuElement_Text *) pElement;
				Write_unsigned_short(pText->m_iSize);
				Write_unsigned_char(pText->m_R);
				Write_unsigned_char(pText->m_G);
				Write_unsigned_char(pText->m_B);
				Write_string(pText->m_sFont);
				Write_string(pText->m_sText);
			}
		}

		Write_unsigned_short((unsigned short) MYSTL_SIZEOF_LONG_MAP(pMenu->m_mapMenuInputs));
		MYSTL_ITERATE_LONG_MAP(pMenu->m_mapMenuInputs,VIPMenuInput,pInput,iInputs)
		{
			Write_long(pInput->m_iInputNumber);
			Write_unsigned_short(pInput->m_iNumberOfChars);
			Write_long(pInput->m_iVariableID);
			Write_unsigned_char(pInput->m_iTerminateWhenNumberOfChars);
			Write_unsigned_char(pInput->m_iClearVariable);
			Write_unsigned_short(pInput->m_pTextElement ? pInput->m_pTextElement->m_iElementNumber : -1);
		}
	}

	m_iBinarySize = (unsigned long) (m_pcCurrentPosition-m_pcDataBlock);
	m_pBinary = m_pcDataBlock;
#endif //VIPDESIGN

	return true;
}

void VIPMenuResolution::SetGotoMenu(class VIPMenu *pGotoMenu)
{
	if( pGotoMenu )
		m_iMenuNumber_Goto = pGotoMenu->m_iMenuNumber;
	else
		m_iMenuNumber_Goto = 0;
}
