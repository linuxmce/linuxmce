/*
 *      Copyright (C) 2008 Harald Klein <hari@vt100.at>
 *
 *      This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
 *      This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *      of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *      See the GNU General Public License for more details.
 */

#ifndef _ZW_API_H_

#define _ZW_API_H_

#define SOF				0x01
#define ACK				0x06
#define NAK				0x15
#define CAN				0x18

#define MAGIC_LEN			29

#define REQUEST				0x0
#define RESPONSE			0x1

#define ZW_GET_VERSION			0x15
#define ZW_MEMORY_GET_ID		0x20	// response: 4byte home id, node id
#define ZW_MEM_GET_BUFFER		0x23
#define ZW_MEM_PUT_BUFFER		0x24
#define ZW_CLOCK_SET			0x30

#define TRANSMIT_OPTION_ACK         	0x01
#define TRANSMIT_OPTION_LOW_POWER   	0x02
#define TRANSMIT_OPTION_AUTO_ROUTE  	0x04
#define TRANSMIT_OPTION_FORCE_ROUTE 	0x08

#define TRANSMIT_COMPLETE_OK      	0x00
#define TRANSMIT_COMPLETE_NO_ACK  	0x01
#define TRANSMIT_COMPLETE_FAIL    	0x02
#define TRANSMIT_COMPLETE_NOROUTE 	0x04

#define RECEIVE_STATUS_TYPE_BROAD     			0x04
#define NODE_BROADCAST					0xff

#define FUNC_ID_SERIAL_API_GET_INIT_DATA		0x02
#define FUNC_ID_SERIAL_API_GET_CAPABILITIES             0x07

#define FUNC_ID_SERIAL_API_SOFT_RESET			0x8

#define FUNC_ID_APPLICATION_COMMAND_HANDLER             0x04

#define FUNC_ID_ZW_APPLICATION_UPDATE                   0x49
#define FUNC_ID_ZW_SET_DEFAULT				0x42
#define FUNC_ID_ZW_REPLICATION_COMMAND_COMPLETE         0x44
#define FUNC_ID_ZW_GET_NODE_PROTOCOL_INFO               0x41
#define FUNC_ID_ZW_REQUEST_NODE_NEIGHBOR_UPDATE         0x48
#define FUNC_ID_ZW_SEND_DATA                            0x13
#define FUNC_ID_ZW_SET_LEARN_MODE                       0x50
#define FUNC_ID_ZW_ASSIGN_SUC_RETURN_ROUTE		0x51
#define FUNC_ID_ZW_ENABLE_SUC                           0x52
#define FUNC_ID_ZW_REQUEST_NETWORK_UPDATE		0x53
#define FUNC_ID_ZW_SET_SUC_NODE_ID                      0x54
#define FUNC_ID_ZW_GET_SUC_NODE_ID                      0x56
#define FUNC_ID_ZW_ADD_NODE_TO_NETWORK			0x4a
#define FUNC_ID_ZW_REMOVE_NODE_FROM_NETWORK		0x4b
#define FUNC_ID_ZW_REQUEST_NODE_INFO                    0x60
#define FUNC_ID_ZW_REMOVE_FAILED_NODE_ID                0x61
#define FUNC_ID_ZW_IS_FAILED_NODE_ID			0x62
#define ZW_GET_ROUTING_INFO			0x80

#define ADD_NODE_ANY					0x01
#define ADD_NODE_STOP					0x05
#define ADD_NODE_STATUS_LEARN_READY          		0x01
#define ADD_NODE_STATUS_NODE_FOUND           		0x02
#define ADD_NODE_STATUS_ADDING_SLAVE         		0x03
#define ADD_NODE_STATUS_ADDING_CONTROLLER    		0x04
#define ADD_NODE_STATUS_PROTOCOL_DONE        		0x05
#define ADD_NODE_STATUS_DONE                 		0x06
#define ADD_NODE_STATUS_FAILED               		0x07
#define ADD_NODE_OPTION_HIGH_POWER			0x80

#define REMOVE_NODE_ANY					0x01
#define REMOVE_NODE_STOP				0x05
#define REMOVE_NODE_STATUS_LEARN_READY          	0x01
#define REMOVE_NODE_STATUS_NODE_FOUND           	0x02
#define REMOVE_NODE_STATUS_ADDING_SLAVE         	0x03
#define REMOVE_NODE_STATUS_ADDING_CONTROLLER    	0x04
#define REMOVE_NODE_STATUS_PROTOCOL_DONE        	0x05
#define REMOVE_NODE_STATUS_DONE                 	0x06
#define REMOVE_NODE_STATUS_FAILED               	0x07

#define ZW_SUC_FUNC_BASIC_SUC				0x00
#define ZW_SUC_FUNC_NODEID_SERVER			0x01
#define FUNC_ID_ZW_ASSIGN_RETURN_ROUTE			0x46

#define UPDATE_STATE_NODE_INFO_RECEIVED     		0x84
#define UPDATE_STATE_NODE_INFO_REQ_FAILED		0x81
#define UPDATE_STATE_DELETE_DONE			0x20
#define UPDATE_STATE_NEW_ID_ASSIGNED			0x40

#define BASIC_TYPE_CONTROLLER                           0x01
#define BASIC_TYPE_STATIC_CONTROLLER                    0x02
#define BASIC_TYPE_SLAVE                                0x03
#define BASIC_TYPE_ROUTING_SLAVE                        0x04

#define GENERIC_TYPE_GENERIC_CONTROLLER                 0x01
#define GENERIC_TYPE_STATIC_CONTROLLER                  0x02
#define GENERIC_TYPE_AV_CONTROL_POINT                   0x03
#define GENERIC_TYPE_DISPLAY                            0x06
#define GENERIC_TYPE_GARAGE_DOOR                        0x07
#define GENERIC_TYPE_THERMOSTAT                         0x08
#define GENERIC_TYPE_WINDOW_COVERING                    0x09
#define GENERIC_TYPE_REPEATER_SLAVE                     0x0F

#define GENERIC_TYPE_SWITCH_BINARY                      0x10

#define GENERIC_TYPE_SWITCH_MULTILEVEL                  0x11
#define SPECIFIC_TYPE_NOT_USED				0x00
#define SPECIFIC_TYPE_POWER_SWITCH_MULTILEVEL		0x01
#define SPECIFIC_TYPE_MOTOR_MULTIPOSITION		0x03
#define SPECIFIC_TYPE_SCENE_SWITCH_MULTILEVEL		0x04
#define SPECIFIC_TYPE_CLASS_A_MOTOR_CONTROL		0x05
#define SPECIFIC_TYPE_CLASS_B_MOTOR_CONTROL		0x06
#define SPECIFIC_TYPE_CLASS_C_MOTOR_CONTROL		0x07

#define GENERIC_TYPE_SWITCH_REMOTE                      0x12
#define GENERIC_TYPE_SWITCH_TOGGLE                      0x13
#define GENERIC_TYPE_SENSOR_BINARY                      0x20
#define GENERIC_TYPE_SENSOR_MULTILEVEL                  0x21
#define GENERIC_TYPE_SENSOR_ALARM			0xa1
#define GENERIC_TYPE_WATER_CONTROL                      0x22
#define GENERIC_TYPE_METER_PULSE                        0x30
#define GENERIC_TYPE_ENTRY_CONTROL                      0x40
#define GENERIC_TYPE_SEMI_INTEROPERABLE                 0x50
#define GENERIC_TYPE_NON_INTEROPERABLE                  0xFF

#define SPECIFIC_TYPE_ADV_ZENSOR_NET_SMOKE_SENSOR	0x0a
#define SPECIFIC_TYPE_BASIC_ROUTING_SMOKE_SENSOR	0x06
#define SPECIFIC_TYPE_BASIC_ZENSOR_NET_SMOKE_SENSOR	0x08
#define SPECIFIC_TYPE_ROUTING_SMOKE_SENSOR		0x07
#define SPECIFIC_TYPE_ZENSOR_NET_SMOKE_SENSOR		0x09


#define COMMAND_CLASS_MARK				0xef

#define COMMAND_CLASS_BASIC				0x20
#define BASIC_SET					0x01
#define BASIC_GET					0x02
#define BASIC_REPORT					0x03

#define COMMAND_CLASS_VERSION				0x86
#define VERSION_GET					0x11
#define VERSION_REPORT					0x12

#define COMMAND_CLASS_BATTERY				0x80
#define BATTERY_GET					0x02
#define BATTERY_REPORT					0x03

#define COMMAND_CLASS_WAKE_UP                         	0x84

#define WAKE_UP_INTERVAL_SET                         	0x04
#define WAKE_UP_NOTIFICATION                         	0x07
#define WAKE_UP_NO_MORE_INFORMATION                  	0x08

#define COMMAND_CLASS_CONTROLLER_REPLICATION          	0x21
#define CTRL_REPLICATION_TRANSFER_GROUP              	0x31

#define COMMAND_CLASS_SWITCH_MULTILEVEL               	0x26
#define SWITCH_MULTILEVEL_SET                        	0x01
#define SWITCH_MULTILEVEL_GET                        	0x02
#define SWITCH_MULTILEVEL_REPORT                        0x03

#define COMMAND_CLASS_SWITCH_ALL			0x27
#define SWITCH_ALL_ON					0x04
#define SWITCH_ALL_OFF					0x05

#define COMMAND_CLASS_SENSOR_BINARY			0x30
#define SENSOR_BINARY_REPORT				0x03

#define COMMAND_CLASS_SENSOR_MULTILEVEL			0x31
#define SENSOR_MULTILEVEL_VERSION			0x01
#define SENSOR_MULTILEVEL_GET				0x04
#define SENSOR_MULTILEVEL_REPORT			0x05

#define COMMAND_CLASS_SENSOR_ALARM			0x9c
#define SENSOR_ALARM_GET				0x1
#define SENSOR_ALARM_REPORT				0x2
#define SENSOR_ALARM_SUPPORTED_GET			0x3
#define SENSOR_ALARM_SUPPORTED_REPORT			0x4

#define SENSOR_MULTILEVEL_REPORT_TEMPERATURE		0x01
#define SENSOR_MULTILEVEL_REPORT_GENERAL_PURPOSE_VALUE	0x02
#define SENSOR_MULTILEVEL_REPORT_LUMINANCE		0x03
#define SENSOR_MULTILEVEL_REPORT_POWER			0x04
#define SENSOR_MULTILEVEL_REPORT_RELATIVE_HUMIDITY	0x05
#define SENSOR_MULTILEVEL_REPORT_CO2_LEVEL		0x11

#define SENSOR_MULTILEVEL_REPORT_SIZE_MASK	0x07
#define SENSOR_MULTILEVEL_REPORT_SCALE_MASK	0x18
#define SENSOR_MULTILEVEL_REPORT_SCALE_SHIFT	0x03
#define SENSOR_MULTILEVEL_REPORT_PRECISION_MASK	0xe0
#define SENSOR_MULTILEVEL_REPORT_PRECISION_SHIFT	0x5

#define COMMAND_CLASS_ALARM				0x71
#define ALARM_REPORT					0x05

#define COMMAND_CLASS_MULTI_CMD                         0x8F
#define MULTI_CMD_VERSION                               0x01
#define MULTI_CMD_ENCAP                                 0x01
#define MULTI_CMD_RESPONSE_ENCAP                        0x02

#define COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE		0x46
#define SCHEDULE_SET                                    0x01
#define SCHEDULE_GET                                    0x02
#define SCHEDULE_CHANGED_GET				0x04
#define SCHEDULE_CHANGED_REPORT				0x05
#define SCHEDULE_OVERRIDE_GET				0x07
#define SCHEDULE_OVERRIDE_REPORT			0x08

#define COMMAND_CLASS_CLOCK				0x81
#define CLOCK_GET					0x05
#define CLOCK_SET                                       0x04
#define CLOCK_REPORT                                    0x06

#define COMMAND_CLASS_ASSOCIATION			0x85
#define ASSOCIATION_SET					0x01
#define ASSOCIATION_GET					0x02
#define ASSOCIATION_REPORT				0x03
#define ASSOCIATION_REMOVE				0x04

#define COMMAND_CLASS_CONFIGURATION			0x70
#define CONFIGURATION_GET				0x05
#define CONFIGURATION_SET				0x04

#define COMMAND_CLASS_MANUFACTURER_SPECIFIC		0x72
#define MANUFACTURER_SPECIFIC_GET			0x04
#define MANUFACTURER_SPECIFIC_REPORT			0x05

#define COMMAND_CLASS_APPLICATION_STATUS 		0x22
#define COMMAND_CLASS_ASSOCIATION_COMMAND_CONFIGURATION 0x9B
#define COMMAND_CLASS_AV_CONTENT_DIRECTORY_MD		0x95
#define COMMAND_CLASS_AV_CONTENT_SEARCH_MD		0x97
#define COMMAND_CLASS_AV_RENDERER_STATUS		0x96
#define COMMAND_CLASS_AV_TAGGING_MD			0x99
#define COMMAND_CLASS_BASIC_WINDOW_COVERING		0x50
#define COMMAND_CLASS_CHIMNEY_FAN			0x2A
#define COMMAND_CLASS_COMPOSITE				0x8D
#define COMMAND_CLASS_DOOR_LOCK				0x62
#define COMMAND_CLASS_ENERGY_PRODUCTION			0x90
#define COMMAND_CLASS_FIRMWARE_UPDATE_MD		0x7a
#define COMMAND_CLASS_GEOGRAPHIC_LOCATION		0x8C
#define COMMAND_CLASS_GROUPING_NAME			0x7B
#define COMMAND_CLASS_HAIL				0x82
#define COMMAND_CLASS_INDICATOR				0x87
#define COMMAND_CLASS_IP_CONFIGURATION			0x9A
#define COMMAND_CLASS_LANGUAGE				0x89
#define COMMAND_CLASS_LOCK				0x76
#define COMMAND_CLASS_MANUFACTURER_PROPRIETARY		0x91
#define COMMAND_CLASS_METER_PULSE			0x35
#define COMMAND_CLASS_METER				0x32

#define METER_GET					0x01
#define METER_REPORT					0x02
#define METER_REPORT_ELECTRIC_METER			0x01
#define METER_REPORT_GAS_METER				0x02
#define METER_REPORT_WATER_METER			0x03

#define METER_REPORT_SIZE_MASK				0x07
#define METER_REPORT_SCALE_MASK				0x18
#define METER_REPORT_SCALE_SHIFT			0x03
#define METER_REPORT_PRECISION_MASK			0xe0
#define METER_REPORT_PRECISION_SHIFT			0x05

#define COMMAND_CLASS_MTP_WINDOW_COVERING		0x51
#define COMMAND_CLASS_MULTI_INSTANCE_ASSOCIATION	0x8E
#define COMMAND_CLASS_MULTI_INSTANCE			0x60

#define MULTI_INSTANCE_VERSION				0x01
#define MULTI_INSTANCE_GET				0x04
#define MULTI_INSTANCE_CMD_ENCAP			0x6
#define MULTI_INSTANCE_REPORT				0x05

#define MULTI_INSTANCE_ASSOCIATION_VERSION              0x01
#define MULTI_INSTANCE_ASSOCIATION_GET                 0x2
#define MULTI_INSTANCE_ASSOCIATION_GROUPINGS_GET        0x05
#define MULTI_INSTANCE_ASSOCIATION_GROUPINGS_REPORT     0x06
#define MULTI_INSTANCE_ASSOCIATION_REMOVE               0x04
#define MULTI_INSTANCE_ASSOCIATION_REPORT               0x03
#define MULTI_INSTANCE_ASSOCIATION_SET                  0x01
#define MULTI_INSTANCE_ASSOCIATION_REMOVE_MARKER        0x00
#define MULTI_INSTANCE_ASSOCIATION_REPORT_MARKER        0x00
#define MULTI_INSTANCE_ASSOCIATION_SET_MARKER           0x00


#define COMMAND_CLASS_NO_OPERATION			0x00
#define COMMAND_CLASS_NODE_NAMING			0x77
#define COMMAND_CLASS_NON_INTEROPERABLE			0xf0
#define COMMAND_CLASS_POWERLEVEL			0x73
#define COMMAND_CLASS_PROPRIETARY			0x88
#define COMMAND_CLASS_PROTECTION			0x75
#define COMMAND_CLASS_REMOTE_ASSOCIATION_ACTIVATE	0x7c
#define COMMAND_CLASS_REMOTE_ASSOCIATION		0x7d
#define COMMAND_CLASS_SCENE_ACTIVATION			0x2b
#define COMMAND_CLASS_SCENE_ACTUATOR_CONF		0x2C
#define COMMAND_CLASS_SCENE_CONTROLLER_CONF		0x2D
#define COMMAND_CLASS_SCREEN_ATTRIBUTES			0x93
#define COMMAND_CLASS_SCREEN_MD				0x92

#define COMMAND_CLASS_SECURITY				0x98
#define SECURITY_SCHEME_GET				0x04
#define SECURITY_SCHEME_REPORT				0x5
#define SECURITY_NONCE_GET				0x40
#define SECURITY_NONCE_REPORT				0x80
#define NETWORK_KEY_SET		0x06
#define NETWORK_KEY_VERIFY	0x07
#define SECURITY_MESSAGE_ENCAPSULATION 0x81
#define SECURITY_MESSAGE_ENCAPSULATION_NONCE_GET 0xc1

#define COMMAND_CLASS_SENSOR_CONFIGURATION		0x9E
#define COMMAND_CLASS_SILENCE_ALARM			0x9d

#define COMMAND_CLASS_SIMPLE_AV_CONTROL			0x94
#define SIMPLE_AV_CONTROL_VERSION			0x01
#define SIMPLE_AV_CONTROL_GET				0x02
#define SIMPLE_AV_CONTROL_REPORT			0x3
#define SIMPLE_AV_CONTROL_SET				0x01
#define SIMPLE_AV_CONTROL_SUPPORTED_GET			0x04
#define SIMPLE_AV_CONTROL_SUPPORTED_REPORT		0x05
#define SIMPLE_AV_CONTROL_SET_PROPERTIES1_KEY_ATTRIBUTES_MASK 0x07
#define SIMPLE_AV_CONTROL_SET_PROPERTIES1_RESERVED_MASK 0xF8
#define SIMPLE_AV_CONTROL_SET_PROPERTIES1_RESERVED_SHIFT 0x03

#define COMMAND_CLASS_SWITCH_BINARY			0x25
#define SWITCH_BINARY_VERSION				0x1
#define SWITCH_BINARY_GET				0x2
#define SWITCH_BINARY_REPORT				0x3
#define SWITCH_BINARY_SET				0x01

#define COMMAND_CLASS_SWITCH_TOGGLE_BINARY		0x28
#define COMMAND_CLASS_SWITCH_TOGGLE_MULTILEVEL		0x29
#define COMMAND_CLASS_THERMOSTAT_FAN_MODE		0x44

#define THERMOSTAT_FAN_MODE_VERSION                     0x01
#define THERMOSTAT_FAN_MODE_GET                         0x02
#define THERMOSTAT_FAN_MODE_REPORT                      0x03
#define THERMOSTAT_FAN_MODE_SET                         0x01
#define THERMOSTAT_FAN_MODE_SUPPORTED_GET               0x04
#define THERMOSTAT_FAN_MODE_SUPPORTED_REPORT            0x05
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_MASK        0x0F
#define THERMOSTAT_FAN_MODE_REPORT_RESERVED_MASK        0xf0
#define THERMOSTAT_FAN_MODE_REPORT_RESERVED_SHIFT       0x04
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_MASK           0x0F
#define THERMOSTAT_FAN_MODE_SET_RESERVED_MASK           0xF0
#define THERMOSTAT_FAN_MODE_SET_RESERVED_SHIFT          0x04

#define COMMAND_CLASS_THERMOSTAT_FAN_STATE		0x45
#define COMMAND_CLASS_THERMOSTAT_HEATING		0x38
#define COMMAND_CLASS_THERMOSTAT_MODE			0x40

#define THERMOSTAT_MODE_VERSION                         0x01
#define THERMOSTAT_MODE_GET                             0x02
#define THERMOSTAT_MODE_REPORT                          0x03
#define THERMOSTAT_MODE_SET                             0x01
#define THERMOSTAT_MODE_SUPPORTED_GET                   0x04
#define THERMOSTAT_MODE_SUPPORTED_REPORT                0x05

#define COMMAND_CLASS_THERMOSTAT_OPERATING_STATE	0x42
#define COMMAND_CLASS_THERMOSTAT_SETBACK		0x47
#define COMMAND_CLASS_THERMOSTAT_SETPOINT		0x43

#define THERMOSTAT_SETPOINT_VERSION                     0x01
#define THERMOSTAT_SETPOINT_GET                         0x02
#define THERMOSTAT_SETPOINT_REPORT                      0x03
#define THERMOSTAT_SETPOINT_SET                         0x01
#define THERMOSTAT_SETPOINT_SUPPORTED_GET               0x04
#define THERMOSTAT_SETPOINT_SUPPORTED_REPORT            0x05
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_MASK      0x0F
#define THERMOSTAT_SETPOINT_GET_RESERVED_MASK           0xf0
#define THERMOSTAT_SETPOINT_GET_RESERVED_SHIFT          0x04
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_MASK   0xF
#define THERMOSTAT_SETPOINT_REPORT_RESERVED_MASK        0xf0
#define THERMOSTAT_SETPOINT_REPORT_RESERVED_SHIFT       0x04
#define THERMOSTAT_SETPOINT_REPORT_SIZE_MASK            0x07
#define THERMOSTAT_SETPOINT_REPORT_SCALE_MASK           0x18
#define THERMOSTAT_SETPOINT_REPORT_SCALE_SHIFT          0x3
#define THERMOSTAT_SETPOINT_REPORT_PRECISION_MASK       0xe0
#define THERMOSTAT_SETPOINT_REPORT_PRECISION_SHIFT      0x05
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_MASK      0x0F
#define THERMOSTAT_SETPOINT_SET_RESERVED_MASK           0xF0
#define THERMOSTAT_SETPOINT_SET_RESERVED_SHIFT          0x04
#define THERMOSTAT_SETPOINT_SET_SIZE_MASK               0x07
#define THERMOSTAT_SETPOINT_SET_SCALE_MASK              0x18
#define THERMOSTAT_SETPOINT_SET_SCALE_SHIFT             0x03
#define THERMOSTAT_SETPOINT_SET_PRECISION_MASK          0xE0
#define THERMOSTAT_SETPOINT_SET_PRECISION_SHIFT         0x05

#define COMMAND_CLASS_TIME_PARAMETERS			0x8B
#define COMMAND_CLASS_TIME				0x8a
#define COMMAND_CLASS_USER_CODE				0x63
#define COMMAND_CLASS_ZIP_ADV_CLIENT			0x34
#define COMMAND_CLASS_ZIP_ADV_SERVER			0x33
#define COMMAND_CLASS_ZIP_ADV_SERVICES			0x2F
#define COMMAND_CLASS_ZIP_CLIENT			0x2e
#define COMMAND_CLASS_ZIP_SERVER			0x24
#define COMMAND_CLASS_ZIP_SERVICES			0x23

#include <string>
#include <deque>
#include <map>
#include <iostream>
#include <algorithm>

namespace ZWApi {
    struct ZWJob {
	char buffer[512];
	size_t len;
	time_t timeout;
	int sendcount;
	int callbackid;
	int callback_type;
	bool await_response;
	int nodeid;
    };
    struct ZWIntent {
	int type;
	int nodeid;
	time_t timeout;
	int retrycount;
    };

    struct ZWNode {
	    ZWNode(int node_id) {
		    id = node_id;
	    }
        int id;
	int iPKDevice;
	int typeBasic;
	int typeGeneric;
	int typeSpecific;
	bool sleepingDevice;
	bool secureDevice;
	int nonceCounter;
	unsigned char nonce[8];
	std::map <int, int>mapCCInstanceCount;
	int plutoDeviceTemplateConst;
	// holds the device state (used by setback schedule thermostat)
	int stateBasic;
	 std::string associationList[4];
    };

    class ZWApi {
      private:
	// this will be our reader/writer thread for the serial port
	static pthread_t readThread;

	// mutex to lock out command queue
	pthread_mutex_t mutexSendQueue;

	int serialPort;

	// queue for sending
	 std::deque < ZWJob * >ZWSendQueue;

	// postpone queue for wakeup
	 std::multimap < int, ZWJob * >ZWWakeupQueue;

	// intent queue to hold nodeids for now because get_node_protocol_info does not return a node id
	 std::deque < ZWIntent * >ZWIntentQueue;

	 std::map < int, ZWNode * >ZWNodeMap;
	 std::map < int, ZWNode * >::iterator ZWNodeMapIt;

	// will be filled with the pluto syntax to report child devices
	 std::string deviceList;

	// counter to get a unique callback id
	int callbackpool;

	// the node id of our dongle
	int ournodeid;

	int maxnodeid;


	// string to hold the routing table
	std::string routingtable;

	// controller dump
	int memory_dump_offset;
	int memory_dump_len;
	int memory_dump_counter;
	unsigned char memory_dump[16384];


	// set true when we await an ACK from the dongle, influences state machine
	bool await_ack;
	// same for callback
	int await_callback;
	// callback type temp var for state handling
	int callback_type;

	// Number of consecutive dropped jobs
	int dropped_jobs;
	void dropSendQueueJob();

	// polling state
	bool poll_state;

	// reference to our ZWave DCE device
	void *myZWave;

	bool wakeupHandler(int nodeid);

	void parseNodeInfo(int nodeid, char *nodeinfo, size_t length);
	void parseManufacturerSpecific(int nodeid, int manuf, int type, int prod);
	std::string commandClassToString(char cclass);
	void updateNodeCapabilities(ZWNode* node, char *nodeinfo, size_t length);

	int getDeviceTemplate(int basic, int generic, int specific,
			      char *nodeinfo, size_t len);

	std::string nodeInfo2String(char *nodeinfo, size_t length, bool bInclControlCC);


      public:
	 ZWApi(void *myZWave);

	~ZWApi();

	// opens the serial port and starts the initalization of the zwave device
	void *init(const char *device);

	// calculate a xor checksum for the zwave frame
	char checksum(char *buf, int len);

	// decodes a frame received from the dongle
	void *decodeFrame(char *frame, size_t length);

	// this is the function for our reader/writer thread, handles frame flow and read/write
	void *receiveFunction();

	// high level intent queue, abused for enumerating the nodes for now
	bool addIntent(int nodeid, int type);
	int getIntent(int type);
	size_t getIntentSize();


	// adds a zwave job to the queue
	bool sendFunction(char *buffer, size_t length, int type,
			  bool response = 0);
	// adds a zwave job to the wake up queue
	bool sendFunctionSleeping(int nodeid, char *buffer, size_t length,
				  int type, bool response = 0);

	// called by the zwave device to receive the deviceList string
	 std::string getDeviceList();

	// used by the ZWave DCE device to call BASIC SET class command
	bool zwBasicSet(int node_id, int level, int instance);

	// multilevel command class v2
	bool zwMultilevelSet(int node_id, int level, int duration, int instance);

	// get the association list for a specific group from a device
	bool zwAssociationGet(int node_id, int group);
	bool zwAssociationSet(int node_id, int group, int target_node_id);
	bool zwAssociationSetMulti(int node_id, int group, int target_node_id, int channel);
	bool zwAssociationRemove(int node_id, int group, int target_node_id);

	bool zwAssignReturnRoute(int src_node_id, int dst_node_id);

	// send a basic report
	bool zwSendBasicReport(int node_id);

	// update the neighbour information of a node
	bool zwRequestNodeNeighborUpdate(int node_id);

	// request a basic report
	void zwRequestBasicReport(int node_id, int instance);

	// called by download configuration to replicate the z-wave network information
	bool zwReplicateController(int mode);

	// used to reset the controller and remove it from the z-wave network
	bool zwSetDefault();

	// add a node to the network
	bool zwAddNodeToNetwork(int startstop, bool highpower);

	// remove a node from the network
	bool zwRemoveNodeFromNetwork(int startstop);
	bool zwRemoveFailedNodeId(int nodeid);

	// configuration_set
	bool zwConfigurationSet(int node_id, int parameter, int value, int size);
	bool zwConfigurationGet(int node_id, int parameter);

	// wakeup set
	bool zwWakeupSet(int node_id, int value, bool multi);

	// check if device powers down the rf part to save power
	bool zwIsSleepingNode(int node_id);

	// we hijack this dce command for now to do some z-wave tests
	void zwStatusReport();

	// battery functions
	void zwGetBatteryLevel(int node_id);

	// clock functions
	void zwSetClock(int node_id);

	// test functions
	void zwReadMemory(int offset, int len);
	void zwWriteMemory(int offset, int len, unsigned char *data);
	void zwControllerBackup();
	void zwControllerRestore();


	void zwRequestManufacturerSpecificReport(int node_id);

	// request multilevel sensor report
	void zwRequestMultilevelSensorReport(int node_id);

	// request the version from a node
	void zwRequestVersion(int node_id);

	// request the node information frame from a node
	void zwRequestNodeInfo(int node_id);

	// toggle polling
	void zwPollDevices(bool onoff);

	// read meter
	bool zwMeterGet(int node_id);

	// thermostat
	// fan mode, 0 - auto/auto low, 1 - on/on low, 2 - auto high, 3 - on high
	bool zwThermostatFanModeSet(int node_id, int fan_mode);

	// 0 off, 1 heat, 2 cool, 3 auto, 4 aux/emer heat, 5 resume, 6 fan only, 7 furnace, 8 dry air, 9 moist air, 10 auto changeover
	bool zwThermostatModeSet(int node_id, int mode);
	void zwThermostatModeGet(int node_id);
	bool zwThermostatSetpointSet(int node_id, int type, int value);
	void zwThermostatSetpointGet(int node_id, int type);

	void zwMultiInstanceGet(int node_id, int command_class);
	void zwRequestMultilevelSensorReportInstance(int node_id,int instance);

	void zwAVControlSet(int node_id, int sequence, int command);
	
	void zwSoftReset();

	bool zwAssignSUCReturnRoute(int node_id);
	bool zwSetPromiscMode(bool promisc);
	void zwGetRoutingInfo(int node_id);

	void zwSecuritySchemeGet(int node_id);
	void zwSecurityNonceGet(int node_id);
	void zwSecurityKeySet(int node_id);

	void zwIsFailedNode(int node_id);
	void resetNodeInstanceCount(ZWNode *node, std::string capa);
	void multiInstanceGetAllCCsForNode(unsigned int node_id);
	void handleCommandSensorMultilevelReport(int nodeid, int instance_id, int sensortype, int metadata,
						 int val1, int val2, int val3, int val4);
    };



}
#endif
