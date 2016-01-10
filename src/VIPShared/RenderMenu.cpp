/*
RenderMenu
Copyright (C) 2004 Pluto, Inc., a Florida Corporation
www.plutohome.com		
Phone: +1 (877) 758-8648
This program is distributed according to the terms of the Pluto Public License, available at: 
http://plutohome.com/index.php?section=public_license 
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
*/

#include "VIPIncludes.h"
#include "VIPShared/VIPMenu.h"
#include "VIPShared/RenderMenu.h"
#include "VIPShared/BD_PC_KeyWasPressed.h"

#if !defined(SYMBIAN) && !defined(SMARTPHONE)
	#include "VIPShared/PlutoConfig.h"
	#include "BD/BDCommandProcessor.h"
#endif

#include "PlutoUtils/StringUtils.h"

#ifdef SYMBIAN
	#define SYMBIAN_ALL
	#include "PlutoMOAppUi.h"
#endif

#ifdef SMARTPHONE
	#include "Orbiter/CENet_Smartphone/OrbiterApp.h"
#endif

bool KeyCodeInString(string str, char key)
{
	for(int i = 0; i < str.length(); i++)
		if(str[i] == key)
			return true;
	return false;
}

RenderMenu::RenderMenu()
{
#ifndef SYMBIAN
	m_ctLastKey=0;
#endif
	m_pListOnScreen=NULL;
	m_iListOffset=0;
	m_pBasketItem_First=m_pBasketItem_Last=NULL;
	m_bListIsMore=false;
	m_pBasketItem=NULL;
	m_pMenu=NULL;
	m_pMenuCollection=NULL;
	m_pInput=NULL;
	m_pImageStatic_Type=0;
	m_pImageStatic_Size=0;
	m_pImageStatic_Data=NULL;
	m_CaptureKeyboardParam.bOnOff = false;
}

void RenderMenu::DoRender()
{
	if( m_pImageStatic_Size && m_pImageStatic_Data )
	{
		DrawImage(m_pImageStatic_Type,(void *)m_pImageStatic_Data,m_pImageStatic_Size,0,0,0,0);
		return;
	}

	// Be sure we clear out any on-screen list
	MYSTL_ITERATE_LIST(m_listVIPList,VIPList,pList,i)
	{
		delete pList;
	}
	MYSTL_CLEAR_LIST(m_listVIPList);
	m_pListOnScreen=NULL;

    m_pInput = NULL;
    MYSTL_ITERATE_LONG_MAP(m_pMenu->m_mapMenuInputs, VIPMenuInput, m_pInput, it)
    {
        VIPVariable *pVariable = (m_pInput!=NULL && m_pInput->m_iVariableID!=-1 ? 
            m_pMenuCollection->m_mapVariables_Find(m_pInput->m_iVariableID) : NULL);

        if(pVariable)
            m_pInput->m_pTextElement->m_sText = pVariable->m_sCurrentValue;

        break;
    }

	MYSTL_ITERATE_LONG_MAP(m_pMenu->m_mapMenuElements,VIPMenuElement,pElement,ie)
	{
        if( !pElement)
            continue;

		if( pElement->m_iVariableIDHidden!=0 )
		{
			// Skip this item based on the value of the variable
			VIPVariable *pVariable = m_pMenu->m_pCollection->m_mapVariables_Find(abs(pElement->m_iVariableIDHidden));
			if( pVariable )
			{
				// A positive variable means hide if 0, negative means hide if non 0
				if( pElement->m_iVariableIDHidden>0 && pVariable->m_sCurrentValue=="0" )
					continue;
				else if( pElement->m_iVariableIDHidden<0 && pVariable->m_sCurrentValue!="0" )
					continue;
			}
		}

		// Be sure the up and down are *after* the list with the basket, otherwise they will always be drawn
		if( pElement->m_sBasketOption=="<%=UP%>" )
		{
			if( m_iListOffset==0 )
				continue;
		}
		else if( pElement->m_sBasketOption=="<%=DOWN%>" )
		{
			if( !m_bListIsMore )
				continue;
		}
		else if( pElement->m_sBasketOption!="" )
		{
			bool bInverse=false;
			if( pElement->m_sBasketOption[0]=='!' )
			{
				bInverse=true;
				pElement->m_sBasketOption = pElement->m_sBasketOption.substr(1);
			}
			if( m_pBasketItem )
			{
				bool bFound=false;
				MYSTL_ITERATE_LIST(m_pBasketItem->m_listBasketOptions,BasketOption,pBO,itBasketOptions)
				{
					if( pBO->m_sID == pElement->m_sBasketOption )
					{
						bFound=true;
						break;
					}
				}
				if( (!bInverse && !bFound) || (bInverse && bFound) )
					continue;
			}
			else if( !bInverse )  // If the option doesn't exist we won't draw unless we're set to Inverse
				continue;
		}

		if( pElement->MenuElementID()==MENU_ELEMENT_GRAPHIC )
		{
			VIPMenuElement_Graphic *pGraphic = (VIPMenuElement_Graphic *) pElement;

			int Width = pGraphic->m_iWidth==0 ? pGraphic->m_pMenu->m_pCollection->m_iWidth : pGraphic->m_iWidth;
			int Height = pGraphic->m_iHeight==0 ? pGraphic->m_pMenu->m_pCollection->m_iHeight : pGraphic->m_iHeight;

			// In the designer we want to work with the file first
#ifdef VIPDESIGN
			if( !DrawImage(pGraphic->m_sFileName.c_str(),pGraphic->m_iX,pGraphic->m_iY,Width,Height) )
#endif
				if( pGraphic->m_pGraphicData && pGraphic->m_iGraphicSize )
					DrawImage(GRAPHICTYPE_JPG,pGraphic->m_pGraphicData,pGraphic->m_iGraphicSize,pGraphic->m_iX,pGraphic->m_iY,Width,Height);
		}

		if( pElement->MenuElementID()==MENU_ELEMENT_TEXT || pElement->MenuElementID()==MENU_ELEMENT_LIST )
		{
			VIPMenuElement_Text *pText = (VIPMenuElement_Text *) pElement;
			string Text = SubstituteValues(pText->m_sText);
			SetTextProperties(pText->m_iSize,pText->m_sFont.c_str(),pText->m_R,pText->m_G,pText->m_B);

			MyRect rect(pText->m_iY,pText->m_iX,pText->m_iWidth+pText->m_iX,pText->m_iHeight+pText->m_iY);

			if( pElement->MenuElementID()==MENU_ELEMENT_LIST )
			{
				m_bListIsMore=false;
				VIPMenuElement_List *pList = (VIPMenuElement_List *) pText;
				m_pListOnScreen=pList;

				int LineSpacing = pList->m_iMaxItemsPerScreen ? (rect.bottom-rect.top) / pList->m_iMaxItemsPerScreen : 0;

				if( pText->m_sText=="<%=BASKET%>" )
				{
#ifndef SYMBIAN
					ListBasketItem::iterator itList=m_listBasket.begin();
					// Skip the number of items in the offset
					for(int i=0;i<m_iListOffset;++i)
					{
						if( itList!=m_listBasket.end() )
							++itList;
					}

					m_pBasketItem_First = *itList;
#else
					TInt itList = 0;
					if (m_iListOffset - 1 < m_listBasket.Count())
						itList = m_iListOffset - 1;
					else
						itList = m_listBasket.Count() - 1;
					m_pBasketItem_First = m_listBasket[itList];

#endif

					int BasketLineNum=0;
					string::size_type pos=0;
					for(int iLineNum=0;iLineNum<pList->m_iMaxItemsPerScreen;++iLineNum)
					{
						string::size_type pos_column=0,pos_tab=0;

						BasketItem *pBasketItem;
#ifndef SYMBIAN
						if( itList==m_listBasket.end() )
							break;
						else
							pBasketItem = *itList;
#else
						if (itList == m_listBasket.Count())
							break;
						else
							pBasketItem = m_listBasket[itList];
#endif

						BasketLineNum++;
						VIPList *pL = new VIPList();
						pL->m_pBasketItem = pBasketItem;
						pL->m_sValue = pBasketItem->m_sID;
						MYSTL_ADDTO_LIST(m_listVIPList,pL);

						int XPos_Number = atoi(StringUtils::Tokenize(pList->m_sTabStops,",",pos_tab).c_str());
						int XPos_Description = atoi(StringUtils::Tokenize(pList->m_sTabStops,",",pos_tab).c_str());
						int XPos_Amount = atoi(StringUtils::Tokenize(pList->m_sTabStops,",",pos_tab).c_str());
						int XPos_RightEdge = atoi(StringUtils::Tokenize(pList->m_sTabStops,",",pos_tab).c_str());
						if( XPos_RightEdge==0 )
							XPos_RightEdge = rect.right;

						MyRect r(rect);

						r.left = XPos_Number;
						r.right = XPos_Description;
						r.top = iLineNum*LineSpacing + rect.top;
						r.bottom = (iLineNum+1) * LineSpacing + rect.top;

						// If we're on the list line already, and this item has options, just exit now without displaying this item
						if( iLineNum==pList->m_iMaxItemsPerScreen-1 && MYSTL_SIZEOF_LIST(pBasketItem->m_listBasketOptions)>0 )
							break;

						// Write the number
						string Field = StringUtils::itos(BasketLineNum);

						DrawText(Field.c_str(), r);

						r.left = XPos_Description;
						r.right = XPos_Amount;

						// Write the description
						Field = pBasketItem->m_sDescription;
						DrawText(Field.c_str(), r);

						if( pBasketItem->m_iCost )
						{
							r.left = XPos_Amount;
							r.right = XPos_RightEdge;

							// Write the amount
							Field = StringUtils::itos(pBasketItem->m_iCost);
							DrawText(Field.c_str(), r);
						}

						// Write out any options
						MYSTL_ITERATE_LIST(pBasketItem->m_listBasketOptions,BasketOption,pBO,itBasketOptions)
						{
							iLineNum++;
							r.top = iLineNum*LineSpacing + rect.top;
							r.bottom = (iLineNum+1) * LineSpacing + rect.top;

							// If we're not able to display all the options for this item, then 
							// when the user scrolls down it should stay on this line item
#ifndef SYMBIAN
							bool bLastLine = (iLineNum==pList->m_iMaxItemsPerScreen-1) && itBasketOptions!=pBasketItem->m_listBasketOptions.end();
#else
							bool bLastLine = (iLineNum==pList->m_iMaxItemsPerScreen-1) && itBasketOptions != pBasketItem->m_listBasketOptions.Count();
#endif
							if( bLastLine )
							{
								m_bListIsMore = true;
								Field = "... more ...";
								itList--; // Scroll back one so that we hit this item again
							}
							else 
								Field = pBO->m_sDescription;

							r.left = XPos_Description;
							r.right = XPos_Amount;
							DrawText(Field.c_str(), r);

							if( bLastLine )
								break;

							if( pBO->m_iCost )
							{
								r.left = XPos_Amount;
								r.right = XPos_RightEdge;

								// Write the amount
								Field = StringUtils::itos(pBO->m_iCost);
								DrawText(Field.c_str(), r);
							}
						}
						++itList;
					}

					// We might have set it above already
					if( !m_bListIsMore )
#ifndef SYMBIAN
						m_bListIsMore = (itList!=m_listBasket.end());
#else
						m_bListIsMore = (itList!=m_listBasket.Count());
#endif

					if( m_bListIsMore )
#ifndef SYMBIAN
						m_pBasketItem_Last = *itList;
#else
						m_pBasketItem_Last = m_listBasket[itList];
#endif
				}
				else
				{

					string::size_type pos=0;
					// Skip the number of items in the offset
					for(int i=0;i<m_iListOffset;++i)
						StringUtils::Tokenize(Text,"\n",pos);

					//--- CHANGED4WM5 ----//
					int iLineNum = 0;
					for( iLineNum=0;iLineNum<pList->m_iMaxItemsPerScreen;++iLineNum)
					{
						string Line = StringUtils::Tokenize(Text,"\n",pos);
                        StringUtils::Replace(Line, "\r", "");
						if( Line.length()==0 )
							break;

						string::size_type pos_column=0,pos_tab=0;

						VIPList *pL = new VIPList();
						pL->m_sValue = StringUtils::Tokenize(Line,"\t",pos_column);

						MYSTL_ADDTO_LIST(m_listVIPList,pL);

						int XPos = atoi(StringUtils::Tokenize(pList->m_sTabStops,",",pos_tab).c_str());
						string Field="";
						if( XPos==0 )  
						{
							Field = StringUtils::itos(iLineNum+1);
						}
						else
							Field = StringUtils::Tokenize(Line,"\t",pos_column);

						bool bFirstLoop=true;
						while(true)
						{
							MyRect r(rect);
							int NextXPos = atoi(StringUtils::Tokenize(pList->m_sTabStops,",",pos_tab).c_str());
							if( !NextXPos )
								r.right = rect.right;
							else
								r.right = NextXPos;
							r.left = XPos;
							r.top = iLineNum*LineSpacing + rect.top;
							r.bottom = (iLineNum+1) * LineSpacing + rect.top;

							DrawText(Field.c_str(), r);
							MyRect rectRealText(r);
							rectRealText.left = pText->m_iX + 5;
							DrawText(Line.c_str(), rectRealText);

							if( NextXPos==0 )
								break;

							Field = StringUtils::Tokenize(Line,"\t",pos_column);
							if( bFirstLoop )
							{
								bFirstLoop=false;
								pL->m_sText = Field;
							}
							XPos = NextXPos;
						}
					}
					if( iLineNum==pList->m_iMaxItemsPerScreen )
					{
						string Line = StringUtils::Tokenize(Text,"\n",pos);
                        StringUtils::Replace(Line, "\r", "");
						m_bListIsMore=Line.length()!=0;
					}
					else
						m_bListIsMore=false;
				}
			}
			else
				DrawText(Text.c_str(), rect);

			Draw3dRect(rect);
		}
	}
}

void RenderMenu::KeyPressed(int KeyCode)
{
	bool bRedrawScreen = false;



#if !defined(SYMBIAN) && !defined(SMARTPHONE)
#ifdef EMULATOR
	if( g_pPlutoConfig->m_bSendKeyStrokes && g_pPlutoConfig->m_pBDCommandProcessor )
	{
		BDCommand *pCommand = new BD_PC_KeyWasPressed(KeyCode, EventType);
		g_pPlutoConfig->m_pBDCommandProcessor->AddCommand(pCommand);
	}
#endif
#endif


#ifdef SYMBIAN
	if(
		((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_bSendKeyStrokes     && 
		((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_pBDCommandProcessor
		)
	{
		BDCommand *pCommand = new BD_PC_KeyWasPressed(KeyCode, 2 /*execute*/);
		((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_pBDCommandProcessor->AddCommand(pCommand);
	}
#endif

#ifdef SMARTPHONE
	OrbiterApp::GetInstance()->SendKey(KeyCode, 2);
#endif

	if( !m_pMenu || !m_pMenuCollection )
		return;

    m_pInput = NULL;
    MYSTL_ITERATE_LONG_MAP(m_pMenu->m_mapMenuInputs, VIPMenuInput, m_pInput, it)
    {
        //just get the first one
        break;
    }

	VIPVariable *pVariable = (m_pInput!=NULL && m_pInput->m_iVariableID!=-1 ? 
		m_pMenuCollection->m_mapVariables_Find(m_pInput->m_iVariableID) : NULL);

	if( (KeyCode >= BUTTON_1_CONST && KeyCode <= BUTTON_9_CONST) ||  KeyCode == BUTTON_0_CONST ) 
	{
		if( pVariable && pVariable->m_iFormat==MENU_VARFORMAT_TEXT  )
			pVariable->m_sCurrentValue += NumericToText(KeyCode/*-48*/,pVariable->m_sCurrentValue.c_str());
		else if( pVariable )
			pVariable->m_sCurrentValue += ('0' + ButtonToNumeric(KeyCode)); //'0' or '1' or ... '9'
	}
	else if( KeyCode==BUTTON_Phone_C_CONST && pVariable && pVariable->m_sCurrentValue.length()>0 )
		pVariable->m_sCurrentValue = pVariable->m_sCurrentValue.substr(0,pVariable->m_sCurrentValue.length()-1);

    if(m_pInput && m_pInput->m_pTextElement)
    {
        m_pInput->m_pTextElement->m_sText = pVariable->m_sCurrentValue;
        bRedrawScreen = true;
    }

	bool bFilledInput = (pVariable && pVariable->m_sCurrentValue.length()>=m_pInput->m_iNumberOfChars);
	if( m_pListOnScreen )
	{
		// The lists are an exception.  They are considered 'full' when the user
		// selects something
		bFilledInput = false;

		if( !m_pListOnScreen->m_iIsAlpha )
		{
			// It's a 'press the number type of list'
			if(
				((KeyCode >= BUTTON_1_CONST && KeyCode <= BUTTON_9_CONST) ||  KeyCode == BUTTON_0_CONST) || 
				KeyCode==BUTTON_Asterisk_CONST || 
				KeyCode==BUTTON_Pound_CONST
				)
			{
				int Number = KeyCode - BUTTON_1_CONST;

				if(KeyCode == BUTTON_Asterisk_CONST)
					Number=11;
				else if(KeyCode == BUTTON_Pound_CONST)
					Number=12;
				else if(KeyCode == BUTTON_0_CONST)
					Number=10;

				if( Number < MYSTL_SIZEOF_LIST(m_listVIPList) && Number >= 0 )
				{
					MYSTL_POSITION_INLIST(m_listVIPList, VIPList, pL, Number);
					m_sListValue=pL->m_sValue;
					m_sListText=pL->m_sText;
					m_sListSearch=KeyCode;
					m_pBasketItem = pL->m_pBasketItem;
					bFilledInput = true;
				}
				bRedrawScreen = true;
			}
			else if( KeyCode=='U' ) //TODO! map 'U' to an unsigned char 
			{
				if( m_iListOffset>0 )
				{
					// It's complicated to scroll basket items because there are a variable number
					// of options per basket item
					if( m_pBasketItem_First )
					{
						//m_iListOffset=m_listBasket.size();
						MYSTL_SIZEOF_LIST(m_listBasket);
#ifndef SYMBIAN
						ListBasketItem::reverse_iterator itList=m_listBasket.rbegin();
#else
						TInt itList = m_listBasket.Count() - 1;
#endif
#ifndef SYMBIAN
						while( (*itList)!=m_pBasketItem_First )
						{
							--m_iListOffset;
							itList++;
						}
#else
						while( itList > 0 )
						{
							--m_iListOffset;
							itList--;
						}
#endif

						--m_iListOffset;
#ifndef SYMBIAN
						++itList;  // This will be the offset for the item right above where we're at.
#else
						itList--;
#endif
						// Now we're pointing to the first, scroll back X lines
						int Lines=0;
						while( Lines<m_pListOnScreen->m_iMaxItemsPerScreen && m_iListOffset>0 )
						{
#ifndef SYMBIAN
							BasketItem *pBI = (*itList);
#else
							BasketItem *pBI = m_listBasket[itList];
#endif

							// this item will scroll us up the number of options + 1
							//							Lines+=pBI->m_listBasketOptions.size() + 1;
							Lines+=MYSTL_SIZEOF_LIST(pBI->m_listBasketOptions) + 1;
							--m_iListOffset;
							++itList;
						}
					}
					else
					{
						m_iListOffset -= m_pListOnScreen->m_iMaxItemsPerScreen;
						if( m_iListOffset<0 )
							m_iListOffset=0;
					}
				}
				bRedrawScreen = true;
			}
			else if( KeyCode=='D' && m_bListIsMore ) //TODO! map 'D' to an unsigned char 
			{
				if( m_pBasketItem_Last )
				{
					m_iListOffset=0;
#ifndef SYMBIAN
					ListBasketItem::iterator itList=m_listBasket.begin();
					while( (*itList)!=m_pBasketItem_Last )
					{
						++m_iListOffset;
						itList++;
					}
#else
					TInt itList = 0;
					while( m_listBasket[itList]!=m_pBasketItem_Last )
					{
						++m_iListOffset;
						itList++;
					}
#endif

				}
				else
					m_iListOffset += m_pListOnScreen->m_iMaxItemsPerScreen;
				bRedrawScreen = true;
			}
		}
		else
		{
		}
	}

	MYSTL_ITERATE_LONG_MAP(m_pMenu->m_mapMenuResolutions,VIPMenuResolution,pRes,iRes)
	{
		// The resolution is triggered by the key, or if we filled the input
		//strchr(pRes->m_sTerminatingKey.c_str(),KeyCode)
		if( KeyCodeInString(pRes->m_sTerminatingKey, KeyCode) || (bFilledInput && pRes->m_sTerminatingKey.find("F")!=string::npos) )
		{
			// This resolution matches so far.  See if we have to match a variable
			if( pRes->m_iCriteriaVariableID!=-1 )
			{
				VIPVariable *pVariable = m_pMenuCollection->m_mapVariables_Find( pRes->m_iCriteriaVariableID );
				if( pVariable && pVariable->m_sCurrentValue!=pRes->m_sCriteriaVariableValue )
					continue;
			}
			//
			if(pRes->m_sProgramName.length())
			{
				string sParams;
				string sUrl = pRes->m_sProgramName;
				if(pRes->m_iReportToServer)
				{
					MYSTL_ITERATE_LONG_MAP(m_pMenuCollection->m_mapVariables, VIPVariable, pVar, iVar)
					//for(map<long,class VIPVariable *>::iterator it = m_pMenuCollection->m_mapVariables.begin();
					//	it != m_pMenuCollection->m_mapVariables.end(); it++)
					{
						VIPVariable *pVariable = pVar;
						sParams += pVariable->m_sDescription + "=" + pVariable->m_sCurrentValue + "&";
					}
				}

				//if(sParams != "")
				//	sParams += "dummy=0";

				sUrl += sParams;	

#if !defined(SYMBIAN) && !defined(SMARTPHONE)
				MessageBox(sUrl.c_str(), "Running program"); //debugging
#endif

				OpenProgram(sUrl);
				//bRedrawScreen = true;
				break;
			}

			// OK this res is a match
			if( pRes->m_iActionVariableID>0 )
			{
				VIPVariable *pActVariable = m_pMenuCollection->m_mapVariables_Find( pRes->m_iActionVariableID );
				if( pActVariable )
				{
					pActVariable->m_sCurrentValue = SubstituteValues(pRes->m_sActionVariableValue);
				}
			}

			if( pRes->m_iBasketAction )
			{
				bool bRemoved=false;  // Used for all the toggles
				switch( pRes->m_iBasketAction )
				{
				case BASKET_ADD:
					if( pRes->m_iBasketToggle )
					{
						MYSTL_ITERATE_LIST(m_listBasket,BasketItem,bi,itList)
						{
							if( bi->m_sID == pRes->m_sBasketID )
							{
#ifndef SYMBIAN
								m_listBasket.erase(itList);
#else
								m_listBasket.Remove(itList);
#endif
								bRemoved=true;
								break;
							}
						}
					}
					if( !bRemoved )
					{
						m_pBasketItem = new BasketItem(pRes->m_sBasketID,pRes->m_sBasketDescription,1,pRes->m_iBasketCost);
						MYSTL_ADDTO_LIST(m_listBasket,m_pBasketItem);
					}
					break;
				case BASKET_REMOVE:
					if( m_pBasketItem )
					{
#ifndef SYMBIAN
						m_listBasket.remove(m_pBasketItem);
#else
						TInt entry = m_listBasket.Find(m_pBasketItem);
						if (entry != KErrNone)
							m_listBasket.Remove(entry);
#endif
						m_pBasketItem=NULL;
					}
					break;
				case BASKET_ADD_OPTION:
					if( m_pBasketItem )
					{
						if( pRes->m_iBasketToggle )
						{
							MYSTL_ITERATE_LIST(m_pBasketItem->m_listBasketOptions,BasketOption,bo,itBasketOptions)
							{
								if( bo->m_sID == pRes->m_sBasketID )
								{
#ifndef SYMBIAN
									m_pBasketItem->m_listBasketOptions.erase(itBasketOptions);
#else
									m_pBasketItem->m_listBasketOptions.Remove(itBasketOptions);
#endif
									bRemoved=true;
									break;
								}
							}
						}
						if( !bRemoved )
						{
							BasketOption *pOption = new BasketOption(pRes->m_sBasketID,pRes->m_sBasketDescription,1,pRes->m_iBasketCost);
							MYSTL_ADDTO_LIST(m_pBasketItem->m_listBasketOptions,pOption);
						}
					}
					break;
				case BASKET_CLEAR:
					{
						MYSTL_ITERATE_LIST(m_listBasket,BasketItem,bi,itList)
						{
							delete bi;
						}
						//						m_listBasket.clear();
						MYSTL_CLEAR_LIST(m_listBasket);
					}
					break;
				}
			}

			if( pRes->m_iMenuNumber_Goto<MYSTL_SIZEOF_LONG_MAP(m_pMenuCollection->m_mapMenus))
			{
				VIPMenu *pMenu=m_pMenuCollection->m_mapMenus_Find(pRes->m_iMenuNumber_Goto);
				SwitchToMenu(pMenu);

#if defined(SYMBIAN) || defined(SMARTPHONE)
				bRedrawScreen = true;
#endif
				m_pBasketItem_First=m_pBasketItem_Last=NULL;
				m_iListOffset=0;
				break;
			}
			if( pRes->m_iCloseRequest || pRes->m_iReportToServer )
			{
#ifdef VIPDESIGN
				if( g_pPlutoConfig->m_bWaitingForTermination )
				{
					g_pPlutoConfig->m_iResolutionValue = pRes->m_iCloseRequest;
					g_pPlutoConfig->m_bWaitingForTermination = false;
					MessageBox("Request Closed","Emulation over");
				}
				else
				{
					/*
					CResetEmulatorDlg c(m_pMenuCollection,true);
					c.DoModal();
					if( pRes->m_iCloseRequest )
					MessageBox("Request Closed","Terminating Application");
					*/
				}
#endif
			}
			if( pRes->m_iHideApp )
			{
				MessageBox("Hiding Application","Hiding Application");
			}
		}
	}
	if(bRedrawScreen)
		Redraw();
}
int  RenderMenu::ButtonToNumeric(int Key)
{
	int IndexKey;
	switch(Key)
	{
	case BUTTON_0_CONST:	IndexKey = 0; break;
	case BUTTON_1_CONST:	IndexKey = 1; break;
	case BUTTON_2_CONST:	IndexKey = 2; break;
	case BUTTON_3_CONST:	IndexKey = 3; break;
	case BUTTON_4_CONST:	IndexKey = 4; break;
	case BUTTON_5_CONST:	IndexKey = 5; break;
	case BUTTON_6_CONST:	IndexKey = 6; break;
	case BUTTON_7_CONST:	IndexKey = 7; break;
	case BUTTON_8_CONST:	IndexKey = 8; break;
	case BUTTON_9_CONST:	IndexKey = 9; break;
	default:				IndexKey = -1; break;
	}
	return IndexKey;
}
char RenderMenu::NumericToText(int Key,const char *Value)
{
	char *Keys[] = {".",".","abc","def","ghi","jkl","mno","pqrs","tuv","wxyz"};
	char *Values = Keys[ButtonToNumeric(Key)];

#if defined(SYMBIAN) || defined(SMARTPHONE)
	bool bNewCharacter = true;
#else
	bool bNewCharacter=(Value[0]==0 || m_ctLastKey + (CLOCKS_PER_SEC/2) < clock());
	m_ctLastKey = clock();
#endif

	if( !bNewCharacter )
	{
		// Be sure that the last char in the input is in the same group
		char *LastChar = strchr(Values,Value[strlen(Value)-1]);
		if( LastChar!=NULL )
		{
			// It really is a repeat
			LastChar++;
			if( !*LastChar )  // If we're at the end, loop back again
				LastChar = Values;
			return *LastChar;
		}
		else
			bNewCharacter=true;
	}

	return Values[0];
}

string RenderMenu::SubstituteValues(string Input)
{
#ifndef SYMBIAN_ALL
	string::size_type pos=0;
	while( (pos=Input.find("<%="))!=string::npos )
	{
		string::size_type end = Input.find("%>",pos+1);
		if( end==string::npos )
			return "ERROR";

		string Token = StringUtils::ToUpper(Input.substr(pos+3,end-pos-3));

		if( Token[0]=='V' )
		{
			// It's a variable
			int VariableID = atoi(Input.substr(pos+4).c_str());
			VIPVariable *pVariable = m_pMenu->m_pCollection->m_mapVariables_Find(VariableID);
			if( pVariable )
			{
				if( pVariable->m_iFormat==MENU_VARFORMAT_NUMERIC_PIN )
				{
					string stars="******************************";
					Input = Input.replace(pos,end-pos+3,stars.substr(0,pVariable->m_sCurrentValue.length()));
				}
				else
					Input = Input.replace(pos,end-pos+3,pVariable->m_sCurrentValue);
			}
			else
				Input = Input.replace(pos,end-pos+3,"unknown variable");
		}
		else if( Token=="LIST_VALUE" )
		{
			Input = Input.replace(pos,end-pos+3,m_sListValue);
		}
		else if( Token=="LIST_TEXT" )
		{
			Input = Input.replace(pos,end-pos+3,m_sListText);
		}
		else if( Token=="LIST_SEARCH" )
		{
			Input = Input.replace(pos,end-pos+3,m_sListSearch);
		}
		else if( Token=="BASKET_TOTAL" )
		{
			long Total = 0;
			MYSTL_ITERATE_LIST(m_listBasket,BasketItem,bi,itList)
			{
				Total += bi->m_iCost;
				MYSTL_ITERATE_LIST(bi->m_listBasketOptions,BasketOption,pBO,itBasketOptions)
				{
					Total += pBO->m_iCost;
				}
			}
			Input = Input.replace(pos,end-pos+3, StringUtils::itos(Total));
		}
		else if( Token=="BASKET_DESCRIPTION" && m_pBasketItem )
		{
			Input = Input.replace(pos,end-pos+3,m_pBasketItem->m_sDescription);
		}
		else
			Input = Input.replace(pos,end-pos+3,"??ERROR??");

	}
#endif //#ifndef SYMBIAN_ALL
	return Input;
}

