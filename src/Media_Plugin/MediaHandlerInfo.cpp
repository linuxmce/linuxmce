/*
 MediaHandlerInfo

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

#include "PlutoUtils/CommonIncludes.h"
#include "MediaHandlerInfo.h"
#include "MediaHandlerBase.h"
#include "EntertainArea.h"
#include "Media_Plugin.h"

#include "DCERouter.h"
#include "DeviceData_Router.h"
#include "Orbiter_Plugin/Orbiter_Plugin.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_MediaType.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "DataGrid.h"

int g_MediaHandlerID=0;  // So we give each handler a unique ID

MediaHandlerInfo::MediaHandlerInfo( class MediaHandlerBase *pMediaHandlerBase, class Command_Impl *pCommand_Impl, int PK_MediaType, int PK_DeviceTemplate, bool bCanJumpPosition, bool bUsesDevices, bool bDontRegister )
{
	m_MediaHandlerID=g_MediaHandlerID++;
    m_pMediaHandlerBase=pMediaHandlerBase;
    m_pCommand_Impl=pCommand_Impl;
    m_PK_MediaType=PK_MediaType;
    m_bCanJumpPosition=bCanJumpPosition;
    m_bUsesDevices=bUsesDevices;
    m_bUsesRemovableMedia=true;
	m_bMultipleDestinations=false;
	m_PK_DeviceTemplate=PK_DeviceTemplate;

    if( m_PK_DeviceTemplate )
    {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"XXXXXXXXXXXXXX MediaHandlerInfo template is %d",m_PK_DeviceTemplate);
        ListDeviceData_Router *pListDeviceData_Router=m_pMediaHandlerBase->m_pMedia_Plugin->m_pRouter->m_mapDeviceByTemplate_Find( m_PK_DeviceTemplate );
		if( !pListDeviceData_Router )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media Handler Info cannot find any devices for template: %d",m_PK_DeviceTemplate);
			return;
		}
        for( ListDeviceData_Router::iterator it=pListDeviceData_Router->begin( );it!=pListDeviceData_Router->end( );++it )
        {
            DeviceData_Router *pDeviceData_Router = *it;
            MediaDevice *pMediaDevice = m_pMediaHandlerBase->m_pMedia_Plugin->m_mapMediaDevice_Find( pDeviceData_Router->m_dwPK_Device );
            if( !pMediaDevice )
                LoggerWrapper::GetInstance()->Write( LV_WARNING, "Device %d (%s) isn't in an entertainment area", pDeviceData_Router->m_dwPK_Device, pDeviceData_Router->m_sDescription.c_str() );
            else
                m_listMediaDevice.push_back( pMediaDevice );
        }
    }

    Row_DeviceTemplate_MediaType *pRow_DeviceTemplate_MediaType=NULL;
    if( m_PK_DeviceTemplate )
    {
        vector<Row_DeviceTemplate_MediaType *> vectRow_DeviceTemplate_MediaType;
        pMediaHandlerBase->m_pMedia_Plugin->m_pDatabase_pluto_main->DeviceTemplate_MediaType_get( )->GetRows(
                DEVICETEMPLATE_MEDIATYPE_FK_DEVICETEMPLATE_FIELD + string( "=" ) + StringUtils::itos( m_PK_DeviceTemplate ) + " AND " +
                DEVICETEMPLATE_MEDIATYPE_FK_MEDIATYPE_FIELD + "=" + StringUtils::itos( PK_MediaType ), &vectRow_DeviceTemplate_MediaType );

        if( vectRow_DeviceTemplate_MediaType.size( ) ) // there should only be 1 anyway
            pRow_DeviceTemplate_MediaType = vectRow_DeviceTemplate_MediaType[0];
    }

    // See if we have information specific to this media type on this device. Otherwise we'll use the defaults for the media type
    if( pRow_DeviceTemplate_MediaType )
    {
        string::size_type pos=0;
        while( true )
        {
            string Extension_list;
			if( pRow_DeviceTemplate_MediaType->Extensions_isNull( ) )
				Extension_list = pRow_DeviceTemplate_MediaType->FK_MediaType_getrow()->Extensions_get();
			else
				Extension_list = pRow_DeviceTemplate_MediaType->Extensions_get( );

            string Extension = StringUtils::ToUpper( StringUtils::Tokenize( Extension_list, string( ", " ), pos ) );
            if( !Extension.size( ) )
                break;
            m_listExtensions.push_back( Extension );
        }
		m_bUsesRemovableMedia = pRow_DeviceTemplate_MediaType->CanPlayFromDiskDrive_get( )==1;
		m_bCanStoreOnServer = pRow_DeviceTemplate_MediaType->CanStoreOnServer_get( )==1;
		m_bIsExternalTransmission = pRow_DeviceTemplate_MediaType->FK_MediaType_getrow()->IsExternalTransmission_get( )==1;
		m_bMultipleDestinations = pRow_DeviceTemplate_MediaType->CanPlayInMultipleAreas_get( )==1;
    }
    else
    {
        Row_MediaType *pRow_MediaType=m_pMediaHandlerBase->m_pMedia_Plugin->m_pDatabase_pluto_main->MediaType_get( )->GetRow( m_PK_MediaType );
        if( pRow_MediaType )
        {
            string::size_type pos=0;
            while( true )
            {
                string Extension_list = pRow_MediaType->Extensions_get( );
                string Extension = StringUtils::ToUpper( StringUtils::Tokenize( Extension_list, string( ", " ), pos ) );
                if( !Extension.size( ) )
                    break;
                m_listExtensions.push_back( Extension );
            }
        }
    }

    // Now find all the entertainment areas served by those devices, and put ourselves as a media handler there
    for( list<class MediaDevice *>::iterator itListMD=m_listMediaDevice.begin( );itListMD!=m_listMediaDevice.end( );++itListMD )
    {
        MediaDevice *pMediaDevice = *itListMD;
        for( map<int,class EntertainArea *>::iterator itEntArea=pMediaDevice->m_mapEntertainArea.begin( );itEntArea!=pMediaDevice->m_mapEntertainArea.end( );++itEntArea )
        {
            EntertainArea *pEntertainArea = itEntArea->second;
            List_MediaHandlerInfo *pList_MediaHandlerInfo = pEntertainArea->m_mapMediaHandlerInfo_MediaType_Find( m_PK_MediaType,pMediaDevice->m_iPK_MediaProvider );
            if( !pList_MediaHandlerInfo )
            {
                pList_MediaHandlerInfo = new List_MediaHandlerInfo( );
                pEntertainArea->m_mapMediaHandlerInfo_MediaType[make_pair (m_PK_MediaType,pMediaDevice->m_iPK_MediaProvider) ] = pList_MediaHandlerInfo;
            }
            pList_MediaHandlerInfo->push_back( this );

            for( list<string>::iterator itExt=m_listExtensions.begin( );itExt!=m_listExtensions.end( );++itExt )
            {
                string Extension = *itExt;

                pList_MediaHandlerInfo = pEntertainArea->m_mapMediaHandlerInfo_Extension_Find( Extension );
                if( !pList_MediaHandlerInfo )
                {
                    pList_MediaHandlerInfo = new List_MediaHandlerInfo( );
                    pEntertainArea->m_mapMediaHandlerInfo_Extension[Extension] = pList_MediaHandlerInfo;
                }
                pList_MediaHandlerInfo->push_back( this );
            }
        }
    }
// todo m_pMediaHandlerBase->m_pMedia_Plugin->RegisterMediaPlugin( this );
}

bool MediaHandlerInfo::ControlsDevice(int PK_Device)
{
	for(list<class MediaDevice *>::iterator it=m_listMediaDevice.begin();it!=m_listMediaDevice.end();++it)
		if( (*it)->m_pDeviceData_Router->m_dwPK_Device==PK_Device )
			return true;
	return false;
}
