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
//<-dceag-d-b->
#ifndef VDRPlugin_h
#define VDRPlugin_h

//	DCE Implemenation for #1704 VDR-Plugin

#include "Gen_Devices/VDRPluginBase.h"
//<-dceag-d-e->

#include "DCE/PlainClientSocket.h"

#include "../Media_Plugin/Media_Plugin.h"
#include "../Media_Plugin/MediaStream.h"
#include "../Media_Plugin/MediaHandlerBase.h"
#include "VDRMediaStream.h"
class Row_Bookmark;

	struct VDRRecording
	{
		union
		{
			u_int64_t int64;
			struct
			{
				int channel_id;
				time_t StartTime;
			} time;
		} data;
	};

//<-dceag-decl-b->!
namespace DCE
{
	class VDRSource
	{
	public:
		VDRSource(int dwID,string sDescription) : m_dwID(dwID), m_sDescription(sDescription) {}

		int m_dwID;
		string m_sDescription;
	};

	typedef map<int,int> MapBookmark;  // Map of PK_Users to PK_Bookmarks 

	// A program is a unique episode of a series, or a unique showing if there's not a series
	class VDREpisode
	{
	public:
		class VDRSeries *m_pVDRSeries;  // The series, like the Simpsons, or if this is a one-time show, it points to the description like "Batman 2"
		string m_sID;
		string m_sDescription; // Bart learns to drive
		string m_sSynopsis;
		list<class VDRPerformer *> m_listVDRPerformer;

		VDREpisode(string sID,class VDRSeries *pVDRSeries) : m_sID(sID),m_pVDRSeries(pVDRSeries) {} 
	};

	// The series info i.e. "The Simpsons", "Batman 2"
	class VDRSeries
	{
	public:
		string m_sDescription; // i.e. "The Simpsons", "Batman 2"
		string m_sID;
		map<string,VDREpisode *> m_mapVDREpisode;
		VDREpisode *m_mapVDREpisode_Find(string sEpisodeID) { map<string,VDREpisode *>::iterator it = m_mapVDREpisode.find(sEpisodeID); return it==m_mapVDREpisode.end() ? NULL : (*it).second; }

		VDRSeries(string sID) : m_sID(sID) {}

		VDREpisode *GetNewEpisode(string sID)
		{
			VDREpisode *pVDREpisode = m_mapVDREpisode_Find(sID);
			if( !pVDREpisode )
				pVDREpisode = new VDREpisode(sID,this);
			return pVDREpisode;
		}
	};

	// A performer
	class VDRPerformer
	{
	public:
		string m_sName;
		list<VDREpisode *> m_listVDREpisode;
	};

	// A particular airing of a show (ie a channel & start time)
	class VDRProgramInstance
	{
	public:
		time_t m_tStartTime,m_tStopTime;
		class VDREpisode *m_pVDREpisode;
		VDRProgramInstance *m_pVDRProgramInstance_Next;
		int m_iID;

		VDRProgramInstance() : m_pVDRProgramInstance_Next(NULL), m_pVDREpisode(NULL), m_iID(0) {}

		string GetSynopsis()
		{
			if( m_pVDREpisode )
				return m_pVDREpisode->m_sSynopsis;
			else
				return "";
		}

		string GetSeriesId()
		{
			if( m_pVDREpisode && m_pVDREpisode->m_pVDRSeries )
				return m_pVDREpisode->m_pVDRSeries->m_sID;
			else
				return "";
		}

		string GetProgramId()
		{
			if( m_pVDREpisode )
				return m_pVDREpisode->m_sID;
			else
				return "";
		}

		string GetTitle()
		{
			if( m_pVDREpisode && m_pVDREpisode->m_pVDRSeries )
				return m_pVDREpisode->m_pVDRSeries->m_sDescription;
			else if( m_pVDREpisode )
				return m_pVDREpisode->m_sDescription;
			else
				return "";
		}
	};

	class VDRChannel
	{
	public:
		int m_dwChanNum;
		string m_sID;
		VDRSource *m_pVDRSource;
		VDRProgramInstance *m_pVDRProgramInstance_First;
		string m_sShortName,m_sLongName;
		char *m_pPic;
		size_t m_Pic_size;
		MapBookmark m_mapBookmark;  // Map of PK_Users to PK_Bookmarks who have bookmarked this
		DataGridCell *m_pCell;  // A temporary pointer only valid while created a grid

		VDRChannel(string sID,int dwChanNum,VDRSource *pVDRSource, string sShortName,string sLongName,char *pPic, size_t Pic_size)
		{
			m_pVDRSource=pVDRSource;
			m_sID=sID;
			m_dwChanNum=dwChanNum;
			m_sShortName=sShortName;
			m_sLongName=sLongName;
			m_pPic=pPic;
			m_Pic_size=Pic_size;
			m_pCell=NULL;
			m_pVDRProgramInstance_First=NULL;
		}

		~VDRChannel()
		{
			delete m_pPic;
			m_pPic=NULL;
			m_Pic_size=0;
		}

		class VDRProgramInstance *GetCurrentProgramInstance(time_t tTime)
		{
			VDRProgramInstance *pVDRProgramInstance = m_pVDRProgramInstance_First;
			while(pVDRProgramInstance && pVDRProgramInstance->m_tStopTime < tTime )
				pVDRProgramInstance = pVDRProgramInstance->m_pVDRProgramInstance_Next;

			if( !pVDRProgramInstance || pVDRProgramInstance->m_tStartTime>tTime )
				return NULL;
			return pVDRProgramInstance;
		}
	};

	typedef list<VDRChannel *> ListVDRChannel;
	class VDRPlugin : public VDRPlugin_Command, public MediaHandlerBase, public DataGridGeneratorPlugIn
	{
//<-dceag-decl-e->
		// Private member variables
		ListVDRChannel m_ListVDRChannel; // The channels in the right order for display
		map<string,VDRChannel *> m_mapVDRChannel; // The channels for quick searching
		VDRChannel *m_mapVDRChannel_Find(string sChannelID) { map<string,VDRChannel *>::iterator it = m_mapVDRChannel.find(sChannelID); return it==m_mapVDRChannel.end() ? NULL : (*it).second; }

		map<string,VDRSource *> m_mapVDRSource;
		VDRSource *m_mapVDRSource_Find(string sSourceID) { map<string,VDRSource *>::iterator it = m_mapVDRSource.find(sSourceID); return it==m_mapVDRSource.end() ? NULL : (*it).second; }

		map<string,VDRSeries *> m_mapVDRSeries;
		VDRSeries *m_mapVDRSeries_Find(string sSeriesID) { map<string,VDRSeries *>::iterator it = m_mapVDRSeries.find(sSeriesID); return it==m_mapVDRSeries.end() ? NULL : (*it).second; }

		string m_sVDRIp;

		class Orbiter_Plugin *m_pOrbiter_Plugin;
		class Datagrid_Plugin *m_pDatagrid_Plugin;
	    pluto_pthread_mutex_t m_VDRMutex; // Protect the maps
		pthread_cond_t m_VDRCond; /** < condition for the messages in the queue */

		bool m_bBookmarksNeedRefreshing;
		map<string,MapBookmark *> m_mapSeriesBookmarks,m_mapProgramBookmarks;  // What users have bookmarked series and programs
		MapBookmark *m_mapSeriesBookmarks_Find(string sSeries) { map<string,MapBookmark *>::iterator it = m_mapSeriesBookmarks.find(sSeries); return it==m_mapSeriesBookmarks.end() ? NULL : (*it).second; }
		MapBookmark *m_mapProgramBookmarks_Find(string sProgram) { map<string,MapBookmark *>::iterator it = m_mapProgramBookmarks.find(sProgram); return it==m_mapProgramBookmarks.end() ? NULL : (*it).second; }

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		VDRPlugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~VDRPlugin();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

		void PurgeChannelList();
		void BuildChannelList();
		void RefreshBookmarks();
		VDRSource *GetNewSource(string sSource);
		VDRSeries *GetNewSeries(string sSeriesID);
		bool MediaInfoChanged( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );
		bool PlaybackStarted( class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);

		virtual void PrepareToDelete();
		// Datagrids
		class DataGridTable *CurrentShows(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage);
		class DataGridTable *AllShows(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage);
		class DataGridTable *FavoriteChannels(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage);
		class DataGridTable *FavoriteShows(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage);
		class DataGridTable *OtherShowtimes(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage);

		// Utilities
		/*
		bool GetVdrAndEpgFromOrbiter(int PK_Device,class MediaDevice *&pMediaDevice_VDR,VDREPG::EPG *&pEPG,VDRMediaStream *&pVDRMediaStream);
		VDREPG::Event *GetStartingEvent(VDREPG::EPG *pEPG,int PK_Users);
		VDREPG::Event *GetEventForBookmark(VDREPG::EPG *pEPG,Row_Bookmark *pRow_Bookmark,int &iPriority_Bookmark);
		*/
		bool TuneToChannel( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );
		bool NewBookmarks( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );
		void UpdateTimers();

//<-dceag-const2-b->!
		/**
		* @brief
		*/
		virtual class MediaStream *CreateMediaStream( class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider, vector<class EntertainArea *> &vectEntertainArea, MediaDevice *pMediaDevice, int iPK_Users, deque<MediaFile *> *dequeFilenames, int StreamID );

		/**
		* @brief Start media playback
		*/
		virtual bool StartMedia( class MediaStream *pMediaStream,string &sError );

		/**
		* @brief Stop media playback
		*/
		virtual bool StopMedia( class MediaStream *pMediaStream );

		virtual MediaDevice *FindMediaDeviceForEntertainArea(EntertainArea *pEntertainArea);


//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	int DATA_Get_Priority();
	bool DATA_Get_Only_One_Per_PC();
	int DATA_Get_EK_Provider();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** Change channels.  +1 and -1 mean up and down 1 channel. */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID) { string sCMD_Result; CMD_Jump_Position_In_Playlist(sValue_To_Assign.c_str(),iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #185 - Schedule Recording */
	/** This will schedule a recording. */
		/** @param #14 Type */
			/** The type of recording: O=Once, C=Channel */
		/** @param #39 Options */
			/** Options for this recording, tbd later */
		/** @param #68 ProgramID */
			/** The program which will need to be recorded. (The format is defined by the device which created the original datagrid) */

	virtual void CMD_Schedule_Recording(string sType,string sOptions,string sProgramID) { string sCMD_Result; CMD_Schedule_Recording(sType.c_str(),sOptions.c_str(),sProgramID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Schedule_Recording(string sType,string sOptions,string sProgramID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #698 - Get Extended Media Data */
	/** Returns extra data about the given media, such as the title, airtime, whether it's currently scheduled to record, etc. */
		/** @param #3 PK_DesignObj */
			/** If specified the sender will be sent a goto-screen with this screen.  If not the sender will be sent a refresh */
		/** @param #68 ProgramID */
			/** If specified, the program to retrive info on.  If not specified, assumed to be the currently playing media */

	virtual void CMD_Get_Extended_Media_Data(string sPK_DesignObj,string sProgramID) { string sCMD_Result; CMD_Get_Extended_Media_Data(sPK_DesignObj.c_str(),sProgramID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Get_Extended_Media_Data(string sPK_DesignObj,string sProgramID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #764 - Set Active Menu */
	/** Indicate which menu is active, options are:
live, nonlive, osd */
		/** @param #9 Text */
			/** The menu currently active */

	virtual void CMD_Set_Active_Menu(string sText) { string sCMD_Result; CMD_Set_Active_Menu(sText.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Active_Menu(string sText,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #824 - Sync Providers and Cards */
	/** Synchronize settings for pvr cards and provders */
		/** @param #2 PK_Device */
			/** If specified, this is the capture card that triggered this change to call checktvproviders for */
		/** @param #198 PK_Orbiter */
			/** If specified, this is the orbiter to notify of the progress if this results in scanning for channels */

	virtual void CMD_Sync_Providers_and_Cards(int iPK_Device,int iPK_Orbiter) { string sCMD_Result; CMD_Sync_Providers_and_Cards(iPK_Device,iPK_Orbiter,sCMD_Result,NULL);};
	virtual void CMD_Sync_Providers_and_Cards(int iPK_Device,int iPK_Orbiter,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #846 - Make Thumbnail */
	/** Thumbnail the current frame */
		/** @param #13 Filename */
			/** Can be a fully qualified filename, or a !F+number, or !A+number for an attribute */
		/** @param #19 Data */
			/** The picture */

	virtual void CMD_Make_Thumbnail(string sFilename,char *pData,int iData_Size) { string sCMD_Result; CMD_Make_Thumbnail(sFilename.c_str(),pData,iData_Size,sCMD_Result,NULL);};
	virtual void CMD_Make_Thumbnail(string sFilename,char *pData,int iData_Size,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #910 - Reporting EPG Status */
	/** Reporting the status of an EPG update */
		/** @param #9 Text */
			/** Any messages about this */
		/** @param #40 IsSuccessful */
			/** true if the process succeeded */
		/** @param #257 Task */
			/** The type of EPG task: channel (retrieving channels), guide (retrieving guide) */

	virtual void CMD_Reporting_EPG_Status(string sText,bool bIsSuccessful,string sTask) { string sCMD_Result; CMD_Reporting_EPG_Status(sText.c_str(),bIsSuccessful,sTask.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Reporting_EPG_Status(string sText,bool bIsSuccessful,string sTask,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #911 - Remove Scheduled Recording */
	/** Remove a scheduled recording */
		/** @param #10 ID */
			/** The ID of the recording rule to remove.  This will remove all recordings with this ID, and ProgramID is ignored if this is specified. */
		/** @param #68 ProgramID */
			/** The ID of the program to remove.  If ID is empty, remove just this program. */

	virtual void CMD_Remove_Scheduled_Recording(string sID,string sProgramID) { string sCMD_Result; CMD_Remove_Scheduled_Recording(sID.c_str(),sProgramID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Remove_Scheduled_Recording(string sID,string sProgramID,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
