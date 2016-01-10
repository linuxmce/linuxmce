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
// LineReader.cpp
//
// Copyright (c) 2002 Symbian Ltd.  All rights reserved.
//

#include "LineReader.h"

void CAsciiLineReader::OpenL(TFileName aFileName)
	{
  	User::LeaveIfError(iSession.Connect());
	User::LeaveIfError(iFile.Open(iSession,aFileName,EFileShareAny|EFileRead));
	iEof=EFalse;
	iFilePos=0;
	}

void CAsciiLineReader::Close()
	{
	iFile.Close();
	iSession.Close();
	}

TInt CAsciiLineReader::ReadL(TPtrC8& aPtr, TInt& aFilePos)
	{
	TInt lfPos=-1;
	
	while (lfPos<0)
		{
		lfPos=iPtr.LocateF(10);

		if (lfPos<0)
			{	//no line feed was found
			if (iEof)
				{
				aPtr.Set(iPtr);	//last line	had no LF
				aFilePos=iFilePos;
				return EFalse;
				}

			if (iPtr.Length())
				{// in middle of line, so rewind and read from the beginning
				TInt moveBack=-iPtr.Length();
				User::LeaveIfError(iFile.Seek(ESeekCurrent, moveBack));
				}

			TRequestStatus status;
			iFile.Read(iLineBuffer, status);
			User::WaitForRequest(status);
			iPtr.Set(iLineBuffer);
			iEof=(iLineBuffer.Length()<256);
			}
		}
		
	//found line feed
	TInt length=lfPos;
	if ((lfPos>0) && (iPtr[lfPos-1]==13))
		length--;
	aPtr.Set(iPtr.Left(length));

	//move past the line feed
	iPtr.Set(iPtr.Mid(++lfPos));
	aFilePos=iFilePos;
	iFilePos+=lfPos;
	
	if ((iPtr.Length()==0) && (iEof))
		{
		iFile.Size(aFilePos);
		return EFalse;	//last line had a LF
		}
	return ETrue;
	}
