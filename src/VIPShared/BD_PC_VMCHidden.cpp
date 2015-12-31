/*
 BD_PC_VMCHidden
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#ifndef SYMBIAN
#include "PlutoUtils/CommonIncludes.h"
#include "BD/BDCommandProcessor.h"
#endif

#include "PlutoUtils/MyStl.h"
#include "BD_PC_VMCHidden.h"

BD_PC_VMCHidden::BD_PC_VMCHidden(unsigned long MenuCollectionID) 
	
{
	m_iMenuCollectionID=MenuCollectionID;
}

void BD_PC_VMCHidden::ConvertCommandToBinary()
{
	BDCommand::ConvertCommandToBinary();
	Write_unsigned_long(m_iMenuCollectionID);
}

void BD_PC_VMCHidden::ParseCommand(unsigned long size,const char *data)
{
	BDCommand::ParseCommand(size,data);
	m_iMenuCollectionID = Read_unsigned_long();
}

bool BD_PC_VMCHidden::ProcessCommand(BDCommandProcessor *pProcessor)
{
	return true;
}

