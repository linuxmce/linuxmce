#include "LCDManager.h"
#include "MenuHolder.h"
#include "MenuItem.h"
#include "MenuItemAction.h"
//--------------------------------------------------------------------------------------------------------
#include "pluto_main/Table_Device.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "PlutoUtils/ProcessUtils.h"
//--------------------------------------------------------------------------------------------------------
#define REFRESH_INTERVAL	400
#define IDLE_INTERVAL		100
//--------------------------------------------------------------------------------------------------------
LCDManager::LCDManager(MenuHolder *pMenuHolder, Database_pluto_main *pDatabase_pluto_main) : 
	IInputProcessor(), m_pMenuHolder(pMenuHolder), m_RenderMutex("render"), 
	m_RerendererThreadID(0), m_ulRendererInterval(0), m_bQuit(false),
	m_pDatabase_pluto_main(pDatabase_pluto_main)
{
	m_display_state.m_sStatusMessage = "Welcome to Fiire 1.0!";
	m_RenderMutex.Init(NULL);

	if(NULL != m_pMenuHolder)
	{
		Expand(m_pMenuHolder->RootMenu());
		SetupCoreNodes();
		LoadNetworkInfo();
	}
}
//--------------------------------------------------------------------------------------------------------
LCDManager::~LCDManager()
{
	m_bQuit = true;

	if(m_RerendererThreadID)
		pthread_join(m_RerendererThreadID, 0);

	pthread_mutex_destroy(&m_RenderMutex.mutex);
}
//--------------------------------------------------------------------------------------------------------
void LCDManager::AddRenderer(IRenderer *pRenderer)
{
	if(NULL != pRenderer)
	{
		PLUTO_SAFETY_LOCK(sm, m_RenderMutex);
		m_listRenderer.push_back(pRenderer);

		Render();
	}
}
//--------------------------------------------------------------------------------------------------------
bool LCDManager::ProcessInput(const Input &input)
{
	PLUTO_SAFETY_LOCK(sm, m_RenderMutex);

	if(NULL == m_pMenuHolder)
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Got not menu holder. The input will be ignored");
		return false;
	}

	//process the input
	switch(input.type)
	{
		case itKeyboardInput:
		{
			DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING, "ProcessInput key %d", input.keyboardinput);
			
			{
				switch(input.keyboardinput)
				{
					case kiUp :		
						m_pMenuHolder->MoveUp();
						break;
					case kiDown :	
						m_pMenuHolder->MoveDown();
						break;
					case kiLeft :	
						m_pMenuHolder->MoveLeft();
						break;
					case kiRight :	
						m_pMenuHolder->MoveRight();
						break;
					default :	
						break;
				}
			}
		}
		break;

		case itChangeStatus:
			m_pMenuHolder->Home();
			m_display_state.m_sStatusMessage = input.status;
			m_display_state.m_sProgress = input.progress;
		break;

		default:
			break;
	}

	Render();

	return true;
}
//--------------------------------------------------------------------------------------------------------
void LCDManager::Render()
{
	CancelRerender();

	Prepare();

	for(list<IRenderer *>::iterator it = m_listRenderer.begin(), end = m_listRenderer.end(); it != end; ++it)
		(*it)->Render(m_display_state);
}
//--------------------------------------------------------------------------------------------------------
void LCDManager::Prepare()
{
	if(NULL == m_pMenuHolder || NULL == m_pMenuHolder->CurrentMenuItem())
	{
		m_display_state.m_sHeader = "Got no menu";
		return;
	}

	switch(m_pMenuHolder->CurrentMenuItem()->Type())
	{
		case itStatusItem:
		{
			m_display_state.m_sHeader = m_display_state.m_sStatusMessage;
		}
		break;

		case itListItem:
		{
			m_display_state.m_sHeader = ListItemDescription(m_pMenuHolder->CurrentMenuItem());
		}
		break;

		case itInputBox:
		{
			m_display_state.m_sHeader = m_pMenuHolder->CurrentMenuItem()->Description();
			m_display_state.m_sStatusMessage = m_pMenuHolder->CurrentMenuItem()->Status();
			ScheduleRerender(REFRESH_INTERVAL);
		}
		break;

		default:
		break;
	}

	//parent
	if(NULL != m_pMenuHolder->CurrentMenuItem()->Parent())
		m_display_state.m_sStatusMessage = ListItemDescription(m_pMenuHolder->CurrentMenuItem()->Parent());

	//navigation
	m_display_state.m_bCanGoDown = m_pMenuHolder->CurrentMenuItem()->CanGoDown();
	m_display_state.m_bCanGoUp = m_pMenuHolder->CurrentMenuItem()->CanGoUp();
	m_display_state.m_bCanGoLeft = m_pMenuHolder->CurrentMenuItem()->CanGoLeft();
	m_display_state.m_bCanGoRight = m_pMenuHolder->CurrentMenuItem()->CanGoRight();

	//reset last state
	m_display_state.m_vectNextOptions.clear();
	m_display_state.m_vectPriorOptions.clear();

	//prev options
	int nIndex = ADIACENT_OPTIONS_NUMBER;
	MenuItem *pCurrentItem = m_pMenuHolder->CurrentMenuItem();
	while(NULL != pCurrentItem && nIndex-- > 0 && NULL != pCurrentItem->Parent())
	{
		MenuItem *pPrevItem = pCurrentItem->Parent()->PrevChild(pCurrentItem);
		if(NULL != pPrevItem)
		{
            m_display_state.m_vectPriorOptions.push_back(ListItemDescription(pPrevItem));
			pCurrentItem = pPrevItem;
		}
		else
			break;
	}

	//next options
	nIndex = ADIACENT_OPTIONS_NUMBER;
	pCurrentItem = m_pMenuHolder->CurrentMenuItem();
	while(NULL != pCurrentItem && nIndex-- > 0 && NULL != pCurrentItem->Parent())
	{
		MenuItem *pNextItem = pCurrentItem->Parent()->NextChild(pCurrentItem);
		if(NULL != pNextItem)
		{
            m_display_state.m_vectNextOptions.push_back(ListItemDescription(pNextItem));
			pCurrentItem = pNextItem;
		}
		else
			break;
	}
}
//--------------------------------------------------------------------------------------------------------
void *RerenderThread(void *p)
{
	LCDManager *pManager = reinterpret_cast<LCDManager *>(p);

	while(NULL != pManager && !pManager->Quit())
	{
		if(pManager->RerenderInterval())
		{
			pManager->Render(); 
			Sleep(pManager->RerenderInterval());
		}
		else
		{
            Sleep(IDLE_INTERVAL);
		}
	}

	return NULL;
}
//--------------------------------------------------------------------------------------------------------
unsigned long LCDManager::RerenderInterval()
{
	return m_ulRendererInterval;
}
//--------------------------------------------------------------------------------------------------------
bool LCDManager::Quit()
{
	return m_bQuit;
}
//--------------------------------------------------------------------------------------------------------
void LCDManager::CancelRerender()
{
	m_ulRendererInterval = 0;
}
//--------------------------------------------------------------------------------------------------------
void LCDManager::ScheduleRerender(unsigned long ulMiliseconds)
{
	m_ulRendererInterval = ulMiliseconds;

	if(!m_RerendererThreadID)
		pthread_create(&m_RerendererThreadID, NULL, RerenderThread, (void *)this);
}
//--------------------------------------------------------------------------------------------------------
void LCDManager::SetVariable(string sVar, string sValue)
{
	m_mapVariables[sVar] = sValue;
}
//--------------------------------------------------------------------------------------------------------
string LCDManager::ListItemDescription(MenuItem *pMenuItem)
{
	if(!pMenuItem->Value().empty())
		return pMenuItem->Description() + ": " + m_mapVariables[pMenuItem->Value()];

	return pMenuItem->Description();
}
//--------------------------------------------------------------------------------------------------------
bool LCDManager::Expand(MenuItem *pMenuItem)
{
	if(NULL != pMenuItem)
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "Analyzing item %s type %d", 
			pMenuItem->Description().c_str(), pMenuItem->Type());

		if(pMenuItem->Type() == itExpandItem && NULL != pMenuItem->Parent())
		{
			if(pMenuItem->Value() == "{ORBITER}")
			{
				GenerateOrbiterNodes(pMenuItem);
				return true;
			}
			else if(pMenuItem->Value() == "{MD}")
			{
				GenerateMDNodes(pMenuItem);
				return true;
			}
		}
		else
		{
			for(vector<MenuItem *>::const_iterator it = pMenuItem->Children().begin(); it != pMenuItem->Children().end(); ++it)
			{
				MenuItem *pChild = *it;
				if(Expand(pChild))
					return false;
			}
		}
	}

	return false;
}
//--------------------------------------------------------------------------------------------------------
void LCDManager::ShowStatus(string sStatus, int /*nType*/)
{
	if(NULL != m_pMenuHolder)
	{
		m_pMenuHolder->Home();
		m_display_state.m_sStatusMessage = sStatus;
	}

	Render();
}
//--------------------------------------------------------------------------------------------------------
void LCDManager::RestoreState()
{
	if(NULL != m_pMenuHolder)
		m_pMenuHolder->Restore();

	Render();	
}
//--------------------------------------------------------------------------------------------------------
void LCDManager::GenerateOrbiterNodes(MenuItem *pMenuItem)
{
	if(NULL == m_pDatabase_pluto_main)
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Don't have a mysql connection!");
		return;
	}

	vector<Row_Device *> vectRow_Device;
	string sql = 
		"JOIN DeviceTemplate ON FK_DeviceTemplate = PK_DeviceTemplate WHERE FK_DeviceCategory IN ( " +
		StringUtils::ltos(DEVICECATEGORY_Mobile_Orbiter_CONST) + ", " + 
		StringUtils::ltos(DEVICECATEGORY_Standard_Orbiter_CONST) + ")";

	m_pDatabase_pluto_main->Device_get()->GetRows(sql, &vectRow_Device);
	list<MenuItem *> listExpandedItems;

	//for each Orbiter, create clones and replace 
	for(vector<Row_Device *>::iterator it = vectRow_Device.begin(); it != vectRow_Device.end(); ++it)
	{
		Row_Device *pRow_Device = *it;
		string sOrbiter = StringUtils::ltos(pRow_Device->PK_Device_get());

		MenuItemAction *pAction = pMenuItem->Action()->Clone();
		pAction->UpdateValueParam(sOrbiter);
		listExpandedItems.push_back(new MenuItem("Regen orbiter " + sOrbiter, pMenuItem->Parent(), itListItem, pAction));
	}

	pMenuItem->Parent()->Expand(pMenuItem, listExpandedItems);
}
//--------------------------------------------------------------------------------------------------------
void LCDManager::GenerateMDNodes(MenuItem *pMenuItem)
{
	//get audio and video settings nodes
	MenuItem *pAudioSettingsNode = m_pMenuHolder->GetTopChildNode("Audio settings");
	MenuItem *pVideoSettingsNode = m_pMenuHolder->GetTopChildNode("Video settings");

	if(NULL == pAudioSettingsNode || NULL == pVideoSettingsNode)
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Didn't have audio and video settings nodes!");
		return;
	}

	if(NULL == m_pDatabase_pluto_main)
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Don't have a mysql connection!");
		return;
	}

	vector<Row_Device *> vectRow_Device;
	string sql = 
		"JOIN DeviceTemplate ON FK_DeviceTemplate = PK_DeviceTemplate WHERE FK_DeviceCategory = " +
		StringUtils::ltos(DEVICECATEGORY_Media_Director_CONST);
	m_pDatabase_pluto_main->Device_get()->GetRows(sql, &vectRow_Device);

	list<MenuItem *> listExpandedItems;

	//for each MD, create clones and replace 
	for(vector<Row_Device *>::iterator it = vectRow_Device.begin(); it != vectRow_Device.end(); ++it)
	{
		Row_Device *pRow_Device = *it;
		string sMD = StringUtils::ltos(pRow_Device->PK_Device_get());

		vector<Row_Device *> vectDevices;
		m_pDatabase_pluto_main->Device_get()->GetRows(
			"WHERE FK_DeviceTemplate = " + StringUtils::ltos(DEVICETEMPLATE_App_Server_CONST) + " "
			"AND FK_Device_ControlledVia = " + sMD,
			&vectDevices);

		if(!vectDevices.empty())
		{
			long AppServerID = vectDevices[0]->PK_Device_get();
			MenuItem *pMDMenuItem = new MenuItem("MD " + sMD, pMenuItem->Parent(), 
				itListItem, pMenuItem->Action()->Clone());

			pMDMenuItem->AddChild(pVideoSettingsNode->Clone());
			pMDMenuItem->AddChild(pAudioSettingsNode->Clone());
			pMDMenuItem->ReplaceVariable("{APP_SERVER}", StringUtils::ltos(AppServerID));
			listExpandedItems.push_back(pMDMenuItem);
		}
	}

	pMenuItem->Parent()->Expand(pMenuItem, listExpandedItems);
}
//--------------------------------------------------------------------------------------------------------
void LCDManager::SetupCoreNodes()
{
	//get audio and video settings nodes
	MenuItem *pAudioSettingsNode = m_pMenuHolder->GetTopChildNode("Audio settings");
	MenuItem *pVideoSettingsNode = m_pMenuHolder->GetTopChildNode("Video settings");

	if(NULL == pAudioSettingsNode || NULL == pVideoSettingsNode)
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Didn't have audio and video settings nodes!");
		return;
	}

	if(NULL == m_pDatabase_pluto_main)
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Don't have a mysql connection!");
		return;
	}

	vector<Row_Device *> vectDevices;
	m_pDatabase_pluto_main->Device_get()->GetRows(
		"JOIN DeviceTemplate ON FK_DeviceTemplate = PK_DeviceTemplate "
		"WHERE FK_DeviceCategory = " + StringUtils::ltos(DEVICECATEGORY_Core_CONST),
		&vectDevices);

	long CoreID = 0;
	if(!vectDevices.empty())
		CoreID = vectDevices[0]->PK_Device_get();

	vectDevices.clear();

	m_pDatabase_pluto_main->Device_get()->GetRows(
		"WHERE FK_DeviceTemplate = " + StringUtils::ltos(DEVICETEMPLATE_App_Server_CONST) + " "
		"AND FK_Device_ControlledVia = " + StringUtils::ltos(CoreID),
		&vectDevices);

	string sAppServerID;
	if(!vectDevices.empty())
	{
		long AppServerID = vectDevices[0]->PK_Device_get();
		sAppServerID = StringUtils::ltos(AppServerID);
	}

	pAudioSettingsNode->ReplaceVariable("{APP_SERVER}", sAppServerID);
	pVideoSettingsNode->ReplaceVariable("{APP_SERVER}", sAppServerID);
}
//--------------------------------------------------------------------------------------------------------
void LCDManager::LoadNetworkInfo()
{
	MenuItem* pMenuItem = m_pMenuHolder->GetChildNode("Network Settings", "View Settings");

	if(NULL == pMenuItem || NULL == pMenuItem->Action())
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Network Settings -> View Settings node is missing");
		return;
	}

	string sNetworkInfo;
	string sDisplayNetworkSettings = pMenuItem->Action()->Description();

	if(!ProcessUtils::RunApplicationAndGetOutput(sDisplayNetworkSettings, sNetworkInfo))
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Got now info from %s", sDisplayNetworkSettings.c_str());
		return;
	}

	//get rid of any \r
	sNetworkInfo = StringUtils::Replace(sNetworkInfo, "\r\n", "\n");

	vector<string> vectLines;
	StringUtils::Tokenize(sNetworkInfo, "\n=", vectLines);

	for(vector<string>::iterator it = vectLines.begin(); it != vectLines.end(); ++it)
	{
		string sValue;
		string sVariable = *it;

		++it;

		if(it != vectLines.end())
		{
			sValue = *it;
			SetVariable("{"  + sVariable + "}", sValue);
		}
		else
			break;
	}
}
//--------------------------------------------------------------------------------------------------------
