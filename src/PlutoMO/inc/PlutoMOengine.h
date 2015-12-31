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
#ifndef PLUTOMOENGINE_H
#define PLUTOMOENGINE_H

#include <coecntrl.h>
#include <coemain.h>
#include <coeccntx.h>

#include <f32file.h>        // for CDir
#include <documenthandler.h>// for launching
#include <badesca.h>        // for CDesCArray
#include <apmstd.h>         // for TDataType

#include <eikenv.h>
#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <eikmenup.h>
#include <eikon.hrh>

#include <aknnotewrappers.h>

//------------------------------------------------------------------------------------------------------------------
const TInt KLengthOfStringToShowIndex = 40;
//------------------------------------------------------------------------------------------------------------------
class CPlutoMOEngine : public CCoeControl
{
public: 

    void ConstructL();

    ~CPlutoMOEngine();

public: 

    TInt StartPlutoMO();

    void GetPlutoMOItems(CDesCArray* iItems);

    void SetDirectory(TInt aDirectory);
    void SetSizeDate(TInt aSizeDate);

    void EndPlutoMO();

    TBool RemoveItems(CDesCArray* aItems);

    void LaunchCurrent(TInt aPosition);

	//Remove file
	void RemoveFile(TInt aPosition);

	// Finf Files
	void FindFiles();

private: //Data
    CDir* iDirList;

    // Fileserver session
    RFs iFsSession;

	//Information Note
	CAknInformationNote* informationNote;

private: // variables
    TInt iDirectory;    
    TInt iSizeDate;     
	TInt iCount;

};
//------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------
// End of File
