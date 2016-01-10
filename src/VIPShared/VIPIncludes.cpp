/*
 VIPIncludes
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#include "VIPIncludes.h"

bool BasketItem::Serialize(bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition, void *pExtraSerializationData)
{
	return true;
}

bool BasketOption::Serialize(bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition, void *pExtraSerializationData)
{
	return true;
}

/*
bool BasketItem::Serialize(bool bWriting,char *&pDataBlock,unsigned long &iAllocatedSize,char *&pCurrentPosition,void *pExtraSerializationData=NULL)
{
	unsigned long NumberOfBasketItems = Read_unsigned_long();

	for(size_t iB=0;iB<NumberOfBasketItems;++iB)

	{

		BasketItem *bi = new BasketItem();

		Read_string(bi->m_sID);

		Read_string(bi->m_sDescription);

		bi->m_iQty = Read_long();

		bi->m_iCost = Read_long();

		unsigned long NumberOfBasketOptions = Read_unsigned_long();

		for(size_t i2=0;i2<NumberOfBasketOptions;++i2)

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


	void VR_ShowMenu::ConvertRequestToBinary()

{

	RA_Request::ConvertRequestToBinary();



	if( !m_pMenuCollection )

	{

		Write_unsigned_long(0);  // no menu or variables

		Write_unsigned_short(0);

	}

	else

	{

		if( !m_pMenuCollection->m_iBinarySize )

			m_pMenuCollection->ConvertToBinary();



		Write_unsigned_long(m_pMenuCollection->m_iBinarySize);

		Write_block(m_pMenuCollection->m_pBinary,m_pMenuCollection->m_iBinarySize);

		
		Write_unsigned_short((unsigned short) MYSTL_SIZEOF_LIST(m_listInputVariables));
		//Write_unsigned_short((unsigned short) m_listInputVariables.size());



		MYSTL_ITERATE_LIST(m_listInputVariables,VIPVariable,pVariable,it2)

		{

			Write_long(pVariable->m_iVariableID);

			Write_string(pVariable->m_sInitialValue);

		}

	}

	

	;
	Write_unsigned_long((unsigned long) MYSTL_SIZEOF_LIST(m_listBasket));
//	Write_unsigned_long((unsigned long) m_listBasket.size());



	MYSTL_ITERATE_LIST(m_listBasket,BasketItem,bi,it3)

	{

		Write_string(bi->m_sID);

		Write_string(bi->m_sDescription);

		Write_long(bi->m_iQty);

		Write_long(bi->m_iCost);



		Write_unsigned_long((unsigned long) MYSTL_SIZEOF_LIST(bi->m_listBasketOptions));
//		Write_unsigned_long((unsigned long) bi->m_listBasketOptions.size());



		MYSTL_ITERATE_LIST(bi->m_listBasketOptions,BasketOption,bo,it4)

		{

			Write_string(bo->m_sID);

			Write_string(bo->m_sDescription);

			Write_long(bo->m_iQty);

			Write_long(bo->m_iCost);

		}

	}



	m_dwRequestSize = (unsigned long) (m_pcCurrentPosition-m_pcDataBlock);

	m_pcRequest = m_pcDataBlock;

}
}
*/
