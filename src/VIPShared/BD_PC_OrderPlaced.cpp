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
#ifndef SYMBIAN
#include <iostream>
#include <fstream>
#include "PlutoUtils/CommonIncludes.h"
#include "BD/BDCommandProcessor.h"
#endif
#include "VIPShared/VIPIncludes.h"

#include "PlutoUtils/MyStl.h"
#include "BD_PC_OrderPlaced.h"

BD_PC_OrderPlaced::BD_PC_OrderPlaced(unsigned long MenuCollectionID) 
	
{
	m_iMenuCollectionID=MenuCollectionID;
}

void BD_PC_OrderPlaced::ConvertCommandToBinary()
{
#ifndef SMARTPHONE
	BDCommand::ConvertCommandToBinary();
	Write_unsigned_long(m_iMenuCollectionID);
	Write_unsigned_long((unsigned long) MYSTL_SIZEOF_LIST(m_listBasket));

	MYSTL_ITERATE_LIST(m_listBasket,BasketItem,bi,it3)
	{
		Write_string(bi->m_sID);
		Write_string(bi->m_sDescription);
		Write_long(bi->m_iQty);
		Write_long(bi->m_iCost);

		Write_unsigned_long((unsigned long) MYSTL_SIZEOF_LIST(bi->m_listBasketOptions));

		MYSTL_ITERATE_LIST(bi->m_listBasketOptions,BasketOption,bo,it4)
		{
			Write_string(bo->m_sID);
			Write_string(bo->m_sDescription);
			Write_long(bo->m_iQty);
			Write_long(bo->m_iCost);
		}
	}
#endif
}

void BD_PC_OrderPlaced::ParseCommand(unsigned long size,const char *data)
{
#ifndef SMARTPHONE
	BDCommand::ParseCommand(size,data);
	m_iMenuCollectionID = Read_unsigned_long();
	unsigned long NumberOfBasketItems = Read_unsigned_long();
	for(unsigned long iB=0;iB<NumberOfBasketItems;++iB)
	{
		BasketItem *bi = new BasketItem();
		Read_string(bi->m_sID);
		Read_string(bi->m_sDescription);
		bi->m_iQty = Read_long();
		bi->m_iCost = Read_long();
		unsigned long NumberOfBasketOptions = Read_unsigned_long();
		for(unsigned long i2=0;i2<NumberOfBasketOptions;++i2)
		{
			BasketOption *bo = new BasketOption();
			Read_string(bo->m_sID);
			Read_string(bo->m_sDescription);
			bo->m_iQty = Read_long();
			bo->m_iCost = Read_long();
			MYSTL_ADDTO_LIST(bi->m_listBasketOptions,bo);
		}
		MYSTL_ADDTO_LIST(m_listBasket,bi);
	}
#endif
}

bool BD_PC_OrderPlaced::ProcessCommand(BDCommandProcessor *pProcessor)
{
	return true;
}

