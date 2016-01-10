//give the abstract knxDevice class, and several classes inherited from that one

#ifndef KNXDEVICE
#define KNXDEVICE

#include "Telegram.h"
#include "DCE/DeviceData_Impl.h"
#include "DCE/Logger.h"
#include "pluto_main/Define_DeviceData.h"
#include "Gen_Devices/EIBBase.h"

#include <vector>
#include <string>
#include <stdexcept>


//some events/command/devicetemplates not yet implemented
#ifndef EVENT_Temperature_Changed_CONST
#define EVENT_Temperature_Changed_CONST 25
#endif

#ifndef EVENT_Level_Changed_CONST
#define EVENT_Level_Changed_CONST 0
#endif

#ifndef DEVICETEMPLATE_Blinds_Switch_CONST
#define DEVICETEMPLATE_Blinds_Switch_CONST 0
#endif

#ifndef COMMAND_Read_CONST
#define COMMAND_Read_CONST 60000
#endif

#ifndef COMMAND_Set_Angle_Percent_CONST
#define COMMAND_Set_Angle_Percent_CONST 60001
#endif

#ifndef COMMAND_Set_Mode_CONST
#define COMMAND_Set_Mode_CONST 60002
#endif

#ifndef COMMANDPARAMETER_Angle_Percent_CONST
#define COMMANDPARAMETER_Angle_Percent_CONST 0
#endif

#ifndef COMMANDPARAMETER_Mode_CONST
#define COMMANDPARAMETER_Mode_CONST 0
#endif

//each command wich could be sent to a knxDevice shall be declared using DEFINE_COMMAND( command(args) ) ;
#define DEFINE_COMMAND( x ) virtual Telegram *Command_##x{commandUnhandled(string( #x )); return NULL;}
#define DEFINE_COMMAND_ARRAY( x ) virtual vector<Telegram *> Command_##x{commandUnhandled(string( #x )); }

//each class inheriting knxDevice must start with DEVICEHEADER( name ,number of address t read from, char* of adresses values) ; where name is its exact name
#define DEVICEHEADER( x , numberread , tableread ) \
	private: \
		string stype(){return string( #x );};\
	public:\
		static const char *readparams##x(){return ( tableread );};\
		x (DeviceData_Impl *pDeviceData_Impl):knxDevice(pDeviceData_Impl){};\
		virtual Message *handleTelegram(const Telegram *tl);\
		virtual vector<Telegram> Command_Read(){return ReadVectFromTable( numberread , readparams##x() );}

using namespace std;
using namespace DCE;

namespace knx
{
	//abstract
	class knxDevice
	{
	protected:
		vector<eibaddr_t> _v_addrlist;	//number to addr translation
		int _device_DCEID;

	protected:
		virtual void commandUnhandled(string scommand);

	public:
		knxDevice(DeviceData_Impl *pDeviceData_Impl);
		virtual ~knxDevice(){_v_addrlist.clear();};
		virtual Message *handleTelegram(const Telegram *tl){return NULL;};
		virtual string stype()=0;		//to represent it in debug/log

		//here the list of the command that can be sent to the device
		virtual vector<Telegram> Command_Read()=0;
		DEFINE_COMMAND( On() ) ;
		DEFINE_COMMAND( Off() ) ;
		DEFINE_COMMAND( Open() ) ;
		DEFINE_COMMAND( Close() ) ;
		DEFINE_COMMAND( Stop() ) ;
		DEFINE_COMMAND( Set_Level(int Level) ) ;
		DEFINE_COMMAND_ARRAY( Set_Color(int R, int G, int B) ) ;
		DEFINE_COMMAND( Set_Angle_Percent(int Angle) ) ;
		DEFINE_COMMAND( Set_Mode(int Mode) ) ;
		DEFINE_COMMAND( Set_Temperature(float Temperature) ) ;

		static knxDevice *getKnxDevice(DeviceData_Impl *pDevData);

	protected:
		//creates a vector of READ Telegram according to the addresses of the device enumerated in table
		vector<Telegram> ReadVectFromTable(int tablesize, const char *table);

		//some functions to create the telegram
		//they take a value and set it in the data/shormessage field, with destination _v_addrlist[addrnumber]
		
		//fill shortuser with int
		Telegram *createsTelegramFromAddress(char data, int addressnumber);
		//fill data with char
		Telegram *createcharTelegramFromAddress(char data, int addressnumber);
		//fill data with float
		Telegram *createfloatTelegramFromAddress(float data, int addressnumber);

		inline Message *createStateChangedEventMessage(int state){return new Message(_device_DCEID, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT,EVENT_State_Changed_CONST, 1, EVENTPARAMETER_State_CONST, StringUtils::itos(state).c_str());};

		inline Message *createLevelChangedEventMessage(int state){return new Message(_device_DCEID, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT,EVENT_Level_Changed_CONST, 1, EVENTPARAMETER_Value_CONST, StringUtils::itos(state).c_str());};

		inline Message *createAngleChangedEventMessage(int angle){return new Message(_device_DCEID, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT,EVENT_Level_Changed_CONST, 1, EVENTPARAMETER_Value_CONST, StringUtils::itos(angle).c_str());};
		
		inline Message *createSetPointChangedEventMessage(float setpoint){return new Message(_device_DCEID, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT,EVENT_Thermostat_Set_Point_Chan_CONST, 1, EVENTPARAMETER_Value_CONST, StringUtils::ftos(setpoint).c_str());};
		
		inline Message *createModeChangedEventMessage(int mode){return new Message(_device_DCEID, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT,EVENT_State_Changed_CONST, 1, EVENTPARAMETER_State_CONST, StringUtils::itos(mode).c_str());};
		
		inline Message *createBrightnessChangedEventMessage(float value){return new Message(_device_DCEID, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT,EVENT_Brightness_Changed_CONST, 1, EVENTPARAMETER_Value_CONST, StringUtils::ftos(value).c_str());};

		inline Message *createTemperatureChangedEventMessage(float value){return new Message(_device_DCEID, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT,EVENT_Temperature_Changed_CONST, 1, EVENTPARAMETER_Value_CONST, StringUtils::ftos(value).c_str());};

		inline Message *createSensorTrippedEventMessage(int state){return new Message(_device_DCEID, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT,EVENT_Sensor_Tripped_CONST, 1, EVENTPARAMETER_Tripped_CONST, StringUtils::itos(state).c_str());};
	};

//here you define the knxDevices, redefining used commands
//then you need to create the handleTelegram in the cpp
//see 'developers.txt' in the doc folder

	class LightSwitchOnOff:public knxDevice
	{
		DEVICEHEADER( LightSwitchOnOff , 1, "\0" ) ;
		virtual Telegram *Command_On(){return createsTelegramFromAddress(1,0);};
		virtual Telegram *Command_Off(){return createsTelegramFromAddress(0,0);};
		virtual Telegram *Command_Set_Level(int level){return createsTelegramFromAddress( (level>0?1:0) ,0);};
	};

	class LightSwitchdimmable:public knxDevice
	{
		DEVICEHEADER( LightSwitchdimmable , 2, "\0\1" ) ;
		virtual Telegram *Command_On(){return createsTelegramFromAddress(1,0);};
		virtual Telegram *Command_Off(){return createsTelegramFromAddress(0,0);};
		virtual Telegram *Command_Set_Level(int level){return createcharTelegramFromAddress(level,1);};
	};

	class LightSwitchRGB:public knxDevice
	{
		DEVICEHEADER( LightSwitchRGB , 4, "\0\1\2\3" ) ;
		virtual Telegram *Command_On(){return createsTelegramFromAddress(1,0);};
		virtual Telegram *Command_Off(){return createsTelegramFromAddress(0,0);};
		virtual vector<Telegram *> Command_Set_Color(int R, int G, int B){
			vector<Telegram *> commands;
			commands.reserve(3);
			commands.push_back(createcharTelegramFromAddress(R,1));
			commands.push_back(createcharTelegramFromAddress(G,2));
			commands.push_back(createcharTelegramFromAddress(B,3));
			return commands;
		};
	};

	class Drapes_Switch:public knxDevice
	{
		DEVICEHEADER( Drapes_Switch , 2 , "\0\2" ) ;
		virtual Telegram *Command_Open(){return createsTelegramFromAddress(1,2);};
		virtual Telegram *Command_Close(){return createsTelegramFromAddress(0,2);};
		virtual Telegram *Command_On(){return createsTelegramFromAddress(1,2);};
		virtual Telegram *Command_Off(){return createsTelegramFromAddress(0,2);};
		virtual Telegram *Command_Stop(){return createsTelegramFromAddress(1,3);};
		virtual Telegram *Command_Set_Level(int level){return createcharTelegramFromAddress(level,0);};
	};

	class Blinds_Switch:public knxDevice
	{
		DEVICEHEADER( Blinds_Switch , 2 , "\0\1" ) ;
		virtual Telegram *Command_Open(){return createsTelegramFromAddress(1,4);};
		virtual Telegram *Command_Close(){return createsTelegramFromAddress(0,4);};
		virtual Telegram *Command_On(){return createsTelegramFromAddress(1,4);};
		virtual Telegram *Command_Off(){return createsTelegramFromAddress(0,4);};
		virtual Telegram *Command_Stop(){return createsTelegramFromAddress(1,5);};
		virtual Telegram *Command_Set_Level(int level){return createcharTelegramFromAddress(level,0);};
		virtual Telegram *Command_Set_Angle_Percent(int angle){return createcharTelegramFromAddress(angle,1);};
	};

	class Standard_Thermostat:public knxDevice
	{
		DEVICEHEADER( Standard_Thermostat , 2 , "\0\1" ) ;
		virtual Telegram *Command_Set_Temperature(float Temperature){return createfloatTelegramFromAddress(Temperature,0);};
		virtual Telegram *Command_Set_Mode(int Mode){return createcharTelegramFromAddress(Mode,1);};
	};

	class BrightnessSensor:public knxDevice
	{
		DEVICEHEADER( BrightnessSensor , 1 , "\0" ) ;
	};

	class TemperatureSensor:public knxDevice
	{
		DEVICEHEADER( TemperatureSensor , 1 , "\0" ) ;
	};

	class GenericSensor:public knxDevice
	{
		DEVICEHEADER( GenericSensor , 0, "" ) ;
	};
}


#endif
