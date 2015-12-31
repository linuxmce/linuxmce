/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU Lesser General Public License for more details.

*/

/** @file Virtual_Device_Translator.h
 Header file for the Virtual_Device_Translator class.
 @todo notcommented
 */
#ifndef Virtual_Device_Translator_h
#define Virtual_Device_Translator_h

namespace DCE
{
	class DeviceData_Base;
}
using namespace DCE;

/**
@class Virtual_Device_Translator
This class ???.
*/
class Virtual_Device_Translator
{
	DeviceData_Base *m_pDeviceData_Base;

	// Some virtual devices local to this media director
	int m_dwPK_Device_MediaDirector,m_dwPK_Device_DiscDrive,m_dwPK_Device_AppServer,m_dwPK_Device_LCD_VFD,m_dwPK_Device_IRReceiver,m_dwPK_Device_MediaBurner,m_dwPK_Device_VideoConf,m_dwPK_Device_CamcorderCapt,m_dwPK_Device_Orbiter,m_dwPK_Device_WebBrowser;

	// Some global devices
	int m_dwPK_Device_Router,m_dwPK_Device_DatagridPlugIn,m_dwPK_Device_InfraredPlugIn,m_dwPK_Device_GeneralInfoPlugIn,m_dwPK_Device_EventPlugIn,m_dwPK_Device_OrbiterPlugIn,
		m_dwPK_Device_LightingPlugIn, m_dwPK_Device_ClimatePlugIn, m_dwPK_Device_MediaPlugIn, m_dwPK_Device_TelecomPlugIn, m_dwPK_Device_SecurityPlugIn, m_dwPK_Device_PlugAndPlayPlugIn,
		m_dwPK_Device_LocalAppServer,m_dwPK_Device_LocalMediaPlayer, m_dwPK_Device_WeatherPlugIn;

public:
	Virtual_Device_Translator() {};
	virtual ~Virtual_Device_Translator() {};
	virtual bool GetConfig(DeviceData_Base *pDeviceData_Base);

	void TranslateVirtualDevice(int PK_DeviceTemplate,long &PK_Device);

};

#endif
