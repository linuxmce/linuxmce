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
#ifndef __POPULATE_LIST_IN_VMC_H__
#define __POPULATE_LIST_IN_VMC_H__
//------------------------------------------------------------------------------------------------------
#include <string>
using namespace std;
//------------------------------------------------------------------------------------------------------
enum MenuIds 
{ 
    miMain, 
    miStandardScenarios, 
    miSetHouseMode, 
    miChooseMode, 
    miSpeakInHouse, 
    miViewCameras 
};
//------------------------------------------------------------------------------------------------------
class PopulateListsInVMC
{
public:

    //constructor
    PopulateListsInVMC(string sSourceTemplateVMC, string sDestVMC, long dwPKDevice, 
        class Database_pluto_main *pDatabase_pluto_main, long dwInstallation);

    //create new vmc for the device
    bool DoIt();

private:

    //private data
    class VIPMenuCollection *m_pMenuCollection;
    class Database_pluto_main *m_pDatabase_pluto_main;

    string m_sSourceTemplateVMC;
    string m_sDestinationVMC;

    long m_dwPKDevice;
    long m_dwInstallation;

    //static private methods
    static class VIPMenuCollection *LoadMenuCollection(string sSourceVMCFile);
    static bool SaveMenuCollection(class VIPMenuCollection *pMenuCollection, string sDestVMCFile);

    //private methods
    class VIPMenu *GetMenu(MenuIds MenuID, const string& csDescription);
    class VIPMenuElement_List *GetListElement(MenuIds MenuID, long dwListId, const string& csDescription);

    bool PopulateStandardScenariosList();
    bool PopulateCamerasList();
    bool PopulateSpeechDevicesList();

    void SaveIt();
};
//------------------------------------------------------------------------------------------------------
#endif //__POPULATE_LIST_IN_VMC_H__
