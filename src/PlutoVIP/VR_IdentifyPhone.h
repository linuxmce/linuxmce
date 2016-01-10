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
#ifndef VR_IdentifyPhone_H
#define VR_IdentifyPhone_H

#include "RA/RA_Request.h"
#include "VIPShared/VIPIncludes.h"
#include "SerializeClass/SerializeClass.h"
#include "PlutoVIPRequests.h"

#define SERIALIZE_DATA_TYPE_LIST_PHONE_NUM	4000
#define SERIALIZE_DATA_TYPE_LIST_VIP_ATTR	4001

class VR_IdentifyPhone : public RA_Request
{
public:
	// Request Variables
	string m_sBluetoothID,m_sIdentifiedPlutoIdPin;
	unsigned long long m_iMacAddress;
	unsigned long m_iCachedRecordVersion,m_iCachedPlutoId,m_iIdentifiedPlutoId;
	unsigned long m_iEstablishmentID;

	// Response Variables
	char m_iUseCache;
	unsigned long m_iRecordVersion,m_iPlutoId;
	char m_iImageType;
	PlutoDataBlock m_pdbImage;
	string m_sEmail,m_sNickname,m_sFirstName,m_sLastName;
	string m_sAddress,m_sCity,m_sState,m_sZip,m_sCountry;
	string m_sGender,m_sBirthdate;
	string m_sComments,m_sEstablishmentCustomerId;
	list<VIPPhoneNumber *> m_listPhoneNumbers;
	list<MiscVIPAttribute *> m_listAttributes;

	// Other
	class Customer *m_pCustomer; // Used if we get back actions to send to the customer

	VR_IdentifyPhone(
		unsigned long EstablishmentID,
		string sBluetooth,
		unsigned long long MacAddress,
		unsigned long CachedRecordVersion,
		unsigned long CachedPlutoId,
		unsigned long IdentifiedPlutoId,
		string IdentiedPlutoIdPin,
		class Customer *pCustomer
	);
	VR_IdentifyPhone() { m_iRecordVersion = 0; }

	virtual void LogVisit(class RA_Processor *pRA_Processor);

	virtual uint32_t ID() { return VRS_IDENFITY_PHONE; }

	virtual void SetupSerialization_Request()
	{
		RA_Request::SetupSerialization_Request();
		StartSerializeList() + m_sBluetoothID + m_sIdentifiedPlutoIdPin 
			+ m_iMacAddress + m_iCachedRecordVersion + m_iCachedPlutoId + m_iIdentifiedPlutoId
			+ m_iEstablishmentID;
	}
	virtual void SetupSerialization_Response()
	{
		RA_Request::SetupSerialization_Response();
		StartSerializeList() + m_iUseCache + m_iRecordVersion + m_iPlutoId
			+ m_iImageType + m_pdbImage
			+ m_sEmail + m_sNickname + m_sFirstName + m_sLastName
			+ m_sAddress + m_sCity + m_sState + m_sZip + m_sCountry
			+ m_sGender + m_sBirthdate
			+ m_sComments + m_sEstablishmentCustomerId;
			
		(*this) + m_listPhoneNumbers + m_listAttributes; // this is serialized custom
	}

	VR_IdentifyPhone &operator+ ( list<VIPPhoneNumber *> &i ) { m_vectItemToSerialize.push_back( new ItemToSerialize( SERIALIZE_DATA_TYPE_LIST_PHONE_NUM, (void *) &i) ); return (*this); }
	VR_IdentifyPhone &operator+ ( list<MiscVIPAttribute *> &i ) { m_vectItemToSerialize.push_back( new ItemToSerialize( SERIALIZE_DATA_TYPE_LIST_VIP_ATTR, (void *) &i) ); return (*this); }
	virtual bool UnknownSerialize( ItemToSerialize *pItem, bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition );
	virtual bool ProcessRequest(class RA_Processor *pRA_Processor);
};


#endif

