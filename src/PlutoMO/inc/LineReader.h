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
#ifndef __LINEREADER_H
#define __LINEREADER_H

#include <f32file.h>

class CAsciiLineReader : public CBase
	{
public:
	void OpenL(TFileName aFileName);
	TBool ReadL(TPtrC8& aDes, TInt& aFilePos);
	void Close();
private:
	TBuf8<256> iLineBuffer;
	TPtrC8 iPtr;
	TBool iEof;
	RFile iFile;
	RFs iSession;
	TInt iFilePos;
	};

#endif