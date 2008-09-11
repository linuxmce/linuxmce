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
#define FUNC_ID_APPLICATION_COMMAND_HANDLER             0x04

#define FUNC_ID_ZW_APPLICATION_UPDATE                   0x49
#define FUNC_ID_ZW_SET_DEFAULT				0x42
#define FUNC_ID_ZW_REPLICATION_COMMAND_COMPLETE         0x44
#define FUNC_ID_ZW_GET_NODE_PROTOCOL_INFO               0x41
#define FUNC_ID_ZW_REQUEST_NODE_NEIGHBOR_UPDATE         0x48
#define FUNC_ID_ZW_SEND_DATA                            0x13
#define FUNC_ID_ZW_SET_LEARN_MODE                       0x50
#define FUNC_ID_ZW_ENABLE_SUC                           0x52
#define FUNC_ID_ZW_SET_SUC_NODE_ID                      0x54
#define FUNC_ID_ZW_GET_SUC_NODE_ID                      0x56
#define FUNC_ID_ZW_ADD_NODE_TO_NETWORK			0x4a
#define FUNC_ID_ZW_REMOVE_NODE_FROM_NETWORK		0x4b

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
#define GENERIC_TYPE_SWITCH_REMOTE                      0x12
#define GENERIC_TYPE_SWITCH_TOGGLE                      0x13
#define GENERIC_TYPE_SENSOR_BINARY                      0x20
#define GENERIC_TYPE_SENSOR_MULTILEVEL                  0x21
#define GENERIC_TYPE_WATER_CONTROL                      0x22
#define GENERIC_TYPE_METER_PULSE                        0x30
#define GENERIC_TYPE_ENTRY_CONTROL                      0x40
#define GENERIC_TYPE_SEMI_INTEROPERABLE                 0x50
#define GENERIC_TYPE_NON_INTEROPERABLE                  0xFF

#define COMMAND_CLASS_BASIC				0x20
#define BASIC_SET					0x01
#define BASIC_GET					0x02
#define BASIC_REPORT					0x03

#define COMMAND_CLASS_BATTERY				0x80
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
#define CONFIGURATION_SET				0x04

#define COMMAND_CLASS_MANUFACTURER_SPECIFIC		0x72
#define MANUFACTURER_SPECIFIC_GET			0x04
#define MANUFACTURER_SPECIFIC_REPORT			0x05


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
	int typeBasic;
	int typeGeneric;
	int typeSpecific;
	bool sleepingDevice;
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

	// set true when we await an ACK from the dongle, influences state machine
	bool await_ack;
	// same for callback
	int await_callback;
	// callback type temp var for state handling
	int callback_type;

	// reference to our ZWave DCE device
	void *myZWave;

	bool wakeupHandler(int nodeid);

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
	size_t getIntentSize() ;


	// adds a zwave job to the queue
	bool sendFunction(char *buffer, size_t length, int type, bool response = 0);
	// adds a zwave job to the wake up queue
	bool sendFunctionSleeping(int nodeid, char *buffer, size_t length, int type, bool response = 0);

	// called by the zwave device to receive the deviceList string
	 std::string getDeviceList();

	// used by the ZWave DCE device to call BASIC SET class command
	bool zwBasicSet(int node_id, int level);

	// get the association list for a specific group from a device
	bool zwAssociationGet(int node_id, int group);
	bool zwAssociationSet(int node_id, int group, int target_node_id);

	bool zwAssignReturnRoute(int src_node_id, int dst_node_id);

	// send a basic report
	bool zwSendBasicReport(int node_id);

	// update the neighbour information of a node
	bool zwRequestNodeNeighborUpdate(int node_id);

	// request a basic report
	void zwRequestBasicReport(int node_id);

	// called by download configuration to replicate the z-wave network information
	bool zwReplicateController(int mode);

	// used to reset the controller and remove it from the z-wave network
	bool zwSetDefault();

	// add a node to the network
	bool zwAddNodeToNetwork(int startstop,bool highpower);

	// remove a node from the network
	bool zwRemoveNodeFromNetwork(int startstop);

	// configuration_set
	bool zwConfigurationSet(int node_id,int parameter,int value);

	// wakeup set
	bool zwWakeupSet(int node_id,int value, bool multi);

	// check if device powers down the rf part to save power
	bool zwIsSleepingNode(int node_id);

	// we hijack this dce command for now to do some z-wave tests
	void zwStatusReport();

	// test functions
	void zwReadMemory(int offset);
	void zwRequestManufacturerSpecificReport(int node_id); 
    };

}
#endif
