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
#include <iostream>
#include <fstream>

#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "OrbiterGen.h"
#include "ScreenMap.h"
#include "Renderer.h"

#include "DCE/Logger.h"
#include "DesignObj_Generator.h"
#include "OGCommand.h"
#include "OGArray.h"
#include "SerializeClass/ShapesColors.h"

#include "PlutoUtils/minilzo.h"
#define HEAP_ALLOC(var,size) \
        lzo_align_t __LZO_MMODEL var [ ((size) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ]
static HEAP_ALLOC(wrkmem,LZO1X_1_MEM_COMPRESS);

#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_Screen.h"
#include "pluto_main/Table_Screen_DesignObj.h"
#include "pluto_main/Table_Text_LS.h"
#include "pluto_main/Table_Image.h"
#include "pluto_main/Table_CachedScreens.h"
#include "pluto_main/Table_Criteria.h"
#include "pluto_main/Table_DesignObj.h"
#include "pluto_main/Table_DesignObjVariation.h"
#include "pluto_main/Table_DesignObjVariation_Text_Skin_Language.h"
#include "pluto_main/Table_DesignObjVariation_Text.h"
#include "pluto_main/Table_DesignObjVariation_Zone.h"
#include "pluto_main/Table_DesignObjVariation_DesignObj.h"
#include "pluto_main/Table_DesignObjVariation_DesignObjParameter.h"
#include "pluto_main/Table_Size.h"
#include "pluto_main/Table_Skin.h"
#include "pluto_main/Table_EffectType_Effect_Skin.h"
#include "pluto_main/Table_Language.h"
#include "pluto_main/Table_UI.h"
#include "pluto_main/Table_Text.h"
#include "pluto_main/Table_Style.h"
#include "pluto_main/Table_Room_Users.h"
#include "pluto_main/Table_Variable.h"
#include "pluto_main/Table_StyleVariation.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_DeviceCategory.h"
#include "pluto_main/Table_Device_DeviceData.h"
#include "pluto_main/Table_Device_EntertainArea.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_Orbiter.h"
#include "pluto_main/Table_Orbiter_Variable.h"
#include "pluto_main/Table_Users.h"
#include "pluto_main/Table_Room.h"
#include "pluto_main/Table_Text_LS_AltVersions.h"
#include "pluto_main/Table_CommandGroup.h"
#include "pluto_main/Table_EntertainArea.h"
#include "pluto_main/Table_Icon.h"
#include "pluto_main/Table_CommandParameter.h"
#include "pluto_main/Table_DeviceData.h"
#include "pluto_main/Table_DesignObjParameter.h"
#include "pluto_main/Table_Installation_Users.h"
#include "pluto_main/Table_DesignObjType.h"
#include "pluto_main/Table_Command.h"
#include "pluto_main/Table_Variable.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_Event.h"
#include "pluto_main/Table_EventHandler.h"
#include "pluto_main/Table_CommandGroup_Command_CommandParameter.h"
#include "pluto_main/Table_CommandGroup_Command.h"
#include "pluto_main/Table_MediaType.h"
#include "pluto_media/Table_AttributeType.h"
#include "PlutoUtils/DatabaseUtils.h"
#include "DCE/DCEConfig.h"

DCEConfig g_DCEConfig;

#include "../include/version.h"

static bool LocationComparer(LocationInfo *x, LocationInfo *y)
{
	return x->Description<y->Description;
}

bool g_bBootSplash=false;

#ifdef WIN32
int _tmain(int argc, _TCHAR* argv[])
{

	//	WSADATA wsaData;
	//	err = WSAStartup(MAKEWORD( 1, 1 ),(LPWSADATA)  &wsaData);
	string sLogger="/temp/orbitergen.log";
#else
int main(int argc, char *argv[])
{
	setenv("SDL_VIDEODRIVER", "dummy", 1); // force SDL to use its dummy video driver (removed a dependency on the X server)
	string sLogger="/var/log/pluto/OrbiterGen.log";
#endif


	string DBHost="dcerouter",DBUser="root",DBPassword="",DBName="pluto_main";
#ifdef WIN32
	string GraphicsFiles="/pluto/orbiter/skins",FontFiles="/windows/fonts",OutputFiles="/pluto/orbiter";
#else
	string GraphicsFiles="/usr/pluto/orbiter/skins",FontFiles="/usr/share/fonts/truetype/msttcorefonts",OutputFiles="/usr/pluto/orbiter";
#endif
	int DBPort=3306,PK_Orbiter=0,iDontAutoRegenArrays=0;
	string sSoleScreenToGen;
	bool bRegen=false;
	bool bError=false; // An error parsing the command line
	char c;
	for(int optnum=1;optnum<argc;++optnum)
	{
		if( argv[optnum][0]!='-' )
		{
			if( argv[optnum][0]==0 )
				continue; // Skip blank options
			cerr << "Unknown option " << argv[optnum] << endl;
			exit(1);
		}

		c=argv[optnum][1];
		switch (c)
		{
		case 'b':
			g_bBootSplash = true;
			break;
		case 'h':
			DBHost = argv[++optnum];
			break;
		case 'u':
			DBUser = argv[++optnum];
			break;
		case 'p':
			DBPassword = argv[++optnum];
			break;
		case 'D':
			DBName = argv[++optnum];
			break;
		case 'P':
			DBPort = atoi(argv[++optnum]);
			break;
		case 'd':
			PK_Orbiter = atoi(argv[++optnum]);
			break;
		case 's':
			sSoleScreenToGen = argv[++optnum];
			break;
		case 'g':
			GraphicsFiles = argv[++optnum];
			break;
		case 'f':
			FontFiles = argv[++optnum];
			break;
		case 'o':
			OutputFiles = argv[++optnum];
			break;
		case 'a':
			iDontAutoRegenArrays = 1;
			break;
		case 'r':
			bRegen=true;
			break;
		case 'l':
			sLogger = argv[++optnum];
			break;
		default:
			bError=true;
			break;
		};
	}

	if (bError || GraphicsFiles.length()==0 || FontFiles.length()==0 || OutputFiles.length()==0 || !PK_Orbiter)
	{
		cout << "OrbiterGen, v." << VERSION << endl
			<< "Usage: OrbiterGen -d Orbiter [-r] [-a] [-g graphics files] [-f font files]" << endl
				<< "[-o output path] [-h hostname] [-u username] [-p password] [-D database]" << endl
				<< "[-P mysql port] [-s screen to regen]" << endl
			<< "-d orbiter   -- the ID of the orbiter to generate" << endl
			<< "-r           -- Regenerate all screens--ignore cache" << endl
			<< "-a           -- Don't auto regenerate screens with arrays" << endl
			<< "-g graphics  -- path to find the input (source) graphics files" << endl
			<< "-f fonts     -- path to find the font files" << endl
			<< "-o output    -- path to put all output files" << endl
			<< "-h hostname  -- address or DNS of database host, default is `localhost`" << endl
			<< "-u username  -- username for database connection" << endl
			<< "-p password  -- password for database connection, default is `` (empty)" << endl
			<< "-D database  -- database name" << endl
			<< "-P port      -- port for database connection, default is 3306" << endl
			<< "-s screen    -- only regenerate this one screen" << endl;
		exit(1);
	}

	try
	{
		if( sLogger=="null" )
			LoggerWrapper::SetType(LT_LOGGER_NULL);
		else if( sLogger!="stdout" )
			LoggerWrapper::SetType(LT_LOGGER_FILE,sLogger);

		LoggerWrapper::GetInstance()->LogAll();
	}
	catch(...)
	{
		cerr << "Unable to create logger" << endl;
	}

	// Windows will work with / also
	StringUtils::Replace(&GraphicsFiles,"\\","/");
	StringUtils::Replace(&FontFiles,"\\","/");
	StringUtils::Replace(&OutputFiles,"\\","/");

	if( GraphicsFiles.length() && GraphicsFiles[GraphicsFiles.length()-1]!='/' )
		GraphicsFiles+="/";
	if( FontFiles.length() && FontFiles[FontFiles.length()-1]!='/' )
		FontFiles+="/";
	if( OutputFiles.length() && OutputFiles[OutputFiles.length()-1]!='/' )
		OutputFiles+="/";

	int iResult=0;

	if( !FileUtils::FileExists(GraphicsFiles + "Basic") )
	{
		cerr << "Cannot find 'Basic' skin directory under " << GraphicsFiles << ".  Specify the directory with -g" << endl;
		exit(1);
	}
	if( !FileUtils::FileExists(FontFiles + "arial.ttf") )
	{
		cerr << "Cannot find 'fonts' -- looked for " << FontFiles << "arial.ttf.  Specify the directory with -f" << endl;
		exit(1);
	}
	OrbiterGenerator *pOrbiterGenerator=NULL;
	try
	{
		pOrbiterGenerator = new OrbiterGenerator(GraphicsFiles,FontFiles,OutputFiles,PK_Orbiter,DBHost,DBUser,DBPassword,DBName,DBPort);
		if( bRegen )
			pOrbiterGenerator->m_bOrbiterChanged=true;
		pOrbiterGenerator->m_bDontAutoRegenArrays= iDontAutoRegenArrays==1;

		string::size_type pos=0;
		while(pos!=string::npos && pos<sSoleScreenToGen.length())
			pOrbiterGenerator->m_map_PK_DesignObj_SoleScreenToGen[atoi(StringUtils::Tokenize(sSoleScreenToGen,",",pos).c_str())]=true;

		struct tm *newtime;
		time_t aclock;
		time( &aclock );   // Get time in seconds
		newtime = localtime( &aclock );   // Convert time to struct tm form 
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Generation started");

		iResult = pOrbiterGenerator->DoIt();

		time( &aclock );   // Get time in seconds
		newtime = localtime( &aclock );   // Convert time to struct tm form 
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Generation done");
	}
	catch(int i)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Caught exception number: %d",i);
	}
	if( pOrbiterGenerator && pOrbiterGenerator->m_pRow_Orbiter )
	{
		string sAllScenariosFloorplans = pOrbiterGenerator->m_pRegenMonitor->AllScenariosFloorplans();
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter %d setting AllScenariosFloorplans to %s",
			pOrbiterGenerator->m_pRow_Orbiter->PK_Orbiter_get(),sAllScenariosFloorplans.c_str());

		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Setting RegenInProgress_set to false for %d",pOrbiterGenerator->m_pRow_Orbiter->PK_Orbiter_get());
		DatabaseUtils::LockTable(pOrbiterGenerator->m_spDatabase_pluto_main.get(),"Orbiter");
		pOrbiterGenerator->m_pRow_Orbiter->Reload();  // We already updated floorplans
		pOrbiterGenerator->m_pRow_Orbiter->Regen_set(false);
		pOrbiterGenerator->m_pRow_Orbiter->RegenInProgress_set(false);
		pOrbiterGenerator->m_pRow_Orbiter->RegenStatus_set("Done");
		pOrbiterGenerator->m_pRow_Orbiter->RegenPercent_set(100);
		pOrbiterGenerator->m_pRow_Orbiter->FirstRegen_set(pOrbiterGenerator->m_bNewOrbiter);
		pOrbiterGenerator->m_pRow_Orbiter->ScenariosFloorplans_set( sAllScenariosFloorplans );
		pOrbiterGenerator->m_pRow_Orbiter->Size_set( pOrbiterGenerator->m_sSize_Regen_Data );
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Committing");
		pOrbiterGenerator->m_pRow_Orbiter->Table_Orbiter_get()->Commit();
		string sql = "UPDATE Orbiter SET Modification_LastGen=psc_mod,psc_mod=psc_mod WHERE PK_Orbiter=" + StringUtils::itos(pOrbiterGenerator->m_pRow_Orbiter->PK_Orbiter_get());
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Updating");
		pOrbiterGenerator->m_spDatabase_pluto_main->threaded_db_wrapper_query(sql);
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Unlocking");
		DatabaseUtils::UnLockTables(pOrbiterGenerator->m_spDatabase_pluto_main.get());
	}
	if( pOrbiterGenerator )
        delete pOrbiterGenerator;

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Finished!");
	return iResult;
}

int OrbiterGenerator::DoIt()
{
	// and also the sql2cpp classes
	if( !m_spDatabase_pluto_main->Connect(m_sDBHost,m_sDBUser,m_sDBPass,m_sDBDBName,m_iDBPort) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Failed to connect to database");
		exit(1);
	}

	if( !m_spDatabase_pluto_media->Connect(m_sDBHost,m_sDBUser,m_sDBPass,"pluto_media",m_iDBPort) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Failed to connect to pluto_media database");
		exit(1);
	}

	m_pRegenMonitor = new RegenMonitor(m_spDatabase_pluto_main.get(), m_spDatabase_pluto_media.get());

	m_pRow_Device = m_spDatabase_pluto_main->Device_get()->GetRow(m_iPK_Orbiter);
	if( !m_pRow_Device )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Device %d doesn't exist",m_iPK_Orbiter);
		exit(1);
	}

	m_bIsMobilePhone = m_pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get()==DEVICECATEGORY_Mobile_Orbiter_CONST;

	m_bNewOrbiter=false; // Will set to true if this is the first time this Orbiter was generated
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Locking orbiter before Generating %d",m_iPK_Orbiter);
	DatabaseUtils::LockTable(m_spDatabase_pluto_main.get(),"Orbiter");
	m_pRow_Orbiter = m_spDatabase_pluto_main->Orbiter_get()->GetRow(m_iPK_Orbiter);
	if( !m_pRow_Orbiter )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"new orbiter %d",m_iPK_Orbiter);
		m_bNewOrbiter=true;
		m_pRow_Orbiter = m_spDatabase_pluto_main->Orbiter_get()->AddRow();
		m_pRow_Orbiter->PK_Orbiter_set(m_iPK_Orbiter);
		m_pRow_Orbiter->Table_Orbiter_get()->Commit();
	}
	else
		m_pRow_Orbiter->Reload();

	if( m_pRow_Orbiter->RegenInProgress_get() )
	{
		time_t tModDate = StringUtils::SQLDateTime(m_pRow_Orbiter->psc_mod_get());
		time_t tNow = time(NULL);
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"*****SEEMS THIS ORBITER IS BEING GENERATED %d now %d",long(tModDate),long(tNow));
		if( tNow-tModDate < 120 )  // There's recent activity within the last 2 minutes, so skip it
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"skipping");
#ifndef WIN32
 			exit(1);
#endif
		}
	}

	if( m_pRow_Orbiter->Regen_get()==2 )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Full regen for %d",m_pRow_Orbiter->PK_Orbiter_get());
		m_bOrbiterChanged=true;
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Generating %d",m_pRow_Orbiter->PK_Orbiter_get());

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Setting RegenInProgress_set to true for %d",m_pRow_Orbiter->PK_Orbiter_get());
	m_pRow_Orbiter->RegenInProgress_set(true);
//	m_pRow_Orbiter->ScenariosFloorplans_set( m_pRegenMonitor->AllScenariosFloorplans() );
	m_pRow_Orbiter->Table_Orbiter_get()->Commit();
	DatabaseUtils::UnLockTables(m_spDatabase_pluto_main.get());

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Generating: #%d %s",m_pRow_Device->PK_Device_get(),m_pRow_Device->Description_get().c_str());

	if( m_sOutputPath[ m_sOutputPath.length()-1 ]!='/' )
		m_sOutputPath += "/";

	if( m_sFontPath[ m_sFontPath.length()-1 ]!='/' )
		m_sFontPath += "/";

	m_sOutputPath += "C" + StringUtils::itos(m_iPK_Orbiter) + "/";
	FileUtils::MakeDir(m_sOutputPath);

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Building orbiter: %d inst %d",m_iPK_Orbiter,m_pRow_Device->FK_Installation_get());

#ifdef WIN32
	system(("mkdir \"" + m_sOutputPath + "\" 2> null").c_str());
#else
	system(("mkdir -p " + m_sOutputPath + " 2>/dev/null").c_str());
#endif

	m_pRow_Skin = NULL;
	Row_Device_DeviceData *pRow_Device_DeviceData = m_spDatabase_pluto_main->Device_DeviceData_get()->GetRow(m_pRow_Device->PK_Device_get(),DEVICEDATA_PK_Skin_CONST);
	if( pRow_Device_DeviceData )
		m_pRow_Skin = m_spDatabase_pluto_main->Skin_get()->GetRow( atoi(pRow_Device_DeviceData->IK_DeviceData_get().c_str()) );

	if( !m_pRow_Skin  )
	{
		if( m_bIsMobilePhone )
			m_pRow_Skin = m_spDatabase_pluto_main->Skin_get()->GetRow( 3 );  // The default phone skin
		else
			m_pRow_Skin = m_spDatabase_pluto_main->Skin_get()->GetRow( 1 );  // The default skin
	}

	if( !m_pRow_Skin )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find Orbiter's Skin");
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Setting RegenInProgress_set 2 to false for %d",m_pRow_Orbiter->PK_Orbiter_get());

		DatabaseUtils::LockTable(m_spDatabase_pluto_main.get(),"Orbiter");
		m_pRow_Orbiter->RegenInProgress_set(false);
		m_spDatabase_pluto_main->Orbiter_get()->Commit();
		DatabaseUtils::UnLockTables(m_spDatabase_pluto_main.get());
		exit(1);
	}

	Row_Skin *pRow_Skin_Translated = TranslateSkin(m_pRow_Skin);

	//ignore TranslateSkin if there's nothing to translate (no ReplaceSkin_* for DCEConfig)
	if(pRow_Skin_Translated)
		m_pRow_Skin = pRow_Skin_Translated;

	m_sSkin = m_pRow_Skin->Description_get();


	// Get the UI
	m_pRow_UI = NULL;
	pRow_Device_DeviceData = m_spDatabase_pluto_main->Device_DeviceData_get()->GetRow(m_pRow_Device->PK_Device_get(),DEVICEDATA_PK_UI_CONST);
	if( pRow_Device_DeviceData )
		m_pRow_UI = m_spDatabase_pluto_main->UI_get()->GetRow( atoi(pRow_Device_DeviceData->IK_DeviceData_get().c_str()) );
	if( !m_pRow_UI )
		m_pRow_UI = m_spDatabase_pluto_main->UI_get()->GetRow( 1 ); // Default

	if( !m_pRow_UI )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find Orbiter's UI");
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Setting RegenInProgress_set 5 to false for %d",m_pRow_Orbiter->PK_Orbiter_get());
		DatabaseUtils::LockTable(m_spDatabase_pluto_main.get(),"Orbiter");
		m_pRow_Orbiter->RegenInProgress_set(false);
		m_spDatabase_pluto_main->Orbiter_get()->Commit();
		DatabaseUtils::UnLockTables(m_spDatabase_pluto_main.get());
		exit(1);
	}
	m_iUiVersion = m_pRow_UI->Version_get();

	// Maybe we're using a substituted skin.  If so, for the sake of calculating the screen
	// mapping we should use the original.
	Row_Skin *pRow_Skin_For_Translation = m_pRow_Skin;
	int PK_Skin_Original = m_mapSkinTranslated[m_pRow_Skin->PK_Skin_get()];
	if( PK_Skin_Original )
	{
		pRow_Skin_For_Translation = m_spDatabase_pluto_main->Skin_get()->GetRow(PK_Skin_Original);
		if( !pRow_Skin_For_Translation )
			pRow_Skin_For_Translation = m_pRow_Skin;
	}

	//this will map each effect type to an effect based on the skin
	PopulateEffects(m_mapEffects, m_pRow_Skin->PK_Skin_get());

	vector<Row_MediaType *> vectRow_MediaType;
	m_spDatabase_pluto_main->MediaType_get()->GetRows("1=1",&vectRow_MediaType);
	for(vector<Row_MediaType *>::iterator it=vectRow_MediaType.begin();it!=vectRow_MediaType.end();++it)
	{
		Row_MediaType *pRow_MediaType = *it;
		m_mapPK_MediaType_PK_Attribute_Sort[ pRow_MediaType->PK_MediaType_get() ] = pRow_MediaType->EK_AttributeType_DefaultSort_get();
		m_mapPK_MediaType_Description[ pRow_MediaType->PK_MediaType_get() ] = pRow_MediaType->Description_get();
	}

	vector<Row_AttributeType *> vectRow_AttributeType;
	m_spDatabase_pluto_media->AttributeType_get()->GetRows("1=1",&vectRow_AttributeType);
	for(vector<Row_AttributeType *>::iterator it=vectRow_AttributeType.begin();it!=vectRow_AttributeType.end();++it)
		m_mapPK_AttributeType_Description[ (*it)->PK_AttributeType_get() ] = (*it)->Description_get();

	PopulateScreenMap(m_spDatabase_pluto_main.get(), m_mapDesignObj, m_pRow_UI, pRow_Skin_For_Translation, m_pRow_Device,false);
	// Now call it again passing in 'true' and the translated skin so if there's any replacements with the translated skin they'll get picked up
	PopulateScreenMap(m_spDatabase_pluto_main.get(), m_mapDesignObj, m_pRow_UI, m_pRow_Skin, m_pRow_Device, true);

	vector<Row_Screen *> vectRow_Screen_GoBack;
	m_spDatabase_pluto_main->Screen_get()->GetRows("GoBackToScreen=1",&vectRow_Screen_GoBack);
	for(vector<Row_Screen *>::iterator it_vsgb=vectRow_Screen_GoBack.begin();it_vsgb!=vectRow_Screen_GoBack.end();++it_vsgb)
		m_mapPK_Screen_GoBackToScreen[ (*it_vsgb)->PK_Screen_get() ]=1;

	vector<Row_Screen *> vectRow_Screen_Interrupt;
	m_spDatabase_pluto_main->Screen_get()->GetRows("1=1",&vectRow_Screen_Interrupt);
	for(vector<Row_Screen *>::iterator it_vsgb=vectRow_Screen_Interrupt.begin();it_vsgb!=vectRow_Screen_Interrupt.end();++it_vsgb)
		m_mapScreen_Interrupt[ (*it_vsgb)->PK_Screen_get() ]=(*it_vsgb)->AllowInterruptions_get();

	m_pRow_Screen_MainMenu = NULL;
	pRow_Device_DeviceData = m_spDatabase_pluto_main->Device_DeviceData_get()->GetRow(m_pRow_Device->PK_Device_get(),DEVICEDATA_PK_Screen_CONST);

	if( pRow_Device_DeviceData )
		m_pRow_Screen_MainMenu = m_spDatabase_pluto_main->Screen_get()->GetRow(atoi(pRow_Device_DeviceData->IK_DeviceData_get().c_str()) );

	if( !m_pRow_Screen_MainMenu )
		m_pRow_Screen_MainMenu = m_pRow_Skin->FK_Screen_MainMenu_getrow();

	if( !m_pRow_Screen_MainMenu )
		m_pRow_Screen_MainMenu = m_spDatabase_pluto_main->Screen_get()->GetRow(SCREEN_Main_CONST);

	if( !m_pRow_Screen_MainMenu )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find Orbiter's Main Menu: ");
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Setting RegenInProgress_set 3 to false for %d",m_pRow_Orbiter->PK_Orbiter_get());
		DatabaseUtils::LockTable(m_spDatabase_pluto_main.get(),"Orbiter");
		m_pRow_Orbiter->RegenInProgress_set(false);
		m_spDatabase_pluto_main->Orbiter_get()->Commit();
		DatabaseUtils::UnLockTables(m_spDatabase_pluto_main.get());
		exit(1);
	}

	if( m_pRow_Screen_MainMenu->PK_Screen_get()!=SCREEN_Main_CONST )  // It's possible we chose a non-standard main menu
		m_mapDesignObj[SCREEN_Main_CONST] = GetDesignObjFromScreen(m_pRow_Screen_MainMenu->PK_Screen_get())->PK_DesignObj_get();

	m_pRow_Screen_Sleeping = m_pRow_Skin->FK_Screen_Sleeping_getrow();

	if( !m_pRow_Screen_Sleeping )
		m_pRow_Screen_Sleeping = m_spDatabase_pluto_main->Screen_get()->GetRow(SCREEN_Sleeping_CONST);

	if( !m_pRow_Screen_Sleeping )
		m_pRow_Screen_Sleeping = m_pRow_Screen_MainMenu;

	m_pRow_Screen_ScreenSaver = m_pRow_Skin->FK_Screen_ScreenSaver_getrow();

	if( !m_pRow_Screen_ScreenSaver )
		m_pRow_Screen_ScreenSaver = m_spDatabase_pluto_main->Screen_get()->GetRow(SCREEN_ScreenSaver_CONST);

	if( !m_pRow_Screen_ScreenSaver )
		m_pRow_Screen_ScreenSaver = m_pRow_Screen_MainMenu;

	m_pRow_DesignObj_MainMenu = GetDesignObjFromScreen(m_pRow_Screen_MainMenu);
	m_pRow_DesignObj_Sleeping = GetDesignObjFromScreen(m_pRow_Screen_Sleeping);
	m_pRow_DesignObj_ScreenSaver = GetDesignObjFromScreen(m_pRow_Screen_ScreenSaver);

	// Get the language
	m_pRow_Language = NULL;
	pRow_Device_DeviceData = m_spDatabase_pluto_main->Device_DeviceData_get()->GetRow(m_pRow_Device->PK_Device_get(),DEVICEDATA_PK_Language_CONST);
	if( pRow_Device_DeviceData )
		m_pRow_Language = m_spDatabase_pluto_main->Language_get()->GetRow( atoi(pRow_Device_DeviceData->IK_DeviceData_get().c_str()) );
	if( !m_pRow_Language )
		m_pRow_Language = m_spDatabase_pluto_main->Language_get()->GetRow( 1 ); // English

	if( !m_pRow_Language )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find Orbiter's Language");
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Setting RegenInProgress_set 4 to false for %d",m_pRow_Orbiter->PK_Orbiter_get());
		DatabaseUtils::LockTable(m_spDatabase_pluto_main.get(),"Orbiter");
		m_pRow_Orbiter->RegenInProgress_set(false);
		m_spDatabase_pluto_main->Orbiter_get()->Commit();
		DatabaseUtils::UnLockTables(m_spDatabase_pluto_main.get());
		exit(1);
	}

	// Get the no effects flag
	pRow_Device_DeviceData = m_spDatabase_pluto_main->Device_DeviceData_get()->GetRow(m_pRow_Device->PK_Device_get(),DEVICEDATA_No_Effects_CONST);
	if( pRow_Device_DeviceData )
		m_bNoEffects = atoi(pRow_Device_DeviceData->IK_DeviceData_get().c_str())==1;
	else
		m_bNoEffects = false;

	// Get the use alpha blended gui flag
	pRow_Device_DeviceData = m_spDatabase_pluto_main->Device_DeviceData_get()->GetRow(m_pRow_Device->PK_Device_get(),DEVICEDATA_Use_alpha_blended_UI_CONST);
	if( pRow_Device_DeviceData )
		m_bUseAlphaBlending = atoi(pRow_Device_DeviceData->IK_DeviceData_get().c_str())==1;
	else
		m_bUseAlphaBlending = false;

	m_bUseMask = (m_pRow_UI->PK_UI_get()==UI_V2_Normal_Horizontal_16_9_CONST && m_bUseAlphaBlending==false);

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Use alpha %d use mask %d",(int) m_bUseAlphaBlending,(int) m_bUseMask);
// HACK - TEMPORARILY DISABLE EFFECTS
m_bNoEffects = true;

	pRow_Device_DeviceData = m_spDatabase_pluto_main->Device_DeviceData_get()->GetRow(m_pRow_Device->PK_Device_get(),DEVICEDATA_Rotation_CONST);
	m_iRotation = pRow_Device_DeviceData ? atoi(pRow_Device_DeviceData->IK_DeviceData_get().c_str()) : 0;

	// Get the ignore state flag
	pRow_Device_DeviceData = m_spDatabase_pluto_main->Device_DeviceData_get()->GetRow(m_pRow_Device->PK_Device_get(),DEVICEDATA_Ignore_CONST);
	if( pRow_Device_DeviceData )
	{
		string sIgnoreState = pRow_Device_DeviceData->IK_DeviceData_get();
		m_bIgnoreSelected = sIgnoreState.find('S')!=string::npos;
		m_bIgnoreHighlighted = sIgnoreState.find('H')!=string::npos;
		m_bIgnoreAlt = sIgnoreState.find('A')!=string::npos;
	}
	else
		m_bIgnoreSelected = m_bIgnoreHighlighted = m_bIgnoreAlt = false;

	m_bIsOSD=false;
	Row_Device *pRow_Device_MD=m_pRow_Device->FK_Device_ControlledVia_getrow();
	Row_DeviceCategory *pDeviceCategory_MD=pRow_Device_MD ? pRow_Device_MD->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow() : NULL;
	while( pDeviceCategory_MD )
	{
		if( pDeviceCategory_MD->PK_DeviceCategory_get()==DEVICECATEGORY_Media_Director_CONST )
		{
			m_bIsOSD=true;
			break;
		}
		pDeviceCategory_MD = pDeviceCategory_MD->FK_DeviceCategory_Parent_getrow();
	}

	pRow_Device_DeviceData = m_spDatabase_pluto_main->Device_DeviceData_get()->GetRow(m_pRow_Device->PK_Device_get(),DEVICEDATA_Use_OCG_Format_CONST);
	if( pRow_Device_DeviceData )
		m_bUseOCG = atoi(pRow_Device_DeviceData->IK_DeviceData_get().c_str())==1;
	else
		m_bUseOCG = false;

	if( m_bUseOCG )
	{
		if( m_pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get()==DEVICECATEGORY_Mobile_Orbiter_CONST ||
			m_bIsOSD )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"There's a phone or onscreen orbiter with OCG.  Resetting it");
			pRow_Device_DeviceData->IK_DeviceData_set("0");
			pRow_Device_DeviceData->Table_Device_DeviceData_get()->Commit();
		}
	}

	// See if there's a default entertainment area for this orbiter
	Row_EntertainArea *pRow_EntertainArea_Default = NULL;
	vector<Row_Device_EntertainArea *> vectRow_Device_EntertainArea;
	m_pRow_Device->Device_EntertainArea_FK_Device_getrows(&vectRow_Device_EntertainArea);
	if( vectRow_Device_EntertainArea.size() )
		pRow_EntertainArea_Default = vectRow_Device_EntertainArea[0]->FK_EntertainArea_getrow();

	vector<Row_Variable *> vectrv;
	m_spDatabase_pluto_main->Variable_get()->GetRows("1=1",&vectrv);
	for(size_t s=0;s<vectrv.size();++s)
	{
		Row_Variable *drVariable = vectrv[s];
		Row_Orbiter_Variable *drCV = m_spDatabase_pluto_main->Orbiter_Variable_get()->GetRow(m_pRow_Orbiter->PK_Orbiter_get(),drVariable->PK_Variable_get());
		if( drCV && !drCV->Value_isNull() )
		{
			m_mapVariable[drVariable->PK_Variable_get()]=drCV->Value_get();
		}
		else
			m_mapVariable[drVariable->PK_Variable_get()]="";
	}

	Row_Users *drUsers_Default = NULL;
	Row_Device_DeviceData *drD_C_DP_DefaultUser = m_spDatabase_pluto_main->Device_DeviceData_get()->GetRow(m_pRow_Orbiter->PK_Orbiter_get(),DEVICEDATA_PK_Users_CONST);

	if( drD_C_DP_DefaultUser )
	{
		Row_Installation_Users *pRow_Installation_Users=m_spDatabase_pluto_main->Installation_Users_get()->GetRow(m_pRow_Device->FK_Installation_get(),atoi(drD_C_DP_DefaultUser->IK_DeviceData_get().c_str()));
		if( pRow_Installation_Users )
			drUsers_Default = pRow_Installation_Users->FK_Users_getrow();
	}

	if( !drUsers_Default )
	{
		vector<Row_Installation_Users *> vectRow_Installation_Users;
		m_spDatabase_pluto_main->Installation_Users_get()->GetRows(INSTALLATION_USERS_FK_INSTALLATION_FIELD "=" + StringUtils::itos(m_pRow_Device->FK_Installation_get()),&vectRow_Installation_Users);
		if( vectRow_Installation_Users.size() )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"***Warning*** No default user specified.  Picking first one: %d",vectRow_Installation_Users[0]->FK_Users_get());
			drUsers_Default = vectRow_Installation_Users[0]->FK_Users_getrow();
			if( !drUsers_Default )
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database problem -- There is a user in Installation_Users that is not a valid user");
		}
		else
		{
			vector<Row_Users *> vectRow_Users;
			m_spDatabase_pluto_main->Users_get()->GetRows("1=1",&vectRow_Users); // Just find any user
			if( vectRow_Users.size()>0 )
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING,"***Warning*** No users at all in this installation.  Picking one");
				drUsers_Default = vectRow_Users[0];
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"***Error*** The users table is completely empty.");
			}
		}
	}

	if( drUsers_Default )
		m_dwPK_Users_Default = drUsers_Default->PK_Users_get();

//	if( m_pRow_Device->FK_Room_isNull() )
//		throw "no room for orbiter: " + StringUtils::itos(m_pRow_Device->PK_Device_get());

	//	map<int,listDesignObj_Generator *> htGeneratedScreens;
	//	map<int,string> htUsedDevices;
	//	map<int,string> m_mapUsedOrbiterCriteria;
	m_GraphicsBasePath=m_sGraphicsBasePath;
	//	m_htGeneratedScreens=&htGeneratedScreens;
	//	m_htUsedStyleVariation=&htUsedStyleVariation;
	//	m_htUsedVariables=htUsedVariables;
	//	m_htUsedDevices=htUsedDevices;
	//	m_m_mapUsedOrbiterCriteria = &m_mapUsedOrbiterCriteria;
	//	m_swErrors=swErrors;
	//	m_htVariables=m_mapVariable;
	if( !m_bOrbiterChanged )  // It may have already been set to true if -r regen was specified
	{
		time_t lModDate = StringUtils::SQLDateTime(m_pRow_Orbiter->psc_mod_get());
		time_t lModDate_LastGen = StringUtils::SQLDateTime(m_pRow_Orbiter->Modification_LastGen_get());
		m_bOrbiterChanged = false; //(lModDate!=lModDate_LastGen);
		if( m_bOrbiterChanged )
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter changed date from: %d to %d",long(lModDate_LastGen),long(lModDate));
	}

	m_pRow_Size = NULL;
	if( !m_pRow_Device->FK_Device_ControlledVia_isNull() &&
		m_pRow_Device->FK_Device_ControlledVia_getrow()->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get()==DEVICECATEGORY_Media_Director_CONST )
	{
		// This is an on-screen orbiter
		pRow_Device_DeviceData = m_spDatabase_pluto_main->Device_DeviceData_get()->GetRow(m_pRow_Device->FK_Device_ControlledVia_get(),DEVICEDATA_Video_settings_CONST);
		if( pRow_Device_DeviceData )
		{
			string sSize = pRow_Device_DeviceData->IK_DeviceData_get();
			string::size_type pos=sSize.find('/');
			if( pos!=string::npos )
				sSize = sSize.substr(0,pos);
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Found OSD using size: %s",sSize.c_str());

			m_pRow_Size = TranslateSize(sSize);
		}
	}

	if( !m_pRow_Size )
	{
		pRow_Device_DeviceData = m_spDatabase_pluto_main->Device_DeviceData_get()->GetRow(m_pRow_Device->PK_Device_get(),DEVICEDATA_PK_Size_CONST);
		if( pRow_Device_DeviceData )
			m_pRow_Size = m_spDatabase_pluto_main->Size_get()->GetRow( atoi(pRow_Device_DeviceData->IK_DeviceData_get().c_str()) );
	}

	if( !m_pRow_Size )
	{
		if( m_bIsMobilePhone )
			m_pRow_Size = m_spDatabase_pluto_main->Size_get()->GetRow( 3 );  // The default phone size
		else
			m_pRow_Size = m_spDatabase_pluto_main->Size_get()->GetRow( 1 );  // The default size
	}

	if( !m_pRow_Size )
		throw "Cannot determine the size";

	m_Width = m_pRow_Size->Width_get();
	m_Height = m_pRow_Size->Height_get();

	m_sScale.Width = m_pRow_Size->ScaleX_get();
	m_sScale.Height = m_pRow_Size->ScaleY_get();
	int iSpacingParameter=0;

	pRow_Device_DeviceData = m_spDatabase_pluto_main->Device_DeviceData_get()->GetRow(m_pRow_Device->PK_Device_get(),DEVICEDATA_Spacing_CONST);
	if( pRow_Device_DeviceData && atoi(pRow_Device_DeviceData->IK_DeviceData_get().c_str()) )
	{
		iSpacingParameter = atoi(pRow_Device_DeviceData->IK_DeviceData_get().c_str());
		if( iSpacingParameter>0 && iSpacingParameter<80 )
		{
			m_rSpacing.Width = m_rSpacing.X = int(m_pRow_Size->Width_get() * ( ((double) iSpacingParameter)/2 ) / 100);
			m_rSpacing.Height = m_rSpacing.Y = int(m_pRow_Size->Height_get() * ( ((double) iSpacingParameter)/2 ) / 100);
			// We're not going to ever commit m_pRow_Size, so just change the scale accordingly
			m_pRow_Size->ScaleX_set( m_pRow_Size->ScaleX_get() - (m_pRow_Size->ScaleX_get() * iSpacingParameter / 100) );
			m_pRow_Size->ScaleY_set( m_pRow_Size->ScaleY_get() - (m_pRow_Size->ScaleY_get() * iSpacingParameter / 100) );
			pRow_Device_DeviceData = m_spDatabase_pluto_main->Device_DeviceData_get()->GetRow(m_pRow_Device->PK_Device_get(),DEVICEDATA_Offset_CONST);
			if( pRow_Device_DeviceData )
			{
				string::size_type pos=0;
				string sValue = pRow_Device_DeviceData->IK_DeviceData_get();
				int XOffset = atoi(StringUtils::Tokenize(sValue,",",pos).c_str());
				if( abs(XOffset)>m_rSpacing.X )  // Just in case the offset is too much
					XOffset = XOffset<0 ? m_rSpacing.X*-1 : m_rSpacing.X;
				int YOffset = atoi(StringUtils::Tokenize(sValue,",",pos).c_str());
				if( abs(YOffset)>m_rSpacing.Y )  // Just in case the offset is too much
					YOffset = YOffset<0 ? m_rSpacing.Y*-1 : m_rSpacing.Y;
				m_rSpacing.X += XOffset;
				m_rSpacing.Width -= XOffset;
				m_rSpacing.Y += YOffset;
				m_rSpacing.Height -= YOffset;
			}
		}
		else
			iSpacingParameter=0;
	}


	m_sSize_Regen_Data = StringUtils::itos(m_pRow_Size->Width_get()) + ","
		+ StringUtils::itos(m_pRow_Size->Height_get()) + ","
		+ StringUtils::itos(m_sScale.Width) + ","
		+ StringUtils::itos(m_sScale.Height) + ","
		+ StringUtils::itos(m_pRow_Skin->PK_Skin_get()) + ","
		+ StringUtils::itos(m_pRow_UI->PK_UI_get()) + ","
		+ StringUtils::itos(m_iRotation) 
		+ StringUtils::itos(m_rSpacing.X) + "," 
		+ StringUtils::itos(m_rSpacing.Y) + "," 
		+ StringUtils::itos(m_rSpacing.Width) + "," 
		+ StringUtils::itos(m_rSpacing.Height) + "," 
		+ (m_bUseAlphaBlending ? "A" : "N") + "," 
		+ (m_bUseMask ? "M" : "N") + "," 
		+ (m_bUseOCG ? ",OCG" : ",NO_OCG");

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"OrbiterGen::CheckRegen %d now <%s%> was <%s>",
		m_pRow_Orbiter->PK_Orbiter_get(),m_sSize_Regen_Data.c_str(),m_pRow_Orbiter->Size_get().c_str());

	if( m_pRow_Orbiter->Size_get()!=m_sSize_Regen_Data && m_map_PK_DesignObj_SoleScreenToGen.size()==0 )
	{
		if( m_pRow_Orbiter->Size_get().size()==0  )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"new orbiter size was 0");
			m_bNewOrbiter=true;
		}

		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Regenerating all: Orbiter data changed from %s to %s",m_pRow_Orbiter->Size_get().c_str(),m_sSize_Regen_Data.c_str());
		string sSQL = "DELETE FROM CachedScreens WHERE FK_Orbiter=" + StringUtils::itos(m_pRow_Orbiter->PK_Orbiter_get());
		m_spDatabase_pluto_main->threaded_db_wrapper_query(sSQL);
	}

	if( !m_bNewOrbiter )
	{
		vector<Row_Room *> vectRow_Room;
		m_spDatabase_pluto_main->Room_get()->GetRows("WHERE 1=1 LIMIT 1",&vectRow_Room);
		
		vector<Row_Users *> vectRow_Users;
		m_spDatabase_pluto_main->Users_get()->GetRows("WHERE 1=1 LIMIT 1",&vectRow_Users);

		if( vectRow_Room.size()==0 || vectRow_Users.size()==0 || m_pRow_Device->FK_Room_get()==0 )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"new orbiter no rooms/users/room specified");
			m_bNewOrbiter = true;
		}
	}

	pRow_Device_DeviceData = m_spDatabase_pluto_main->Device_DeviceData_get()->GetRow(m_pRow_Device->PK_Device_get(),DEVICEDATA_ScreenWidth_CONST);
	if( !pRow_Device_DeviceData )
	{
		pRow_Device_DeviceData = m_spDatabase_pluto_main->Device_DeviceData_get()->AddRow();
		pRow_Device_DeviceData->FK_Device_set(m_pRow_Device->PK_Device_get());
		pRow_Device_DeviceData->FK_DeviceData_set(DEVICEDATA_ScreenWidth_CONST);
	}
	if( m_iRotation==90 || m_iRotation==270 )
		pRow_Device_DeviceData->IK_DeviceData_set( StringUtils::itos(m_pRow_Size->Height_get()) );
	else
		pRow_Device_DeviceData->IK_DeviceData_set( StringUtils::itos(m_pRow_Size->Width_get()) );
	m_spDatabase_pluto_main->Device_DeviceData_get()->Commit();

	pRow_Device_DeviceData = m_spDatabase_pluto_main->Device_DeviceData_get()->GetRow(m_pRow_Device->PK_Device_get(),DEVICEDATA_ScreenHeight_CONST);
	if( !pRow_Device_DeviceData )
	{
		pRow_Device_DeviceData = m_spDatabase_pluto_main->Device_DeviceData_get()->AddRow();
		pRow_Device_DeviceData->FK_Device_set(m_pRow_Device->PK_Device_get());
		pRow_Device_DeviceData->FK_DeviceData_set(DEVICEDATA_ScreenHeight_CONST);
	}
	if( m_iRotation==90 || m_iRotation==270 )
		pRow_Device_DeviceData->IK_DeviceData_set( StringUtils::itos(m_pRow_Size->Width_get()) );
	else
		pRow_Device_DeviceData->IK_DeviceData_set( StringUtils::itos(m_pRow_Size->Height_get()) );
	m_spDatabase_pluto_main->Device_DeviceData_get()->Commit();

	// m_sizeScreen is the unscaled resolution
	m_sizeScreen = new PlutoSize(m_pRow_Size->Width_get() * 1000 / m_sScale.Width,m_pRow_Size->Height_get() * 1000 / m_sScale.Height);
	m_iPK_DesignObj_Screen = GetDesignObjFromScreen(m_pRow_Screen_MainMenu)->PK_DesignObj_get();

	// UI2 is always 16:9 and needs to be edge-to-edge, so we will ignore the normal scaling rules and 
	// always scale the full image
	if( m_pRow_UI->PK_UI_get()==UI_V2_Normal_Horizontal_16_9_CONST )
	{
		// This is always 16:9, so whatever the width is needs to be scaled accordingly
		m_sScale.Width = m_pRow_Size->Width_get() * 1000 / (m_sizeScreen->Height * 16 / 9);
		m_sizeScreen->Width = m_pRow_Size->Width_get() * 1000 / m_sScale.Width;
	}

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"OrbiterGen before adj spacing X %d Y %d W %d H %d scale W %d H %d",
		m_rSpacing.X,m_rSpacing.Y,m_rSpacing.Width,m_rSpacing.Height,m_sScale.Width,m_sScale.Height);
#endif

	m_sScale_Orig = m_sScale;
	// See if we need to reduce the scaling because of borders around the screen
	if( m_rSpacing.X || m_rSpacing.Width )
		m_sScale.Width = int((double) m_sScale.Width * (100-iSpacingParameter) / 100);

	if( m_rSpacing.Y || m_rSpacing.Height )
		m_sScale.Height = int((double) m_sScale.Height * (100-iSpacingParameter) / 100);

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"OrbiterGen after adj spacing X %d Y %d W %d H %d scale W %d H %d",
		m_rSpacing.X,m_rSpacing.Y,m_rSpacing.Width,m_rSpacing.Height,m_sScale.Width,m_sScale.Height);
#endif

	int i=0;

	/*  This is tricky with Mysql since there are no full joins or sub-queries.  We have to do it in multiple passes
	string sql = string("SELECT PK_Room,Room.Description AS RoomDescription,") \
	+	"PK_Device,X.Description AS DeviceDescription,FK_Icon FROM Room " \
	+	"FULL OUTER JOIN " \
	+	"(SELECT * FROM Device WHERE FK_DeviceTemplate=" + StringUtils::itos(DeviceTemplate_Entertain_Unit_CONST) + ") AS X " \
	+	"ON FK_Room=PK_Room " \
	+	"WHERE (Room.FK_Installation=" + StringUtils::itos(m_pRow_Device->FK_Installation_get()) + " Or Room.FK_Installation IS NULL) " \
	+	"AND (X.FK_Installation=" + StringUtils::itos(m_pRow_Device->FK_Installation_get()) + " Or X.FK_Installation IS NULL)";
	*/

	// First put this in a list.  We really need a deque so we can access the members by their numeric
	// position, but we also need to sort it, and deque's don't implement sorting
	list<LocationInfo *> listLocationInfo;

	// If a User has the flag 'requires pin' set to true, then unless the user is specifically allowed
	// to use this orbiter by having a record in Room_Users with FK_Room=NULL, then this user requires a pin
	vector<Row_Users *> vectRow_Users;
	m_spDatabase_pluto_main->Users_get()->GetRows("LEFT JOIN Room_Users ON FK_Users=PK_Users AND FK_Orbiter=" + StringUtils::itos(m_pRow_Device->PK_Device_get()) + " WHERE RequirePinToSelect=1 AND FK_Users IS NULL",
		&vectRow_Users);
	for(size_t s=0;s<vectRow_Users.size();++s)
		m_vectPK_Users_RequiringPIN.push_back( vectRow_Users[s]->PK_Users_get() );

	vector<Row_Room *> vectRow_Room;
	m_spDatabase_pluto_main->Room_get()->GetRows("HideFromOrbiter=0 AND FK_Installation=" + StringUtils::itos(m_pRow_Device->FK_Installation_get()),&vectRow_Room);
	for(size_t s=0;s<vectRow_Room.size();++s)
	{
		Row_Room *pRow_Room = vectRow_Room[s];
		vector<Row_Room_Users *> vectRow_Room_Users;
		pRow_Room->Room_Users_FK_Room_getrows(&vectRow_Room_Users);

		// If there are records in Room_Users for this room, then this room has restrictions.  There must be a
		// record in the database with FK_Orbiter=this to explicitly allow this orbiter, or with
		// FK_Orbiter=NULL to allow all orbiters
		if(vectRow_Room_Users.size()) 
		{
			size_t s;
			for(s=0;s<vectRow_Room_Users.size();++s)
			{
				Row_Room_Users *pRow_Room_Users = vectRow_Room_Users[s];
				if( pRow_Room_Users->FK_Orbiter_isNull() ||
						pRow_Room_Users->FK_Orbiter_get() == m_pRow_Device->PK_Device_get() )
					break; // this one is good
			}
			if( s>=vectRow_Room_Users.size() )
				continue;  // we didn't find anything matching
		}

		LocationInfo *li = new LocationInfo();
		li->Description = pRow_Room->Description_get();
		li->PK_EntertainArea = 0;
		li->PK_Room = pRow_Room->PK_Room_get();
		li->drIcon=NULL;
		if( !pRow_Room->FK_Icon_isNull() )
			li->drIcon = pRow_Room->FK_Icon_getrow();

		// If there are record in Room_Users for this room, and users are specified, then
		// only those users can access this room
		for(size_t s=0;s<vectRow_Room_Users.size();++s)
		{
			Row_Room_Users *pRow_Room_Users = vectRow_Room_Users[s];
			if( !pRow_Room_Users->FK_Users_isNull() )
				li->m_vectAllowedUsers.push_back(pRow_Room_Users->FK_Users_get());
		}

		listLocationInfo.push_back(li);
	}

	m_sMainMenu="";
	m_dwPK_Device_LocalAppServer=m_dwPK_Device_LocalMediaPlayer=m_dwPK_Device_LocalOsdIRReceiver=m_dwPK_Device_LocalOsdVfdLcd=0;

	vector<Row_Device *> vectRow_Device;
	m_spDatabase_pluto_main->Device_get()->GetRows(DEVICE_FK_INSTALLATION_FIELD + string("=") + StringUtils::itos(m_pRow_Device->FK_Installation_get()),&vectRow_Device);
	for(size_t s=0;s<vectRow_Device.size();++s)
	{
		Row_Device *pRow_Device = vectRow_Device[s];
		Row_DeviceTemplate *pRow_DeviceTemplate = pRow_Device->FK_DeviceTemplate_getrow();
		if( !pRow_DeviceTemplate )
			continue; // shouldn't happen
		switch(pRow_DeviceTemplate->FK_DeviceCategory_get())
		{
		case DEVICECATEGORY_DCE_Router_CONST:
			m_dwPK_Device_Router = pRow_Device->PK_Device_get();
			break;
		case DEVICECATEGORY_Lighting_Plugins_CONST:
			m_dwPK_Device_LightingPlugIn = pRow_Device->PK_Device_get();
			break;
		case DEVICECATEGORY_Climate_Plugins_CONST:
			m_dwPK_Device_ClimatePlugIn = pRow_Device->PK_Device_get();
			break;
		case DEVICECATEGORY_Media_Plugins_CONST:
			m_dwPK_Device_MediaPlugIn = pRow_Device->PK_Device_get();
			break;
		case DEVICECATEGORY_Telecom_Plugins_CONST:
			m_dwPK_Device_TelecomPlugIn = pRow_Device->PK_Device_get();
			break;
		case DEVICECATEGORY_Security_Plugins_CONST:
			m_dwPK_Device_SecurityPlugIn = pRow_Device->PK_Device_get();
			break;
		case DEVICECATEGORY_Datagrid_Plugins_CONST:
			m_dwPK_Device_DatagridPlugIn = pRow_Device->PK_Device_get();
			break;
		case DEVICECATEGORY_General_Info_Plugins_CONST:
			m_dwPK_Device_GeneralInfoPlugIn = pRow_Device->PK_Device_get();
			break;
		case DEVICECATEGORY_Infrared_Plugins_CONST:
			m_dwPK_Device_InfraredPlugIn = pRow_Device->PK_Device_get();
			break;
		case DEVICECATEGORY_Event_Plugins_CONST:
			m_dwPK_Device_EventPlugIn = pRow_Device->PK_Device_get();
			break;
		case DEVICECATEGORY_Orbiter_Plugins_CONST:
			m_dwPK_Device_OrbiterPlugIn = pRow_Device->PK_Device_get();
			break;
		case DEVICECATEGORY_Plug_and_Play_Plugins_CONST:
			m_dwPK_Device_PlugAndPlayPlugIn = pRow_Device->PK_Device_get();
			break;
		case DEVICECATEGORY_App_Server_CONST:
			if( CommonControlledVia(m_pRow_Device,pRow_Device) )
				m_dwPK_Device_LocalAppServer = pRow_Device->PK_Device_get();
			break;
		case DEVICECATEGORY_Media_Players_CONST:
			if( CommonControlledVia(m_pRow_Device,pRow_Device) &&
				pRow_Device->FK_DeviceTemplate_get()==DEVICETEMPLATE_Xine_Player_CONST )  // For now only xine is a general purpose player
					m_dwPK_Device_LocalMediaPlayer = pRow_Device->PK_Device_get();
			break;
		case DEVICECATEGORY_Infrared_Receivers_CONST:
		case DEVICECATEGORY_LIRC_Drivers_CONST:
			if( CommonControlledVia(m_pRow_Device,pRow_Device) )
				m_dwPK_Device_LocalOsdIRReceiver = pRow_Device->PK_Device_get();
			break;
		case DEVICECATEGORY_LCDVFD_Displays_CONST:
			if( CommonControlledVia(m_pRow_Device,pRow_Device) )
				m_dwPK_Device_LocalOsdVfdLcd = pRow_Device->PK_Device_get();
			break;
		};
	}

	// We need to know which objects have arrays that are room specific so we can generate
	// one for each location
	string sql = "select DISTINCT Parent.FK_DesignObj FROM DesignObjVariation_DesignObjParameter"
		" JOIN DesignObjVariation ON DesignObjVariation.PK_DesignObjVariation=FK_DesignObjVariation"
		" JOIN DesignObj ON PK_DesignObj=DesignObjVariation.FK_DesignObj"
		" JOIN DesignObjVariation_DesignObj ON FK_DesignObj_Child=PK_DesignObj"
		" JOIN DesignObjVariation as Parent ON FK_DesignObjVariation_Parent=Parent.PK_DesignObjVariation"
		" WHERE FK_DesignObjParameter=11 AND FK_DesignObjType=3 AND Value IN ('1','2','3','4','5','16','21')";
	PlutoSqlResult result_set_array;
	DB_ROW row;
	if( (result_set_array.r=m_spDatabase_pluto_main->db_wrapper_query_result(sql)) )
	{
		while ((row = db_wrapper_fetch_row(result_set_array.r)))
			m_mapDesignObj_WithArrays[ atoi(row[0]) ] = true;
	}

	for(size_t s=0;s<vectRow_Room.size();++s)
	{
		Row_Room *pRow_Room = vectRow_Room[s];
		vector<Row_EntertainArea *> vectRow_EntertainArea;
		pRow_Room->EntertainArea_FK_Room_getrows(&vectRow_EntertainArea);
		LocationInfo *li=NULL;
		for(size_t s=0;s<vectRow_EntertainArea.size();++s)
		{
			Row_EntertainArea *pRow_EntertainArea = vectRow_EntertainArea[s];

			// Find the entry for the room
			list<LocationInfo *>::iterator it;
			for(it=listLocationInfo.begin();it!=listLocationInfo.end();++it)
			{
				li = (*it);
				if( li->PK_Room==pRow_EntertainArea->FK_Room_get() )
				{
					if( li->PK_EntertainArea==0 )
					{
						break;
					}
				}
				li = NULL; // This wasn't a valid match
			}

			if( !li )
			{
				li = new LocationInfo();
				li->PK_Room = pRow_EntertainArea->FK_Room_get();
				li->drIcon=NULL;
				listLocationInfo.push_back(li);
			}

			li->Description = pRow_EntertainArea->Description_get();
			li->PK_EntertainArea = pRow_EntertainArea->PK_EntertainArea_get();

			Row_Device *pRow_Device_MediaDirector = NULL;
			Row_Device *pRow_Device_Orbiter = NULL;
			vector<Row_Device_EntertainArea *> vectRow_Device_EntertainArea;
			pRow_EntertainArea->Device_EntertainArea_FK_EntertainArea_getrows(&vectRow_Device_EntertainArea);
			for(size_t s=0;s<vectRow_Device_EntertainArea.size();++s)
			{
				Row_Device_EntertainArea *pRow_Device_EntertainArea = vectRow_Device_EntertainArea[s];
				Row_Device *pRow_Device = pRow_Device_EntertainArea->FK_Device_getrow();
				if( pRow_Device && pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get()==DEVICECATEGORY_Media_Director_CONST )
					pRow_Device_MediaDirector = pRow_Device_EntertainArea->FK_Device_getrow();
				if( pRow_Device && pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get()==DEVICECATEGORY_Standard_Orbiter_CONST )
					pRow_Device_Orbiter = pRow_Device_EntertainArea->FK_Device_getrow();
			}

			// There's a media director here.  Match up all the virtual devices
			if( pRow_Device_MediaDirector )
			{
				li->m_dwPK_Device_MediaDirector = pRow_Device_MediaDirector->PK_Device_get();
				MatchChildDevicesToLocation(li,pRow_Device_MediaDirector);
			}
			if( li->m_dwPK_Device_Orbiter==-1 && pRow_Device_Orbiter )
				li->m_dwPK_Device_Orbiter = pRow_Device_Orbiter->PK_Device_get();
		}
	}

	FixupSpecialOrbiters(listLocationInfo);

	listLocationInfo.sort(LocationComparer);

	// First add the location numbers so they're available from the beginning, and put them in 
	// the deque.  We only used a list up till now so we could sort it.
	list<LocationInfo *>::iterator it_loc;
	for(it_loc=listLocationInfo.begin();it_loc!=listLocationInfo.end();++it_loc)
	{
		LocationInfo *li = (*it_loc);
		li->iLocation=i++;
		m_dequeLocation.push_back(li);
	}

	if( m_dequeLocation.size()==0 )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Warning: No entertainment areas and no rooms");
	
		// We've got an empty house with no rooms and no entertainment areas.  Just add something
		LocationInfo *li = new LocationInfo();
		li->iLocation=0;
		li->Description = "Anywhere";
		li->PK_EntertainArea = 0;
		li->PK_Room = 0;
		li->drIcon=NULL;
		m_dequeLocation.push_back(li);					
	}

	// The screen saver is not location specific
	m_pRow_Room = NULL;
	m_pRow_EntertainArea = NULL;
	m_iLocation = 0;

	m_sScreenSaveMenu = StringUtils::itos( GetDesignObjFromScreen(m_pRow_Screen_ScreenSaver)->PK_DesignObj_get()) + ".0.0";
	if( m_pRow_Screen_ScreenSaver!=m_pRow_Screen_MainMenu )
	{
		Row_DesignObj *pRow_DesignObj=GetDesignObjFromScreen(m_pRow_Screen_ScreenSaver);
		if(pRow_DesignObj && (m_map_PK_DesignObj_SoleScreenToGen.size()==0 || m_map_PK_DesignObj_SoleScreenToGen[pRow_DesignObj->PK_DesignObj_get()]==true) )
			DesignObj_Generator *ocDesignObj3 = new DesignObj_Generator(this,pRow_DesignObj,PlutoRectangle(0,0,0,0),NULL,true,false);
	}

	m_iLocation_Initial=0;
	DequeLocationInfo::iterator itd;
	for(itd=m_dequeLocation.begin();itd!=m_dequeLocation.end();++itd)
	{
		LocationInfo *li = (*itd);
		if( (pRow_EntertainArea_Default && li->PK_EntertainArea==pRow_EntertainArea_Default->PK_EntertainArea_get()) ||
			(!pRow_EntertainArea_Default && li->PK_Room==m_pRow_Device->FK_Room_get()) || m_dequeLocation.size()==1 )
		{
			m_sMainMenu = StringUtils::itos( GetDesignObjFromScreen(m_pRow_Screen_MainMenu)->PK_DesignObj_get()) + "." + StringUtils::itos(li->iLocation) + ".0";
			m_sSleepingMenu = StringUtils::itos( GetDesignObjFromScreen(m_pRow_Screen_Sleeping)->PK_DesignObj_get()) + "." + StringUtils::itos(li->iLocation) + ".0";
			m_iLocation_Initial = li->iLocation;
		}

		m_pRow_Room = m_spDatabase_pluto_main->Room_get()->GetRow(li->PK_Room);
		m_pRow_EntertainArea = li->PK_EntertainArea>0 ? m_spDatabase_pluto_main->EntertainArea_get()->GetRow(li->PK_EntertainArea) : NULL;
		m_iLocation = li->iLocation;
		DesignObj_Generator *ocDesignObj = new DesignObj_Generator(this,GetDesignObjFromScreen(m_pRow_Screen_MainMenu),PlutoRectangle(0,0,0,0),NULL,true,false);
		if( m_pRow_Screen_Sleeping!=m_pRow_Screen_MainMenu )
			DesignObj_Generator *ocDesignObj2 = new DesignObj_Generator(this,GetDesignObjFromScreen(m_pRow_Screen_Sleeping),PlutoRectangle(0,0,0,0),NULL,true,false);
	}

	if( m_sMainMenu=="" )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"WARNING: No valid main menu found.  Check that a default room and entertainment area are specified in the Orbiter table.  Using the first entry as the main menu.");
		
		LocationInfo *li = m_dequeLocation.front();
		m_sMainMenu = StringUtils::itos(GetDesignObjFromScreen(m_pRow_Screen_MainMenu)->PK_DesignObj_get()) + "." + StringUtils::itos(li->iLocation) + ".0";
		m_sSleepingMenu = StringUtils::itos(GetDesignObjFromScreen(m_pRow_Screen_Sleeping)->PK_DesignObj_get()) + "." + StringUtils::itos(li->iLocation) + ".0";
		m_sScreenSaveMenu = StringUtils::itos(GetDesignObjFromScreen(m_pRow_Screen_ScreenSaver)->PK_DesignObj_get()) + "." + StringUtils::itos(li->iLocation) + ".0";
		m_iLocation_Initial = li->iLocation;
	}

bool bFirstLoop=true;
loop_to_keep_looking_for_objs_to_include:
	list<Row_DesignObj *> alNewDesignObjsToGenerate;  // Have to cache the list because I can't modify htgeneratedscreens while enumerating
	list< pair<Row_DesignObj *,int> > alNewDesignObjLocationsToGenerate; 

	map<string,listDesignObj_Generator *>::iterator itgs;

	bool bKeepLooking=true;
	while(bKeepLooking)
	{
		bKeepLooking=false;  // We'll reset it if we found screens and want to restart the search to find more screens that may be deeper
		int iNumGeneratedScreens = int(m_htGeneratedScreens.size());
		for(itgs=m_htGeneratedScreens.begin();itgs!=m_htGeneratedScreens.end();++itgs)
		{
			listDesignObj_Generator *o = (*itgs).second;
			if( !o )
				continue;
			Row_DesignObj *pRow_DesignObj = NULL;

			if( o->size()>0 )
			{
				DesignObj_Generator *oco = o->front();
				// Be sure that if this is location specific we have added for every location
				if( oco->m_bLocationSpecific && StringUtils::EndsWith(itgs->first,".0") )
				{
					DequeLocationInfo::iterator itd;
					for(itd=m_dequeLocation.begin();itd!=m_dequeLocation.end();++itd)
					{
						LocationInfo *li = (*itd);
						if( li->iLocation==0 )
							continue;  // Only matters for non-zero locations

						if( m_htGeneratedScreens.find( StringUtils::itos( oco->m_pRow_DesignObj->PK_DesignObj_get() ) + "." + StringUtils::itos( li->iLocation ) )==m_htGeneratedScreens.end() )
							alNewDesignObjLocationsToGenerate.push_back( make_pair<Row_DesignObj *,int> (oco->m_pRow_DesignObj, li->iLocation) );
					}
				}
				if( oco->m_bUsingCache )
				{
					// We're going to have to lookup the Row_DesignObj manually, and we're going to need to search for goto's in all the children
					// since this was built from cache, and so the children were not created when the object was created
					pRow_DesignObj = m_spDatabase_pluto_main->DesignObj_get()->GetRow(atoi(oco->m_ObjectID.c_str()));
					SearchForGotos(oco);
					if( iNumGeneratedScreens!=m_htGeneratedScreens.size() )
					{
						bKeepLooking=true;
						break; // We added some more, start again until we've got them all
					}
				}
				else
					pRow_DesignObj = oco->m_pRow_DesignObj;
			}
			if( pRow_DesignObj!=NULL)
			{
				vector<Row_DesignObj *> vectros;
				pRow_DesignObj->DesignObj_FK_DesignObj_IncludeIfOtherIncluded_getrows(&vectros);
				for(size_t s=0;s<vectros.size();++s)
				{
					Row_DesignObj *m_pRow_DesignObjDependancy = vectros[s];
					alNewDesignObjsToGenerate.push_back(m_pRow_DesignObjDependancy);
				}
			}
		}
	}

	for(map<int,int>::iterator it=m_mapDesignObj.begin();it!=m_mapDesignObj.end();++it)
	{
		Row_Screen *pRow_Screen = m_spDatabase_pluto_main->Screen_get()->GetRow(it->first);
		if( pRow_Screen && pRow_Screen->AlwaysInclude_get() )
		{
			Row_DesignObj *drNewDesignObj = m_spDatabase_pluto_main->DesignObj_get()->GetRow(it->second);
			alNewDesignObjsToGenerate.push_back(drNewDesignObj);
		}
	}

	sql = string("SELECT ") + COMMANDGROUP_COMMAND_COMMANDPARAMETER_IK_COMMANDPARAMETER_TABLE_FIELD + " FROM " +
		EVENTHANDLER_TABLE + " JOIN " + COMMANDGROUP_COMMAND_TABLE + " ON " + COMMANDGROUP_COMMAND_FK_COMMANDGROUP_TABLE_FIELD + "=" +
		EVENTHANDLER_FK_COMMANDGROUP_TABLE_FIELD + " JOIN " + COMMANDGROUP_COMMAND_COMMANDPARAMETER_TABLE + " ON " +
		COMMANDGROUP_COMMAND_COMMANDPARAMETER_FK_COMMANDGROUP_COMMAND_FIELD + "=" + COMMANDGROUP_COMMAND_PK_COMMANDGROUP_COMMAND_FIELD +
		" WHERE (" + EVENTHANDLER_FK_INSTALLATION_TABLE_FIELD + " IS NULL OR " + EVENTHANDLER_FK_INSTALLATION_TABLE_FIELD + "=" + StringUtils::itos(m_pRow_Device->FK_Installation_get()) + ") AND " + COMMANDGROUP_COMMAND_FK_COMMAND_FIELD + 
		"=" + StringUtils::itos(COMMAND_Goto_Screen_CONST) + " AND " + COMMANDGROUP_COMMAND_COMMANDPARAMETER_FK_COMMANDPARAMETER_FIELD + "=" + StringUtils::itos(COMMANDPARAMETER_PK_DesignObj_CONST);

	PlutoSqlResult result_set3b;
	if( (result_set3b.r=m_spDatabase_pluto_main->db_wrapper_query_result(sql)) )
	{
		while ((row = db_wrapper_fetch_row(result_set3b.r)))
		{
			try
			{
				Row_DesignObj *drNewDesignObj = GetDesignObjFromScreen(atoi(row[0]));
				alNewDesignObjsToGenerate.push_back(drNewDesignObj);
			}
			catch(...)
			{}
		}
	}

	sql = string("SELECT ") + COMMANDGROUP_COMMAND_COMMANDPARAMETER_IK_COMMANDPARAMETER_TABLE_FIELD + " FROM " +
		EVENTHANDLER_TABLE + " JOIN " + COMMANDGROUP_COMMAND_TABLE + " ON " + COMMANDGROUP_COMMAND_FK_COMMANDGROUP_TABLE_FIELD + "=" +
		EVENTHANDLER_FK_COMMANDGROUP_TABLE_FIELD + " JOIN " + COMMANDGROUP_COMMAND_COMMANDPARAMETER_TABLE + " ON " +
		COMMANDGROUP_COMMAND_COMMANDPARAMETER_FK_COMMANDGROUP_COMMAND_FIELD + "=" + COMMANDGROUP_COMMAND_PK_COMMANDGROUP_COMMAND_FIELD +
		" WHERE (" + EVENTHANDLER_FK_INSTALLATION_TABLE_FIELD + " IS NULL OR " + EVENTHANDLER_FK_INSTALLATION_TABLE_FIELD + "=" + StringUtils::itos(m_pRow_Device->FK_Installation_get()) + ") AND " + COMMANDGROUP_COMMAND_FK_COMMAND_FIELD + 
		"=" + StringUtils::itos(COMMAND_Goto_DesignObj_CONST) + " AND " + COMMANDGROUP_COMMAND_COMMANDPARAMETER_FK_COMMANDPARAMETER_FIELD + "=" + StringUtils::itos(COMMANDPARAMETER_PK_DesignObj_CONST);

	PlutoSqlResult result_set3;
	if( (result_set3.r=m_spDatabase_pluto_main->db_wrapper_query_result(sql)) )
	{
		while ((row = db_wrapper_fetch_row(result_set3.r)))
		{
			try
			{
				Row_DesignObj *drNewDesignObj = m_spDatabase_pluto_main->DesignObj_get()->GetRow(atoi(row[0]));
				alNewDesignObjsToGenerate.push_back(drNewDesignObj);
			}
			catch(...)
			{}
		}
	}

	vector<Row_DesignObj *> vectRow_DesignObj_AlwaysInclude;
	m_spDatabase_pluto_main->DesignObj_get()->GetRows("AlwaysInclude=1",&vectRow_DesignObj_AlwaysInclude);
	for(size_t s=0;s<vectRow_DesignObj_AlwaysInclude.size();++s)
		alNewDesignObjsToGenerate.push_back(vectRow_DesignObj_AlwaysInclude[s]);

	// Find all records for objects used as remotes for devices within this installation.  Ignore remotes
	// that only apply to devices that are orbiters (FK_DeviceCategory NOT IN (2,3,5)) unless it's for this orbiter
	sql = string("select DeviceTemplate_MediaType_DesignObj.FK_Screen,Device.FK_DesignObj,FK_DesignObj_Popup,FK_Screen_FileList,FK_Screen_OSD,FK_Screen_Alt,FK_Screen_Alt_OSD,FK_Screen_OSD_Speed,FK_Screen_OSD_Track") + 
		" FROM Device " +
		" INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate " +
		" INNER JOIN DeviceTemplate_MediaType ON DeviceTemplate_MediaType.FK_DeviceTemplate=PK_DeviceTemplate " + 
		" INNER JOIN DeviceTemplate_MediaType_DesignObj ON FK_DeviceTemplate_MediaType=PK_DeviceTemplate_MediaType " +
		" WHERE FK_Installation=" + StringUtils::itos(m_pRow_Device->FK_Installation_get()) +
		" AND (FK_DeviceCategory NOT IN (2,3,5) or PK_Device=" + StringUtils::itos(m_pRow_Device->PK_Device_get()) + ");";

	PlutoSqlResult result_set4;
	if( (result_set4.r=m_spDatabase_pluto_main->db_wrapper_query_result(sql)) )
	{
		while ((row = db_wrapper_fetch_row(result_set4.r)))
		{
			try
			{
				if( row[0] ) // FK_Screen
				{
					Row_DesignObj *drNewDesignObj = GetDesignObjFromScreen(atoi(row[0]));
					if( !drNewDesignObj )
						LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find FK_Screen: %s",row[0]);
					else
						alNewDesignObjsToGenerate.push_back(drNewDesignObj);
				}
				if( row[1] ) // Device.FK_DesignObj
				{
					Row_DesignObj *drNewDesignObj = m_spDatabase_pluto_main->DesignObj_get()->GetRow(atoi(row[1]));
					if( !drNewDesignObj )
						LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find Device.FK_DesignObj: %s",row[1]);
					else
						alNewDesignObjsToGenerate.push_back(drNewDesignObj);
				}
				if( row[2] ) // FK_DesignObj_Popup
				{
					Row_DesignObj *drNewDesignObj = m_spDatabase_pluto_main->DesignObj_get()->GetRow(atoi(row[2]));
					if( !drNewDesignObj )
						LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find FK_DesignObj_Popup: %s",row[2]);
					else
						alNewDesignObjsToGenerate.push_back(drNewDesignObj);
					m_mapPopups_set(drNewDesignObj->PK_DesignObj_get(),true);
				}
				if( row[3] ) // FK_Screen_FileList
				{
					Row_DesignObj *drNewDesignObj = GetDesignObjFromScreen(atoi(row[3]));
					if( !drNewDesignObj )
						LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find FK_Screen_FileList: %s",row[3]);
					else
						alNewDesignObjsToGenerate.push_back(drNewDesignObj);
				}
				if( row[4] && m_bIsOSD ) // FK_Screen_OSD
				{
					Row_DesignObj *drNewDesignObj = GetDesignObjFromScreen(atoi(row[4]));
					if( !drNewDesignObj )
						LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find FK_Screen_OSD: %s",row[4]);
					else
						alNewDesignObjsToGenerate.push_back(drNewDesignObj);
					m_mapPopups_set(drNewDesignObj->PK_DesignObj_get(),true);
				}
				if( row[5] ) // FK_Screen_Alt
				{
					Row_DesignObj *drNewDesignObj = GetDesignObjFromScreen(atoi(row[5]));
					if( !drNewDesignObj )
						LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find FK_Screen_Alt: %s",row[5]);
					else
						alNewDesignObjsToGenerate.push_back(drNewDesignObj);
					m_mapPopups_set(drNewDesignObj->PK_DesignObj_get(),true);
				}
				if( row[6] && m_bIsOSD ) // FK_Screen_Alt_OSD
				{
					Row_DesignObj *drNewDesignObj = GetDesignObjFromScreen(atoi(row[6]));
					if( !drNewDesignObj )
						LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find FK_Screen_Alt_OSD: %s",row[6]);
					else
						alNewDesignObjsToGenerate.push_back(drNewDesignObj);
					m_mapPopups_set(drNewDesignObj->PK_DesignObj_get(),true);
				}
				if( row[7] && m_bIsOSD ) // FK_Screen_OSD_Speed
				{
					Row_DesignObj *drNewDesignObj = GetDesignObjFromScreen(atoi(row[7]));
					if( !drNewDesignObj )
						LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find FK_Screen_OSD_Speed: %s",row[7]);
					else
						alNewDesignObjsToGenerate.push_back(drNewDesignObj);
					m_mapPopups_set(drNewDesignObj->PK_DesignObj_get(),true);
				}
				if( row[8] && m_bIsOSD ) // FK_Screen_OSD_Track
				{
					Row_DesignObj *drNewDesignObj = GetDesignObjFromScreen(atoi(row[8]));
					if( !drNewDesignObj )
						LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find FK_Screen_OSD_Speed: %s",row[8]);
					else
						alNewDesignObjsToGenerate.push_back(drNewDesignObj);
					m_mapPopups_set(drNewDesignObj->PK_DesignObj_get(),true);
				}
			}
			catch(...)
			{}
		}
	}

	sql = "select FK_Screen,FK_DesignObj_Popup,FK_Screen_FileList,FK_Screen_OSD,FK_Screen_Alt,FK_Screen_Alt_OSD,FK_Screen_OSD_Speed,FK_Screen_OSD_Track FROM MediaType_DesignObj"
		" WHERE FK_Skin IS NULL OR FK_Skin=" + StringUtils::itos(m_pRow_Skin->PK_Skin_get());

	PlutoSqlResult result_set4b;
	if( (result_set4b.r=m_spDatabase_pluto_main->db_wrapper_query_result(sql)) )
	{
		while ((row = db_wrapper_fetch_row(result_set4b.r)))
		{
			try
			{
				if( row[0] )  // FK_Screen
				{
					Row_DesignObj *drNewDesignObj = GetDesignObjFromScreen(atoi(row[0]));
					if( !drNewDesignObj )
						LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find FK_Screen: %s",row[0]);
					else
						alNewDesignObjsToGenerate.push_back(drNewDesignObj);
				}
				if( row[1] ) // FK_DesignObj_Popup
				{
					Row_DesignObj *drNewDesignObj = m_spDatabase_pluto_main->DesignObj_get()->GetRow(atoi(row[1]));
					if( !drNewDesignObj )
						LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find FK_DesignObj_Popup: %s",row[1]);
					else
					{
						alNewDesignObjsToGenerate.push_back(drNewDesignObj);
						m_mapPopups_set(drNewDesignObj->PK_DesignObj_get(),true);
					}
				}
				if( row[2] ) // FK_Screen_FileList
				{
					Row_DesignObj *drNewDesignObj = GetDesignObjFromScreen(atoi(row[2]));
					if( !drNewDesignObj )
						LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find FK_Screen_FileList: %s",row[2]);
					else
						alNewDesignObjsToGenerate.push_back(drNewDesignObj);
				}
				if( row[3] && m_bIsOSD ) // FK_Screen_OSD
				{
					Row_DesignObj *drNewDesignObj = GetDesignObjFromScreen(atoi(row[3]));
					if( !drNewDesignObj )
						cerr << "Cannot find FK_Screen_OSD: " << row[3] << endl;
					else
					{
						alNewDesignObjsToGenerate.push_back(drNewDesignObj);
					}
				}
				if( row[4] ) // FK_Screen_Alt
				{
					Row_DesignObj *drNewDesignObj = GetDesignObjFromScreen(atoi(row[4]));
					if( !drNewDesignObj )
						LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find FK_Screen_Alt: %s",row[4]);
					else
					{
						alNewDesignObjsToGenerate.push_back(drNewDesignObj);
					}
				}
				if( row[5] && m_bIsOSD ) // FK_Screen_Alt_OSD
				{
					Row_DesignObj *drNewDesignObj = GetDesignObjFromScreen(atoi(row[5]));
					if( !drNewDesignObj )
						LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find FK_Screen_Alt_OSD: %s",row[5]);
					else
					{
						alNewDesignObjsToGenerate.push_back(drNewDesignObj);
					}
				}
				if( row[6] && m_bIsOSD ) // FK_Screen_OSD_Speed
				{
					Row_DesignObj *drNewDesignObj = GetDesignObjFromScreen(atoi(row[6]));
					if( !drNewDesignObj )
						LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find FK_Screen_Alt_OSD: %s",row[6]);
					else
					{
						alNewDesignObjsToGenerate.push_back(drNewDesignObj);
					}
				}
				if( row[7] && m_bIsOSD ) // FK_Screen_OSD_Track
				{
					Row_DesignObj *drNewDesignObj = GetDesignObjFromScreen(atoi(row[7]));
					if( !drNewDesignObj )
						LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find FK_Screen_Alt_OSD: %s",row[7]);
					else
					{
						alNewDesignObjsToGenerate.push_back(drNewDesignObj);
					}
				}
			}
			catch(...)
			{}
		}
	}

	// exclude dt=8 (orbiter) because that will just drag in all the main menus
	sql = "select DeviceTemplate_DesignObj.FK_DesignObj FROM DeviceTemplate_DesignObj JOIN Device ON Device.FK_DeviceTemplate=DeviceTemplate_DesignObj.FK_DeviceTemplate JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate WHERE PK_DeviceTemplate<>8 AND FK_Installation=" + StringUtils::itos(m_pRow_Device->FK_Installation_get());

	PlutoSqlResult result_set5;
	if( (result_set5.r=m_spDatabase_pluto_main->db_wrapper_query_result(sql)) )
	{
		while ((row = db_wrapper_fetch_row(result_set5.r)))
		{
			try
			{
				if( row[0] )
				{
					Row_DesignObj *drNewDesignObj = m_spDatabase_pluto_main->DesignObj_get()->GetRow(atoi(row[0]));
					if( !drNewDesignObj )
						LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find devicetempate_designobj: %s",row[0]);
					else
						alNewDesignObjsToGenerate.push_back(drNewDesignObj);
				}
			}
			catch(...)
			{}
		}
	}

	bool bUseVideoWizard=false;
	if( m_bNewOrbiter && m_bIsOSD )
	{
		bUseVideoWizard = atoi(g_DCEConfig.ReadString("UseVideoWizard").c_str())==1;
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"First time generating this orbiter.  Wizard: %d",bUseVideoWizard);
		Row_DesignObj *drNewDesignObj;
		if( bUseVideoWizard )
		{
			drNewDesignObj = GetDesignObjFromScreen(SCREEN_VideoWizard_CONST);
			m_sInitialScreen=StringUtils::itos(SCREEN_VideoWizard_CONST);
		}
		else
		{
			drNewDesignObj = GetDesignObjFromScreen(SCREEN_FirstTime_CONST);
			m_sInitialScreen=StringUtils::itos(SCREEN_FirstTime_CONST);
		}

		if( !drNewDesignObj )
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find 'first time' menu");
		else
			alNewDesignObjsToGenerate.push_back(drNewDesignObj);
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Initial screen: %s",m_sInitialScreen.c_str());

	list<Row_DesignObj *>::iterator itno;
	list<Row_DesignObj *> alNewDesignObjsToGenerate2;  // Create a second list of any new dependencies we drug in
	for(itno=alNewDesignObjsToGenerate.begin();itno!=alNewDesignObjsToGenerate.end();++itno)
	{
		Row_DesignObj *pRow_DesignObj = *itno;

		if(NULL != pRow_DesignObj)
		{
			vector<Row_DesignObj *> vectros;
			pRow_DesignObj->DesignObj_FK_DesignObj_IncludeIfOtherIncluded_getrows(&vectros);

			for(size_t s=0;s<vectros.size();++s)
			{
				Row_DesignObj *m_pRow_DesignObjDependancy = vectros[s];
				alNewDesignObjsToGenerate2.push_back(m_pRow_DesignObjDependancy);
			}
		}
	}

	for(itno=alNewDesignObjsToGenerate2.begin();itno!=alNewDesignObjsToGenerate2.end();++itno)
	{
		alNewDesignObjsToGenerate.push_back(*itno); // Merge these back in
	}

	int iNumScreensPrior = int(m_htGeneratedScreens.size());
	for(itno=alNewDesignObjsToGenerate.begin();itno!=alNewDesignObjsToGenerate.end();++itno)
	{
		Row_DesignObj *m_pRow_DesignObjDependancy = *itno;
		if( m_htGeneratedScreens.find(StringUtils::itos(m_pRow_DesignObjDependancy->PK_DesignObj_get()) + ".0")==m_htGeneratedScreens.end() )
		{
			m_iPK_DesignObj_Screen = m_pRow_DesignObjDependancy->PK_DesignObj_get();
			DesignObj_Generator *ocDesignObj = new DesignObj_Generator(this,m_pRow_DesignObjDependancy,PlutoRectangle(0,0,0,0),NULL,true,false);
			if( ocDesignObj->m_bUsingCache )
			{
				SearchForGotos(ocDesignObj);
			}
		}
	}

	// Now include all the media sort options if this it UI version 2
	Row_DesignObj *pRow_DesignObj_Array[5] = { NULL, NULL, NULL, NULL, NULL };
	if( m_pRow_UI->PK_UI_get()==UI_V2_Normal_Horizontal_16_9_CONST )
	{
		pRow_DesignObj_Array[0] = m_spDatabase_pluto_main->DesignObj_get()->GetRow(DESIGNOBJ_popFBSF_Sort_CONST);
		pRow_DesignObj_Array[1] = m_spDatabase_pluto_main->DesignObj_get()->GetRow(DESIGNOBJ_popFBSF_Genres_CONST);
		pRow_DesignObj_Array[2] = m_spDatabase_pluto_main->DesignObj_get()->GetRow(DESIGNOBJ_popFBSF_PK_MediaSubType_CONST);
		pRow_DesignObj_Array[3] = m_spDatabase_pluto_main->DesignObj_get()->GetRow(DESIGNOBJ_popFBSF_MediaSource_CONST);
		pRow_DesignObj_Array[4] = m_spDatabase_pluto_main->DesignObj_get()->GetRow(DESIGNOBJ_popFBSF_More_CONST);
	}
	else if( m_pRow_UI->PK_UI_get()==UI_Normal_Horizontal_3_4_CONST )
	{
		pRow_DesignObj_Array[0] = m_spDatabase_pluto_main->DesignObj_get()->GetRow(DESIGNOBJ_mnuFileList_SortOptions_CONST);
	}

	int iPK_MediaType_Searchable[] = {MEDIATYPE_pluto_StoredAudio_CONST,MEDIATYPE_pluto_StoredVideo_CONST,MEDIATYPE_pluto_Pictures_CONST,MEDIATYPE_np_Game_CONST,MEDIATYPE_misc_DocViewer_CONST,MEDIATYPE_misc_Playlist_CONST,MEDIATYPE_lmce_Game_CONST};
	for(int i=0;i<sizeof(iPK_MediaType_Searchable)/sizeof(int);++i)
	{
		m_dwMediaType = iPK_MediaType_Searchable[i];
		for(int iRow_DesignObj=0;iRow_DesignObj<5;++iRow_DesignObj)
		{
			Row_DesignObj *pRow_DesignObj = pRow_DesignObj_Array[iRow_DesignObj];
			if( !pRow_DesignObj )
				continue;  // Shouldn't happen

			m_mapDesignObj_WithArrays[ pRow_DesignObj->PK_DesignObj_get() ] = true;
			DesignObj_Generator *ocDesignObj = new DesignObj_Generator(this,pRow_DesignObj,PlutoRectangle(0,0,0,0),NULL,true,false);
			if( m_pRow_UI->PK_UI_get()==UI_V2_Normal_Horizontal_16_9_CONST )
			{
				ocDesignObj->m_bIsPopup=true;
				m_mapPopups_set(ocDesignObj->m_pRow_DesignObj->PK_DesignObj_get(),true);
			}
			else
				ocDesignObj->Process();

			if( ocDesignObj->m_bUsingCache )
			{
				SearchForGotos(ocDesignObj);
			}
		}
	}
	m_dwMediaType=0;

	list< pair<Row_DesignObj *,int> >::iterator itnol;
	for(itnol=alNewDesignObjLocationsToGenerate.begin();itnol!=alNewDesignObjLocationsToGenerate.end();++itnol)
	{
		Row_DesignObj *m_pRow_DesignObjDependancy = itnol->first;
		m_iLocation=itnol->second;
		m_pRow_Room = NULL;
		m_pRow_EntertainArea = NULL;
		for(itd=m_dequeLocation.begin();itd!=m_dequeLocation.end();++itd)
		{
			LocationInfo *li = (*itd);
			if( li->iLocation == m_iLocation )
			{
				m_pRow_Room = m_spDatabase_pluto_main->Room_get()->GetRow(li->PK_Room);
				m_pRow_EntertainArea = li->PK_EntertainArea>0 ? m_spDatabase_pluto_main->EntertainArea_get()->GetRow(li->PK_EntertainArea) : NULL;
				break;
			}
		}

		if( m_htGeneratedScreens.find(StringUtils::itos(m_pRow_DesignObjDependancy->PK_DesignObj_get()) + "." + StringUtils::itos(m_iLocation))==m_htGeneratedScreens.end() )
		{
			m_iPK_DesignObj_Screen = m_pRow_DesignObjDependancy->PK_DesignObj_get();
			DesignObj_Generator *ocDesignObj = new DesignObj_Generator(this,m_pRow_DesignObjDependancy,PlutoRectangle(0,0,0,0),NULL,true,false);
			if( ocDesignObj->m_bUsingCache )
			{
				SearchForGotos(ocDesignObj);
			}
		}
	}

	if( iNumScreensPrior != m_htGeneratedScreens.size() ) // Bad, bad code.  The problem is the above SearchForGotos may have drug in more screens which also have dependencies
	{
		bFirstLoop=false;
		goto loop_to_keep_looking_for_objs_to_include;
	}

	int NumScreens=0;
	for(itgs=m_htGeneratedScreens.begin();itgs!=m_htGeneratedScreens.end();++itgs)
	{
		listDesignObj_Generator *o = (*itgs).second;
		if( !o )
			continue;
		for(listDesignObj_Generator::iterator itlcgo=o->begin();itlcgo!=o->end();++itlcgo)
		{
			DesignObj_Generator *oco = (*itlcgo);

			if( !oco->m_bUsingCache )
			{
				// The scaling process will also let us know if we have any arrays within our child objects.  If so
				// we know we cannot cache this screen.  So we pass in the top-most object throughout
				if( oco->m_bDontScale )
					oco->ScaleAllValues(m_sScale_Orig.Width,m_sScale_Orig.Height,oco);
				else
					oco->ScaleAllValues(m_sScale.Width,m_sScale.Height,oco);
			}
			if( oco->m_alMPArray.size()<=1 )
				NumScreens++;
			else
				NumScreens += (int) oco->m_alMPArray.size();
		}
	}

	m_sScaledSize = PlutoSize(m_Width,m_Height);
	m_AnimationStyle = 1; // TODO -- is this used anymore? m_pRow_Orbiter->FK_Skin_getrow()->FK_AnimationStyle_get();


	// temp -- use of styles isn't yet well defined.  just hack in something to get a style variation for each style right now
	vector<Row_Style *> vectRow_Style;
	m_spDatabase_pluto_main->Style_get()->GetRows("1=1",&vectRow_Style); // all styles
	for(size_t style=0;style<vectRow_Style.size();++style)
	{
		Row_Style *pRow_Style = vectRow_Style[style];

		vector<Row_StyleVariation *> vectrsv;
		pRow_Style->StyleVariation_FK_Style_getrows(&vectrsv);

		if( vectrsv.size()==0 )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"WARNING! No variation for style: %d",
				pRow_Style->PK_Style_get());
			pRow_Style = pRow_Style->Table_Style_get()->GetRow(1);
			pRow_Style->StyleVariation_FK_Style_getrows(&vectrsv);
			if( vectrsv.size()==0 )
				throw "No variation for style: " + StringUtils::itos(pRow_Style->PK_Style_get());
		}

		TextStyle *pTextStyle = DesignObj_Generator::PickStyleVariation(vectrsv,this,0);
		if( pTextStyle )
		{
			pTextStyle->m_iVersion=0;
			m_mapTextStyle[pRow_Style->PK_Style_get()]=pTextStyle;
		}
	}

	vector<Row_Text *> vectRow_Text;
	m_spDatabase_pluto_main->Text_get()->GetRows("AddToOrbiter=1",&vectRow_Text); // all Texts to pass on to the Orbiter
	for(size_t Text=0;Text<vectRow_Text.size();++Text)
	{
		Row_Text *pRow_Text = vectRow_Text[Text];
		Row_Text_LS *pRow_Text_LS = CGText::GetText_LS(pRow_Text->PK_Text_get(),this);
		if( !pRow_Text_LS )
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Text %d not found", pRow_Text->PK_Text_get());
		else
			m_mapTextString[pRow_Text->PK_Text_get()] = pRow_Text_LS->Description_get();
	}

	char cScaleMenuBg,cScaleOtherGraphics;
	string str = m_pRow_Size->ScaleMenuBg_get();
	cScaleMenuBg = str.size() ? str[0] : 'F';
	str = m_pRow_Size->ScaleOtherGraphics_get();
	cScaleOtherGraphics = str.size() ? str[0] : 'F';

	m_iScreensTotal=m_iScreensToRender;
	m_iScreensToRender=m_iLastReportedPercentage=0;

	for(itgs=m_htGeneratedScreens.begin();itgs!=m_htGeneratedScreens.end();++itgs)
	{
		listDesignObj_Generator *o = (*itgs).second;
		if( !o )
			continue;
		for(listDesignObj_Generator::iterator itlcgo=o->begin();itlcgo!=o->end();++itlcgo)
		{
			DesignObj_Generator *oco = (*itlcgo);
			if( !oco->m_bUsingCache )
			{
				int Percent = m_iScreensToRender++ * 100 / m_iScreensTotal;
				if( !m_iLastReportedPercentage || Percent - m_iLastReportedPercentage > 3 )
				{
					m_iLastReportedPercentage = max(1,Percent);
					if( g_bBootSplash )
						system( ("/usr/pluto/bin/BootMessage.sh \"Stage 2 of 2 - Rendering screen " + StringUtils::itos(m_iScreensToRender) + " of " + StringUtils::itos(m_iScreensTotal) + "\"").c_str());
					else
					{
						m_pRow_Orbiter->Reload();
						m_pRow_Orbiter->RegenStatus_set("Stage 2 of 2 - Rendering screen " + StringUtils::itos(m_iScreensToRender) + " of " + StringUtils::itos(m_iScreensTotal));
						m_pRow_Orbiter->RegenPercent_set(m_iLastReportedPercentage);
						m_pRow_Orbiter->Table_Orbiter_get()->Commit();
					}
				}
				OutputScreen(oco);

				bool bScaledDown=false;  // Keep track of whether or not we scaled this screen down
				// Don't force popups to be full-screen
				if( oco->m_bDontScale==false && m_mapPopups.find(oco->m_pRow_DesignObj->PK_DesignObj_get())==m_mapPopups.end() )
				{
					oco->m_rPosition.Right(m_Width-m_rSpacing.Width);
					oco->m_rPosition.Bottom(m_Height-m_rSpacing.Height);
					oco->m_rBackgroundPosition.Right(m_Width-m_rSpacing.Width);
					oco->m_rBackgroundPosition.Bottom(m_Height-m_rSpacing.Height);
					bScaledDown=true;
				}
				else
					oco->m_bIsPopup=true;
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Rendering screen %s in %d",oco->m_ObjectID.c_str(),m_pRow_Device->PK_Device_get());
				try
				{
					Renderer::GetInstance().Setup(m_sFontPath, m_sOutputPath, m_Width, m_Height, m_bUseAlphaBlending,
						cScaleMenuBg, cScaleOtherGraphics, (float) m_sScale.Width/1000, (float) m_sScale.Height/1000, m_iRotation, m_bUseMask);

					// Render everything
					Renderer::GetInstance().RenderObject(NULL, oco, PlutoPoint(0,0), -1);  					

					oco->HandleRotation(m_iRotation);
				}
				catch(string s)
				{
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Failed to render screen %s / %s",oco->m_ObjectID.c_str(),s.c_str());
					throw "Failed to render screen " + oco->m_ObjectID + " error: " + s ;
				}

				if( bScaledDown )
				{
					oco->m_rPosition.X=oco->m_rPosition.Y=oco->m_rBackgroundPosition.X=oco->m_rBackgroundPosition.Y=0;
					oco->m_rPosition.Width=oco->m_rBackgroundPosition.Width=m_Width;
					oco->m_rPosition.Height=oco->m_rBackgroundPosition.Height=m_Height;
				}

				// We can cache this the next time
				Row_CachedScreens *pdrCachedScreen = m_spDatabase_pluto_main->CachedScreens_get()->GetRow(m_pRow_Orbiter->PK_Orbiter_get(),oco->m_pRow_DesignObj->PK_DesignObj_get(),oco->m_iVersion);
				if( !pdrCachedScreen )
				{
					pdrCachedScreen = m_spDatabase_pluto_main->CachedScreens_get()->AddRow();
					pdrCachedScreen->FK_Orbiter_set(m_pRow_Orbiter->PK_Orbiter_get());
					pdrCachedScreen->FK_DesignObj_set(oco->m_pRow_DesignObj->PK_DesignObj_get());
					pdrCachedScreen->Version_set(oco->m_iVersion);
				}
				string Filename = m_sOutputPath + "screen " + StringUtils::itos(m_pRow_Orbiter->PK_Orbiter_get()) + "." + 
					StringUtils::itos(oco->m_pRow_DesignObj->PK_DesignObj_get()) + "." + StringUtils::itos(oco->m_iVersion) + "." + 
					StringUtils::itos((int) StringUtils::SQLDateTime(oco->m_pRow_DesignObj->psc_mod_get())) + ".cache";

				oco->SerializeWrite(Filename);
				oco->WriteRegenVersion(Filename+".regen");
				if( oco->m_iNumFloorplanItems )
					oco->WriteFloorplanInfo(Filename+".fp");

				pdrCachedScreen->Schema_set(ORBITER_SCHEMA);
				pdrCachedScreen->ContainsArrays_set(oco->m_bContainsArrays ? 1 : 0);
				pdrCachedScreen->Modification_LastGen_set(oco->m_pRow_DesignObj->psc_mod_get());
				m_spDatabase_pluto_main->CachedScreens_get()->Commit();
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Using cached version of screen %s",oco->m_ObjectID.c_str());
				// Got to look for any used styles
			}
			m_ScreenMap[ oco->m_ObjectID ] = oco;
		}
	}
	/*
	fstr_Orbiter << StringUtils::itos((int) m_mapUsedOrbiterCriteria.size()) << "|";
	for(map<int,int>::iterator itucc=m_mapUsedOrbiterCriteria.begin();itucc!=m_mapUsedOrbiterCriteria.end();++itucc)
	{
	Row_Criteria * drCriteria = m_spDatabase_pluto_main->Criteria_get()->GetRow((*itucc).first);
	fstr_Orbiter << drCriteria->FK_CriteriaList_get() << "|" << drCriteria->PK_Criteria_get() << "|";
	OutputCriteriaNest(drCriteria->FK_CriteriaParmNesting_getrow());
	}
	*/
	/*
	// Lastly, we should now have a complete list of the variables that we used in m_htVariablesUsed.  Go through and fill in all the defaults
	for(map<int,int>::iterator ituv=htUsedVariables.begin();ituv!=htUsedVariables.end();++ituv)
	{
	ocOrbiter->m_alUsedVariables.push_back(new OCUsedVariables((*ituv).first,m_mapVariable[(*ituv).first]));
	}

	fstr_Orbiter << StringUtils::itos((int) ocOrbiter->m_alUsedVariables.size()) << "|";
	for(list<OCUsedVariables *>::iterator ituv=ocOrbiter->m_alUsedVariables.begin();ituv!=ocOrbiter->m_alUsedVariables.end();++ituv)
	{
	OCUsedVariables *uv = (*ituv);
	fstr_Orbiter << uv->m_iPK_Variable << "|" << (uv->m_oInitialValue.length()>0 ? uv->m_oInitialValue : "") << "|";
	}
	*/
/* todo 2.0
	// Be sure to add any styles that are forced to be included
	vector<Row_Style *> vectStyles = m_spDatabase_pluto_main->Style_get()->GetRows(string(STYLE_ALWAYSINCLUDEONORBITER_FIELD) + "=1");
	for(size_t s=0;s<vectStyles.size();++s)
	{
		Row_StyleVariation *temp1,*temp2;
		Row_Style * drStyle = vectStyles[s];
		Row_StyleVariation * drSV = DesignObj_Generator::PickStyleVariation(drStyle,this,temp1,temp2);
		if( m_htStylesUsedInOrbiterText.find(drSV->FK_Style_get())==m_htStylesUsedInOrbiterText.end() )
			m_htStylesUsedInOrbiterText[drSV->FK_Style_get()]=drSV;
	}
*/
/*
	for(map<int,class Row_StyleVariation *>::iterator itsuic=m_htStylesUsedInOrbiterText.begin();itsuic!=m_htStylesUsedInOrbiterText.end();++itsuic)
	{
		Row_StyleVariation * drStyleVariation = (*itsuic).second;
		DesignObjStyle *pDesignObjStyle = new DesignObjStyle();
		pDesignObjStyle->m_iPK_Style = drStyleVariation->FK_Style_get();
		pDesignObjStyle->m_iPixelHeight = drStyleVariation->PixelHeight_get() * m_pRow_Size->Scale_get() / 1000;
		pDesignObjStyle->m_iShadowX = drStyleVariation->ShadowX_get();
		pDesignObjStyle->m_iShadowY = drStyleVariation->ShadowY_get();
		pDesignObjStyle->m_sFont = drStyleVariation->Font_get();
		pDesignObjStyle->m_PlutoColor = PlutoColor(drStyleVariation->Color_get());
		pDesignObjStyle->m_PlutoColor_Shadow = PlutoColor(drStyleVariation->ShadowColor_get());
		pDesignObjStyle->m_bBold = drStyleVariation->Bold_get()==1;
		pDesignObjStyle->m_bItalic = drStyleVariation->Italic_get()==1;
		pDesignObjStyle->m_bUnderline = drStyleVariation->Underline_get()==1;
		m_mapDesignObjStyle[pDesignObjStyle->m_iPK_Style]=pDesignObjStyle;
	}
*/
	// This will update the _LastGen without changing the Modification field
	m_pRow_Orbiter->Reload();
	m_pRow_Orbiter->FloorplanInfo_set(StringUtils::itos(m_iNumFloorplanItems) + m_sFloorPlanData);
	m_pRow_Orbiter->Table_Orbiter_get()->Commit();

	m_iSC_Version = ORBITER_SCHEMA;
	m_tGenerationTime = (unsigned long)time(NULL);

	string sFilename = m_sOutputPath + "C" + StringUtils::itos(m_iPK_Orbiter) + ".info";
	bool b=SerializeWrite(sFilename);

	int UncompressedSize = static_cast<int>(m_pcCurrentPosition - m_pcDataBlock);
	char *Data = new char[UncompressedSize + UncompressedSize / 64 + 16 + 3 + 4];
    lzo_uint out_len;
	*((int *)Data)=UncompressedSize;
    lzo1x_1_compress((lzo_byte *)m_pcDataBlock,UncompressedSize,(lzo_byte *)Data+4,&out_len,wrkmem);
	int CompressedSize = (int)out_len+4;

	FILE *file = fopen( (sFilename + ".lzo").c_str(),"wb" );
	if( file )
	{
		fwrite(Data,CompressedSize,1,file);
		fclose(file);
	}

	if( b )
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Successfully finished");
	else
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not create output file");

#ifdef _WIN32
	// no dumping leaks now	WSACleanup();
#endif

	if( !m_bNewOrbiter )
	{
		// Don't reset the psc_mod so this doesn't result in a 'new devices' messages
		string sql = "UPDATE Device SET NeedConfigure=0,psc_mod=psc_mod WHERE PK_Device=" + StringUtils::itos(m_pRow_Device->PK_Device_get());
		m_spDatabase_pluto_main->threaded_db_wrapper_query(sql);
	}

	return 0;
}

void OrbiterGenerator::SearchForGotos(DesignObj_Data *pDesignObj_Data)
{
	SearchForGotos(pDesignObj_Data,&pDesignObj_Data->m_Action_LoadList);
	SearchForGotos(pDesignObj_Data,&pDesignObj_Data->m_Action_UnloadList);
	SearchForGotos(pDesignObj_Data,&pDesignObj_Data->m_Action_HighlightList);
	SearchForGotos(pDesignObj_Data,&pDesignObj_Data->m_Action_UnhighlightList);
	SearchForGotos(pDesignObj_Data,&pDesignObj_Data->m_Action_StartupList);
	SearchForGotos(pDesignObj_Data,&pDesignObj_Data->m_Action_TimeoutList);
	DesignObjZoneList::iterator itZone;
	for(itZone=pDesignObj_Data->m_ZoneList.begin();itZone!=pDesignObj_Data->m_ZoneList.end();++itZone)
	{
		CGZone *oz = (CGZone *) *itZone;
		SearchForGotos(pDesignObj_Data,&oz->m_Commands);
	}
	DesignObj_DataList::iterator it;
	for(it=pDesignObj_Data->m_ChildObjects.begin();it!=pDesignObj_Data->m_ChildObjects.end();++it)
		SearchForGotos(*it);

}

void OrbiterGenerator::SearchForGotos(DesignObj_Data *pDesignObj_Data,DesignObjCommandList *alCommands)
{
	DesignObjCommandList::iterator itActions;
	for(itActions=alCommands->begin();itActions!=alCommands->end();++itActions)
	{
		CGCommand *oca = (CGCommand *) *itActions;
		if( oca->m_PK_Command == COMMAND_Goto_DesignObj_CONST || oca->m_PK_Command == COMMAND_Goto_Screen_CONST || oca->m_PK_Command == COMMAND_Show_Popup_CONST || oca->m_PK_Command == COMMAND_Set_Floorplan_CONST )
		{
            map<int, string>::iterator itParm;
			if( oca->m_PK_Command == COMMAND_Goto_Screen_CONST )
				itParm=oca->m_ParameterList.find(COMMANDPARAMETER_PK_Screen_CONST);
			else
				itParm=oca->m_ParameterList.find(COMMANDPARAMETER_PK_DesignObj_CONST);

			if( itParm!=oca->m_ParameterList.end() )
			{
				string sDesignObj;
				if( oca->m_PK_Command == COMMAND_Goto_Screen_CONST )
				{
					Row_DesignObj *pRow_DesignObj = GetDesignObjFromScreen(atoi((*itParm).second.c_str()));
					if( pRow_DesignObj )
						sDesignObj = StringUtils::itos(pRow_DesignObj->PK_DesignObj_get());
					map<int, string>::iterator itParmLocation = oca->m_ParameterList.find(COMMANDPARAMETER_Location_CONST);
					if( itParmLocation!=oca->m_ParameterList.end() && itParmLocation->second.size() )
					{
						sDesignObj += "." + itParmLocation->second;
						m_iLocation = atoi(itParmLocation->second.c_str());
						if( m_iLocation< int(m_dequeLocation.size()) )
						{
							LocationInfo *li = m_dequeLocation[m_iLocation];
							m_pRow_Room = m_spDatabase_pluto_main->Room_get()->GetRow(li->PK_Room);
							m_pRow_EntertainArea = m_spDatabase_pluto_main->EntertainArea_get()->GetRow(li->PK_EntertainArea);
						}
						else
							LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ERROR: Cached screen refers to invalid location");
					}
						
				}
				else
					sDesignObj = (*itParm).second;
				Row_DesignObj *p_m_pRow_DesignObj = m_spDatabase_pluto_main->DesignObj_get()->GetRow(atoi(sDesignObj.c_str()));

				StringUtils::Replace(&sDesignObj,"<%=L:0%>","0");

				if( p_m_pRow_DesignObj && m_htGeneratedScreens.find(First2Dots(sDesignObj))==m_htGeneratedScreens.end() )
				{
					m_iPK_DesignObj_Screen = p_m_pRow_DesignObj->PK_DesignObj_get();
					if( oca->m_PK_Command == COMMAND_Show_Popup_CONST ||
							(oca->m_PK_Command == COMMAND_Set_Floorplan_CONST && 
							oca->m_ParameterList.find(COMMANDPARAMETER_TrueFalse_CONST)!=oca->m_ParameterList.end() &&
							oca->m_ParameterList[COMMANDPARAMETER_TrueFalse_CONST]=="1") )
						m_mapPopups_set(m_iPK_DesignObj_Screen,true);
	 				DesignObj_Generator *ocDesignObj = new DesignObj_Generator(this,p_m_pRow_DesignObj,PlutoRectangle(0,0,0,0),NULL,true,false);
					SearchForGotos(ocDesignObj);
				}
			}
		}
	}
}

void OrbiterGenerator::OutputScreen(DesignObj_Generator *ocDesignObj)
{
	// Always output the first page, even if there's no other
	for(size_t i=0;i<ocDesignObj->m_alMPArray.size() || i==0;++i)
	{
		string ParentScreen;
		if( ocDesignObj->m_pRow_DesignObj->FK_DesignObj_SubstForSkin_get() )
			ParentScreen = StringUtils::itos(ocDesignObj->m_pRow_DesignObj->FK_DesignObj_SubstForSkin_get()) + "." + StringUtils::itos(ocDesignObj->m_iVersion) + "." + StringUtils::itos((int) i);
		else
			ParentScreen = StringUtils::itos(ocDesignObj->m_pRow_DesignObj->PK_DesignObj_get()) + "." + StringUtils::itos(ocDesignObj->m_iVersion) + "." + StringUtils::itos((int) i);
		ocDesignObj->m_iPage= int(i);
		OutputDesignObjs(ocDesignObj,(int) i,false,ParentScreen);
		if( ocDesignObj->m_iNumFloorplanItems )
		{
			m_iNumFloorplanItems+=ocDesignObj->m_iNumFloorplanItems;
			m_sFloorPlanData+=ocDesignObj->m_sFloorPlanData;
		}
	}
}

void OrbiterGenerator::OutputDesignObjs(DesignObj_Generator *ocDesignObj,int ArrayPage,bool bIsChild,string ParentScreen)
{
if( ocDesignObj->m_pRow_DesignObj->PK_DesignObj_get()==4949 )
int k=2;
	ocDesignObj->m_bChild=bIsChild;

	int PK_Orbiter = ocDesignObj->m_pOrbiterGenerator->m_pRow_Orbiter->PK_Orbiter_get();
	Row_Orbiter * m_pRow_Orbiter = ocDesignObj->m_pOrbiterGenerator->m_pRow_Orbiter;

	if( ocDesignObj->m_bChild )
	{
		if( ocDesignObj->m_pRow_DesignObj->FK_DesignObjType_get()==DESIGNOBJTYPE_Floorplan_CONST )
		{
			size_t FirstDot = ParentScreen.find('.');
			size_t SecondDot = ParentScreen.find('.',FirstDot+1);
			string NewParent = ParentScreen.substr(0,SecondDot) + "." + StringUtils::itos(ocDesignObj->m_iFloorplanPage) + ".";
			ocDesignObj->m_iPage=ocDesignObj->m_iFloorplanPage;
			if( ocDesignObj->m_pRow_DesignObj->FK_DesignObj_SubstForSkin_get() )
				ocDesignObj->m_ObjectID = NewParent + StringUtils::itos(ocDesignObj->m_pRow_DesignObj->FK_DesignObj_SubstForSkin_get()) + "." + StringUtils::itos(Counter++);
			else
				ocDesignObj->m_ObjectID = NewParent + StringUtils::itos(ocDesignObj->m_pRow_DesignObjVariation->FK_DesignObj_get()) + "." + StringUtils::itos(Counter++);
		}
		else if( ocDesignObj->m_pRow_DesignObj->FK_DesignObj_SubstForSkin_get() )
			ocDesignObj->m_ObjectID = ParentScreen + "." + StringUtils::itos(ocDesignObj->m_pRow_DesignObj->FK_DesignObj_SubstForSkin_get()) + "." + StringUtils::itos(Counter++);
		else
			ocDesignObj->m_ObjectID = ParentScreen + "." + StringUtils::itos(ocDesignObj->m_pRow_DesignObjVariation->FK_DesignObj_get()) + "." + StringUtils::itos(Counter++);
	}
	else
		ocDesignObj->m_ObjectID = ParentScreen;

/* AB 2005-08-19 this code caused a crash since an object was off the screen and therefore given
	a negative width/height.  Don't know if this code was needed for anything anyway
	if( ocDesignObj->m_rBackgroundPosition.Right()>m_Width )
		ocDesignObj->m_rBackgroundPosition.Right(m_Width);
	if( ocDesignObj->m_rPosition.Right()>m_Width )
		ocDesignObj->m_rPosition.Right(m_Width);
	if( ocDesignObj->m_rBackgroundPosition.Bottom()>m_Height )
		ocDesignObj->m_rBackgroundPosition.Bottom(m_Height);
	if( ocDesignObj->m_rPosition.Bottom()>m_Height )
		ocDesignObj->m_rPosition.Bottom(m_Height);
*/
	if( ocDesignObj->m_iFloorplanPage>0 && ocDesignObj->m_iFloorplanDevice )
	{
		string PageList = "";
		if( htDevicePages.find(abs(ocDesignObj->m_iFloorplanDevice))!=htDevicePages.end() )
			PageList = htDevicePages[abs(ocDesignObj->m_iFloorplanDevice)];
		if( PageList.find(StringUtils::itos(ocDesignObj->m_iFloorplanPage) + ",")!=string::npos )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Floorplan device %d Page %d already in the system",
				ocDesignObj->m_iFloorplanDevice, ocDesignObj->m_iFloorplanPage);
		}
		else
		{
			PageList += StringUtils::itos(ocDesignObj->m_iFloorplanPage) + ",";
			htDevicePages[abs(ocDesignObj->m_iFloorplanDevice)] = PageList;
			ocDesignObj->GetTopMostObject()->m_sFloorPlanData += "\t" + StringUtils::itos(ocDesignObj->m_iFloorplanDevice) + "\t" + StringUtils::itos(ocDesignObj->m_iFloorplanPage) + "\t" + ocDesignObj->m_ObjectID + "\t" + StringUtils::itos(ocDesignObj->m_pFloorplanFillPoint.X) + "\t" + StringUtils::itos(ocDesignObj->m_pFloorplanFillPoint.Y);
			ocDesignObj->GetTopMostObject()->m_iNumFloorplanItems++;
		}
	}

	ocDesignObj->m_ObjectType=ocDesignObj->m_pRow_DesignObj->FK_DesignObjType_get();
	ocDesignObj->m_PK_Effect_On_Screen=atoi(ocDesignObj->GetParm(DESIGNOBJPARAMETER_PK_Effect_On_Screen_CONST).c_str());
	ocDesignObj->m_PK_Effect_Off_Screen=atoi(ocDesignObj->GetParm(DESIGNOBJPARAMETER_PK_Effect_Off_Screen_CONST).c_str());
	ocDesignObj->m_bCustomRender=atoi(ocDesignObj->GetParm(DESIGNOBJPARAMETER_Custom_Render_CONST).c_str())==1;
	ocDesignObj->m_bCanBeHidden = bIsChild && ocDesignObj->m_bCanBeHidden;
	//	ocDesignObj->m_bDontMergeBackground =  
	//		(ocDesignObj->m_bHideByDefault ? "1" : "0") << "|" <<
	ocDesignObj->m_bDontResetState = ocDesignObj->m_pRow_DesignObjVariation->DontResetSelectedState_get()==1;
	ocDesignObj->m_iRepeatIntervalInMS = ocDesignObj->m_pRow_DesignObjVariation->RepeatMS_get();
	ocDesignObj->m_bRepeatParm = ocDesignObj->m_pRow_DesignObjVariation->RepeatParm_get()==1;
	ocDesignObj->m_bCantGoBack = ocDesignObj->m_pRow_DesignObj->CantGoBack_get()==1;
	ocDesignObj->m_bKeepGraphicInCache = ocDesignObj->m_pRow_DesignObj->KeepGraphicInCache_get()==1 || 
		(ocDesignObj->m_ocoParent && ocDesignObj->m_ocoParent->m_bKeepGraphicInCache);
	string sScreenType = ocDesignObj->m_pRow_DesignObj->ScreenType_get();
	ocDesignObj->m_cScreenType = sScreenType.size() ? sScreenType[0] : 0;
	ocDesignObj->m_bCanGoBackToSameScreen = ocDesignObj->m_pRow_DesignObj->CanGoBackToSameScreen_get()==1;

	//		StringUtils::itos(ocDesignObj->m_bChildrenBeforeText) << "|" << 
	//StringUtils::itos(ocDesignObj->m_bChildrenBehind) << "|" << 
	ocDesignObj->m_bProcessActionsAtServer = ocDesignObj->m_pRow_DesignObj->CommandsProcessedAtServer_get()==1;
	ocDesignObj->m_dwTimeoutSeconds = ocDesignObj->m_pRow_DesignObj->TimeoutSeconds_isNull() ? 0 : ocDesignObj->m_pRow_DesignObj->TimeoutSeconds_get();
	ocDesignObj->m_Priority=ocDesignObj->m_pRow_DesignObj->Priority_get();

	if( !ocDesignObj->m_pRow_DesignObjVariation->FK_Criteria_Orbiter_isNull() )
		ocDesignObj->m_iPK_Criteria = ocDesignObj->m_pRow_DesignObjVariation->FK_Criteria_Orbiter_get();
	else
		ocDesignObj->m_iPK_Criteria = 0;

	ocDesignObj->m_bAnimate = ocDesignObj->m_pRow_DesignObj->Animate_get()==1;

	int NumParms=0;
	vector<Row_DesignObjVariation_DesignObjParameter *> vectovp;
	ocDesignObj->m_pRow_DesignObjVariation->DesignObjVariation_DesignObjParameter_FK_DesignObjVariation_getrows(&vectovp);
	for(size_t s=0;s<vectovp.size();++s)
	{
		Row_DesignObjVariation_DesignObjParameter * drOVCP = vectovp[s];
		if( drOVCP->FK_DesignObjParameter_get()!=DESIGNOBJPARAMETER_Graphic_Filename_CONST && drOVCP->FK_DesignObjParameter_get()!=DESIGNOBJPARAMETER_Selected_Graphic_Fil_CONST && 
			drOVCP->FK_DesignObjParameter_get()!=DESIGNOBJPARAMETER_Alt_Graphic_File_CONST && drOVCP->FK_DesignObjParameter_get()!=DESIGNOBJPARAMETER_X_Position_CONST && 
			drOVCP->FK_DesignObjParameter_get()!=DESIGNOBJPARAMETER_Y_Position_CONST && drOVCP->FK_DesignObjParameter_get()!=DESIGNOBJPARAMETER_Width_CONST && 
			drOVCP->FK_DesignObjParameter_get()!=DESIGNOBJPARAMETER_Height_CONST && drOVCP->FK_DesignObjParameter_get()!=DESIGNOBJPARAMETER_Page_CONST )
			NumParms++;
	}

	if( ocDesignObj->m_pRow_DesignObj->FK_DesignObjType_get()==DESIGNOBJTYPE_Floorplan_CONST )
		NumParms++;

	if( ocDesignObj->m_pRow_DesignObj->FK_DesignObjType_get()==DESIGNOBJTYPE_Floorplan_CONST )
	{
		ocDesignObj->m_mapObjParms[DESIGNOBJPARAMETER_Page_CONST]=ocDesignObj->m_iFloorplanPage;
	}

	for(size_t s=0;s<vectovp.size();++s)
	{
		Row_DesignObjVariation_DesignObjParameter * drOVCP = vectovp[s];
		if( drOVCP->FK_DesignObjParameter_get()!=DESIGNOBJPARAMETER_Graphic_Filename_CONST && drOVCP->FK_DesignObjParameter_get()!=DESIGNOBJPARAMETER_Selected_Graphic_Fil_CONST && 
			drOVCP->FK_DesignObjParameter_get()!=DESIGNOBJPARAMETER_Alt_Graphic_File_CONST && drOVCP->FK_DesignObjParameter_get()!=DESIGNOBJPARAMETER_X_Position_CONST && 
			drOVCP->FK_DesignObjParameter_get()!=DESIGNOBJPARAMETER_Y_Position_CONST && drOVCP->FK_DesignObjParameter_get()!=DESIGNOBJPARAMETER_Width_CONST && 
			drOVCP->FK_DesignObjParameter_get()!=DESIGNOBJPARAMETER_Height_CONST && drOVCP->FK_DesignObjParameter_get()!=DESIGNOBJPARAMETER_Page_CONST )
		{
			string Value="";
			if( drOVCP->Value_isNull() || drOVCP->Value_get()=="" )
			{
				Row_DesignObjVariation_DesignObjParameter * drOVCP2 = m_spDatabase_pluto_main->DesignObjVariation_DesignObjParameter_get()->GetRow(ocDesignObj->m_pRow_DesignObjVariation_Standard->PK_DesignObjVariation_get(),drOVCP->FK_DesignObjParameter_get());
				if( drOVCP2 )
					Value = drOVCP2->Value_get();
			}
			else
				Value = drOVCP->Value_get();

			// See if this is one of the parameters that we need to scale
			if( drOVCP->FK_DesignObjParameter_get()==DESIGNOBJPARAMETER_Fixed_Column_Width_CONST ||
				drOVCP->FK_DesignObjParameter_get()==DESIGNOBJPARAMETER_Column_Spacing_CONST ||
				drOVCP->FK_DesignObjParameter_get()==DESIGNOBJPARAMETER_First_Column_Width_CONST )
			{
				if( Value=="" )
					ocDesignObj->m_mapObjParms[drOVCP->FK_DesignObjParameter_get()]="";
				else
				{
					ocDesignObj->m_mapObjParms[drOVCP->FK_DesignObjParameter_get()]=StringUtils::itos((atoi(Value.c_str()) * ocDesignObj->m_pOrbiterGenerator->m_sScale.Width / 1000));
				}
			}
			else if( drOVCP->FK_DesignObjParameter_get()==DESIGNOBJPARAMETER_Fixed_Row_Height_CONST || 
				drOVCP->FK_DesignObjParameter_get()==DESIGNOBJPARAMETER_Row_Spacing_CONST || 
				drOVCP->FK_DesignObjParameter_get()==DESIGNOBJPARAMETER_First_Row_Height_CONST )
			{
				if( Value=="" )
					ocDesignObj->m_mapObjParms[drOVCP->FK_DesignObjParameter_get()]="";
				else
				{
					ocDesignObj->m_mapObjParms[drOVCP->FK_DesignObjParameter_get()]=StringUtils::itos((atoi(Value.c_str()) * ocDesignObj->m_pOrbiterGenerator->m_sScale.Height / 1000));
				}
			}
			else if( drOVCP->FK_DesignObjParameter_get()==DESIGNOBJPARAMETER_PK_Style_CONST || drOVCP->FK_DesignObjParameter_get()==DESIGNOBJPARAMETER_PK_Style_FirstRow_CONST ||
				drOVCP->FK_DesignObjParameter_get()==DESIGNOBJPARAMETER_PK_Style_FirstColumn_CONST || drOVCP->FK_DesignObjParameter_get()==DESIGNOBJPARAMETER_PK_Style_Selected_CONST ||
				drOVCP->FK_DesignObjParameter_get()==DESIGNOBJPARAMETER_PK_Style_Alt_CONST )
			{
				int Style = atoi(Value.c_str());
/* todo, I'm just going to grab all styles now.  This is a problem when it doesn't regen a cached screen, it doesn't get the styles
				vector<Row_StyleVariation *> vectrsv;
				Row_Style *pRow_Style = m_spDatabase_pluto_main->Style_get()->GetRow(Style);
				if( pRow_Style )
				{
					pRow_Style->StyleVariation_FK_Style_getrows(&vectrsv);
					if( vectrsv.size()==0 )
						throw "No variation for style; " + StringUtils::itos(pRow_Style->PK_Style_get());

					TextStyle *pTextStyle = DesignObj_Generator::PickStyleVariation(vectrsv,this,0);
					if( pTextStyle )
					{
						pTextStyle->m_iVersion=0;
						m_mapTextStyle[Style]=pTextStyle;
					}
				}
*/
				ocDesignObj->m_mapObjParms[drOVCP->FK_DesignObjParameter_get()]=Value;
			}
			else if( drOVCP->FK_DesignObjParameter_get()==DESIGNOBJPARAMETER_Auto_Invalidate_Time_CONST )
				ocDesignObj->m_iRegenInterval = atoi(Value.c_str());
			else
				ocDesignObj->m_mapObjParms[drOVCP->FK_DesignObjParameter_get()]=StringUtils::Replace(&Value,"<%=!%>",StringUtils::itos(PK_Orbiter));
		}
	}

	if( ocDesignObj->m_ZoneList.size()==0 && ocDesignObj->m_sDesignObjGoto.length()>0 )
	{
		// There's an object goto, but no zones.  This can happen with the arrays forward and back
		// Just output 1 zone that is the full object size with a nav_goto

		DesignObjZone *p_DesignObjZone = new DesignObjZone(ocDesignObj->m_rPosition.X,ocDesignObj->m_rPosition.Y,ocDesignObj->m_rPosition.Width,ocDesignObj->m_rPosition.Height);
		DesignObjCommand *p_DesignObjCommand = new DesignObjCommand(COMMAND_Goto_Screen_CONST,0,true,0,0,false);
		p_DesignObjZone->m_Commands.push_back(p_DesignObjCommand);
		p_DesignObjCommand->m_ParameterList[COMMANDPARAMETER_PK_DesignObj_CONST] = ocDesignObj->m_sDesignObjGoto;
		ocDesignObj->m_ZoneList.push_back(p_DesignObjZone);
	}
	else
	{
		DesignObjZoneList::iterator itZone;
		for(itZone=ocDesignObj->m_ZoneList.begin();itZone!=ocDesignObj->m_ZoneList.end();++itZone)
		{
			CGZone *oz = (CGZone *) *itZone;

			if( oz->m_Rect.Width==0 || oz->m_Rect.Height==0 )
			{
				oz->m_Rect.X=ocDesignObj->m_rPosition.X;
				oz->m_Rect.Y=ocDesignObj->m_rPosition.Y;
			}

			ScaleCommandList(ocDesignObj,oz->m_Commands);
		}
	}
	if( ocDesignObj->m_ObjectType==DESIGNOBJTYPE_App_Desktop_CONST && (ocDesignObj->m_rPosition.Width==0 || ocDesignObj->m_rPosition.Height==0) )
	{
		if( ocDesignObj->m_bDontScale )
		{
			if( ocDesignObj->m_rPosition.Width==0 )
				ocDesignObj->m_rPosition.Width=m_Width;
			if( ocDesignObj->m_rPosition.Height==0 )
				ocDesignObj->m_rPosition.Height=m_Height;
		}
		else
		{
			if( ocDesignObj->m_rPosition.Width==0 )
				ocDesignObj->m_rPosition.Width=m_Width-m_rSpacing.Width*2;
			if( ocDesignObj->m_rPosition.Height==0 )
				ocDesignObj->m_rPosition.Height=m_Height-m_rSpacing.Height*2;
		}
	}
	ScaleCommandList(ocDesignObj,ocDesignObj->m_Action_LoadList);
	ScaleCommandList(ocDesignObj,ocDesignObj->m_Action_StartupList);
	ScaleCommandList(ocDesignObj,ocDesignObj->m_Action_TimeoutList);
	ScaleCommandList(ocDesignObj,ocDesignObj->m_Action_UnloadList);
	ScaleCommandList(ocDesignObj,ocDesignObj->m_Action_HighlightList);
	ScaleCommandList(ocDesignObj,ocDesignObj->m_Action_UnhighlightList);

	// It probably makes more sense to just leave all the action groups attached to buttons within the orbiters, and only export action groups
	// associated with events to event manager.  Perhaps then all the fkid_object_goto's should be eliminated, and it is all just actions??
	// Or leave fkid_object_goto in the database, but just make it an additional local action when it gets exported.
	// I WAS TRYING TO MOVE THE LOCAL ACTION INTO ACTION GROUPS, BUT THE PROBLEM IS THAT WHEN YOU TOUCH A BUTTON, A LOCAL ACTION IS NEVER SENT!!!
	// issues with removing fkid_action from objectvariation; action groups attached to an object with fkid_installation=null
	// will not get output.
	//			/* this isn't needed anymore since I'm always adding the action to the group, right?
	// The basic action applied to this button
	/*
	Row_Command * drCommand=null;
	if( !ocDesignObj->m_pRow_DesignObjVariation->FK_Command_isNull() || !ocDesignObj->m_pRow_DesignObjVariation_Standard->FK_Command_isNull())
	{
	DataRow[] drAPs;

	if( !ocDesignObj->m_pRow_DesignObjVariation->FK_Command_isNull() )
	{
	drCommand = ocDesignObj->m_pRow_DesignObjVariation->FK_Command_getrow();
	drAPs = ocDesignObj->m_pRow_DesignObjVariation.dr.GetChildRows(MyDataSet.MyRelations.DesignObjVariation_CommandParameter_FK_DesignObjVariation);
	}
	else
	{
	drCommand = ocDesignObj->m_pRow_DesignObjVariation->FK_Command_getrow();
	drAPs = ocDesignObj->m_pRow_DesignObjVariation_Standard.dr.GetChildRows(MyDataSet.MyRelations.DesignObjVariation_CommandParameter_FK_DesignObjVariation);
	}

	// If this action is handled locally, we won't put it here, we put it in the local actions later
	if( drCommand->HandledByOrbiter_get() )
	fstr_Orbiter << "0|0|");
	else
	{
	fstr_Orbiter << drCommand->PK_Command_get() + "|");
	fstr_Orbiter << drAPs.length() + "|");
	foreach(DataRow drAP in drAPs)
	{
	DesignObjVariation_Row_CommandParameter * drOVCAP = new DesignObjVariation_Row_CommandParameter *(drAP);
	fstr_Orbiter << drOVCAP->FK_CommandParameter_get() + "|" + (drOVCAP->FK_Variable_isNull() ? 0 : drOVCAP->FK_Variable_get()) + "|" + drOVCAP->Value_get() + "|");
	}
	}
	}
	else
	fstr_Orbiter << "0|0|");

	Row_CommandGroup * drAG = (ocDesignObj->m_iPK_CommandGroup==0 ? null : m_spDatabase_pluto_main->tCommandGroup[ocDesignObj->m_iPK_CommandGroup]);

	fstr_Orbiter <<  ocDesignObj->m_iPK_CommandGroup==0 ? "" : ocDesignObj->m_iPK_CommandGroup.ToString() );
	fstr_Orbiter << "|");
	**************************   was action|action parms|action group
	*/
	/*			
	string sDesignObjGoto="";
	if( ocDesignObj->m_sDesignObjGoto!=null )
	sDesignObjGoto=ocDesignObj->m_sDesignObjGoto;
	else if( ocDesignObj->m_DesignObj_GeneratorGoto!=null )
	sDesignObjGoto = ocDesignObj->m_DesignObj_GeneratorGoto->m_pRow_DesignObj->PK_DesignObj_get() + "." + ocDesignObj->m_DesignObj_GeneratorGoto->m_iVersion + ".0";

	// We're pointing to a valid action group, look if there are any variable assign actions in there
	if( drAG!=null )
	{
	DataRow[] drAs = drAG.dr.GetChildRows(MyDataSet.MyRelations.CommandGroup_Command_FK_CommandGroup);
	foreach(DataRow dr in drAs)
	{
	Row_CommandGroup_Command * drAGA = new Row_CommandGroup_Command *(dr);
	if( drAGA->FK_Command_get() == COMMAND_Set_Variable_CONST )
	{
	int PK_Variable=0;
	string sValue=null;
	DataRow[] drAGAPs = drAGA.dr.GetChildRows(MyDataSet.MyRelations.CommandGroup_Command_CommandParameter_FK_CommandGroup_Command);
	foreach(DataRow dragap in drAGAPs)
	{
	Row_CommandGroup_Command_CommandParameter * drAGAP = new Row_CommandGroup_Command_CommandParameter *(dragap);
	if( drAGAP->FK_CommandParameter_get()==COMMANDPARAMETER_PK_Variable_CONST )
	{
	if( !drAGAP->FK_Variable_isNull() )
	PK_Variable = Convert.ToInt32(ocDesignObj->m_htVariables[drAGAP->FK_Variable_get()]);
	else if( !drAGAP->Value_isNull() )
	PK_Variable = Convert.ToInt32(drAGAP->Value_get());
	}
	else if( drAGAP->FK_CommandParameter_get()==COMMANDPARAMETER_Value_To_Assign_CONST )
	{
	if( !drAGAP->FK_Variable_isNull() )
	sValue = ocDesignObj->m_htVariables[drAGAP->FK_Variable_get()].ToString();
	else if( !drAGAP->Value_isNull() )
	sValue = drAGAP->Value_get();
	}
	}
	if( PK_Variable>0 && sValue!=null )
	{
	VariableAssignment va = new VariableAssignment(PK_Variable,sValue);
	ocDesignObj->m_alVariableAssignments.Add(va);
	if( !ocDesignObj->m_pOrbiterGenerator->m_htUsedVariables.ContainsKey(PK_Variable) )
	ocDesignObj->m_pOrbiterGenerator->m_htUsedVariables.Add(PK_Variable,1);
	}
	}
	}
	}
	*/
	/*			fstr_Orbiter << ocDesignObj->m_alVariableAssignments.Count + "|");
	foreach(VariableAssignment va in ocDesignObj->m_alVariableAssignments)
	{
	fstr_Orbiter << va.m_iPK_Variable + "|" + va.m_sValue + "|");
	}

	ArrayList alLocalCommands = new ArrayList();
	foreach(CGZone oz in ocDesignObj->m_alZones)
	{
	foreach(CGCommand oa in oz->m_alCommands)
	{
	// Don't add the goto's, we did this already
	if( oa->m_bLocalCommand || 
	(oa->drCommand->HandledByOrbiter_get() && oa->PK_Command!=COMMAND_Goto_Screen_CONST && oa->PK_Command!=COMMAND_NAV_GOTO_DEVICE_CONST) )
	alLocalCommands.Add(oa);
	}
	}

	if( drCommand!=null && drCommand->HandledByOrbiter_get() )
	{
	if( !ocDesignObj->m_pRow_DesignObjVariation->FK_Command_isNull() )
	{
	CGCommand oa = new CGCommand(ocDesignObj->m_pRow_DesignObjVariation,ocDesignObj);
	alLocalCommands.Add(oa);
	}
	else
	{
	CGCommand oa = new CGCommand(ocDesignObj->m_pRow_DesignObjVariation_Standard,ocDesignObj);
	alLocalCommands.Add(oa);
	}
	}
	*/
	/*			foreach(CGCommand oa in alLocalCommands)
	{
	fstr_Orbiter << oa->PK_Command + "|" + oa->m_PK_C_ExecTime + "|" + oa->m_alParms.Count + "|");
	foreach(CGCommandParm op in oa->m_alParms)
	{
	fstr_Orbiter << op.PK_CommandParameter + "|" + op.sValue + "|");
	}
	}

	fstr_Orbiter <<  sDesignObjGoto + "|");
	*/

	// This is the text that is not prerendered, going to the orbiter: pDesignObj_Generator->m_TextList
	// m_alText has all the text objects
	for(size_t s=0;s<ocDesignObj->m_vectDesignObjText.size();++s)
	{
		CGText *pDesignObjText = (CGText *) ocDesignObj->m_vectDesignObjText[s];
		OutputText(ocDesignObj,pDesignObjText,pDesignObjText->m_bPreRender,ParentScreen);
		if( pDesignObjText->m_bPreRender )
			continue;

		for(MapTextStyle::iterator it=pDesignObjText->m_mapTextStyle.begin();it!=pDesignObjText->m_mapTextStyle.end();++it)
		{
			TextStyle *pTextStyle = (*it).second;;
			m_mapTextStyle[pTextStyle->m_iPK_Style]=pTextStyle;
		}
	}

	/*
	int Children = (int) ocDesignObj->m_alChildDesignObjs.size();
	for(size_t s=0;s<ocDesignObj->m_alNonMPArrays.size();++s)
	{
	CGArray *oca = ocDesignObj->m_alNonMPArrays[s];
	Children += (int) oca->m_alChildDesignObjs.size();
	}

	// ArrayPage is 0 based, count is 1 based, so output whenever count>arraypage
	if( ( (int)ocDesignObj->m_alMPArray.size())>ArrayPage )
	Children += (int) (ocDesignObj->m_alMPArray[ArrayPage])->m_alChildDesignObjs.size();
	*/
	for(size_t s=0;s<ocDesignObj->m_alChildDesignObjs.size();++s)
	{
		DesignObj_Generator *oco = ocDesignObj->m_alChildDesignObjs[s];
		ocDesignObj->m_ChildObjects.push_back(oco);
		oco->m_bContainsFloorplans = ocDesignObj->m_bContainsFloorplans;
		OutputDesignObjs(oco,0,true,ParentScreen);
	}

	for(size_t s=0;s<ocDesignObj->m_alNonMPArrays.size();++s)
	{
		CGArray *oca = ocDesignObj->m_alNonMPArrays[s];
		for(size_t s=0;s<oca->m_alChildDesignObjs_OfArray.size();++s)
		{
			DesignObj_Generator *oco = oca->m_alChildDesignObjs_OfArray[s];
			ocDesignObj->m_ChildObjects.push_back(oco);
			oco->m_bContainsFloorplans = ocDesignObj->m_bContainsFloorplans;
			OutputDesignObjs(oco,0,true,ParentScreen);
		}
	}

	if( ((int)ocDesignObj->m_alMPArray.size())>ArrayPage )
	{
		CGArray *oca = ocDesignObj->m_alMPArray[ArrayPage];
		for(size_t s=0;s<oca->m_alChildDesignObjs_OfArray.size();++s)
		{
			DesignObj_Generator *oco = oca->m_alChildDesignObjs_OfArray[s];
			ocDesignObj->m_ChildObjects.push_back(oco);
			oco->m_bContainsFloorplans = ocDesignObj->m_bContainsFloorplans;
			OutputDesignObjs(oco,ArrayPage,true,ParentScreen);
		}
	}
if( ocDesignObj->m_pRow_DesignObj->PK_DesignObj_get()==5112 )
int k=2;

if( ocDesignObj->m_ObjectID.find("5112.5.0.5113")!=string::npos )
int k=2;

	// Offset for the spacing
	if( ocDesignObj->m_bDontScale==false && m_mapPopups.find(atoi(ParentScreen.c_str()))==m_mapPopups.end() )
	{
if( ocDesignObj->m_ObjectID.find("5112")!=string::npos )
int k=2;
		ocDesignObj->m_rPosition.X += m_rSpacing.X;
		ocDesignObj->m_rPosition.Y += m_rSpacing.Y;
		ocDesignObj->m_rBackgroundPosition.X += m_rSpacing.X;
		ocDesignObj->m_rBackgroundPosition.Y += m_rSpacing.Y;
	}
}


void OrbiterGenerator::OutputText(DesignObj_Generator *ocDesignObj,CGText *p_DesignObjText,bool bPrerender,string ParentScreen)
{
	p_DesignObjText->m_PK_Text = p_DesignObjText->m_pdrText_LS->FK_Text_get();
	if( p_DesignObjText->m_rPosition.Right()>m_sScaledSize.Width )
		p_DesignObjText->m_rPosition.Right(m_sScaledSize.Width);
	if( p_DesignObjText->m_rPosition.Bottom()>m_sScaledSize.Height )
		p_DesignObjText->m_rPosition.Bottom(m_sScaledSize.Height);

	if( p_DesignObjText->m_iPK_HorizAlignment==0 ) // Wasn't already set
	{
		if( !p_DesignObjText->m_pRow_DesignObjVariation_Text_Skin_Language->FK_HorizAlignment_isNull() )
			p_DesignObjText->m_iPK_HorizAlignment = p_DesignObjText->m_pRow_DesignObjVariation_Text_Skin_Language->FK_HorizAlignment_get();
		else
		{
			MapTextStyle::iterator it=p_DesignObjText->m_mapTextStyle.begin();
			if( it==p_DesignObjText->m_mapTextStyle.end() )
				p_DesignObjText->m_iPK_HorizAlignment=1;
			else
				p_DesignObjText->m_iPK_HorizAlignment=it->second->m_iPK_HorizAlignment;
		}
	}

	if( p_DesignObjText->m_iPK_VertAlignment==0 ) // Wasn't already set
	{
		if( !p_DesignObjText->m_pRow_DesignObjVariation_Text_Skin_Language->FK_VertAlignment_isNull() )
			p_DesignObjText->m_iPK_VertAlignment = p_DesignObjText->m_pRow_DesignObjVariation_Text_Skin_Language->FK_VertAlignment_get();
		else
		{
			MapTextStyle::iterator it=p_DesignObjText->m_mapTextStyle.begin();
			if( it==p_DesignObjText->m_mapTextStyle.end() )
				p_DesignObjText->m_iPK_VertAlignment=1;
			else
				p_DesignObjText->m_iPK_VertAlignment=it->second->m_iPK_VertAlignment;
		}
	}
//	p_DesignObjText->m_rotate = p_DesignObjText->m_pRow_DesignObjVariation_Text_Skin_Language->Rotate_get();
//	p_DesignObjText->m_Opacity = p_DesignObjText->m_pRow_DesignObjVariation_Text_Skin_Language->Opacity_get();
	// If we force a background color, put it in the default style

/* 
AB 1/17/2005 - text styles are shared -- this was causing it to change backgrounds unintentionally
	if( !p_DesignObjText->m_pRow_DesignObjVariation_Text_Skin_Language->PlainBackgroundColor_isNull() )
	{
		PlutoColor cBackground(p_DesignObjText->m_pRow_DesignObjVariation_Text_Skin_Language->PlainBackgroundColor_get());
		TextStyle *pTextStyle = p_DesignObjText->m_mapTextStyle_Find(0);
		pTextStyle->m_BackColor=cBackground;
	}
*/

	if( !bPrerender )
	{
		map<int,string>::iterator itatv;
		for(itatv=p_DesignObjText->m_mapAltVersions.begin();itatv!=p_DesignObjText->m_mapAltVersions.end();++itatv)
		{
			p_DesignObjText->m_mapAltVersions[(*itatv).first] = (*itatv).second;
		}
	}
if( ocDesignObj->m_pRow_DesignObj->PK_DesignObj_get()==4967 || ocDesignObj->m_pRow_DesignObj->PK_DesignObj_get()==3558 )
int k=2;
	// Offset for the spacing
	if( ocDesignObj->m_bDontScale==false && m_mapPopups.find(atoi(ParentScreen.c_str()))==m_mapPopups.end() )
	{
		p_DesignObjText->m_rPosition.X += m_rSpacing.X;
		p_DesignObjText->m_rPosition.Y += m_rSpacing.Y;
	}
}

void OrbiterGenerator::OutputCriteriaNest(Row_CriteriaParmNesting *row)
{
}

void OrbiterGenerator::MatchChildDevicesToLocation(LocationInfo *li,Row_Device *pRow_Device,bool bStartWithTopMost)
{
	if( bStartWithTopMost )
		while( pRow_Device->FK_Device_ControlledVia_getrow() )
			pRow_Device = pRow_Device->FK_Device_ControlledVia_getrow();

	vector<Row_Device *> vectChildren;
	pRow_Device->Device_FK_Device_ControlledVia_getrows(&vectChildren);

	for(size_t s=0;s<vectChildren.size();++s)
	{
		Row_Device *pRow_Device_MDChild = vectChildren[s];
		MatchChildDevicesToLocation(li,pRow_Device_MDChild,false);  // These devices may be children of an orbiter also

		switch( pRow_Device_MDChild->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get() )
		{
		case DEVICECATEGORY_App_Server_CONST:
			li->m_dwPK_Device_AppServer = pRow_Device_MDChild->PK_Device_get();
			break;
		case DEVICECATEGORY_Infrared_Receivers_CONST:
			li->m_dwPK_Device_IRReceiver = pRow_Device_MDChild->PK_Device_get();
			break;
		case DEVICECATEGORY_LCDVFD_Displays_CONST:
			li->m_dwPK_Device_LCD_VFD = pRow_Device_MDChild->PK_Device_get();
			break;
		case DEVICECATEGORY_Disc_Drives_CONST:
			li->m_dwPK_Device_DiscDrive = pRow_Device_MDChild->PK_Device_get();
			break;

#pragma message("NOTE: There is not yet a web browser category! (OrbiterGenerator::MatchChildDevicesToLocation)")

//todo		case DEVICECATEGORY_WebBrowser_CONST:
//			li->m_dwPK_Device_WebBrowser = pRow_Device_MDChild->PK_Device_get();
//			break;
//todo		case DEVICECATEGORY_General_Info_CONST:
//			li->m_dwPK_Device_MediaBurner = pRow_Device_MDChild->PK_Device_get();
//			break;
//todo		case DEVICECATEGORY_General_Info_CONST:
//			li->m_dwPK_Device_VideoConf = pRow_Device_MDChild->PK_Device_get();
//			break;
//todo		case DEVICECATEGORY_General_Info_CONST:
//			li->m_dwPK_Device_CamcorderCapt = pRow_Device_MDChild->PK_Device_get();
//			break;
		case DEVICECATEGORY_Standard_Orbiter_CONST:
			if( pRow_Device_MDChild->FK_DeviceTemplate_get()==DEVICETEMPLATE_OnScreen_Orbiter_CONST )
				li->m_dwPK_Device_Orbiter = pRow_Device_MDChild->PK_Device_get();
			break;

		};
	}
}

Row_Size *OrbiterGenerator::TranslateSize(string sSize)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Translating size: %s",sSize.c_str());
	string::size_type pos=0;
	int Width=atoi(StringUtils::Tokenize(sSize," ",pos).c_str());
	int Height=atoi(StringUtils::Tokenize(sSize," ",pos).c_str());
	if( Width<200 || Width>5000 || Height<200 || Height>5000 )
		return NULL; // Check some reasonable ranges

	// Try to find an exact match for this UI
	vector<Row_Size *> vectRow_Size;
	m_spDatabase_pluto_main->Size_get()->GetRows("Width=" + StringUtils::itos(Width) + " AND Height=" + StringUtils::itos(Height) + " AND FK_UI=" + StringUtils::itos(m_pRow_UI->PK_UI_get()),&vectRow_Size);
	if( vectRow_Size.size() )
		return vectRow_Size[0];

	// Any match for this width and height
	m_spDatabase_pluto_main->Size_get()->GetRows("Width=" + StringUtils::itos(Width) + " AND Height=" + StringUtils::itos(Height),&vectRow_Size);
	if( vectRow_Size.size() )
		return vectRow_Size[0];

	// No matches.  Build our own
	int Scale = Height * 1000 / 1600;
	Row_Size *pRow_Size = new Row_Size(m_spDatabase_pluto_main->Size_get());
	pRow_Size->Width_set(Width);
	pRow_Size->Height_set(Height);
	if( Width==1280 && Height==1024 )  // 1280x1024 is not a normal 3:4 aspect ratio.  Other VGA resolutions are
	{
		pRow_Size->ScaleX_set(600);
		pRow_Size->ScaleY_set(640);
	}
	else
	{
		pRow_Size->ScaleX_set(Scale);
		pRow_Size->ScaleY_set(Scale);
	}
	pRow_Size->PreserveAspectRatio_set(1);
	return pRow_Size;
}

void OrbiterGenerator::ScaleCommandList(DesignObj_Generator *ocDesignObj,DesignObjCommandList &CommandList)
{
	DesignObjCommandList::iterator itActions;
	for(itActions=CommandList.begin();itActions!=CommandList.end();++itActions)
	{	
		CGCommand *oa = (CGCommand *) *itActions;

		map<int, string>::iterator itParm;
		for(itParm=oa->m_ParameterList.begin();itParm!=oa->m_ParameterList.end();++itParm)
		{
			if( oa->m_PK_Command==COMMAND_Goto_DesignObj_CONST && (*itParm).first==COMMANDPARAMETER_PK_DesignObj_CONST )
			{
				if( (*itParm).second.find("<%=")==string::npos )
				{
					string Value="";
					if( ocDesignObj->m_sDesignObjGoto.length()>0 )  // This must be going to another page in an array
						Value = ocDesignObj->m_sDesignObjGoto;
					else if( ocDesignObj->m_DesignObj_GeneratorGoto!=NULL )
						Value = StringUtils::itos(ocDesignObj->m_DesignObj_GeneratorGoto->m_pRow_DesignObj->PK_DesignObj_get()) + "." + StringUtils::itos(ocDesignObj->m_DesignObj_GeneratorGoto->m_iVersion) + ".0";
	//						else if( (*itParm).second.find("<%=")!=string::npos ) // ?? todo -- this means that screens, like the main menu that got skipped in handle goto were ignored
					else
						Value = (*itParm).second;
					oa->m_ParameterList[COMMANDPARAMETER_PK_DesignObj_CONST]=Value;
				}
			}
			else if( (oa->m_PK_Command==COMMAND_Show_Popup_CONST || oa->m_PK_Command==COMMAND_Use_Popup_Remote_Controls_CONST || oa->m_PK_Command==COMMAND_Use_Popup_File_List_CONST || oa->m_PK_Command==COMMAND_Show_Floorplan_CONST ) 
				&& ((*itParm).first==COMMANDPARAMETER_Position_X_CONST || (*itParm).first==COMMANDPARAMETER_Position_Y_CONST) )
			{
				int Size=atoi(itParm->second.c_str());
				Size = Size * ((*itParm).first==COMMANDPARAMETER_Position_X_CONST ? m_sScale.Width : m_sScale.Height) / 1000 * ocDesignObj->m_iScale/100;
				oa->m_ParameterList[(*itParm).first] = StringUtils::itos(Size);
			}
			else
				oa->m_ParameterList[(*itParm).first] = StringUtils::Replace((*itParm).second,"<%=!%>",StringUtils::itos(m_pRow_Device->PK_Device_get()));
		}
	}
}

bool OrbiterGenerator::CommonControlledVia(Row_Device *pRow_Device1,Row_Device *pRow_Device2)
{
	map<int,bool> mapDevices;
	Row_Device *pRow_Device_CV=pRow_Device1->FK_Device_ControlledVia_getrow();
	while( pRow_Device_CV )
	{
		mapDevices[pRow_Device_CV->PK_Device_get()] = true;
		pRow_Device_CV = pRow_Device_CV->FK_Device_ControlledVia_getrow();
	}

	pRow_Device_CV=pRow_Device2->FK_Device_ControlledVia_getrow();
	while( pRow_Device_CV )
	{
		if( mapDevices[pRow_Device_CV->PK_Device_get()] )
			return true;
		pRow_Device_CV = pRow_Device_CV->FK_Device_ControlledVia_getrow();
	}
	return false;
}

string OrbiterGenerator::First2Dots(string sDesignObj)
{
	string::size_type pos_first_dot = sDesignObj.find('.');
	if( pos_first_dot==string::npos )
		return sDesignObj + ".0";

	string::size_type pos_second_dot = sDesignObj.find('.',pos_first_dot+1);
	if( pos_second_dot==string::npos )
		return sDesignObj;
    m_iLocation = atoi(sDesignObj.substr(pos_first_dot+1).c_str());

	DequeLocationInfo::iterator itd;
	LocationInfo *li=NULL;
	for(itd=m_dequeLocation.begin();itd!=m_dequeLocation.end();++itd)
	{
		if( (*itd)->iLocation==m_iLocation )
		{
			li = *itd;
			break;
		}
	}

	m_pRow_Room = m_spDatabase_pluto_main->Room_get()->GetRow(li->PK_Room);
	m_pRow_EntertainArea = li->PK_EntertainArea>0 ? m_spDatabase_pluto_main->EntertainArea_get()->GetRow(li->PK_EntertainArea) : NULL;
	return sDesignObj.substr(0,pos_second_dot);
}

/*
void OrbiterGenerator::PopulateScreenMap()
{
	string SQL="(FK_UI=" + StringUtils::itos(m_pRow_UI->PK_UI_get()) + " OR FK_UI IS NULL) AND "
		"(FK_Skin=" + StringUtils::itos(m_pRow_Skin->PK_Skin_get()) + " OR FK_Skin IS NULL) AND "
		"(FK_DeviceTemplate=" + StringUtils::itos(m_pRow_Device->FK_DeviceTemplate_get()) + " OR FK_DeviceTemplate IS NULL) "
		"ORDER BY FK_Screen,FK_DeviceTemplate DESC, FK_UI DESC, FK_Skin DESC";

	vector<Row_Screen_DesignObj *> vectRow_Screen_DesignObj;
	m_spDatabase_pluto_main->Screen_DesignObj_get()->GetRows(SQL,&vectRow_Screen_DesignObj);

	int PK_Screen_Last=0;  // Keep track of the last one, we only 1 want design obj per screen, the first one
	for(size_t s=0;s<vectRow_Screen_DesignObj.size();++s)
	{
		Row_Screen_DesignObj *pRow_Screen_DesignObj = vectRow_Screen_DesignObj[s];
		if( pRow_Screen_DesignObj->FK_Screen_get()==PK_Screen_Last )
			continue;
		PK_Screen_Last=pRow_Screen_DesignObj->FK_Screen_get();
		m_mapDesignObj[PK_Screen_Last] = pRow_Screen_DesignObj->FK_DesignObj_get();
	}
}
*/
Row_DesignObj *OrbiterGenerator::GetDesignObjFromScreen(int PK_Screen)
{
	map<int,int>::iterator it=m_mapDesignObj.find(PK_Screen);
	if( it==m_mapDesignObj.end() )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"No design obj for screen %d",PK_Screen);
		return NULL;
	}

	return m_spDatabase_pluto_main->DesignObj_get()->GetRow(it->second);
}

Row_DesignObj *OrbiterGenerator::GetDesignObjFromScreen(Row_Screen *pRow_Screen)
{
	return GetDesignObjFromScreen(pRow_Screen->PK_Screen_get());
}

Row_Skin *OrbiterGenerator::TranslateSkin(Row_Skin *pRow_Skin)
{
	int PK_Skin_New=0;
	for(map<string,string>::const_iterator it=g_DCEConfig.Parameters().begin();it!=g_DCEConfig.Parameters().end();++it)
	{
		if( StringUtils::StartsWith(it->first,"UseSkinSettings_") )
		{
			int PK_Skin = atoi(it->first.substr(16).c_str());
			m_mapSkinTranslated[ PK_Skin ] = atoi(it->second.c_str());
		}
		else if( StringUtils::StartsWith(it->first,"ReplaceSkin_") )
		{
			int PK_Skin = atoi(it->first.substr(12).c_str());
			if( PK_Skin==pRow_Skin->PK_Skin_get() )
				PK_Skin_New = atoi(it->second.c_str());
		}
	}


	if( PK_Skin_New )
		return m_spDatabase_pluto_main->Skin_get()->GetRow(PK_Skin_New);
	else
		return NULL;
}

void OrbiterGenerator::PopulateEffects(map<int, int> &mapEffects, int FK_Skin)
{
	mapEffects.clear();

	vector<Row_EffectType_Effect_Skin *> vectRow_EffectType_Effect_Skin;
	m_spDatabase_pluto_main->EffectType_Effect_Skin_get()->GetRows("WHERE FK_Skin = " + StringUtils::ltos(FK_Skin), &vectRow_EffectType_Effect_Skin);

	vector<Row_EffectType_Effect_Skin *>::iterator it;
	for(it = vectRow_EffectType_Effect_Skin.begin(); it != vectRow_EffectType_Effect_Skin.end(); ++it)
	{
		Row_EffectType_Effect_Skin *pRow = *it;
		mapEffects[pRow->FK_EffectType_get()] = pRow->FK_Effect_get();
	}
}

void OrbiterGenerator::FixupSpecialOrbiters(list<LocationInfo *> &listLocationInfo)
{
	// There are special cases with Orbiter's that have device data to refer to a device that is their m/d
	vector<Row_Device *> vectRow_Device;
	m_spDatabase_pluto_main->Device_get()->GetRows("JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=" TOSTRING(DEVICECATEGORY_Standard_Orbiter_CONST),&vectRow_Device);
	for(vector<Row_Device *>::iterator it=vectRow_Device.begin();it!=vectRow_Device.end();++it)
	{
		Row_Device *pRow_Device = *it;
		Row_Device_DeviceData *pRow_Device_DeviceData = m_spDatabase_pluto_main->Device_DeviceData_get()->GetRow(pRow_Device->PK_Device_get(),DEVICEDATA_PK_Device_CONST);
		if( pRow_Device_DeviceData )
		{
			Row_Device *pRow_Device_MD = m_spDatabase_pluto_main->Device_get()->GetRow(atoi(pRow_Device_DeviceData->IK_DeviceData_get().c_str()));
			if( !pRow_Device_MD )
				continue;  // Nothing to do

			// This may point to an m/d, or the child of an m/d
			Row_DeviceTemplate *pRow_DeviceTemplate = pRow_Device_MD->FK_DeviceTemplate_getrow();
			if( !pRow_DeviceTemplate )
				continue;

			if( pRow_DeviceTemplate->FK_DeviceCategory_get()!=DEVICECATEGORY_Media_Director_CONST )
			{
				// See if it was the child
				pRow_Device_MD = pRow_Device_MD->FK_Device_ControlledVia_getrow();
				pRow_DeviceTemplate = pRow_Device_MD->FK_DeviceTemplate_getrow();
				if( !pRow_DeviceTemplate )
					continue;
				if( pRow_DeviceTemplate->FK_DeviceCategory_get()!=DEVICECATEGORY_Media_Director_CONST )
					continue;
			}

			// Yep pRow_Device_MD is now the m/d.  Find the location
			list<LocationInfo *>::iterator it;
			for(it=listLocationInfo.begin();it!=listLocationInfo.end();++it)
			{
				LocationInfo *pLocationInfo = (*it);
				if( pLocationInfo->m_dwPK_Device_MediaDirector==pRow_Device_MD->PK_Device_get() )
				{
					// Got it
					if( pLocationInfo->m_dwPK_Device_Orbiter<1 )
						pLocationInfo->m_dwPK_Device_Orbiter = pRow_Device->PK_Device_get();
					continue;
				}
			}
		}
	}
}

