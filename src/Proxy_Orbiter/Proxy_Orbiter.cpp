/*
 Proxy_Orbiter
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to 
 the terms of the Pluto Public License, available at: 
 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, 
 but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. 
 
 See the Pluto Public License for more details.
 
 */

#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/PlutoDefs.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/HttpUtils.h"
#include "DCE/Logger.h"
#include "Proxy_Orbiter.h"
#include "SerializeClass/ShapesColors.h"
#include "pluto_main/Define_Button.h"
#include "pluto_main/Define_DesignObjParameter.h"
#include "Orbiter/ScreenHistory.h"
#include "pluto_main/Define_DesignObj.h"
#include "../Orbiter/SDL/OrbiterRenderer_SDL.h"

#include "DataGrid.h"
#include "Gen_Devices/AllCommandsRequests.h"

#include <fcntl.h>

using namespace DCE;
#if defined(WIN32) 
void WriteStatusOutput(const char *) {} //do nothing
#endif

#define RELEVANT_SCREEN_INTERVAL 800
#define CISCO_LISTEN_PORT_START 3451
#define REQUEST_INTERVAL_TIMEOUT 400
#define CISCO_FAILURE_WATCHDOG_INTERVAL 1000
#define MINIMUM_PUSH_INTERVAL 6000
#define XML_MAX_TOUCH_ZONES 32

static string XMLEntities(const string &input)
{
	string result;
	for (size_t i = 0; i < input.length(); i++)
	{
		switch (input[i])
		{
			case '<': result += "&lt;"; break;
			case '>': result += "&gt;"; break;
			case '"': result += "&quot;"; break;
			case '\'': result += "&apos;"; break;
			case '&': result += "&amp;"; break;
			default: result += input[i];
		}
	}

	return result;
}

static string XMLEntities(const char *input)
{
	if (input == NULL)
		return "(null)";
	return XMLEntities(string(input));
}

/* release the mutex when calling Orbiter methods to avoid deadlock, and relock after call
 * sequence:
 *     DCE::Proxy_Orbiter::ReceivedString
 *         lock m_ActionMutex
 *     DCE::Orbiter::RegionDown
 *     DCE::Orbiter::ClickedRegion
 *         lock m_ScreenMutex
 * sequence:
 *     OrbiterRenderer::RefreshScreen
 *         lock m_ScreenMutex
 *     DCE::Proxy_OrbiterRenderer_SDL::RenderScreen
 *     DCE::OrbiterRenderer_SDL::RenderScreen
 *     DCE::Proxy_OrbiterRenderer_SDL::DisplayImageOnScreen
 *     DCE::Proxy_Orbiter::ImageGenerated
 *         lock m_ActionMutex
 */
#define PROXY_ORBITER_MUTEX_SUSPEND(code) \
	do { \
		am.Release(); \
		code; \
		am.DoLock(); \
	} while (false);

//-----------------------------------------------------------------------------------------------------
Proxy_Orbiter::Proxy_Orbiter(int DeviceID, int PK_DeviceTemplate, string ServerAddress)
: Orbiter(DeviceID, PK_DeviceTemplate, ServerAddress, (string)"", false, 0, 
    0, NULL, false), SocketListener("Proxy_Orbiter"), m_ActionMutex("action"), m_ResourcesMutex("resources")
{
	m_iImageCounter = 1;
	m_iLastImageSent = -1;
	m_nCurrentScreenId = 0;

	m_ImageQuality = 70;
	m_bDisplayOn=true;  // Override the default behavior -- when the phone starts the display is already on

	m_bNews = true;

	pthread_cond_init( &m_ActionCond, NULL );
	m_ActionMutex.Init(NULL, &m_ActionCond);
	m_ResourcesMutex.Init(NULL);

	m_sBitsDirectory = "/usr/pluto/orbiter/bits/" + StringUtils::itos(m_dwPK_Device);
	mkdir(m_sBitsDirectory.c_str(), 0600);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ bool Proxy_Orbiter::GetConfig()
{
	if(!Orbiter::GetConfig())
		return false;

	m_ImageQuality = DATA_Get_ImageQuality();
	m_sRemotePhoneIP = DATA_Get_Remote_Phone_IP();
	m_bReportTimeCode = false;

	if(m_ImageQuality < 10 || m_ImageQuality > 100)
		m_ImageQuality = 70; //default
	m_iTimeoutScreenSaver=0; // disable it
	m_iListenPort = DATA_Get_Listen_Port();
	if( !m_iListenPort )
		m_iListenPort = CISCO_LISTEN_PORT_START;

	m_sInternalServerAddress = m_pData->GetTopMostDevice()->GetIPAddress();
	m_sBaseUrl = "http://" + m_sInternalServerAddress + "/lmce-admin/";

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Server's internal ip address: %s", m_sInternalServerAddress.c_str()); 

	return true;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Proxy_Orbiter::Run()
{
	m_sRequestUrl = m_sBaseUrl + 
		"index.php?"
		"section=proxySocket&amp;"
		"address=" + m_sInternalServerAddress + "&amp;"
		"port=" + StringUtils::ltos(m_iListenPort) + "&amp;"
		"command=XML&amp;"
		"deviceID=" + StringUtils::ltos(m_dwPK_Device) + "&amp;";
	m_sPngImageUrl = m_sBaseUrl + "security_images/" + StringUtils::ltos(m_dwPK_Device) + "_cisco.png";

	Initialize(gtSDLGraphic);
	StartListening(m_iListenPort);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ Proxy_Orbiter::~Proxy_Orbiter()
{
	KillMaintThread();

	Sleep(2500);
	PushRefreshEvent(true, true); //force push, ignore any previous push 
	pthread_mutex_destroy(&m_ActionMutex.mutex);
}
//-----------------------------------------------------------------------------------------------------
string Proxy_Orbiter::GetDevicePngFileName()
{
	return StringUtils::ltos(m_dwPK_Device) + "_" + CURRENT_SCREEN_IMAGE;
}
//-----------------------------------------------------------------------------------------------------
string Proxy_Orbiter::GetDeviceXmlFileName()
{
	return StringUtils::ltos(m_dwPK_Device) + "_" + CURRENT_SCREEN_XML;
}
//-----------------------------------------------------------------------------------------------------
bool Proxy_Orbiter::PushRefreshEvent(bool bForce,bool bIgnoreMinimumInterval/*=false*/)
{
	if (m_sRemotePhoneIP == "")
		return false;
	static bool bFirstTime = true;
	static timespec tLastImageGenerated;
	timespec tCurrentImageGenerated;
	gettimeofday(&tCurrentImageGenerated, NULL);
	timespec tInterval = tCurrentImageGenerated - tLastImageGenerated;
	long nMilisecondsPassed = tInterval.tv_sec * 1000 + tInterval.tv_nsec / 1000000;
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Time for last push event %d ms", nMilisecondsPassed);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Current screen id %d", m_nCurrentScreenId);

	if(m_nCurrentScreenId == DESIGNOBJ_mnuDVDmenu_CONST)
		bIgnoreMinimumInterval = false;

	if(!bIgnoreMinimumInterval)
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Minimum interval NOT ignored!");		
	
	if(!bIgnoreMinimumInterval && !bFirstTime && nMilisecondsPassed < MINIMUM_PUSH_INTERVAL && !bForce)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Ignoring push event request...");
		return false;
	}

	tLastImageGenerated = tCurrentImageGenerated;	
	bFirstTime = false;

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Need to refresh phone's browser!");

	m_bPhoneRespondedToPush = false;
	vector<string> vectHeaders;
	map<string, string> mapParams;
	string sPriorityLevel = bForce ? "0" : "1";
	string sRequestUrl = 
		string() + 
		"<CiscoIPPhoneExecute>"
			"<ExecuteItem Priority=\"" + sPriorityLevel + "\" URL=\"" + m_sRequestUrl + "\"/>"
		"</CiscoIPPhoneExecute>";
	mapParams["XML"] = StringUtils::URLEncode(sRequestUrl);

	string Response = HttpPost("http://" + m_sRemotePhoneIP + "/CGI/Execute", vectHeaders, mapParams, "user", "pluto");

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "XML param req %s", sRequestUrl.c_str());
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Push phone action completed with response: %s", Response.c_str());

	if(!bForce)
	{
		//we have to be sure the cisco phone refreshed; we'll start a watchdog
		CallMaintenanceInMiliseconds(CISCO_FAILURE_WATCHDOG_INTERVAL, (OrbiterCallBack)&Proxy_Orbiter::PushRefreshEventWatchdog, NULL, pe_ALL);
	}

	return Response == "Success";
}
//-----------------------------------------------------------------------------------------------------
void *Proxy_Orbiter::PushRefreshEventWatchdog(void *)
{
	if(!m_bPhoneRespondedToPush)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Cisco phone failed to execute request! Forcing a push...");
		//I sent a push event 1 second ago with priority 1; the phone ignored me
		//It should queue my url request and execute it when to phone goes idle
		//geeeeez, cisco, fix your phone! :(
		PushRefreshEvent(true);
	}

	return NULL;
}
//-----------------------------------------------------------------------------------------------------
bool Proxy_Orbiter::IsProcessingRequest()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Is processing request? R: %s", m_bProcessingRequest ? "YES" : "NO");
	return m_bProcessingRequest;
}
//-----------------------------------------------------------------------------------------------------
void Proxy_Orbiter::StartProcessingRequest()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Starting processing request...");
	m_bProcessingRequest = true;
}
//-----------------------------------------------------------------------------------------------------
void Proxy_Orbiter::EndProcessingRequest()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Stopping processing request in few ms...");
	CallMaintenanceInMiliseconds(REQUEST_INTERVAL_TIMEOUT, (OrbiterCallBack)&Proxy_Orbiter::StopProcessingRequest, this, pe_ALL );
}
//-----------------------------------------------------------------------------------------------------
void Proxy_Orbiter::StopProcessingRequest(void *p)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Stopped processing request...");
	m_bProcessingRequest = false;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Proxy_Orbiter::SaveXML(string sFileName)
{
	//if(m_iListenPort >= CISCO_LISTEN_PORT_START && m_iListenPort < CISCO_LISTEN_PORT_START + 10) //only cisco orbiters
	{
		m_dequeXMLItems.clear();
		GenerateXMLItems(m_pScreenHistory_Current->GetObj());
		string sSoftKeys = GenerateSoftKeys(m_pScreenHistory_Current->GetObj());

		string sXMLItems;
		int nCount = 1;
		deque<string>::iterator it;
		for(it = m_dequeCellXMLItems.begin(); it != m_dequeCellXMLItems.end(); it++)
		{
			if(nCount > XML_MAX_TOUCH_ZONES)
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "More than " TOSTRING(XML_MAX_TOUCH_ZONES) " objects needed! Cisco 7970 will not work. Screen: %s", 
					m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str());
			}

			sXMLItems += *it;
			nCount++;
		}

		for(it = m_dequeXMLItems.begin(); it != m_dequeXMLItems.end(); it++)
		{
			if(nCount > XML_MAX_TOUCH_ZONES)
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "More than " TOSTRING(XML_MAX_TOUCH_ZONES) " objects needed! Cisco 7970 will not work. Screen: %s", 
					m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str());
			}

			sXMLItems += *it;
			nCount++;
		}

		string sPrompt;
		string sXMLString = 
			"<CiscoIPPhoneGraphicFileMenu>\r\n"
			"<Title>Pluto Orbiter</Title>\r\n"
			"<Prompt>" + sPrompt + "</Prompt>\r\n"
			"<LocationX>0</LocationX>\r\n"
			"<LocationY>0</LocationY>\r\n"
			"<URL>" + m_sPngImageUrl + "</URL>\r\n" + 
			sXMLItems + 
			sSoftKeys + 
			"</CiscoIPPhoneGraphicFileMenu>\r\n";

		FileUtils::WriteBufferIntoFile(GetDeviceXmlFileName(), sXMLString.c_str(), sXMLString.size());
	}
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Proxy_Orbiter::GenerateXMLItems(DesignObj_Orbiter *pObj) //recursive
{
	bool bTouchArea = false;
	DesignObjZoneList::iterator iZone;
	for( iZone=pObj->m_ZoneList.begin(  );iZone!=pObj->m_ZoneList.end(  );++iZone )
	{
		DesignObjZone *pDesignObjZone = ( *iZone );
		//ExecuteCommandsInList( &pDesignObjZone->m_Commands, pObj, -1, -1, pObj->m_iRepeatParm+1 );

		if(pDesignObjZone->m_Commands.size())
		{
		    bTouchArea = true;
		    break;
		}
	}

	if(bTouchArea)
	{
		string sX1 = StringUtils::ltos(pObj->m_rPosition.X);
		string sY1 = StringUtils::ltos(pObj->m_rPosition.Y);
		string sX2 = StringUtils::ltos(pObj->m_rPosition.Right());
		string sY2 = StringUtils::ltos(pObj->m_rPosition.Bottom());

		string sTouchX = StringUtils::ltos(pObj->m_rPosition.X + pObj->m_rPosition.Width / 2);
		string sTouchY = StringUtils::ltos(pObj->m_rPosition.Y + pObj->m_rPosition.Height / 2);

		string sXMLItem = 
			"<MenuItem>\r\n"
			"\t<Name>Button</Name>\r\n"
			"\t<URL>" + m_sRequestUrl + "x=" + sTouchX + "&amp;" + "y=" + sTouchY + "</URL>\r\n"
			"\t<TouchArea X1=\"" + sX1 + "\" Y1=\"" + sY1 + "\" X2=\"" + sX2 + "\" Y2=\"" + sY2 + "\"/>\r\n"
			"</MenuItem>\r\n";
		m_dequeXMLItems.push_front(sXMLItem);
	}

	DesignObj_DataList::iterator it;
	for(it = pObj->m_ChildObjects.begin(); it != pObj->m_ChildObjects.end(); ++it)
		GenerateXMLItems((DesignObj_Orbiter *)*it);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ string Proxy_Orbiter::GenerateSoftKeys(DesignObj_Orbiter *pObj)
{
	if(m_sNowPlaying.length() > 0)
		return sAllSoftKeysXml;

	return sNoMediaSoftKeysXml;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Proxy_Orbiter::ParseHardKeys()
{
	string::size_type pos = 0;
	string::size_type posTemp = 0;
	string sKeymapping = DATA_Get_Hard_Keys_mapping();
	int nIndex = 1;

	while(pos < DATA_Get_Hard_Keys_mapping().size())
	{
		posTemp = 0;

		//a line
		string sLineToken = StringUtils::Tokenize(sKeymapping, "\n", pos);

		//soft key name
		string sName = StringUtils::Tokenize(sLineToken, "\t", posTemp);

		//key
		int nKey = atoi(StringUtils::Tokenize(sLineToken, "\t", posTemp).c_str());
		if(!nKey)
			continue;

		//media playing flag
		bool bMediaPlaying = 1 == atoi(StringUtils::Tokenize(sLineToken, "\t", posTemp).c_str());
		
		//the message to send
		string sMessageArguments = StringUtils::Tokenize(sLineToken, "\t", posTemp);
		sMessageArguments = StringUtils::Replace(sMessageArguments, "devid", StringUtils::ltos(m_dwPK_Device));
		Message *pMessage = new Message(sMessageArguments);
		if( pMessage->m_dwPK_Device_To<0 )
			TranslateVirtualDevice(pMessage->m_dwPK_Device_To,pMessage->m_dwPK_Device_To);
		m_mapHardKeys[nKey] = pMessage;

		//create xml tokens
		string sXmlSoftKey;

		string sUrl = 
			nKey != BUTTON_F4_CONST ?
			(m_sRequestUrl + "key=" + StringUtils::ltos(nKey)) :
			(m_sBaseUrl + "ServicesMenu.php");

		sXmlSoftKey = 
			"<SoftKeyItem>\r\n"
			"\t<Name>" + sName + "</Name>\r\n"
			"\t<URL>" + sUrl + "</URL>\r\n"
			"\t<Position>" + StringUtils::ltos(nIndex++) + "</Position>\r\n"
			"</SoftKeyItem>\r\n";

		if(!bMediaPlaying)
		    sNoMediaSoftKeysXml += sXmlSoftKey;

		sAllSoftKeysXml += sXmlSoftKey;
	}
}
//-----------------------------------------------------------------------------------------------------
// copied from database - NULL fills the gaps
const char *ObjType[] = { NULL, "Bitmap", "Rectangle", "Array", "Broadcast Video", "App Desktop", "Datagrid", "Rectangle", "Web Browser", "Floorplan", "Pronto File",
	"wxWidgets Applet", "Scrolling Frame" };
const char *ObjParm[] = { NULL, "Graphic Filename", "X Position", "Y Position", NULL, NULL, NULL, "# of Columns", "# of Rows", "Fixed Row Height?", "Fixed Column Width", "PK_Array", "Down then Across", "Row Spacing", "Column Spacing", "More button / fwd", "More button / back", "PK_Device", "Width", "Height", NULL, NULL, NULL, NULL, NULL, NULL, "First Row Height", "First Column Width", "Keep Row Header?", "Keep Column Header", NULL, "Initial Row Number", "Initial Column Numb", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, "Is Multi Select", "PK_Variable", "Data grid ID", "Cycle Grid", NULL, NULL, "Source", NULL, "Options", "Selected Graphic Fil", "Alt Graphic File", NULL, "Persist XY Position", NULL, "Extra Info", "Page", "Type", "Pronto File", "Highlighted Graphic", "PK_Datagrid", "PK_Style", "PK_Style_FirstRow", "PK_Style_FirstColumn", "PK_Style_Selected", "PK_Style_Alt", "PK_Style_Highlighted", "PK_DeviceTemplate", "Seek Value", "Seek Column", "Auto Invalidate Time", "Crop X", "Crop Y", "PK_Effect_On_Screen", "PK_Effect_Off_Screen", "Custom Render", "In Background", "Alpha channel", "PK_DesignObj_Row1", "PK_DesignObj_Coll1", "PK_DesignObj_Cell", "Dont Scale", "Adjustments" };

static map<string, DataGridTable *> mapDatagridID_DataGridTable;
static map<string, DesignObj_DataGrid *> mapDatagridID_DesignObjDataGrid;

/*virtual*/ void Proxy_Orbiter::SaveBits(DesignObj_Orbiter *Data)
{
	DesignObj_Orbiter *DataRef = Data;
	static FILE *f = NULL;
	static int Level = 0;

	if (Data == NULL)
	{
		system(("rm -f " + m_sBitsDirectory + "/*").c_str());
		DataRef = m_pScreenHistory_Current->GetObj();
		f = fopen((m_sBitsDirectory + "/screen.xml").c_str(), "w");
		fprintf(f, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
		fprintf(f, "<screen>\n");
	}

	++Level;
#if 0
	fprintf(f, "> "
		"L=%d ObjID: objId=%s baseId=%d type=%d hidden=%d pos=%dx%d area=%dx%d"
		"\n", Level,
		DataRef->m_ObjectID.c_str(), DataRef->m_iBaseObjectID, DataRef->m_ObjectType, DataRef->IsHidden(),
		DataRef->m_rPosition.X, DataRef->m_rPosition.Y, DataRef->m_rPosition.Width, DataRef->m_rPosition.Height
	);
#endif
	if (!DataRef->IsHidden())
	{
		fprintf(f,
			"<object id=\"%s\" base=\"%d\" type=\"%s\" x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\">\n",
			DataRef->m_ObjectID.c_str(), DataRef->m_iBaseObjectID, ObjType[DataRef->m_ObjectType],
			DataRef->m_rPosition.X, DataRef->m_rPosition.Y, DataRef->m_rPosition.Width, DataRef->m_rPosition.Height
		);

		// save image
		if (DataRef->m_pvectCurrentGraphic != NULL && DataRef->m_pvectCurrentGraphic->size() > 0)
		{
			unsigned int CurrentFrame = DataRef->m_iCurrentFrame;
			if (CurrentFrame >= DataRef->m_pvectCurrentGraphic->size())
				CurrentFrame = 0;
			PlutoGraphic *pPlutoGraphic = (*DataRef->m_pvectCurrentGraphic)[CurrentFrame];
			string srcfn = "/usr/pluto/orbiter/C" + StringUtils::itos(m_dwPK_Device) + "/" + pPlutoGraphic->m_Filename;
			string dstfn = m_sBitsDirectory + "/" + DataRef->m_ObjectID + ".img";
			int fsrc = open(srcfn.c_str(), O_RDONLY);
			int fdst = open(dstfn.c_str(), O_WRONLY | O_CREAT, 0666);
			char buffer[4096];
			int bytes = 0;
			while ((bytes = read(fsrc, buffer, 4096)) > 0)
				write(fdst, buffer, bytes);
			close(fdst);
			close(fsrc);

			fprintf(f, "<image src=\"%s\"/>\n", srcfn.c_str());
		}

		fprintf(f, "<parameters>\n");
		map<int, string>::iterator ParmIt;
		for (ParmIt = DataRef->m_mapObjParms.begin(); ParmIt != DataRef->m_mapObjParms.end(); ParmIt++)
		{
			const char *ObjParmName = ObjParm[(*ParmIt).first];
			string S = StringUtils::itos((*ParmIt).first);
			if (ObjParmName == NULL)
				ObjParmName = S.c_str();
			if (strcmp(ObjParmName, "Page") == 0) // contains binary data (looks like a char value inside a string)
				continue;
			fprintf(f, "<parameter id=\"%s\" value=\"%s\" />\n", ObjParmName, SubstituteVariables((*ParmIt).second, DataRef, 0, 0).c_str());
		}
		fprintf(f, "</parameters>\n");

		fprintf(f, "<children>\n");
		DesignObj_DataList::iterator DataIt;
		for (DataIt = DataRef->m_ChildObjects.begin(); DataIt != DataRef->m_ChildObjects.end(); DataIt++)
		{
			DesignObj_Orbiter *Child = (DesignObj_Orbiter *) (*DataIt);
			SaveBits(Child);
		}
		fprintf(f, "</children>\n");

		fprintf(f, "</object>\n");

		if (DataRef->m_ObjectType == DESIGNOBJTYPE_Datagrid_CONST)
			SaveDataGrid(dynamic_cast<DesignObj_DataGrid *>(DataRef));
	}

	--Level;
#if 0
	fprintf(f, "< "
		"L=%d ObjID: objId=%s baseId=%d type=%d hidden=%d pos=%dx%d area=%dx%d"
		"\n", Level,
		DataRef->m_ObjectID.c_str(), DataRef->m_iBaseObjectID, DataRef->m_ObjectType, DataRef->IsHidden(),
		DataRef->m_rPosition.X, DataRef->m_rPosition.Y, DataRef->m_rPosition.Width, DataRef->m_rPosition.Height
	);
#endif

	if (Data == NULL)
	{
		fprintf(f, "</screen>\n");
		fclose(f);
	}
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Proxy_Orbiter::SaveDataGrid(DesignObj_DataGrid *pDesignObj_DataGrid)
{
	if (pDesignObj_DataGrid == NULL)
		return;

	if (pDesignObj_DataGrid->m_mapObjParms.find(DESIGNOBJPARAMETER_Data_grid_ID_CONST) == pDesignObj_DataGrid->m_mapObjParms.end())
		return;

	string sGridID = pDesignObj_DataGrid->m_mapObjParms[DESIGNOBJPARAMETER_Data_grid_ID_CONST];

	int data_size = 0;
	char *data = NULL;

	int MaxRow = 1000000 /* one million entries should be enough for everybody */, MaxCol = pDesignObj_DataGrid->m_MaxCol;
	int StartRow = 0, StartCol = 0;
	bool bKeepRowHeader = false, bKeepColHeader = false;
	bool bAdd_UpDown_Arrows = false;
	string sSeek = "";
	int iSeekColumn = 0;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "iOS: Saving datagrid: %s", sGridID.c_str());
	DCE::CMD_Request_Datagrid_Contents CMD_Request_Datagrid_Contents(m_dwPK_Device, m_dwPK_Device_DatagridPlugIn,
			StringUtils::itos(m_dwIDataGridRequestCounter), sGridID,
			MaxRow, MaxCol, bKeepRowHeader, bKeepColHeader, bAdd_UpDown_Arrows, sSeek, iSeekColumn, &data, &data_size, &StartRow, &StartCol);

	DataGridTable *pDataGridTable = NULL;
	if (SendCommand(CMD_Request_Datagrid_Contents) && data_size > 0 && data != NULL)
	{
		pDataGridTable = new DataGridTable(data_size, data);
		delete[] data;
		data = NULL;
	}
	else
	{
		pDataGridTable = new DataGridTable();
	}

	if (mapDatagridID_DataGridTable.find(sGridID) != mapDatagridID_DataGridTable.end())
		delete mapDatagridID_DataGridTable[sGridID];
	mapDatagridID_DataGridTable[sGridID] = pDataGridTable;
	mapDatagridID_DesignObjDataGrid[sGridID] = pDesignObj_DataGrid;

	string fname = m_sBitsDirectory + "/datagrid-" + sGridID + ".xml";
	FILE *f = fopen(fname.c_str(), "w");
	fprintf(f, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	fprintf(f, "<datagrid id=\"%s\" rows=\"%d\" cols=\"%d\">\n", sGridID.c_str(), pDataGridTable->GetRows(), pDataGridTable->GetCols());
	if (!pDesignObj_DataGrid->sSelVariable.empty())
	{
		PLUTO_SAFETY_LOCK(vm, pDesignObj_DataGrid->m_pOrbiter->m_VariableMutex)
		string sSelectedIndex = pDesignObj_DataGrid->m_pOrbiter->m_mapVariable[atoi(pDesignObj_DataGrid->sSelVariable.c_str())];
		fprintf(f, "<selected_index value=\"%s\"/>\n", sSelectedIndex.c_str());
	}
	for (int row = 0; row < pDataGridTable->GetRows(); row++)
	{
		fprintf(f, "<row>\n");
		for (int col = 0; col < pDataGridTable->GetCols(); col++)
		{
			DataGridCell *pCell = pDataGridTable->GetData(col, row);
			if (pCell != NULL)
			{
				fprintf(f, "<column>\n");
				fprintf(f, "<text>%s</text>\n", XMLEntities(pCell->m_Text).c_str());
				fprintf(f, "<value>%s</value>\n", XMLEntities(pCell->m_Value).c_str());
				if (pCell->m_pGraphic != NULL)
					fprintf(f, "<has_graphic/>\n");
				PlutoColor Color(pCell->m_AltColor);
				fprintf(f, "<alt_color value=\"#%02x%02x%02x\"/>\n", Color.R(), Color.G(), Color.B());
				if (pCell->m_ImagePath != NULL)
					fprintf(f, "<image src=\"%s\" />\n", XMLEntities(pCell->m_ImagePath).c_str());
				fprintf(f, "<parameters>\n");
				for (map<string, string>::iterator AttrIt = pCell->m_mapAttributes.begin(); AttrIt != pCell->m_mapAttributes.end(); AttrIt++)
				{
					string ParmName = (*AttrIt).first;
					string ParmValue = (*AttrIt).second;
					fprintf(f, "<parameter name=\"%s\" value=\"%s\" />\n", XMLEntities(ParmName).c_str(), XMLEntities(ParmValue).c_str());
				}
				fprintf(f, "</parameters>\n");
				fprintf(f, "</column>\n");
			}
			else
			{
				fprintf(f, "<column empty=\"empty\"/>\n");
			}
		}
		fprintf(f, "</row>\n");
	}
	fprintf(f, "</datagrid>\n");
	fclose(f);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "iOS: Saved datagrid: %s", sGridID.c_str());
}

//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Proxy_Orbiter::FireImageQualityChanged(unsigned long nImageQuality)
{
	m_ImageQuality = nImageQuality;
	DATA_Set_ImageQuality(nImageQuality, true);

	m_bRerenderScreen = true;
	m_pOrbiterRenderer->RedrawObjects();
}
//-----------------------------------------------------------------------------------------------------
void Proxy_Orbiter::ImageGenerated()
{
	m_nCurrentScreenId = m_pScreenHistory_Current->GetObj()->m_iBaseObjectID;
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Current screen generated: %d", m_nCurrentScreenId);

	PLUTO_SAFETY_LOCK(am, m_ActionMutex);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Image/xml generated. Wake up! Screen %s", 
		m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str());
	pthread_cond_broadcast(&m_ActionCond);
	m_bNews = true;

	if(m_iListenPort >= CISCO_LISTEN_PORT_START && m_iListenPort < CISCO_LISTEN_PORT_START + 10 && //only cisco orbiters
		!IsProcessingRequest() 
		)
	{
		if(!PendingCallbackScheduled((OrbiterCallBack)&Proxy_Orbiter::PushRefreshEventTask))
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Scheduling push refresh event to execute in 1000 ms");
			CallMaintenanceInMiliseconds(1000, (OrbiterCallBack)&Proxy_Orbiter::PushRefreshEventTask, NULL, pe_ALL);
		}
	}
}

bool Proxy_Orbiter::ReceivedString( Socket *pSocket, string sLine, int nTimeout )
{
	if(m_bQuit_get()|| m_bReload)
	{
		pSocket->SendString("NOT_CONNECTED");
		return false;
	}

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Received: %s", sLine.c_str());
	m_bPhoneRespondedToPush = true;

	PLUTO_SAFETY_LOCK(am, m_ActionMutex);
	
	if( sLine.substr(0,5)=="IMAGE" )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Current screen to analyze: %d", m_nCurrentScreenId);
		if(
			m_nCurrentScreenId == DESIGNOBJ_mnuFilelist_Video_Music_Small_CONST || 
			m_nCurrentScreenId == DESIGNOBJ_mnuSecurityPanelSmallUI_CONST ||
			m_nCurrentScreenId == DESIGNOBJ_mnuFileSaveSmallUI_CONST
		)
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "One of those screens. Sleeping 500 ms...");
			Sleep(500);
		}
		
		PLUTO_SAFETY_LOCK(rm, m_ResourcesMutex);
        
		size_t size;
		char *pBuffer = FileUtils::ReadFileIntoBuffer(GetDevicePngFileName(),size);
		if( !pBuffer )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Sent: ERROR");
			pSocket->SendString("ERROR"); // Shouldn't happen
			return true;
		}

		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Sent: IMAGE %d\\n\\n<IMAGE>", size);
		pSocket->SendString("IMAGE " + StringUtils::itos(int(size)));
		pSocket->SendData(int(size),pBuffer);
		delete[] pBuffer;

		m_bNews = false;
		return true;
	}
	else if( sLine.substr(0,3)=="XML" )
	{
		PLUTO_SAFETY_LOCK(rm, m_ResourcesMutex);
		
		size_t size;
		char *pBuffer = FileUtils::ReadFileIntoBuffer(GetDeviceXmlFileName(),size);
		if( !pBuffer )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Sent: ERROR");
			pSocket->SendString("ERROR"); // Shouldn't happen
			return true;
		}

		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Sent: XML %d\\n<XML>", size);
		pSocket->SendString("XML " + StringUtils::itos(int(size)));
		pSocket->SendData(int(size), pBuffer);
		delete[] pBuffer;

		EndProcessingRequest();
		
		m_bNews = false;
		return true;
	}
	else if( sLine.substr(0,9)=="PLUTO_KEY" && sLine.size()>10 )
	{
		int Key = atoi( sLine.substr(9).c_str() );
		if( Key )
		{
			if(BUTTON_F4_CONST != Key)
			{
				StartProcessingRequest();
				
				PROXY_ORBITER_MUTEX_SUSPEND(ButtonDown(Key));
				PROXY_ORBITER_MUTEX_SUSPEND(ButtonUp(Key));

				LoggerWrapper::GetInstance()->Write(LV_WARNING, "Waiting the image/xml to be generated...");        
				timespec abstime;
				abstime.tv_sec = (long) (time(NULL) + 2); //wait max 2 seconds
				abstime.tv_nsec = 0;
				am.TimedCondWait(abstime);

				if(m_bQuit_get()|| m_bReload)
				{
					pSocket->SendString("NOT_CONNECTED");
					return false;
				}

				LoggerWrapper::GetInstance()->Write(LV_WARNING, "Sent: OK");
				pSocket->SendString("OK");
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "F4 was sent. No need to kill proxy orbiter."); 
			}
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Sent: ERROR");        
			pSocket->SendString("ERROR"); // Shouldn't happen
		}
		return true;
	}
	else if( sLine.substr(0,5)=="TOUCH" && sLine.size()>6 )
	{
		StartProcessingRequest();
		
		int X = atoi(sLine.substr(6).c_str()),Y = 0;
		string::size_type pos_y = sLine.find('x');
		if( pos_y!=string::npos )
			Y = atoi(sLine.substr(pos_y+1).c_str());
		PROXY_ORBITER_MUTEX_SUSPEND(RegionDown(X,Y));

		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Waiting the image/xml to be generated...");        
		timespec abstime;
		abstime.tv_sec = (long) (time(NULL) + 2); //wait max 2 seconds
		abstime.tv_nsec = 0;
		am.TimedCondWait(abstime);

		if(m_bQuit_get()|| m_bReload)
		{
			pSocket->SendString("NOT_CONNECTED");
			return false;
		}

		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Sent: OK");        
		pSocket->SendString("OK");
		return true;
	}
	else if (sLine.substr(0, 8) == "ANYNEWS?")
	{
		if (m_bNews)
		{
			pSocket->SendString("NEWS " + StringUtils::itos(4));
			pSocket->SendString("yes");
		}
		else
		{
			pSocket->SendString("NEWS " + StringUtils::itos(3));
			pSocket->SendString("no");
		}
		return true;
	}
	else if (sLine.substr(0, 10) == "SCREEN_XML")
	{
		PLUTO_SAFETY_LOCK(rm, m_ResourcesMutex);
        
		size_t size;
		char *pBuffer = FileUtils::ReadFileIntoBuffer(m_sBitsDirectory + "/screen.xml", size);
		if (!pBuffer)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Sent: ERROR");
			pSocket->SendString("ERROR"); // Shouldn't happen
			return true;
		}

		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Sent: SCREEN_XML %d\\n\\n<SCREEN_XML>", size);
		pSocket->SendString("SCREEN_XML " + StringUtils::itos(int(size)));
		pSocket->SendData(int(size), pBuffer);
		delete[] pBuffer;

		m_bNews = false;
		return true;
	}
	else if (sLine.substr(0, 12) == "DATAGRID_XML")
	{
		// DATAGRID_XML <GridID>
		vector<string> vect_parameters;
		StringUtils::Tokenize(sLine, " ", vect_parameters);
		if (vect_parameters.size() != 2)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Sent: ERROR Wrong parameter count");
			pSocket->SendString("ERROR Wrong parameter count");
			return true;
		}
		string &sGridID = vect_parameters[1];

		size_t size;
		char *pBuffer = FileUtils::ReadFileIntoBuffer(m_sBitsDirectory + "/datagrid-" + sGridID + ".xml", size);
		if (!pBuffer)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Sent: ERROR");
			pSocket->SendString("ERROR"); // Shouldn't happen
			return true;
		}

		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Sent: DATAGRID_XML %d\\n\\n<DATAGRID_XML>", size);
		pSocket->SendString("DATAGRID_XML " + StringUtils::itos(int(size)));
		pSocket->SendData(int(size), pBuffer);
		delete[] pBuffer;

		return true;
	}
	else if (sLine.substr(0, 14) == "DATAGRID_TOUCH")
	{
		// DATAGRID_TOUCH <GridID> <Row> <Col>
		vector<string> vect_parameters;
		StringUtils::Tokenize(sLine, " ", vect_parameters);
		if (vect_parameters.size() != 4)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Sent: ERROR Wrong parameter count");
			pSocket->SendString("ERROR Wrong parameter count");
			return true;
		}
		string &sGridID = vect_parameters[1];
		int Row = atoi(vect_parameters[2].c_str());
		int Col = atoi(vect_parameters[3].c_str());
		
		if (mapDatagridID_DataGridTable.find(sGridID) == mapDatagridID_DataGridTable.end())
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Sent: ERROR Grid %s not found", sGridID.c_str());
			pSocket->SendString(StringUtils::Format("ERROR Grid %s not found", sGridID.c_str()));
			return true;
		}
		DataGridTable *pDataGridTable = mapDatagridID_DataGridTable[sGridID];
		DesignObj_DataGrid *pDesignObj_DataGrid = mapDatagridID_DesignObjDataGrid[sGridID];
		DataGridCell *pCell = pDataGridTable->GetData(Col, Row);
		if (pCell == NULL)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Sent: ERROR Unknown location row=%d,col=%d in grid %s", Row, Col, sGridID.c_str());
			pSocket->SendString(StringUtils::Format("ERROR Unknown location row=%d,col=%d in grid %s", Row, Col, sGridID.c_str()));
			return true;
		}

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Calling SelectedGrid");
		PROXY_ORBITER_MUTEX_SUSPEND(SelectedGrid(pDesignObj_DataGrid, pCell, smCommand, Row, Col));

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Executing Commands in ZoneList: %d commands", pDesignObj_DataGrid->m_ZoneList.size());
		PROXY_ORBITER_MUTEX_SUSPEND(
		DesignObjZoneList::iterator iZone;
		for (iZone = pDesignObj_DataGrid->m_ZoneList.begin(); iZone != pDesignObj_DataGrid->m_ZoneList.end(); ++iZone)
		{
			DesignObjZone *pDesignObjZone = *iZone;
			ExecuteCommandsInList(&pDesignObjZone->m_Commands, pDesignObj_DataGrid, smCommand);
		}
		);

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Finished DATAGRID_TOUCH");

		pSocket->SendString("OK");

		return true;
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Not a valid communication tokey. Check the specs. Sending error...");        
		pSocket->SendString("ERROR"); // Shouldn't happen
	}
	return false;
}

void *Proxy_Orbiter::PushRefreshEventTask(void *)
{
	PushRefreshEvent(false, true);
	return NULL;
}

void Proxy_Orbiter::CMD_Quit(string &sCMD_Result,Message *pMessage)
{
	m_bQuit_set(true);
	pthread_cond_broadcast(&m_ActionCond);
}

void Proxy_Orbiter::CMD_Terminate_Orbiter(string &sCMD_Result,Message *pMessage)
{
	m_bQuit_set(true);
	pthread_cond_broadcast(&m_ActionCond);
}

void Proxy_Orbiter::CMD_Regen_Screen(string &sCMD_Result,Message *pMessage)
{
	Orbiter::CMD_Regen_Screen(sCMD_Result, pMessage);
	m_bQuit_set(true);
	pthread_cond_broadcast(&m_ActionCond);
}

void Proxy_Orbiter::OnReload()
{
	m_bReload = true;
	m_bQuit_set(true);
	
	Orbiter_Command::OnReload(); 
	pthread_cond_broadcast(&m_ActionCond);
}

bool Proxy_Orbiter::IsRepeatedKeyForScreen(DesignObj_Orbiter* pObj, int iPK_Button)
{
	//we don't have a key up and a key down event for web orbiter or small ui
	//using repeated keys can be dangerous, since the command attached to the object
	//with repeated key enabled will be send forever.

	//no repeated keys for small ui or web orbiter
	return false;
}

void LoadUI_From_ConfigurationData()
{
}

extern void (*g_pDeadlockHandler)(PlutoLock *pPlutoLock);
extern void (*g_pSocketCrashHandler)(Socket *pSocket);
extern Command_Impl *g_pCommand_Impl;
void DeadlockHandler(PlutoLock *pPlutoLock);
void SocketCrashHandler(Socket *pSocket);

bool StartOrbiter(class BDCommandProcessor *pBDCommandProcessor, int DeviceID, 
			int PK_DeviceTemplate, string ServerAddress)
{
	Proxy_Orbiter *pProxy_Orbiter = new Proxy_Orbiter(DeviceID, 0, ServerAddress);
	if ( pProxy_Orbiter->GetConfig() && pProxy_Orbiter->Connect(pProxy_Orbiter->PK_DeviceTemplate_get()) ) 
	{
		pProxy_Orbiter->Run();
		g_pCommand_Impl=pProxy_Orbiter;
		g_pDeadlockHandler=DeadlockHandler;
		g_pSocketCrashHandler=SocketCrashHandler;
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Connect OK");
		pProxy_Orbiter->CreateChildren();
		pthread_join(pProxy_Orbiter->m_RequestHandlerThread, NULL);  // This function will return when the device is shutting down
		g_pDeadlockHandler=NULL;
		g_pSocketCrashHandler=NULL;
	} 
	else 
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Connect() Failed");
	}

	bool bReload = pProxy_Orbiter->m_bReload;
	delete pProxy_Orbiter;
	return bReload;
}
