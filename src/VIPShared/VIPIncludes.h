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
#ifndef VIPINCLUDES_H
#define VIPINCLUDES_H

#include "PlutoUtils/MyStl.h"
#include "SerializeClass/SerializeClass.h"

#ifndef SYMBIAN
#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/MultiThreadIncludes.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#else // Symbian stuff
#include <eikenv.h>
#include <e32std.h>
//#define string TBuf8<255>
#define u_int64_t TInt64
#define malloc(x) User::Alloc(x)
#define free(x) User::Free(x)
#define realloc(x,y) User::ReAlloc(x,y)
#define memcpy Mem::Copy
#endif

// Used by Customer and VR_IdentifyPhone
class VIPPhoneNumber
{
public:
	//unsigned char m_iPhoneNumberType; //it seems that m_iPhoneNumberType is serialized as an unsigned long (chris 10.11.2004)
	unsigned long m_iPhoneNumberType;
	string m_sNumber;
};

class MiscVIPAttribute
{
public:
	string m_sType,m_sValue;
	MiscVIPAttribute(string Type,string Value) { m_sType=Type; m_sValue=Value; }
};


class BasketOption : public SerializeClass
{
public:
	string m_sID,m_sDescription;
	long m_iCost,m_iQty;

	BasketOption() {};
	BasketOption (string ID,string Description,long Qty, long Cost) { m_sID=ID; m_sDescription=Description; m_iQty=Qty; m_iCost=Cost; }

	// We will need to serialize this manually because it needs to work under Symbian, and I don't think Symbian supports the usual overloaded + operator
	virtual bool Serialize(bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition, void *pExtraSerializationData=NULL);
};

class BasketItem : public SerializeClass
{
public:
	string m_sID,m_sDescription;
	long m_iCost,m_iQty;

	MYSTL_CREATE_LIST(m_listBasketOptions,BasketOption);

	BasketItem() {};
	BasketItem (string ID,string Description,long Qty, long Cost) { m_sID=ID; m_sDescription=Description; m_iQty=Qty; m_iCost=Cost; }

	~BasketItem() 
	{
		MYSTL_ITERATE_LIST(m_listBasketOptions,BasketOption,pBO,it)
		{
			delete pBO;
		}
	}

	// We will need to serialize this manually because it needs to work under Symbian, and I don't think Symbian supports the usual overloaded + operator
	virtual bool Serialize(bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition, void *pExtraSerializationData=NULL);
};

#ifndef SYMBIAN
typedef list<class BasketItem *> ListBasketItem;
#endif

// Phone Status Codes
#define C_PHONESTATUS_NORMAL_CONST		1
#define C_PHONESTATUS_LOST_CONST		2
#define C_PHONESTATUS_RELEASED_CONST	3
#define C_PHONESTATUS_NEW_PHONE_CONST	4
#endif

