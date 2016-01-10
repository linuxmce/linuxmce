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
#ifndef VR_RequestPayment_H
#define VR_RequestPayment_H

#include "RA/RA_Request.h"
#include "PlutoVIPRequests.h"

#define SERIALIZE_DATA_TYPE_LIST_INV_DETAIL		4011


class InvoiceDetail : public SerializeClass
{
public:
	string m_sLineID,m_sProductID,m_sDescription;
	long m_iAmount;
	InvoiceDetail() {};
	InvoiceDetail(string LineID,string ProductID,string Description,long Amount) { 
		m_sLineID=LineID; m_sProductID=ProductID; m_sDescription=Description; m_iAmount=Amount; 
	}

	void SetupSerialization(int iSC_Version)
	{
		SerializeClass::SetupSerialization(iSC_Version);
		StartSerializeList() + m_sLineID + m_sProductID + m_sDescription + m_iAmount;
	}
};

class VR_RequestPayment : public RA_Request
{
public:
	// Request Variables
	long m_iAmount;
	string m_sInvoiceNumber;
	unsigned long m_FKID_PlutoId_Establishment,m_FKID_PlutoId_User;
	unsigned long long m_iMacAddress;  // The phone that this will go to
	string m_sDescription,m_sCashierName;
	list<InvoiceDetail *> m_listInvoiceDetail;

	// Response Variables
	short m_iPaymentResponse;
	unsigned long m_iTransactionNumber;
	string m_sApprovalCode,m_sCardNumber;

	// The establishment will call this constructor, then ConvertRequestToBinary
	VR_RequestPayment(u_int64_t iMacAddress,long Amount,string InvoiceNumber,
		string Description,string CashierName,unsigned long FKID_PlutoId_Establishment,
		unsigned long FKID_PlutoId_User);
	VR_RequestPayment() {}

	virtual uint32_t ID() { return VRS_REQUEST_PAYMENT; }

	virtual void SetupSerialization_Request()
	{
		RA_Request::SetupSerialization_Request();
		StartSerializeList() + m_iAmount + m_sInvoiceNumber
			+ m_FKID_PlutoId_Establishment + m_FKID_PlutoId_User
			+ m_iMacAddress + m_sDescription + m_sCashierName;
		(*this) + m_listInvoiceDetail;
	}
	virtual void SetupSerialization_Response()
	{
		RA_Request::SetupSerialization_Response();
		StartSerializeList() + m_iPaymentResponse
			+ m_iTransactionNumber
			+ m_sApprovalCode + m_sCardNumber;
	}
	VR_RequestPayment &operator+ (list<InvoiceDetail *> &i ) { m_vectItemToSerialize.push_back( new ItemToSerialize( SERIALIZE_DATA_TYPE_LIST_INV_DETAIL, (void *) &i) ); return (*this); }
	virtual bool UnknownSerialize( ItemToSerialize *pItem, bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition );

	virtual bool ProcessRequest(class RA_Processor *pRA_Processor);
};


#endif

