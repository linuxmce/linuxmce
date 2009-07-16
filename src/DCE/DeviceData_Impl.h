/*
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
 * @file DeviceData_Impl.h
 * @brief header file for the DeviceData_Impl classe
 * @author
 * @todo notcommented
 *
 */


#ifndef DEVICEDATA_IMPL_h
#define DEVICEDATA_IMPL_h

#include "Event_Impl.h"
#include "Message.h"
#include "DeviceData_Base.h"
#include "SerializeClass/SerializeClass.h"

#define SERIALIZE_DATA_TYPE_VECTOR_DEVICEDATA_IMPL		3000

namespace DCE
{

	typedef vector<class DeviceData_Impl *> VectDeviceData_Impl;

	/** @class DeviceData_Impl
	 Containes the actual device data from this we derive the class containing data for each specific device

    @see DeviceData_Base class
	 */
	class DeviceData_Impl : public DeviceData_Base
	{

	public:
		Event_Impl *m_pEvent_Impl; /**< pointer to an event implementation assicieted with the current device @todo ask */
		map<int, string> m_mapParameters; /**< integer-keyed map with the parameters this device has @todo ask - is string=paramvalue? */
        string m_mapParameters_Find(int PK_DeviceData) { map<int, string>::iterator it = m_mapParameters.find(PK_DeviceData); return it==m_mapParameters.end() ? "" : (*it).second; }

		VectDeviceData_Impl m_vectDeviceData_Impl_Children; /**< vector containing the child devices  */
		bool m_bUsePingToKeepAlive,m_bRunningWithoutDeviceData;

		/** @todo check comment */
		/*
		DCEDeviceData_Impl(char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition,int x)
			: DCEDeviceData_Base(pDataBlock,AllocatedSize,CurrentPosition,x)
		{
			Read_string(m_sDescription);

			unsigned long NumParms = Read_unsigned_long();
			for(unsigned long lP=0;lP<NumParms;++lP)
			{
				int ParmNum = Read_unsigned_long();
				string Value;
				Read_string(Value);
				SetParm(ParmNum,Value.c_str());
			}

			unsigned long NumChildDevice = Read_unsigned_long();
			for(unsigned long lC=0;lC<NumChildDevice;++lC)
			{
				DCEDeviceData_Impl *ChildDevice = CreateData(this,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
				m_vectDCEDeviceData_Impl_Children.push_back(ChildDevice);
			}
		}
		*/

		/**
		 * @brief default constructor
		 */
		DeviceData_Impl()
		{
			m_bRunningWithoutDeviceData=false;
			m_bUsePingToKeepAlive=false;
			m_bDeviceData_Impl = true;
		}

		/**
		 * @brief constructor, it just assigns values to the member data (and calls the base class constructor)
		 */
		DeviceData_Impl( unsigned long dwPK_Device, unsigned long dwPK_Installation, unsigned long dwPK_Device_Template, unsigned long  dwPK_Device_Controlled_Via,
			unsigned long m_dwPK_DeviceCategory, unsigned long dwPK_Room, bool bImplementsDCE, bool bIsEmbedded, string sCommandLine, bool bIsPlugIn, string sDescription,
			string sIPAddress, string sMacAddress, bool bInheritsMacFromPC, bool bDisabled )
			: DeviceData_Base( dwPK_Device, dwPK_Installation, dwPK_Device_Template, dwPK_Device_Controlled_Via, m_dwPK_DeviceCategory, dwPK_Room,
			bImplementsDCE, bIsEmbedded, sCommandLine, bIsPlugIn, sDescription, sIPAddress, sMacAddress, bInheritsMacFromPC, bDisabled )
		{
			m_bRunningWithoutDeviceData=false;
			m_bUsePingToKeepAlive=false;
			m_bDeviceData_Impl = true;
		}

		virtual ~DeviceData_Impl()
		{
			for(size_t iIndex = 0; iIndex < m_vectDeviceData_Impl_Children.size(); iIndex++)
				delete m_vectDeviceData_Impl_Children[iIndex];
		}

		// TODO -- This should be pure virtual as below.  for some reason MS won't link when it's implemented in the auto-generated
		// derived classes.  Got to figure out why.
		//virtual class DeviceData_Impl *CreateData(DeviceData_Impl *Parent,char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition)=0;

		/**
		 * @brief returnes the parameter value for the specified parameter
		 */
		string mapParameters_Find( int iPK_DeviceData )
		{
			map<int,string>::iterator it = m_mapParameters.find( iPK_DeviceData );
			return it == m_mapParameters.end() ? "" : (*it).second;
		}


		/**
		 * @brief should be overriden to return the description for the device as a char*
		 */
		virtual const char *GetDeviceDescription() { return NULL; };

		/**
		 * @brief setups serialization for the class member data
		 * @warning the m_vectDeviceData_Impl_Children vector is serialized custom
		 * @see the overloads for the + operator
		 */
		void SetupSerialization(int iSC_Version)
		{
			DeviceData_Base::SetupSerialization(iSC_Version);
			StartSerializeList() + m_mapParameters + m_bUsePingToKeepAlive;
			(*this) + m_vectDeviceData_Impl_Children; // this is serialized custom
		}

		/**
		 * @returns "DeviceData_Impl" so that the SerializeClass knows what to serialize
		 */
		virtual string SerializeClassClassName() { return "DeviceData_Impl"; }

		/** overloding + for our custom serialize types */

		/**
		 * @brief adds a new ItemToSerialize to the m_vectItemToSerialize list (the m_vectDeviceData_Impl_Children vector)
		 */
		DeviceData_Impl &operator+ ( VectDeviceData_Impl &i ) { m_vectItemToSerialize.push_back( new ItemToSerialize( SERIALIZE_DATA_TYPE_VECTOR_DEVICEDATA_IMPL,(void *) &i ) ); return (*this); }

		/**
		 * @brief overrides the SerializeClass::UnknownSerialize method
		 * @todo ask
		 */
		virtual bool UnknownSerialize( ItemToSerialize *pItem, bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition );

		/**
		 * @brief should create a new DeviceData_Impl object based on the parameters
		 * @warning this must be implemented in a derived class
		 * @todo ask for more specific
		 * @todo This should be pure virtual as below.  for some reason MS won't link when it's implemented in the auto-generated derived classes.  Got to figure out why.
		 * virtual class DeviceData_Impl *CreateData(DeviceData_Impl *Parent,char *pDataBlock,unsigned long AllocatedSize,char CurrentPosition)=0;
		 */
		virtual class DeviceData_Impl *CreateData( DeviceData_Impl * /*pParent*/, char * /*pcDataBlock*/,
				unsigned long /*dwAllocatedSize*/, char * /*pcCurrentPosition*/) { return NULL; }

		/*
		 * @brief Find self or child within category
		 */
		DeviceData_Impl *FindSelfOrChildWithinCategory( unsigned long dwPK_DeviceCategory )
		{
			if( WithinCategory(dwPK_DeviceCategory) )
				return this;

			for(size_t s=0;s<m_vectDeviceData_Impl_Children.size();++s)
			{
				DeviceData_Impl *pDevice = m_vectDeviceData_Impl_Children[s];
				if( pDevice->WithinCategory(dwPK_DeviceCategory) )
					return pDevice;
			}

			return NULL;
		}

		/**
		 * @brief sets the specified value for the specified parameter
		 */
		virtual void SetParm( unsigned long dwParmNum, const char *pcParm )
		{
			m_mapParameters[dwParmNum] = pcParm;
		}

		virtual DeviceData_Impl *FindChild(int PK_Device)
		{
			for(size_t s=0;s<m_vectDeviceData_Impl_Children.size();++s)
			{
				DeviceData_Impl *pDevice = m_vectDeviceData_Impl_Children[s];
				if( pDevice->m_dwPK_Device==PK_Device )
					return pDevice;
				DeviceData_Impl *pDevice_Child = pDevice->FindChild(PK_Device);
				if( pDevice_Child )
					return pDevice_Child;
			}
			return NULL;
		}

		virtual DeviceData_Impl *FindChild(int PK_DeviceData,string sValue)
		{
			for(size_t s=0;s<m_vectDeviceData_Impl_Children.size();++s)
			{
				DeviceData_Impl *pDevice = m_vectDeviceData_Impl_Children[s];
				if( pDevice->m_mapParameters_Find(PK_DeviceData)==sValue )
					return pDevice;
				DeviceData_Impl *pDevice_Child = pDevice->FindChild(PK_DeviceData,sValue);
				if( pDevice_Child )
					return pDevice_Child;
			}
			return NULL;
		}
	};
}

#endif
