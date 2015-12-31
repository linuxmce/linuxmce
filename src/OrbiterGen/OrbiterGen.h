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
#ifndef OrbiterGenerator_h
#define OrbiterGenerator_h

#include <stdlib.h>

#include "pluto_main/Database_pluto_main.h"
#include "pluto_media/Database_pluto_media.h"
#include "DesignObj_Generator.h"
#include "Orbiter/OrbiterData.h"
#include "PlutoUtils/DBHelper.h"
#include "RegenMonitor.h"

#include <memory>

class OrbiterGenerator : public OrbiterData, public DBHelper
{
public:
	std::auto_ptr<Database_pluto_main> m_spDatabase_pluto_main;
	std::auto_ptr<Database_pluto_media> m_spDatabase_pluto_media;

	class Row_Orbiter * m_pRow_Orbiter;
	class Row_Device * m_pRow_Device;
	class Row_Room * m_pRow_Room;
	class Row_EntertainArea * m_pRow_EntertainArea;
	class Row_Screen *m_pRow_Screen_MainMenu,*m_pRow_Screen_Sleeping,*m_pRow_Screen_ScreenSaver;
	class Row_DesignObj *m_pRow_DesignObj_MainMenu,*m_pRow_DesignObj_Sleeping,*m_pRow_DesignObj_ScreenSaver;
	class Row_Skin *m_pRow_Skin;
	class Row_Language *m_pRow_Language;
	class RegenMonitor *m_pRegenMonitor;
	bool m_bNoEffects,m_bUseOCG;
	bool m_bIgnoreSelected, m_bIgnoreHighlighted, m_bIgnoreAlt;

	int m_dwMediaType;
	int m_iLocation;
	map<int,int> m_htUsedStyleVariation;
	map<int,int> m_mapUsedOrbiterCriteria;
	map<int,int> m_mapSkinTranslated;
	map<string,listDesignObj_Generator *> m_htGeneratedScreens;
	map<int,class TextStyle *> m_htStylesUsedInOrbiterText;
	map<int,bool> m_mapDesignObj_WithArrays;
	map<int,bool> m_mapPopups;
//	Hashtable m_htUsedDevices;
//	StringWriter m_swErrors;
	bool m_bOrbiterChanged,m_bIsMobilePhone;
	string m_iID_Array;
	string m_GraphicsBasePath;
	string m_sSize_Regen_Data;
	int m_iPK_DesignObj_Screen;
	int m_iPK_CommandGroup; // If this is an array and we are supposed to execute an action group, this will be non-zero.  Arrays are always customer created command groups
	int m_iFloorplanPage;
	map<int,bool> m_map_PK_DesignObj_SoleScreenToGen; // Only re-generate this one screen
	map<int, int> m_mapEffects; //The map with effects
	int m_iPK_Orbiter;
	int m_iScreensTotal,m_iScreensToRender,m_iLastReportedPercentage;  // Keep track of screens we're going to need to render
	PlutoSize m_sScale,m_sScale_Orig;  // orig is the value without reduction for the borders in case the object has DESIGNOBJPARAMETER_Dont_Scale_CONST==1
	bool m_bDontAutoRegenArrays;
	string m_sDesc_Array;
	class Row_Icon * m_pRow_Icon;
	class PlutoSize *m_sizeScreen;
	class Row_Size * m_pRow_Size;
	class Row_UI * m_pRow_UI;
	string m_sFloorPlanData;
	int m_iNumFloorplanItems;

	string m_sDBHost,m_sDBUser,m_sDBPass,m_sDBDBName;
	int m_iDBPort;
	string m_sOutputPath,m_sGraphicsBasePath,m_sFontPath;
	PlutoSize m_sScaledSize;

	OrbiterGenerator(string sGraphicsFiles,string sFontFiles,string sOutputFiles,int PK_Orbiter,string DBHost,string DBUser,string DBPassword,string DBName,int DBPort)
	{
		m_sDBHost=DBHost;
		m_sDBUser=DBUser;
		m_sDBPass=DBPassword;
		m_sDBDBName=DBName;
		m_iDBPort=DBPort;

		m_spDatabase_pluto_main.reset(new Database_pluto_main(LoggerWrapper::GetInstance()));
		m_spDatabase_pluto_media.reset(new Database_pluto_media(LoggerWrapper::GetInstance()));

		m_bOrbiterChanged=false;
		m_sGraphicsBasePath=sGraphicsFiles;
		m_sFontPath=sFontFiles;
		m_sOutputPath=sOutputFiles;
		m_iPK_Orbiter=PK_Orbiter;
		m_pRegenMonitor = NULL;
/*		m_sMySQLHost=DBHost;
		m_sMySQLUser=DBUser;
		m_sMySQLPass=DBPassword;
		m_sMySQLDBName=DBName;
		m_iMySQLPort=DBPort;
*/

		m_sFloorPlanData="";
		m_iNumFloorplanItems=0;
		Counter=0;
		m_dwMediaType=0;
		m_iID_Array="0";
		m_bDontAutoRegenArrays=false;
		m_iPK_DesignObj_Screen=0;
		m_iPK_CommandGroup=0; // If this is an array and we are supposed to execute an action group, this will be non-zero
		m_iFloorplanPage=0;
		m_iScreensToRender=m_iScreensTotal=m_iLastReportedPercentage=0;
		m_pRow_Icon=NULL;

        m_pRow_Orbiter = NULL;
	}

    ~OrbiterGenerator() {}
	int DoIt();
	Row_DesignObj *GetDesignObjFromScreen(int PK_Screen);
	Row_DesignObj *GetDesignObjFromScreen(Row_Screen *pRow_Screen);
	void m_mapPopups_set(int PK_DesignObj,bool bValue)
	{
		m_mapPopups[PK_DesignObj]=bValue;
	}

private:
	map<int,string> htDevicePages;
	long Counter;

	void OutputScreen(class DesignObj_Generator *ocDesignObj);
	void OutputDesignObjs(class DesignObj_Generator *ocDesignObj,int ArrayPage,bool bIsChild,string ParentScreen);
	void OutputText(class DesignObj_Generator *ocDesignObj,class CGText *ot,bool bPrerender,string ParentScreen);
	void OutputCriteriaNest(class Row_CriteriaParmNesting *row);
	void SearchForGotos(class DesignObj_Data *pDesignObj_Data);
	void SearchForGotos(class DesignObj_Data *pDesignObj_Data,DesignObjCommandList *alCommands);
	void MatchChildDevicesToLocation(LocationInfo *li,Row_Device *pRow_Device,bool bStartWithTopMost=true);
	void FixupSpecialOrbiters(list<LocationInfo *> &listLocationInfo);
	void ScaleCommandList(DesignObj_Generator *pocDesignObj,DesignObjCommandList &CommandList);
	class Row_Size *TranslateSize(string sSize);
	bool CommonControlledVia(Row_Device *pRow_Device1,Row_Device *pRow_Device2);
	string First2Dots(string sDesignObj);
	class Row_Skin *TranslateSkin(Row_Skin *pRow_Skin);
	//void PopulateScreenMap();

	//this will map each effect type to an effect based on the skin
	void PopulateEffects(map<int, int> &mapEffects, int FK_Skin);
};

#endif
