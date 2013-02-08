/*
	DeviceData_Base

	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU Lesser General Public License for more details.
*/

/**
 *
 * @file DeviceData_Base.cpp
 * @brief source file for the AllDevices class
 *
 */


#include "Logger.h"
#include "Command_Impl.h"
#include "PlutoUtils/CommonIncludes.h"
#include "DeviceData_Base.h"

using namespace DCE;

AllDevices::~AllDevices()
{
	if( m_bDontDeleteData )
		return;

	for(Map_DeviceData_Base::iterator itData_Base=m_mapDeviceData_Base.begin();itData_Base!=m_mapDeviceData_Base.end();++itData_Base)
		delete (*itData_Base).second;

	for(Map_DeviceCategory::iterator itCategory=m_mapDeviceCategory.begin();itCategory!=m_mapDeviceCategory.end();++itCategory)
		delete (*itCategory).second;

	for(Map_DeviceGroup::iterator itGroup=m_mapDeviceGroup.begin();itGroup!=m_mapDeviceGroup.end();++itGroup)
		delete (*itGroup).second;
}

bool AllDevices::Serialize( bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition, void *pExtraSerializationData )
{
	bool bResult = SerializeClass::Serialize( bWriting, pcDataBlock, dwAllocatedSize, pcCurrentPosition, pExtraSerializationData );

	if( !bWriting )
	{
		// Map them up to the parents
		Map_DeviceCategory::iterator itCat;
		for( itCat = m_mapDeviceCategory.begin(); itCat != m_mapDeviceCategory.end(); ++itCat )
		{
			DeviceCategory *pCat = (*itCat).second;
			if( pCat->m_dwPK_DeviceCategory_Parent ) // must make this ceck, not all cateories have a parent
			{
				DeviceCategory *pParent = m_mapDeviceCategory_Find( pCat->m_dwPK_DeviceCategory_Parent );
				pCat->m_pDeviceCategory_Parent = pParent;
			}
		}

		// Map them up to the category
		Map_DeviceData_Base::iterator itDevice;
		for( itDevice = m_mapDeviceData_Base.begin(); itDevice != m_mapDeviceData_Base.end(); itDevice++ )
		{
			DeviceData_Base *pDeviceData_Base = (*itDevice).second;
			pDeviceData_Base->m_pDeviceCategory = m_mapDeviceCategory_Find( pDeviceData_Base->m_dwPK_DeviceCategory );
			if( pDeviceData_Base->m_dwPK_Device_ControlledVia )
			{
				pDeviceData_Base->m_pDevice_ControlledVia = m_mapDeviceData_Base_Find(pDeviceData_Base->m_dwPK_Device_ControlledVia);
				if( pDeviceData_Base->m_pDevice_ControlledVia )
					pDeviceData_Base->m_pDevice_ControlledVia->m_vectDeviceData_Base_Children.push_back(pDeviceData_Base);
			}
			if( pDeviceData_Base->m_dwPK_Device_Core )
				pDeviceData_Base->m_pDevice_Core = m_mapDeviceData_Base_Find(pDeviceData_Base->m_dwPK_Device_Core);
			if( pDeviceData_Base->m_dwPK_Device_MD )
				pDeviceData_Base->m_pDevice_MD = m_mapDeviceData_Base_Find(pDeviceData_Base->m_dwPK_Device_MD);
		}

		Map_DeviceGroup::iterator itGroup;
		for( itGroup = m_mapDeviceGroup.begin(); itGroup != m_mapDeviceGroup.end(); ++itGroup )
		{
			DeviceGroup *pDeviceGroup = (*itGroup).second;
			for(size_t s=0;s<pDeviceGroup->m_vectPK_Device.size();++s)
			{
				DeviceData_Base *pDeviceData_Base = m_mapDeviceData_Base_Find(pDeviceGroup->m_vectPK_Device[s]);
				pDeviceGroup->m_vectDeviceData_Base.push_back(pDeviceData_Base);
				pDeviceData_Base->m_vectDeviceGroup.push_back(pDeviceGroup);
			}
		}
	}

	return bResult;
}

bool AllDevices::UnknownSerialize( ItemToSerialize *pItem, bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition )
{
	m_pcDataBlock = pcDataBlock; m_dwAllocatedSize = dwAllocatedSize; m_pcCurrentPosition = pcCurrentPosition;

	if( bWriting ) // writing
	{
		switch( pItem->m_iSerializeDataType )
		{
		case SERIALIZE_DATA_TYPE_MAP_DCEDEVICE_GROUP:
			{
				Map_DeviceGroup *pMap = (Map_DeviceGroup *) pItem->m_pItem;
				Write_unsigned_long((unsigned long) pMap->size());
				for( Map_DeviceGroup::iterator it = pMap->begin(); it != pMap->end(); ++it )
				{
					DeviceGroup *pDeviceGroup = (*it).second; // we know how to serialize this
					pDeviceGroup->Serialize( bWriting, m_pcDataBlock, m_dwAllocatedSize, m_pcCurrentPosition );
				}
				return true; // We handled it
			}
			break;
		case SERIALIZE_DATA_TYPE_MAP_DCEDEVICEDATA_BASE:
			{
				Map_DeviceData_Base *pMap = (Map_DeviceData_Base *) pItem->m_pItem;
				Write_unsigned_long( (unsigned long) pMap->size() );
				for( Map_DeviceData_Base::iterator it = pMap->begin(); it != pMap->end(); ++it )
				{
					DeviceData_Base *pDeviceData_Base = (*it).second; // we know how to serialize this
					pDeviceData_Base->Serialize( bWriting, m_pcDataBlock, m_dwAllocatedSize, m_pcCurrentPosition );
				}
				return true; // We handled it
			}
			break;
		case SERIALIZE_DATA_TYPE_MAP_DCECATEGORY:
			{
				Map_DeviceCategory *pMap = (Map_DeviceCategory *) pItem->m_pItem;
				Write_unsigned_long((unsigned long) pMap->size());
				for( Map_DeviceCategory::iterator it = pMap->begin(); it != pMap->end(); ++it )
				{
					DeviceCategory *pDeviceCategory = (*it).second; // we know how to serialize this
					pDeviceCategory->Serialize( bWriting, m_pcDataBlock, m_dwAllocatedSize, m_pcCurrentPosition );
				}
				return true; // We handled it
			}
			break;

		};
	}
	else // reading
	{
		switch( pItem->m_iSerializeDataType )
		{
		case SERIALIZE_DATA_TYPE_MAP_DCEDEVICE_GROUP:
			{
				Map_DeviceGroup *pMap = (Map_DeviceGroup *) pItem->m_pItem;
				unsigned long count = Read_unsigned_long();
				for( unsigned long i = 0; i < count; ++i )
				{
					DeviceGroup *pDeviceGroup = new DeviceGroup();
					pDeviceGroup->Serialize( bWriting, m_pcDataBlock, m_dwAllocatedSize, m_pcCurrentPosition );
					(*pMap)[pDeviceGroup->m_dwPK_DeviceGroup] = pDeviceGroup;
				}
				return true;  // We handled it
			}
			break;

		case SERIALIZE_DATA_TYPE_MAP_DCEDEVICEDATA_BASE:
			{
				Map_DeviceData_Base *pMap = (Map_DeviceData_Base *) pItem->m_pItem;
				unsigned long count = Read_unsigned_long();
				for( unsigned long i = 0; i < count; ++i )
				{
					DeviceData_Base *pDeviceData_Base = new DeviceData_Base();
					pDeviceData_Base->Serialize( bWriting, m_pcDataBlock, m_dwAllocatedSize, m_pcCurrentPosition );
					(*pMap)[pDeviceData_Base->m_dwPK_Device] = pDeviceData_Base;
				}
				return true;  // We handled it
			}
			break;

		case SERIALIZE_DATA_TYPE_MAP_DCECATEGORY:
			{
				Map_DeviceCategory *pMap = (Map_DeviceCategory *) pItem->m_pItem;
				unsigned long count = Read_unsigned_long();
				for( unsigned long i = 0; i < count; ++i )
				{
					DeviceCategory *pDeviceCategory = new DeviceCategory();
					pDeviceCategory->Serialize( bWriting, m_pcDataBlock, m_dwAllocatedSize, m_pcCurrentPosition );
					(*pMap)[pDeviceCategory->m_dwPK_DeviceCategory] = pDeviceCategory;
				}
				return true;  // We handled it
			}
			break;

		};
	}

	pcDataBlock = m_pcDataBlock; dwAllocatedSize = m_dwAllocatedSize; pcCurrentPosition = m_pcCurrentPosition;
	return true;
}

DeviceData_Base *AllDevices::m_mapDeviceData_Base_FindFirstOfCategory( unsigned long dwPK_DeviceCategory )
{
	for(map<int,class DeviceData_Base *>::iterator it = m_mapDeviceData_Base.begin();it != m_mapDeviceData_Base.end();++it)
		if( (*it).second->m_dwPK_DeviceCategory==dwPK_DeviceCategory )
			return (*it).second;
	return NULL;
}

DeviceData_Base *AllDevices::m_mapDeviceData_Base_FindFirstOfTemplate( long dwPK_DeviceTemplate )
{
	for(map<int,class DeviceData_Base *>::iterator it = m_mapDeviceData_Base.begin();it != m_mapDeviceData_Base.end();++it)
		if( (*it).second->m_dwPK_DeviceTemplate==dwPK_DeviceTemplate )
			return (*it).second;
	return NULL;
}


// This will return the first device within the given category that is in any way
// related (ie also a child of the topmost device, meaning it runs on the same PC).  Call leaving the default parameters unspecified.
DeviceData_Base *DeviceData_Base::FindFirstRelatedDeviceOfCategory(int PK_DeviceCategory,Command_Impl *pCommand_Impl_Confirm_Registration,int TimeoutToWait,bool bScanParent,int PK_Device_ExcludeChild)
{
	if( m_bDeviceData_Impl )
	{
		DeviceData_Base *pDeviceData_Base = m_AllDevices.m_mapDeviceData_Base_Find(m_dwPK_Device);
		if( pDeviceData_Base )
			return pDeviceData_Base->FindFirstRelatedDeviceOfCategory(PK_DeviceCategory,pCommand_Impl_Confirm_Registration,TimeoutToWait);
		else
			return NULL;
	}

	if( WithinCategory(PK_DeviceCategory) )
	{
		if( pCommand_Impl_Confirm_Registration )
		{
			time_t tTimeout = time(NULL) + TimeoutToWait;
			char Status = 'N';
			while (Status != 'Y')
			{
				Status = pCommand_Impl_Confirm_Registration->DeviceIsRegistered(m_dwPK_Device);
				switch (Status)
				{
					case 'Y': // is treated in the while loop
						return this;
					case 'N': // not registered
						if( tTimeout < time(NULL) )
						{

								LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "FindFirstRelatedDeviceOfCategory %d dev %d never registered",PK_DeviceCategory,m_dwPK_Device);
							return NULL;
						}
#ifdef DEBUG

							LoggerWrapper::GetInstance()->Write(LV_STATUS, "FindFirstRelatedDeviceOfCategory %d dev %d waiting for device",PK_DeviceCategory,m_dwPK_Device);
#endif
						Sleep(1000);
						break;
					case 'D':

							LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "FindFirstRelatedDeviceOfCategory %d dev %d is disabled",PK_DeviceCategory,m_dwPK_Device);
						return NULL;
					case 'E':

							LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "FindFirstRelatedDeviceOfCategory %d dev %d comm error",PK_DeviceCategory,m_dwPK_Device);
						return NULL;
					default:

							LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "FindFirstRelatedDeviceOfCategory %d dev %d has unknown status %c",PK_DeviceCategory,m_dwPK_Device,Status);
				}
			}
		}
		return this;
	}
	for(size_t s=0;s<m_vectDeviceData_Base_Children.size();++s)
	{
		DeviceData_Base *pDeviceData_Base = m_vectDeviceData_Base_Children[s];
		if( pDeviceData_Base->m_dwPK_Device==PK_Device_ExcludeChild )
			continue;
		DeviceData_Base *pDeviceData_Base_Result = pDeviceData_Base->FindFirstRelatedDeviceOfCategory(PK_DeviceCategory,pCommand_Impl_Confirm_Registration,TimeoutToWait,false);
		if( pDeviceData_Base_Result )
			return pDeviceData_Base_Result;
	}
	if( bScanParent && m_pDevice_ControlledVia )
		return m_pDevice_ControlledVia->FindFirstRelatedDeviceOfCategory(PK_DeviceCategory,pCommand_Impl_Confirm_Registration,TimeoutToWait,true,m_dwPK_Device);
	return NULL;
}

// This will return the first device of the given template that is in any way
// related (ie also a child of the topmost device, meaning it runs on the same PC).  Call leaving the default parameters unspecified.
DeviceData_Base *DeviceData_Base::FindFirstRelatedDeviceOfTemplate(int PK_DeviceTemplate,Command_Impl *pCommand_Impl_Confirm_Registration,int TimeoutToWait,bool bScanParent, int PK_Device_ExcludeChild)
{
	if( m_bDeviceData_Impl )
	{
		DeviceData_Base *pDeviceData_Base = m_AllDevices.m_mapDeviceData_Base_Find(m_dwPK_Device);
		if( pDeviceData_Base )
			return pDeviceData_Base->FindFirstRelatedDeviceOfTemplate(PK_DeviceTemplate,pCommand_Impl_Confirm_Registration,TimeoutToWait);
		else
			return NULL;
	}

	if( m_dwPK_DeviceTemplate==PK_DeviceTemplate )
	{
		if( pCommand_Impl_Confirm_Registration )
		{
			time_t tTimeout = time(NULL) + TimeoutToWait;
			char Status = 'N';
			while (Status != 'Y')
			{
				Status = pCommand_Impl_Confirm_Registration->DeviceIsRegistered(m_dwPK_Device);
				switch (Status)
				{
					case 'Y': // is treated in the while loop
						return this;
					case 'N': // not registered
						if( tTimeout < time(NULL) )
						{

								LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "FindFirstRelatedDeviceOfTemplate %d never registered",m_dwPK_Device);
							return NULL;
						}
						Sleep(1000);
						break;
					case 'D':

							LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "FindFirstRelatedDeviceOfTemplate %d is disabled",m_dwPK_Device);
						return NULL;
					case 'E':

							LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "FindFirstRelatedDeviceOfTemplate %d comm error",m_dwPK_Device);
						return NULL;
				}
			}
		}

		return this;
	}
	for(size_t s=0;s<m_vectDeviceData_Base_Children.size();++s)
	{
		DeviceData_Base *pDeviceData_Base = m_vectDeviceData_Base_Children[s];
		if( pDeviceData_Base->m_dwPK_Device==PK_Device_ExcludeChild )
			continue;
		DeviceData_Base *pDeviceData_Base_Result = pDeviceData_Base->FindFirstRelatedDeviceOfTemplate(PK_DeviceTemplate,pCommand_Impl_Confirm_Registration,TimeoutToWait,false);
		if( pDeviceData_Base_Result )
			return pDeviceData_Base_Result;
	}
	if( bScanParent && m_pDevice_ControlledVia )
		return m_pDevice_ControlledVia->FindFirstRelatedDeviceOfTemplate(PK_DeviceTemplate,pCommand_Impl_Confirm_Registration,TimeoutToWait,true,m_dwPK_Device);
	return NULL;
}
