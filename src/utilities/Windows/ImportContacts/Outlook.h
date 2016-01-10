#ifndef  OutlookWraper_h
#define  OutlookWraper_h

#include "Contacts.h"

// Define this according to the Outlook Object
// Model version you are compiling under
#define OUTL11// Outlook 2003
//#define OUTL10// Outlook 2002
//#define OUTL9// Outlook 2000

#pragma warning(disable:4146)

#if defined(OUTL11) // Outlook 2003
#import "C:\Program Files\Common Files\Microsoft Shared\OFFICE11\mso.dll" \
no_namespace
#import "C:\Program Files\Microsoft Office\OFFICE11\msoutl.olb" \
rename_namespace("Outlook")
#elif defined(OUTL10) // Outlook 2002
#import "C:\Program Files\Common Files\Microsoft Shared\OFFICE10\mso.dll" \
no_namespace, rename("DocumentProperties", "DocProps")
#import "C:\Program Files\Microsoft Office\OFFICE10\msoutl.olb" \
rename_namespace("Outlook")
#elif defined(OUTL9) // Outlook 2000
#import "C:\Program Files\Microsoft Office\Office\mso9.dll" \
no_namespace, rename("DocumentProperties", "DocProps")
#import "C:\Program Files\Microsoft Office\Office\msoutl9.olb" \
rename_namespace("Outlook")
#endif // OUTL11

#pragma warning(default:4146)

using namespace Outlook;

class OutlookWraper
{
private:
	//Outlook types Object Model
	
	_NameSpacePtr m_pMAPI;

	MAPIFolderPtr m_pFolder;
	_ItemsPtr m_pItems;
	_ContactItemPtr m_pContact;

	string errDesc;
public:
	int readContacts(ContactsList &list);
	~OutlookWraper(void);
};

#endif