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

/**
 *
 * @file Media_Plugin.h
 * @brief header file for the  Media_Plugin, MediaDevice, MediaHandlerBase classes
 */

//<-dceag-d-b->
#ifndef Media_Plugin_h
#define Media_Plugin_h

//	DCE Implemenation for #2 Media Plug-in

#include "Gen_Devices/Media_PluginBase.h"
//<-dceag-d-e->

#include <deque>

#include "DeviceData_Router.h"
#include "DCE/Logger.h"
#include "DCERouter.h"
#include "AlarmManager.h"
#include "MediaHandlerInfo.h"
#include "MediaHandlerBase.h"
#include "pluto_main/Table_MediaType.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_DeviceTemplate_MediaType.h"
#include "pluto_main/Table_DeviceTemplate_MediaType_DesignObj.h"
#include "pluto_main/Table_MediaType_DesignObj.h"
#include "pluto_main/Define_Screen.h"

#include "pluto_media/Define_AttributeType.h"
#include "Datagrid_Plugin/Datagrid_Plugin.h"
#include "MediaAttributes.h"
#include "Gen_Devices/AllCommandsRequests.h"

#include "../Orbiter/Floorplan.h"
#include "Orbiter_Plugin/FollowMe_Device.h"
#include "MediaStream.h"
#include "EntertainArea.h"
#include "Orbiter/OrbiterFileBrowser.h"
#include "Orbiter_Plugin/OH_Orbiter.h"
#include "Gen_Devices/AllScreens.h"
#include "JobHandler/JobHandler.h"
#include "JobHandler/Job.h"
#include "JobHandler/Task.h"

using namespace nsJobHandler;
using namespace DCE;

class Database_pluto_main;
class Database_pluto_media;
class Row_EntertainArea;
class Row_File;
class MediaFile;
class MediaListGrid;
class DatabaseInfoOnPath;

#define NUM_UI_VERSIONS	2
#define MAX_MEDIA_COLORS    5 // We need some unique colors to color-code the media streams

namespace DCE
{
  class PendingTaskList;
}

namespace nsJobHandler
{

  class MoveJob : public Job
  {
    friend class MoveTask;
  public:
    bool m_bReportResult;
    string m_sFileName;
    string m_sDestinationFileName;
    Row_File *m_pRow_File;
    Database_pluto_media *m_pDatabase_pluto_media;
    int m_iPK_Orbiter;
  public:
    MoveJob(Database_pluto_media *pDatabase_pluto_media,
	    class JobHandler *pJobHandler,
	    int iPK_Orbiter,
	    Row_File *pRow_File,
	    string sFileName,
	    string sDestinationFileName,
	    bool bReportResult,
	   Command_Impl *pCommand_Impl);
    virtual ~MoveJob();

    virtual bool ReadyToRun();
    void AddMoveTasks(TasklistPosition position=position_TasklistEnd);

    virtual bool ReportPendingTasks(PendingTaskList *pPendingTaskList);
    virtual int SecondsRemaining();
    virtual string ToString();
    virtual int PercentComplete();
    virtual string GetType() { return "MoveJob"; }

    int Get_PK_Orbiter();

  };

  class MoveTask : public Task
  {
  public:
    class MoveJob *m_pMoveJob;
    bool m_bReportResult;
    Row_File *m_pRow_File;
    string m_sText;
    int m_iTime;
    string m_sFileName;
    string m_sDestinationFileName;
    bool m_bAlreadySpawned; 
    string m_sSpawnName;
    DeviceData_Base *m_pDevice_App_Server;

    MoveTask(class MoveJob *pMoveJob, 
	     string sName, 
	     bool bReportResult,
	     string sFileName, 
	     string sDestinationFileName);
    virtual ~MoveTask();
    virtual string GetType() { return "MoveTask"; }
    
    virtual int Run();
    virtual bool Abort();
    int RunAlreadySpawned();
    void ReportFailure();
    void ReportSuccess();
    void ReportProgress();
    void UpdateProgress(string sStatus, int iPercent, int iTime, string sText);
    virtual int SecondsRemaining();
  };

  class MoveDBTask : public Task
  {
  public:
    class MoveJob *m_pMoveJob;
    bool m_bReportResult;
    Row_File *m_pRow_File;
    string m_sText;
    bool m_bAlreadySpawned;
    
    MoveDBTask(class MoveJob *pMoveJob,
	       string sName,
	       bool bReportResult);
    virtual ~MoveDBTask();
    virtual string GetType() { return "MoveDBTask"; }
    
    virtual int Run();
    virtual bool Abort();
    int RunAlreadySpawned();

  };

  class TranscodeJob : public Job
  {
    friend class TranscodeTask;
  public:
    bool m_bDeleteSourceAfterTranscode;
    bool m_bReportResult;
    string m_sFileName;
    string m_sDestinationFileName;
    int m_iPK_Orbiter;
    map<string, string> m_mapParametersAndValues;
  public:
TranscodeJob(class JobHandler *pJobHandler,
	     int iPK_Orbiter,
	     string sFileName,
	     string sDestinationFileName,
	     map<string, string> mapParametersAndValues,
	     bool bDeleteSourceAfterTranscode,
	     Command_Impl *pCommand_Impl);
    virtual ~TranscodeJob();

    virtual bool ReadyToRun();
    void AddTranscodeTask();

    virtual bool ReportPendingTasks(PendingTaskList *pPendingTaskList);
    virtual int SecondsRemaining();
    virtual string ToString();
    virtual int PercentComplete();
    virtual string GetType() { return "TranscodeJob"; }

    int Get_PK_Orbiter();

  };

  class TranscodeTask : public Task
  {
  public:
    class TranscodeJob *m_pTranscodeJob;
    string m_sText;
    int m_iTime;
    string m_sFileName;
    string m_sDestinationFileName;
    bool m_bAlreadySpawned; 
    string m_sSpawnName;
    DeviceData_Base *m_pDevice_App_Server;
    map<string,string> m_mapParametersAndValues;

    TranscodeTask(class TranscodeJob *pTranscodeJob, 
	     string sName, 
	     string sFileName, 
	     string sDestinationFileName);
    virtual ~TranscodeTask();
    virtual string GetType() { return "TranscodeTask"; }
    
    virtual int Run();
    virtual bool Abort();
    int RunAlreadySpawned();
    void ReportFailure();
    void ReportSuccess();
    void ReportProgress();
    void UpdateProgress(string sStatus, int iPercent, int iTime, string sText);
    virtual int SecondsRemaining();
    string GetParameters();
  };

}

namespace DCE
{

//<-dceag-decl-b->!
class Media_Plugin : public Media_Plugin_Command, public DataGridGeneratorPlugIn, public FloorplanInfoProvider, public FollowMe_Device, public AlarmEvent
{
//<-dceag-decl-e->
    friend class MediaStream;
    friend class MediaAttributes;
//<-dceag-const-b->!
public:
        // Constructors/Destructor
        Media_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
        virtual ~Media_Plugin();
		virtual bool GetConfig();
        virtual bool Register();
        virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
        virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
	virtual void PrepareToDelete();

    class MediaAttributes *m_pMediaAttributes;
	class Generic_NonPluto_Media *m_pGeneric_NonPluto_Media;
    class MediaHandlerInfo *m_pGenericMediaHandlerInfo;
	class AlarmManager *m_pAlarmManager;
	class JobHandler *m_pJobHandler;

    friend class MediaHandlerInfo;
    pluto_pthread_mutex_t m_MediaMutex; // Other classes may need this

	Database_pluto_main *m_pDatabase_pluto_main;
	Database_pluto_media *m_pDatabase_pluto_media;

private:
    // Private member variables
    class Orbiter_Plugin *m_pOrbiter_Plugin;
    class File_Grids_Plugin *m_pFile_Grids_Plugin;
    int m_iStreamID;
	int m_iPK_File_Last_Scanned_For_New;
	int m_dwPK_Device_MediaIdentification;
    pthread_mutexattr_t m_MutexAttr;
    MapMediaStream m_mapMediaStream; // All active streams
	string m_sPK_Devices_Online; // List of available storage devices, comma separated
	time_t m_tLastScanOfOnlineDevices,m_tLastSearchTokenUpdate;
	map< pair<int,int>,bool > m_mapMediaType_AttributeType_Identifier;  // MediaType/AttributeType Combinations that uniquely identify a media file
	map< int, pair<time_t,bool> > m_mapMediaDevicesOnline; // PK_Device (for NAS, drives, etc.) with the last time the status was checked and true if it's online
	pair<time_t,bool> m_mapMediaDevicesOnline_Find(int iPK_Device) { map< int, pair<time_t,bool> >::iterator it = m_mapMediaDevicesOnline.find(iPK_Device); return it==m_mapMediaDevicesOnline.end() ? make_pair<time_t,bool> (0,false) : (*it).second; }

//  Have to always include it so all plugins see the same header.  Delete the following when removing sim_jukebox #ifdef SIM_JUKEBOX
	map< int, string > m_mapPK_FilesForSimulatedPurchase;   // Simulate downloadable premium content

	map<int,string> m_mapMediaType_2_Directory;
	map<int,bool> m_mapMediaType_Bookmarkable;

	map<int,int> m_mapOrbiterUiVersion;

	// This has all the media catalog->media handler associations
	map<string,MediaHandlerBase *> m_mapMediaCatalog;
    MediaHandlerBase *m_mapMediaCatalog_Find(string sKey) { map<string,MediaHandlerBase *>::iterator it = m_mapMediaCatalog.find(sKey); return it==m_mapMediaCatalog.end() ? NULL : (*it).second; }

	// Keep track of which users,media types want to be prompted to resume.  Valid char values are 'N' (never resume), 'A' (always resume), 'P' or anything else (prompt)
	map< pair<int,int>,char > m_mapPromptResume;

	// When deciding what remote control to use, we will look in these 4 maps in this order,
	// first for a specific remote for this orbiter/source device/mediatype combination, then for this
	// orbiter/media type, then for the source device/media type, lastly any remote for the media type
	map< pair<int,pair<int,int> >, class RemoteControlSet *> m_mapOrbiter_DeviceTemplate_MediaType_RemoteControl[NUM_UI_VERSIONS];
	map< pair<int,int>, class RemoteControlSet *> m_mapOrbiter_MediaType_RemoteControl[NUM_UI_VERSIONS];
	map< pair<int,int>, class RemoteControlSet *> m_mapDeviceTemplate_MediaType_RemoteControl[NUM_UI_VERSIONS];
	map< int, class RemoteControlSet *> m_mapMediaType_RemoteControl[NUM_UI_VERSIONS];

    class Datagrid_Plugin *m_pDatagrid_Plugin;

	void PopulateRemoteControlMaps();

protected:
    void PlayMediaByDeviceTemplate(int iPK_DeviceTemplate, int iPK_Device, int iPK_Device_Orbiter, EntertainArea *pEntertainArea, string &sCMD_Result);
    void PlayMediaByFileName(string sFilename, int iPK_Device, int iPK_Device_Orbiter, EntertainArea *pEntertainArea, string &sCMD_Result);
    bool EnsureCorrectMediaStreamForDevice(MediaHandlerInfo *pMediaHandlerInfo, EntertainArea *pEntertainArea, int iPK_Device);

	bool DiskDriveIsRipping(int iPK_Device);
	bool HandleDeviceOnOffEvent(MediaDevice *pMediaDevice,bool bIsOn);

	RemoteControlSet *PickRemoteControlMap(int PK_Orbiter,int iPK_DeviceTemplate,int PK_MediaType,int UIVersion_Forced=0)
	{
		int UIVersion = UIVersion_Forced ? UIVersion_Forced : 1;
		// We're going to look in 4 places, from most specific to least specific
		if( PK_Orbiter )
		{
			if( UIVersion_Forced==0 )
			{
				UIVersion = m_mapOrbiterUiVersion[PK_Orbiter];
				if( !UIVersion )
					UIVersion = 1;
			}

			// The first 2 maps will see if the user directly specified a remote control to use for the given remote
			map< pair<int,pair<int,int> >, class RemoteControlSet *>::iterator it1;
			pair<int,pair<int,int> > p1 = make_pair< int,pair<int,int> > (
				PK_Orbiter,
				make_pair< int,int > ( iPK_DeviceTemplate, PK_MediaType ) );
			if( (it1=m_mapOrbiter_DeviceTemplate_MediaType_RemoteControl[UIVersion-1].find(p1))!=m_mapOrbiter_DeviceTemplate_MediaType_RemoteControl[UIVersion-1].end() )
				return it1->second;

			map< pair<int,int>, class RemoteControlSet *>::iterator it2;
			pair<int,int> p2 = make_pair< int,int > (
				PK_Orbiter,
				PK_MediaType );
			if( (it2=m_mapOrbiter_MediaType_RemoteControl[UIVersion-1].find(p2))!=m_mapOrbiter_MediaType_RemoteControl[UIVersion-1].end() )
				return it2->second;
		}

		// Next if a remote is specified for this particular type of media device
		map< pair<int,int>, class RemoteControlSet *>::iterator it3;
		pair<int,int> p3 = make_pair< int,int > (
			iPK_DeviceTemplate,
			PK_MediaType );

		if( (it3=m_mapDeviceTemplate_MediaType_RemoteControl[UIVersion-1].find(p3))!=m_mapDeviceTemplate_MediaType_RemoteControl[UIVersion-1].end() )
			return it3->second;

		// Next if a remote is specified for this particular type of orbiter
		if( PK_Orbiter )
		{
			DeviceData_Router *pDevice = m_pRouter->m_mapDeviceData_Router_Find(PK_Orbiter);
			if( pDevice )
			{
				p3.first = pDevice->m_dwPK_DeviceTemplate;
				if( (it3=m_mapDeviceTemplate_MediaType_RemoteControl[UIVersion-1].find(p3))!=m_mapDeviceTemplate_MediaType_RemoteControl[UIVersion-1].end() )
					return it3->second;
			}
		}

		if( (it3=m_mapDeviceTemplate_MediaType_RemoteControl[UIVersion-1].find(p3))!=m_mapDeviceTemplate_MediaType_RemoteControl[UIVersion-1].end() )
			return it3->second;

		map< int, class RemoteControlSet *>::iterator it4;
		if( (it4=m_mapMediaType_RemoteControl[UIVersion-1].find(PK_MediaType))!=m_mapMediaType_RemoteControl[UIVersion-1].end() )
			return it4->second;

		if( UIVersion_Forced==0 && UIVersion!=1 )
			return PickRemoteControlMap(PK_Orbiter,iPK_DeviceTemplate,PK_MediaType,1);  // Try again with UI 1
		return NULL;
	}

    /**
     * Find a media plugin info object that will play the specified file.
     */
    MediaHandlerInfo *FindMediaHandlerInfoForFileName(EntertainArea *pEntertainArea, string sFileToPlay);
    /**
     * Find a media plugin info object that will play the specified media type.
     */
    MediaHandlerInfo *FindMediaHandlerInfoForMediaType(EntertainArea *pEntertainArea, int iPK_MediaType);

    /**
     * Since a destination device may have separate output zones in different EA's, this function confirms the destination output and creates alternate pipes to handle other zones 
	 * Returns false if there's no way to get media from the source device to the destination EA
     */
	bool CheckForAlternatePipes(MediaStream *pMediaStream,EntertainArea *pEntertainArea);
	bool CheckForAlternatePipes(DeviceData_Router *pDevice_From,DeviceData_Router *&pDevice_To,EntertainArea *pEntertainArea);  // Recursive
	void AddAlternativeRoute(DeviceData_Router *pDevice_From,DeviceData_Router *&pDevice_To,Pipe *pPipe,EntertainArea *pEntertainArea);
	// Add the COMMANDPARAMETER_PipeID_CONST to the message if it's a command going to an ea with a custom pipe
	void CheckForCustomPipe(EntertainArea *pEntertainArea,Message *pMessage);

	/**
	 * Used by GetConfig to recursively add all source devices into an output zone that can feed into that output zone
	 */
	void RecursivelyAddSendingDevice(MediaDevice *pMediaDevice_FeedInto,MediaDevice *pMediaDevice_FeedFrom);

	/**
	 * When we don't know the media type, go through the entertainment areas and try to find it
	 */
	int GetMediaTypeForFile(deque<MediaFile *> *p_dequeMediaFile,vector<EntertainArea *> &vectEntertainArea);

	/**
     * Given that media was playing on the Prior list of devices, and now is on the Current list of devices, send the appropriate on/off's
     */
	void HandleOnOffs(int PK_MediaType_Prior,int PK_MediaType_Current, map<int,MediaDevice *> *pmapMediaDevice_Prior,map<int,MediaDevice *> *pmapMediaDevice_Current,MediaStream *pMediaStream,EntertainArea *pEntertainArea);

    /**
     * Turn off the device and other devices in the pipe, but without turning off devices we are currently using
     */
	void TurnDeviceOff(int PK_Pipe,DeviceData_Router *pDeviceData_Router,map<int,MediaDevice *> *pmapMediaDevice_Current,EntertainArea *pEntertainArea,vector<int> *p_vectDevice=NULL);

	/**
	 * Handle adjustments the user wants, like changing zoom and audio setttings, based on values from the media.
	 * For each AV device a parameter AV Adjustment Rules can specify what commands to send based on strings from
	 * within the sAudio & sVideo.  Example: 
	 * dolby digital 2=312
	 * dolby digital 5=292
	 * 3:2=683
	 * !3:2=684
	 * means when the string "dolby digital 2" is contained in audio or video, send the command 312 (COMMAND_Stadium_CONST)
	 * to the device.  If "3:2" is in either string, send command 683
	 * If the user has turned off the 'Preserve Aspect Ratio' setting for a device, it will attempt
	 * first to match the string with an ! in front, so command 684 would be sent instead of that setting was set to false
	 */
	void HandleAVAdjustments(MediaStream *pMediaStream,string sAudio,string sVideo);

    /**
     * Add the file to the pluto_media database
     */
	void AddFileToDatabase(MediaFile *pMediaFile,int PK_MediaType);

    /**
     * Find a media type specific for a file name
     */
    //     int FindMediaTypeForFile(string strFileName);
    // Private methods
	vector<MediaHandlerInfo *> m_vectMediaHandlerInfo;

public:
    // Public member variables
    map<int,EntertainArea *> m_mapEntertainAreas;  // Plug-ins may want this
    map<int,MediaDevice *> m_mapMediaDevice;
	MapMediaStream *m_mapMediaStream_get() { return &m_mapMediaStream; }

    EntertainArea *m_mapEntertainAreas_Find(int iPK_EntertainArea) { map<int,class EntertainArea *>::iterator it = m_mapEntertainAreas.find(iPK_EntertainArea); return it==m_mapEntertainAreas.end() ? NULL : (*it).second; }
    MediaDevice *m_mapMediaDevice_Find(int iPK_Device) { map<int,class MediaDevice *>::iterator it = m_mapMediaDevice.find(iPK_Device); return it==m_mapMediaDevice.end() ? NULL : (*it).second; }
    MediaStream *m_mapMediaStream_Find(int StreamID,int iPK_Device_Source)
	{ 
		MapMediaStream::iterator it = m_mapMediaStream.find(StreamID);  
		if( it!=m_mapMediaStream.end() )
			return it->second; 
		// Sometimes the media player doesn't use the stream ID.  If so we can't be 100% certain, but it's probably the media stream in that source device's ent area.
		if( StreamID!=0 )
			return NULL; // The media player is using a stream ID, but it wasn't specified
		MediaDevice *pMediaDevice = m_mapMediaDevice_Find(iPK_Device_Source);
		if( !pMediaDevice )
			return NULL;
		for(map<int,class EntertainArea *>::iterator it2=pMediaDevice->m_mapEntertainArea.begin();it2!=pMediaDevice->m_mapEntertainArea.end();++it2)
		{
			EntertainArea *pEntertainArea = it2->second;
			if( pEntertainArea->m_pMediaStream )
				return pEntertainArea->m_pMediaStream;
		}
		return NULL;
	}
	bool m_mapMediaType_Bookmarkable_Find(int iPK_MediaType) { map<int,bool>::iterator it=m_mapMediaType_Bookmarkable.find(iPK_MediaType); return it==m_mapMediaType_Bookmarkable.end() ? false : (*it).second; }
	void m_mapMediaStream_Remove(int StreamID) { MapMediaStream::iterator it = m_mapMediaStream.find(StreamID); if( it!=m_mapMediaStream.end() ) m_mapMediaStream.erase(it); }

	Database_pluto_main *GetMainDatabaseConnection()
	{
		return m_pDatabase_pluto_main;
	}

	Database_pluto_media *GetMediaDatabaseConnect()
	{
		return m_pDatabase_pluto_media;
	}

    // A MediaPlugIn call call this function, passing in the type of device that it will handle.  This will automatically build MediaPlugInInfo's for all the
    // types that the device supports, using all the default values from the database.  A plug-in could also call the MediaHandlerInfo constructor that will
    // create and register a specific type of media.  This means it is not necessary to call RegisterMediaPlugin.  This allows the plug-in to change
    // some of the defaults.  The Plugin could also create a MediaHandlerInfo type manually for complete control over the parameters, and then call
    // RegisterMediaPlugin
    void RegisterMediaPlugin(class Command_Impl *pCommand_Impl,class MediaHandlerBase *pMediaHandlerBase,int iPK_DeviceTemplate,bool bUsesDCE)
	{
		vector<int> vectPK_DeviceTemplate;
		vectPK_DeviceTemplate.push_back(iPK_DeviceTemplate);
		RegisterMediaPlugin(pCommand_Impl,pMediaHandlerBase,vectPK_DeviceTemplate,bUsesDCE);
	}

    void RegisterMediaPlugin(class Command_Impl *pCommand_Impl,class MediaHandlerBase *pMediaHandlerBase,vector<int> &vectPK_DeviceTemplate,bool bUsesDCE);

    void AddDeviceToEntertainArea(EntertainArea *pEntertainArea,Row_Device *pRow_Device);
	void DetermineEntArea( int iPK_Device_Orbiter, int iPK_Device, string sPK_EntertainArea, vector<EntertainArea *> &vectEntertainArea, int *p_iStreamID=NULL );
    class MediaStream *DetermineStreamOnOrbiter(int iPK_Device_Orbiter,bool bErrorIfNotFound=true)
    {
        vector<class EntertainArea *> vectEntertainArea;
		DetermineEntArea(iPK_Device_Orbiter,0,"",vectEntertainArea);
        if( (vectEntertainArea.size()!=1 || !vectEntertainArea[0]->m_pMediaStream) && bErrorIfNotFound ) LoggerWrapper::GetInstance()->Write(LV_WARNING,"No stream on orbiter: %d",iPK_Device_Orbiter);
        return vectEntertainArea.size()==1 ? vectEntertainArea[0]->m_pMediaStream : NULL;
    }

    int DetermineUserOnOrbiter(int iPK_Device_Orbiter);
    bool IgnoreFileForMediaType(string sFilename, int iPK_MediaType);

	// For the alarm callbacks
	virtual void AlarmCallback(int id, void* param);
	void ProcessMediaFileTimeout(MediaStream *pMediaStream);
	void UpdateSearchTokens();

    /**
     * @brief Helper functions for the process of auto-creating entertainment areas
     */
	void DeleteEntertainArea(Row_EntertainArea *pRow_EntertainArea);
	void AddDefaultCommandsToEntArea(Row_EntertainArea *pRow_EntertainArea);
	void AddDevicesToEntArea(Row_EntertainArea *pRow_EntertainArea);
	// Returns 0 if a command group exists for this entarea/template.  Otherwise creates one and returns the id
		int FindCommandGroupByTemplate(Row_EntertainArea *pRow_EntertainArea,int PK_Template,string sDescription);
	void AddCommand(int PK_CommandGroup,int PK_Device,int PK_Command,int NumParms,...);
	// For each MD, all it's direct children go in the same room, and if it has an on-screen Orbiter, it's direct children too
	void PutMDsChildrenInRoom(DeviceData_Router *pDeviceData_Router);
	void GetMediaHandlersForEA(int iPK_MediaType,int iPK_MediaProvider,int &iPK_Device, int iPK_DeviceTemplate, vector<EntertainArea *> &vectEntertainArea, vector< pair< MediaHandlerInfo *,vector<EntertainArea *> > > &vectEA_to_MediaHandler, map<int, pair<MediaDevice *,MediaDevice *> > &mapEntertainmentArea_OutputZone);
	MediaDevice *GetMediaDeviceForEA(int iPK_MediaType,EntertainArea *pEntertainArea);

	/**
     * @brief Save the last positions so we can resume
     */
	void SaveLastPlaylistPosition(MediaStream *pMediaStream);
	void SaveLastFilePosition(MediaStream *pMediaStream);
	void SaveLastDiscPosition(MediaStream *pMediaStream);
	void RestoreMediaResumePreferences();
	void SaveMediaResumePreferences();
	int CheckForAutoResume(MediaStream *pMediaStream);

	/**
     * @brief Required for plug-ins that render floorplans
     */
    virtual void GetFloorplanDeviceInfo(DeviceData_Router *pDeviceData_Router,EntertainArea *pEntertainArea,int iFloorplanObjectType,int &iPK_FloorplanObjectType_Color,int &Color,string &sDescription,string &OSD,int &PK_DesignObj_Toolbar);

	// Get a list of all the screens an orbiter will use to show the user a file picker
	OrbiterFileBrowser_Collection *CreateOrbiterFileList(OH_Orbiter *pOH_Orbiter)
	{
		OrbiterFileBrowser_Collection *pOrbiterFileBrowser_Collection = new OrbiterFileBrowser_Collection();

		for(map<int,string>::iterator it=m_mapMediaType_2_Directory.begin();it!=m_mapMediaType_2_Directory.end();++it)
		{
			RemoteControlSet *pRemoteControlSet = 
				PickRemoteControlMap(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_DeviceTemplate,it->first);

			if( pRemoteControlSet )
			{
				OrbiterFileBrowser_Entry *pOrbiterFileBrowser_Entry = new OrbiterFileBrowser_Entry();
				pOrbiterFileBrowser_Entry->m_PK_Screen=pRemoteControlSet->m_iPK_Screen_FileList;
				pOrbiterFileBrowser_Entry->m_MediaType=it->first;
				pOrbiterFileBrowser_Entry->m_sFilename=m_mapMediaType_2_Directory[it->first];
				pOrbiterFileBrowser_Collection->m_mapOrbiterFileBrowser[it->first]=pOrbiterFileBrowser_Entry;
			}
		}
		return pOrbiterFileBrowser_Collection;
	}

	// This version is called by MH_Play_Media.  It may result in multiple handlers and multiple streams
	// if there isn't 1 handler that can do it all.  If p_vectMediaStream is passed it will have a list of all the streams that were created as a result
	void StartMedia( int iPK_MediaType, int iPK_MediaProvider, unsigned int iPK_Device_Orbiter, vector<EntertainArea *> &vectEntertainArea, int iPK_Device, int iPK_DeviceTemplate, deque<MediaFile *> *dequeMediaFile, bool bQueue, bool bResume, int iRepeat, string sStartingPosition, bool bBypass_Event, vector<MediaStream *> *p_vectMediaStream=NULL);

	// This creates a single media stream for a given media handler and starts playing it by calling the next StartMedia, or returns NULL if it cannot create the stream
	MediaStream *StartMedia(MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider, unsigned int PK_Device_Orbiter,vector<EntertainArea *> &vectEntertainArea,int PK_Device_Source,deque<MediaFile *> *dequeMediaFile, bool bQueue, bool bResume,int iRepeat, string sStartingPosition, bool bBypass_Event, int iPK_Playlist=0, map<int, pair<MediaDevice *,MediaDevice *> > *p_mapEntertainmentArea_OutputZone=NULL);

	// This is the second to the final stage of 'StartMedia'.  
	bool StartMedia(MediaStream *pMediaStream, map<int, pair<MediaDevice *,MediaDevice *> > *p_mapEntertainmentArea_OutputZone, bool bBypass_Event);

	// This is the final stage of 'StartMedia' that starts playing the given stream.  This is also called when the stream changes, or is moved, and needs to be restarted
	bool StartMedia(MediaStream *pMediaStream);

	// An alarm callback when we're holding up starting the stream while waiting for a jukebox to load a drive
	void WaitingForJukebox( MediaStream *pMediaStream );

	// Used by StartMedia to confirm the m_pMediaDevice_ActiveDest are set for each ea
	void FindActiveDestination(MediaStream *pMediaStream,map<int, pair<MediaDevice *,MediaDevice *> > *p_mapEntertainmentArea_OutputZone);

	// If there's a capture card active, StartMedia will call this
	void StartCaptureCard(MediaStream *pMediaStream);
	void StopCaptureCard(MediaStream *pMediaStream);

	/**
     * @brief More capable StartMedia. Does not need an actual device since it will search for it at the play time.
     *
     * It will also take care of moving the playlists when we use another device to play the media. It will also receive and actual device play list and not only
     *  a filename.
     */
    bool StartMediaOnPlugin(MediaHandlerInfo *pMediaHandlerInfo, EntertainArea *pEntertainArea);

    /**
     * @brief Whenever the state of the media changes, the plug-in should call this function so we can update all the orbiter's pictures, descriptions, etc.
	 * If the content of the media changed, such as when a new CD is identified and all the track lists need updating, set bRefreshScreen=true
     */
    void MediaInfoChanged(MediaStream *pMediaStream, bool bRefreshScreen=false );

    // We need our own message queue so that if a message comes in we don't know how to handle, we first hand it off to the plug-in, and if
    // that doesn't handle it either, we send it to the actual media device
    ReceivedMessageResult ReceivedMessage(class Message *pMessage);

    // Our message interceptors
    /**
     * @brief EVENT_Media_Inserted event interceptor. A removable device should fire this event when it detected a new media inserted.
     */
    bool MediaInserted(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);

	/**
     * @brief EVENT_Playback_Completed event interceptor. Called when the router finds an event of this type in the queue.
     */
    bool PlaybackCompleted( class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);

	/**
     * @brief EVENT_Playback_Started event interceptor. Called when the router finds an event of this type in the queue.
     */
	bool PlaybackStarted( class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);

    /**
     * @brief EVENT_Media_Followme event interceptor. Called when the router finds an event of this type in the queue.
     */
	bool MediaFollowMe( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );

	/**
	 * @brief EVENT_Device_On_Off_CONST event interceptor, when some equipment was turned on or off manually
	 */
	bool DeviceOnOff( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );

	/**
	 * @brief Process in response to a CMD_Retransmit
	 */
	void HandleRetransmitOnOff(char A_or_V,char P_or_I,MediaDevice *pMediaDevice,int PK_Device_From,EntertainArea *pEntertainArea);

	/**
	 * @brief EVENT_AV_Input_Changed_CONST event interceptor, when some equipment was has the input changed manually
	 */
	bool AvInputChanged( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );

	/**
	 * @brief EVENT_MediaDescriptionChanged_CONST event interceptor, when some equipment wants to change the description for the media it's playing
	 */
	bool MediaDescriptionChanged( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );

	/**
	 * @brief Interceptor for set volume and mute commands, and volume changed event
	 */
	bool VolumeChanged( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );

	/**
	 * @brief Interceptor for subtitle track changed event
	 */
	bool SubtitleTrackChanged( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );

	/**
	 * @brief Interceptor for audio track changed event
	 */
	bool AudioTrackChanged( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );

	/**
	 * @brief Interceptor for video track changed event
	 */
	bool VideoTrackChanged( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );

	// Sometimes when MediaHanderBase::GetRenderDevices is called, only the top-level render devices (ie the media source)
	// is desired.  However, HandleOnOffs wants everything in the pipe, and the following function
	// can be used to add the other devices into the map
	void AddOtherDevicesInPipesToRenderDevices(int PK_Pipe, map<int,MediaDevice *> *pmapMediaDevice,EntertainArea *pEntertainArea);
	void AddOtherDevicesInPipes_Loop(int PK_Pipe, DeviceData_Router *pDevice,map<int,MediaDevice *> *pmapMediaDevice,EntertainArea *pEntertainArea,vector<int> *p_vectDevice=NULL);

	// If we're playing from a playlist, there may be a timeout specified, meaning after a certain time stop playing
	// this and go to the next item in the playlist
	void CheckStreamForTimeout(MediaStream *pMediaStream);
	void UpdateViewDate(MediaStream *pMediaStream);

	// Follow-me
	virtual void FollowMe_EnteredRoom(int iPK_Event, int iPK_Orbiter, int iPK_Device, int iPK_Users, int iPK_RoomOrEntArea, int iPK_RoomOrEntArea_Left);
	virtual void FollowMe_LeftRoom(int iPK_Event, int iPK_Orbiter, int iPK_Device, int iPK_Users, int iPK_RoomOrEntArea, int iPK_RoomOrEntArea_Left);

	// If it ended because we're starting another stream, bSendOff==false so we don't turn everything off
	// If the new stream we're starting has bResume true, then bDeleteStream==false so we don't delete the stream since we'll be resuming it
	// If a new stream is replacing an old one, pMediaStream_Replacement is set to the new stream so we know if we should
	// fire the watching/listening events again.
	void StreamEnded(MediaStream *pMediaStream,bool bSendOff=true,bool bDeleteStream=true,MediaStream *pMediaStream_Replacement=NULL,vector<EntertainArea *> *p_vectEntertainArea=NULL,bool bNoAutoResume=false,bool bTurnOnOSD=true,bool bFireEvent=true);
	void MediaInEAEnded(EntertainArea *pEntertainArea,bool bFireEvent=true);

	virtual bool ReportPendingTasks(PendingTaskList *pPendingTaskList=NULL);

	void ShowMediaPlaybackState(EntertainArea *pEntertainArea);  // Update any VFD displays with the new playback state in this entertainment area

	// Given pMediaDevice, this function will find out if that device is presently being used by any media streams
	// and return a 1 if it's involved as the source, and 2 if the destination, or 0 if there's no involvement.
	// If it returns non-zero, pMediaStream will be set to the stream.  If it's a source, but not the direct source (ie fed by another)
	// pMediaDevice_Source will be the direct source.  The same is true with pMediaDevice_Dest
	// If it's involved in the destination, pEntertainArea will also be set
	int DetermineInvolvement(MediaDevice *pMediaDevice, MediaDevice *&pMediaDevice_Source,MediaDevice *&pMediaDevice_Dest,
		EntertainArea *&pEntertainArea,MediaStream *&pMediaStream);


	int GetStorageDeviceWithMostFreeSpace(string& sFullDescription, string& sMountedPath);
	string GetMRLFromDiscID( int PK_Disc,int PK_Device_Drive );
	void TransformFilenameToDeque(string sFilename,deque<MediaFile *> &dequeMediaFile);

	/**
	 * @brief Check a file and add its individual items to the deque if it is a playlist
	 * sFileName - check this file if it is a playlist
	 * MediaType - the mediatype to set for each added MediaFile, as we dont load and detect each and every one
	 * dequeMediaFile - The deque to add the playlist items to
	 * @returns true if the playlist was succesfully expanded into its components, false if not
	 */
	bool ExpandPlaylist(string sFilename, int MediaType, deque<MediaFile* > &dequeMediaFile);

	/*
	*	Since this class is so big, all the datagrid function have been moved to Media_Plugin_Grids.cpp
	*/
    class DataGridTable *MediaBrowser( string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage );
        bool IsGameMediaType(int iPK_MediaType);
	string GameMediaTypes();
	void AttributesBrowser( MediaListGrid *pMediaListGrid,int PK_MediaType, string sPK_Attribute, int PK_AttributeType_Sort, bool bShowFiles, string &sPK_MediaSubType, string &sPK_FileFormat, string &sPK_Attribute_Genres, string &sPK_Sources, string &sPK_Users_Private, int PK_Users, int iLastViewed, int *iPK_Variable, string *sValue_To_Assign );
	void FileBrowser( MediaListGrid *pMediaListGrid,int PK_MediaType, string &sPK_MediaSubType, string &sPK_FileFormat, string &sPK_Attribute_Genres, string &sSources, string &sPK_Users_Private, int PK_Users, int *iPK_Variable, string *sValue_To_Assign );
	void FetchPictures(string sWhichTable,string &sPK_File_Or_Disc,map<int,int> &mapFile_To_Pic,int PK_AttributeType_Sort);
	void PopulateFileBrowserInfoForFile(MediaListGrid *pMediaListGrid,int PK_AttributeType_Sort, bool bSubDirectory, string &sPath, string &sPK_File,map<int,int> &mapFile_To_Pic);
	void PopulateFileBrowserInfoForDisc(MediaListGrid *pMediaListGrid,int PK_AttributeType_Sort,string &sPK_Disk,map<int,int> &mapDisk_To_Pic);
	void PopulateFileBrowserInfoForAttribute(MediaListGrid *pMediaListGrid,int PK_AttributeType_Sort, int PK_Attribute,string &sPK_File_Or_Disc,string sTable);
	void PopulateFileBrowserInfoForPlayList(MediaListGrid *pMediaListGrid,string sPK_Users_Private);
	void PopulateFileBrowserInfoForBookmark(MediaListGrid *pMediaListGrid,string &sPK_File,string &sPK_Disc);
	void PopulateWithDatabaseInfoOnPath(map<string,DatabaseInfoOnPath *> &mapDatabaseInfoOnPath,string &sSearchPath); // helper for FileBrowser
    class DataGridTable *CurrentMedia( string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage );
    class DataGridTable *CurrentMediaSections( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
    class DataGridTable *MediaSections( string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage );
    class DataGridTable *MediaAttrXref( string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage );
    class DataGridTable *MediaAttrCollections( string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage );
    class DataGridTable *MediaAttrFiles( string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage );
    class DataGridTable *MediaSearchAutoCompl( string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage );
    class DataGridTable *MediaItemAttr( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
    class DataGridTable *DevicesPipes( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *MediaAttrFile( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *MediaAttrCurStream( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	void AddMediaSectionToDataGrid(DataGridTable *pDataGrid,int &iRow,deque<MediaSection *> &dequeMediaSection,string sPreface,bool bThumbnail=false);
	void AddMediaTitlesToDataGrid(DataGridTable *pDataGrid,int &iRow,deque<MediaTitle *> &dequeMediaTitle,string sPreface,bool bThumbnail=false);
	void DevicesPipes_Loop(int PK_Orbiter,DeviceData_Router *pDevice,DataGridTable *&pDataGrid,int &iRow,Command *pCommand_Input=NULL,Command *pCommand_Output=NULL,vector<int> *p_vectDevice=NULL);

	class DataGridTable *AvailablePlaylists( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
    class DataGridTable *FloorplanMediaChoices( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
    class DataGridTable *ActiveMediaStreams( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );

    class DataGridTable *DVDSubtitles( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
    class DataGridTable *DVDAudioTracks( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
    class DataGridTable *DVDAngles( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
    class DataGridTable *Bookmarks( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *BookmarksByMediaType( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *CaptureCardPorts( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *CaptureCardAudioPorts( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *DevicesForCaptureCardPort( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *DevicesNeedingProviders( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *ProvidersForDevice( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *ThumbnailableAttributes( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *CDTracks( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );

	bool CanThumbnail(int PK_AttributeType);
	void ReleaseDriveLock(MediaStream *pMediaStream);  // When playing media from a disk drive, we issue a CMD_Lock to block others from using the drive.  Call this to release any lock
	bool AssignDriveForDisc(MediaStream *pMediaStream,MediaFile *pMediaFile);  // If we're playing a disk, be sure we fill in a drive that can be used to play the disc, since DiscLocation means copies of the disc can be in multiple drives

	/*
	*	Above functions in Media_Plugin_Grids.cpp
	*/


	void SetNothingNowPlaying(OH_Orbiter *pOH_Orbiter, bool bRefreshScreen, bool bGotoRemote=false, Message *pMessage=NULL) // Call when nothing is playing
	{
		DCE::CMD_Set_Now_Playing CMD_Set_Now_Playing( m_dwPK_Device, pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device, 
			"", "", "", 0, 0, 0, "", "", bRefreshScreen);
		if( pMessage )
		{
			pMessage->m_vectExtraMessages.push_back(CMD_Set_Now_Playing.m_pMessage);
			if( bGotoRemote )
			{
				DCE::CMD_Goto_Screen CMD_Goto_Screen(m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,"",SCREEN_Main_CONST,interuptAlways,false,false);
				pMessage->m_vectExtraMessages.push_back(CMD_Goto_Screen.m_pMessage);
			}
		}
		else
		{
			if( bGotoRemote )
			{
				DCE::CMD_Goto_Screen CMD_Goto_Screen(m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,"",SCREEN_Main_CONST,interuptAlways,false,false);
				CMD_Set_Now_Playing.m_pMessage->m_vectExtraMessages.push_back(CMD_Goto_Screen.m_pMessage);
			}
			SendCommand( CMD_Set_Now_Playing );
		}
	}
	// Get the remote control set for this stream.  If PK_MediaType is specified, use that media type instead, which
	// is used so that a stored video can have titles and be treated as a dvd
	RemoteControlSet *GetRemoteControlSet(int dwPK_Device, MediaStream *pMediaStream,int PK_MediaType=0)
	{
		RemoteControlSet *pRemoteControlSet = NULL;
		if( !PK_MediaType )  // If a media type was specified we'll use that instead.  Otherwise we'll store the remote control sets with the stream
			pRemoteControlSet = pMediaStream->m_mapRemoteControlSet[dwPK_Device];
		if( !pRemoteControlSet )
		{
			pRemoteControlSet = PickRemoteControlMap(
				dwPK_Device,
				pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_DeviceTemplate,
				PK_MediaType ? PK_MediaType : pMediaStream->m_iPK_MediaType);
			if( !pRemoteControlSet )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::SetNowPlaying Cannot find a remote for %d %d %d",
					dwPK_Device,pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_DeviceTemplate,
					pMediaStream->m_iPK_MediaType);
				return NULL;
			}
			if( !PK_MediaType )  // If a media type was specified, this is just temporary.  Don't store it
				pMediaStream->m_mapRemoteControlSet[dwPK_Device]=pRemoteControlSet;
		}
		return pRemoteControlSet;
	}

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_Type();
	string DATA_Get_Default_Repeat();
	void DATA_Set_Default_Repeat(string Value,bool bUpdateDatabase=false);

			*****EVENT***** accessors inherited from base class
	void EVENT_Watching_Media(int iPK_Room);
	void EVENT_Stopped_Watching_Media(int iPK_Room);
	void EVENT_Listening_to_Media(int iPK_Room);
	void EVENT_Stopped_Listening_To_Medi(int iPK_Room);

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #43 - MH Play Media */
	/** The Orbiters send the play media command to the actual media handler. rnrnThe Orbiter can send anyone or a combination of parameters. rnrnIt's up to media handler to figure out how to handle it. The media handler must find out if the media is already pla */
		/** @param #2 PK_Device */
			/** The ID of the actual device to start playing. */
		/** @param #13 Filename */
			/** The filename to play or a pipe delimited list of filenames. */
		/** @param #29 PK_MediaType */
			/** The ID of the media type descriptor (if it is TV, movie, audio, etc ..  ) */
		/** @param #44 PK_DeviceTemplate */
			/** The DeviceTemplate ID. */
		/** @param #45 PK_EntertainArea */
			/** The desired target area for the playback. If this is missing then the orbiter should decide the target based on his controlled area. */
		/** @param #116 Resume */
			/** If true, when this media finishes, resume whatever was playing previously.  Useful for making announcements and similar. */
		/** @param #117 Repeat */
			/** 0=default for media type, 1=loop, -1=do not loop */
		/** @param #253 Queue */
			/** If true, the file won't be played unless the queue is empty.  It will be appended to the queue only */
		/** @param #254 Bypass Event */
			/** If true, the 'listening to media' event won't be fire. */
		/** @param #276 Dont Setup AV */
			/** If true, the on/input selects won't be sent to the a/v gear */

	virtual void CMD_MH_Play_Media(int iPK_Device,string sFilename,int iPK_MediaType,int iPK_DeviceTemplate,string sPK_EntertainArea,bool bResume,int iRepeat,bool bQueue,bool bBypass_Event,bool bDont_Setup_AV) { string sCMD_Result; CMD_MH_Play_Media(iPK_Device,sFilename.c_str(),iPK_MediaType,iPK_DeviceTemplate,sPK_EntertainArea.c_str(),bResume,iRepeat,bQueue,bBypass_Event,bDont_Setup_AV,sCMD_Result,NULL);};
	virtual void CMD_MH_Play_Media(int iPK_Device,string sFilename,int iPK_MediaType,int iPK_DeviceTemplate,string sPK_EntertainArea,bool bResume,int iRepeat,bool bQueue,bool bBypass_Event,bool bDont_Setup_AV,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #44 - MH Stop Media */
	/** Stop media. All parameters are optional. The Media handler must find out what media to stop. */
		/** @param #2 PK_Device */
			/** The specific device to stop media on. */
		/** @param #29 PK_MediaType */
			/** The type of media to stop. */
		/** @param #44 PK_DeviceTemplate */
			/** The type of device to stop the media on. */
		/** @param #45 PK_EntertainArea */
			/** This is the location on which we need to stop the media. This is optional. If not specified the orbiter will decide the location based on the controlled area. */
		/** @param #254 Bypass Event */
			/** If true, the usual event for 'Stopped Watching Media' won't be fired */

	virtual void CMD_MH_Stop_Media(int iPK_Device,int iPK_MediaType,int iPK_DeviceTemplate,string sPK_EntertainArea,bool bBypass_Event) { string sCMD_Result; CMD_MH_Stop_Media(iPK_Device,iPK_MediaType,iPK_DeviceTemplate,sPK_EntertainArea.c_str(),bBypass_Event,sCMD_Result,NULL);};
	virtual void CMD_MH_Stop_Media(int iPK_Device,int iPK_MediaType,int iPK_DeviceTemplate,string sPK_EntertainArea,bool bBypass_Event,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** This will allow an orbiter to change the current playing position in the playlist */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID) { string sCMD_Result; CMD_Jump_Position_In_Playlist(sValue_To_Assign.c_str(),iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #74 - Bind to Media Remote */
	/** When an orbiter goes to a media remote control screen, it fires this command so that the media plug-in knows it is sitting at a remote, and needs to be notified when the media changes or the cover art changes.  This should be in the onLoad commands of eve */
		/** @param #2 PK_Device */
			/** The device the orbiter is controlling. */
		/** @param #3 PK_DesignObj */
			/** The object where the remote displays the graphical image of the cover art.  It will get update object images when the cover art changes. */
		/** @param #8 On/Off */
			/** If 1, bind (the Orbiter is sitting at the remote screen).  If 0, the orbiter has left the remote screen, and does not need media changed commands. */
		/** @param #39 Options */
			/** Miscellaneous options.  These are not pre-defined, but are specific to a remote and the plug-in.  For example, the PVR plug-in needs to know what tuning device is active. */
		/** @param #45 PK_EntertainArea */
			/** The entertainment area the orbiter is controlling. */
		/** @param #63 PK_Text_Synopsis */
			/** The text object for the synopsis, a full description.  Examples are a DVD synopsis, or a description of a tv show. */
		/** @param #159 PK_Screen */
			/** The current screen. */

	virtual void CMD_Bind_to_Media_Remote(int iPK_Device,string sPK_DesignObj,string sOnOff,string sOptions,string sPK_EntertainArea,int iPK_Text_Synopsis,int iPK_Screen) { string sCMD_Result; CMD_Bind_to_Media_Remote(iPK_Device,sPK_DesignObj.c_str(),sOnOff.c_str(),sOptions.c_str(),sPK_EntertainArea.c_str(),iPK_Text_Synopsis,iPK_Screen,sCMD_Result,NULL);};
	virtual void CMD_Bind_to_Media_Remote(int iPK_Device,string sPK_DesignObj,string sOnOff,string sOptions,string sPK_EntertainArea,int iPK_Text_Synopsis,int iPK_Screen,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #214 - Save playlist */
	/** This will instruct the device to save the currently playing list */
		/** @param #17 PK_Users */
			/** The user that will own the new playlist. Can be missing. It will pick the current user then. */
		/** @param #45 PK_EntertainArea */
			/** Which playlist to save. You can direct the command to save a specific entertainment area's playlist or you can leave it blank to pick the current entertainment area's playlist */
		/** @param #50 Name */
			/** It will use the this name when saving. If it is not specified it will either use the name of the loaded playlist in the database or it will generate a new one. */
		/** @param #77 Save as new */
			/** Save the playlist as a new playlist. This will override the default behaviour. (If this playlist was not loaded from the database it will be saved as new. If it was loaded it will be overridded). This will make it always save it as new. */

	virtual void CMD_Save_playlist(int iPK_Users,string sPK_EntertainArea,string sName,bool bSave_as_new) { string sCMD_Result; CMD_Save_playlist(iPK_Users,sPK_EntertainArea.c_str(),sName.c_str(),bSave_as_new,sCMD_Result,NULL);};
	virtual void CMD_Save_playlist(int iPK_Users,string sPK_EntertainArea,string sName,bool bSave_as_new,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #231 - Load Playlist */
	/** This will instruct the device to load the specific playlist. */
		/** @param #45 PK_EntertainArea */
			/** The entertainment area in which to load the  playlist. By defualt it will be the entertainment in which the current orbiter is running. */
		/** @param #78 EK_Playlist */
			/** The id of the playlist to load */

	virtual void CMD_Load_Playlist(string sPK_EntertainArea,int iEK_Playlist) { string sCMD_Result; CMD_Load_Playlist(sPK_EntertainArea.c_str(),iEK_Playlist,sCMD_Result,NULL);};
	virtual void CMD_Load_Playlist(string sPK_EntertainArea,int iEK_Playlist,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #241 - MH Move Media */
	/** Moves an existing media stream to the specified entertainment areas. */
		/** @param #41 StreamID */
			/** The ID of the media stream to move */
		/** @param #45 PK_EntertainArea */
			/** The entertainment area(s) to send the Media Stream to. */

	virtual void CMD_MH_Move_Media(int iStreamID,string sPK_EntertainArea) { string sCMD_Result; CMD_MH_Move_Media(iStreamID,sPK_EntertainArea.c_str(),sCMD_Result,NULL);};
	virtual void CMD_MH_Move_Media(int iStreamID,string sPK_EntertainArea,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #269 - Move Playlist entry Up */
	/** Moves a entry up in the current playlist. */
		/** @param #48 Value */
			/** The id of the entry that needs to be moved up. */

	virtual void CMD_Move_Playlist_entry_Up(int iValue) { string sCMD_Result; CMD_Move_Playlist_entry_Up(iValue,sCMD_Result,NULL);};
	virtual void CMD_Move_Playlist_entry_Up(int iValue,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #270 - Move Playlist entry Down */
	/** Moves a entry down in the current playlist. */
		/** @param #48 Value */
			/** The id of the entry that needs to be moved down in the playlist. */

	virtual void CMD_Move_Playlist_entry_Down(int iValue) { string sCMD_Result; CMD_Move_Playlist_entry_Down(iValue,sCMD_Result,NULL);};
	virtual void CMD_Move_Playlist_entry_Down(int iValue,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #271 - Remove playlist entry. */
	/** Removes an entry from the playlist. */
		/** @param #48 Value */
			/** The Id of the entry that needs to be removed from the playlist. */

	virtual void CMD_Remove_playlist_entry(int iValue) { string sCMD_Result; CMD_Remove_playlist_entry(iValue,sCMD_Result,NULL);};
	virtual void CMD_Remove_playlist_entry(int iValue,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #331 - Get EntAreas For Device */
	/** Returns the EntArea(s) that a given device is in. */
		/** @param #2 PK_Device */
			/** The Device */
		/** @param #9 Text */
			/** A comma delimted list of EntAreas */

	virtual void CMD_Get_EntAreas_For_Device(int iPK_Device,string *sText) { string sCMD_Result; CMD_Get_EntAreas_For_Device(iPK_Device,sText,sCMD_Result,NULL);};
	virtual void CMD_Get_EntAreas_For_Device(int iPK_Device,string *sText,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #337 - Rip Disk */
	/** This will try to RIP a DVD to the HDD. */
		/** @param #2 PK_Device */
			/** The ID of the disk drive or jukebox */
		/** @param #13 Filename */
			/** The target disk name, or for cd's, a comma-delimited list of names for each track. */
		/** @param #17 PK_Users */
			/** The user who needs this rip in his private area. */
		/** @param #20 Format */
			/** wav, flac, ogg, etc. */
		/** @param #121 Tracks */
			/** For CD's, this must be a comma-delimted list of tracks (1 based) to rip. */
		/** @param #131 EK_Disc */
			/** The ID of the disc to rip.  If not specified this will be whatever disc is currently playing the entertainment area. */
		/** @param #151 Slot Number */
			/** The slot if this is a jukebox */
		/** @param #233 DriveID */
			/** The PK_Device ID of the storage drive that will be ripped to. Can be the ID of the core to store in /home */
		/** @param #234 Directory */
			/** The relative directory for the file to rip */

	virtual void CMD_Rip_Disk(int iPK_Device,string sFilename,int iPK_Users,string sFormat,string sTracks,int iEK_Disc,int iSlot_Number,int iDriveID,string sDirectory) { string sCMD_Result; CMD_Rip_Disk(iPK_Device,sFilename.c_str(),iPK_Users,sFormat.c_str(),sTracks.c_str(),iEK_Disc,iSlot_Number,iDriveID,sDirectory.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Rip_Disk(int iPK_Device,string sFilename,int iPK_Users,string sFormat,string sTracks,int iEK_Disc,int iSlot_Number,int iDriveID,string sDirectory,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #372 - MH Set Volume */
	/** Set the volume */
		/** @param #45 PK_EntertainArea */
			/** The Entertainment Area(s) */
		/** @param #76 Level */
			/** The level as an abolute value from 0-100, or a relative value like -1 or +1. */

	virtual void CMD_MH_Set_Volume(string sPK_EntertainArea,string sLevel) { string sCMD_Result; CMD_MH_Set_Volume(sPK_EntertainArea.c_str(),sLevel.c_str(),sCMD_Result,NULL);};
	virtual void CMD_MH_Set_Volume(string sPK_EntertainArea,string sLevel,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #388 - Set Media Private */
	/** Indicate if the media in an entertainment area is private or not. */
		/** @param #45 PK_EntertainArea */
			/** The entertainment area */
		/** @param #119 True/False */
			/** True if the media in this area is public (the default), false if it's not */

	virtual void CMD_Set_Media_Private(string sPK_EntertainArea,bool bTrueFalse) { string sCMD_Result; CMD_Set_Media_Private(sPK_EntertainArea.c_str(),bTrueFalse,sCMD_Result,NULL);};
	virtual void CMD_Set_Media_Private(string sPK_EntertainArea,bool bTrueFalse,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #391 - Add Media Attribute */
	/** Changes the media attribute */
		/** @param #5 Value To Assign */
			/** The new value of the attribute */
		/** @param #41 StreamID */
			/** The ID of the stream */
		/** @param #121 Tracks */
			/** If empty, the attribute is for the disc.  If specified, it is for this track number */
		/** @param #122 EK_AttributeType */
			/** The type of attribute to set */
		/** @param #123 EK_Attribute */
			/** The ID of the attribute */
		/** @param #135 Section */
			/** If specified the attribute is added for this section only */
		/** @param #145 EK_File */
			/** The file to add the attribute for.  If not specified, then a stream ID must be specified and the current file in that stream will be used */

	virtual void CMD_Add_Media_Attribute(string sValue_To_Assign,int iStreamID,string sTracks,int iEK_AttributeType,string sSection,int iEK_File,int *iEK_Attribute) { string sCMD_Result; CMD_Add_Media_Attribute(sValue_To_Assign.c_str(),iStreamID,sTracks.c_str(),iEK_AttributeType,sSection.c_str(),iEK_File,iEK_Attribute,sCMD_Result,NULL);};
	virtual void CMD_Add_Media_Attribute(string sValue_To_Assign,int iStreamID,string sTracks,int iEK_AttributeType,string sSection,int iEK_File,int *iEK_Attribute,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #392 - Set Media Attribute Text */
	/** Updates the text for an attribute */
		/** @param #5 Value To Assign */
			/** The new value.  If it's a name, LastName^Firstname format */
		/** @param #123 EK_Attribute */
			/** The attribute */

	virtual void CMD_Set_Media_Attribute_Text(string sValue_To_Assign,int iEK_Attribute) { string sCMD_Result; CMD_Set_Media_Attribute_Text(sValue_To_Assign.c_str(),iEK_Attribute,sCMD_Result,NULL);};
	virtual void CMD_Set_Media_Attribute_Text(string sValue_To_Assign,int iEK_Attribute,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #393 - Get Attribute */
	/** Returns the attribute text for the given ID */
		/** @param #9 Text */
			/** The value */
		/** @param #123 EK_Attribute */
			/** The attribute */

	virtual void CMD_Get_Attribute(int iEK_Attribute,string *sText) { string sCMD_Result; CMD_Get_Attribute(iEK_Attribute,sText,sCMD_Result,NULL);};
	virtual void CMD_Get_Attribute(int iEK_Attribute,string *sText,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #409 - Save Bookmark */
	/** Save the current position as a bookmark */
		/** @param #17 PK_Users */
			/** The user to save this under, or 0 for public */
		/** @param #19 Data */
			/** The picture to save as the thumbnail, in jpg format.  If not specified the plugin will try to grab a frame from the media player */
		/** @param #29 PK_MediaType */
			/** The media type, if not specified it will get it from the stream */
		/** @param #45 PK_EntertainArea */
			/** The entertainment area with the media */
		/** @param #163 Description */
			/** The description for this bookmark. */
		/** @param #188 Position */
			/** The position to save as.  For TV, CHAN: or PROG: or SERIES:.  Leave blank and the plugin will grab the current value */
		/** @param #225 Always */
			/** If true, then this is the start position */

	virtual void CMD_Save_Bookmark(int iPK_Users,char *pData,int iData_Size,int iPK_MediaType,string sPK_EntertainArea,string sDescription,string sPosition,bool bAlways) { string sCMD_Result; CMD_Save_Bookmark(iPK_Users,pData,iData_Size,iPK_MediaType,sPK_EntertainArea.c_str(),sDescription.c_str(),sPosition.c_str(),bAlways,sCMD_Result,NULL);};
	virtual void CMD_Save_Bookmark(int iPK_Users,char *pData,int iData_Size,int iPK_MediaType,string sPK_EntertainArea,string sDescription,string sPosition,bool bAlways,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #410 - Delete Bookmark */
	/** Delete a bookmark */
		/** @param #129 EK_Bookmark */
			/** The bookmark to delete */

	virtual void CMD_Delete_Bookmark(int iEK_Bookmark) { string sCMD_Result; CMD_Delete_Bookmark(iEK_Bookmark,sCMD_Result,NULL);};
	virtual void CMD_Delete_Bookmark(int iEK_Bookmark,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #411 - Rename Bookmark */
	/** Rename a bookmark */
		/** @param #5 Value To Assign */
			/** The new name */
		/** @param #17 PK_Users */
			/** The user, if this is a private bookmark */
		/** @param #129 EK_Bookmark */
			/** The bookmark */

	virtual void CMD_Rename_Bookmark(string sValue_To_Assign,int iPK_Users,int iEK_Bookmark) { string sCMD_Result; CMD_Rename_Bookmark(sValue_To_Assign.c_str(),iPK_Users,iEK_Bookmark,sCMD_Result,NULL);};
	virtual void CMD_Rename_Bookmark(string sValue_To_Assign,int iPK_Users,int iEK_Bookmark,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #412 - Set Media Position */
	/** Position the current media to a bookmark */
		/** @param #41 StreamID */
			/** The stream to set */
		/** @param #42 MediaPosition */
			/** The media position.  When MediaPlugin gets this, it will be a bookmark ID, when a media player gets it, the string */

	virtual void CMD_Set_Media_Position(int iStreamID,string sMediaPosition) { string sCMD_Result; CMD_Set_Media_Position(iStreamID,sMediaPosition.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Media_Position(int iStreamID,string sMediaPosition,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #417 - Set Auto Resume Options */
	/** Specify if the user should be prompted to resume the given media type or not */
		/** @param #5 Value To Assign */
			/** Valid values are: NEVER, ALWAYS, PROMPT */
		/** @param #17 PK_Users */
			/** The user to set */
		/** @param #29 PK_MediaType */
			/** The media type */

	virtual void CMD_Set_Auto_Resume_Options(string sValue_To_Assign,int iPK_Users,int iPK_MediaType) { string sCMD_Result; CMD_Set_Auto_Resume_Options(sValue_To_Assign.c_str(),iPK_Users,iPK_MediaType,sCMD_Result,NULL);};
	virtual void CMD_Set_Auto_Resume_Options(string sValue_To_Assign,int iPK_Users,int iPK_MediaType,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #623 - Shuffle */
	/** Randomizes the order of the current playlist. */
		/** @param #45 PK_EntertainArea */
			/** The Entertainment Area to send the shuffle command to. If not specified, This is assumed to come from an orbiter, and will be automatically determined. */

	virtual void CMD_Shuffle(string sPK_EntertainArea) { string sCMD_Result; CMD_Shuffle(sPK_EntertainArea.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Shuffle(string sPK_EntertainArea,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #742 - Media Identified */
	/** A disc has been identified */
		/** @param #2 PK_Device */
			/** The disk drive */
		/** @param #5 Value To Assign */
			/** The identified data */
		/** @param #10 ID */
			/** The ID of the disc */
		/** @param #19 Data */
			/** The picture/cover art */
		/** @param #20 Format */
			/** The format of the data */
		/** @param #29 PK_MediaType */
			/** The type of media */
		/** @param #59 MediaURL */
			/** The URL for the disc drive */
		/** @param #131 EK_Disc */
			/** If a disc was added accordingly, this reports the disc id */
		/** @param #193 URL */
			/** The URL for the picture */

	virtual void CMD_Media_Identified(int iPK_Device,string sValue_To_Assign,string sID,char *pData,int iData_Size,string sFormat,int iPK_MediaType,string sMediaURL,string sURL,int *iEK_Disc) { string sCMD_Result; CMD_Media_Identified(iPK_Device,sValue_To_Assign.c_str(),sID.c_str(),pData,iData_Size,sFormat.c_str(),iPK_MediaType,sMediaURL.c_str(),sURL.c_str(),iEK_Disc,sCMD_Result,NULL);};
	virtual void CMD_Media_Identified(int iPK_Device,string sValue_To_Assign,string sID,char *pData,int iData_Size,string sFormat,int iPK_MediaType,string sMediaURL,string sURL,int *iEK_Disc,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #780 - Remove playlist */
	/** Removes a playlist from database */
		/** @param #78 EK_Playlist */
			/** The id of the playlist to be removed. */

	virtual void CMD_Remove_playlist(int iEK_Playlist) { string sCMD_Result; CMD_Remove_playlist(iEK_Playlist,sCMD_Result,NULL);};
	virtual void CMD_Remove_playlist(int iEK_Playlist,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #807 - Get Attributes For Media */
	/** Get the attributes for a file */
		/** @param #5 Value To Assign */
			/** A tab delimited list of attributes: Attribute type \t Name \t type ... */
		/** @param #13 Filename */
			/** The file to retrieve the attributes for.  Can be a filename, or !F + PK_File, or a URL.  If not specified, specify the entertainment area */
		/** @param #45 PK_EntertainArea */
			/** Gets the currently active media in this entertainment area.  If not specified, specify file */

	virtual void CMD_Get_Attributes_For_Media(string sFilename,string sPK_EntertainArea,string *sValue_To_Assign) { string sCMD_Result; CMD_Get_Attributes_For_Media(sFilename.c_str(),sPK_EntertainArea.c_str(),sValue_To_Assign,sCMD_Result,NULL);};
	virtual void CMD_Get_Attributes_For_Media(string sFilename,string sPK_EntertainArea,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #817 - Get Default Ripping Info */
	/** Get default ripping info: default filename, id and name of the storage device with most free space. */
		/** @param #13 Filename */
			/** Default ripping name. */
		/** @param #53 UseDefault */
			/** If true, use the default directory structure for public/private */
		/** @param #131 EK_Disc */
			/** The disc to rip.  If not specified, it will be whatever is playing in the entertainment area that sent this */
		/** @param #219 Path */
			/** Base path for ripping. */
		/** @param #233 DriveID */
			/** The id of the storage device with most free space. */
		/** @param #234 Directory */
			/** The directory for this, such as video, audio, etc. */
		/** @param #235 Storage Device Name */
			/** The name of the storage device with most free space. */

	virtual void CMD_Get_Default_Ripping_Info(int iEK_Disc,string *sFilename,bool *bUseDefault,string *sPath,int *iDriveID,string *sDirectory,string *sStorage_Device_Name) { string sCMD_Result; CMD_Get_Default_Ripping_Info(iEK_Disc,sFilename,bUseDefault,sPath,iDriveID,sDirectory,sStorage_Device_Name,sCMD_Result,NULL);};
	virtual void CMD_Get_Default_Ripping_Info(int iEK_Disc,string *sFilename,bool *bUseDefault,string *sPath,int *iDriveID,string *sDirectory,string *sStorage_Device_Name,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #819 - Get ID from Filename */
	/** Given a filename, get the ID for the file.  The ID will be 0 if the indicated file does not exist */
		/** @param #13 Filename */
			/** The file to get the ID for */
		/** @param #145 EK_File */
			/** The file id */

	virtual void CMD_Get_ID_from_Filename(string sFilename,int *iEK_File) { string sCMD_Result; CMD_Get_ID_from_Filename(sFilename.c_str(),iEK_File,sCMD_Result,NULL);};
	virtual void CMD_Get_ID_from_Filename(string sFilename,int *iEK_File,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #823 - Specify Media Provider */
	/** Specify the media provider for a device */
		/** @param #2 PK_Device */
			/** The device to set the provider for */
		/** @param #9 Text */
			/** The media providers information */
		/** @param #163 Description */
			/** A description for this provider */

	virtual void CMD_Specify_Media_Provider(int iPK_Device,string sText,string sDescription) { string sCMD_Result; CMD_Specify_Media_Provider(iPK_Device,sText.c_str(),sDescription.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Specify_Media_Provider(int iPK_Device,string sText,string sDescription,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #825 - Specify Capture Card Port */
	/** Specify the capture card port for a device */
		/** @param #2 PK_Device */
			/** The device to set the port for */
		/** @param #201 PK_Device_Related */
			/** The capture card port */

	virtual void CMD_Specify_Capture_Card_Port(int iPK_Device,int iPK_Device_Related) { string sCMD_Result; CMD_Specify_Capture_Card_Port(iPK_Device,iPK_Device_Related,sCMD_Result,NULL);};
	virtual void CMD_Specify_Capture_Card_Port(int iPK_Device,int iPK_Device_Related,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #831 - Refresh List of Online Devices */
	/** Send this command when the list of devices that are online/offline changes */

	virtual void CMD_Refresh_List_of_Online_Devices() { string sCMD_Result; CMD_Refresh_List_of_Online_Devices(sCMD_Result,NULL);};
	virtual void CMD_Refresh_List_of_Online_Devices(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #839 - Check For New Files */
	/** Check to see if there are any new files that have been picked up by UpdateMedia that we should do some post processing on */

	virtual void CMD_Check_For_New_Files() { string sCMD_Result; CMD_Check_For_New_Files(sCMD_Result,NULL);};
	virtual void CMD_Check_For_New_Files(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #845 - Delete File */
	/** Delete a file.  Can be a fully qualified filename, or a !F syntax */
		/** @param #13 Filename */
			/** The file to delete */

	virtual void CMD_Delete_File(string sFilename) { string sCMD_Result; CMD_Delete_File(sFilename.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Delete_File(string sFilename,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #846 - Make Thumbnail */
	/** Thumbnail a file or attribute */
		/** @param #13 Filename */
			/** Can be a fully qualified filename, or a !F+number, or !A+number for an attribute */
		/** @param #19 Data */
			/** The picture */

	virtual void CMD_Make_Thumbnail(string sFilename,char *pData,int iData_Size) { string sCMD_Result; CMD_Make_Thumbnail(sFilename.c_str(),pData,iData_Size,sCMD_Result,NULL);};
	virtual void CMD_Make_Thumbnail(string sFilename,char *pData,int iData_Size,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #847 - Live AV Path */
	/** Switch the given a/v device to use the live a/v path */
		/** @param #45 PK_EntertainArea */
			/** The entertainment area */
		/** @param #252 Turn On */
			/** If true, the audio/video inputs for direct viewing, not through the capture card, will be used */

	virtual void CMD_Live_AV_Path(string sPK_EntertainArea,bool bTurn_On) { string sCMD_Result; CMD_Live_AV_Path(sPK_EntertainArea.c_str(),bTurn_On,sCMD_Result,NULL);};
	virtual void CMD_Live_AV_Path(string sPK_EntertainArea,bool bTurn_On,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #868 - Retransmit A/V Commands */
	/** Resend a/v commands to the active video or audio output device */
		/** @param #9 Text */
			/** AP=Audio Power
AI=Audio Input
VP=Video Power
VI=Video Input */
		/** @param #45 PK_EntertainArea */
			/** The entertainment area */

	virtual void CMD_Retransmit_AV_Commands(string sText,string sPK_EntertainArea) { string sCMD_Result; CMD_Retransmit_AV_Commands(sText.c_str(),sPK_EntertainArea.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Retransmit_AV_Commands(string sText,string sPK_EntertainArea,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #871 - Update Ripping Status */
	/** Update the status of a ripping job */
		/** @param #9 Text */
			/** A text message */
		/** @param #13 Filename */
			/** The filename being ripped */
		/** @param #102 Time */
			/** How much longer in seconds it will take */
		/** @param #199 Status */
			/** The status: [p] in progress, [e]rror, [s]uccess */
		/** @param #256 Percent */
			/** The percentage of completion */
		/** @param #257 Task */
			/** The task id */
		/** @param #258 Job */
			/** The job id */

	virtual void CMD_Update_Ripping_Status(string sText,string sFilename,string sTime,string sStatus,int iPercent,string sTask,string sJob) { string sCMD_Result; CMD_Update_Ripping_Status(sText.c_str(),sFilename.c_str(),sTime.c_str(),sStatus.c_str(),iPercent,sTask.c_str(),sJob.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Update_Ripping_Status(string sText,string sFilename,string sTime,string sStatus,int iPercent,string sTask,string sJob,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #882 - Abort Task */
	/** Abort a pending task */
		/** @param #248 Parameter ID */
			/** The ID of the task to abort */

	virtual void CMD_Abort_Task(int iParameter_ID) { string sCMD_Result; CMD_Abort_Task(iParameter_ID,sCMD_Result,NULL);};
	virtual void CMD_Abort_Task(int iParameter_ID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #942 - Get Ripping Status */
	/** Get ripping status */
		/** @param #199 Status */
			/** Ripping status */

	virtual void CMD_Get_Ripping_Status(string *sStatus) { string sCMD_Result; CMD_Get_Ripping_Status(sStatus,sCMD_Result,NULL);};
	virtual void CMD_Get_Ripping_Status(string *sStatus,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #955 - Specify Repeat Options */
	/** Change the repeat option for an entertainment area */
		/** @param #45 PK_EntertainArea */
			/** The entertainment area */
		/** @param #117 Repeat */
			/** 0=none, 1=repeat queue, 2=repeat track */

	virtual void CMD_Specify_Repeat_Options(string sPK_EntertainArea,int iRepeat) { string sCMD_Result; CMD_Specify_Repeat_Options(sPK_EntertainArea.c_str(),iRepeat,sCMD_Result,NULL);};
	virtual void CMD_Specify_Repeat_Options(string sPK_EntertainArea,int iRepeat,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1083 - Move File */
	/** Move a file from one place to another. */
		/** @param #13 Filename */
			/** !F notation referring to PK_File entry in the pluto_media File table. */
		/** @param #219 Path */
			/** The fully qualified destination Path and Filename */

	virtual void CMD_Move_File(string sFilename,string sPath) { string sCMD_Result; CMD_Move_File(sFilename.c_str(),sPath.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Move_File(string sFilename,string sPath,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1088 - Specify Capture Card Audio Port */
	/** Specify the capture card audio port for a device */
		/** @param #2 PK_Device */
			/** The Device to set the capture card audio port */
		/** @param #201 PK_Device_Related */
			/** The Device to set the capture card audio port to */

	virtual void CMD_Specify_Capture_Card_Audio_Port(int iPK_Device,int iPK_Device_Related) { string sCMD_Result; CMD_Specify_Capture_Card_Audio_Port(iPK_Device,iPK_Device_Related,sCMD_Result,NULL);};
	virtual void CMD_Specify_Capture_Card_Audio_Port(int iPK_Device,int iPK_Device_Related,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1089 - Update Move Status */
	/** Update the Status of a Move Task */
		/** @param #9 Text */
			/** A Text message to return to the Task. */
		/** @param #102 Time */
			/** Time in Seconds remaining. */
		/** @param #199 Status */
			/** The status: [p] in progress, [e]rror, [s]uccess */
		/** @param #256 Percent */
			/** The Percentage to update. */
		/** @param #257 Task */
			/** Task ID to update */
		/** @param #258 Job */
			/** Job ID to update */

	virtual void CMD_Update_Move_Status(string sText,string sTime,string sStatus,int iPercent,string sTask,string sJob) { string sCMD_Result; CMD_Update_Move_Status(sText.c_str(),sTime.c_str(),sStatus.c_str(),iPercent,sTask.c_str(),sJob.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Update_Move_Status(string sText,string sTime,string sStatus,int iPercent,string sTask,string sJob,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1090 - Get Attribute Image */
	/** Returns PK_Image file for supplied !A */

	virtual void CMD_Get_Attribute_Image() { string sCMD_Result; CMD_Get_Attribute_Image(sCMD_Result,NULL);};
	virtual void CMD_Get_Attribute_Image(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1091 - Transcode File */
	/** Transcode a file */
		/** @param #13 Filename */
			/** Source Filename */
		/** @param #20 Format */
			/** Format parameters key=value separated by | */
		/** @param #50 Name */
			/** Destination Filename */
		/** @param #234 Directory */
			/** Destination Directory */

	virtual void CMD_Transcode_File(string sFilename,string sFormat,string sName,string sDirectory) { string sCMD_Result; CMD_Transcode_File(sFilename.c_str(),sFormat.c_str(),sName.c_str(),sDirectory.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Transcode_File(string sFilename,string sFormat,string sName,string sDirectory,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1092 - Update Transcode Status */
	/** Update the status of a transcode job. */
		/** @param #9 Text */
			/** STatus text to return to task */
		/** @param #102 Time */
			/** Time remaining */
		/** @param #199 Status */
			/** Status e s f */
		/** @param #256 Percent */
			/** Percent update for task */
		/** @param #257 Task */
			/** Task number */
		/** @param #258 Job */
			/** Job Number */

	virtual void CMD_Update_Transcode_Status(string sText,string sTime,string sStatus,int iPercent,string sTask,string sJob) { string sCMD_Result; CMD_Update_Transcode_Status(sText.c_str(),sTime.c_str(),sStatus.c_str(),iPercent,sTask.c_str(),sJob.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Update_Transcode_Status(string sText,string sTime,string sStatus,int iPercent,string sTask,string sJob,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1095 - Get Attribute Types */
	/** Get  attribute types */
		/** @param #9 Text */
			/** The attribute types in the format of: id:name<newline>id:name<newline> */
		/** @param #29 PK_MediaType */
			/** Media type to find attribute types for. If 0, will return all. */

	virtual void CMD_Get_Attribute_Types(int iPK_MediaType,string *sText) { string sCMD_Result; CMD_Get_Attribute_Types(iPK_MediaType,sText,sCMD_Result,NULL);};
	virtual void CMD_Get_Attribute_Types(int iPK_MediaType,string *sText,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1096 - Get Attributes For Type */
	/** Get the attributes for attribute type */
		/** @param #9 Text */
			/** The attributes for the specified attribute type in the format: id:name<newline>id:name<newline> */
		/** @param #122 EK_AttributeType */
			/** The attribute type to get the attributes for */

	virtual void CMD_Get_Attributes_For_Type(int iEK_AttributeType,string *sText) { string sCMD_Result; CMD_Get_Attributes_For_Type(iEK_AttributeType,sText,sCMD_Result,NULL);};
	virtual void CMD_Get_Attributes_For_Type(int iEK_AttributeType,string *sText,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1097 - Get File Formats */
	/** Get file formats for the specified media type. */
		/** @param #20 Format */
			/** Comma-delimited list of file formats. */
		/** @param #29 PK_MediaType */
			/** Media Type to get file formats for. If 0, get all. */

	virtual void CMD_Get_File_Formats(int iPK_MediaType,string *sFormat) { string sCMD_Result; CMD_Get_File_Formats(iPK_MediaType,sFormat,sCMD_Result,NULL);};
	virtual void CMD_Get_File_Formats(int iPK_MediaType,string *sFormat,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1098 - Get Media Sub Type */
	/** Get media sub types for specified media type. */
		/** @param #29 PK_MediaType */
			/** The Media Type to get sub types for. If 0, gets all. */
		/** @param #50 Name */
			/** Comma delimited list of media sub types. */

	virtual void CMD_Get_Media_Sub_Type(int iPK_MediaType,string *sName) { string sCMD_Result; CMD_Get_Media_Sub_Type(iPK_MediaType,sName,sCMD_Result,NULL);};
	virtual void CMD_Get_Media_Sub_Type(int iPK_MediaType,string *sName,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
};

class MediaSectionGrid
{
public:
	int m_iSort1,m_iSort2;
	class DataGridCell *m_pDataGridCell;

	MediaSectionGrid(int iSort1,int iSort2,DataGridCell *pDataGridCell) { m_pDataGridCell=pDataGridCell; m_iSort1=iSort1; m_iSort2=iSort2; }
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
