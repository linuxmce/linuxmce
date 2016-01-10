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
#if !defined(__BD_CP_SENDFILE_H__)
#define __BD_CP_SENDFILE_H__

#include "BD/BDCommand.h"
#include "VIPShared/PlutoPhoneCommands.h"

class BD_CP_SendFile : public BDCommand
{
	char *m_pFileName;
	char *m_pFileData;

	unsigned long m_iFileNameSize;
	unsigned long m_iFileDataSize;

public:
	BD_CP_SendFile()  
	{
		m_pFileName = NULL;	
		m_pFileData = NULL;

		m_iFileNameSize = 0; 
		m_iFileDataSize = 0;
	}

	BD_CP_SendFile(
		char *pFileName, 
		char *pFileData, 
		unsigned long iFileNameSize,
		unsigned long iFileDataSize
	);

	~BD_CP_SendFile();

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size,const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_CP_SEND_FILE; }

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);

	virtual const char *Description() { return "Send file"; };
};

#endif //__BD_CP_SENDFILE_H__