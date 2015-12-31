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
#include "PopulateListsInVMC.h"
//------------------------------------------------------------------------------------------------------
#include "Orbiter_Plugin.h"
#include "DCE/Logger.h"
#include "VIPShared/VIPIncludes.h"
#include "VIPShared/VIPMenu.h"
#include "PlutoUtils/PlutoDefs.h"
#include "pluto_main/Table_Array.h"
#include "pluto_main/Table_CommandGroup.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Define_Button.h"
#include "pluto_main/Define_DeviceCategory.h"
//------------------------------------------------------------------------------------------------------
#define liStandardScenarios 1
#define liSetHouseMode 1
#define liViewCameras 1
#define liSpeechDevices 2
//------------------------------------------------------------------------------------------------------
PopulateListsInVMC::PopulateListsInVMC(string sSourceTemplateVMC, string sDestinationVMC, long dwPKDevice,
						Database_pluto_main *pDatabase_pluto_main, long dwInstallation)
{
    m_pMenuCollection = NULL;
    m_pDatabase_pluto_main = pDatabase_pluto_main;
    m_sSourceTemplateVMC = sSourceTemplateVMC;
    m_sDestinationVMC = sDestinationVMC;
    m_dwPKDevice = dwPKDevice;
    m_dwInstallation = dwInstallation;

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "<VMC> Ready to create VMC file '%s' from template '%s' file for device %d", 
        m_sSourceTemplateVMC.c_str(), m_sDestinationVMC.c_str(), m_dwPKDevice);
}
//------------------------------------------------------------------------------------------------------
void PopulateListsInVMC::SaveIt()
{
    if(!m_pMenuCollection)
        return;

    //save the vmc file
    if(!PopulateListsInVMC::SaveMenuCollection(m_pMenuCollection, m_sDestinationVMC))
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Unable to save %s!", m_sDestinationVMC.c_str());

    PLUTO_SAFE_DELETE(m_pMenuCollection);
}
//------------------------------------------------------------------------------------------------------
bool PopulateListsInVMC::DoIt()
{
    //check if the file exists
    if(!FileUtils::FileExists(m_sSourceTemplateVMC))
    {
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "File %s not found!", m_sSourceTemplateVMC.c_str());
        return false;
    }

    //load the template vmc file
    m_pMenuCollection = PopulateListsInVMC::LoadMenuCollection(m_sSourceTemplateVMC);

    //do it!
    PopulateStandardScenariosList();
    PopulateCamerasList();
    PopulateSpeechDevicesList();

    SaveIt();

    return true;
}
//------------------------------------------------------------------------------------------------------
/*static*/ VIPMenuCollection *PopulateListsInVMC::LoadMenuCollection(string sVMCFile)
{
	size_t iSize = 0;
	char *pData = FileUtils::ReadFileIntoBuffer(sVMCFile, iSize);

	if(!pData)
		return NULL;

	VIPMenuCollection *pMenuCollection = new VIPMenuCollection(long(iSize), pData);
	delete[] pData;

	return pMenuCollection;
}
//------------------------------------------------------------------------------------------------------
/*static*/ bool PopulateListsInVMC::SaveMenuCollection(VIPMenuCollection *pMenuCollection, string sVMCFile)
{
	if( !pMenuCollection->ConvertToBinary() || pMenuCollection->m_pBinary==NULL || pMenuCollection->m_iBinarySize==0 )
		return false;

	return FileUtils::WriteBufferIntoFile(sVMCFile, pMenuCollection->m_pBinary,
		pMenuCollection->m_iBinarySize);
}
//------------------------------------------------------------------------------------------------------
VIPMenu *PopulateListsInVMC::GetMenu(MenuIds MenuID, const string& csDescription)
{
    //get menu page
    VIPMenu *pMenu = m_pMenuCollection->m_mapMenus_Find(MenuID);
    if(!pMenu)
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, 
            "Couldn't get '%s' menu with id %d in vmc file", 
            csDescription.c_str(), MenuID);
        return NULL;
    }

    return pMenu;
}
//------------------------------------------------------------------------------------------------------
VIPMenuElement_List *PopulateListsInVMC::GetListElement(MenuIds MenuID, long dwListId, const string& csDescription)
{
    //get menu page
    VIPMenu *pMenu = GetMenu(MenuID, csDescription);
    if(!pMenu)
        return NULL;

    //get the list object from the menu
    VIPMenuElement *pElement = pMenu->m_mapMenuElements_Find(dwListId);
    if(!pElement)
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, 
            "Couldn't get the object with id %d from '%s' menu from vmc template file.", 
            dwListId, csDescription.c_str());
        return NULL;
    }

    VIPMenuElement_List *pList = NULL;
    if(pElement->MenuElementID() == MENU_ELEMENT_LIST)
        pList = (VIPMenuElement_List *)pElement;
    else
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, 
            "The object with id %d '%s' menu is not a list.", liStandardScenarios, csDescription.c_str());
        return NULL;
    }

    return pList;
}
//------------------------------------------------------------------------------------------------------
bool PopulateListsInVMC::PopulateStandardScenariosList()
{
    const string csMenuDescription = "Standard Scenarios";

    VIPMenu *pMenu = GetMenu(miStandardScenarios, csMenuDescription);

    if(!pMenu)
        return false;

    VIPMenuElement_List *pList = GetListElement(miStandardScenarios, liStandardScenarios, csMenuDescription);

    if(!pList)
        return false; //list element not found

    //get all mobile orbiter scenarios
	vector<Row_CommandGroup *> vectRow_CommandGroup;
	m_pDatabase_pluto_main->CommandGroup_get()->GetRows(
        "FK_Array = " + StringUtils::ltos(ARRAY_Mobile_Orbiter_Scenarios_CONST) + " AND " + 
        "FK_Installation = " + StringUtils::ltos(m_dwInstallation),
		&vectRow_CommandGroup);

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "<VMC> About to populate the list with standard scenarios, items %d", 
        vectRow_CommandGroup.size());

	int iIndex = 0;
	pList->m_sText = "";
    vector<Row_CommandGroup *>::iterator it;
	for(it = vectRow_CommandGroup.begin(); it != vectRow_CommandGroup.end(); it++)
	{
        Row_CommandGroup* pRow_CommandGroup = *it;

        long dwPK_CommandGroup = pRow_CommandGroup->PK_CommandGroup_get();
        string sDescription = pRow_CommandGroup->Description_get();
        int iKeyCode = BUTTON_1_CONST + iIndex;
		string sID = StringUtils::ltos(dwPK_CommandGroup);

		pList->m_sText += sDescription + "\r\n";
		VIPMenuResolution *pMenuResolution = new VIPMenuResolution("",0,"","", 0,0,0,0,0,"");
		pMenuResolution->m_sProgramName = "cgid=" + sID;
		pMenuResolution->m_sTerminatingKey += char(iKeyCode);

        LoggerWrapper::GetInstance()->Write(LV_STATUS, 
            "<VMC> Added '%s' in the list with standard scenarios, path '%s', key %d", 
            sDescription.c_str(), pMenuResolution->m_sProgramName.c_str(), iKeyCode);

		pMenu->AddResolution(pMenuResolution);
		iIndex++;
	}
	vectRow_CommandGroup.clear();
    return true;
}
//------------------------------------------------------------------------------------------------------
bool PopulateListsInVMC::PopulateCamerasList()
{
    const string csMenuDescription = "View cameras";

    VIPMenu *pMenu = GetMenu(miViewCameras, csMenuDescription);

    if(!pMenu)
        return false;

    VIPMenuElement_List *pList = GetListElement(miViewCameras, liViewCameras, csMenuDescription);

    if(!pList)
        return false; //list element not found
  
    //get all device templates from 'surveillance cameras' category
    vector<Row_DeviceTemplate *> vectRow_DeviceTemplate;
    m_pDatabase_pluto_main->DeviceTemplate_get()->GetRows(
        "FK_DeviceCategory IN (" + StringUtils::ltos(DEVICECATEGORY_Surveillance_Cameras_CONST) + "," + 
        StringUtils::ltos(DEVICECATEGORY_IP_Cameras_CONST) + ")",
        &vectRow_DeviceTemplate);

    if(!vectRow_DeviceTemplate.size()) 
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "<VMC> No device templates found for %d ('surveillance cameras' category)",
            DEVICECATEGORY_Surveillance_Cameras_CONST);
        return false;
    }

    //create a comma separated string with our device templates
    string sDeviceTemplatesCommaSeparated;
    vector<Row_DeviceTemplate *>::iterator itDeviceTemplate = vectRow_DeviceTemplate.begin();
    for(; itDeviceTemplate != vectRow_DeviceTemplate.end(); itDeviceTemplate++)
    {
        if(itDeviceTemplate != vectRow_DeviceTemplate.begin())
            sDeviceTemplatesCommaSeparated += ", ";

        Row_DeviceTemplate *pDeviceTemplate = *itDeviceTemplate;
        sDeviceTemplatesCommaSeparated += StringUtils::ltos(pDeviceTemplate->PK_DeviceTemplate_get()); 
    }
    vectRow_DeviceTemplate.clear();

    //get all devices from the current installation which have the device template we need
    vector<Row_Device *> vectRow_Device;
    m_pDatabase_pluto_main->Device_get()->GetRows(
        "FK_Installation = " + StringUtils::ltos(m_dwInstallation) + " AND " +
        "FK_DeviceTemplate IN ( " +  sDeviceTemplatesCommaSeparated + " ) ",
        &vectRow_Device);

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "<VMC> About to populate the list with cameras, items %d", 
        vectRow_Device.size());    

    int iIndex = 0;
    pList->m_sText = "";

    vector<Row_Device *>::iterator itDevices = vectRow_Device.begin();
    for(; itDevices != vectRow_Device.end(); itDevices++)
    {
        Row_Device *pDevice = *itDevices;

        long dwPK_Device = pDevice->PK_Device_get();
        string sDeviceName = pDevice->Description_get();
        string sDescription = "View " + sDeviceName;
        string sID = StringUtils::ltos(dwPK_Device);

        int iKeyCode = BUTTON_1_CONST + iIndex;

        pList->m_sText += sDescription + "\r\n";
        VIPMenuResolution *pMenuResolution = new VIPMenuResolution("",0,"","", 0,0,0,0,0,"");
        pMenuResolution->m_sProgramName = "cameraid=" + sID;
        pMenuResolution->m_sTerminatingKey += char(iKeyCode);

        LoggerWrapper::GetInstance()->Write(LV_STATUS, 
            "<VMC> Added '%s' in the list with cameras, url '%s', key %d", 
            sDescription.c_str(), pMenuResolution->m_sProgramName.c_str(), iKeyCode);

        pMenu->AddResolution(pMenuResolution);
        iIndex++;
    }
    vectRow_Device.clear();
    return true;
}
//------------------------------------------------------------------------------------------------------
bool PopulateListsInVMC::PopulateSpeechDevicesList()
{
    const string csMenuDescription = "Speak in the house";

    VIPMenu *pMenu = GetMenu(miSpeakInHouse, csMenuDescription);

    if(!pMenu)
        return false;

    VIPMenuElement_List *pList = GetListElement(miSpeakInHouse, liSpeechDevices, csMenuDescription);

    if(!pList)
        return false; //list element not found

    //get all mobile orbiter scenarios
    vector<Row_Device *> vectRow_Device;
    m_pDatabase_pluto_main->Device_get()->GetRows(
        "FK_DeviceTemplate = " + StringUtils::ltos(DEVICETEMPLATE_Speech_CONST) + " AND " + 
        "FK_Installation = " + StringUtils::ltos(m_dwInstallation),
        &vectRow_Device);

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "<VMC> About to populate the list with speech devices, items %d", 
        vectRow_Device.size());

    int iIndex = 0;
    pList->m_sText = "";
    vector<Row_Device *>::iterator it;
    for(it = vectRow_Device.begin(); it != vectRow_Device.end(); it++)
    {
        Row_Device* pRow_Device = *it;

        long dwPK_Device = pRow_Device->PK_Device_get();
        string sDescription = pRow_Device->Description_get();
        int iKeyCode = BUTTON_1_CONST + iIndex;
        string sID = StringUtils::ltos(dwPK_Device);

        pList->m_sText += sDescription + "\r\n";
        VIPMenuResolution *pMenuResolution = new VIPMenuResolution("",0,"","", 0,0,0,0,0,"");
        pMenuResolution->m_sProgramName = "speechid=" + sID;
        pMenuResolution->m_sTerminatingKey += char(iKeyCode);

        LoggerWrapper::GetInstance()->Write(LV_STATUS, 
            "<VMC> Added '%s' in the list with speech devices, path '%s', key %d", 
            sDescription.c_str(), pMenuResolution->m_sProgramName.c_str(), iKeyCode);

        pMenu->AddResolution(pMenuResolution);
        iIndex++;
    }
    vectRow_Device.clear();
    return true;
}
//------------------------------------------------------------------------------------------------------