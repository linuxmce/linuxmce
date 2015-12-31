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
//Predefined registry keys for Pluto VIP Service

//definitions for registry keys
//all the entries are UNICODE compliant
#define REGISTRY_KEY_SOFTWARE							_T("Software")
#define REGISTRY_KEY_1CONTROL							_T("1Control")
#define REGISTRY_KEY_PLUTO								_T("Pluto")
#define REGISTRY_KEY_VIPSERVICE							_T("VIP Service")
#define REGISTRY_KEY_PLUTO_FULL							_T("Software\\1Control\\PlutoVIP")
#define REGISTRY_KEY_VIPSERVICE_FULL					_T("Software\\1Control\\PlutoVIP\\VIP Service")

//Property pages registry value names
//PageTerminals
#define REGISTRY_PAGE_TERMINALS_TERMINALS				_T("TerminalsTerminals")
#define REGISTRY_PAGE_TERMINALS_CLIENT					_T("TerminalsClient")
#define REGISTRY_PAGE_TERMINALS_DIRECTORY				_T("TerminalsDirectory")
#define REGISTRY_PAGE_TERMINALS_SERVER					_T("TerminalsServer")

#define REGISTRY_SHARED_DIRECTORY						REGISTRY_PAGE_TERMINALS_DIRECTORY

//PagePanelPosition
#define REGISTRY_PAGE_PANEL_POSITION_ACTIVE_APPEARANCE	_T("PanelActiveAppearance")
#define REGISTRY_PAGE_PANEL_POSITION_RECENT_APPEARANCE	_T("PanelRecentAppearance")
#define REGISTRY_PAGE_PANEL_POSITION_NEW_APPEARANCE		_T("PanelNewAppearance")
#define REGISTRY_PAGE_PANEL_POSITION_NEW_TIME			_T("PanelNewTime")
#define REGISTRY_PAGE_PANEL_POSITION_RECENT_TIME		_T("PanelRecentTime")
#define REGISTRY_PAGE_PANEL_POSITION_NEW_SIDE			_T("PanelNewSide")
#define REGISTRY_PAGE_PANEL_POSITION_ACTIVE_SIDE		_T("PanelActiveSide")
#define REGISTRY_PAGE_PANEL_POSITION_RECENT_SIDE		_T("PanelRecentSide")

#define APPBAR_POSITION_LEFT			0
#define APPBAR_POSITION_TOP				1
#define APPBAR_POSITION_RIGHT			2
#define APPBAR_POSITION_BOTTOM			3

#define APPBAR_APPEARANCE_VISIBLE		0
#define APPBAR_APPEARANCE_AUTOHIDE		1
#define APPBAR_APPEARANCE_INVISIBLE		2

//PageDatabase
#define REGISTRY_PAGE_DATABASE_COPY						_T("DatabaseCopy")
#define REGISTRY_PAGE_DATABASE_SWITCH					_T("DatabaseSwitch")
#define REGISTRY_PAGE_DATABASE_WHAT_DO					_T("DatabaseWhatDo")
#define REGISTRY_PAGE_DATABASE_WHICH_ONE				_T("DatabaseWhichOne")

//PageInternet
#define REGISTRY_PAGE_INTERNET_DIALUP_CONNECTIONS		_T("InternetDialupConnections")
#define REGISTRY_PAGE_INTERNET_DIALUP_CONNECTIONS_NAME	_T("InternetDialupConnectionsName")
#define REGISTRY_PAGE_INTERNET_INTERNET_CONNECTION		_T("InternetConnection")
#define REGISTRY_PAGE_INTERNET_INTERNET_WHEN			_T("InternetWhen")

#define INTERNET_CONNECTION_DSL_LAN		0	//connecting to internet via LAN/DSL/...
#define INTERNET_CONNECTION_DIALUP		1	//connecting to internet via dial up connection
#define INTERNET_CONNECTION_NOT_CONNECT	2	//not connect to internet

//Version of the program
#define PLUTOVIP_VERSION_FULL			_T("VersionFull")
#define PLUTOVIP_VERSION_FULL_NUMBER	_T("1.0.0.0")		//1.0.0.0
#define PLUTOVIP_VERSION_VERSION		_T("Version")
#define PLUTOVIP_VERSION_VERSION_NUMBER	_T("1")	
#define PLUTOVIP_VERSION_MAJOR			_T("Major")
#define PLUTOVIP_VERSION_MAJOR_NUMBER	_T("0")
#define PLUTOVIP_VERSION_MINOR			_T("Minor")
#define PLUTOVIP_VERSION_MINOR_NUMBER	_T("0")
#define PLUTOVIP_VERSION_BUILD			_T("Build")
#define PLUTOVIP_VERSION_BUILD_NUMBER	_T("0")

//URL
#define PLUTOVIP_VERSION_URL	_T("http://www.plutovip.com/PlutoVIP_Version.txt")

//Setup program
#define PLUTOVIP_SETUP_PROGRAM	_T("PlutoVIP_Setup.exe")

//PlutoVIP on the web
#define PLUTOVIP_WEB_HOMEPAGE		_T("http://www.plutovip.com/")
#define PLUTOVIP_WEB_FAQ			_T("http://www.plutovip.com/")
#define PLUTOVIP_WEB_PRODUCT_NEWS	_T("http://www.plutovip.com/")
#define PLUTOVIP_WEB_ONLINE_SUPPORT	_T("http://www.plutovip.com/")
#define PLUTOVIP_WEB_FREE_STUFF		_T("http://www.plutovip.com/")
#define PLUTOVIP_HELP				_T("PlutoVIP.pdf")

//registry keys for appbars
#define APPBAR_CX					_T("Cx")
#define APPBAR_AUTOHIDE				_T("AutoHide")
