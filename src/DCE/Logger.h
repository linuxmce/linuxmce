/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU Lesser General Public License for more details.

*/

/** @file Logger.h
 Header file for the Logger, NullLogger and FileLogger classes.
 * @todo notcommented
 */


#ifndef DCELogger_H
#define DCELogger_H

#ifndef Q_OS_SYMBIAN
#include <stdio.h>
#endif

#include <pthread.h>
#if !defined(WIN32) && !defined(__APPLE_CC__) &&  !defined(Q_OS_SYMBIAN)
#include <features.h>
#endif

#include <string>
#include <set>
#include <queue>

#define LV_CRITICAL 		1
#define LV_WARNING 			5
#define LV_EVENTHANDLER 	6
#define LV_EVENT 			7
#define LV_ACTION 			8
#define LV_CRITERIA 		9
#define LV_STATUS 			10
#define LV_EPG				11
#define LV_ALARM			12
#define LV_SOCKET			13
#define LV_LIGHTING			14
#define LV_CONTROLLER		15
#define LV_APPDAEMON		16
#define LV_CAMERA			17
#define LV_CORPCLIENT		18
#define LV_FILEREQUEST		19
#define LV_DATAGRID			20
#define LV_LOCKING			21
#define LV_REGISTRATION		22
#define LV_XINE				23
#define LV_BURNER			24
#define LV_1394				25
#define LV_SPHINX			26
#define LV_FESTIVAL			27
#define LV_IR				28
#define LV_START			29
#define LV_START_DEVICE		30
#define LV_STOP				31
#define LV_MOUSE_CLICKS		32
#define LV_EPG_UPDATE		33
#define LV_MEDIA			34
#define LV_DEBUG			35
#define LV_ZWAVE			36
#define LV_PROFILER			37
#define LV_PROCESSUTILS		38
#define LV_TV				39
#define LV_SEND_DATA		40
#define LV_RECEIVE_DATA		41
#define LV_INSTEON			42
#define LV_COLOR			99

#include "SerializeClass/SerializeClass.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "PlutoLockLogger.h"

#define LT_LOGGER_NULL		 1
#define LT_LOGGER_FILE		 2
#define LT_LOGGER_SERVER	 3
#define LT_LOGGER_ORBITERWIN 4
#define LT_LOGGER_QT             5

namespace DCE
{
	/**
	* While global variables aren't recommended, every object within the same memory space
	* will be using the same logger anyway.  This is cleaner than passing a pointer to a
	* the logger around.  Each program that uses this will have to create the actual
	* instance of g_pDCELogger, using whatever derived class is appropriate.  Also, we'll put
	* 2 strings that the main() function can use to store the currently executing binary and path
	* which may be useful for logging
	*/
	extern string g_sBinary,g_sBinaryPath;

	/** @class Logger
	* Logger for the data command and events
	*/
	class Logger
	{

	protected:

		std::string m_Name; /**< name of the logger */
		int m_dwPK_Installation; /**< the installation */
		pluto_pthread_mutex_t m_Lock; /**< @todo ask */
		pthread_mutexattr_t m_MutexAttr; /**< @todo ask */
		bool *m_bLogLevels;
		int m_iMaxLogLevel;

	public:

		class Entry;

		/**
		* @brief default constructor
		*/
		Logger( const char* name = NULL );

		/**
		* @brief destructor
		*/
		virtual ~Logger();

		/**
		* @brief sets the value for the type of logger
		*/
		virtual int GetType()=0;

		/**
		* @brief sets the value for the m_dwPK_Installation member
		*/
		virtual void SetInstallation(int Installation);

		/**
		* @brief sets the name for logger
		*/
		virtual void SetName(const char* pcName);

		/**
		* @brief should be overriden to start the logger
		*/
		virtual void Start() {}

		/**
		* @brief should be overriden to flush the logger
		*/
		virtual void Flush() {}

		/**
		* @brief Write a block of data in the log
		*/
		virtual void WriteBlock(const char *pBlock, size_t sBlockLen )=0;

		/**
		* @brief an entry in a specific format depending on it's level
		* @todo ask
		*/
		virtual void Write( int iLevel, const char *pcFormat, ... );

		/**
		* @brief writes an entry to the log
		*/
		virtual void WriteEntry( class Logger::Entry& entry ) = 0;

		/**
		* @bried rotates the log
		*/
		virtual void Rotate() {}

		/**
		* @bried Reload the log levels from the conf file
		*/
		virtual void ReloadLogLevels();
		virtual void LogAll();  // Removes the log level filter

		/**
		* @brief an entry to the logger
		*/
		class Entry : public SerializeClass
		{

		public:

			int m_iLevel; /**< this entry's level > @todo ask*/
			timeval m_TimeStamp; /**< the time for the entry time */
			std::string m_sName; /**< entry name */
			std::string m_sData; /**< entry data */
			int m_dwPK_Device; /**< the device that generated the entry */

			Entry() {} /**< default (empty) constructor */

			/**
			* @brief constructor that assignes values to the data members
			*/
			Entry( int Level, timeval TimeStamp, string sName, string sData, int iPK_Device )
			{
				m_iLevel = Level; m_TimeStamp = TimeStamp;
				m_sName = sName; m_sData = sData; m_dwPK_Device = iPK_Device;
			}

			/**
			* @brief serializes the class data
			*/
			void SetupSerialization(int /*iSC_Version*/)
			{
				StartSerializeList() +	m_iLevel + m_TimeStamp.tv_sec + m_TimeStamp.tv_usec + m_sName + m_sData + m_dwPK_Device;
			}

			/**
			* @brief returns the class name so that the SerializeClass will know how to serialize it
			* @todo ask if I got it right
			*/
			virtual string BinarySerializeClassName() { return "Logger::Entry"; }
		};
	};

	/** @class NullLogger
	A null DCE logger
	*/
	class NullLogger : public Logger
	{

	public:

		NullLogger() : Logger("")
		{}

		virtual ~NullLogger()
		{}

		virtual void WriteBlock(const char * /*pBlock*/, size_t /*sBlockLen*/) {}

		virtual void Write( int /*iLevel*/, const char * /*pcFormat*/, ... ) {}

		virtual void WriteEntry( class Logger::Entry& /*entry*/ ) {}

		virtual int GetType() { return LT_LOGGER_NULL; }
	};

	/** @class FileLogger
	A logger thet writes entries to a file.
	*/
	class FileLogger : public Logger
	{

	protected:

		FILE* m_LogFile; /**< the file to write the log to */

		string m_sFilename;

		bool m_bNeedToClose; /**< set to truth if the file needs to be closed, default value false */

	public:

		/**
		* @brief constructor, gets a FILE pointer, standard value for m_LogFile is stderr, m_bNeedToClose default is false
		*/
		FileLogger( FILE* log = stderr );

		/**
		* @brief writes the log to a file specified by name
		* open mode = append, m_bNeedToClose default is true,
		* if error while opening the file redirect to stderr
		*/
		FileLogger( const char* pcFile );

		/**
		* @brief destructor
		* closes the m_LogFile if m_bNeedToClose is set to true
		*/
		virtual ~FileLogger();

		/**
		* @brief returnes a new FileLogger to the file specified in the ts parameter
		* @param ts the file name should be specified like this: {filename}
		*/
//		static FileLogger* Parse( TokenStream& ts, StringStringMap& vars );

		/**
		* @brief used to the console foreground and background color and bold attribute if in Windows
		* @param iColor depending of it's value sets an specific scheme @see the implementation
		*/
		virtual void SetColor( int iColor );

		/**
		* @brief clrscr under windows
		*/
		virtual void ClearConsole();

		virtual void WriteBlock(const char *pBlock, size_t sBlockLen );

		/**
		* @brief prints the log entry data to the log file
		*/
		virtual void WriteEntry( class Logger::Entry& entry );

		virtual int GetType() { return LT_LOGGER_FILE; }

		virtual void Rotate();

	};

	/** @class WinOrbiterLogger
	* @brief a WinOrbiter DCE logger
	*/
#ifdef WIN32
	class WinOrbiterLogger : public Logger
	{
		HWND m_hWndList;

	public:

		WinOrbiterLogger(HWND hWndList) : Logger("")
		{ m_hWndList = hWndList; }

		virtual ~WinOrbiterLogger()
		{}

		virtual int GetType() { return LT_LOGGER_ORBITERWIN; }

		virtual void WriteBlock(const char *pBlock, size_t sBlockLen ) {}

		virtual void WriteEntry( class Logger::Entry& entry );
	};
#endif

#ifdef ANDROID
	class QtLogger : public Logger
	{

	public:

		QtLogger() : Logger("")
		{ 
		    m_iMaxLogLevel = 10;
		}

		virtual ~QtLogger()
		{}

		virtual int GetType() { return LT_LOGGER_QT; }
		virtual void WriteBlock(const char *pBlock, size_t sBlockLen ) {};
		virtual void WriteEntry( class Logger::Entry& entry );
	};
#endif


	typedef map<string, Logger*> NameLoggerMap;
	typedef map<string, string> StringStringMap;


    /**
    @class LoggerWrapper
    This class ???
     */
	class LoggerWrapper
	{
	private:
		static Logger *m_pPlutoLogger;
		static int m_iType;
		static string m_sFilename;

	public:
		static void Delete()
		{
			delete m_pPlutoLogger;
			m_pPlutoLogger=NULL;
		}
		static Logger *GetInstance()
		{
			if( m_pPlutoLogger==NULL )
			{
				switch( m_iType )
				{
				case LT_LOGGER_NULL:
					m_pPlutoLogger = new NullLogger();
					break;

				default:
#ifdef ANDROID
				        m_pPlutoLogger = new QtLogger();
#else			  
					if( m_sFilename.empty() )
						m_pPlutoLogger = new FileLogger(stdout);
					else
						m_pPlutoLogger = new FileLogger(m_sFilename.c_str());
#endif
					break;
				}
			}
			return m_pPlutoLogger;
		}

		static void SetInstance(Logger *pLogger)
		{
			m_pPlutoLogger=pLogger;
		}

		static int GetType() { return m_iType; }

		static void SetType(int Type,string sFilename="");
	};
}

#endif
