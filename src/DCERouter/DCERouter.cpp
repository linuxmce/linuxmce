/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the GNU Public License, available at:
 http://www.fsf.org/licensing/licenses/gpl.html

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Public License for more details.

 */
/*
 * $Id: Router.cpp,v 1.21 2004/10/28 05:11:28 mihaid Exp $
 */
#include <string>
using namespace std;

#include <math.h>
#ifdef WIN32
#include <WinSock.h>
#endif
#if !defined(WIN32) && !defined(__APPLE_CC__)
#include <sys/vfs.h>
#include <dirent.h>
#endif
#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCE/Message.h"
#include "DCE/ServerSocket.h"
#include <sys/stat.h>
#include "DCE/DCERouter.h"
#include "DCE/DeviceData_Router.h"
#include "DCE/AlarmManager.h"
#include "DCE/DCEConfig.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_DeviceData.h"
#include "pluto_main/Table_DeviceTemplate_DeviceData.h"
#include "pluto_main/Table_DeviceCategory_DeviceData.h"
#include "pluto_main/Table_CommandParameter.h"
#include "pluto_main/Table_Command_Pipe.h"
#include "pluto_main/Table_CommandGroup.h"
#include "pluto_main/Table_CommandGroup_Command.h"
#include "pluto_main/Table_CommandGroup_Command_CommandParameter.h"
#include "pluto_main/Table_Event.h"
#include "pluto_main/Table_EventParameter.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_DeviceGroup.h"
#include "pluto_main/Table_Device_DeviceGroup.h"
#include "pluto_main/Table_Device_Device_Related.h"
#include "pluto_main/Table_DeviceCategory.h"
#include "pluto_main/Table_Device_DeviceData.h"
#include "pluto_main/Table_DeviceTemplate_DeviceCommandGroup.h"
#include "pluto_main/Table_DeviceCommandGroup.h"
#include "pluto_main/Table_DeviceCommandGroup_Command.h"
#include "pluto_main/Table_DeviceTemplate_Input.h"
#include "pluto_main/Table_DeviceTemplate_DSPMode.h"
#include "pluto_main/Table_Device_Device_Pipe.h"
#include "pluto_main/Table_Command.h"
#include "pluto_main/Table_CommandCategory.h"
#include "pluto_main/Table_Room.h"
#include "pluto_main/Table_Installation.h"

#include "DCE/MessageXML.h"
#include "DCE/Message.h"

#ifndef WIN32
#include "dlfcn.h" // for Plugins (Dynamically Loaded Shared Objects)
#endif

#ifdef WIN32
#include<conio.h>
#endif

using namespace DCE;
extern DCEConfig g_DCEConfig;

#define EXTERN_REVISION_DATE
#include "version.h"

#define	 SERIALIZE_MESSAGE_FILE		"/var/PersistentDceMessages"

bool SerializeMessageXML(Message *pMessage, char *&pData, size_t &nSize);
bool DeserializeMessageXML(Message *pMessage, char *pData, size_t nSize);

bool g_WatchDogFlag=false;
void* WatchDogRoutine(void* param)
{
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"DCERouter Started watchdog routine\n");
    Sleep(20000);
	if (g_WatchDogFlag)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Terminating DCERouter: watchdog detected hard deadlock, seems soft reload failed\n");
#if !defined(WIN32) && !defined(__APPLE_CC__)
		fflush(stdout);
		kill(getpid(), SIGKILL);
#endif
	}
		
	return NULL;
}

void* MessageQueueThread_DCER(void* param) // renamed to cancel link-time name collision in MS C++ 7.0 / VS .NET 2002
{
    Router* pc = (Router*)param;
    pc->ProcessQueue();

    return NULL;
}

#define ALARM_DELAYED_COMMAND_EXECUTION		1

class DelayedCommandInfo {
	public:
		int m_PK_CommandGroup,m_iStartingCommand;
		string m_sDescription,m_sHint;
		DeviceData_Router *m_pDevice_Sender;
};

extern void (*g_pDeadlockHandler)(PlutoLock *pPlutoLock);
Router *g_pRouter=NULL;
void DeadlockHandler(PlutoLock *pPlutoLock)
{
	// This isn't graceful, but for the moment in the event of a deadlock we'll just kill everything and force a reload
	if( g_pRouter )
	{
		int Delay = atoi(g_DCEConfig.ReadString("DelayReloadOnDeadlock").c_str());
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Deadlock detected.  DCERouter will die and reload in %d seconds",Delay);

		time_t tTimeout = time(NULL) + Delay;
		if( Delay )
			while( tTimeout > time(NULL) )
				Sleep(10);

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Telling DCERouter to reload and quit");
		g_pRouter->Reload();
		g_pRouter->Quit();
	}
}

Router::Router(int PK_Device,int PK_Installation,string BasePath,string DBHost,string DBUser,string DBPassword,string DBName,int DBPort,int ListenPort) :
    SocketListener("Router Dev #" + StringUtils::itos(PK_Device)), DBHelper(DBHost,DBUser,DBPassword,DBName,DBPort), 
		m_CoreMutex("core"), m_InterceptorMutex("interceptor"), m_MessageQueueMutex("messagequeue")
{
	g_pRouter=this;  // For the deadlock handler
	g_pDeadlockHandler=DeadlockHandler;
    m_sBasePath=BasePath;
    m_bReload = false;
    m_bQuit = false;
    m_dwPK_Device=PK_Device;
    m_dwPK_Installation=PK_Installation;
    m_pDeviceStructure=NULL;
	m_pBufferForDeviceCategories=NULL;
	m_pRow_Device_Me=NULL;
    m_sDBHost=DBHost;
    m_sDBUser=DBUser;
    m_sDBPassword=DBPassword;
    m_sDBName=DBName;
    m_dwIDBPort=DBPort;
    m_bIsLoading=false;
	m_dwPK_Device_Largest=0;
	m_bStopProcessingMessages=false;
	m_pRow_Installation=NULL;

    m_CoreMutex.Init(NULL);
    m_InterceptorMutex.Init(NULL);
    pthread_cond_init(&m_MessageQueueCond, NULL);
    m_MessageQueueMutex.Init(NULL,&m_MessageQueueCond);

    if(pthread_create(&m_pthread_queue_id, NULL, MessageQueueThread_DCER, (void*)this))
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot create device event thread1");
    }

    // Parse the config file
    char buf[200];
    gethostname(buf,sizeof(buf));

    m_Routing_DeviceToController.clear();
    m_Routing_ControllerToIP.clear();

    m_Port = ListenPort;

    m_pDatabase_pluto_main = new Database_pluto_main(LoggerWrapper::GetInstance());

    if(!m_pDatabase_pluto_main->Connect(m_sDBHost,m_sDBUser,m_sDBPassword,m_sDBName,m_dwIDBPort) )
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot connect to database!");
		exit(1);
    }

    if( !m_dwPK_Installation && g_DCEConfig.m_iPK_Installation>0 )
        m_dwPK_Installation = g_DCEConfig.m_iPK_Installation;

    if( !m_dwPK_Device && g_DCEConfig.m_iPK_Device_Computer>0 )
    {
        vector<Row_Device *> vectRow_Device;
        m_pDatabase_pluto_main->Device_get()->GetRows( string(DEVICE_FK_DEVICETEMPLATE_FIELD) + "=" + StringUtils::itos(DEVICETEMPLATE_DCERouter_CONST)
            + " AND " + DEVICE_FK_DEVICE_CONTROLLEDVIA_FIELD + "=" + StringUtils::itos(g_DCEConfig.m_iPK_Device_Computer), &vectRow_Device);
        if( vectRow_Device.size()==1 )
        {
            Row_Device *pRow_Device = vectRow_Device[0];
            m_dwPK_Device = pRow_Device->PK_Device_get();
        }
    }

    if( !m_dwPK_Installation && !m_dwPK_Device )
    {
        vector<Row_Device *> vectRow_Device;
        m_pDatabase_pluto_main->Device_get()->GetRows( string(DEVICE_FK_DEVICETEMPLATE_FIELD) + "=" + StringUtils::itos(DEVICETEMPLATE_DCERouter_CONST), &vectRow_Device);
        if( vectRow_Device.size()!=1 )
        {
            LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot determine my device ID automatically.  # of records in DB: %d",(int) vectRow_Device.size());
            cerr << "Sorry...   I cannot figure out what my device ID is automatically.  To do this there must be only" << endl;
            cerr << "1 record in the database for a device with the device template of 1 (DCE Router).  There are " << (int) vectRow_Device.size() << endl;
            cerr << "You will need to specify the device id or the installation id on the command line." << endl;
            exit(1);
        }
        m_pRow_Device_Me=vectRow_Device[0];
    }

    if( !m_pRow_Device_Me && m_dwPK_Device )
    {
        m_pRow_Device_Me = m_pDatabase_pluto_main->Device_get()->GetRow(m_dwPK_Device);
        if( m_pRow_Device_Me && m_dwPK_Installation && m_pRow_Device_Me->FK_Installation_get()!=m_dwPK_Installation )
        {
            LoggerWrapper::GetInstance()->Write(LV_WARNING,"You specified an installation ID of %d on the command line, but also a device of %d which belongs to installation %d",
				m_dwPK_Installation,m_dwPK_Device,m_pRow_Device_Me->FK_Installation_get());
        }
    }

    if( !m_pRow_Device_Me )
    {
        vector<Row_Device *> vectRow_Device;
        m_pDatabase_pluto_main->Device_get()->GetRows( string(DEVICE_FK_INSTALLATION_FIELD) + "=" + StringUtils::itos(m_dwPK_Installation) + " AND " + string(DEVICE_FK_DEVICETEMPLATE_FIELD) + "=" + StringUtils::itos(DEVICETEMPLATE_DCERouter_CONST),&vectRow_Device);
        if( vectRow_Device.size()==0 )
		{
	        m_pDatabase_pluto_main->Device_get()->GetRows( DEVICE_FK_DEVICETEMPLATE_FIELD "=" TOSTRING(DEVICETEMPLATE_DCERouter_CONST),&vectRow_Device);
	        if( vectRow_Device.size()==1 )
				LoggerWrapper::GetInstance()->Write(LV_WARNING,"Not using the installation #%d you specified",m_dwPK_Installation);
		}
        if( vectRow_Device.size()!=1 )
        {
            LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot determine my device ID automatically.  # of records in DB: %d",(int) vectRow_Device.size());
            cerr << "Sorry...   I cannot figure out what my device ID is automatically.  To do this there must be only" << endl;
            cerr << "1 record in the database for a device with the device template type of 1 (DCE Router) in installation " << m_dwPK_Installation << ".  There are " << (int) vectRow_Device.size() << endl;
            cerr << "You will need to specify the device id on the command line." << endl;
            exit(1);
        }
        m_pRow_Device_Me=vectRow_Device[0];
    }

	m_dwPK_Device = m_pRow_Device_Me->PK_Device_get();
    m_dwPK_Installation = m_pRow_Device_Me->FK_Installation_get();

	Row_Device_DeviceData *pRow_Device_DeviceData = 
        m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(m_dwPK_Device,DEVICEDATA_PK_Language_CONST);

	m_dwPK_Language = pRow_Device_DeviceData ? atoi(pRow_Device_DeviceData->IK_DeviceData_get().c_str()) : 1;
    if( m_sBasePath=="" )
    {
        // Let's find it
#ifdef WIN32
        m_sBasePath="/pluto/";
#else
        m_sBasePath="/usr/pluto/";
#endif
    }

    LoggerWrapper::GetInstance()->Write(LV_STATUS,"Running as device: %d installation: %d using path: %s",m_dwPK_Device,m_dwPK_Installation,m_sBasePath.c_str());

    m_pAlarmManager = new AlarmManager();
    m_pAlarmManager->Start(4);      //4 = number of worker threads

    Configure();
}

Router::~Router()
{
	m_bStopProcessingMessages=true;
    map<int,class Command_Impl *>::iterator it;
    for(it=m_mapPlugIn.begin();it!=m_mapPlugIn.end();++it)
	{
		class Command_Impl *pCommand_Impl = (*it).second;
		pCommand_Impl->PrepareToDelete(); // Let it kill all it's threads before we delete it, in case some threads are still using other plugins
	}

	for(it=m_mapPlugIn.begin();it!=m_mapPlugIn.end();++it)
	{
		class Command_Impl *pCommand_Impl = (*it).second;
		if (pCommand_Impl) delete pCommand_Impl;
	}
	m_mapPlugIn.clear();
	m_bTerminate=true;
	cout << "Killing everything in DCERouter" << endl;

	DropAllSockets();
	
	delete[] m_pDeviceStructure;
	delete[] m_pBufferForDeviceCategories;

#ifndef WIN32
	for(list<void *>::iterator it=m_listPluginHandles.begin();it!=m_listPluginHandles.end();++it)
	{
		void *pSO = *it;
		dlclose(pSO);
	}
	m_listPluginHandles.clear();
#endif

	for(map<int, ListCommand_Impl *>::iterator it=m_mapPlugIn_DeviceTemplate.begin();it!=m_mapPlugIn_DeviceTemplate.end();++it)
		delete (*it).second;
	m_mapPlugIn_DeviceTemplate.clear();

	for(list<class MessageInterceptorCallBack *>::iterator it=m_listMessageInterceptor_Global.begin();it!=m_listMessageInterceptor_Global.end();++it)
		delete *it;

	for(map<int,class MessageTypeInterceptor *>::iterator it=m_mapMessageTypeInterceptor.begin();it!=m_mapMessageTypeInterceptor.end();++it)
		delete (*it).second;

	for(map<int,class Room *>::iterator it=m_mapRoom.begin();it!=m_mapRoom.end();++it)
		delete (*it).second;
	m_mapRoom.clear();

    for(map<int,class Command *>::iterator it=m_mapCommand.begin();it!=m_mapCommand.end();++it)
		delete (*it).second;
	m_mapCommand.clear();

    for(map<int,class Event_Router *>::iterator it=m_mapEvent_Router.begin();it!=m_mapEvent_Router.end();++it)
		delete (*it).second;
	m_mapEvent_Router.clear();

	for(map<int,class CommandGroup *>::iterator it=m_mapCommandGroup.begin();it!=m_mapCommandGroup.end();++it)
		delete (*it).second;
	m_mapCommandGroup.clear();

	list<class DeviceData_Router *> listTopDevices;
    for(map<int,class DeviceData_Router *>::iterator it=m_mapDeviceData_Router.begin();it!=m_mapDeviceData_Router.end();++it)
	{
		class DeviceData_Router *pDeviceData_Router = (*it).second;
		if( !pDeviceData_Router->m_pDevice_ControlledVia )
			listTopDevices.push_back(pDeviceData_Router); // Delete only top level devices, since other devices delete their children
	}
	m_mapDeviceData_Router.clear();

	// Delete the top level devices
	for(list<class DeviceData_Router *>::iterator it=listTopDevices.begin();it!=listTopDevices.end();++it)
	{
		DeviceData_Router *pDeviceData_Router = *it;
			delete pDeviceData_Router;
	}

    for(map<int,ListDeviceData_Router *>::iterator it=m_mapDeviceByTemplate.begin();it!=m_mapDeviceByTemplate.end();++it)
		delete (*it).second;
	m_mapDeviceByTemplate.clear();

    for(map<int,ListDeviceData_Router *>::iterator it=m_mapDeviceByCategory.begin();it!=m_mapDeviceByCategory.end();++it)
		delete (*it).second;
	m_mapDeviceByCategory.clear();

    for(map<int,class DeviceGroup *>::iterator it=m_mapDeviceGroup.begin();it!=m_mapDeviceGroup.end();++it)
		delete (*it).second;
	m_mapDeviceGroup.clear();

    for(Map_DeviceCategory::iterator it=m_mapDeviceCategory.begin();it!=m_mapDeviceCategory.end();++it)
		delete (*it).second;
	m_mapDeviceCategory.clear();

    m_dwPK_Device_To_CommandLine.clear();
    m_Routing_DeviceToController.clear();
    m_Routing_VideoDeviceToController.clear();
    m_Routing_ControllerToIP.clear();
    m_RunningDevices.clear();
    m_MessageQueue.clear();
    m_mapCommandParmNames.clear();
	m_mapEventParmNames.clear();

    m_pAlarmManager->Stop();
    delete m_pAlarmManager;
    m_pAlarmManager = NULL;

    delete m_pDatabase_pluto_main;

	pthread_mutex_destroy(&m_CoreMutex.mutex);
	pthread_mutex_destroy(&m_InterceptorMutex.mutex);
	pthread_mutex_destroy(&m_MessageQueueMutex.mutex);
}


typedef struct
{
	int PK_Device, PK_DeviceTemplate;
	RAP_FType func;
	string log;
} plugin_activation_data;

void Router::RegisterAllPlugins()
{
	list<plugin_activation_data> list_fRAP;
	list<plugin_activation_data>::iterator i_fRAP;
	plugin_activation_data pad_tmp;
    string sLogFile;
	int iPK_Device, iPK_DeviceTemplate;
	RAP_FType RAP_RegisterAsPlugin;
	size_t s;
	
	// We're going to store a map with all the plug-ins we loaded so we can try to dynamically load plug-ins that weren't in the database
	map<string,int> mapPluginCommands;
    vector<Row_Device *> vectRow_Device;
    m_pDatabase_pluto_main->Device_get()->GetRows( string(DEVICE_FK_DEVICE_CONTROLLEDVIA_FIELD) + "=" + StringUtils::itos(m_dwPK_Device), &vectRow_Device);
    for (s = 0; s < vectRow_Device.size(); s++)
    {
        Row_Device *pRow_Device = vectRow_Device[s];

        if( !pRow_Device->FK_DeviceTemplate_getrow()->ImplementsDCE_get() )
        {
            LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"The router has a child device %d (%s).  But this isn't a valid plug-in.  ImplementsDCE must be true.",
                pRow_Device->PK_Device_get(),pRow_Device->FK_DeviceTemplate_getrow()->Description_get().c_str());
            continue;
        }
        string CommandLine = pRow_Device->FK_DeviceTemplate_getrow()->CommandLine_get();
        if( CommandLine.size()==0 )
            CommandLine = FileUtils::ValidCPPName( pRow_Device->FK_DeviceTemplate_getrow()->Description_get() );

		mapPluginCommands[CommandLine] = pRow_Device->PK_Device_get();

#ifdef AUDIDEMO
        Command_Impl *pPlugIn = CreatePlugInHardCoded(pRow_Device->PK_Device_get(), pRow_Device->FK_DeviceTemplate_get(), CommandLine);
#else
		iPK_Device = pRow_Device->PK_Device_get();
		iPK_DeviceTemplate = pRow_Device->FK_DeviceTemplate_get();
		sLogFile = StringUtils::itos(iPK_Device) + "_" + CommandLine + "_" + StringUtils::itos(iPK_DeviceTemplate) + ".log";
		
		RAP_RegisterAsPlugin = PlugIn_Load(iPK_Device, iPK_DeviceTemplate, CommandLine);
		if (RAP_RegisterAsPlugin)
		{
			pad_tmp.PK_Device = iPK_Device;
			pad_tmp.PK_DeviceTemplate = iPK_DeviceTemplate;
			pad_tmp.func = RAP_RegisterAsPlugin;
			pad_tmp.log = sLogFile;
			list_fRAP.push_back(pad_tmp);
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot load plug-in for device: %d. Be sure the file %s exists",
				iPK_Device, CommandLine.c_str());
		}
#endif
	}
	
	// Repeat and see if we can find any plugins to load dynamically
	// this is a method to "sneak" in plugins without device templates
	// as this caused troubles with some non-plugin libs we now only search in a
	// specific directory ("plugin-inject") for *.so/dll files
	list<string> listFiles;
#ifdef WIN32
	// FileUtils::FindFiles(listFiles,".","*.dll",false,true);
	FileUtils::FindFiles(listFiles,"/pluto/bin/plugin-inject","*.dll",false,true);
#else
	// FileUtils::FindFiles(listFiles,".","*.so",false,true);
	FileUtils::FindFiles(listFiles,"/usr/pluto/bin/plugin-inject","*.so",false,true);
#endif

	int PlugInNumber=0;  // How many plug-ins we're loading dynamically

	for(list<string>::iterator itFile=listFiles.begin();itFile!=listFiles.end();++itFile)
	{
		string sFile = *itFile;
		if( mapPluginCommands.find(FileUtils::FilenameWithoutPath(sFile))==mapPluginCommands.end() && mapPluginCommands.find(FileUtils::FilenameWithoutPath(sFile,false))==mapPluginCommands.end())
		{
			int PK_DeviceTemplate = DynamicallyLoadPlugin(sFile);
			if( !PK_DeviceTemplate )
				continue;

			PlugInNumber++;

			iPK_Device = m_dwPK_Device_Largest + PlugInNumber + 1000000;   // Use artificially high numbers > 1 million so there's no confusion with other devices including new ones we will create

			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Router::RegisterAllPlugins Dynamically loading plugin %s as device %d", sFile.c_str(), iPK_Device);

			DeviceData_Router *pDevice = new DeviceData_Router(iPK_Device,PK_DeviceTemplate,m_dwPK_Installation,m_dwPK_Device);
			m_mapDeviceData_Router[iPK_Device]=pDevice;
			ListDeviceData_Router *pListDeviceData_Router = m_mapDeviceByTemplate_Find(PK_DeviceTemplate);
			if( !pListDeviceData_Router )
			{
				pListDeviceData_Router = new ListDeviceData_Router();
				m_mapDeviceByTemplate[PK_DeviceTemplate] = pListDeviceData_Router;
			}
			pListDeviceData_Router->push_back(pDevice);

			RAP_RegisterAsPlugin = PlugIn_Load(iPK_Device, PK_DeviceTemplate, sFile);
			sLogFile = StringUtils::itos(iPK_Device) + "_" + sFile + "_" + StringUtils::itos(PK_DeviceTemplate) + ".log";

			if (RAP_RegisterAsPlugin)
			{
				pad_tmp.PK_Device = iPK_Device;
				pad_tmp.PK_DeviceTemplate = PK_DeviceTemplate;
				pad_tmp.func = RAP_RegisterAsPlugin;
				pad_tmp.log = sLogFile;
				list_fRAP.push_back(pad_tmp);
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot load plug-in for device: %d. Be sure the file %s exists",
					iPK_Device, sFile.c_str());
			}
		}
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Activating %d plug-ins",(int) list_fRAP.size());
	for (i_fRAP = list_fRAP.begin(); i_fRAP != list_fRAP.end(); i_fRAP++)
	{
        m_mapPlugIn[i_fRAP->PK_Device] = NULL;

		Command_Impl * pPlugIn = PlugIn_Activate(i_fRAP->PK_Device, i_fRAP->func, i_fRAP->log);
        if( !pPlugIn )
        {
            LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot initialize plug-in for device: %d", i_fRAP->PK_Device);
	    m_mapPlugIn.erase(i_fRAP->PK_Device);
        }
        else
        {
			m_mapPlugIn[i_fRAP->PK_Device] = pPlugIn;
            ListCommand_Impl *pListCommand_Impl = m_mapPlugIn_DeviceTemplate[i_fRAP->PK_DeviceTemplate];
            if( !pListCommand_Impl )
            {
                pListCommand_Impl = new ListCommand_Impl();
                m_mapPlugIn_DeviceTemplate[i_fRAP->PK_DeviceTemplate] = pListCommand_Impl;
            }
            pListCommand_Impl->push_back(pPlugIn);
        }
    }

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Registering %d plug-ins",(int) m_mapPlugIn.size());
    map<int,class Command_Impl *>::iterator it;
    for(it=m_mapPlugIn.begin();it!=m_mapPlugIn.end();++it)
    {
        Command_Impl *pPlugIn = (*it).second;
        LoggerWrapper::GetInstance()->Write(LV_STATUS,"Registering plugin: %s",pPlugIn->m_sName.c_str());
        pPlugIn->Register();
    }
}

// sCommandLine = Plugin filename
// PK_Device = 4 for now
// Description: Loads SO/DLL; Calls RegisterAsPlugin(...) from that
// Return: Result from RegisterAsPlugin(...) on success; NULL on error
RAP_FType Router::PlugIn_Load(int PK_Device, int PK_DeviceTemplate, string sCommandLine)
{
    RAP_FType RegisterAsPlugin;
    void * so_handle;
    string ErrorMessage;
#ifdef WIN32
    char MS_ErrorMessage[1024];
#endif

    if (sCommandLine == "")
        return NULL;
    
#ifndef WIN32
    #ifdef __APPLE_CC__
    sCommandLine += ".dylib";
    #else
    sCommandLine += ".so";
    #endif
    if (sCommandLine.find("/") == string::npos)
        sCommandLine = "./" + sCommandLine;
    so_handle = dlopen(sCommandLine.c_str(), RTLD_LAZY | RTLD_GLOBAL);
#else
    so_handle = LoadLibrary(sCommandLine.c_str());
#endif

    if (so_handle == NULL)
    {
#ifndef WIN32
        ErrorMessage = dlerror();
#else
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), MS_ErrorMessage, 1024 / sizeof(TCHAR), NULL);
        ErrorMessage = MS_ErrorMessage;
#endif
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Can't open plug-in file '%s': %s", sCommandLine.c_str(), ErrorMessage.c_str());
        return NULL;
    }

#ifndef WIN32
    RegisterAsPlugin = (RAP_FType) dlsym(so_handle, "RegisterAsPlugIn");
#else
    RegisterAsPlugin = (RAP_FType) GetProcAddress((HMODULE) so_handle, "RegisterAsPlugIn");
#endif

    if (RegisterAsPlugin == NULL)
    {
#ifndef WIN32
        ErrorMessage = dlerror();
#else
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), MS_ErrorMessage, 1024 / sizeof(TCHAR), NULL);
        ErrorMessage = MS_ErrorMessage;
#endif
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to load symbol 'RegisterAsPlugin' from file '%s': %s", sCommandLine.c_str(), ErrorMessage.c_str());
        return NULL;
    }

    LoggerWrapper::GetInstance()->Write(LV_WARNING, "Loaded plug-in device: %d master device: %d -- %s",PK_Device, PK_DeviceTemplate, sCommandLine.c_str());

	return RegisterAsPlugin;
}

Command_Impl * Router::PlugIn_Activate(int PK_Device, RAP_FType RegisterAsPlugin, string sLogFile)
{
    string ErrorMessage;

    char sz[100];
    strcpy(sz, sLogFile.c_str());

    // HACK ___ when I pass in a string > 16 characters, even a copy like this, it crashes when the string is destroyed as the function exits!!!!  But a constant string works????
//  return RegisterAsPlugin(this, PK_Device, sz);
	Command_Impl *pCommand_Impl=NULL;
	try
	{
		pCommand_Impl = RegisterAsPlugin(this, PK_Device, LoggerWrapper::GetInstance());
	}
	catch(...)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Plugin %d threw an exception",PK_Device);
	}

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Plugin %d activated", PK_Device);
	return pCommand_Impl;
}

int Router::DynamicallyLoadPlugin(string sFile)
{
    typedef int (* RAP_FType) ();
    RAP_FType IsRuntimePlugin;
    string ErrorMessage;
#ifdef WIN32
    char MS_ErrorMessage[1024];
#endif

    void * so_handle;

    if (sFile == "")
        return NULL;

#ifndef WIN32
    so_handle = dlopen(sFile.c_str(), RTLD_LAZY | RTLD_GLOBAL);
#else
    so_handle = LoadLibrary(sFile.c_str());
#endif

    if (so_handle == NULL)
    {
#ifndef WIN32
        ErrorMessage = dlerror();
#else
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), MS_ErrorMessage, 1024 / sizeof(TCHAR), NULL);
        ErrorMessage = MS_ErrorMessage;
#endif
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Can't open plug-in file '%s': %s", sFile.c_str(), ErrorMessage.c_str());
        return NULL;
    }

	m_listPluginHandles.push_back(so_handle);

#ifndef WIN32
    IsRuntimePlugin = (RAP_FType) dlsym(so_handle, "IsRuntimePlugin");
#else
    IsRuntimePlugin = (RAP_FType) GetProcAddress((HMODULE) so_handle, "IsRuntimePlugin");
#endif

    if (IsRuntimePlugin == NULL)
    {
        return 0;
    }

    return IsRuntimePlugin();
}

void Router::RegisterMsgInterceptor(Message *pMessage)
{
    int PK_Device_From = atoi( pMessage->m_mapParameters[PARM_FROM].c_str() );
    int PK_Device_To = atoi( pMessage->m_mapParameters[PARM_TO].c_str() );
    int PK_DeviceTemplate = atoi( pMessage->m_mapParameters[PARM_TEMPLATE].c_str() );
    int PK_DeviceCategory = atoi( pMessage->m_mapParameters[PARM_CATEGORY].c_str() );
    int MessageType = atoi( pMessage->m_mapParameters[PARM_MESSAGE_TYPE].c_str() );
    int MessageID = atoi( pMessage->m_mapParameters[PARM_MESSAGE_ID].c_str() );

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Device %d registered Message interceptor %d/%d/%d/%d/%d/%d",
		pMessage->m_dwPK_Device_From,PK_Device_From,PK_Device_To,PK_DeviceTemplate,PK_DeviceCategory,MessageType,MessageID);

    RegisterMsgInterceptor(
            new MessageInterceptorCallBack(pMessage->m_dwPK_Device_From, pMessage->m_dwID),
            PK_Device_From,PK_Device_To,PK_DeviceTemplate,PK_DeviceCategory,MessageType,MessageID );
}

void Router::StopPendingCommandGroup(int PK_CommandGroup,string sDescription,string sHint)
{
	pluto_pthread_mutex_t *p_pluto_pthread_mutex_t = m_pAlarmManager->m_Mutex_get();

	bool bGotEntry=false;
	PLUTO_SAFETY_LOCK(mm,*p_pluto_pthread_mutex_t);
	AlarmManager::SortedAlarmMap *p_SortedAlarmMap = m_pAlarmManager->m_SortedAlarms_get();
	AlarmManager::SortedAlarmMap::iterator i;
	for(i=p_SortedAlarmMap->begin();i!=p_SortedAlarmMap->end();i++)
	{
		AlarmManager::AlarmEntry *pAlarmEntry = i->second;
		if( pAlarmEntry->id==ALARM_DELAYED_COMMAND_EXECUTION )
		{
			DelayedCommandInfo *pDelayedCommandInfo = (DelayedCommandInfo *) pAlarmEntry->param;
			if( pDelayedCommandInfo->m_PK_CommandGroup==PK_CommandGroup
				|| (sDescription.empty()==false && pDelayedCommandInfo->m_sDescription==sDescription)
				|| (sHint.empty()==false && sHint==pDelayedCommandInfo->m_sHint) )
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Router::StopPendingCommandGroup Canceling pending alarm %d",pDelayedCommandInfo->m_PK_CommandGroup);
				pAlarmEntry->deleted=true;
				bGotEntry=true;
			}
		}
	}
}

void Router::ExecuteCommandGroup(int PK_CommandGroup,DeviceData_Router *pDevice_Sender,int sStartingCommand)
{
	string sSql = "SELECT PK_CommandGroup_Command,FK_Command,FK_Device,FK_DeviceGroup,DeliveryConfirmation "
		"FROM CommandGroup_Command WHERE FK_CommandGroup=" + StringUtils::itos(PK_CommandGroup) + " ORDER BY OrderNum";

	int RowCount=0;
	PlutoSqlResult result_set;
    DB_ROW row;
	if( (result_set.r=db_wrapper_query_result(sSql)) )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Execute command group: %d with %d commands from %d",
			PK_CommandGroup,result_set.r->row_count,sStartingCommand);
		while ((row = db_wrapper_fetch_row(result_set.r)))
		{
			if( ++RowCount<sStartingCommand )
				continue;

			if( atoi(row[1])==COMMAND_Delay_CONST )
			{
				sSql = "SELECT IK_CommandParameter FROM CommandGroup_Command_CommandParameter "
					"WHERE FK_CommandGroup_Command=" + string(row[0]) + " AND FK_CommandParameter="
					+ StringUtils::itos(COMMANDPARAMETER_Time_CONST);

				PlutoSqlResult result_set2;
				if( (result_set.r=db_wrapper_query_result(sSql)) && (row = db_wrapper_fetch_row(result_set.r)) )
				{
					int iSeconds = atoi(row[0]);
					DelayedCommandInfo *pDelayedCommandInfo = new DelayedCommandInfo;
					pDelayedCommandInfo->m_PK_CommandGroup=PK_CommandGroup;
					pDelayedCommandInfo->m_iStartingCommand=RowCount+1;
					pDelayedCommandInfo->m_pDevice_Sender=pDevice_Sender;

					sSql = "SELECT Description,Hint FROM CommandGroup WHERE PK_CommandGroup=" + StringUtils::itos(PK_CommandGroup);
					PlutoSqlResult result_set2;
					if( (result_set2.r=db_wrapper_query_result(sSql)) && (row = db_wrapper_fetch_row(result_set2.r)) )
					{
						if( row[0] )
							pDelayedCommandInfo->m_sDescription = row[0];
						if( row[1] )
							pDelayedCommandInfo->m_sHint = row[1];
					}

					m_pAlarmManager->AddRelativeAlarm(iSeconds,this,ALARM_DELAYED_COMMAND_EXECUTION,pDelayedCommandInfo);
					return;
				}
				else
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot execute command group delay: %d",PK_CommandGroup);
			}
			else
			{
				Message *pMessage = new Message();
				pMessage->m_dwPK_Device_From = pDevice_Sender ? pDevice_Sender->m_dwPK_Device : m_dwPK_Device;
				pMessage->m_dwMessage_Type = MESSAGETYPE_COMMAND;
				pMessage->m_dwID = atoi(row[1]);
				if( row[4][0]=='1' )
					pMessage->m_eExpectedResponse = ER_DeliveryConfirmation;
				if( row[3] && atoi(row[3]) )
				{
					pMessage->m_dwPK_Device_To = DEVICEID_GROUP;
					pMessage->m_dwPK_Device_Group_ID_To = atoi(row[3]);
				}
				else
				{
					pMessage->m_dwPK_Device_To = atoi(row[2]);
					if( pMessage->m_dwPK_Device_To==DEVICETEMPLATE_This_Orbiter_CONST && NULL != pDevice_Sender )
						pMessage->m_dwPK_Device_To = pDevice_Sender->m_dwPK_Device;
				}

				sSql = "SELECT FK_CommandParameter,IK_CommandParameter FROM CommandGroup_Command_CommandParameter "
					"WHERE FK_CommandGroup_Command=" + string(row[0]);

				PlutoSqlResult result_set2;
				if( (result_set2.r=db_wrapper_query_result(sSql)) )
				{
					while ((row = db_wrapper_fetch_row(result_set2.r)))
					{
						if( row[0] && row[1] )
							pMessage->m_mapParameters[ atoi(row[0]) ] = row[1];
					}
				}
				ReceivedMessage(NULL,pMessage);
			}
		}
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot execute command group db error: %d",PK_CommandGroup);
}

void Router::ReceivedMessage(Socket *pSocket, Message *pMessageWillBeDeleted, bool bAutoDelete)
{
	if( m_bStopProcessingMessages )
		return;

	SafetyMessage SafetyMessage(pMessageWillBeDeleted, bAutoDelete);
    DeviceData_Router *pDeviceFrom = m_mapDeviceData_Router_Find(pMessageWillBeDeleted->m_dwPK_Device_From);
    DeviceData_Router *pDeviceTo = m_mapDeviceData_Router_Find(pMessageWillBeDeleted->m_dwPK_Device_To);

	if( (*SafetyMessage)->m_dwMessage_Type==MESSAGETYPE_REGISTER_INTERCEPTOR )
    {
        RegisterMsgInterceptor( (*SafetyMessage) );
		if( pSocket )
			pSocket->SendString("OK");
		for(size_t s=0;s<(*SafetyMessage)->m_vectExtraMessages.size();++s)
			ReceivedMessage(pSocket,(*SafetyMessage)->m_vectExtraMessages[s]);
        return;
    }
	else if( (*SafetyMessage)->m_dwMessage_Type==MESSAGETYPE_PURGE_INTERCEPTORS )
    {
		PLUTO_SAFETY_LOCK(im,m_InterceptorMutex);  // Protect the interceptor map
		DeviceData_Router *pDevice = m_mapDeviceData_Router_Find( (*SafetyMessage)->m_dwPK_Device_From );
		if( pDevice )
		{
			for(size_t s=0;s<pDevice->m_vectMessageInterceptorCallBack.size();++s)
			{
				MessageInterceptorCallBack *pMessageInterceptorCallBack = pDevice->m_vectMessageInterceptorCallBack[s];
				if( pMessageInterceptorCallBack->m_plistMessageInterceptor ) // should always be there
					pMessageInterceptorCallBack->m_plistMessageInterceptor->remove( pMessageInterceptorCallBack );
			}
			pDevice->m_vectMessageInterceptorCallBack.clear();
		}
        return;
    }
    else if( (*SafetyMessage)->m_dwMessage_Type==MESSAGETYPE_EXEC_COMMAND_GROUP )
    {
		ExecuteCommandGroup((*SafetyMessage)->m_dwID,pDeviceFrom);
        return;
    }

    if( (*SafetyMessage)->m_dwPK_Device_To==DEVICEID_MASTERDEVICE || (*SafetyMessage)->m_dwPK_Device_To==DEVICEID_CATEGORY || (*SafetyMessage)->m_dwPK_Device_To==DEVICEID_GROUP )
    {
        // We're going to have to come up with the list of matching devices
        if( (*SafetyMessage)->m_dwPK_Device_To==DEVICEID_MASTERDEVICE )
            (*SafetyMessage)->m_sPK_Device_List_To=GetDevicesByDeviceTemplate((*SafetyMessage)->m_bRelativeToSender ? pDeviceFrom : pDeviceTo,(*SafetyMessage)->m_dwPK_Device_Template,(*SafetyMessage)->m_eBroadcastLevel);
        else if( (*SafetyMessage)->m_dwPK_Device_To==DEVICEID_CATEGORY )
            (*SafetyMessage)->m_sPK_Device_List_To=GetDevicesByCategory(pDeviceFrom,(*SafetyMessage)->m_dwPK_Device_Category_To,(*SafetyMessage)->m_eBroadcastLevel);
		else if( (*SafetyMessage)->m_dwPK_Device_To==DEVICEID_GROUP )
            (*SafetyMessage)->m_sPK_Device_List_To=GetDevicesByGroup((*SafetyMessage)->m_dwPK_Device_Group_ID_To);

        (*SafetyMessage)->m_dwPK_Device_To=DEVICEID_LIST;
    }
    if( (*SafetyMessage)->m_dwPK_Device_To==m_dwPK_Device || (*SafetyMessage)->m_dwPK_Device_To==DEVICEID_CATEGORY || (*SafetyMessage)->m_dwPK_Device_To==DEVICETEMPLATE_VirtDev_DCE_Router_CONST || (*SafetyMessage)->m_dwPK_Device_To==DEVICEID_DCEROUTER )
    {
		if( HandleRouterMessage(pMessageWillBeDeleted) )
			return;
	}

    if( (*SafetyMessage)->m_dwPK_Device_To==DEVICEID_LIST && (*SafetyMessage)->m_sPK_Device_List_To.length()==0 )
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Received message type: %d id: %d from: %d going to an empty device list md: %d cat: %d",
            (*SafetyMessage)->m_dwMessage_Type,(*SafetyMessage)->m_dwID,(*SafetyMessage)->m_dwPK_Device_From,(*SafetyMessage)->m_dwPK_Device_Template,(*SafetyMessage)->m_dwPK_Device_Category_To);
        ErrorResponse(pSocket,(*SafetyMessage));
        return;
    }

	if( (*SafetyMessage)->m_dwMessage_Type!=MESSAGETYPE_MESSAGE_INTERCEPTED )
	{
		PLUTO_SAFETY_LOCK(im,m_InterceptorMutex);  // Protect the interceptor map

		// Make a copy of the interceptors that we need to call since calling them may loop back into this function on another thread, so we can't use a recursive mutex
		// but we still need to protect the map in case other devices register interceptors
		list<class MessageInterceptorCallBack *> listMessageInterceptor;

		list<class MessageInterceptorCallBack *>::iterator itInterceptors;
		for(itInterceptors=m_listMessageInterceptor_Global.begin();
			itInterceptors!=m_listMessageInterceptor_Global.end();++itInterceptors)
		{
			class MessageInterceptorCallBack *pMessageInterceptorCallBack = (*itInterceptors);
			listMessageInterceptor.push_back(pMessageInterceptorCallBack);
		}

		// See if we have a handler for this type of message
		map<int,class MessageTypeInterceptor *>::iterator itMessageType = m_mapMessageTypeInterceptor.find(pMessageWillBeDeleted->m_dwMessage_Type);
		if( itMessageType!=m_mapMessageTypeInterceptor.end() )
		{
			class MessageTypeInterceptor *pMessageTypeInterceptors = (*itMessageType).second;
			CheckInterceptor(pMessageTypeInterceptors,pSocket,pMessageWillBeDeleted,pDeviceFrom,pDeviceTo,listMessageInterceptor);
		}

		// Check against all the Interceptors that didn't specify a message type
		itMessageType = m_mapMessageTypeInterceptor.find(0);
		if( itMessageType!=m_mapMessageTypeInterceptor.end() )
		{
			class MessageTypeInterceptor *pMessageTypeInterceptors = (*itMessageType).second;
			CheckInterceptor(pMessageTypeInterceptors,pSocket,pMessageWillBeDeleted,pDeviceFrom,pDeviceTo,listMessageInterceptor);
		}

		im.Release();

		for(itInterceptors=listMessageInterceptor.begin();
			itInterceptors!=listMessageInterceptor.end();++itInterceptors)
		{
			class MessageInterceptorCallBack *pMessageInterceptorCallBack = (*itInterceptors);

			class Command_Impl *pPlugIn = pMessageInterceptorCallBack->m_pPlugIn;
			MessageInterceptorFn pMessageInterceptorFn = pMessageInterceptorCallBack->m_pMessageInterceptorFn;

#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"DCERouter calling message interceptor for msg type %d id %d, device %d",
				(*SafetyMessage)->m_dwMessage_Type,(*SafetyMessage)->m_dwID,(pPlugIn ? pPlugIn->m_dwPK_Device : pMessageInterceptorCallBack->m_dwPK_Device));
#endif


			if( pMessageInterceptorFn )  // It's a plug-in
			{
				if( CALL_MEMBER_FN(*pPlugIn,pMessageInterceptorFn) (pSocket, pMessageWillBeDeleted, pDeviceFrom, pDeviceTo) )
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"void Router::ReceivedMessage -- Aborting further processing of message type %d id %d as per interceptor plugin",(*SafetyMessage)->m_dwMessage_Type,(*SafetyMessage)->m_dwID);
					return;
				}
			}
			else
			{
				Message *pMessageOriginator = new Message(pMessageWillBeDeleted);
				Message *pMessageInterceptor = new Message(0,pMessageInterceptorCallBack->m_dwPK_Device,PRIORITY_NORMAL,
					MESSAGETYPE_MESSAGE_INTERCEPTED,pMessageInterceptorCallBack->m_dwID,0);
				pMessageInterceptor->m_vectExtraMessages.push_back( pMessageOriginator );
				pMessageInterceptor->m_eExpectedResponse=ER_ReplyMessage;
				ReceivedMessage(NULL,pMessageInterceptor,false);
				bool bAbort = pMessageInterceptor->m_mapParameters.find(-1) != pMessageInterceptor->m_mapParameters.end() && pMessageInterceptor->m_mapParameters[-1]=="ABORT" ;
				delete pMessageInterceptor;
				if( bAbort ) // Special case
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"void Router::ReceivedMessage -- Aborting further processing of message type %d id %d as per interceptor",(*SafetyMessage)->m_dwMessage_Type,(*SafetyMessage)->m_dwID);
					return;
				}
			}
		}
	}
	for(int s=-1;s<(int) (*SafetyMessage)->m_vectExtraMessages.size(); ++s)
	{
		Message *pMessage = s>=0 ? (*SafetyMessage)->m_vectExtraMessages[s] : (*SafetyMessage);
		if( !pMessage )
			continue;

		string Desc="",sCommand="";
		Command *pCommand=NULL;
		Event_Router *pEvent_Router=NULL;

		if( pMessage->m_dwMessage_Type==MESSAGETYPE_COMMAND )
		{
			pCommand = m_mapCommand_Find(pMessage->m_dwID);
			if( pCommand )
			{
				sCommand = pCommand->m_sDescription;
				Desc = "Command:\x1b[35;1m" + sCommand + "\x1b[0m";
			}
		}
		else if( pMessage->m_dwMessage_Type==MESSAGETYPE_EVENT )
		{
			pEvent_Router = m_mapEvent_Router_Find(pMessage->m_dwID);
			if( pEvent_Router )
				Desc = "Event:\x1b[32;1m" + pEvent_Router->m_sDescription + "\x1b[0m";
		}

		if( (!pCommand || pCommand->m_bLog) && (!pEvent_Router || pEvent_Router->m_bLog) )
		{
			int LogType = pMessage->m_dwMessage_Type==MESSAGETYPE_EVENT ? LV_EVENT : (pMessage->m_dwMessage_Type==MESSAGETYPE_COMMAND ? LV_ACTION : LV_STATUS);
			if( pMessage->m_dwMessage_Type==MESSAGETYPE_EVENT || pMessage->m_dwMessage_Type==MESSAGETYPE_COMMAND )
			{
				if (pMessage->m_dwPK_Device_To == DEVICEID_LIST)
				{
					string DeviceList;
					string::size_type pos=0;
					while(true)
					{
						string Device = StringUtils::Tokenize(pMessage->m_sPK_Device_List_To,",",pos);
						if( !Device.size() )
							break;
						DeviceData_Router *pDeviceData_Router = m_mapDeviceData_Router_Find( atoi(Device.c_str()) );
						if( pDeviceData_Router )
							DeviceList += pDeviceData_Router->m_sDescription + "(" + Device + "),";
						else
							DeviceList += "**unknown dev: " + Device + ",";
					}

					LoggerWrapper::GetInstance()->Write(LogType, "Received Message from %d (\x1b[36;1m%s / %s\x1b[0m) to %s type %d id  %d %s, retry %s, parameters:",
						pMessage->m_dwPK_Device_From,
						(pDeviceFrom ? pDeviceFrom->m_sDescription.c_str() : "unknown"),
						(pDeviceFrom  && pDeviceFrom->m_pRoom ? pDeviceFrom->m_pRoom->m_sDescription.c_str() : ""),
						DeviceList.c_str(),
						pMessage->m_dwMessage_Type, pMessage->m_dwID,Desc.c_str(),
						pMessage->m_eRetry == MR_None ? "none" : (pMessage->m_eRetry == MR_Retry ? "retry" : "persist"));
				}
				else if (pMessage->m_dwPK_Device_To != DEVICEID_LOGGER)
					LoggerWrapper::GetInstance()->Write(LogType, "Received Message from %d (\x1b[36;1m%s / %s\x1b[0m) to %d (\x1b[36;1m%s / %s\x1b[0m), type %d id %d %s, retry %s, parameters:",
						pMessage->m_dwPK_Device_From,
						(pDeviceFrom ? pDeviceFrom->m_sDescription.c_str() : "unknown"),
						(pDeviceFrom  && pDeviceFrom->m_pRoom ? pDeviceFrom->m_pRoom->m_sDescription.c_str() : ""),
						pMessage->m_dwPK_Device_To,
						(pDeviceTo ? pDeviceTo->m_sDescription.c_str() : "unknown"),
						(pDeviceTo && pDeviceTo->m_pRoom ? pDeviceTo->m_pRoom->m_sDescription.c_str() : ""),
						pMessage->m_dwMessage_Type, pMessage->m_dwID,Desc.c_str(),
						pMessage->m_eRetry == MR_None ? "none" : (pMessage->m_eRetry == MR_Retry ? "retry" : "persist"));
			}
			else if( pMessage->m_dwMessage_Type != MESSAGETYPE_LOG )
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Received Message from %d (\x1b[36;1m%s\x1b[0m) to %d (\x1b[36;1m%s\x1b[0m), type %d id %d, retry %s, parameters:",
					pMessage->m_dwPK_Device_From,
					(pDeviceFrom ? pDeviceFrom->m_sDescription.c_str() : "unknown"),pMessage->m_dwPK_Device_To,
					(pDeviceTo ? pDeviceTo->m_sDescription.c_str() : "unknown"),
					pMessage->m_dwMessage_Type, pMessage->m_dwID,
					pMessage->m_eRetry == MR_None ? "none" : (pMessage->m_eRetry == MR_Retry ? "retry" : "persist"));

			if (pMessage->m_dwPK_Device_To != DEVICEID_LOGGER)
			{
				map<long,string>::iterator i;
				for(i=pMessage->m_mapParameters.begin();i!=pMessage->m_mapParameters.end();++i)
				{
					Desc="";
					if( pMessage->m_dwMessage_Type==MESSAGETYPE_COMMAND )
						Desc = m_mapCommandParmNames[(*i).first];
					else if( pMessage->m_dwMessage_Type==MESSAGETYPE_EVENT )
						Desc = m_mapEventParmNames[(*i).first];

					if( pMessage->m_dwMessage_Type==MESSAGETYPE_COMMAND && 
						((*i).first==COMMANDPARAMETER_PK_Command_Input_CONST ||
						(*i).first==COMMANDPARAMETER_PK_Command_Output_CONST ) )
					{
						Command *pCommand = m_mapCommand_Find(atoi((*i).second.c_str()));
						if( pCommand )
							Desc += "(" + pCommand->m_sDescription + ")";
					}


					LoggerWrapper::GetInstance()->Write(LogType, "  Parameter %d(%s): %s", (*i).first, Desc.c_str(), (*i).second.c_str());
				}
				map<long,unsigned long>::iterator il;
				for(il=pMessage->m_mapData_Lengths.begin();il!=pMessage->m_mapData_Lengths.end();++il)
				{
					Desc="";
					if( pMessage->m_dwMessage_Type==MESSAGETYPE_COMMAND )
						Desc = m_mapCommandParmNames[(*il).first];
					else if( pMessage->m_dwMessage_Type==MESSAGETYPE_EVENT )
						Desc = m_mapEventParmNames[(*il).first];
					if( pMessage->m_dwMessage_Type==MESSAGETYPE_EVENT || pMessage->m_dwMessage_Type==MESSAGETYPE_COMMAND )
						LoggerWrapper::GetInstance()->Write(LogType, "  Data Parm %d(%s): %d bytes", (*il).first, Desc.c_str(), (*il).second);
				}
			}
		}
	}

    int tempid = (*SafetyMessage)->m_dwPK_Device_To;
    if(tempid == m_dwPK_Device)
        tempid = DEVICEID_DCEROUTER;
    switch(tempid)
    {
    case DEVICEID_LOGGER:
    case DEVICEID_DCEROUTER:
        if ((*SafetyMessage)->m_dwMessage_Type == MESSAGETYPE_SYSCOMMAND)
        {
			int iMessageID = (*SafetyMessage)->m_dwID;
            switch(iMessageID)
            {
            case SYSCOMMAND_QUIT:
                m_bQuit=true;
				m_bReload=false;
                break;
#ifdef LL_DEBUG
            case SYSCOMMAND_SHOW_SOCKETS:
                ShowSockets();
                break;
#endif
            case SYSCOMMAND_RELOAD:
			case SYSCOMMAND_RELOAD_FORCED:
				if( (*SafetyMessage)->m_dwID!=SYSCOMMAND_RELOAD_FORCED && !RequestReload((*SafetyMessage)->m_dwPK_Device_From) )
				{
					LoggerWrapper::GetInstance()->Write(LV_WARNING,"Aborting reload per RequestReload");
					return;
				}
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Reloading per id %d", (*SafetyMessage)->m_dwID);
				m_bQuit = false;
				m_bReload = true;
                break;

			case SYSCOMMAND_RETRIEVE_INSTALLATION_ID:
				(*SafetyMessage)->m_bRespondedToMessage=true;
				pSocket->SendMessage(new Message(m_dwPK_Device, (*SafetyMessage)->m_dwPK_Device_From, PRIORITY_NORMAL, MESSAGETYPE_REPLY, 0, 1, 0, StringUtils::itos(m_dwPK_Installation).c_str()));
				break;

            case SYSCOMMAND_SEGFAULT:
				{
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Forcing a crash");
					char *pNULLPointer=NULL;
					strcpy(pNULLPointer,"this always crashes any os");
				}
				break;
            case SYSCOMMAND_DEVICE_UP:
            case SYSCOMMAND_DEVICE_DOWN:
				{
#ifdef DEBUG
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"Device up down %d is %s",(*SafetyMessage)->m_dwPK_Device_From,(*SafetyMessage)->m_dwID==SYSCOMMAND_DEVICE_UP ? "UP" : "DOWN");
#endif
					pair<time_t,time_t> pOldValue = m_mapDeviceUpStatus_Find((*SafetyMessage)->m_dwPK_Device_From);
					pOldValue.second = time(NULL);  // The second is the last communication
					if( (*SafetyMessage)->m_dwID==SYSCOMMAND_DEVICE_DOWN )
						pOldValue.first = 0;  // The first is 0 if the device is down
					else if( pOldValue.first == 0 )
						pOldValue.first = pOldValue.second;  // The first is the time the machine first came up
					PLUTO_SAFETY_LOCK(cm,m_CoreMutex);
					m_mapDeviceUpStatus[(*SafetyMessage)->m_dwPK_Device_From] = pOldValue;
				}
				break;
            case SYSCOMMAND_DEADLOCK:
				{
					for(ServerSocketMap::iterator iDeviceConnection=m_mapServerSocket.begin();iDeviceConnection!=m_mapServerSocket.end();++iDeviceConnection)
					{
			            ServerSocket *pDeviceConnection = (*iDeviceConnection).second;
						PLUTO_SAFETY_LOCK(slConnMutex,pDeviceConnection->m_ConnectionMutex);
						slConnMutex.m_bReleased=true; // So it never gets released
					}
					PLUTO_SAFETY_LOCK(slListener,m_CoreMutex);
					slListener.m_bReleased=true;
                    PLUTO_SAFETY_LOCK(slCore,m_CoreMutex);
					slCore.m_bReleased=true; // So it never gets released
				}
				break;
			case SYSCOMMAND_ROTATE:
				DoLogRotation();
				break;
			case SYSCOMMAND_RELOAD_LOGLEVEL:
				DoLogReload();
				break;
			default:
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "Received unknown SYSCOMMAND %d", iMessageID);
				break;
			}
            return;
        }
        else if( (*SafetyMessage)->m_dwMessage_Type == MESSAGETYPE_LOG && (*SafetyMessage)->m_mapData_Lengths.size() )
        {
            Logger::Entry e;
            e.SerializeRead( (*SafetyMessage)->m_mapData_Lengths[0], (*SafetyMessage)->m_mapData_Parameters[0] );
            LoggerWrapper::GetInstance()->WriteEntry(e);
        }
		else if ( (*SafetyMessage)->m_dwMessage_Type == MESSAGETYPE_DATAPARM_CHANGE )
		{
			map<long, string>::iterator p = (*SafetyMessage)->m_mapParameters.begin();
			if ( p != (*SafetyMessage)->m_mapParameters.end() )
				SetDeviceDataInDB((*SafetyMessage)->m_dwID,p->first,p->second);
		}
		else if ( (*SafetyMessage)->m_dwMessage_Type == MESSAGETYPE_DATAPARM_REQUEST )
		{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"DCERouter MESSAGETYPE_DATAPARM_REQUEST");
#endif
			map<long, string>::iterator p = (*SafetyMessage)->m_mapParameters.begin();
			if ( p != (*SafetyMessage)->m_mapParameters.end() )
			{
				int PK_Device = atoi(p->second.c_str());
				Row_Device_DeviceData *pRow_Device_DeviceData = 
					m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(PK_Device,(*SafetyMessage)->m_dwID);
				if( pRow_Device_DeviceData )
				{
					pRow_Device_DeviceData->Reload();
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"DCERouter MESSAGETYPE_DATAPARM_REQUEST device %d data %d=%s",
				pRow_Device_DeviceData->FK_Device_get(),pRow_Device_DeviceData->FK_DeviceData_get(),
				pRow_Device_DeviceData->IK_DeviceData_get().c_str());
#endif
					pSocket->SendMessage(new Message(m_dwPK_Device, (*SafetyMessage)->m_dwPK_Device_From, PRIORITY_NORMAL, MESSAGETYPE_REPLY, 0, 1, (*SafetyMessage)->m_dwID, pRow_Device_DeviceData->IK_DeviceData_get().c_str()));
					(*SafetyMessage)->m_bRespondedToMessage=true;
				}
			}
			if( !(*SafetyMessage)->m_bRespondedToMessage )
			{
				pSocket->SendMessage(new Message(m_dwPK_Device, (*SafetyMessage)->m_dwPK_Device_From, PRIORITY_NORMAL, MESSAGETYPE_REPLY, 0, 1, (*SafetyMessage)->m_dwID, "ERR Parameter not found"));
				(*SafetyMessage)->m_bRespondedToMessage=true;
			}
		}
        else if ((*SafetyMessage)->m_dwMessage_Type == MESSAGETYPE_START_PING)
        {
			int PK_Device = (*SafetyMessage)->m_dwID;
			if( PK_Device>0 )
			{
				ServerSocket *pServerSocket;
				GET_SERVER_SOCKET(gs, pServerSocket, PK_Device );

				if( pServerSocket && !pServerSocket->m_bUsePingToKeepAlive )
					pServerSocket->StartPingLoop();
				Message *pMessage = new Message(m_dwPK_Device,PK_Device,PRIORITY_NORMAL,MESSAGETYPE_START_PING,0,0);
	            AddMessageToQueue(pMessage);
			}
		}
        else if ((*SafetyMessage)->m_dwMessage_Type == MESSAGETYPE_STOP_PING)
        {
			int PK_Device = (*SafetyMessage)->m_dwID;
			if( PK_Device>0 )
			{
				ServerSocket *pServerSocket;
				GET_SERVER_SOCKET(gs, pServerSocket, PK_Device );
				if( pServerSocket )
					pServerSocket->m_bUsePingToKeepAlive = false;
				Message *pMessage = new Message(m_dwPK_Device,PK_Device,PRIORITY_NORMAL,MESSAGETYPE_STOP_PING,0,0);
	            AddMessageToQueue(pMessage);
			}
		}

		break;
    default:
        if( (*SafetyMessage)->m_dwMessage_Type==MESSAGETYPE_COMMAND && (*SafetyMessage)->m_eExpectedResponse==ER_None )
        {
            // Do this on a separate thread.  Sometimes devices will send a message requesting a response, holding a blocked mutex
            // while the device they're waiting for a response from sends them a command.  It's safer to send messages on separate
            // threads whenever possible
            AddMessageToQueue(SafetyMessage.Detach());
            return;
        }

        RealSendMessage(pSocket,&SafetyMessage);

        break;
    }

    ErrorResponse(pSocket,(*SafetyMessage));
}

bool Router::ReceivedString(Socket *pSocket, string Line, int nTimeout/* = -1*/)
{
    ServerSocket *pServerSocket = /*dynamic_cast<*/(ServerSocket *)/*>*/(pSocket);
    if (Line=="READY")
	{
		if( m_bIsLoading )
			pServerSocket->SendString("NO");  // We haven't finished loading the plugins
		else
			pServerSocket->SendString("YES");  // We haven't finished loading the plugins
		return true;
	}
    if (Line=="CONFIG")
    {
		if( pServerSocket->m_dwPK_Device>m_dwPK_Device_Largest )
		{
			pServerSocket->SendString("CONFIG 0");
			return true;
		}
        class DeviceData_Router *pDevice = m_mapDeviceData_Router_Find(pServerSocket->m_dwPK_Device);
        if (!pDevice)
        {
            LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Device ID %d is requesting its configuration, but it does not exist.", pServerSocket->m_dwPK_Device);
            pSocket->SendString(string("ERROR"));
        }
        else
        {
            if( m_bIsLoading && (pDevice->m_dwPK_Device_ControlledVia != m_dwPK_Device || !pDevice->m_bImplementsDCE) )
            {
                pServerSocket->SendString("WAIT");  // We haven't finished loading the plugins
            }
            else
            {
                pServerSocket->SendString(string("CONFIG ")+StringUtils::itos((int)pDevice->m_iConfigSize));
                pServerSocket->SendData(pDevice->m_iConfigSize, pDevice->m_pMySerializedData);
            }
        }
        return true;
    }
    else if( Line=="DEVICELIST" )
    {
        pServerSocket->SendString(string("DEVICELIST ")+StringUtils::itos(m_dwIDeviceStructure_Size));
        pServerSocket->SendData(m_dwIDeviceStructure_Size, m_pDeviceStructure);
        return true;
    }
    else if (Line=="CORRUPT SOCKET")
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Upon connection, the socket received an invalid response (socket %d).  Terminating.", pSocket->m_Socket);
        closesocket(pSocket->m_Socket);
	return false;
    }
    else if( Line=="CONFIG_TIMESTAMP" )
    {
        string Result = "0";
        struct stat filestat;
        ::stat((m_sBasePath+"DeviceConfig.txt").c_str(), &filestat);
        Result = StringUtils::itos((int) filestat.st_mtime);
        pSocket->SendString(Result);
    }
    else if( Line.substr(0,10)=="SET_STATUS" )
	{
		string::size_type pos = 11;
		int PK_Device = atoi(StringUtils::Tokenize(Line," ",pos).c_str());
		DeviceData_Router *pDevice = m_mapDeviceData_Router_Find(PK_Device);
		if( !pDevice )
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Invalid Device in %s",Line.c_str());
		else if( pos!=Line.length() && pos!=string::npos )
			pDevice->m_sStatus_set( Line.substr(pos) );
		else
			pDevice->m_sStatus_set( "" );
        return true;
	}
    else if( Line.substr(0,9)=="SET_STATE" )
	{
		string::size_type pos = 10;
		int PK_Device = atoi(StringUtils::Tokenize(Line," ",pos).c_str());
		DeviceData_Router *pDevice = m_mapDeviceData_Router_Find(PK_Device);
		if( !pDevice )
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Invalid Device in %s",Line.c_str());
		else if( pos!=Line.length() && pos!=string::npos )
			pDevice->m_sState_set( Line.substr(pos) );
		else
			pDevice->m_sState_set( "" );
        return true;
	}
    else if( Line.substr(0,9)=="GET_STATE" )
	{
		string::size_type pos = 10;
		int PK_Device = atoi(StringUtils::Tokenize(Line," ",pos).c_str());
		DeviceData_Router *pDevice = m_mapDeviceData_Router_Find(PK_Device);
		if( pDevice )
			pSocket->SendString( pDevice->m_sState_get() );
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Invalid get_State %s",Line.c_str());
			pSocket->SendString( "BAD DEVICE" );
		}
        return true;
	}
    else if( Line.substr(0,10)=="GET_STATUS" )
	{
		string::size_type pos = 11;
		int PK_Device = atoi(StringUtils::Tokenize(Line," ",pos).c_str());
		DeviceData_Router *pDevice = m_mapDeviceData_Router_Find(PK_Device);
		if( pDevice )
			pSocket->SendString( pDevice->m_sStatus_get() );
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Invalid get_State %s",Line.c_str());
			pSocket->SendString( "BAD DEVICE" );
		}
        return true;
	}
    else if( Line.substr(0,6)=="GET_IP" )
	{
		string::size_type pos = 7;
		string sSql = "SELECT IPaddress FROM Device WHERE PK_Device=" + StringUtils::Tokenize(Line," ",pos);

		PlutoSqlResult result_set;
		DB_ROW row;
		if( (result_set.r=db_wrapper_query_result(sSql)) && (row = db_wrapper_fetch_row(result_set.r))!=NULL && row[0] )
			pSocket->SendString( row[0] );
		else
			pSocket->SendString( "BAD DEVICE" );
        return true;
	}
    else if( Line.substr(0,6)=="RELOAD" )
	{
		if( RequestReload(0) )
		{
		    pSocket->SendString("OK");
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Reloading per line %s", Line.c_str());
			m_bReload=true;
		}
		else
		    pSocket->SendString("PLUGINS DENIED RELOAD REQUEST");
		return true;
	}
    else if( Line.substr(0,20)=="DEVICES BY TEMPLATE " || Line.substr(0,20)=="DEVICES BY CATEGORY ")
	{
		string sResponse;

		string sSQL;
		if( Line.substr(0,20)=="DEVICES BY TEMPLATE " )
			sSQL = "SELECT PK_Device,Device.Description,Room.Description as Room,IPAddress FROM Device LEFT JOIN Room on FK_Room=PK_Room WHERE FK_DeviceTemplate=" + Line.substr(19);
		else
			sSQL = "SELECT PK_Device,Device.Description,Room.Description as Room,IPAddress FROM Device "
				"JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate LEFT JOIN Room on FK_Room=PK_Room "
				"LEFT JOIN DeviceCategory ON DeviceTemplate.FK_DeviceCategory=DeviceCategory.PK_DeviceCategory "
				"LEFT JOIN DeviceCategory As Parent ON DeviceCategory.FK_DeviceCategory_Parent=Parent.PK_DeviceCategory "
				"WHERE Device.FK_Installation=" + StringUtils::itos(m_dwPK_Installation) + " AND (FK_DeviceCategory=" + Line.substr(19) + " OR Parent.PK_DeviceCategory=" + Line.substr(19) + 
				" OR Parent.FK_DeviceCategory_Parent=" + Line.substr(19) + ")"; // Check 3 levels deep
		PlutoSqlResult result_set;
		DB_ROW row;
		if( (result_set.r=db_wrapper_query_result(sSQL)) )
		{
			sResponse = "DEVICE_INFO " + StringUtils::ltos(static_cast<unsigned long>(result_set.r->row_count)) + "\t";
	        PLUTO_SAFETY_LOCK(lm,m_ListenerMutex);
			ServerSocketMap::iterator iDeviceConnection;
			while ((row = db_wrapper_fetch_row(result_set.r)))
			{
				sResponse += string(row[0]) + "\t" + row[1] + "\t" + (row[2] ? row[2] : "") + "\t" + (row[3] ? row[3] : "") + "\t";
				ServerSocket *pServerSocket;
				GET_SERVER_SOCKET(gs, pServerSocket, atoi(row[0]) );
				if( pServerSocket ) 
					sResponse += pServerSocket->m_sIPAddress;
				sResponse += "\t";
			}
		}
	    pSocket->SendString(sResponse);
		return true;
	}
    else if( Line.substr(0,18)=="DEVICE_REGISTERED ")
	{
		int PK_Device = atoi( Line.substr(18).c_str() );
		ServerSocket *pServerSocket;
		GET_SERVER_SOCKET(gs, pServerSocket, PK_Device );

		DeviceData_Router *pDevice = m_mapDeviceData_Router_Find(PK_Device);
		if (pDevice && pDevice->m_pRow_Device->Disabled_get())
		{
			pSocket->SendString("DEVICE_REGISTERED D");
		}
		else if(! pServerSocket || pServerSocket->m_Socket == INVALID_SOCKET)
			pSocket->SendString("DEVICE_REGISTERED N");
		else
		{
			string sResponse;
			{
				PLUTO_SAFETY_LOCK(slConnMutex,pServerSocket->m_ConnectionMutex)
				sResponse = pServerSocket->SendReceiveString("PING",PING_TIMEOUT);
			}
			if( sResponse=="PONG" )
                pSocket->SendString("DEVICE_REGISTERED Y");
			else
			{
				gs.DeletingSocket();
				RemoveAndDeleteSocket(pServerSocket);
                pSocket->SendString("DEVICE_REGISTERED N");
			}
		}
		return true;
	}
	else if(Line.find("PLAIN_TEXT") == 0 || Line.find("BINARY") == 0 || Line.find("XML") == 0)
	{
		ServerSocket *pServerSocket_Command;
		GET_SERVER_SOCKET(gs, pServerSocket_Command, pServerSocket->m_dwPK_Device );

		DataFormat data_format = 
			Line.find("PLAIN_TEXT") == 0 ? 
				dfPlainText :
				Line.find("BINARY") == 0 ? dfBinary : dfXml;

		if( pServerSocket_Command )
			pServerSocket_Command->m_DataFormat = data_format;

		pServerSocket->m_DataFormat = data_format;
		pServerSocket->SendString("OK");
		return true;
	}  
	else if( Line.substr(0,6)=="PARENT" )
	{
		string::size_type pos = 7;
		int PK_Device = atoi(StringUtils::Tokenize(Line," ",pos).c_str());
		Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(PK_Device);
		string sDevice_ControlledVia;
		if( pRow_Device )
			sDevice_ControlledVia = StringUtils::itos( pRow_Device->FK_Device_ControlledVia_get() );
		pSocket->SendString( "PARENT " + sDevice_ControlledVia );
        return true;
	}
	else if( Line.substr(0,13)=="CHILD_DEVICES" && Line.size()>14 )
	{
		string sSql = "SELECT PK_Device,DeviceTemplate.Description,CommandLine FROM Device JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate "
			" WHERE IsEmbedded=0 AND ImplementsDCE=1 AND FK_Device_ControlledVia = " + Line.substr(14);

		string sResponse;
		PlutoSqlResult result_set;
		DB_ROW row;
		if( (result_set.r=db_wrapper_query_result(sSql)) )
		{
			while ((row = db_wrapper_fetch_row(result_set.r)))
			{
				if( row[1]==NULL )
					continue; // shouldn't happen
				string sCommandLine = row[2] ? row[2] : "";
				if( sCommandLine.empty() )
					sCommandLine = FileUtils::ValidCPPName( row[1] );
				sResponse += row[0] + string("\t") + sCommandLine + "\t";
			}
		}
		pSocket->SendString( "CHILD_DEVICES " + sResponse );
	}
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Router: Don't know how to handle %s.", Line.c_str());
    pSocket->SendString("ERROR");
    return false;
}


void Router::OnDisconnected(int DeviceID)
{

	db_wrapper_thread_end();


	m_RunningDevices.erase(DeviceID);
}

void Router::RegisteredEventHandler(ServerSocket *pSocket, int DeviceID)
{
    PLUTO_SAFETY_LOCK(sl,m_CoreMutex);
    DeviceData_Router *pDevice = m_mapDeviceData_Router_Find(DeviceID);
    if( !pDevice )
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING,"Device: %d tried to register event handler but it doesn't exist",DeviceID);
        return;
    }

	pDevice->m_mapSocket_Event[pSocket->m_iSocketCounter] = pSocket;
}

void Router::RegisteredCommandHandler(ServerSocket *pSocket, int DeviceID)
{
    PLUTO_SAFETY_LOCK(sl,m_CoreMutex);

	db_wrapper_thread_init();

    DeviceData_Router *pDevice = m_mapDeviceData_Router_Find(DeviceID);
    if( !pDevice )
        LoggerWrapper::GetInstance()->Write(LV_WARNING,"Device: %d tried to register but it doesn't exist",DeviceID);

    if( pDevice && pDevice->m_bForceReloadOnFirstConnect )
    {
        pDevice->m_bForceReloadOnFirstConnect = false;
        pDevice->m_bIsReady=false;
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Controller %d's first connection since the server rebooted it, reloading.", DeviceID);
        sl.Release();
        ReceivedMessage(NULL, new Message(0, DeviceID, PRIORITY_NORMAL, MESSAGETYPE_SYSCOMMAND, SYSCOMMAND_RELOAD, 0));
    }
	else if( pDevice && pDevice->m_bUsePingToKeepAlive )
	{
		ServerSocket *pServerSocket;
		GET_SERVER_SOCKET(gs, pServerSocket, DeviceID );
		if( pServerSocket )
			pServerSocket->StartPingLoop();
	}

    // This device has registered, so we can always route its messages back to itself
    m_Routing_DeviceToController[DeviceID] = DeviceID;

	CheckForMessageRetries(DeviceID);

	if( pDevice )
	{
		pDevice->m_pSocket_Command=pSocket;
		pDevice->m_bIsReady=true;
	}

	sl.Release();  // Do the database updates without blocking the core mutex; they can take a while

	if( pDevice )
	{
		// Don't use sql2cpp class because we don't want the psc_mod timestamp to change
		string sSQL = "UPDATE Device SET Registered=1,psc_mod=psc_mod WHERE PK_Device=" + StringUtils::itos(DeviceID);
		m_pDatabase_pluto_main->threaded_db_wrapper_query(sSQL);
	}
	else
	{
		// Don't use sql2cpp class because we don't want the psc_mod timestamp to change
		string sSQL = "UPDATE Device SET Registered=-1,psc_mod=psc_mod WHERE PK_Device=" + StringUtils::itos(DeviceID);
		m_pDatabase_pluto_main->threaded_db_wrapper_query(sSQL);
	}

}


void Router::StopAllDevices(bool bReload)
{
    m_bIsLoading=true;

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Starting Reload....");

    m_bClosed = true;
    // TODO - reload config
    // This should have stopped everything that is automatically startable
    // Now, restart everything else (like controllers)
	PLUTO_SAFETY_LOCK(lm,m_ListenerMutex);
    ServerSocketMap::iterator iDC;
    for(iDC = m_mapServerSocket.begin(); iDC!=m_mapServerSocket.end(); ++iDC)
    {
        ServerSocket *pServerSocket = (*iDC).second;
        PLUTO_SAFETY_LOCK(slConnMutex,(pServerSocket->m_ConnectionMutex))
        {
			pServerSocket->SendMessage(new Message(0, (*iDC).first, PRIORITY_URGENT, MESSAGETYPE_SYSCOMMAND, bReload ? SYSCOMMAND_RELOAD : SYSCOMMAND_QUIT, 1, 1, "1"));
        }
    }

    m_bClosed=false;
    m_bReload=false;
}

void Router::DoLogRotation()
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Starting log rotation...");
	LoggerWrapper::GetInstance()->Rotate();
	PLUTO_SAFETY_LOCK(lm,m_ListenerMutex); // I don't know if this is needed, but DoReload uses it...
    ServerSocketMap::iterator iDC;
    for(iDC = m_mapServerSocket.begin(); iDC!=m_mapServerSocket.end(); ++iDC)
    {
        ServerSocket *pServerSocket = (*iDC).second;
        PLUTO_SAFETY_LOCK(slConnMutex,(pServerSocket->m_ConnectionMutex))
        {
            pServerSocket->SendMessage(new Message(0, (*iDC).first, PRIORITY_URGENT, MESSAGETYPE_SYSCOMMAND, SYSCOMMAND_ROTATE, 0));
        }
    }
}

void Router::DoLogReload()
{
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Starting log reload...");
	PLUTO_SAFETY_LOCK(lm,m_ListenerMutex); // I don't know if this is needed, but DoReload uses it...
    ServerSocketMap::iterator iDC;
    for(iDC = m_mapServerSocket.begin(); iDC!=m_mapServerSocket.end(); ++iDC)
    {
        ServerSocket *pServerSocket = (*iDC).second;
        PLUTO_SAFETY_LOCK(slConnMutex,(pServerSocket->m_ConnectionMutex))
        {
            pServerSocket->SendMessage(new Message(0, (*iDC).first, PRIORITY_URGENT, MESSAGETYPE_SYSCOMMAND, SYSCOMMAND_RELOAD_LOGLEVEL, 0));
        }
    }
}

bool Router::Run()
{
	Message::RegisterSerializer(dfXml, &SerializeMessageXML);
	Message::RegisterDeserializer(dfXml, &DeserializeMessageXML);
/*
Message::FromXML(string sXMLData)
>>	MessageXML msg_xml(sXMLData, this);

string Message::ToXML()
>>	MessageXML msg_xml(this);
>>	return msg_xml.GetXML();
*/

    m_bIsLoading=true;
	ReadPersistentMessages();
    StartListening();
	int Timeout=0;
	while( !m_bRunning && Timeout++<10 )
	{
		Sleep(500); // Give the listener socket a chance to start before we load the plugins
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Waiting for listener thread");
	}
	if( !m_bRunning )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Could not start listening %d",(int) m_bFailedToBind);
		exit(m_bFailedToBind ? 0 : 2);  // If we failed to bind, exit with 1 so we're not restarted because there must be another router running
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "DCERouter Version: %s",VERSION);
    RegisterAllPlugins();
    m_bIsLoading=false;
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Plug-ins loaded");

    //TODO keep track of the 'status'..and set it here
//  if(m_pCorpClient)
//  {
//      m_pCorpClient->Send_StartupEvent(CORPSERVER_STATUS_OK);
//      SendPackageVersions();
//  }

	bool bReload=false;
    // The main loop of this app doesn't do anything!
    while(!m_bQuit && m_bRunning && !m_bReload)
    {
        // TODO: Check m_mapServerSocket and see if we've lost any
        // command connections.
        Sleep(1000);
    }

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "DCERouter will exit quit %d running %d reload %d",
		(int) m_bQuit, (int) m_bRunning, (int) m_bReload);

	m_bQuit=true;
	m_bRunning=false;
	RefuseIncomingConnections();

	if (m_bReload || m_bQuit)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Detected m_bReload=true %d %d",(int) m_bQuit,(int) m_bRunning);

		bReload=m_bReload;  // Return true if we want to be restarted
		StopAllDevices(m_bReload);
		Sleep(3000); // Wait 3 seconds for all devices to get the message before dropping the sockets
	}
	else
		bReload=true; // We quit for some other reason, like a system crash

	pthread_cond_broadcast(&m_MessageQueueCond);
	Sleep(1000); // Let the sockets close

	//mark all devices as unregistered
	string sSQL = "UPDATE Device SET Registered = 0, psc_mod = psc_mod";
	m_pDatabase_pluto_main->threaded_db_wrapper_query(sSQL);

	//start watchdog
	pthread_t watchdog_thread;
	g_WatchDogFlag = true;
	pthread_create(&watchdog_thread, NULL,WatchDogRoutine, NULL);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "PlutoServer: All Done!");
    return bReload;
}

void Router::OutputChildren(DeviceData_Impl *pDevice,string &Response)
{
    Response+=StringUtils::itos((int) pDevice->m_vectDeviceData_Impl_Children.size()) + "|";
    for(int i=0;i<(int)pDevice->m_vectDeviceData_Impl_Children.size();++i)
    {
        DeviceData_Impl *pChildDevice = pDevice->m_vectDeviceData_Impl_Children[i];
        Response += StringUtils::itos(pChildDevice->m_dwPK_Device) + "|" + StringUtils::itos(pChildDevice->m_dwPK_DeviceTemplate) + "|";
        OutputChildren(pChildDevice,Response);
    }
}

bool Router::GetParameter(int ToDevice,int ParmType, string &sResult)
{
    PLUTO_SAFETY_LOCK(slock,m_CoreMutex);
    ServerSocket *pSocket=NULL;

    map<int,int>::iterator iRoute = m_Routing_DeviceToController.find(ToDevice);
    if (iRoute!=m_Routing_DeviceToController.end())
		GET_SERVER_SOCKET(gs, pSocket, (*iRoute).second );

    slock.Release();
    if( pSocket )
    {
        PLUTO_SAFETY_LOCK(slConn,pSocket->m_ConnectionMutex);
        if (!pSocket->SendMessage(new Message(0, ToDevice, PRIORITY_NORMAL, MESSAGETYPE_DATAPARM_REQUEST, ParmType, 0)))
        {
            LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Socket %p failure sending dataparm request to device %d",
                pSocket, ToDevice);

            // TODO :  The socket failed, core needs to remove client socket

        }
        if (pSocket->ReceiveString(sResult) && sResult.find("MESSAGE") == 0)
        {
            Message *pMessage = pSocket->ReceiveMessageRaw(sResult);

            if (pMessage)
            {
                sResult = pMessage->m_mapParameters[ParmType];
                delete pMessage;
                return true;
            }
        }
    }
    return false;
}

bool Router::GetParameterWithDefinedMessage(Message *sendMessage, string &sResult)
{
	int messageID;
	PLUTO_SAFETY_LOCK(slock,m_CoreMutex);

    LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "GetParameterWithDefinedMessage %s", StringUtils::itos(sendMessage->m_dwID).c_str());
	messageID = sendMessage->m_dwID;

    map<int,int>::iterator iRoute = m_Routing_DeviceToController.find(sendMessage->m_dwPK_Device_To);
    if (iRoute!=m_Routing_DeviceToController.end())
    {
		ServerSocket *pServerSocket;
		GET_SERVER_SOCKET(gs, pServerSocket, sendMessage->m_dwPK_Device_To );
        if (pServerSocket)
        {
            PLUTO_SAFETY_LOCK(slConn,pServerSocket->m_ConnectionMutex);
            if (!pServerSocket->SendMessage(sendMessage)) //This evidentally overwrite sendMessage, do not use sendMessage after here
            {
                LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Socket %p failure sending dataparm request to device %d", pServerSocket, sendMessage->m_dwPK_Device_To);

                // TODO :  The socket failed, core needs to remove client socket

            }
            if (pServerSocket->ReceiveString(sResult) && sResult.substr(0,7)=="MESSAGE" && sResult.size()>7 )
            {
				Message *pMessage = pServerSocket->ReceiveMessageRaw(sResult);
                if (pMessage)
                {
                    sResult = pMessage->m_mapParameters[messageID];
                    delete pMessage;
                    return true;
                }
            }
        }
    }
    return false;
}


// moved inline in the header
// int Router::FindClosestRelative(int MasterDeviceType, int CurrentDevice)

string Router::GetPublicIP()
{
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"Trying to obtain public IP");
#ifndef WIN32
    char buf[20];
    FILE* f = popen("wget -q -O - http://www.1control.com/cgi-bin/remoteaddress.pl", "r");
    if(f == NULL)
    {
#endif
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Couldn't get the my public IP");
        return "";
#ifndef WIN32
    }
    else
    {
        fgets(buf, sizeof(buf), f);
        fclose(f);
        size_t len = strlen(buf);
        if(buf[len-1] == '\n');
            buf[len-1] = 0;
        LoggerWrapper::GetInstance()->Write(LV_CORPCLIENT, "My public IP is '%s'",buf);
        return string(buf);
    }
#endif
}


void Router::CleanFileName(string &FileName)
{
    string::size_type s;
    while( (s=FileName.find(':'))!=string::npos )
        FileName.replace(s,1,"-");

    while( (s=FileName.find('\''))!=string::npos )
        FileName.replace(s,1,"");

    while( (s=FileName.find('\"'))!=string::npos )
        FileName.replace(s,1,"");
}

void Router::AddMessageToQueue(Message *pMessage)
{
    PLUTO_SAFETY_LOCK(mm,m_MessageQueueMutex);
#ifdef DEBUG
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"AddMessageToQueue(ProcessQueue) adding message from %d to %d type %d id %d to queue size was: %d",
        pMessage->m_dwPK_Device_From,pMessage->m_dwPK_Device_To,pMessage->m_dwMessage_Type,pMessage->m_dwID,(int) m_MessageQueue.size());
#endif
    m_MessageQueue.push_back(pMessage);
	mm.Release();
    pthread_cond_broadcast(&m_MessageQueueCond);
#ifdef DEBUG
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"AddMessageToQueue(ProcessQueue) sent broadcast");
#endif
}

void Router::ProcessQueue()
{
    PLUTO_SAFETY_LOCK(mm,m_MessageQueueMutex);
    while(!m_bReload)
    {
        while(!m_bReload && m_MessageQueue.size()==0)
        {
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_STATUS,"ProcessQueue going to sleep");
#endif
			mm.CondWait();
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_STATUS,"ProcessQueue woke up with size: %d",(int) m_MessageQueue.size());
#endif
        }

		if(m_bReload)
			break;

        // We are holding the mutex
        Message *pMessage = m_MessageQueue.front();
        m_MessageQueue.pop_front();
#ifdef DEBUG
        LoggerWrapper::GetInstance()->Write(LV_STATUS,"ProcessQueue sending message from %d to %d type %d id %d to queue now size: %d",
            pMessage->m_dwPK_Device_From,pMessage->m_dwPK_Device_To,pMessage->m_dwMessage_Type,pMessage->m_dwID,(int) m_MessageQueue.size());
#endif
        mm.Release();

        SafetyMessage pSafetyMessage(pMessage);
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_STATUS,"ProcessQueue Calling realsendmessage from queue");
#endif
        RealSendMessage(NULL,&pSafetyMessage);
        mm.Relock();
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_STATUS,"ProcessQueue finished Calling realsendmessage from queue");
#endif
    }
}

void Router::HandleCommandPipes(Socket *pSocket,SafetyMessage *pSafetyMessage)
{

	// RE: COMMANDPARAMETER_PK_Pipe_CONST - Normally when a device is turned on all the inputs and 
	// outputs are selected automatically.  If this parameter is specified, only the settings along 
	//this pipe will be set.

	// RE: COMMANDPARAMETER_PK_Device_Pipes_CONST - Normally when a device is turned on the corresponding
	// "pipes" are enabled by default. if this parameter is blank.  If this parameter is 0, no pipes will 
	// be enabled.  This can also be a comma seperated list of devices, meaning only the pipes to those devic
    Command *pCommand = m_mapCommand_Find((*(*pSafetyMessage))->m_dwID);
    int PK_Device = (*(*pSafetyMessage))->m_dwPK_Device_To;
    DeviceData_Router *pDeviceData_Router = m_mapDeviceData_Router_Find(PK_Device);
    if( !pCommand || !pDeviceData_Router )
    {
        if( pCommand && pCommand->m_dwPK_Command==COMMAND_Generic_Off_CONST && PK_Device==DEVICEID_NULL )
            m_bQuit=true;
        return;
    }

    if( pCommand->m_dwPK_Command==COMMAND_Generic_Off_CONST )
    {
		int PK_Pipe=0;
        if( (*(*pSafetyMessage))->m_mapParameters.find(COMMANDPARAMETER_PK_Pipe_CONST)!=(*(*pSafetyMessage))->m_mapParameters.end() )
			PK_Pipe = atoi((*(*pSafetyMessage))->m_mapParameters[COMMANDPARAMETER_PK_Pipe_CONST].c_str());

		if( PK_Pipe==-1 )  // Special value means don't propagate
			return;

		int PK_PipeID = 0;
		MapPipe *pMapPipe = NULL;
		if( (*(*pSafetyMessage))->m_mapParameters.find(COMMANDPARAMETER_PipeID_CONST)!=(*(*pSafetyMessage))->m_mapParameters.end() )
		{
			PK_PipeID = atoi((*(*pSafetyMessage))->m_mapParameters[COMMANDPARAMETER_PipeID_CONST].c_str());
			pMapPipe = pDeviceData_Router->m_mapPipe_Temporary_Find(PK_PipeID);
		}
		else
			pMapPipe = &pDeviceData_Router->m_mapPipe_Active;

		if( pMapPipe )
		{
			for(MapPipe::iterator it=pMapPipe->begin();it!=pMapPipe->end();++it)
			{
				Pipe *pPipe = it->second;

				Message *pMessage = new Message( (*(*pSafetyMessage))->m_dwPK_Device_From, pPipe->m_pDevice_To->m_dwPK_Device,
					PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Generic_Off_CONST,0);
				if( PK_Pipe )
					pMessage->m_mapParameters[COMMANDPARAMETER_PK_Pipe_CONST] = (*(*pSafetyMessage))->m_mapParameters[COMMANDPARAMETER_PK_Pipe_CONST];
				if( PK_PipeID )
					pMessage->m_mapParameters[COMMANDPARAMETER_PipeID_CONST] = (*(*pSafetyMessage))->m_mapParameters[COMMANDPARAMETER_PipeID_CONST];
				if( (*(*pSafetyMessage))->m_mapParameters.find(COMMANDPARAMETER_PK_Device_Pipes_CONST)!=(*(*pSafetyMessage))->m_mapParameters.end() )
					pMessage->m_mapParameters[COMMANDPARAMETER_PK_Device_Pipes_CONST] = (*(*pSafetyMessage))->m_mapParameters[COMMANDPARAMETER_PK_Device_Pipes_CONST];
				ReceivedMessage(NULL,pMessage);

				if( PK_PipeID )
					delete pPipe;
			}
			if( PK_PipeID )
			{
				pDeviceData_Router->m_mapPipe_Temporary.erase(PK_PipeID);
				delete pMapPipe;
			}
			else
				pDeviceData_Router->m_mapPipe_Active.clear();
		}
    }
    else if( pCommand->m_dwPK_Command==COMMAND_Generic_On_CONST )
    {
		int PK_Pipe=0;
        if( (*(*pSafetyMessage))->m_mapParameters.find(COMMANDPARAMETER_PK_Pipe_CONST)!=(*(*pSafetyMessage))->m_mapParameters.end() )
			PK_Pipe = atoi((*(*pSafetyMessage))->m_mapParameters[COMMANDPARAMETER_PK_Pipe_CONST].c_str());

		if( PK_Pipe==-1 )  // Special value means don't propagate
			return;

        string sPipesDevices;
        if( (*(*pSafetyMessage))->m_mapParameters.find(COMMANDPARAMETER_PK_Device_Pipes_CONST)!=(*(*pSafetyMessage))->m_mapParameters.end() )
            sPipesDevices = "," + (*(*pSafetyMessage))->m_mapParameters[COMMANDPARAMETER_PK_Device_Pipes_CONST] + ",";
		Pipe *pPipe_Prior=NULL;

		int PK_PipeID = 0;
		MapPipe *pMapPipe = NULL;
		if( (*(*pSafetyMessage))->m_mapParameters.find(COMMANDPARAMETER_PipeID_CONST)!=(*(*pSafetyMessage))->m_mapParameters.end() )
		{
			PK_PipeID = atoi((*(*pSafetyMessage))->m_mapParameters[COMMANDPARAMETER_PipeID_CONST].c_str());
			pMapPipe = pDeviceData_Router->m_mapPipe_Temporary_Find(PK_PipeID);
		}
		else
			pMapPipe = &pDeviceData_Router->m_mapPipe_Available;

		if( pMapPipe )
		{	
			for(MapPipe::iterator it=pMapPipe->begin();it!=pMapPipe->end();++it)
			{
				Pipe *pPipe = (*it).second;

	#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Walking the command pipe: Command input: %d Command_Output: %d Device From: %d, FK_Pipe: %d",
					pPipe->m_pCommand_Input ? pPipe->m_pCommand_Input->m_dwPK_Command : 0,
					pPipe->m_pCommand_Output ? pPipe->m_pCommand_Output->m_dwPK_Command : 0,
					pPipe->m_pDevice_From->m_dwPK_Device);
	#endif
				if( PK_Pipe && PK_Pipe!=pPipe->m_PK_Pipe )
					continue;

				// Don't activate a pipe if we're using an alternate pipe that was already active
				if( PK_PipeID==0 && (sPipesDevices.length()<3 || sPipesDevices.find("," + StringUtils::itos((*it).first) + ",")!=string::npos) ) // It may be 2 characters: ,,
					pDeviceData_Router->m_mapPipe_Active[pPipe->m_PK_Pipe]=pPipe;

				if( pPipe_Prior && pPipe_Prior->m_pDevice_To->m_dwPK_Device==pPipe->m_pDevice_To->m_dwPK_Device 
						&& pPipe->m_pCommand_Input==pPipe_Prior->m_pCommand_Input
						&& pPipe->m_pCommand_Output==pPipe_Prior->m_pCommand_Output )
					continue;  // Don't bother if there's another pipe going to the same device--we will have already done this
				pPipe_Prior=pPipe;

				if( pPipe->m_bDontSendOn==false )
				{
					// Forward the on command up the pipe
					Message *pMessage = new Message( (*(*pSafetyMessage))->m_dwPK_Device_From, pPipe->m_pDevice_To->m_dwPK_Device,
						PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Generic_On_CONST,0);
					if( PK_Pipe )
						pMessage->m_mapParameters[COMMANDPARAMETER_PK_Pipe_CONST] = (*(*pSafetyMessage))->m_mapParameters[COMMANDPARAMETER_PK_Pipe_CONST];
					if( PK_PipeID )
						pMessage->m_mapParameters[COMMANDPARAMETER_PipeID_CONST] = (*(*pSafetyMessage))->m_mapParameters[COMMANDPARAMETER_PipeID_CONST];
					if( sPipesDevices.size() )
						pMessage->m_mapParameters[COMMANDPARAMETER_PK_Device_Pipes_CONST] = (*(*pSafetyMessage))->m_mapParameters[COMMANDPARAMETER_PK_Device_Pipes_CONST];

					ReceivedMessage(NULL,pMessage);
				}

				// The pipe's auto selection of inputs could be disabled if we're using some other output device, like a capture card
				if( pPipe->m_bDontSendInputs==false && pPipe->m_pCommand_Input )
				{
					Message *pMessage = new Message( (*(*pSafetyMessage))->m_dwPK_Device_From, pPipe->m_pDevice_To->m_dwPK_Device,
						PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Input_Select_CONST,1,
						COMMANDPARAMETER_PK_Command_Input_CONST,StringUtils::itos(pPipe->m_pCommand_Input->m_dwPK_Command).c_str());
					if( PK_PipeID )
						pMessage->m_mapParameters[COMMANDPARAMETER_PipeID_CONST] = (*(*pSafetyMessage))->m_mapParameters[COMMANDPARAMETER_PipeID_CONST];
					ReceivedMessage(NULL,pMessage);
				}
				if( pPipe->m_pCommand_Output )
				{
					Message *pMessage = new Message( (*(*pSafetyMessage))->m_dwPK_Device_From, pPipe->m_pDevice_To->m_dwPK_Device,
						PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Output_Select_CONST,1,
						COMMANDPARAMETER_PK_Command_Output_CONST,StringUtils::itos(pPipe->m_pCommand_Output->m_dwPK_Command).c_str());
					if( PK_PipeID )
						pMessage->m_mapParameters[COMMANDPARAMETER_PipeID_CONST] = (*(*pSafetyMessage))->m_mapParameters[COMMANDPARAMETER_PipeID_CONST];
					ReceivedMessage(NULL,pMessage);
				}
			}
		}
    }
    else if( pCommand->m_listPipe.size() )
    {
        for(list<int>::iterator it=pCommand->m_listPipe.begin();it!=pCommand->m_listPipe.end();++it)
        {
			int PK_PipeID = 0;
			MapPipe *pMapPipe = NULL;
			if( (*(*pSafetyMessage))->m_mapParameters.find(COMMANDPARAMETER_PipeID_CONST)!=(*(*pSafetyMessage))->m_mapParameters.end() )
			{
				PK_PipeID = atoi((*(*pSafetyMessage))->m_mapParameters[COMMANDPARAMETER_PipeID_CONST].c_str());
				pMapPipe = pDeviceData_Router->m_mapPipe_Temporary_Find(PK_PipeID);
			}
			else
				pMapPipe = &pDeviceData_Router->m_mapPipe_Active;

			if( pMapPipe )
			{	
				MapPipe::iterator itMP = pMapPipe->find(*it);
				if( itMP!=pMapPipe->end() )
				{
					Pipe *pPipe=itMP->second;
					(*(*pSafetyMessage))->m_dwPK_Device_To = pPipe->m_pDevice_To->m_dwPK_Device;
	#ifdef DEBUG
					DeviceData_Router *pDevice = m_mapDeviceData_Router_Find((*(*pSafetyMessage))->m_dwPK_Device_To);
					LoggerWrapper::GetInstance()->Write(LV_ACTION,"Forwarding %d Command:\x1b[35;1m%s\x1b[0m up pipe to %d (\x1b[36;1m%s\x1b[0m)",
						pCommand->m_dwPK_Command,pCommand->m_sDescription.c_str(),
						(*(*pSafetyMessage))->m_dwPK_Device_To,pDevice!=NULL ? pDevice->m_sDescription.c_str() : "*unknown*");
	#endif
					RealSendMessage(pSocket,pSafetyMessage);
				}
				(*(*pSafetyMessage))->m_dwPK_Device_To = PK_Device;
			}
        }
    }
}

void Router::RealSendMessage(Socket *pSocket,SafetyMessage *pSafetyMessage)
{
    if ( (*(*pSafetyMessage))->m_dwPK_Device_To==DEVICEID_LIST )
    {
        // Loop back and call ourselves for each message
        string::size_type pos=0;
        string sPK_Device;
        while( (sPK_Device=StringUtils::Tokenize((*(*pSafetyMessage))->m_sPK_Device_List_To,",",pos)).length() )
        {
            (*(*pSafetyMessage))->m_dwPK_Device_To=atoi(sPK_Device.c_str());
			for(size_t s=0;s<(*(*pSafetyMessage))->m_vectExtraMessages.size();++s)
			{
				// This may be NULL because the embedded message may have had a destination, which means it would
				// have been removed from the vect and sent separately
				if( (*(*pSafetyMessage))->m_vectExtraMessages[s] )
					(*(*pSafetyMessage))->m_vectExtraMessages[s]->m_dwPK_Device_To=atoi(sPK_Device.c_str());
			}

            RealSendMessage(pSocket,pSafetyMessage);
        }
        return;
    }

    // We have to do this here after we've parsed the DEVICEID_LIST
    DeviceData_Router *pDeviceTo = m_mapDeviceData_Router_Find((*(*pSafetyMessage))->m_dwPK_Device_To);

    if ( (*(*pSafetyMessage))->m_dwMessage_Type==MESSAGETYPE_COMMAND )
	{
        HandleCommandPipes(pSocket,pSafetyMessage);
		if( pDeviceTo && pDeviceTo->m_bIgnoreOnOff && ((*(*pSafetyMessage))->m_dwID==COMMAND_Generic_On_CONST || (*(*pSafetyMessage))->m_dwID==COMMAND_Generic_Off_CONST) )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Skipping on/off's to device %d", pDeviceTo->m_dwPK_Device);
			return;
		}
	}

	if( pDeviceTo && pDeviceTo->m_pDevice_RouteTo )
	{
        (*(*pSafetyMessage))->m_dwPK_Device_To=pDeviceTo->m_pDevice_RouteTo->m_dwPK_Device;
		for(size_t s=0;s<(*(*pSafetyMessage))->m_vectExtraMessages.size();++s)
		{
			Message *pMessage = (*(*pSafetyMessage))->m_vectExtraMessages[s];
			if( pMessage )
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Setting route to for message %p",pMessage);
				(*(*pSafetyMessage))->m_vectExtraMessages[s]->m_dwPK_Device_To=pDeviceTo->m_pDevice_RouteTo->m_dwPK_Device;
			}
		}
	}
#ifdef DEBUG
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"begin realsendmessage before lock");
#endif
    PLUTO_SAFETY_LOCK(slCore,m_CoreMutex);
    int RouteToDevice = DEVICEID_NULL;

#ifdef DEBUG
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"realsendmessage from %d to %d type %d id %d ",
      (*(*pSafetyMessage))->m_dwPK_Device_From,(*(*pSafetyMessage))->m_dwPK_Device_To,(*(*pSafetyMessage))->m_dwMessage_Type,(*(*pSafetyMessage))->m_dwID);
#endif

    map<int,int>::iterator iRoute;
    /* NOT needed for 2.0  -- a video plug-in will handle this
    if ( (*(*pSafetyMessage))->m_dwMessage_Type == MESSAGETYPE_REQUEST && (*(*pSafetyMessage))->m_dwID == REQUESTTYPE_VIDEO_FRAME)
    {
        iRoute = m_Routing_VideoDeviceToController.find((*(*pSafetyMessage))->m_dwPK_Device_To);
        if (iRoute!=m_Routing_VideoDeviceToController.end())
        {
            RouteToDevice = (*iRoute).second;
        }
    }
    */
    if(RouteToDevice == DEVICEID_NULL)
    {
        iRoute = m_Routing_DeviceToController.find((*(*pSafetyMessage))->m_dwPK_Device_To);
        if (iRoute!=m_Routing_DeviceToController.end())
        {
            RouteToDevice = (*iRoute).second;
        }
    }
    slCore.Release();
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_STATUS,"realsendmessage after core release from %d to %d type %d id %d ",
  (*(*pSafetyMessage))->m_dwPK_Device_From,(*(*pSafetyMessage))->m_dwPK_Device_To,(*(*pSafetyMessage))->m_dwMessage_Type,(*(*pSafetyMessage))->m_dwID);
#endif
    if (RouteToDevice > 0)
    {
		ServerSocket *pServerSocket;
		bool bServerSocket_Failed=false;
		GET_SERVER_SOCKET(gs, pServerSocket, RouteToDevice );

        if (pServerSocket)
        {
            {
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_STATUS,"realsendmessage before device conn mutex from %d to %d type %d id %d ",
  (*(*pSafetyMessage))->m_dwPK_Device_From,(*(*pSafetyMessage))->m_dwPK_Device_To,(*(*pSafetyMessage))->m_dwMessage_Type,(*(*pSafetyMessage))->m_dwID);
#endif

#ifdef DEBUG
                DeviceData_Router *pDest = m_mapDeviceData_Router_Find(pServerSocket->m_dwPK_Device);
                if( !pDest )
                {
                    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find destination device: %d",pServerSocket->m_dwPK_Device);
                }
                LoggerWrapper::GetInstance()->Write(LV_SOCKET, "Ready to send message type %d id %d to %d %s on socket %d(%p) using lock: %p",
                    (*(*pSafetyMessage))->m_dwMessage_Type,(*(*pSafetyMessage))->m_dwID,pServerSocket->m_dwPK_Device,
                    (pDest ? pDest->m_sDescription.c_str() : "*UNKNOWN DEVICE*"),
                    pServerSocket->m_iSocketCounter,pServerSocket,&pServerSocket->m_ConnectionMutex);

                string Message = "Device " + StringUtils::itos(pServerSocket->m_dwPK_Device) + " " + (pDest ? pDest->m_sDescription : "*UNKNOWN DEVICE*");
                PLUTO_SAFETY_LOCK_MESSAGE(slConnMutex,pServerSocket->m_ConnectionMutex,Message)
#else
                PLUTO_SAFETY_LOCK(slConnMutex,pServerSocket->m_ConnectionMutex)
#endif

#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_STATUS,"realsendmessage after device conn mutex from %d to %d type %d id %d ",
  (*(*pSafetyMessage))->m_dwPK_Device_From,(*(*pSafetyMessage))->m_dwPK_Device_To,(*(*pSafetyMessage))->m_dwMessage_Type,(*(*pSafetyMessage))->m_dwID);
#endif

#ifdef DEBUG
                // Let's see how long it takes to send
                clock_t clk = clock();
#endif
    #ifdef LL_DEBUG
                LoggerWrapper::GetInstance()->Write(LV_SOCKET, "%d Sending....", pDeviceConnection->m_dwPK_Device);
    #endif
                int MessageType = (*(*pSafetyMessage))->m_dwMessage_Type;
                int ID = (*(*pSafetyMessage))->m_dwID;

				// If we're supposed to retry in the event of failure, we have to at least have delivery confirmation set
				if( (*(*pSafetyMessage))->m_eRetry!=MR_None && (*(*pSafetyMessage))->m_eExpectedResponse==ER_None )
					(*(*pSafetyMessage))->m_eExpectedResponse=ER_DeliveryConfirmation;


				/* AB 8/5, changed this since we still use the message later
                bool bResult = pDeviceConnection->SendMessage(pSafetyMessage->Detach()); */
                bool bResult = pServerSocket->SendMessage(pSafetyMessage->m_pMessage,false);

#ifdef DEBUG
                LoggerWrapper::GetInstance()->Write(LV_SOCKET, "Response %d to realsendmessage type %d id %d socket %d using lock: %p  expected response: %d",
                    bResult ? 1 : 0,MessageType,ID,pServerSocket->m_Socket,&pServerSocket->m_ConnectionMutex,(int) (*(*pSafetyMessage))->m_eExpectedResponse);
#endif

#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_SOCKET, "Got response: %d to message type %d id %d to %d %s on socket %d using lock: %p",
    (bResult ? 1 : 0),
    (*(*pSafetyMessage))->m_dwMessage_Type,(*(*pSafetyMessage))->m_dwID,pServerSocket->m_dwPK_Device,
    (pDest ? pDest->m_sDescription.c_str() : "*UNKNOWN DEVICE*"),
    pServerSocket->m_Socket,&pServerSocket->m_ConnectionMutex);
#endif
                if (!bResult)
                {
                    LoggerWrapper::GetInstance()->Write(LV_WARNING, "Socket %p failure sending message to device %d", pServerSocket,pServerSocket->m_dwPK_Device);
					HandleMessageFailure(pSafetyMessage);
					pServerSocket->Close();
                    bServerSocket_Failed = true;;
                }
                else
                {
#ifdef DEBUG
                    clock_t clk2 = clock();
                    if( clk2-clk>(CLOCKS_PER_SEC*4) )
                        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Took %d secs (%d ticks) to send message to %d",(int) ((clk2-clk)/CLOCKS_PER_SEC),(int) (clk2-clk),pServerSocket->m_dwPK_Device);
#endif

                    if( (*(*pSafetyMessage))->m_eExpectedResponse==ER_None )
                    {
                        LoggerWrapper::GetInstance()->Write(LV_SOCKET,"No response expected.  returning");
                        return;
                    }
                    string sResponse;
                    if (pServerSocket->ReceiveString(sResponse))
                    {

#ifdef DEBUG
                        if( clock()-clk2>(CLOCKS_PER_SEC*4) )
                            LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Took %d secs (ticks: %d) to receive response from %d",(int) ((clock()-clk2)/CLOCKS_PER_SEC),(int) (clock()-clk2),pServerSocket->m_dwPK_Device);
                        LoggerWrapper::GetInstance()->Write(LV_STATUS, "%d Destination realsendmessage responded with %s.", pServerSocket->m_dwPK_Device, sResponse.c_str());
#endif

                        if (sResponse.substr(0,7)  == "MESSAGE" && sResponse.size()>7 )
                        {
#ifdef DEBUG
                            LoggerWrapper::GetInstance()->Write(LV_STATUS, "1 pSocket=%p", pSocket);
#endif
                            if (!(*(*pSafetyMessage))->m_bRespondedToMessage)
                            {
								DCE::Message *pMessage = pServerSocket->ReceiveMessageRaw(sResponse);
								if( !pMessage )
								{
									LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Sent message, got response but it's not a real message: %s",sResponse.c_str());
								}
								else if ( pSocket && pSocket->SendMessage(pMessage) == false )
								{
									LoggerWrapper::GetInstance()->Write(LV_WARNING, "Failed to forward response message properly" );
								}
								else if( !pSocket )  // This must be past in internally.  Just merge the resulting output values
								{
									map<long,string>::iterator i;
									for(i=pMessage->m_mapParameters.begin();i!=pMessage->m_mapParameters.end();++i)
										(*(*pSafetyMessage))->m_mapParameters[ i->first ] = i->second;
								}
                                (*(*pSafetyMessage))->m_bRespondedToMessage = true;
                            }
                        }
                        else if (sResponse.substr(0,7)=="BYE" && MessageType==MESSAGETYPE_SYSCOMMAND )
                        {
                            // delete the other sockets
                            LoggerWrapper::GetInstance()->Write(LV_STATUS, "Got BYE in response to message.  Dropping socket");
//                          DeviceData_Router *pDevice = m_mapDeviceData_Router_Find(pServerSocket->m_dwPK_Device);
                            // cause the server to remove the socket
// AB 1-4-2004 TODO -- have to fix this.  it crashed???
//                              pFaipFailedSocketledSocket = pDeviceConnection;
//                              FlagForRemoval(pDeviceConnection->m_dwPK_Device);
                        }
                        else
                        {
                            if( ( (*(*pSafetyMessage))->m_eExpectedResponse==ER_DeliveryConfirmation || (*(*pSafetyMessage))->m_eExpectedResponse==ER_ReplyString )
                                && !(*(*pSafetyMessage))->m_bRespondedToMessage && pSocket)
                            {
                                (*(*pSafetyMessage))->m_bRespondedToMessage=true;
                                pSocket->SendString(sResponse);  // Will be okay if it was successful
                            }
                            // If the socket was waiting for a message, we'll just report an error at the end of the loop if a response wasn't already sent
#ifdef DEBUG
                            LoggerWrapper::GetInstance()->Write(LV_STATUS, "3 iMessageType= %d pSocket=%p ",MessageType, pSocket);
#endif
                            // This wasn't a request, so the sender is not concerned with the response.  No need to keep it waiting
                            // around for a reponse. We'll just log it.
    //                      LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Destination responded with %s.", sResponse.c_str());
                        }
                    }
                    else
                    {
                        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Socket %p failure waiting for response to message from device %d type %d id %d",
                            pServerSocket,pServerSocket->m_dwPK_Device,
							(*(*pSafetyMessage))->m_dwMessage_Type,(*(*pSafetyMessage))->m_dwID);
						HandleMessageFailure(pSafetyMessage);
                        bServerSocket_Failed = true;
                    }
                }
            }
#ifdef DEBUG
          LoggerWrapper::GetInstance()->Write(LV_STATUS, "realsendmessage - before failed socket");
#endif

			if (bServerSocket_Failed)
			{
				if( pServerSocket->m_dwPK_Device && m_mapPlugIn.find(pServerSocket->m_dwPK_Device)!=m_mapPlugIn.end() )
				{
					// We've got a big problem -- a plugin has stopped responding so we'll need to reload
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Plugin %d stopped responding",pServerSocket->m_dwPK_Device );
					Reload();
					Quit();
					return;
				}
				gs.DeletingSocket();
				RemoveAndDeleteSocket(pServerSocket);
			}
        }
        else
        {
            LoggerWrapper::GetInstance()->Write(LV_WARNING, "The target device %d (routed to %d) has not registered.",(*(*pSafetyMessage))->m_dwPK_Device_To,RouteToDevice);
			HandleMessageFailure(pSafetyMessage);
            // If this is a request we have to let the sender know so it doesn't wait in vain.
            // also, send responses back to corpserver
        }
    }
    ErrorResponse(pSocket,(*(*pSafetyMessage)));
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_STATUS, "realsendmessage - end");
#endif
}

// Will send an error if somebody's waiting for a response and didn't get it
void Router::ErrorResponse(Socket *pSocket,Message *pMessage)
{
    if( pMessage->m_eExpectedResponse==ER_None || pMessage->m_bRespondedToMessage || !pSocket )
        return;  // No response necessary
    if( pMessage->m_eExpectedResponse==ER_ReplyMessage )
    {
        pMessage->m_bRespondedToMessage=true;
        LoggerWrapper::GetInstance()->Write(LV_WARNING,"Sender: %d sent message type: %d ID: %d and expected a message reply from %d",pMessage->m_dwPK_Device_From,pMessage->m_dwMessage_Type,pMessage->m_dwID,pMessage->m_dwPK_Device_To);
        Message *pMessageOut=new Message(0,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,-1,0);
        pSocket->SendMessage(pMessageOut);
    }
    if( pMessage->m_eExpectedResponse==ER_ReplyString )
    {
        pMessage->m_bRespondedToMessage=true;
        LoggerWrapper::GetInstance()->Write(LV_WARNING,"Sender: %d sent message type: %d ID: %d and expected a reply string from %d",pMessage->m_dwPK_Device_From,pMessage->m_dwMessage_Type,pMessage->m_dwID,pMessage->m_dwPK_Device_To);
        pSocket->SendString("No Response");
    }
    if( pMessage->m_eExpectedResponse==ER_DeliveryConfirmation )
    {
        pMessage->m_bRespondedToMessage=true;
        LoggerWrapper::GetInstance()->Write(LV_WARNING,"Sender: %d sent message type: %d ID: %d and expected a confirmation from %d",pMessage->m_dwPK_Device_From,pMessage->m_dwMessage_Type,pMessage->m_dwID,pMessage->m_dwPK_Device_To);
        pSocket->SendString("Delivery failed");
    }
}


void Router::ParseDevice(int MasterDeviceID, int ParentDeviceID, class DeviceData_Impl *pDevice)
{
//  if( pDevice->m_dwPK_DeviceCategory == DEVICETEMPLATE_Pluto_Core_CONST )
//      m_dwPK_Device = pDevice->m_dwPK_Device;
    if ( !pDevice->WithinCategory( DEVICECATEGORY_Computers_CONST ) && MasterDeviceID == DEVICEID_DCEROUTER )
    {
        MasterDeviceID = pDevice->m_dwPK_Device;
    }

    if (MasterDeviceID == DEVICEID_DCEROUTER)
    {
        LoggerWrapper::GetInstance()->Write(LV_STATUS,"Created the server device: %d %s",pDevice->m_dwPK_Device,pDevice->m_sDescription.c_str());
        m_Routing_DeviceToController[pDevice->m_dwPK_Device] = pDevice->m_dwPK_Device;
    }
    else
    {
		string sTop;
		if( pDevice->m_pDevice_Core )
			sTop = StringUtils::itos(pDevice->m_pDevice_Core->m_dwPK_Device) + " " + pDevice->m_pDevice_Core->m_sDescription;
		else if( pDevice->m_pDevice_MD )
			sTop = StringUtils::itos(pDevice->m_pDevice_MD->m_dwPK_Device) + " " + pDevice->m_pDevice_MD->m_sDescription;

		if( pDevice->m_bImplementsDCE && !pDevice->m_bIsEmbedded )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Created DCE device %d %s (mdl: %d) routed to: %d (%s) disabled %d",pDevice->m_dwPK_Device,
				pDevice->m_sDescription.c_str(),pDevice->m_dwPK_DeviceTemplate,pDevice->m_dwPK_Device,sTop.c_str(),(int) pDevice->m_bDisabled);
			m_Routing_DeviceToController[pDevice->m_dwPK_Device] = pDevice->m_dwPK_Device;
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Created device %d %s (mdl: %d) routed to: %d (%s) disabled %d",pDevice->m_dwPK_Device,
				pDevice->m_sDescription.c_str(),pDevice->m_dwPK_DeviceTemplate,MasterDeviceID,sTop.c_str(),(int) pDevice->m_bDisabled);
			m_Routing_DeviceToController[pDevice->m_dwPK_Device] = MasterDeviceID;
		}
    }

    for(size_t i=0;i<pDevice->m_vectDeviceData_Impl_Children.size();i++)
    {
        DeviceData_Impl *pChild = pDevice->m_vectDeviceData_Impl_Children[i];
        ParseDevice(MasterDeviceID, pDevice->m_dwPK_Device, pChild);
    }
}

void Router::Configure()
{
    AllDevices allDevices; // We're going to want a serialized copy of all the device information

	string sSQL = "UPDATE Device SET Registered=0,psc_mod=psc_mod";
	m_pDatabase_pluto_main->threaded_db_wrapper_query(sSQL);

	sSQL = "UPDATE Device JOIN Device AS Parent ON Device.FK_Device_ControlledVia=Parent.PK_Device SET Device.FK_Installation=Parent.FK_Installation";
	while( m_pDatabase_pluto_main->threaded_db_wrapper_query(sSQL)>0 );

	// Get the rooms
    vector<Row_Room *> vectRow_Room;
    GetDatabase()->Room_get()->GetRows("1=1",&vectRow_Room);  // All rows
    for(size_t s=0;s<vectRow_Room.size();++s)
    {
        Row_Room *pRow_Room = vectRow_Room[s];
        Room *pRoom = new Room(pRow_Room->PK_Room_get(),pRow_Room->Description_get());
        m_mapRoom[pRoom->m_dwPK_Room] = pRoom;
    }

    // Get the device categories
    vector<Row_DeviceCategory *> vectDeviceCategories;
    GetDatabase()->DeviceCategory_get()->GetRows("1=1",&vectDeviceCategories);  // All rows
    for(size_t s=0;s<vectDeviceCategories.size();++s)
    {
        Row_DeviceCategory *pRow_DeviceCategory = vectDeviceCategories[s];
        DeviceCategory *pDeviceCategory = new DeviceCategory(pRow_DeviceCategory->PK_DeviceCategory_get(),
            pRow_DeviceCategory->FK_DeviceCategory_Parent_get(),pRow_DeviceCategory->Description_get());

        m_mapDeviceCategory[pDeviceCategory->m_dwPK_DeviceCategory] = pDeviceCategory;
        allDevices.m_mapDeviceCategory[pDeviceCategory->m_dwPK_DeviceCategory] = pDeviceCategory;  // All devices will want this structure too
    }

    // Map them up to the parents
    Map_DeviceCategory::iterator itCat;
    for(itCat=m_mapDeviceCategory.begin();itCat!=m_mapDeviceCategory.end();++itCat)
    {
        DeviceCategory *pCat = (*itCat).second;
        if( pCat->m_dwPK_DeviceCategory_Parent )
        {
            DeviceCategory *pParent = m_mapDeviceCategory_Find(pCat->m_dwPK_DeviceCategory_Parent);
            pCat->m_pDeviceCategory_Parent = pParent;
        }
    }

	// Be sure nothing is recursive
    for(itCat=m_mapDeviceCategory.begin();itCat!=m_mapDeviceCategory.end();++itCat)
    {
        DeviceCategory *pCat = (*itCat).second;
        DeviceCategory *pCat2 = pCat;
        while( pCat2 && pCat2->m_pDeviceCategory_Parent )
		{
			if( pCat2->m_pDeviceCategory_Parent==pCat )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Category %d recurses",pCat->m_dwPK_DeviceCategory);
				pCat->m_pDeviceCategory_Parent = NULL;
				pCat->m_dwPK_DeviceCategory_Parent = 0;
		        Row_DeviceCategory *pRow_DeviceCategory = 
					m_pDatabase_pluto_main->DeviceCategory_get()->GetRow(pCat->m_dwPK_DeviceCategory);
				pRow_DeviceCategory->Table_DeviceCategory_get()->Commit();
			}
			pCat2 = pCat2->m_pDeviceCategory_Parent;
		}
    }

    // Build some static arrays
    vector<Row_Command *> vectRow_Command;
    GetDatabase()->Command_get()->GetRows("1=1",&vectRow_Command);  // All rows
    for(size_t s=0;s<vectRow_Command.size();++s)
    {
        Row_Command *pRow_Command = vectRow_Command[s];
		Row_CommandCategory *pRow_CommandCategory = pRow_Command->FK_CommandCategory_getrow();
		if( !pRow_CommandCategory )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Command %d has no category",pRow_Command->PK_Command_get());
			continue;
		}
        Command *pCommand = new Command(pRow_Command->PK_Command_get(),pRow_Command->Description_get(),
			pRow_CommandCategory->PK_CommandCategory_get(),pRow_CommandCategory->Description_get(),
			pRow_Command->Log_get()==1);
        vector<Row_Command_Pipe *> vectRow_Command_Pipe;
        pRow_Command->Command_Pipe_FK_Command_getrows(&vectRow_Command_Pipe);
        for(size_t sp=0;sp<vectRow_Command_Pipe.size();++sp)
        {
            Row_Command_Pipe *pRow_Command_Pipe = vectRow_Command_Pipe[sp];
            pCommand->m_listPipe.push_back( pRow_Command_Pipe->FK_Pipe_get() );
        }
        m_mapCommand[pRow_Command->PK_Command_get()]=pCommand;
    }

    // Build some static arrays
    vector<Row_Event *> vectRow_Event;
    GetDatabase()->Event_get()->GetRows("1=1",&vectRow_Event);  // All rows
    for(size_t s=0;s<vectRow_Event.size();++s)
    {
        Row_Event *pRow_Event = vectRow_Event[s];
        Event_Router *pEvent_Router = new Event_Router(pRow_Event->PK_Event_get(),pRow_Event->Description_get(),pRow_Event->Log_get()==1);
        m_mapEvent_Router[pRow_Event->PK_Event_get()]=pEvent_Router;
    }

	vector<Row_Device *> vectDevices;
    GetDatabase()->Device_get()->GetRows(
        string(DEVICE_FK_INSTALLATION_FIELD) + "=" + StringUtils::itos(m_dwPK_Installation),&vectDevices);

	for(vector<Row_Device *>::iterator it=vectDevices.begin();it!=vectDevices.end();++it)
    {
        Row_Device *pRow_Device = *it;
		if( pRow_Device->PK_Device_get()>=m_dwPK_Device_Largest )
			m_dwPK_Device_Largest=pRow_Device->PK_Device_get();

		if(NULL == pRow_Device->FK_DeviceTemplate_getrow())
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Got a device %d with wrong device template %d", 
				pRow_Device->PK_Device_get(), pRow_Device->FK_DeviceTemplate_get());
			continue;
		}

        string CommandLine=pRow_Device->FK_DeviceTemplate_getrow()->CommandLine_get();
        if( CommandLine.size()==0 && pRow_Device->FK_DeviceTemplate_getrow()->ImplementsDCE_get() )
            CommandLine = FileUtils::ValidCPPName(pRow_Device->FK_DeviceTemplate_getrow()->Description_get());

        DeviceData_Router *pDevice = new DeviceData_Router(pRow_Device,m_mapRoom_Find(pRow_Device->FK_Room_get()),CommandLine);

		if( pRow_Device->PingTest_get() )
			pDevice->m_bUsePingToKeepAlive=true;

		m_mapDeviceData_Router[pDevice->m_dwPK_Device]=pDevice;

		Room *pRoom = m_mapRoom_Find(pDevice->m_dwPK_Room);
		if( pRoom )
			pRoom->m_listDevices.push_back(pDevice);

        ListDeviceData_Router *pListDeviceData_Router = m_mapDeviceByTemplate_Find(pDevice->m_dwPK_DeviceTemplate);
        if( !pListDeviceData_Router )
        {
            pListDeviceData_Router = new ListDeviceData_Router();
            m_mapDeviceByTemplate[pDevice->m_dwPK_DeviceTemplate] = pListDeviceData_Router;
        }
        pListDeviceData_Router->push_back(pDevice);

        pDevice->m_pDeviceCategory=m_mapDeviceCategory_Find(pDevice->m_dwPK_DeviceCategory);

		DeviceCategory *pDeviceCategory = pDevice->m_pDeviceCategory;
		while( pDeviceCategory )
		{
			pListDeviceData_Router = m_mapDeviceByCategory_Find(pDeviceCategory->m_dwPK_DeviceCategory);
			if( !pListDeviceData_Router )
			{
				pListDeviceData_Router = new ListDeviceData_Router();
				m_mapDeviceByCategory[pDeviceCategory->m_dwPK_DeviceCategory] = pListDeviceData_Router;
			}
			pListDeviceData_Router->push_back(pDevice);
			pDeviceCategory = pDeviceCategory->m_pDeviceCategory_Parent;
		}

        // We also need a minimal version from the topmost base class that we'll serialize in alldevices and send to each device that registers so they have a complete device tree
        DeviceData_Base *pDevice_Base = new DeviceData_Base(
            pRow_Device->PK_Device_get(),pRow_Device->FK_Installation_get(),pRow_Device->FK_DeviceTemplate_get(),pRow_Device->FK_Device_ControlledVia_get(),
            pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get(),pRow_Device->FK_Room_get(),pRow_Device->FK_DeviceTemplate_getrow()->ImplementsDCE_get()==1,pRow_Device->FK_DeviceTemplate_getrow()->IsEmbedded_get()==1,
            pRow_Device->FK_DeviceTemplate_getrow()->CommandLine_get(),pRow_Device->FK_DeviceTemplate_getrow()->IsPlugIn_get()==1,pRow_Device->Description_get(), pRow_Device->IPaddress_get(),pRow_Device->MACaddress_get(),
			pRow_Device->FK_DeviceTemplate_getrow()->InheritsMacFromPC_get()==1, pRow_Device->Disabled_get() == 1);
        allDevices.m_mapDeviceData_Base[pDevice_Base->m_dwPK_Device]=pDevice_Base;

        // Get a list of all the commands each device supports
        vector<Row_DeviceTemplate_DeviceCommandGroup *> vectRow_DeviceTemplate_DeviceCommandGroup;
        pRow_Device->FK_DeviceTemplate_getrow()->DeviceTemplate_DeviceCommandGroup_FK_DeviceTemplate_getrows(&vectRow_DeviceTemplate_DeviceCommandGroup);
        for(size_t cmds=0;cmds<vectRow_DeviceTemplate_DeviceCommandGroup.size();++cmds)
        {
            Row_DeviceTemplate_DeviceCommandGroup *pRow_DeviceTemplate_DeviceCommandGroup = vectRow_DeviceTemplate_DeviceCommandGroup[cmds];
            // Get all the commands in this device group
            vector<Row_DeviceCommandGroup_Command *> vectRow_DeviceCommandGroup_Command;
            pRow_DeviceTemplate_DeviceCommandGroup->FK_DeviceCommandGroup_getrow()->DeviceCommandGroup_Command_FK_DeviceCommandGroup_getrows(&vectRow_DeviceCommandGroup_Command);
            for(size_t cmd=0;cmd<vectRow_DeviceCommandGroup_Command.size();++cmd)
            {
                Row_DeviceCommandGroup_Command *pRow_DeviceCommandGroup_Command = vectRow_DeviceCommandGroup_Command[cmd];

                pDevice_Base->m_mapCommands[pRow_DeviceCommandGroup_Command->FK_Command_get()] = pRow_DeviceCommandGroup_Command->FK_Command_getrow()->Description_get();
                pDevice->m_mapCommands[pRow_DeviceCommandGroup_Command->FK_Command_get()] = pRow_DeviceCommandGroup_Command->FK_Command_getrow()->Description_get();
            }

        }

        vector<Row_Device_DeviceData *> vectDeviceParms;
        GetDatabase()->Device_DeviceData_get()->GetRows(
            string(DEVICE_DEVICEDATA_FK_DEVICE_FIELD) + "=" + StringUtils::itos(pDevice->m_dwPK_Device),&vectDeviceParms);

        for(size_t s2=0;s2<vectDeviceParms.size();++s2)
        {
            Row_Device_DeviceData *pRow_DeviceParm = vectDeviceParms[s2];
            pDevice->m_mapParameters[ pRow_DeviceParm->FK_DeviceData_get()] = pRow_DeviceParm->IK_DeviceData_get();
        }

		// Fill in any missing data, and fix default values.  First for DeviceTemplate
		vector<Row_DeviceTemplate_DeviceData * > vectRow_DeviceTemplate_DeviceData;
		pRow_Device->FK_DeviceTemplate_getrow()->DeviceTemplate_DeviceData_FK_DeviceTemplate_getrows(&vectRow_DeviceTemplate_DeviceData);
		for(size_t s=0;s<vectRow_DeviceTemplate_DeviceData.size();++s)
		{
			Row_DeviceTemplate_DeviceData *pRow_DeviceTemplate_DeviceData = vectRow_DeviceTemplate_DeviceData[s];
			if( pDevice->m_mapParameters.find( pRow_DeviceTemplate_DeviceData->FK_DeviceData_get() )==pDevice->m_mapParameters.end() )
			{
				// This isn't in the database.  Add it.  It must be new
				Row_Device_DeviceData *pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->AddRow();
				pRow_Device_DeviceData->FK_Device_set( pRow_Device->PK_Device_get() );
				pRow_Device_DeviceData->FK_DeviceData_set( pRow_DeviceTemplate_DeviceData->FK_DeviceData_get() );
				pRow_Device_DeviceData->IK_DeviceData_set( pRow_DeviceTemplate_DeviceData->IK_DeviceData_get() );
				pRow_Device_DeviceData->Table_Device_DeviceData_get()->Commit();
				pDevice->m_mapParameters[pRow_DeviceTemplate_DeviceData->FK_DeviceData_get()] = pRow_DeviceTemplate_DeviceData->IK_DeviceData_get();
			}
			else if( pRow_DeviceTemplate_DeviceData->UseDeviceTemplateDefault_get()==1 && 
				pRow_DeviceTemplate_DeviceData->IK_DeviceData_get()!=pDevice->m_mapParameters[pRow_DeviceTemplate_DeviceData->FK_DeviceData_get()] )
			{
	            Row_Device_DeviceData *pRow_Device_DeviceData = 
					m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(pRow_Device->PK_Device_get(),
					pRow_DeviceTemplate_DeviceData->FK_DeviceData_get());
				pRow_Device_DeviceData->IK_DeviceData_set( pRow_DeviceTemplate_DeviceData->IK_DeviceData_get() );
				pRow_Device_DeviceData->Table_Device_DeviceData_get()->Commit();
				pDevice->m_mapParameters[pRow_DeviceTemplate_DeviceData->FK_DeviceData_get()] = pRow_DeviceTemplate_DeviceData->IK_DeviceData_get();
			}
		}

		// Now for DeviceCategory
		Row_DeviceCategory *pRow_DeviceCategory = pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow();
		while( pRow_DeviceCategory )
		{
			vector<Row_DeviceCategory_DeviceData * > vectRow_DeviceCategory_DeviceData;
			pRow_DeviceCategory->DeviceCategory_DeviceData_FK_DeviceCategory_getrows(&vectRow_DeviceCategory_DeviceData);
			for(size_t s=0;s<vectRow_DeviceCategory_DeviceData.size();++s)
			{
				Row_DeviceCategory_DeviceData *pRow_DeviceCategory_DeviceData = vectRow_DeviceCategory_DeviceData[s];
				if( pDevice->m_mapParameters.find( pRow_DeviceCategory_DeviceData->FK_DeviceData_get() )==pDevice->m_mapParameters.end() )
				{
					// This isn't in the database.  Add it.  It must be new
					Row_Device_DeviceData *pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->AddRow();
					pRow_Device_DeviceData->FK_Device_set( pRow_Device->PK_Device_get() );
					pRow_Device_DeviceData->FK_DeviceData_set( pRow_DeviceCategory_DeviceData->FK_DeviceData_get() );
					pRow_Device_DeviceData->IK_DeviceData_set( pRow_DeviceCategory_DeviceData->IK_DeviceData_get() );
					pRow_Device_DeviceData->Table_Device_DeviceData_get()->Commit();
					pDevice->m_mapParameters[pRow_DeviceCategory_DeviceData->FK_DeviceData_get()] = pRow_DeviceCategory_DeviceData->IK_DeviceData_get();
				}
				else if( pRow_DeviceCategory_DeviceData->UseDeviceCategoryDefault_get()==1 && 
					pRow_DeviceCategory_DeviceData->IK_DeviceData_get()!=pDevice->m_mapParameters[pRow_DeviceCategory_DeviceData->FK_DeviceData_get()] )
				{
					Row_Device_DeviceData *pRow_Device_DeviceData = 
						m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(pRow_Device->PK_Device_get(),
						pRow_DeviceCategory_DeviceData->FK_DeviceData_get());
					pRow_Device_DeviceData->IK_DeviceData_set( pRow_DeviceCategory_DeviceData->IK_DeviceData_get() );
					pRow_Device_DeviceData->Table_Device_DeviceData_get()->Commit();
					pDevice->m_mapParameters[pRow_DeviceCategory_DeviceData->FK_DeviceData_get()] = pRow_DeviceCategory_DeviceData->IK_DeviceData_get();
				}
			}
			pRow_DeviceCategory=pRow_DeviceCategory->FK_DeviceCategory_Parent_getrow();
		}
	}

	for(map<int,class DeviceData_Router *>::iterator it=m_mapDeviceData_Router.begin();it!=m_mapDeviceData_Router.end();++it)
    {
		DeviceData_Router *pDevice = it->second;
		vector<Row_Device_Device_Pipe *> vectRow_Device_Device_Pipe;
		pDevice->m_pRow_Device->Device_Device_Pipe_FK_Device_From_getrows(&vectRow_Device_Device_Pipe);
		for(vector<Row_Device_Device_Pipe *>::iterator itP=vectRow_Device_Device_Pipe.begin();itP!=vectRow_Device_Device_Pipe.end();++itP)
		{
			Row_Device_Device_Pipe *pRow_Device_Device_Pipe = *itP;
			DeviceData_Router *pDevice_To = m_mapDeviceData_Router_Find(pRow_Device_Device_Pipe->FK_Device_To_get());
			if( pDevice_To )
			{
				Command *pCommand_Input = m_mapCommand_Find(pRow_Device_Device_Pipe->FK_Command_Input_get());
				Command *pCommand_Output = m_mapCommand_Find(pRow_Device_Device_Pipe->FK_Command_Output_get());
				pDevice->m_mapPipe_Available[pRow_Device_Device_Pipe->FK_Pipe_get()] = new Pipe(pDevice,pDevice_To,pRow_Device_Device_Pipe->FK_Pipe_get(),pCommand_Input,pCommand_Output);
			}
		}
	}

    // Now match up children to parents
    map<int,class DeviceData_Router *>::iterator itDevice;
    for(itDevice=m_mapDeviceData_Router.begin();itDevice!=m_mapDeviceData_Router.end();++itDevice)
    {
        DeviceData_Router *pDevice = (*itDevice).second;
        DeviceData_Router *pDevice_Parent = m_mapDeviceData_Router_Find(pDevice->m_dwPK_Device_ControlledVia);
        if( pDevice_Parent )
        {
            pDevice->m_pDevice_ControlledVia = pDevice_Parent;
            pDevice_Parent->m_vectDeviceData_Impl_Children.push_back(pDevice);
        }

		DeviceData_Router *pDevice_To_Last = NULL;
		for( MapPipe::iterator it=pDevice->m_mapPipe_Available.begin();
			it!=pDevice->m_mapPipe_Available.end();++it )
		{
			Pipe *pPipe = (*it).second;
			DeviceData_Router *pDevice_To = m_mapDeviceData_Router_Find(pPipe->m_pDevice_To->m_dwPK_Device);
			if( pDevice_To && pDevice_To_Last!=pDevice_To )
				pDevice_To->m_vectDevices_SendingPipes.push_back(pDevice);
			pDevice_To_Last=pDevice_To;  // Don't put the same thing in there twice if both the a/v use the same path
		}
    }

    // Now match up route to's and find any related devices
    for(itDevice=m_mapDeviceData_Router.begin();itDevice!=m_mapDeviceData_Router.end();++itDevice)
    {
        DeviceData_Router *pDevice = (*itDevice).second;
		vector<Row_Device_Device_Related *> vectRow_Device_Device_Related;
		pDevice->m_pRow_Device->Device_Device_Related_FK_Device_getrows(&vectRow_Device_Device_Related);
		for(size_t s=0;s<vectRow_Device_Device_Related.size();++s)
		{
			Row_Device_Device_Related *pRow_Device_Device_Related = vectRow_Device_Device_Related[s];
			DeviceData_Router *pDevice_Related = m_mapDeviceData_Router_Find(pRow_Device_Device_Related->FK_Device_Related_get());
			if( pDevice_Related )
			{
				pDevice->m_mapDeviceRelation[pDevice_Related->m_dwPK_Device] = new DeviceRelation(pDevice_Related,pRow_Device_Device_Related->Value_get());
				pDevice_Related->m_mapDeviceRelation[pDevice->m_dwPK_Device] = new DeviceRelation(pDevice,pRow_Device_Device_Related->Value_get());
			}
		}
        DeviceData_Router *pDevice_RouteTo = m_mapDeviceData_Router_Find(pDevice->m_pRow_Device->FK_Device_RouteTo_get());
        if( pDevice_RouteTo )
            pDevice->m_pDevice_RouteTo = pDevice_RouteTo;
    }

	// Be sure nothing is recursive
    for(itDevice=m_mapDeviceData_Router.begin();itDevice!=m_mapDeviceData_Router.end();++itDevice)
    {
        DeviceData_Router *pDevice = (*itDevice).second;
		vector<int> vect_Device_ControlledVia;
		CheckForRecursiveControlledVia(pDevice,&vect_Device_ControlledVia);
		vector<int> vect_Device_RouteTo;
		CheckForRecursiveRouteTo(pDevice,&vect_Device_RouteTo);
		vector<int> vect_Device_Pipe;
		CheckForRecursivePipes(pDevice,&vect_Device_Pipe);
    }

    // Now match up devices with Core's and Media Directors
    for(itDevice=m_mapDeviceData_Router.begin();itDevice!=m_mapDeviceData_Router.end();++itDevice)
    {
        DeviceData_Router *pDevice = (*itDevice).second;
		pDevice->m_pDevice_Core = pDevice->FindSelfOrParentWithinCategory(DEVICECATEGORY_Core_CONST);
		pDevice->m_pDevice_MD = pDevice->FindSelfOrParentWithinCategory(DEVICECATEGORY_Media_Director_CONST);
        DeviceData_Base *pDevice_AL = allDevices.m_mapDeviceData_Base_Find(pDevice->m_dwPK_Device);

		if( pDevice->m_pDevice_Core )
			pDevice->m_dwPK_Device_Core = pDevice_AL->m_dwPK_Device_Core = pDevice->m_pDevice_Core->m_dwPK_Device;

		if( pDevice->m_pDevice_MD )
			pDevice->m_dwPK_Device_MD = pDevice_AL->m_dwPK_Device_MD = pDevice->m_pDevice_MD->m_dwPK_Device;
    }

	// Get the device groups
    vector<Row_DeviceGroup *> vectDeviceGroup;
    GetDatabase()->DeviceGroup_get()->GetRows(string(DEVICE_FK_INSTALLATION_FIELD) + "=" + StringUtils::itos(m_dwPK_Installation),&vectDeviceGroup);

    for(size_t s=0;s<vectDeviceGroup.size();++s)
    {
        Row_DeviceGroup *pRow_DeviceGroup = vectDeviceGroup[s];
        DeviceGroup *pDeviceGroup = new DeviceGroup(pRow_DeviceGroup->PK_DeviceGroup_get(),pRow_DeviceGroup->Description_get(),pRow_DeviceGroup->Type_get());

        vector<Row_Device_DeviceGroup *> vectRow_Device_DeviceGroup;
        pRow_DeviceGroup->Device_DeviceGroup_FK_DeviceGroup_getrows(&vectRow_Device_DeviceGroup);
        for(size_t s2=0;s2<vectRow_Device_DeviceGroup.size();++s2)
        {
            Row_Device_DeviceGroup *pRow_Device_DeviceGroup = vectRow_Device_DeviceGroup[s2];
            pDeviceGroup->m_vectPK_Device.push_back(pRow_Device_DeviceGroup->FK_Device_get());
			pDeviceGroup->m_vectDeviceData_Base.push_back(m_mapDeviceData_Router_Find(pRow_Device_DeviceGroup->FK_Device_get()));
        }
        allDevices.m_mapDeviceGroup[pDeviceGroup->m_dwPK_DeviceGroup] = pDeviceGroup;
        m_mapDeviceGroup[pDeviceGroup->m_dwPK_DeviceGroup] = pDeviceGroup;
    }

    // Serialize everything
    allDevices.SerializeWrite();
    m_dwIDeviceStructure_Size=(unsigned long) allDevices.CurrentSize();
    m_pDeviceStructure=new char[m_dwIDeviceStructure_Size];
    memcpy(m_pDeviceStructure,allDevices.m_pcDataBlock,m_dwIDeviceStructure_Size);

    for(itDevice=m_mapDeviceData_Router.begin();itDevice!=m_mapDeviceData_Router.end();++itDevice)
    {
        DeviceData_Router *pDevice = (*itDevice).second;

        pDevice->DeviceData_Impl::SerializeWrite();   // Have each device pre-serialize all it's data
        pDevice->m_pMySerializedData=pDevice->m_pcDataBlock;  // When we serialize this device's children or parents, this would get overwritten if we didn't make a copy
        pDevice->m_iConfigSize = (unsigned long) (pDevice->m_pcCurrentPosition-pDevice->m_pcDataBlock);
        pDevice->m_pcDataBlock=NULL; // So the framework doesn't delete this when we serialize another object in the chain
        if( !pDevice->m_pDevice_ControlledVia )
            ParseDevice(DEVICEID_DCEROUTER, DEVICEID_DCEROUTER,pDevice);
    }

	// Build all the command groups
    vector<Row_CommandGroup *> vectRow_CommandGroup;
	m_pRow_Installation = GetDatabase()->Installation_get()->GetRow(m_dwPK_Installation);
	if (m_pRow_Installation == NULL)
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to get data from Installation(PK_Installation=%d).", m_dwPK_Installation);
	// TODO: clean exit if m_pRow_Installation == NULL; next line segfaults if this condition is true
    m_pRow_Installation->CommandGroup_FK_Installation_getrows(&vectRow_CommandGroup);  // All rows
    for(size_t s=0;s<vectRow_CommandGroup.size();++s)
    {
        Row_CommandGroup *pRow_CommandGroup = vectRow_CommandGroup[s];
        CommandGroup *pCommandGroup = new CommandGroup(pRow_CommandGroup->PK_CommandGroup_get(),pRow_CommandGroup->FK_Array_get());
        m_mapCommandGroup[pRow_CommandGroup->PK_CommandGroup_get()]=pCommandGroup;
        vector<Row_CommandGroup_Command *> vectRow_CommandGroup_Command;
        pRow_CommandGroup->CommandGroup_Command_FK_CommandGroup_getrows(&vectRow_CommandGroup_Command);
        for(size_t s2=0;s2<vectRow_CommandGroup_Command.size();++s2)
        {
            Row_CommandGroup_Command *pRow_CommandGroup_Command = vectRow_CommandGroup_Command[s2];
            CommandGroup_Command *pCommandGroup_Command = new CommandGroup_Command(
                m_mapCommand[pRow_CommandGroup_Command->FK_Command_get()],
                m_mapDeviceData_Router_Find(pRow_CommandGroup_Command->FK_Device_get()));

			if( !pCommandGroup_Command->m_pCommand )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CommandGroup %d with invalid command",pRow_CommandGroup->PK_CommandGroup_get());
				delete pCommandGroup_Command;
				continue; // Shouldn't happen
			}
            pCommandGroup->m_vectCommandGroup_Command.push_back(pCommandGroup_Command);
            vector<Row_CommandGroup_Command_CommandParameter *> vectRow_CommandGroup_Command_CommandParameter;
            pRow_CommandGroup_Command->CommandGroup_Command_CommandParameter_FK_CommandGroup_Command_getrows(&vectRow_CommandGroup_Command_CommandParameter);
            for(size_t s3=0;s3<vectRow_CommandGroup_Command_CommandParameter.size();++s3)
            {
                Row_CommandGroup_Command_CommandParameter *pRow_CommandGroup_Command_CommandParameter = vectRow_CommandGroup_Command_CommandParameter[s3];
                pCommandGroup_Command->m_mapParameter[pRow_CommandGroup_Command_CommandParameter->FK_CommandParameter_get()]=
                    pRow_CommandGroup_Command_CommandParameter->IK_CommandParameter_get();

            }

        }

    }

    vector<Row_CommandParameter *> vectRow_CommandParameter;
    GetDatabase()->CommandParameter_get()->GetRows("1=1",&vectRow_CommandParameter);  // All rows
    for(size_t s=0;s<vectRow_CommandParameter.size();++s)
    {
        Row_CommandParameter *pRow_CommandParameter = vectRow_CommandParameter[s];
        m_mapCommandParmNames[pRow_CommandParameter->PK_CommandParameter_get()]=pRow_CommandParameter->Description_get();
    }

    vector<Row_EventParameter *> vectRow_EventParameter;
    GetDatabase()->EventParameter_get()->GetRows("1=1",&vectRow_EventParameter);  // All rows
    for(size_t s=0;s<vectRow_EventParameter.size();++s)
    {
        Row_EventParameter *pRow_EventParameter = vectRow_EventParameter[s];
        m_mapEventParmNames[pRow_EventParameter->PK_EventParameter_get()]=pRow_EventParameter->Description_get();
    }

	m_pBufferForDeviceCategories = allDevices.m_pcDataBlock;
	allDevices.DontDeleteData(); // We use the category and group pointers here.  Just delete the device Data
    //free device data map
    Map_DeviceData_Base::iterator iD;
    for(iD = allDevices.m_mapDeviceData_Base.begin();
            iD != allDevices.m_mapDeviceData_Base.end(); iD++) {
        delete (*iD).second;
    }

}

// tempoarary hack. need to optimize & handle broadcats
// handles some broadcasts ( The Same Computer one )
string Router::GetDevicesByDeviceTemplate(DeviceData_Router *pDeviceData_From,int PK_DeviceTemplate, eBroadcastLevel BroadcastLevel)
{
	if( BroadcastLevel!=BL_AllHouses && BroadcastLevel!=BL_SameHouse && !pDeviceData_From )
		return "";  // If we're going to filter, we need an incoming device

	string Result="";
    map<int,class DeviceData_Router *>::iterator itDevice;
    for(itDevice=m_mapDeviceData_Router.begin();itDevice!=m_mapDeviceData_Router.end();++itDevice)
    {
        DeviceData_Router *pDevice = (*itDevice).second;

        if( !pDevice )
        {
//#pragma message("this shouldn't happen");
            LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"m_mapDeviceData_Router has a NULL assciated with id: %d", (*itDevice).first);
            continue;
        }

        if( pDevice->m_dwPK_DeviceTemplate != PK_DeviceTemplate ||
				(BroadcastLevel==BL_DirectSiblings && pDeviceData_From->m_pDevice_ControlledVia!=pDevice->m_pDevice_ControlledVia) ||
				(BroadcastLevel==BL_SameComputer && (pDeviceData_From->m_pDevice_Core!=pDevice->m_pDevice_Core || pDeviceData_From->m_pDevice_MD!=pDevice->m_pDevice_MD) ) ||
				(BroadcastLevel==BL_SameRoom && pDeviceData_From->m_dwPK_Room!=pDevice->m_dwPK_Room) ||
				(BroadcastLevel==BL_SameHouse && m_dwPK_Installation!=pDevice->m_dwPK_Installation) )
            continue;

        if( Result.length() )
            Result += ",";

        Result += StringUtils::itos(pDevice->m_dwPK_Device);
    }
    return Result;
}

// tempoarary hack.  need to optimize & handle broadcats
string Router::GetDevicesByCategory(DeviceData_Router *pDeviceData_From,int PK_DeviceCategory,eBroadcastLevel BroadcastLevel)
{
	if( BroadcastLevel!=BL_AllHouses && !pDeviceData_From )
		return ""; // If we're going to filter, we need an incoming device
    string Result="";
    map<int,class DeviceData_Router *>::iterator itDevice;
    for(itDevice=m_mapDeviceData_Router.begin();itDevice!=m_mapDeviceData_Router.end();++itDevice)
    {
        DeviceData_Router *pDevice = (*itDevice).second;
if( !pDevice )
{
//#pragma message("this shouldn't happen");
LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"m_mapDeviceData_Router has a NULL in it");
continue;
}
        if( !pDevice->WithinCategory(PK_DeviceCategory) ||
				(BroadcastLevel==BL_DirectSiblings && pDeviceData_From->m_pDevice_ControlledVia!=pDevice->m_pDevice_ControlledVia) ||
				(BroadcastLevel==BL_SameComputer && (pDeviceData_From->m_pDevice_Core!=pDevice->m_pDevice_Core || pDeviceData_From->m_pDevice_MD!=pDevice->m_pDevice_MD) ) ||
				(BroadcastLevel==BL_SameRoom && pDeviceData_From->m_dwPK_Room!=pDevice->m_dwPK_Room) ||
				(BroadcastLevel==BL_SameHouse && pDeviceData_From->m_dwPK_Installation!=pDevice->m_dwPK_Installation) )
            continue;

        if( Result.length() )
            Result += ",";
        Result += StringUtils::itos(pDevice->m_dwPK_Device);
    }
    return Result;
}

string Router::GetDevicesByGroup(int PK_DeviceGroup)
{
	string sResult;
	DeviceGroup *pDeviceGroup = m_mapDeviceGroup_Find(PK_DeviceGroup);
	if( pDeviceGroup )
	{
		for(size_t s=0;s<pDeviceGroup->m_vectPK_Device.size();++s)
			sResult += StringUtils::itos(pDeviceGroup->m_vectPK_Device[s]) + ",";
	}
	return sResult;
}

void Router::AlarmCallback(int id, void* param)
{
	if( id==ALARM_DELAYED_COMMAND_EXECUTION )
	{
		DelayedCommandInfo *pDelayedCommandInfo = (DelayedCommandInfo *) param;
		ExecuteCommandGroup(pDelayedCommandInfo->m_PK_CommandGroup,pDelayedCommandInfo->m_pDevice_Sender,pDelayedCommandInfo->m_iStartingCommand);
		delete pDelayedCommandInfo;
	}
}

int Router::GetDeviceID( int iPK_DeviceTemplate, string sMacAddress, string sIPAddress )
{
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_STATUS,"Looking for device with temp: %d ip %s mac %s",iPK_DeviceTemplate,sIPAddress.c_str(),sMacAddress.c_str());
#endif

	if( sIPAddress.empty() && sMacAddress.empty() ) // We gotta have something to go on
		return 0;

	int PK_Device_MacOnly=0,PK_Device_IPOnly=0;
	ListDeviceData_Router *pListDeviceData_Router = m_mapDeviceByTemplate_Find(iPK_DeviceTemplate);
	if( pListDeviceData_Router )
	{
		for(ListDeviceData_Router::iterator it=pListDeviceData_Router->begin();it!=pListDeviceData_Router->end();++it)
		{
			DeviceData_Router *pDevice = *it;
			if( pDevice->m_sIPAddress==sIPAddress && pDevice->m_sMacAddress==sMacAddress )
				return pDevice->m_dwPK_Device;
			else if( pDevice->m_sMacAddress==sMacAddress )
				PK_Device_MacOnly=pDevice->m_dwPK_Device;
			else if( pDevice->m_sIPAddress==sIPAddress )
				PK_Device_IPOnly=pDevice->m_dwPK_Device;
		}
	}

	// We'll have to search based on IP or Mac Address.  This might be a new device that hasn't been loaded into the router yet
	vector<Row_Device *> vectRow_Device;
	string sWhere="FK_Installation=" + StringUtils::itos(m_dwPK_Installation) + " AND IPaddress='" + StringUtils::SQLEscape(sIPAddress) + "' AND MACaddress='" + StringUtils::SQLEscape(sMacAddress) + "'";
	m_pDatabase_pluto_main->Device_get()->GetRows(sWhere,&vectRow_Device);
	if( vectRow_Device.size()==0 )
	{
		sWhere="FK_Installation=" + StringUtils::itos(m_dwPK_Installation) + " AND MACaddress='" + StringUtils::SQLEscape(sMacAddress) + "'";
		m_pDatabase_pluto_main->Device_get()->GetRows(sWhere,&vectRow_Device);
		if( vectRow_Device.size()==0 )
		{
			sWhere="FK_Installation=" + StringUtils::itos(m_dwPK_Installation) + " AND IPaddress='" + StringUtils::SQLEscape(sIPAddress) + "'";
			m_pDatabase_pluto_main->Device_get()->GetRows(sWhere,&vectRow_Device);
		}
	}

	if( vectRow_Device.size()==1 )  // Only if we found only 1 device consider it a match
		return vectRow_Device[0]->PK_Device_get();
	return 0;
}

int Router::ConfirmDeviceTemplate( int iPK_Device, int iPK_DeviceTemplate )
{
	DeviceData_Router *pDevice = m_mapDeviceData_Router_Find( iPK_Device );
	if( !pDevice )
	{
		Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow( iPK_Device );
		if( pRow_Device && pRow_Device->FK_Installation_get()==m_dwPK_Installation )
			return 3;
		return 0;
	}
	return pDevice->m_dwPK_DeviceTemplate == iPK_DeviceTemplate ? 2 : 1;
}

void Router::PingFailed( ServerSocket *pServerSocket, int dwPK_Device )
{
	LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Router::PingFailed %p %s %d", 
		pServerSocket, pServerSocket->m_sName.c_str(), dwPK_Device );

	pServerSocket->IncrementReferences(); // The ping is one thing that doesn't keep an increment on the counter.  Increment it since RemoveAndDeleteSocket
	// is expecting that we will have done it already.
	RemoveAndDeleteSocket(pServerSocket);  // Nothing else will use this socket anymore anyway
}

void Router::RemoveAndDeleteSocket( ServerSocket *pServerSocket, bool bDontDelete )
{
	LoggerWrapper::GetInstance()->Write( LV_WARNING, "Router::RemoveAndDeleteSocket %p %d", pServerSocket, pServerSocket ? pServerSocket->m_dwPK_Device : 0 );
    PLUTO_SAFETY_LOCK(sl,m_CoreMutex);
	DeviceData_Router *pDevice = m_mapDeviceData_Router_Find( pServerSocket->m_dwPK_Device );
	if( pDevice )
	{
		if( pDevice->m_pSocket_Command == pServerSocket )
			pDevice->m_pSocket_Command = NULL;

		for(map<int,ServerSocket *>::iterator it=pDevice->m_mapSocket_Event.begin();it!=pDevice->m_mapSocket_Event.end();)
		{
			ServerSocket *pServerSocket_Event = (*it).second;
			if( pServerSocket->m_iInstanceID && pServerSocket->m_iInstanceID==pServerSocket_Event->m_iInstanceID )
				pDevice->m_mapSocket_Event.erase(it++);
			else
				++it;
		}
	}
	int PK_Device = pServerSocket->m_dwPK_Device;
	sl.Release();  // Mysql can get slow.  Don't hold the core mutex while doing a database update

	if(!m_bQuit)
	{
		string sSQL = "UPDATE Device SET Registered=0,psc_mod=psc_mod WHERE PK_Device=" + StringUtils::itos(PK_Device);
		m_pDatabase_pluto_main->threaded_db_wrapper_query(sSQL);
	}

	SocketListener::RemoveAndDeleteSocket( pServerSocket, bDontDelete );
}

bool Router::HandleRouterMessage(Message *pMessage)
{
	if( pMessage->m_dwMessage_Type==MESSAGETYPE_COMMAND && pMessage->m_dwID==COMMAND_Execute_Command_Group_CONST )
	{
		int PK_CommandGroup = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_PK_CommandGroup_CONST].c_str());
		if( PK_CommandGroup )
		{
		    LoggerWrapper::GetInstance()->Write(LV_STATUS,"Router::HandleRouterMessage COMMAND_Execute_Command_Group_CONST %d", PK_CommandGroup);
			DeviceData_Router *pDevice = m_mapDeviceData_Router_Find(pMessage->m_dwPK_Device_From);
			ExecuteCommandGroup(PK_CommandGroup,pDevice);
		}
		return true;

	}
	if( pMessage->m_dwMessage_Type==MESSAGETYPE_COMMAND && pMessage->m_dwID==COMMAND_Stop_Pending_Command_Group_CONST )
	{
		int PK_CommandGroup = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_PK_CommandGroup_CONST].c_str());
		string sDescription = pMessage->m_mapParameters[COMMANDPARAMETER_Description_CONST];
		string sHint = pMessage->m_mapParameters[COMMANDPARAMETER_Text_CONST];
		StopPendingCommandGroup(PK_CommandGroup,sDescription,sHint);
		return true;
	}
	return false;
}


void Router::CheckForRecursiveControlledVia(DeviceData_Router *pDevice,vector<int> *pvect_Device_ControlledVia)
{
	for(size_t s=0;s<pvect_Device_ControlledVia->size();++s)
		if( (*pvect_Device_ControlledVia)[s]==pDevice->m_dwPK_Device )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Device %d is recursive controlled via",pDevice->m_dwPK_Device);
			pDevice->m_pDevice_ControlledVia=NULL;
			pDevice->m_dwPK_Device_ControlledVia=0;
			pDevice->m_pRow_Device->FK_Device_ControlledVia_setNull(true);
			pDevice->m_pRow_Device->Table_Device_get()->Commit();
		}

	if( !pDevice->m_pDevice_ControlledVia )
		return;
	
	pvect_Device_ControlledVia->push_back(pDevice->m_dwPK_Device);
	CheckForRecursiveControlledVia((DeviceData_Router *) pDevice->m_pDevice_ControlledVia,pvect_Device_ControlledVia);
}
void Router::CheckForRecursiveRouteTo(DeviceData_Router *pDevice,vector<int> *pvect_Device_RouteTo)
{
	for(size_t s=0;s<pvect_Device_RouteTo->size();++s)
		if( (*pvect_Device_RouteTo)[s]==pDevice->m_dwPK_Device )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Device %d is recursive route to",pDevice->m_dwPK_Device);
			pDevice->m_pDevice_RouteTo=NULL;
			pDevice->m_pRow_Device->FK_Device_RouteTo_setNull(true);
			pDevice->m_pRow_Device->Table_Device_get()->Commit();
		}

	if( !pDevice->m_pDevice_RouteTo )
		return;
	
	pvect_Device_RouteTo->push_back(pDevice->m_dwPK_Device);
	CheckForRecursiveRouteTo((DeviceData_Router *) pDevice->m_pDevice_RouteTo,pvect_Device_RouteTo);
}
void Router::CheckForRecursivePipes(DeviceData_Router *pDevice,vector<int> *pvect_Device_Pipe)
{
	bool bPipesRecurse=false;
    for(MapPipe::iterator it=pDevice->m_mapPipe_Available.begin();it!=pDevice->m_mapPipe_Available.end();++it)
    {
        Pipe *pPipe = (*it).second;
		for(size_t s=0;s<pvect_Device_Pipe->size();++s)
			if( (*pvect_Device_Pipe)[s]==pDevice->m_dwPK_Device )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Device %d is recursive pipe",pDevice->m_dwPK_Device);
				bPipesRecurse=true;
				Row_Device_Device_Pipe *pRow_Device_Device_Pipe = m_pDatabase_pluto_main->Device_Device_Pipe_get()->GetRow(pPipe->m_pDevice_From->m_dwPK_Device,pPipe->m_pDevice_To->m_dwPK_Device,pPipe->m_PK_Pipe);
				if( pRow_Device_Device_Pipe )
				{
					pRow_Device_Device_Pipe->Delete();
					pRow_Device_Device_Pipe->Table_Device_Device_Pipe_get()->Commit();
				}
			}
	}

	if( bPipesRecurse )
		pDevice->m_mapPipe_Available.clear();

	pvect_Device_Pipe->push_back(pDevice->m_dwPK_Device);

	// A device can have both an audio & video pipe going to the same device.  Avoid scanning the same device 
	// twice this way, since it will show up as a false recursive
	map<int,bool> mapDevices_InPipes;

	// Store the destinations in a vect so we don't risk clearing the map while a parent is iterating it
	vector<DeviceData_Router *> vectDevice;
    for(MapPipe::iterator it=pDevice->m_mapPipe_Available.begin();it!=pDevice->m_mapPipe_Available.end();++it)
    {
        Pipe *pPipe = (*it).second;
		if( mapDevices_InPipes[pPipe->m_pDevice_To->m_dwPK_Device] )
			continue;
		mapDevices_InPipes[pPipe->m_pDevice_To->m_dwPK_Device]=true;

		DeviceData_Router *pDevice_Dest = m_mapDeviceData_Router_Find(pPipe->m_pDevice_To->m_dwPK_Device);
		if( pDevice_Dest )
			vectDevice.push_back(pDevice_Dest);
	}

	for(size_t s=0;s<vectDevice.size();++s)
		CheckForRecursivePipes(vectDevice[s],pvect_Device_Pipe);
}

#ifdef LL_DEBUG
#include "Socket.h"
void Router::ShowSockets()
{
	PLUTO_SAFETY_LOCK(sim,*g_pSocketInfoMutex);
	for(map<int,SocketInfo *>::iterator it=g_mapSocketInfo.begin();it!=g_mapSocketInfo.end();++it)
	{
		SocketInfo *pSocketInfo = it->second;
		if( pSocketInfo->m_tDestroyed==0 )
			continue;
		string sDestroyed=asctime(localtime(&pSocketInfo->m_tDestroyed));
		LoggerWrapper::GetInstance()->Write(LV_DEBUG,"CLOSED SOCKET: %p #%d %s\nComment %s Device %s\nCreated %s Destroyed %s Log %s\nLast In %s\nLast Out %s",
			pSocketInfo->m_pSocket,pSocketInfo->m_iSocketCounter,pSocketInfo->m_sName.c_str(),
			pSocketInfo->m_sComment.c_str(),pSocketInfo->m_sDevice.c_str(),
			asctime(localtime(&pSocketInfo->m_tCreated)),sDestroyed.c_str(),
			pSocketInfo->m_sLogFile.c_str(),pSocketInfo->m_sLastStringIn.c_str(),pSocketInfo->m_sLastStringOut.c_str());
	}
	for(map<int,SocketInfo *>::iterator it=g_mapSocketInfo.begin();it!=g_mapSocketInfo.end();++it)
	{
		SocketInfo *pSocketInfo = it->second;
		if( pSocketInfo->m_tDestroyed!=0 )
			continue;
		LoggerWrapper::GetInstance()->Write(LV_DEBUG,"OPEN SOCKET: %p #%d %s\nComment %s Device %s\nCreated %s Log %s\nLast In %s\nLast Out %s",
			pSocketInfo->m_pSocket,pSocketInfo->m_iSocketCounter,pSocketInfo->m_sName.c_str(),
			pSocketInfo->m_sComment.c_str(),pSocketInfo->m_sDevice.c_str(),
			asctime(localtime(&pSocketInfo->m_tCreated)),
			pSocketInfo->m_sLogFile.c_str(),pSocketInfo->m_sLastStringIn.c_str(),pSocketInfo->m_sLastStringOut.c_str());
	}
}
#endif

bool Router::IsPlugin(int iPK_Device)
{
	return ( m_mapPlugIn.find(iPK_Device) !=  m_mapPlugIn.end() );
}

bool SerializeMessageXML(Message *pMessage, char *&pData, size_t &nSize)
{
	//message -> xml
	MessageXML msg_xml(pMessage);
	string sData = msg_xml.GetXML();

	nSize = sData.length() + 1;
	pData = new char[nSize];
	memcpy(pData, sData.c_str(), nSize - 1);
	pData[nSize]  = '\0';

	return !sData.empty();
}

bool DeserializeMessageXML(Message *pMessage, char *pData, size_t nSize)
{
	//xml -> message
	string sXMLData(pData);
	MessageXML msg_xml(sXMLData, pMessage);

	return !msg_xml.Failed();
}

void Router::HandleMessageFailure(SafetyMessage *pSafetyMessage)
{
	if( (*(*pSafetyMessage))->m_eRetry==MR_None ) 
		return;  // Nothing to do

	pSafetyMessage->m_bAutoDelete_set(false);
	HandleMessageFailure(pSafetyMessage->m_pMessage);
}

void Router::HandleMessageFailure(Message *pMessage,bool bSkipSerialization)
{
	if(pMessage->m_dwPK_Device_Template != 0 || pMessage->m_dwPK_Device_Category_To != 0 || !pMessage->m_sPK_Device_List_To.empty())
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Router::HandleMessageFailure WON'T retry message "
			"type %d id %d to %d retry %d because we cannot handle broadcast/multicast messages",
			pMessage->m_dwMessage_Type, pMessage->m_dwID,
			pMessage->m_dwPK_Device_To, (int) pMessage->m_eRetry);

		return;
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Router::HandleMessageFailure will retry message type %d id %d to %d retry %d",
		pMessage->m_dwMessage_Type, pMessage->m_dwID,
		pMessage->m_dwPK_Device_To, (int) pMessage->m_eRetry);

	PLUTO_SAFETY_LOCK(im,m_InterceptorMutex);  // Protect m_mapPendingMessages

	ListMessage *pListMessage = m_mapPendingMessages[pMessage->m_dwPK_Device_To];
	if( pListMessage==NULL )
	{
		pListMessage = new ListMessage();
		m_mapPendingMessages[pMessage->m_dwPK_Device_To]=pListMessage;
	}
	pListMessage->push_back(pMessage);

	if( pMessage->m_eRetry==MR_Persist && bSkipSerialization==false )
	{
		im.Release(); // Not recursive
		SerializePersistentMessages();
	}
}

void Router::CheckForMessageRetries(int DeviceID)
{
	PLUTO_SAFETY_LOCK(im,m_InterceptorMutex);  // Protect m_mapPendingMessages
	map<int,ListMessage *>::iterator it=m_mapPendingMessages.find(DeviceID);
	if( it==m_mapPendingMessages.end() )
		return;  // Nothing to do

	ListMessage *pListMessage = it->second;
	if( !pListMessage )
		return; // Shouldn't happen

	bool bSerializePersistent=false; // Keep track of whether we need to re-serialize this

	for(ListMessage::iterator it=pListMessage->begin();it!=pListMessage->end();++it)
	{
		Message *pMessage = *it;
		if( pMessage->m_eRetry==MR_Persist )
			bSerializePersistent=true;
		AddMessageToQueue(pMessage);
	}
	pListMessage->clear();  // We processed all the ones here.  If they fail again, they'll be re-added

	if( bSerializePersistent )
	{
		im.Release();
		SerializePersistentMessages();
	}
}

void Router::ReadPersistentMessages()
{
	size_t size;
	char *pBuffer =	FileUtils::ReadFileIntoBuffer(SERIALIZE_MESSAGE_FILE,size);
	if( pBuffer )
	{
		SerializeClass serializeClass;
		serializeClass.StartReading(static_cast<unsigned long>(size),pBuffer);
		int NumMessages = serializeClass.Read_unsigned_long();
		for(int i=0;i<NumMessages;++i)
		{
			int Size = serializeClass.Read_unsigned_long();
			char *pBuffer = serializeClass.Read_block(Size);
			Message *pMessage = new Message(Size,pBuffer);
			HandleMessageFailure(pMessage,true);
		}

		delete pBuffer;
	}
	SerializePersistentMessages();  // Re-write taking into account what may have gotten processed (probably nothing)
}

void Router::SerializePersistentMessages()
{
	PLUTO_SAFETY_LOCK(im,m_InterceptorMutex);  // Protect m_mapPendingMessages

	ListMessage listMessage; // Temporary hold all the messages
	for(map<int,ListMessage *>::iterator itList=m_mapPendingMessages.begin();itList!=m_mapPendingMessages.end();++itList)
	{
		ListMessage *pListMessage = itList->second;
		if( pListMessage )
		{
			for(ListMessage::iterator itMsg=pListMessage->begin();itMsg!=pListMessage->end();++itMsg)
			{
				Message *pMessage = *itMsg;
				if( pMessage->m_eRetry==MR_Persist )
					listMessage.push_back(pMessage);
			}
		}
	}

	if( listMessage.empty() )
	{
		FileUtils::DelFile(SERIALIZE_MESSAGE_FILE);
		return;
	}

    LoggerWrapper::GetInstance()->Write(LV_STATUS,"Router::SerializePersistentMessages %d Messages", listMessage.size());

	SerializeClass serializeClass;
	serializeClass.StartWriting();
	serializeClass.Write_unsigned_long((int) listMessage.size());

	for(ListMessage::iterator itList=listMessage.begin();itList!=listMessage.end();++itList)
	{
		Message *pMessage = *itList;
		char *pcData = NULL;
		unsigned long dwSize = 0;
		pMessage->ToData( dwSize, pcData, false ); // converts the message to data
		serializeClass.Write_unsigned_long(dwSize);
		serializeClass.Write_block(pcData,dwSize);
	}

	FILE *pFile = fopen(SERIALIZE_MESSAGE_FILE,"wb");
	if( !pFile || fwrite(serializeClass.m_pcDataBlock, 1, serializeClass.CurrentSize(), pFile) != serializeClass.CurrentSize() )
	{
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Router::SerializePersistentMessages failed to write to file %s",SERIALIZE_MESSAGE_FILE);
		FileUtils::DelFile(SERIALIZE_MESSAGE_FILE);
	}

	if( pFile )
		fclose( pFile );
}

