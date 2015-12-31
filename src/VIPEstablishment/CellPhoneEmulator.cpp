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
#include "../VIPShared/VIPIncludes.h"
#include "../common/PlutoConfig.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"

#include "CellPhoneEmulator.h"
#include "EstablishmentSocket.h"
#include "RA/RA_Action.h"
#include "../VIPShared/VIPMenu.h"
#include "../VIPShared/VR_ShowMenu.h"

const char *CellPhoneEmulator::ReceiveRequest(long iRequestSize,const char *pRequest,long *iResponseSize)
{
	EstablishmentSocket *pSocket = new EstablishmentSocket(LoggerWrapper::GetInstance(),1,"localhost:3461","foo");
	if( !pSocket->Connect() )
	{
		fprintf(stderr,"Could not connect.\n");
		return NULL;
	}

	char RequestHeader[13] = "PVIPVVEEEENN";
	unsigned short *usi = (unsigned short *) (RequestHeader+4);
	*usi = 123; // Software version
	unsigned long *ui = (unsigned long *) (RequestHeader+6);
	*ui = 4838; // m_dwEstablishmentID;
	usi = (unsigned short *) (RequestHeader+10);
	*usi = (unsigned short) 1; // m_listRequests.size();

	pSocket->SendData(12,RequestHeader);

	{
		char RequestHeader[9] = "ZZZZCCCC";
		ui = (unsigned long *) (RequestHeader);
		*ui = iRequestSize;
		ui = (unsigned long *) (RequestHeader+4);
		*ui = 1234;
		pSocket->SendData(8,RequestHeader);
		pSocket->SendData(iRequestSize,pRequest);
	}

	char ResponseHeader[15] = "----VVZZZZCCCC";

	if (!pSocket->ReceiveData(14,ResponseHeader) )  // Not a valid message
	{
		fprintf(stderr,"invalid response 1\n");
		return NULL;
	}

	unsigned long *SanityCheck = (unsigned long *) ResponseHeader;
	unsigned short *Version = (unsigned short *) (ResponseHeader+4);
	unsigned long *Size = (unsigned long *) (ResponseHeader+6);
	unsigned short *CheckSum = (unsigned short *) (ResponseHeader+10);

	if( *SanityCheck!=1380996688 )  // PVPR
	{
		fprintf(stderr,"invalid response 1\n");
		return NULL;
	}

	char *data = new char[*Size];
	if( !pSocket->ReceiveData(*Size,data) )
	{
		delete[] data;
		fprintf(stderr,"invalid response 2\n");
		return NULL;
	}

	*iResponseSize=*Size;
//		pRequest->ParseResponse(*Size,data);
	char NumActions[3];

	if (!pSocket->ReceiveData(2,NumActions) )  // Not a valid message
	{
		fprintf(stderr,"invalid response 3\n");
		return NULL;
	}

	unsigned short *iNumActions = (unsigned short *) NumActions;
	for(long iActionCount=0;iActionCount<*iNumActions;++iActionCount)
	{
		char ActionHeader[9] = "ZZZZCCCC";
		if (!pSocket->ReceiveData(8,ActionHeader) )  // Not a valid message
		{
			fprintf(stderr,"invalid response 4\n");
			return NULL;
		}
		unsigned long *ActionSize = (unsigned long *) ActionHeader;
		unsigned long *ActionChecksum = (unsigned long *) (ActionHeader+4);

		char *actiondata = new char[*ActionSize];
		if( !pSocket->ReceiveData(*ActionSize,actiondata) )
		{
			delete[] actiondata;
			fprintf(stderr,"invalid response 5\n");
			return NULL;
		}

		RA_Action *pAction=RA_Action::BuildActionFromData(m_pPlutoConfig,*ActionSize,actiondata);
		pAction->ProcessAction();
//			m_listActions.push_back(pAction);
		delete[] actiondata;
	}
//		delete pRequest;
	delete pSocket;
	return data;


	/*


	RA_Request *pRA_Request = RA_Request::BuildRequestFromData(m_pVIPConfig,iRequestSize,pRequest);
	pRA_Request->ProcessRequest();
	if( pRA_Request->ID()==VRP_REQUEST_SHOW_MENU )
		ShowMenu((VR_ShowMenu *) pRA_Request);

	pRA_Request->ConvertResponseToBinary();
	*iResponseSize = pRA_Request->ResponseSize();
	return pRA_Request->Response();
*/
}

void CellPhoneEmulator::ShowMenu(class VR_ShowMenu *pMenuAction)
{
	VIPMenuCollection *c=pMenuAction->m_pMenuCollection;
	if( c->m_listMenus.size()<1 )
		return;

	VIPMenu *pActiveMenu = c->m_listMenus.pop_front();
	while(true)
	{
		printf("***AT MENU***: %d  ID: %d\n",pActiveMenu->m_iMenuNumber,pActiveMenu->m_iMenuID);
		for(size_t i=0;i<pActiveMenu->m_vectMenuElements.size();++i)
		{
			VIPMenuElement *pElement = pActiveMenu->m_vectMenuElements[i];
			printf("Element %d type %d x: %d y: %d w: %d h: %d\n",
				pElement->m_iElementNumber,(long) pElement->MenuElementID(),
				pElement->m_iX,pElement->m_iY,pElement->m_iWidth,pElement->m_iHeight);
		}
		
		VIPMenuResolution *pTerminatingResolution=NULL;
		VIPMenuInput *pInput=NULL;
		if( pActiveMenu->m_vectMenuInputs.size()==0 )
		{
			char input[100];
			printf("Displaying menu until any key is pressed.\n");
			scanf(input,"press enter: %d");
			return;
		}
		pInput = pActiveMenu->m_vectMenuInputs[0];
		if( pInput->m_iVariableID!=-1 && pInput->m_iClearVariable )
		{
			VIPVariable *pVariable = c->m_mapVariables[ pInput->m_iVariableID ];
			if( pVariable )
	            pVariable->m_sCurrentValue="";
		}
		while( !pTerminatingResolution )
		{
			printf("Waiting for input: %d #chars: %d term#chars: %d var: %d text: %d  ... ",
				pInput->m_iInputNumber, pInput->m_iNumberOfChars,
				pInput->m_iTerminateWhenNumberOfChars,pInput->m_iVariableID,
				(pInput->m_pTextElement ? pInput->m_pTextElement->MenuElementID() : 0));
			long ch = getc(stdin);

			if( pInput->m_iVariableID!=-1 )
			{
				VIPVariable *pVariable = c->m_mapVariables[ pInput->m_iVariableID ];
				if( pVariable )
	                pVariable->m_sCurrentValue+=(char) ch;
			}

			for(size_t iRes = 0;iRes<pActiveMenu->m_vectMenuResolutions.size();++iRes)
			{
				VIPMenuResolution *pRes = pActiveMenu->m_vectMenuResolutions[iRes];
				if( strchr(pRes->m_sTerminatingKey.c_str(),ch) )
				{
					// This resolution matches so far.  See if we have to match a variable
					if( pRes->m_iActionVariableID!=-1 )
					{
						VIPVariable *pVariable = c->m_mapVariables[ pRes->m_iActionVariableID ];
						if( pVariable && pVariable->m_sCurrentValue!=pRes->m_sCriteriaVariableValue )
							continue;
					}
					// OK this res is a match
					if( pRes->m_iMenuNumber_Goto!=-1 )
					{
						pActiveMenu = pActiveMenu = c->m_vectMenus[pRes->m_iMenuNumber_Goto];
						pTerminatingResolution=pRes;
					}
					if( pRes->m_iCloseRequest )
					{
						printf("Closing request & Reporting tokens...\n");
						map<unsigned short,class VIPVariable *>::iterator itVariables;
						for(itVariables=c->m_mapVariables.begin();itVariables!=c->m_mapVariables.end();++itVariables)
						{
							VIPVariable *pVariable = (*itVariables).second;
							printf("Variable %d value %s\n",
								pVariable->m_iVariableID,pVariable->m_sCurrentValue.c_str());
						}
						pTerminatingResolution=pRes;
					}
					if( pRes->m_iHideApp )
					{
						printf("App ended\n");
						return;
					}
				}
			}
		}

		if( pTerminatingResolution && pTerminatingResolution->m_iCloseRequest )
		{
			map<unsigned short,class VIPVariable *>::iterator itVariables;
			for(itVariables=c->m_mapVariables.begin();itVariables!=c->m_mapVariables.end();++itVariables)
			{
				VIPVariable *pVariable = (*itVariables).second;
				if( pVariable && pVariable->m_iPhoneSetsValue )
					pMenuAction->m_listReturnedVariables.push_back(pVariable);
			}
			pMenuAction->m_iMacAddress=123456;
			pMenuAction->m_iMenuCollectionID = pMenuAction->m_pMenuCollection->m_iMenuCollectionID;

			printf("Reporting tokens back to server\n");
			return;
		}
	}
}
