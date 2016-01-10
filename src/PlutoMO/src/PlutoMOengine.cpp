/*
 PlutoMOengine
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#include <avkon.hrh>
#include <aknnotewrappers.h> 
#include <aknutils.h>  // for Fonts. 

//#include "bitmapmethods.h"
// deprecated in s603rd
// #include "palbimageviewerbasic.h"

#include "PlutoUtils/MyString.h"
#include "PlutoMOengine.h"
#include "PlutoMO.hrh"     // enumerations

#include "PlutoMOAppUi.h"

//------------------------------------------------------------------------------------------------------------------
// Number, name and file size
_LIT(KStringSize,"%d\t%S\t%d bytes");
// Number, name and date modified
_LIT(KStringDate,"%d\t%S\t%S"); 
//------------------------------------------------------------------------------------------------------------------
void CPlutoMOEngine::ConstructL()
    {
    }
//------------------------------------------------------------------------------------------------------------------
CPlutoMOEngine::~CPlutoMOEngine()
    {
    delete iDirList;
    }
//------------------------------------------------------------------------------------------------------------------
TInt CPlutoMOEngine::StartPlutoMO()
{
	CPlutoMOAppUi* pApp = (CPlutoMOAppUi*)(CCoeEnv::Static()->AppUi());

	if (iDirList)
	{        
		delete iDirList;
		iDirList = 0;
	}    
    
	TInt error = KErrNone;
	User::LeaveIfError(iFsSession.Connect()); 

	switch (iDirectory)
	{
		case EPlutoMOPictures:
			error = iFsSession.GetDir(pApp->m_sVMCFolderFilter.Des(),KEntryAttNormal,ESortByName,iDirList);
			break;
		 default:
			break;
	}

	return error;
}
//------------------------------------------------------------------------------------------------------------------
void CPlutoMOEngine::GetPlutoMOItems(CDesCArray* aItems)
{
	if(!iDirList)
		return;

	CPlutoMOAppUi* pApp = (CPlutoMOAppUi*)(CCoeEnv::Static()->AppUi());
                   
	for (TInt i=0;i<iDirList->Count();i++)
    {
		TFileName filename = NULL;
		if(iSizeDate==EPlutoMOSize)
		{
			filename.Format(KStringSize,i+1,&(*iDirList)[i].iName,(*iDirList)[i].iSize);
		}
		else
		{
			TBuf<30> dateString; 
			_LIT(KDateString,"%D%M%Y%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%:2%S%:3%+B");
			(*iDirList)[i].iModified.FormatL(dateString,KDateString); 
			filename.Format(KStringDate,i+1,&(*iDirList)[i].iName,&dateString);
		}

		char* pFilename = new char[256];
		int index;
		for(index = 0; index < filename.Length(); index++)
			pFilename[index] = filename[index];
		pFilename[index] = '\0';
		string sFileName(pFilename);

		if(-1 != sFileName.find("0000.vmc")) 
		{
			char pCurrConnection[] = "1_Orbiter_current connection";
			pCurrConnection[1] = 9;
			pCurrConnection[9] = 9;
			string sCurrConnection(pCurrConnection);
			aItems->AppendL(sCurrConnection.Des());
		}
		else
			aItems->AppendL(filename);

		delete []pFilename;
    }
    
}
//------------------------------------------------------------------------------------------------------------------
void CPlutoMOEngine::SetDirectory(TInt aDirectory)
    {
    if (aDirectory!=EPlutoMODirNoChange)
        iDirectory=aDirectory;
    }
//------------------------------------------------------------------------------------------------------------------
void CPlutoMOEngine::LaunchCurrent(TInt aPosition)
{
    if(!iDirList)
        return;

	CPlutoMOAppUi* pApp = (CPlutoMOAppUi*)(CCoeEnv::Static()->AppUi());

    CDocumentHandler* handler = CDocumentHandler::NewL(NULL);
    CleanupStack::PushL(handler);
    TFileName descr;

    descr.Append(pApp->m_sAppFolder.Des());
	descr.Append(_L("\\"));
    descr.Append((*iDirList)[aPosition].iName);
    TDataType nullType;

	TParse p;
	p.Set(descr, NULL, NULL);

	if(p.Ext() != ".vmc" && p.Ext() != ".VMC")
		handler->OpenFileL(descr, nullType);
	else
	{
		char* pFilename = new char[256];
		int index;
		for(index = 0; index < descr.Length(); index++)
			pFilename[index] = descr[index];
		pFilename[index] = '\0';
		string sFileName(pFilename);

		if(-1 == sFileName.find("0000.vmc")) 
			((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->OpenVMC(false, descr, NULL);
		else
			((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->MakeViewerVisible(true);

		delete []pFilename;
	}

    CleanupStack::PopAndDestroy(); // handler
    };
//------------------------------------------------------------------------------------------------------------------
// Remove Files
void CPlutoMOEngine::RemoveFile(TInt aPosition)
{
    if(!iDirList)
        return;

	CPlutoMOAppUi* pApp = (CPlutoMOAppUi*)(CCoeEnv::Static()->AppUi());

    TFileName descr;
    descr.Append(pApp->m_sAppFolder.Des());

    // Add filename to be deleted
    descr.Append((*iDirList)[aPosition].iName);
	User::LeaveIfError(iFsSession.Connect()); 

     //Remove current item in the list
	User::LeaveIfError(iFsSession.Delete(descr));
	iFsSession.Close();
    
	// Display an information note
	informationNote = new (ELeave) CAknInformationNote;
	informationNote->ExecuteLD(_L("File Deleted"));
}
//------------------------------------------------------------------------------------------------------------------
// Find number of files in current folder
void CPlutoMOEngine::FindFiles()
{
	if(!iDirList)
        return;
	
	_LIT(KString, "Number of files: %d");

	TBuf< KLengthOfStringToShowIndex > number;
	TBuf< KLengthOfStringToShowIndex > format;
	
	format.Append(KString);	

	iCount = iDirList->Count();

	number.Format( format, iCount );

	informationNote = new (ELeave) CAknInformationNote;
    
	informationNote->ExecuteLD(number);	
};
//------------------------------------------------------------------------------------------------------------------
TBool CPlutoMOEngine::RemoveItems(CDesCArray* aItems)
    {
    if(iDirList)
        {        
        if (iDirList->Count())
            {
            aItems->Delete(0,(iDirList->Count()));
            return ETrue;
            }
        }    
    return EFalse;
    };
//------------------------------------------------------------------------------------------------------------------
void CPlutoMOEngine::SetSizeDate(TInt aSizeDate)
    {
    if (aSizeDate==EPlutoMOToggle)
        if (iSizeDate==EPlutoMOSize)
            iSizeDate=EPlutoMODate;
        else
            iSizeDate=EPlutoMOSize;
    else
        if (aSizeDate!=EPlutoMOSizeDateNoChange)
            iSizeDate=aSizeDate;
    };
//------------------------------------------------------------------------------------------------------------------
void CPlutoMOEngine::EndPlutoMO()
    {
    iFsSession.Close();
    };
//------------------------------------------------------------------------------------------------------------------
