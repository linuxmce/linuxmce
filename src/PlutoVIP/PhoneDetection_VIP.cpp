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
/**
 *
 * @file PhoneDetection_VIP.cpp
 * @brief implementation of the PhoneDetection_VIP class
 * @author
 *
 */

/**
 *
 * Copyright Notice goes here
 *
 */
 
#include "stdafx.h"
#include "PlutoUtils/CommonIncludes.h"	
#include "VIPShared/VIPIncludes.h"
#include "VIPShared/PlutoConfig.h"
#include "PlutoUtils/MyStl.h"
#include "VR_IdentifyPhone.h"
#include "BD/PhoneDevice.h"
#include "RA/RA_Processor.h"
#include "VIPShared/VIPMenu.h"
#include "DCE/Picture.h"
#include "PhoneDetection_VIP.h"
#include "ControlsDlg.h"
#include "customer.h"
#include "Registry.h"
#include "DIB.h"
#include "ImageDefs.h"
#include "ADO2.h"
#include "stdlib.h"
#include "RecordError.h"
#include "DatabaseDefs.h"

#define RECORD_DOES_NOT_EXIST		0
#define RECORD_EXISTS				1

UINT FireErrorRecord(LPVOID pParam);

/** global map for customers */
map<unsigned long, Customer*> m_mapCustomers;
CString MACAddress;

PhoneDetection_VIP::PhoneDetection_VIP()
{
	m_pThreadMutex = new pluto_pthread_mutex_t("detection mutex");
	pthread_mutexattr_init(&m_MutexAttr);
	pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
	m_pThreadMutex->Init(&m_MutexAttr);
	m_pNewAB=m_pActiveAB=m_pRecentAB=NULL;
}

/**
 * The derived class must decide how to handle when new devices are detected or lost, 
 * or when the signal strength changes by more than 10 in either direction
 */
void PhoneDetection_VIP::NewDeviceDetected(class PhoneDevice *pDevice)
{
	PLUTO_SAFETY_LOCK(tm,*m_pThreadMutex);

	DWORD CachedRecordVersion=0;
	DWORD CachedPlutoId=0;
	unsigned int IdentifiedPlutoId=pDevice->m_PKID_Users;
	const char *IdentiedPlutoIdPin=pDevice->m_UserVersion.c_str();

	/** @test	A. Riazi -- add this code 
	SELECT FROM MacAddress where PKID_MacAddress=pDevice->m_sMacAddress;
	if( found )
	{
		SELECT FROM PlutoId WHERE PKID_PlutoId = MacAddress.FKID_PlutoId;
		CachedRecordVersion=PlutoId.RecordVersion;
		CachedPlutoId=PlutoId.PlutoID
	}
	*/
	CADODatabase DB;
	CADORecordset record;
	DWORD PlutoID;
	
	/** CString strConnection = DATABASE_PROVIDER; */
	if(DB.Open(DATABASE_PROVIDER))
	{	
		record = CADORecordset(&DB);
		CString szTable;
		MACAddress=pDevice->m_sMacAddress.c_str();
		MACAddress.Replace(_T(":"), _T(""));
		
		szTable.Format(_T("SELECT MacAddress.* FROM [MacAddress] WHERE MacAddress.PKID_MacAddress=\"%s\""), 
				MACAddress.GetBuffer(0));
		
		CADOCommand select(&DB, szTable, CADOCommand::typeCmdText);
		if (record.Execute(&select))
		{
			if (record.GetRecordCount())
			{
				record.GetFieldValue(_T("FKID_PlutoId"), PlutoID);
				record.Close();

				szTable.Format(_T("SELECT PlutoId.* FROM PlutoId WHERE PlutoId.PKID_PlutoId=%d"), 
					PlutoID);
				CADOCommand select(&DB, szTable, CADOCommand::typeCmdText);
				if (record.Execute(&select))
				{
					if (record.GetRecordCount())
					{
						record.GetFieldValue(_T("RecordVersion"), CachedRecordVersion);
						record.GetFieldValue(_T("PKID_PlutoId"),  CachedPlutoId);
					}
					record.Close();
				}
			}
			else
				record.Close();
		}
	}

	CString FileName;

	/** We need to create the customer record first since the request processor may return commands that need to be sent to the phone.  
	 * If the rp returns an unsuccessful, this customer record will be deleted.  It is therefore assumed that the rp will only get actions 
	 * to send stuff to the phone if the request was processed successfully and a valid customer was located.
	 * Otherwise pCustomer will be deleted out from underneath the action
	 */
	 
	Customer *pCustomer = new Customer(this);
	pCustomer->m_pPhoneDevice = pDevice;
	VR_IdentifyPhone ip(g_pPlutoConfig->m_iEstablishmentID,pDevice->m_sID,
 		pDevice->m_iMacAddress,CachedRecordVersion,CachedPlutoId,
		IdentifiedPlutoId,IdentiedPlutoIdPin,pCustomer);

	RA_Processor rp(g_pPlutoConfig->m_iEstablishmentID,g_pPlutoConfig->m_iSoftwareVersion);
	rp.AddRequest(&ip);
	if( !rp.SendRequests(g_pPlutoConfig->m_sServerAddress) )
	{

		/** Log Errors */
		AfxMessageBox(_T("Error receiving information from server."), MB_ICONERROR);

		delete pCustomer;
	}
	else
	{
		if(ip.m_cProcessOutcome==SUCCESSFULLY_PROCESSED) 
		{

			if( ip.m_iUseCache )
			{
				/** @todo ask
				 A. Riazi, fill in these values from teh database
				// some syntax that means create the customer record from the database
				Customer *pCustomer = new Customer(this,ip.m_iPlutoId);
				*/
				CString szTable;
				szTable.Format(_T("SELECT PlutoId.* FROM [PlutoId] WHERE PKID_PlutoId=%d"), PlutoID);
				CADOCommand select(&DB, szTable, CADOCommand::typeCmdText);
				if (record.Execute(&select))
				{
					record.GetFieldValue(_T("PKID_PlutoId"), pCustomer->m_iPlutoId);
					CString email, nickname, firstname, lastname, 
						address, city, state, zip, country, birthdate, 
						comments, establishmentid;

					record.GetFieldValue(_T("email"), email);
					pCustomer->m_sEmail=email.GetBuffer(0);

					record.GetFieldValue(_T("Nickname"), nickname);
					pCustomer->m_sNickname=nickname;

					record.GetFieldValue(_T("FirstName"), firstname);
					pCustomer->m_sFirstName=firstname;

					record.GetFieldValue(_T("LastName"), lastname);
					pCustomer->m_sLastName=lastname;

					record.GetFieldValue(_T("Address"), address);
					pCustomer->m_sAddress=address;

					record.GetFieldValue(_T("City"), city);
					pCustomer->m_sCity=city;

					record.GetFieldValue(_T("State"), state);
					pCustomer->m_sState=state;

					record.GetFieldValue(_T("Zip"), zip);
					pCustomer->m_sZip=zip;

					record.GetFieldValue(_T("Country"), country);
					pCustomer->m_sCountry=country;

					int gender;
					record.GetFieldValue(_T("Gender"), gender);
					if (gender==0)	//male
						pCustomer->m_sGender="m";
					else
						pCustomer->m_sGender="f";
					
					record.GetFieldValue(_T("Birthdate"), birthdate);
					pCustomer->m_sBirthdate=birthdate;
					
					record.GetFieldValue(_T("Comments"), comments);
					pCustomer->m_sComments=comments;

					record.GetFieldValue(_T("EstablishmentId"), establishmentid);
					pCustomer->m_sEstablishmentCustomerId=establishmentid;
					
					pCustomer->m_cAppBar=IMAGE_TYPE_NEW;	/** 0=not detected anymore, 1=new app bar, 2=active, 3=recent */
					
					record.GetFieldValue(_T("RecordVersion"), pCustomer->m_iRecordVersion);
					
					record.GetFieldValue(_T("TotalVisits"), pCustomer->m_iTotalVisit);
					record.GetFieldValue(_T("AvailableVisits"), pCustomer->m_iAvailableVisit);
					record.GetFieldValue(_T("TotalPurchase"), pCustomer->m_iTotalPurchase);
					record.GetFieldValue(_T("AvailablePurchase"), pCustomer->m_iAvailablePurchase);
					
					record.Close();

					/** use chached version of image file */
					FileName=FindCustomerImageFileFromID(pCustomer->m_iPlutoId, GRAPHICTYPE_JPG);

					/** adding attributes and telephone numbers */


					/** adding customer image to ImageListMap */
					PlutoCustomer* pPlutoCustomer=new PlutoCustomer;
					pPlutoCustomer->Time=CTime::GetCurrentTime();	/**< current time */
					

					pPlutoCustomer->ImageNumber=-1;			/**< the customer has no image */
					pPlutoCustomer->ImageType=GRAPHICTYPE_JPG;	/**< the customer has no image */

					mapPlutoImageList[ip.m_iPlutoId]=pPlutoCustomer;

				}
			}
			else	/** not use cache */
			{
				pCustomer->m_iPlutoId=ip.m_iPlutoId;
				pCustomer->m_sEmail=ip.m_sEmail;
				pCustomer->m_sNickname=ip.m_sNickname;
				pCustomer->m_sFirstName=ip.m_sFirstName;
				pCustomer->m_sLastName=ip.m_sLastName;
				pCustomer->m_sAddress=ip.m_sAddress;
				pCustomer->m_sCity=ip.m_sCity;
				pCustomer->m_sState=ip.m_sState;
				pCustomer->m_sZip=ip.m_sZip;
				pCustomer->m_sCountry=ip.m_sCountry;
				pCustomer->m_sGender=ip.m_sGender;
				pCustomer->m_sBirthdate=ip.m_sBirthdate;
				pCustomer->m_sComments=ip.m_sComments;
				pCustomer->m_sEstablishmentCustomerId=ip.m_sEstablishmentCustomerId;
				pCustomer->m_cAppBar=IMAGE_TYPE_NEW;	/** 0=not detected anymore, 1=new app bar, 2=active, 3=recent */
				pCustomer->m_iRecordVersion=ip.m_iRecordVersion;

				TRACE0((LPCTSTR) "\n");TRACE0((LPCTSTR) "\n");
				TRACE0((LPCTSTR) pCustomer->m_sNickname.c_str());TRACE0((LPCTSTR) "\n");
				TRACE0((LPCTSTR) pCustomer->m_sFirstName.c_str());TRACE0((LPCTSTR) "\n");
				TRACE0((LPCTSTR) pCustomer->m_sLastName.c_str());TRACE0((LPCTSTR) "\n");
				TRACE0((LPCTSTR) pCustomer->m_sAddress.c_str());TRACE0((LPCTSTR) "\n");
				TRACE0((LPCTSTR) pCustomer->m_sCity.c_str());TRACE0((LPCTSTR) "\n");
				TRACE0((LPCTSTR) pCustomer->m_sState.c_str());TRACE0((LPCTSTR) "\n");
				TRACE0((LPCTSTR) pCustomer->m_sZip.c_str());TRACE0((LPCTSTR) "\n");
				TRACE0((LPCTSTR) "\n");
				
				MYSTL_ITERATE_LIST(ip.m_listPhoneNumbers,VIPPhoneNumber,pn,itPN)
				{
					pCustomer->m_listPhoneNumbers.push_back(pn);
				}
				MYSTL_ITERATE_LIST(ip.m_listAttributes,MiscVIPAttribute,pa,itPA)
				{
					pCustomer->m_listAttributes.push_back(pa);
				}
				
				
				/** @todo ask
				 // A. Riazi, save this customer to the database

				 //check if the customer is exist
				 //if exist, update the record
				 */
				CString szCommand;
				szCommand.Format(_T("SELECT PlutoId.* FROM [PlutoId] WHERE PlutoId.PKID_PlutoId=%lu"), 
					pCustomer->m_iPlutoId);
				
				int bAlreadyInDatabase=0;
				CADOCommand select(&DB, szCommand, CADOCommand::typeCmdText);
				if (record.Execute(&select))
				{
					if (record.GetRecordCount()==0)
						bAlreadyInDatabase = RECORD_DOES_NOT_EXIST;
					else
						bAlreadyInDatabase = RECORD_EXISTS;
					record.Close();
				}
				

				if (!bAlreadyInDatabase)
				{
					/** 
					 * Add new record
					 * if does not exist, add new record to database
					 */
					 
					CString szTable;
					szTable.Format(_T("[PlutoId]"));
					if (record.Open(szTable, CADORecordset::openTable))
					{
						record.AddNew();
						record.SetFieldValue(_T("PKID_PlutoId"), pCustomer->m_iPlutoId);
						record.SetFieldValue(_T("email"), CString(pCustomer->m_sEmail.c_str()));
						record.SetFieldValue(_T("Nickname"), CString(pCustomer->m_sNickname.c_str()));
						record.SetFieldValue(_T("FirstName"), CString(pCustomer->m_sFirstName.c_str()));
						record.SetFieldValue(_T("LastName"), CString(pCustomer->m_sLastName.c_str()));
						record.SetFieldValue(_T("Address"), CString(pCustomer->m_sAddress.c_str()));
						record.SetFieldValue(_T("City"), CString(pCustomer->m_sCity.c_str()));
						record.SetFieldValue(_T("State"), CString(pCustomer->m_sState.c_str()));
						record.SetFieldValue(_T("Zip"), CString(pCustomer->m_sZip.c_str()));
						record.SetFieldValue(_T("Country"), CString(pCustomer->m_sCountry.c_str()));
						CString TmpGender=pCustomer->m_sGender.c_str();
						int gender;
						if (TmpGender.CompareNoCase(_T("m"))==0)
							gender=0;		/** male */
						else
							gender=1;		/** female */
						record.SetFieldValue(_T("Gender"), gender);
						record.SetFieldValue(_T("Birthdate"), CString(pCustomer->m_sBirthdate.c_str()));
						/** @test record.SetFieldValue(_T("Comments"), CString(pCustomer->m_sComments.c_str())); */
						record.SetFieldValue(_T("EstablishmentId"), CString(pCustomer->m_sEstablishmentCustomerId.c_str()));
						record.SetFieldValue(_T("RecordVersion"), pCustomer->m_iRecordVersion);

						pCustomer->m_iTotalVisit=0;
						pCustomer->m_iAvailableVisit=0;
						pCustomer->m_iTotalPurchase=0;
						pCustomer->m_iAvailablePurchase=0;
						record.SetFieldValue(_T("TotalVisits"), 0);
						record.SetFieldValue(_T("AvailableVisits"), 0);
						record.SetFieldValue(_T("TotalPurchase"), 0);
						record.SetFieldValue(_T("AvailablePurchase"), 0);

						record.Update();
						record.Close();
					}
				}
				else
				{
					/** update record */
					CString szTable;
					szTable.Format(_T("SELECT PlutoId.* FROM [PlutoId] WHERE PlutoId.PKID_PlutoId=%d"),
						pCustomer->m_iPlutoId);
					
					if (record.Open(szTable, CADORecordset::openQuery))
					{
						if (record.GetRecordCount())
						{
							record.Edit();
							record.SetFieldValue(_T("PKID_PlutoId"), pCustomer->m_iPlutoId);
							record.SetFieldValue(_T("email"), CString(pCustomer->m_sEmail.c_str()));
							record.SetFieldValue(_T("Nickname"), CString(pCustomer->m_sNickname.c_str()));
							record.SetFieldValue(_T("FirstName"), CString(pCustomer->m_sFirstName.c_str()));
							record.SetFieldValue(_T("LastName"), CString(pCustomer->m_sLastName.c_str()));
							record.SetFieldValue(_T("Address"), CString(pCustomer->m_sAddress.c_str()));
							record.SetFieldValue(_T("City"), CString(pCustomer->m_sCity.c_str()));
							record.SetFieldValue(_T("State"), CString(pCustomer->m_sState.c_str()));
							record.SetFieldValue(_T("Zip"), CString(pCustomer->m_sZip.c_str()));
							record.SetFieldValue(_T("Country"), CString(pCustomer->m_sCountry.c_str()));
							CString TmpGender=pCustomer->m_sGender.c_str();
							int gender;
							if (TmpGender.CompareNoCase(_T("m"))==0)
								gender=0;		/** male */
							else
								gender=1;		/** female */
							record.SetFieldValue(_T("Gender"), gender);
							record.SetFieldValue(_T("Birthdate"), CString(pCustomer->m_sBirthdate.c_str()));
							/** @test 
							 * record.SetFieldValue(_T("Comments"), CString(pCustomer->m_sComments.c_str())); 
							 */
							record.SetFieldValue(_T("EstablishmentId"), CString(pCustomer->m_sEstablishmentCustomerId.c_str()));
							record.SetFieldValue(_T("RecordVersion"), pCustomer->m_iRecordVersion);

							/** get total/available visit/purchase from customer information */
							record.GetFieldValue(_T("TotalVisits"), pCustomer->m_iTotalVisit);
							record.GetFieldValue(_T("AvailableVisits"), pCustomer->m_iAvailableVisit);
							record.GetFieldValue(_T("TotalPurchase"), pCustomer->m_iTotalPurchase);
							record.GetFieldValue(_T("AvailablePurchase"), pCustomer->m_iAvailablePurchase);

							record.Update();
							record.Close();
						}
					}
				}

				CString szMacAddrTable;
				szMacAddrTable.Format(_T("SELECT MacAddress.* FROM [MacAddress] WHERE MacAddress.PKID_MacAddress=\"%s\""), 
					MACAddress.GetBuffer(0));

				if (record.Open(szMacAddrTable, CADORecordset::openQuery))
				{
					if (record.GetRecordCount())
						record.Delete();

					record.Close();

					CString szTable=_T("[MacAddress]");
					if (record.Open(szTable, CADORecordset::openTable))
					{
						record.AddNew();
						record.SetFieldValue(_T("PKID_MacAddress"), MACAddress);
						record.SetFieldValue(_T("FKID_PlutoId"), pCustomer->m_iPlutoId);
						record.Update();
						record.Close();
					}
				}

				/** save image file shared directory */

				FileName=SaveCustomerImage(&ip);
			}

			/** @todo ask  A. Riazi, fill these in */
			time_t Now;
			pCustomer->m_bDetected=true;
			pCustomer->m_tLastVisit=time(&Now);		/** today */

			CString Today;
			CTime tToday=CTime::GetCurrentTime();
			Today=tToday.FormatGmt(_T("%c"));

			CString szTable=_T("[Visit]");
			if (record.Open(szTable, CADORecordset::openTable))
			{
				record.AddNew();
				record.SetFieldValue(_T("FKID_PlutoId"), pCustomer->m_iPlutoId);
				record.SetFieldValue(_T("VisitDateTime"), Today);
				record.Update();
				record.Close();
			}

			/** @test
			if (FileName.IsEmpty())
				pCustomer->m_pPicture=NULL;
			else
				pCustomer->m_pPicture->Load(FileName);//Riazi -> =NULL; // ARMEN: How should we store the picture?  We're getting a pointer to a JPG file in ip->m_pImage;
			*/
			AddCustomer(pCustomer);
			
			/** Add customer image to global image list */
			AddCustomerImageToImageList(pCustomer->m_iPlutoId, FileName);
			
			SetupAppBars();
			
			/** @todo ask ARMEN: Invalidate() */
		}
		else if(ip.m_cProcessOutcome==RECORD_NOT_FOUND)
		{
			delete pCustomer;
			AfxBeginThread(FireErrorRecord, &MACAddress);
		}
		else
		{
			delete pCustomer;
		}

		/** close the database */
		DB.Close();
	}
}

void PhoneDetection_VIP::LostDevice(class PhoneDevice *pDevice)
{
}

void PhoneDetection_VIP::SignalStrengthChanged(class PhoneDevice *pDevice)
{
}


void PhoneDetection_VIP::AddCustomer(class Customer *pCustomer)
{
	PLUTO_SAFETY_LOCK(tm,*m_pThreadMutex);

	map<unsigned long,class Customer *>::iterator it = m_mapCustomers.find(pCustomer->m_iPlutoId);
	if( it!=m_mapCustomers.end() )
	{
		delete (*it).second;
		m_mapCustomers.erase(it);
	}

	m_mapCustomers[pCustomer->m_iPlutoId]=pCustomer;
}

void PhoneDetection_VIP::SetAppBars(CAppBar *pNewAB,
									CAppBar *pActiveAB,
									CAppBar *pRecentAB)
{
	m_pNewAB=pNewAB;
	((CControlsDlg*) m_pNewAB)->SetPhoneDetection(this);

	m_pActiveAB=pActiveAB;
	((CControlsDlg*) m_pNewAB)->SetPhoneDetection(this);
		
	m_pRecentAB=pRecentAB;
	((CControlsDlg*) m_pNewAB)->SetPhoneDetection(this);
}

void PhoneDetection_VIP::SetupAppBars()
{
	/**  Protect the map so that the map doesn't get changed in between set ups */
	PLUTO_SAFETY_LOCK(tm,*m_pThreadMutex);

	
	if( m_pNewAB )
		((CControlsDlg*) m_pNewAB)->SetupButtons();

	if( m_pActiveAB )
		((CControlsDlg*) m_pActiveAB)->SetupButtons();

	if( m_pRecentAB )
		((CControlsDlg*) m_pRecentAB)->SetupButtons();
	
}

CString PhoneDetection_VIP::SaveCustomerImage(VR_IdentifyPhone* pIP)
{
	PlutoCustomer* pPlutoCustomer=new PlutoCustomer;
	pPlutoCustomer->Time=CTime::GetCurrentTime();	/** current time */
	
	/** if no image transfered! */
	if (pIP->m_pdbImage.m_dwSize<=0)
	{
		pPlutoCustomer->ImageNumber=-1;			/** the customer has no image */
		pPlutoCustomer->ImageType=0;			/** the customer has no image */
		pPlutoCustomer->DefaultBitmap=TRUE;

		mapPlutoImageList[pIP->m_iPlutoId]=pPlutoCustomer;
		return CString(_T(""));
	}

	CRegistry registry;
	registry.OpenKey(HKEY_CURRENT_USER, REGISTRY_KEY_VIPSERVICE_FULL);
	DWORD dwTerminals=registry.GetValueDWORD(REGISTRY_PAGE_TERMINALS_TERMINALS);
	CString Folder;
	if (dwTerminals)	/** True -> we used shared terminals */
		Folder=registry.GetValueString(REGISTRY_PAGE_TERMINALS_DIRECTORY);
	else
		Folder=THUMBNAIL_IMAGE_DIRECTORY;
	registry.CloseKey();
	
	CFile ImageFile;
	CString PlutoImage;
	
	switch (pIP->m_iImageType)
	{
		case GRAPHICTYPE_JPG:
			PlutoImage.Format(_T("%s\\%d.jpg"), 
				Folder.GetBuffer(0), pIP->m_iPlutoId);
			pPlutoCustomer->ImageType=GRAPHICTYPE_JPG;

		break;
		
		case GRAPHICTYPE_PNG:
			PlutoImage.Format(_T("%s\\%d.png"), 
				Folder.GetBuffer(0), pIP->m_iPlutoId);
			pPlutoCustomer->ImageType=GRAPHICTYPE_PNG;
		break;
		
		case GRAPHICTYPE_MNG:
			PlutoImage.Format(_T("%s\\%d.mng"), 
				Folder.GetBuffer(0), pIP->m_iPlutoId);
			pPlutoCustomer->ImageType=GRAPHICTYPE_MNG;
		break;
		
		case GRAPHICTYPE_AGIF:
			PlutoImage.Format(_T("%s\\%d.gif"), 
				Folder.GetBuffer(0), pIP->m_iPlutoId);
			pPlutoCustomer->ImageType=GRAPHICTYPE_AGIF;
		break;
	}
	
	
	if( ImageFile.Open(PlutoImage, CFile::modeWrite | CFile::modeCreate) )
	{
		ImageFile.Write(pIP->m_pdbImage.m_pBlock, pIP->m_pdbImage.m_dwSize);
		ImageFile.Close();
	}

	pPlutoCustomer->DefaultBitmap=FALSE;
	mapPlutoImageList[pIP->m_iPlutoId]=pPlutoCustomer;

	return PlutoImage;
}

void PhoneDetection_VIP::AddCustomerImageToImageList(unsigned long Id, CString FileName)
{
	CBitmap*    pImage = NULL;	
	HBITMAP     hBitmap = NULL;
	HPALETTE    hPal = NULL;
	HDC			hMemDC = NULL;	
	HGDIOBJ		hOldObj = NULL;
	
	CDib		dib;
	//int			nWidth, nHeight; 

	/** no images */

	if (FileName.IsEmpty())
	{
		/** if the customer has not thumbnail we use the default bitmap: default.bmp */
		CString FileNameTemp=FindCustomerImageFileFromID(Id, GRAPHICTYPE_BMP);

		CopyFile(_T("Images\\Default.bmp"), FileNameTemp, FALSE);

		mapPlutoImageList[Id]->DefaultBitmap=TRUE;
		/** @test
		FILE* read=_tfopen(_T("default.bmp"), _T("rb"));
		FILE* write=_tfopen(FileNameTemp, _T("wb"));

		if ((read!=NULL) && (write!=NULL))
		{
			BYTE pData[1000];
			int ReadData=0;
			while (!feof(read))
			{
				ReadData=fread(pData, sizeof(pData), 1, read);
				fwrite(pData, ReadData, 1, write);
			}

			fclose(read);
			fclose(write);
		}

		*/
		/** @test
		//load a default bitmap for customer
		// attach the thumbnail bitmap handle to an CBitmap object
		pImage = new CBitmap();		 
		pImage->LoadBitmap(IDB_DUMMY_BITMAP);
		
		// add bitmap to our image list
		int ImageNumber=PlutoVIPImageList.Add(pImage, (CBitmap*) NULL);
	
		//save image number in mapPlutoImageList
		mapPlutoImageList[Id]->ImageNumber=ImageNumber;

		delete pImage;
		*/
		return;
	}
	
	if(NULL == mapPlutoImageList[Id])
	{
		PlutoCustomer *pPlutoCustomer = new PlutoCustomer;
		pPlutoCustomer->DefaultBitmap = FALSE;

		mapPlutoImageList[Id] = pPlutoCustomer;
	}

	CxImage image;
	CString ImageFile;

	/** @test
	map<unsigned long, PlutoCustomer*>::iterator it;
	for(it=mapPlutoImageList.begin();it!=mapPlutoImageList.end();++it)
	{
		long cid = (*it).first;
		PlutoCustomer *pC = (*it).second;
		int k=2;
	}
	*/

	bool bLoadedImage=false;
	switch (mapPlutoImageList[Id]->ImageType)
	{
		case GRAPHICTYPE_JPG:
			ImageFile=FindCustomerImageFileFromID(Id, GRAPHICTYPE_JPG);
			bLoadedImage=image.Load(ImageFile, CXIMAGE_FORMAT_JPG);
			break;
		
		case GRAPHICTYPE_PNG:
			ImageFile=FindCustomerImageFileFromID(Id, GRAPHICTYPE_PNG);
			bLoadedImage=image.Load(ImageFile, CXIMAGE_FORMAT_PNG);
			break;
		
		case GRAPHICTYPE_AGIF:
			ImageFile=FindCustomerImageFileFromID(Id, GRAPHICTYPE_AGIF);
			bLoadedImage=image.Load(ImageFile, CXIMAGE_FORMAT_GIF);
			break;
		
		case GRAPHICTYPE_MNG:
			/** @test
			 * ImageFile=FindCustomerImageFileFromID(Id, GRAPHICTYPE_MNG);
			 * image.Load(ImageFile, CXIMAGE_FORMAT_MNG);
			*/
			break;
	}

	if( !bLoadedImage )
	{
		/** @test delete pImage; */
		return;
	}

	/** draw the thumbnails; borrow our dib header to create our thumbnail bitmap */
	CString strPath=FindCustomerImageFileFromID(Id, GRAPHICTYPE_BMP);
	image.Save(strPath, CXIMAGE_FORMAT_BMP);

	/** @test
	CFile ImgFile;
		
	if( !ImgFile.Open(strPath, CFile::modeRead) )
	{
		//delete pImage; we have not allocated memory for pImage
		mapPlutoImageList[Id]->ImageNumber=-1;
		return;
	}

	if( dib.Read(ImgFile)==0 )
	{
		ImgFile.Close();
		//delete pImage; we have not allocated memory for pImage
		mapPlutoImageList[Id]->ImageNumber=-1;
		return;
	}
	ImgFile.Close();

	nWidth = dib.m_pBMI->bmiHeader.biWidth;
	nHeight = dib.m_pBMI->bmiHeader.biHeight;
	dib.m_pBMI->bmiHeader.biWidth = THUMBNAIL_WIDTH;
	dib.m_pBMI->bmiHeader.biHeight = THUMBNAIL_HEIGHT;
		
	// create thumbnail bitmap section
	hBitmap = ::CreateDIBSection(NULL, 
								 dib.m_pBMI, 
							     DIB_RGB_COLORS, 
							     NULL, 
							     NULL, 
							     0); 

	// restore dib header
	dib.m_pBMI->bmiHeader.biWidth = nWidth;
	dib.m_pBMI->bmiHeader.biHeight = nHeight;

	// select thumbnail bitmap into screen dc
	hMemDC = ::CreateCompatibleDC(NULL);	
	hOldObj = ::SelectObject(hMemDC, hBitmap);
	  
	// grayscale image, need palette
	if(dib.m_pPalette != NULL)
	{
		hPal = ::SelectPalette(hMemDC, (HPALETTE)dib.m_pPalette->GetSafeHandle(), FALSE);
		::RealizePalette(hMemDC);
	}
	  
	// set stretch mode
	::SetStretchBltMode(hMemDC, COLORONCOLOR);

	// populate the thumbnail bitmap bits
	::StretchDIBits(hMemDC, 0, 0, 
					THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, 
					0, 0, 
					dib.m_pBMI->bmiHeader.biWidth, 
					dib.m_pBMI->bmiHeader.biHeight, 
					dib.m_pBits, 
					dib.m_pBMI, 
					DIB_RGB_COLORS, 
					SRCCOPY);

	// restore DC object
	::SelectObject(hMemDC, hOldObj);
	  
	// restore DC palette
	if(dib.m_pPalette != NULL)
		::SelectPalette(hMemDC, (HPALETTE)hPal, FALSE);
	  
	// clean up
	::DeleteObject(hMemDC);
		
	// attach the thumbnail bitmap handle to an CBitmap object
	pImage = new CBitmap();		 
	pImage->Attach(hBitmap);

	// add bitmap to our image list
	int ImageNumber=PlutoVIPImageList.Add(pImage, (CBitmap*) NULL);
	
	//save image number in mapPlutoImageList
	mapPlutoImageList[Id]->ImageNumber=ImageNumber;

	delete pImage;
*/
}

CString FindCustomerImageFileFromID(ULONG ID, int ImageType)
{
	CRegistry registry;
	registry.OpenKey(HKEY_CURRENT_USER, REGISTRY_KEY_VIPSERVICE_FULL);
	DWORD dwTerminals;
	CString dir;
	dwTerminals=registry.GetValueDWORD(REGISTRY_PAGE_TERMINALS_TERMINALS);

	if (dwTerminals)	/** if true -> we used multiple terminals */
		dir=registry.GetValueString(REGISTRY_PAGE_TERMINALS_DIRECTORY);	
	else				/** image directory of plutovip */
		dir=THUMBNAIL_IMAGE_DIRECTORY;
	
	registry.CloseKey();

	CString File=_T("");
	switch (ImageType)
	{
		case GRAPHICTYPE_JPG:
			File.Format(_T("%s\\%d.jpg"), dir.GetBuffer(0), ID);
			break;

		case GRAPHICTYPE_AGIF:
			File.Format(_T("%s\\%d.gif"), dir.GetBuffer(0), ID);
			break;

		case GRAPHICTYPE_MNG:
			File.Format(_T("%s\\%d.mng"), dir.GetBuffer(0), ID);
			break;

		case GRAPHICTYPE_PNG:
			File.Format(_T("%s\\%d.png"), dir.GetBuffer(0), ID);
			break;

		case GRAPHICTYPE_BMP:
			File.Format(_T("%s\\%d.bmp"), dir.GetBuffer(0), ID);
			break;
	}

	return File;
}

UINT FireErrorRecord(LPVOID pParam)
{
	CRecordError Rerror;
	Rerror.szMACAddress=((CString*) pParam)->GetBuffer(0);
	Rerror.DoModal();	

	return 0;
}
