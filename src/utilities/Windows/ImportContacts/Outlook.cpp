#include "StdAfx.h"
#include "Outlook.h"
#include "Log.h"

OutlookWraper::~OutlookWraper(void)
{
	m_pContact = NULL ;
	m_pItems = NULL;
	m_pFolder = NULL;
	m_pMAPI = NULL;
	//pApp = NULL;
}

int OutlookWraper::readContacts(ContactsList &list)
{
	_ApplicationPtr pApp( "Outlook.Application" );

	Contact *pContactInfo = NULL;
	int nCount = 0;

	try
	{
		m_pMAPI = pApp->GetNamespace("MAPI");
		Log::m_pLog->writeLine( "Outlook init after GetNamespace");
		m_pMAPI->Logon("","",false,false);
		Log::m_pLog->writeLine( "Outlook init after Logon");
		m_pFolder = m_pMAPI->GetDefaultFolder(olFolderContacts);
		Log::m_pLog->writeLine( "Outlook init after getFolder");
		m_pItems = m_pFolder->GetItems();
		Log::m_pLog->writeLine( "Outlook init after getItems");
		m_pContact = m_pItems->GetFirst();
		Log::m_pLog->writeLine( "Outlook init after getFirst" );
		nCount = m_pItems->GetCount();

		Log::m_pLog->writeLine( MAKE_STRING( string("Number of contact:") << nCount ) );
		for(int i=0;i<nCount;i++)
		{

			pContactInfo = new Contact;

			if( (LPCSTR) m_pContact->GetFirstName() != NULL )
				pContactInfo->firstName = (LPCSTR) m_pContact->GetFirstName();
			if( (LPCSTR) m_pContact->GetMiddleName() != NULL )
				pContactInfo->middleName = (LPCSTR) m_pContact->GetMiddleName();
			if( (LPCSTR)m_pContact->GetLastName() != NULL )
				pContactInfo->lastName = (LPCSTR) m_pContact->GetLastName();

			if( (LPCSTR) m_pContact->GetCompanyName() != NULL )
				pContactInfo->companyName = (LPCSTR) m_pContact->GetCompanyName();
			if( (LPCSTR) m_pContact->GetJobTitle() != NULL)
				pContactInfo->title = (LPCSTR) m_pContact->GetJobTitle();

			if( (LPCSTR) m_pContact->GetHomeTelephoneNumber() != NULL )
				pContactInfo->phoneHome = (LPCSTR) m_pContact->GetHomeTelephoneNumber();
			if( (LPCSTR) m_pContact->GetBusinessTelephoneNumber() != NULL )
				pContactInfo->homeBusiness = (LPCSTR) m_pContact->GetBusinessTelephoneNumber();
			if( (LPCSTR) m_pContact->GetMobileTelephoneNumber() != NULL )
				pContactInfo->phoneMobile = (LPCSTR) m_pContact->GetMobileTelephoneNumber();
			if( (LPCSTR) m_pContact->GetBusinessFaxNumber() != NULL )
				pContactInfo->phoneFax = (LPCSTR) m_pContact->GetBusinessFaxNumber();

			if( (LPCSTR) m_pContact->GetHomeAddress() != NULL )
				pContactInfo->homeAddress = (LPCSTR) m_pContact->GetHomeAddress();
			if( (LPCSTR) m_pContact->GetBusinessAddress() != NULL )
				pContactInfo->businessAddress = (LPCSTR) m_pContact->GetBusinessAddress();

			if( (LPCSTR) m_pContact->GetEmail1Address() != NULL )
				pContactInfo->email = (LPCSTR) m_pContact->GetEmail1Address();

			Log::m_pLog->m_file << *pContactInfo;

			list.addContact( pContactInfo );
			m_pContact = m_pItems->GetNext();
		}
	}
	catch(_com_error& ce)
    {
		errDesc = (char *) ce.Description();
		Log::m_pLog->writeLine( "Outlook catch");
		return list.size();
	}

	return list.size();
}