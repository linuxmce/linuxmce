/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU Lesser General Public License for more details.

*/

/** @file Message.h
Header file for the Message and SafetyMessage classes.
It also includes definition for enums.
 @todo notcommented
 */
#ifndef MESSAGEH
#define MESSAGEH

#include "SerializeClass/SerializeClass.h"

#define DEVICEID_NULL           -1  // Invalid device

// Some system devices
#define DEVICEID_DCEROUTER      -1000	// System commands for the router itself
#define DEVICEID_EVENTMANAGER   -1001   // Not a real device, just the destination of events
#define DEVICEID_LOGGER			-1002
#define DEVICEID_MESSAGESEND	-1003	// The message send utility

// Virtual devices
#define DEVICEID_MASTERDEVICE   -2000
#define DEVICEID_CATEGORY       -2001
#define DEVICEID_LIST           -2002
#define DEVICEID_GROUP          -2003

// Other
#define DEVICEID_HANDLED_INTERNALLY -300

#ifdef LL_DEBUG
	// A counter used to mark messages so they can be traced in the low level logs
	extern int g_MessageID;
#endif


/**
@namespace DCE
The Data Commands and Events (DCE) namespace
*/
namespace DCE
{

    /** @enum eMessageType
     Enum containing values associated with message types.
     */
    enum eMessageType { MESSAGETYPE_COMMAND=1, MESSAGETYPE_EVENT=2, MESSAGETYPE_DATAPARM_CHANGE=3, MESSAGETYPE_REPLY=4,
		MESSAGETYPE_DATAPARM_REQUEST=5, MESSAGETYPE_LOG=6, MESSAGETYPE_SYSCOMMAND=7,
		MESSAGETYPE_REGISTER_INTERCEPTOR=8, MESSAGETYPE_MESSAGE_INTERCEPTED=9, MESSAGETYPE_EXEC_COMMAND_GROUP=10,
		MESSAGETYPE_START_PING=11, MESSAGETYPE_STOP_PING=12, MESSAGETYPE_PURGE_INTERCEPTORS=13,
		MESSAGETYPE_PENDING_TASKS=14 };

    /** @enum eParmType
     Enum containing values for the criteria used in a message interceptor.
     */
	enum eParmType { PARM_FROM=1, PARM_TO, PARM_TEMPLATE, PARM_CATEGORY, PARM_MESSAGE_TYPE, PARM_MESSAGE_ID };


    /** @enum
     Enum containing values associated with log entry params.
     */
    enum { LOG_PARAM_TIMESTAMP=0, LOG_PARAM_NAME=1, LOG_PARAM_DEVICE=2,LOG_PARAM_DATA=3, LOG_PARAM_LEVEL=4 };

    /** @enum
     Enum containing values associeted with syscommands.
		RELOAD_FORCED means do the reload no matter what, ignoring the plugins PendingTasks.
     */
    enum { SYSCOMMAND_QUIT=0, SYSCOMMAND_RELOAD=1, SYSCOMMAND_START_MODULE=2, SYSCOMMAND_STOP_MODULE=3, SYSCOMMAND_SEGFAULT=4,
		SYSCOMMAND_DEADLOCK=5, SYSCOMMAND_RELOAD_FORCED=6, SYSCOMMAND_SHOW_SOCKETS=7, SYSCOMMAND_DEVICE_UP=8,
		SYSCOMMAND_DEVICE_DOWN=9, SYSCOMMAND_ROTATE=10, SYSCOMMAND_RELOAD_LOGLEVEL=11, SYSCOMMAND_SPAWN_NEW_CHILDREN=12,
		SYSCOMMAND_RETRIEVE_INSTALLATION_ID=13 };

    /**
     * @brief enum containing values associted with priorities
     */
    enum { PRIORITY_LOW=0, PRIORITY_NORMAL=1, PRIORITY_HIGH=2, PRIORITY_URGENT=3 };

    /**
     * @brief enum containing values associated with broadcast levels
     */
    enum eBroadcastLevel { BL_None=0, BL_DirectSiblings=1, BL_SameComputer=2, BL_SameRoom=3, BL_SameHouse=4, BL_AllHouses=5 };

    /**
     * @brief enum containing values associated with retry, if the message cannot be delivered.
	 * The default is MR_None to not retry.  MR_Retry means resend when the device registers, MR_Persist means
	 * do MR_Retry, but also serialize to disk so the message will survive after a reload router
     */
    enum eRetry { MR_None=0, MR_Retry=1, MR_Persist=2 };

	/**
     * @brief enum containing vlues associated with expected responses
     */
    enum eExpectedResponse { ER_None=0, ER_ReplyMessage, ER_ReplyString, ER_DeliveryConfirmation };

	enum DataFormat
	{
		dfBinary,
		dfPlainText,
		dfXml
	};

	class Message;

	//message -> data
	typedef bool (* SerializeMessage)(Message *pMessage, char *&pData, size_t &nSize);
	//data -> message
	typedef bool (* DeserializeMessage)(Message *pMessage, char *pData, size_t nSize);

    /** @class Message
    Data command and events message (sent between various devices).
     */
    class Message : public SerializeClass
    {

		/** Serializers and deserializer methods */
		static map<DataFormat, SerializeMessage> m_mapSerializers;
		static map<DataFormat, DeserializeMessage> m_mapDeserializers;

    public:

		static void RegisterSerializer(DataFormat format, SerializeMessage pfSerializeMessage);
		static void RegisterDeserializer(DataFormat format, DeserializeMessage pfDeserializeMessage);


		int m_MessageID;  // For low-level debugging only

        /** fields that corespond to primary keys */

        long m_dwPK_Device_From; /**< the id of the device from witch the message is comming */
        long m_dwPK_Device_To; /**< the id of the device to witch the messge is going */
        long m_dwPK_Device_Group_ID_To; /**< the id of the group of devices to witch the message is going */
        long m_dwPK_Device_Category_To; /**< the device category to send the message to (all devices in that category will receive it) */
        long m_dwPK_Device_Template; /**< the master device for the current message */
        string m_sPK_Device_List_To; /**< the message will be sent to all the devices build using this template */

        /** fields describing the message */

        long m_dwID; /**< the message ID */
        long m_dwPriority; /**< the message priority */
        long m_dwMessage_Type; /**< the message type */

        /** flags for setting the way the framework will treat the message */

        bool m_bIncludeChildren;

        /**
         * @brief specifies if the framework will allways sen a response
         * If m_eExpectedResponse is set, then the sending device will be expecting a response.  If any message
         * interceptor takes care of this and sets this flag to true, then the framework knows it does not
         * need to send a response.  If this flag is false, the framework will always send a response.
         */
        bool m_bRespondedToMessage;

        /**
         * @brief specifies to who the message will be sent; see the full comment
         * If broadcast level is not none, then rather than sending the message to the destination,
         * the router will send it to the corresponding masterdevice/devicecategory.  Normally,
         * the "to" device is what is used to determine the broadcast devices (ie devices in the same
         * room as the "to" device).  But if this is set to true, the sending device is used to determine
         * the target devices.  Example: A controller is marked as being in the bedroom, but the user is
         * controlling a DVD player in the living room.  The DVD player is the "to" device used on the remote.
         * If a message is sent to all device category "lights" with broadcast level set to room, then all lights
         * in the living room will get the message, unless relative to sender is 'true', which means all lights
         * in the bedroom
         */
        bool m_bRelativeToSender;

		bool m_bCanBuffer;  // This message can be buffered

        /**
         * @brief the level at which the message is broadcasted
         */
        eBroadcastLevel m_eBroadcastLevel;

        /**
         * @brief how to handle retries if the message cannot be delivered
         */
        eRetry m_eRetry;

		/**
         * @brief specifies the expected response, so that the socket filter stops all other responses
         */
        eExpectedResponse m_eExpectedResponse;

        map<long, string> m_mapParameters; /**< the message parameters */
        map<long, char *> m_mapData_Parameters; /**< the data parameters (pointers to the actual data) */
        map<long, unsigned long>  m_mapData_Lengths; /**< the length of the data from data parameters */
        vector<class Message *> m_vectExtraMessages; /**< vector of messages */

        string sOutputParametersPath; /**< where to store output parameters files */

        /** Constructor.
         Just calls the Clear method
         @see the Message::Clear() method
         */
        Message();

        /** Constructor.
        @param dwSize
        @param pcData is the string
        Calls the FromData method to deserialize the data from pcData
         and after finishing assigns null to SerializeClass::m_pcDataBlock
        @see Message::FromData( ... )
        @warning This replaces the delimiters in char *pcData with NULLs.
        */
        Message( unsigned long dwSize, char *pcData );

        /** Constructor.
        @param pMessage_Clone
        Copies all the member data from the Message pointed by the pMessage_Clone parameter
         */
        Message( Message *pMessage_Clone );

        /** Constructor.
         This function takes a variable list of arguments and returns a string that can be sent through a TCP socket
         */
        Message( long dwDeviceIDFrom, long dwDeviceIDTo, long dwPriority, long dwMessageType, long dwID, unsigned long dwParameterCount, ... );

        /** Constructor.
         This allows for a comma-delimited list of devices to send the command to
         */
        Message( long dwDeviceIDFrom, string sDeviceIDTo, long dwPriority, long dwMessageType, long dwID, unsigned long dwParameterCount, ...);

        /** Constructor.
        This allows for a message to be sent to a category of device.
        @param dwDeviceIDFrom
        @param dwDeviceCategory
        @param bIncludeChildren is a flag
        @param eB
        @param dwPriority
        @param dwMessageType
        @param dwID
        @param dwParameterCount
        If bIncludeChildren is true, then if a device belongs to a child category, it will still apply.
        If the message is sent to devices with  the category: AVEquipment,
         it will still go to a device with the category DVDPlayers,
         because that category is a child of AVEquipment
         */
        Message( long dwDeviceIDFrom, long dwDeviceCategory, bool bIncludeChildren, eBroadcastLevel eB, long dwPriority, long dwMessageType, long dwID, unsigned long dwParameterCount, ... );

        /** Constructor.
         * @brief this allows for a message to be sent to a particular type of device specified by dwMasterDevice
         */
        Message( long dwDeviceIDFrom, long dwMasterDevice, eBroadcastLevel eB, long dwPriority, long dwMessageType, long dwID, unsigned long dwParameterCount, ... );

        /** Constructor.
         * @brief this allows for a string formatted message to be sent to a particular device using the same format
		 * like message send.  Multiple messages can be separated with \n
         */
        Message( string sMessageInStringFormat );

        /** Constructor.
         * @brief this allows for a message coming in from command line arguments
         */
		Message( int iNumArgs, char *cArguments[], int dwPK_DeviceFrom=0 );

        /** Destructor.
         * @brief just calls the Clear method
         * @see the Message::Clear() method
         */
        virtual ~Message();

        /**
         * @brief assignes default values to the member data and clears the map members data
         */
        void Clear();


        virtual void ToData( unsigned long &dwSize, char* &pcData, bool bWithHeader=false );
        virtual string ToString( bool bWithHeader=false );

		virtual bool SerializeToData(DataFormat format, char *&pData, size_t& nSize);
		virtual bool DeserializeFromData(DataFormat format, char *pData, size_t nSize);

        /**
         * @brief start deserializing the pcData to build the instance of the Message
         */
        void FromData( unsigned long dwSize, char *pcData );

        /** Build from command line arguments.
        @param iNumArgs is the arg count.
        @param cArguments is the list of arg strings.
        @param dwPK_DeviceFrom is the device ID.
        @param pMessage_Parent is the message.
        @param bDeleteArray is the delete flag.
         */
		void BuildFromArgs( int iNumArgs, char *cArguments[], int dwPK_DeviceFrom=0, Message *pMessage_Parent=NULL, bool bDeleteArray=true );

	void Dump( int iLogLevel = 35 /*LV_DEBUG*/ );
	};

    /** @class SafetyMessage
     * @brief ...
     * @todo ask
     */
    class SafetyMessage
    {
		bool m_bAutoDelete;
    public:

        Message *m_pMessage;

        Message*& operator*()
        {
            return m_pMessage;
        };

        Message *Detach()
        {
            Message *ReturnValue = m_pMessage;
            m_pMessage = NULL;
            return ReturnValue;

        }

        SafetyMessage( Message *pMessage, bool bAutoDelete=true ) { m_pMessage = pMessage; m_bAutoDelete = bAutoDelete; };

        ~SafetyMessage() { if( m_bAutoDelete ) delete m_pMessage; };

		void m_bAutoDelete_set(bool bAutoDelete) { m_bAutoDelete=bAutoDelete; }
    };
}

#endif
