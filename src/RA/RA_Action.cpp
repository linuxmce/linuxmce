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
 * @file RA_Action.cpp
 * @brief source file for the request processor class
 * @author
 * @todo notcommented
 *
 */

 /**
  *
  * Copyright Notice goes here
  *
  */



//#include "VIPShared/VIPIncludes.h"

#include "RA_Action.h"


RA_Action::RA_Action()
{
    // common-sense initial values
    m_dwActionSize = m_dwActionChecksum = 0;
    m_pcAction = NULL;
}


void RA_Action::CreateAction(uint32_t dwSize, const char *pcData)
{
	SerializeRead(dwSize,(char *) pcData);
}

void RA_Action::ConvertActionToBinary()
{
	SerializeWrite();
    m_dwActionSize = (uint32_t) ( m_pcCurrentPosition - m_pcDataBlock );
    m_pcAction = m_pcDataBlock;
}
