/*
 *      Copyright (C) 2008 Harald Klein <hari@vt100.at>
 *
 *      This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
 *      This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *      of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *      See the GNU General Public License for more details.
 */

#include <sys/time.h>
#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <sys/file.h>

#include <iostream>

#include "Serial.h"
#include "ZWApi.h"

#include "ZWave.h"

#include "Generic_Serial_Device/IOUtils.h"

#include "Gen_Devices/ZWaveBase.h"


extern "C" void *start( void* );

void *start( void *p )
{
ZWApi::ZWApi *base = static_cast<ZWApi::ZWApi*>(p);
base->receiveFunction();
}

ZWApi::ZWApi::ZWApi(void *myZWave) {
	this->myZWave = myZWave;
}

ZWApi::ZWApi::~ZWApi() {
}

void *ZWApi::ZWApi::init(const char *device) {
	char mybuf[1024];

 	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Initialization...");


        callbackpool = 1;
        ournodeid = -1;
        await_ack = 0;

 	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Open serial port %s",device);
	OpenSerialPortEx(device,&serialPort);
        mybuf[0]=NAK;
        WriteSerialStringEx(serialPort,mybuf,1);

	pthread_mutex_init(&mutexSendQueue, NULL);

	static pthread_t readThread;
	pthread_create(&readThread, NULL, start, (void*)this);
	// pthread_create(&readThread, NULL, start, (void*)serialPort);
	//
 	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Get version");
	mybuf[0] = ZW_GET_VERSION;
	sendFunction( mybuf , 1, REQUEST, 0); 

 	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Get home/node id");
	mybuf[0] = ZW_MEMORY_GET_ID;
	sendFunction( mybuf , 1, REQUEST, 0); 

 	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Get capabilities");
	mybuf[0] = FUNC_ID_SERIAL_API_GET_CAPABILITIES;
	sendFunction( mybuf , 1, REQUEST, 0); 
	
 	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Get SUC node id");
	mybuf[0] = FUNC_ID_ZW_GET_SUC_NODE_ID;
	sendFunction( mybuf , 1, REQUEST, 0); 
	
 	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Get init data");
	mybuf[0] = FUNC_ID_SERIAL_API_GET_INIT_DATA;
	sendFunction( mybuf , 1, REQUEST, 0); 


}


char ZWApi::ZWApi::checksum(char *buf, int len) {
	int ret = 0xff;
	for (int i=0;i<len;i++) ret ^= buf[i];
	return ret;

}

void *ZWApi::ZWApi::decodeFrame(char *frame, size_t length) {
	char tempbuf[512];
	char tempbuf2[512];
	if (frame[0] == RESPONSE) {
		switch (frame[1]) {
			case FUNC_ID_ZW_GET_SUC_NODE_ID:
				DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got reply to GET_SUC_NODE_ID, node: %d",frame[2]);
				if (frame[2] == 0) {
					DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"No SUC, we become SUC");
					tempbuf[0]=FUNC_ID_ZW_ENABLE_SUC;
					tempbuf[1]=1; // 0=SUC,1=SIS
					// tempbuf[2]=ZW_SUC_FUNC_BASIC_SUC;
					tempbuf[2]=ZW_SUC_FUNC_NODEID_SERVER;
					sendFunction( tempbuf , 3, REQUEST, 0); 
					tempbuf[0]=FUNC_ID_ZW_SET_SUC_NODE_ID;
					tempbuf[1]=ournodeid;
					tempbuf[2]=1; // TRUE, we want to be SUC/SIS
					tempbuf[3]=0; // no low power
					tempbuf[4]=ZW_SUC_FUNC_NODEID_SERVER;
					sendFunction( tempbuf , 5, REQUEST, 1); 
				}
				break;
			;;
			case ZW_MEMORY_GET_ID:
				DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got reply to ZW_MEMORY_GET_ID, Home id: 0x%02x%02x%02x%02x, our node id: %d",(unsigned char) frame[2],(unsigned char) frame[3],(unsigned char)frame[4],(unsigned char)frame[5],(unsigned char)frame[6]);
				ournodeid = frame[6];
				break;
			;;
			case FUNC_ID_SERIAL_API_GET_INIT_DATA:
				DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got reply to FUNC_ID_SERIAL_API_GET_INIT_DATA:");
				if (frame[4] == MAGIC_LEN) {
					for (int i=5;i<5+MAGIC_LEN;i++) {
						for (int j=0;j<8;j++) {
							// printf("test node %d\n",(i-5)*8+j+1);
							if (frame[i] && (0x01 << j)) {
								DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"found node %d",(i-5)*8+j+1);
								// requesting node protocol information
								// tempbuf[0]=0x60; // req. node info frame - test
								addIntent((i-5)*8+j+1,1);
								tempbuf[0]=FUNC_ID_ZW_GET_NODE_PROTOCOL_INFO;
								tempbuf[1]=(i-5)*8+j+1;
								sendFunction( tempbuf , 2, REQUEST, 0); 
							}
						}
					}
				}
				break;
			;;
			case FUNC_ID_ZW_GET_NODE_PROTOCOL_INFO:
				int tmp_nodeid;
				DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got reply to FUNC_ID_ZW_GET_NODE_PROTOCOL_INFO:");
				tmp_nodeid = getIntent(1);

				// test if node is valid
				if (frame[6] != 0) {
					// printf("***FOUND NODE:\n");
					DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"***FOUND NODE: %d",tmp_nodeid);
					ZWNode *newNode = new ZWNode;
					newNode->plutoDeviceTemplateConst=0;
					newNode->typeBasic = (unsigned char) frame[5];
					newNode->typeGeneric = (unsigned char) frame[6];
					newNode->typeSpecific = (unsigned char) frame[7];
					newNode->stateBasic = -1;
					newNode->associationList[0]="";
					newNode->associationList[1]="";
					newNode->associationList[2]="";
					newNode->associationList[3]="";

					if (((unsigned char)frame[2]) && (0x01 << 7)) {
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"listening node");
						newNode->sleepingDevice = false;
					} else {
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"sleeping node");
						newNode->sleepingDevice = true;
					}
					if (((unsigned char)frame[3]) && (0x01 << 7)) {
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"optional functionality");
					}
					switch (frame[5]) {
						case BASIC_TYPE_CONTROLLER:
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"BASIC TYPE: Controller");
							break;
						;;
						case BASIC_TYPE_STATIC_CONTROLLER:
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"BASIC TYPE: Static Controller");
							break;
						;;
						case BASIC_TYPE_SLAVE:
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"BASIC TYPE: Slave");
							break;
						;;
						case BASIC_TYPE_ROUTING_SLAVE:
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"BASIC TYPE: Routing Slave");
							break;
						;;
						default:
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"BASIC TYPE: %x",frame[5]);
							break;
						;;
					}
					switch (frame[6]) {
						case GENERIC_TYPE_GENERIC_CONTROLLER:
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"GENERIC TYPE: Generic Controller");
							// [internal id] \t [description] \t [room name] \t [device template] \t [floorplan id] \n
							sprintf(tempbuf2, "%d\t\t\t%d\t\n", tmp_nodeid, DEVICETEMPLATE_ZWave_Controller_CONST);
							newNode->plutoDeviceTemplateConst = DEVICETEMPLATE_ZWave_Controller_CONST;
							deviceList += tempbuf2;
							break;
						;;
						case GENERIC_TYPE_STATIC_CONTROLLER:
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"GENERIC TYPE: Static Controller");
							sprintf(tempbuf2, "%d\t\t\t%d\t\n", tmp_nodeid, DEVICETEMPLATE_ZWave_Controller_CONST);
							newNode->plutoDeviceTemplateConst = DEVICETEMPLATE_ZWave_Controller_CONST;
							deviceList += tempbuf2;
							break;
						;;
						case GENERIC_TYPE_THERMOSTAT:
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"GENERIC TYPE: Thermostat");
							sprintf(tempbuf2, "%d\t\t\t%d\t\n", tmp_nodeid, DEVICETEMPLATE_Standard_Thermostat_CONST);
							newNode->plutoDeviceTemplateConst = DEVICETEMPLATE_Standard_Thermostat_CONST;
							deviceList += tempbuf2;
							break;
						;;
						case GENERIC_TYPE_SWITCH_MULTILEVEL:
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"GENERIC TYPE: Multilevel Switch");
							sprintf(tempbuf2, "%d\t\t\t%d\t\n", tmp_nodeid, DEVICETEMPLATE_Light_Switch_dimmable_CONST);
							newNode->plutoDeviceTemplateConst = DEVICETEMPLATE_Light_Switch_dimmable_CONST;
							deviceList += tempbuf2;
							break;
						;;
						case GENERIC_TYPE_SWITCH_REMOTE:
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"GENERIC TYPE: Remote Switch");
							// we read the associations from groups one and two (left and right paddle on the ACT remote switch)
							zwAssociationGet(tmp_nodeid,1);
							zwAssociationGet(tmp_nodeid,2);
							
							break;
						;;
						case GENERIC_TYPE_SWITCH_BINARY:
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"GENERIC TYPE: Binary Switch");
							sprintf(tempbuf2, "%d\t\t\t%d\t\n", tmp_nodeid, DEVICETEMPLATE_Light_Switch_onoff_CONST);
							newNode->plutoDeviceTemplateConst = DEVICETEMPLATE_Light_Switch_onoff_CONST;
							deviceList += tempbuf2;
							break;
						;;
						case GENERIC_TYPE_SENSOR_BINARY:
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"GENERIC TYPE: Sensor Binary");
							sprintf(tempbuf2, "%d\t\t\t%d\t\n", tmp_nodeid, DEVICETEMPLATE_Generic_Sensor_CONST);
							newNode->plutoDeviceTemplateConst = DEVICETEMPLATE_Generic_Sensor_CONST;
							deviceList += tempbuf2;
							break;
						case GENERIC_TYPE_WINDOW_COVERING:
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"GENERIC TYPE: Window Covering");
							sprintf(tempbuf2, "%d\t\t\t%d\t\n", tmp_nodeid, DEVICETEMPLATE_Drapes_Switch_CONST);
							newNode->plutoDeviceTemplateConst = DEVICETEMPLATE_Drapes_Switch_CONST;
							deviceList += tempbuf2;
							break;
						;;
						default:
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"GENERIC TYPE: %x",frame[6]);
							break;
						;;

					}
					DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"SPECIFIC TYPE: %d",frame[7]);

					ZWNodeMap.insert(std::map < int, ZWNode * >::value_type(tmp_nodeid,newNode));

				} else {
					DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Invalid generic class (%i), ignoring device",(unsigned char)frame[6]);
				}
				if (getIntentSize() == 0) {
					// we got all protcol info responses
					DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Finished building node list:");
					ZWNodeMapIt = ZWNodeMap.begin();
					while (ZWNodeMapIt!=ZWNodeMap.end()) {
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Node: %i basic: %i generic: %i specific: %i pluto: %i",(*ZWNodeMapIt).first,(*ZWNodeMapIt).second->typeBasic,(*ZWNodeMapIt).second->typeGeneric,(*ZWNodeMapIt).second->typeSpecific,(*ZWNodeMapIt).second->plutoDeviceTemplateConst);
						ZWNodeMapIt++;
		
					}
				
					DCE::ZWave *tmp_zwave = static_cast<DCE::ZWave*>(myZWave);
					tmp_zwave->CMD_Report_Child_Devices();
					// tmp_zwave->EVENT_Reporting_Child_Devices("",getDeviceList() );	
					
				}
				break;
			;;
			case FUNC_ID_ZW_SEND_DATA:
				switch(frame[2]) {
					case 1:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ZW_SEND delivered to Z-Wave stack");
						break;
					case 0:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ERROR: ZW_SEND could not be delivered to Z-Wave stack");
						break;
					default:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ERROR: ZW_SEND Response is invalid!");
				}

				break;

			default:
				DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"TODO: handle response for %i ",(unsigned char)frame[1]);
				break;
			;;
		}

	} else if (frame[0] == REQUEST) {

		switch (frame[1]) {
			case FUNC_ID_ZW_SEND_DATA:
				DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ZW_SEND Response with callback %i received",(unsigned char)frame[2]);
				if ((unsigned char)frame[2] != await_callback) {
					// wrong callback id
					DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ERROR: callback id is invalid!");

				} else {
					// callback id matches our expectation
					switch(frame[3]) {
						case 1:
							// zwsend failed
							pthread_mutex_lock (&mutexSendQueue);
							if (ZWSendQueue.front()->sendcount > 3) {
								// can't deliver frame, abort
								DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Error: ZW_SEND failed, removing job after three tries");
								ZWSendQueue.pop_front();
							} else {
								DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Error: ZW_SEND failed, retrying");
								// trigger resend
								await_ack = 0;
							}
							await_callback = 0;
							pthread_mutex_unlock (&mutexSendQueue);
							break;
						case 0:
							// command reception acknowledged by node
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ZW_SEND was successful, removing job");
							pthread_mutex_lock (&mutexSendQueue);
							ZWSendQueue.pop_front();
							pthread_mutex_unlock (&mutexSendQueue);
							await_callback = 0;
							
							break;
						default:
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ERROR: ZW_SEND Response is invalid!");

					}
				}
				break;
			case FUNC_ID_ZW_ADD_NODE_TO_NETWORK:
				DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"FUNC_ID_ZW_ADD_NODE_TO_NETWORK:");
				switch (frame[3]) {
					case ADD_NODE_STATUS_LEARN_READY:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ADD_NODE_STATUS_LEARN_READY");
						break;
					case ADD_NODE_STATUS_NODE_FOUND:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ADD_NODE_STATUS_NODE_FOUND");
						break;
					case ADD_NODE_STATUS_ADDING_SLAVE:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ADD_NODE_STATUS_ADDING_SLAVE");
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Adding node id %i",(unsigned int)frame[4]);
						if (((unsigned int)frame[7] == 8) && ((unsigned int)frame[8] == 3)) {
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Setback schedule thermostat detected, triggering configuration");
							zwWakeupSet((unsigned char)frame[4],15,true);
						}
						// finish adding node	
						zwAddNodeToNetwork(0,false);
						break;
					case ADD_NODE_STATUS_ADDING_CONTROLLER:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ADD_NODE_STATUS_ADDING_CONTROLLER");
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Adding node id %i",(unsigned int)frame[4]);
						break;
					case ADD_NODE_STATUS_PROTOCOL_DONE:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ADD_NODE_STATUS_PROTOCOL_DONE");
						// we send no replication info for now
						zwAddNodeToNetwork(0,false);

						break;
					case ADD_NODE_STATUS_DONE:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ADD_NODE_STATUS_DONE");
						break;
					case ADD_NODE_STATUS_FAILED:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ADD_NODE_STATUS_FAILED");
						break;
					default:
						break;
				}
				break;
			case FUNC_ID_ZW_REMOVE_NODE_FROM_NETWORK:
				DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"FUNC_ID_ZW_REMOVE_NODE_FROM_NETWORK:");
				switch (frame[3]) {
					case REMOVE_NODE_STATUS_LEARN_READY:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"REMOVE_NODE_STATUS_LEARN_READY");
						break;
					case REMOVE_NODE_STATUS_NODE_FOUND:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"REMOVE_NODE_STATUS_NODE_FOUND");
						break;
					case REMOVE_NODE_STATUS_ADDING_SLAVE:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"REMOVE_NODE_STATUS_ADDING_SLAVE");
						// DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ing node id %i",(unsigned int)frame[4]);
						// finish adding node	
						zwRemoveNodeFromNetwork(0);
						break;
					case REMOVE_NODE_STATUS_ADDING_CONTROLLER:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"REMOVE_NODE_STATUS_ADDING_CONTROLLER");
						// DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ing node id %i",(unsigned int)frame[4]);
						break;
					case REMOVE_NODE_STATUS_PROTOCOL_DONE:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"REMOVE_NODE_STATUS_PROTOCOL_DONE");
						zwRemoveNodeFromNetwork(0);

						break;
					case REMOVE_NODE_STATUS_DONE:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"REMOVE_NODE_STATUS_DONE");
						break;
					case REMOVE_NODE_STATUS_FAILED:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"REMOVE_NODE_STATUS_FAILED");
						break;
					default:
						break;
				}
				break;

			case FUNC_ID_APPLICATION_COMMAND_HANDLER:
				DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"FUNC_ID_APPLICATION_COMMAND_HANDLER:");
				switch (frame[5]) {
					case COMMAND_CLASS_CONTROLLER_REPLICATION:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_CONTROLLER_REPLICATION");
						// 0x1 0xb 0x0 0x4 0x2 0xef 0x5 0x21 0x31 0x7 0x1 0x1 0xf (#######!1####)
						if (frame[6] == CTRL_REPLICATION_TRANSFER_GROUP) {
							// we simply ack the group information for now
							tempbuf[0]=FUNC_ID_ZW_REPLICATION_COMMAND_COMPLETE;
							sendFunction( tempbuf , 1, REQUEST, 0); 
						} else {
							// ack everything else, too
							tempbuf[0]=FUNC_ID_ZW_REPLICATION_COMMAND_COMPLETE;
							sendFunction( tempbuf , 1, REQUEST, 0); 
						}
						break;
					;;
					case COMMAND_CLASS_WAKE_UP:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_WAKE_UP - ");
						// 0x1 0x8 0x0 0x4 0x4 0x2 0x2 0x84 0x7 0x74 (#########t)
						// if ((COMMAND_CLASS_WAKE_UP == frame[5]) && ( frame[6] == WAKE_UP_NOTIFICATION)) {
						if (frame[6] == WAKE_UP_NOTIFICATION) {
							// we got a wake up frame, make sure we remember the device does not always listen
							ZWNodeMapIt = ZWNodeMap.find((unsigned int)frame[3]);
							if (ZWNodeMapIt != ZWNodeMap.end()) {
								(*ZWNodeMapIt).second->sleepingDevice=true;
							}

							// inject commands from the sleeping queue for this nodeid
							wakeupHandler((unsigned char) frame[3]);	

							// handle broadcasts from unconfigured devices
							if (frame[2] && RECEIVE_STATUS_TYPE_BROAD ) { 
								DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got broadcast wakeup from node %i, doing WAKE_UP_INTERVAL_SET",frame[3]);
								if (ournodeid != -1) { 
									// we assume an ACT PIR for now, no other known devices show that behavior
									zwAssociationSet((unsigned char)frame[3], 1, ournodeid); // receive group 1 events (motion)
									zwAssociationSet((unsigned char)frame[3], 2, ournodeid); // receive group 2 events (casing opened)
									zwAssociationSet((unsigned char)frame[3], 3, ournodeid); // receive group 3 events (battery reports)
									zwConfigurationSet((unsigned char)frame[3],17,2); // sensor mode
									zwConfigurationSet((unsigned char)frame[3],18,0); // no delay
									zwConfigurationSet((unsigned char)frame[3],19,1); // send unsolicited events
									zwConfigurationSet((unsigned char)frame[3],22,30); // wakeup duration
									zwWakeupSet((unsigned char)frame[3],60,false); // wakeup interval
									wakeupHandler((unsigned char) frame[3]); // fire commands, device is awake
								}
							} else {
								DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got unicast wakeup from node %i, doing WAKE_UP_NO_MORE_INFORMATION",frame[3]);
								// send to sleep
								tempbuf[0]=FUNC_ID_ZW_SEND_DATA;
								tempbuf[1]=frame[3]; // destination
								tempbuf[2]=2; // length
								tempbuf[3]=COMMAND_CLASS_WAKE_UP;
								tempbuf[4]=WAKE_UP_NO_MORE_INFORMATION;
								tempbuf[5]=TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
								sendFunction( tempbuf , 6, REQUEST, 1); 
								// ...
							}//
						}
						break;
					case COMMAND_CLASS_SENSOR_BINARY:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_SENSOR_BINARY - ");
						if (frame[6] == SENSOR_BINARY_REPORT) {
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got sensor report from node %i, level: %i",(unsigned char)frame[3],(unsigned char)frame[7]);
							DCE::ZWave *tmp_zwave = static_cast<DCE::ZWave*>(myZWave);
							if ((unsigned char)frame[7] == 0xff) {
								tmp_zwave->SendSensorTrippedEvents (frame[3],true);
							} else {
								tmp_zwave->SendSensorTrippedEvents (frame[3],false);
							}

						}
						break;
					;;
					case COMMAND_CLASS_SWITCH_MULTILEVEL:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_SWITCH_MULTILEVEL - ");
						if (frame[6] == SWITCH_MULTILEVEL_REPORT) {
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got switch multilevel report from node %i, level: %i",frame[3],frame[7]);
							DCE::ZWave *tmp_zwave = static_cast<DCE::ZWave*>(myZWave);
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Send light changed event");
							tmp_zwave->SendLightChangedEvents (frame[3],(unsigned char)frame[7]);
						}
						break;
					case COMMAND_CLASS_SWITCH_ALL:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_SWITCH_ALL - ");
						if (frame[6] == SWITCH_ALL_ON) {
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got switch all ON from node %i",frame[3]);
							DCE::ZWave *tmp_zwave = static_cast<DCE::ZWave*>(myZWave);
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Send light changed event");
							tmp_zwave->SendLightChangedEvents (0,99);
						}
						if (frame[6] == SWITCH_ALL_OFF) {
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got switch all OFF from node %i",frame[3]);
							DCE::ZWave *tmp_zwave = static_cast<DCE::ZWave*>(myZWave);
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Send light changed event");
							tmp_zwave->SendLightChangedEvents (0,0);
						}
						break;
					case COMMAND_CLASS_ALARM:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_ALARM - ");
						if (frame[6] == ALARM_REPORT) {
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got ALARM from node %i, type: %i, level: %i",frame[3],frame[7],frame[8]);
							DCE::ZWave *tmp_zwave = static_cast<DCE::ZWave*>(myZWave);
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Send sensor tripped changed event");
							tmp_zwave->SendSensorTrippedEvents (frame[3],true);
							tmp_zwave->SendSensorTrippedEvents (frame[3],false);
						}
						break;
					case COMMAND_CLASS_BASIC:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_BASIC - ");
						if (frame[6] == BASIC_REPORT) {

							// char tmp[32];
							// sprintf(&tmp,"%d",frame[7]);
							//
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got basic report from node %i, value: %i",frame[3],(unsigned char)frame[7]);
							ZWNodeMapIt = ZWNodeMap.find((unsigned int)frame[3]);
							if (ZWNodeMapIt != ZWNodeMap.end()) {
								if ((*ZWNodeMapIt).second->stateBasic != (unsigned char)frame[7]) {
									(*ZWNodeMapIt).second->stateBasic = (unsigned char)frame[7];
									DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"State changed, send light changed event");
									DCE::ZWave *tmp2_zwave = static_cast<DCE::ZWave*>(myZWave);
									tmp2_zwave->SendLightChangedEvents (frame[3],(unsigned char)frame[7]);

								}
							}

						} else if ((unsigned char)frame[6] == BASIC_SET) {
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got COMMAND_CLASS_BASIC:BASIC_SET, level %i",(unsigned char)frame[7]);
							ZWNodeMapIt = ZWNodeMap.find((unsigned int)frame[3]);
							if (ZWNodeMapIt != ZWNodeMap.end()) {
								if ((*ZWNodeMapIt).second->typeGeneric == GENERIC_TYPE_SWITCH_REMOTE) {
									DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"This is a remote switch, we will traverse the association list and request reports");
									const char *tmp_str = (*ZWNodeMapIt).second->associationList[1].c_str();
									char *pch = strtok((char*)tmp_str,",");
									while (pch != NULL) {
										zwRequestBasicReport(atoi(pch));
										pch = strtok (NULL, ",");
									}
										
									
								}
							}
							
						} else {
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got COMMAND_CLASS_BASIC: %i, ignoring",(unsigned char)frame[6]);
									
							// zwSendBasicReport((unsigned char)frame[3]);
						}						
						break;
					case COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE: - ");
						if (frame[6] == SCHEDULE_GET) {
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got SCHEDULE_GET from node %i for day: %i",frame[3],frame[7]);
							// report no schedules for the given day
							tempbuf[0]=FUNC_ID_ZW_SEND_DATA;
							// node id
							tempbuf[1]=frame[3];
							tempbuf[2]=5;
							tempbuf[3]=COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE;
							tempbuf[4]=SCHEDULE_SET;
							tempbuf[5]=frame[7];
							tempbuf[6]=0;
							tempbuf[7]=0x7f; // unused state, end setpoints
							tempbuf[8]=TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
							sendFunction( tempbuf , 9, REQUEST, 1); 

						}
						break;
					case COMMAND_CLASS_ASSOCIATION:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_ASSOCIATION - ");
						if (frame[6] == ASSOCIATION_REPORT) {
							int tmp_group = (unsigned char)frame[7];
							std::string tmp_nodelist = "";
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Associations for group: %i",tmp_group);
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Max nodes supported: %i",(unsigned char)frame[8]);
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Reports to follow: %i",(unsigned char)frame[9]);
							if ((length-10)>0) {
								DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Nodes: ");
								for (int i=0;i<(length-10);i++) {
									DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"%i",(unsigned char)frame[10+i]);
									if (tmp_nodelist.length()!=0) {
										tmp_nodelist += ',';
									}
									char tmp_string[1024];
									sprintf(tmp_string,"%d",(unsigned char)frame[10+i]);
									tmp_nodelist += tmp_string;

								}
								DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Built nodelist: %s",tmp_nodelist.c_str());
							}
							ZWNodeMapIt = ZWNodeMap.find((unsigned int)frame[3]);
							if ((ZWNodeMapIt != ZWNodeMap.end()) && (tmp_group<5) && (tmp_group > 0)) {
								(*ZWNodeMapIt).second->associationList[tmp_group-1]=tmp_nodelist;
							}
						}
						break;
					case COMMAND_CLASS_MULTI_CMD:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_MULTI_CMD - ");
						if (frame[6] == MULTI_CMD_ENCAP) {
							time_t timestamp;
							struct tm *timestruct;
							int offset = 0;
							// int cmd_length = 0;
							// int cmd_count = 0;

							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got encapsulated multi command from node %i",frame[3]);
							timestamp = time(NULL);
							timestruct = localtime(&timestamp);
							// printf("Time: %i %i %i\n",timestruct->tm_wday, timestruct->tm_hour, timestruct->tm_min);
							// iterate over commands
							offset = 8;	
							for (int i=0; i<frame[7]; i++) {
								DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND LENGTH: %d, CLASS: %x",frame[offset],(unsigned char) frame[offset+1]);
								switch (frame[offset+1]) {
									case COMMAND_CLASS_BATTERY:
										if (BATTERY_REPORT == (unsigned char) frame[offset+2]) {
											DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_BATTERY:BATTERY_REPORT: Battery level: %d",frame[offset+3]);
										}
										break;
									case COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE:
										if (SCHEDULE_CHANGED_GET == (unsigned char) frame[offset+2]) {
											DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE:SCHEDULE_CHANGED_GET");
										}
										if (SCHEDULE_OVERRIDE_GET == (unsigned char) frame[offset+2]) {
											DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE:SCHEDULE_OVERRIDE_GET");
										}
										if (SCHEDULE_OVERRIDE_REPORT == (unsigned char) frame[offset+2]) {
											DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE:SCHEDULE_OVERRIDE_REPORT: Setback state: %i",(unsigned char)frame[offset+4]);
											// update basic device state in map
											ZWNodeMapIt = ZWNodeMap.find((unsigned int)frame[3]);
											if (ZWNodeMapIt != ZWNodeMap.end()) {
												(*ZWNodeMapIt).second->stateBasic = (unsigned char)frame[offset+4]==0 ? 0xff : 0x0;
											}
										}
										break;
									case COMMAND_CLASS_CLOCK:
										if (CLOCK_GET == (unsigned char) frame[offset+2]) {
                                                                                        DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_CLOCK:CLOCK_GET");
										}
										break;
									case COMMAND_CLASS_WAKE_UP:
										if (WAKE_UP_NOTIFICATION == (unsigned char) frame[offset+2]) {
                                                                                        DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_WAKE_UP:WAKE_UP_NOTIFICATION");
										}	
										break;
									default:
									;;
								}
								offset += frame[offset]+1;
							}
							tempbuf[0]=FUNC_ID_ZW_SEND_DATA;
							tempbuf[1]=frame[3]; // node id
							tempbuf[2]=19; // cmd_length;
							tempbuf[3]=COMMAND_CLASS_MULTI_CMD;
							tempbuf[4]=MULTI_CMD_ENCAP;
							tempbuf[5]=4; // cmd_count; 
							tempbuf[6]=4; //length of next command
							tempbuf[7]=COMMAND_CLASS_CLOCK;
							tempbuf[8]=CLOCK_REPORT;
							tempbuf[9]=((timestruct->tm_wday == 0 ? 7 : timestruct->tm_wday) << 5 ) | timestruct->tm_hour;
							tempbuf[10]=timestruct->tm_min == 60 ? 0 : timestruct->tm_min;
							tempbuf[11]=3; // length of next command
							tempbuf[12]=COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE;
							tempbuf[13]=SCHEDULE_CHANGED_REPORT;
							tempbuf[14]=17; // dummy schedule version for now
							tempbuf[15]=3; // length of next command
							tempbuf[16]=COMMAND_CLASS_BASIC;
							tempbuf[17]=BASIC_SET;
							ZWNodeMapIt = ZWNodeMap.find((unsigned int)frame[3]);
							if (ZWNodeMapIt != ZWNodeMap.end()) {
								tempbuf[18]=(*ZWNodeMapIt).second->stateBasic;
							} else {
								tempbuf[18]=0x00;
							}
							tempbuf[19]=2; //length of next command
							tempbuf[20]=COMMAND_CLASS_WAKE_UP;
							tempbuf[21]=WAKE_UP_NO_MORE_INFORMATION;
							tempbuf[22]=TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
							sendFunction( tempbuf , 23, REQUEST, 1); 
 
						}
						break;
					default:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Function not implemented - unhandled command class");
						break;
					;;


				}
				break;
			;;
			case FUNC_ID_ZW_APPLICATION_UPDATE:
				switch(frame[2]) {
					case UPDATE_STATE_NODE_INFO_RECEIVED:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"FUNC_ID_ZW_APPLICATION_UPDATE:UPDATE_STATE_NODE_INFO_RECEIVED received from node %d - ",frame[3]);
						switch(frame[5]) {
							case BASIC_TYPE_ROUTING_SLAVE:
							case BASIC_TYPE_SLAVE:
								//DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"BASIC_TYPE_SLAVE:");
								switch(frame[6]) {
									case GENERIC_TYPE_SWITCH_MULTILEVEL:
										DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"GENERIC_TYPE_SWITCH_MULTILEVE");
										tempbuf[0] = FUNC_ID_ZW_SEND_DATA;
										tempbuf[1] = frame[3];
										tempbuf[2] = 0x02;
										tempbuf[3] = COMMAND_CLASS_SWITCH_MULTILEVEL;
										tempbuf[4] = SWITCH_MULTILEVEL_GET;
										tempbuf[5] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
										sendFunction (tempbuf,6,REQUEST,1);
										tempbuf[0] = FUNC_ID_ZW_SEND_DATA;
										tempbuf[1] = frame[3];
										tempbuf[2] = 0x02;
										tempbuf[3] = COMMAND_CLASS_BASIC;
										tempbuf[4] = BASIC_GET;
										tempbuf[5] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
										sendFunction (tempbuf,6,REQUEST,1);
										break;
									case GENERIC_TYPE_SWITCH_BINARY:
										DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"GENERIC_TYPE_SWITCH_BINARY");
										tempbuf[0] = FUNC_ID_ZW_SEND_DATA;
										tempbuf[1] = frame[3];
										tempbuf[2] = 0x02;
										tempbuf[3] = COMMAND_CLASS_BASIC;
										tempbuf[4] = BASIC_GET;
										tempbuf[5] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
										sendFunction (tempbuf,6,REQUEST,1);
										break;
									case GENERIC_TYPE_SWITCH_REMOTE:
										ZWNodeMapIt = ZWNodeMap.find((unsigned int)frame[3]);
										if (ZWNodeMapIt != ZWNodeMap.end()) {
											if ((*ZWNodeMapIt).second->typeGeneric == GENERIC_TYPE_SWITCH_REMOTE) {
												DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"This is a remote switch, we will traverse the association list and request reports");
												// we receive that on a left paddle press on the ACT remote switch, so get first group
												const char *tmp_str = (*ZWNodeMapIt).second->associationList[0].c_str();
												char *pch = strtok((char*)tmp_str,",");
												while (pch != NULL) {
													zwRequestBasicReport(atoi(pch));
													pch = strtok (NULL, ",");
												}
													
												
											}
										}
										break;
									default:
										DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"unhandled class");
										break;
									;;
								}
							default:
								break;
							;;

						}
						break;
					case UPDATE_STATE_NODE_INFO_REQ_FAILED:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"FUNC_ID_ZW_APPLICATION_UPDATE:UPDATE_STATE_NODE_INFO_REQ_FAILED received");
						break;
					;;
					default:
						break;
					;;
				}	
				break;
			;;
			default:
			;;


		}

		// generic callback handling
		if (await_callback != 0 ) {
			if (callback_type == (unsigned char)frame[1]) {
				DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Generic callback handling for command %i, removing job",(unsigned char)frame[1]);
				pthread_mutex_lock (&mutexSendQueue);
				ZWSendQueue.pop_front();
				pthread_mutex_unlock (&mutexSendQueue);
				await_callback = 0;
			} else {
				DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Generic callback handling for command %i, ERROR: wrong callback type: %i",(unsigned char)frame[1],callback_type);

			}

		}


	} else {
		// should not happen
	}
}

void *ZWApi::ZWApi::receiveFunction() {
	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"receiveFunction started");
	long timer;
	long idletimer;
	char retbuf[1];
	char mybuf2[1024];
	size_t len2;
	while (1) {
		// read a byte from the serial port
		mybuf2[0]=0;
		len2 = ReadSerialStringEx(serialPort,mybuf2, 1, 100);
		// len2 = serialPort->Read(mybuf2, 1, 100);
		if (len2 == 1) {
			switch(mybuf2[0]) {
				case SOF:
					// printf("SOF Found\n");
					// if we await an ack instead, trigger resend to be sure
					if (await_ack) {
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Error: SOF Found while awaiting ack.. triggering resend");
						await_ack = 0;
					}
					// read the length byte
					len2 += ReadSerialStringEx(serialPort,mybuf2+1, 1, 100);
					// printf("Packet Length: %i\n",mybuf2[1]);

					// read the rest of the frame
					len2 += ReadSerialStringEx(serialPort,mybuf2+2,mybuf2[1],100);

					if (len2>0) DCE::LoggerWrapper::GetInstance()->Write(LV_RECEIVE_DATA, DCE::IOUtils::FormatHexAsciiBuffer(mybuf2, len2,"33").c_str());
					// verify checksum
					if ((unsigned char) mybuf2[len2-1] == (unsigned char) checksum(mybuf2+1,mybuf2[1])) {
						// printf("Checksum correct - sending ACK\n");
						retbuf[0]=ACK;
						WriteSerialStringEx(serialPort,retbuf,1);
						decodeFrame(mybuf2+2,len2-3);
					} else {
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Checksum incorrect %x - sending NAK",(unsigned char) checksum(mybuf2+1,mybuf2[1]));
						retbuf[0]=NAK;
						WriteSerialStringEx(serialPort,retbuf,1);

					}
					break;
				;;
				case CAN:
					DCE::LoggerWrapper::GetInstance()->Write(LV_RECEIVE_DATA, "CAN RECEIVED");
						// trigger resend
						await_ack = 0;
					break;
				;;
				case NAK:
					DCE::LoggerWrapper::GetInstance()->Write(LV_RECEIVE_DATA, "NAK RECEIVED");
						// trigger resend
						await_ack = 0;
					break;
				;;
				case ACK:
					DCE::LoggerWrapper::GetInstance()->Write(LV_RECEIVE_DATA, "ACK RECEIVED");
					// if we await an ack pop the command, it got an ACK
					if (await_ack) {
						await_ack = 0;
					}
					if (await_callback == 0) {
						pthread_mutex_lock (&mutexSendQueue);
						ZWSendQueue.pop_front();
						pthread_mutex_unlock (&mutexSendQueue);
					}
					break;
				;;
				default:
					DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "ERROR! Out of frame flow!!");
				;;
			}
			idletimer=0;
		} else {
			// nothing received, let's see if there is a job to send
			pthread_mutex_lock (&mutexSendQueue);
			if (ZWSendQueue.size()>0) {
				idletimer=0;
				if ( await_ack != 1 && await_callback == 0) {
					// printf("Elements on queue: %i\n",ZWSendQueue.size());
					// printf("Pointer: %p\n",ZWSendQueue.front());
					await_callback = (unsigned int) ZWSendQueue.front()->callbackid;
					callback_type = (unsigned int) ZWSendQueue.front()->callback_type;
					DCE::LoggerWrapper::GetInstance()->Write(LV_SEND_DATA, "Sending job %p (cb %i) - %s",ZWSendQueue.front(),await_callback,DCE::IOUtils::FormatHexAsciiBuffer(ZWSendQueue.front()->buffer, ZWSendQueue.front()->len,"31").c_str());

					WriteSerialStringEx(serialPort,ZWSendQueue.front()->buffer, ZWSendQueue.front()->len);
					ZWSendQueue.front()->sendcount++;
					await_ack = 1;
					timer=0;
				} else {
					// DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "QUEUE FILLED: await_callback: %i timer: %i",await_callback,timer++);
					timer++;
					if (timer > 30 && await_callback != 0) {
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "No callback received: await_callback: %i timer: %i",await_callback,timer);
						timer = 0;
						// resend, we got no final callback 
						await_ack = 0;	
						await_callback = 0;
						if (ZWSendQueue.front()->sendcount > 2) {
							ZWSendQueue.pop_front();
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "ERROR: Dropping command, no callback received after three resends");

						}
					}


				}
				
			} else {
				idletimer++;
				if ((idletimer % 100) == 0) {
					DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "IDLE timer: %i",idletimer);

				}

			}
			pthread_mutex_unlock (&mutexSendQueue);

			// we have been idle for 30 seconds, let's poll the devices
			if (idletimer > 300) {
				DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "We have been idle for %i seconds, polling device states", idletimer / 10);
				zwRequestBasicReport(NODE_BROADCAST);
			}

		}
	}
}

bool ZWApi::ZWApi::sendFunction(char *buffer, size_t length, int type, bool response) {
	ZWJob *newJob;
	newJob = new ZWJob;
	int i, index;
	index = 0;
	i = 0;

	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Adding job: %p",newJob);
	newJob->await_response = response && (type == REQUEST);
	newJob->sendcount = 0;

	newJob->buffer[index++] = SOF;
	newJob->buffer[index++] = length + 2 + (response ? 1 : 0);
	newJob->buffer[index++] = type == RESPONSE ? RESPONSE : REQUEST;
	for (i=0;i<length;i++) newJob->buffer[index++] = buffer[i];
	newJob->len = length + 4 + (response ? 1 : 0);
	pthread_mutex_lock (&mutexSendQueue);
	if (response) {
		if (callbackpool>255 || callbackpool==0) { callbackpool=1; }
		newJob->buffer[index++] = callbackpool;
		newJob->callbackid = callbackpool++;
		newJob->callback_type = (unsigned int)newJob->buffer[3];
		// newJob->callback_type = (unsigned int)buffer[3];
	} else {
		newJob->callbackid = 0;
		newJob->callback_type = 0;
	}
	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Setting callback type: %i",newJob->callback_type);

	newJob->buffer[index] = checksum(newJob->buffer+1,length+2+( response ? 1 : 0) );
	ZWSendQueue.push_back(newJob);
	pthread_mutex_unlock (&mutexSendQueue);
	return true;
}

bool ZWApi::ZWApi::sendFunctionSleeping(int nodeid, char *buffer, size_t length, int type, bool response) {
	ZWJob *newJob;
	newJob = new ZWJob;
	int i, index;
	index = 0;
	i = 0;

	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Adding job to sleeping queue: %p",newJob);
	newJob->await_response = response && (type == REQUEST);
	newJob->sendcount = 0;

	newJob->buffer[index++] = SOF;
	newJob->buffer[index++] = length + 2 + (response ? 1 : 0);
	newJob->buffer[index++] = type == RESPONSE ? RESPONSE : REQUEST;
	for (i=0;i<length;i++) newJob->buffer[index++] = buffer[i];
	newJob->len = length + 4 + (response ? 1 : 0);
	pthread_mutex_lock (&mutexSendQueue);
	if (response) {
		if (callbackpool>255 || callbackpool==0) { callbackpool=1; }
		newJob->buffer[index++] = callbackpool;
		newJob->callbackid = callbackpool++;
		newJob->callback_type = (unsigned int)buffer[3];
	} else {
		newJob->callbackid = 0;
		newJob->callback_type = 0;
	}

	newJob->buffer[index] = checksum(newJob->buffer+1,length+2+( response ? 1 : 0) );
	ZWWakeupQueue.insert(std::multimap < int, ZWJob * >::value_type(nodeid,newJob));
	pthread_mutex_unlock (&mutexSendQueue);
	return true;


}


bool ZWApi::ZWApi::addIntent(int nodeid, int type) {
	ZWIntent *newIntent;
	newIntent = new ZWIntent;

	newIntent->nodeid = nodeid;
	newIntent->type = type;

	pthread_mutex_lock (&mutexSendQueue);
	ZWIntentQueue.push_back(newIntent);
	pthread_mutex_unlock (&mutexSendQueue);
	return true;

}

int ZWApi::ZWApi::getIntent(int type) {
	int nodeid = -1;

	if (ZWIntentQueue.size()>0) {
		// printf("Elements on queue: %i\n",ZWSendQueue.size());
		// printf("Pointer: %p\n",ZWSendQueue.front());
		nodeid = ZWIntentQueue.front()->nodeid;
		pthread_mutex_lock (&mutexSendQueue);
		ZWIntentQueue.pop_front();
		pthread_mutex_unlock (&mutexSendQueue);
	}
	return nodeid;
}
size_t ZWApi::ZWApi::getIntentSize() {
	size_t size;
	pthread_mutex_lock (&mutexSendQueue);
	size = ZWIntentQueue.size();	
	pthread_mutex_unlock (&mutexSendQueue);
	return size;
}

std::string ZWApi::ZWApi::getDeviceList() {
	return deviceList;
}

bool ZWApi::ZWApi::zwBasicSet(int node_id, int level) {
	char mybuf[1024];

	ZWNodeMapIt = ZWNodeMap.find(node_id);
	if (ZWNodeMapIt != ZWNodeMap.end()) {
		// check if it is a setback schedule thermostat
		if ( ((*ZWNodeMapIt).second->typeGeneric == 8) && ((*ZWNodeMapIt).second->typeSpecific == 3) ) {
			// only set the stateBasic, later gets sent as multi command on wakeup
			DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Setback schedule override prepared");
			(*ZWNodeMapIt).second->stateBasic = level == 0 ? 0 : 0xff;

		} else {

			mybuf[0] = FUNC_ID_ZW_SEND_DATA;
			mybuf[1] = node_id;
			mybuf[2] = 3;
			mybuf[3] = COMMAND_CLASS_BASIC;
			mybuf[4] = BASIC_SET;
			mybuf[5] = level;
			mybuf[6] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
			sendFunction( mybuf , 7, REQUEST, 1);

		}
	}


}

bool ZWApi::ZWApi::zwSendBasicReport(int node_id) {
	char mybuf[1024];

        mybuf[0] = FUNC_ID_ZW_SEND_DATA;
	mybuf[1] = node_id;
	mybuf[2] = 3;
	mybuf[3] = COMMAND_CLASS_BASIC;
	mybuf[4] = BASIC_REPORT;
	mybuf[5] = 0x00;
	mybuf[6] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
	sendFunction( mybuf , 7, REQUEST, 1);

}

bool ZWApi::ZWApi::zwAssociationGet(int node_id, int group) {
	char mybuf[1024];

        mybuf[0] = FUNC_ID_ZW_SEND_DATA;
	mybuf[1] = node_id;
	mybuf[2] = 3;
	mybuf[3] = COMMAND_CLASS_ASSOCIATION;
	mybuf[4] = ASSOCIATION_GET;
	mybuf[5] = group;
	mybuf[6] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;

	if (zwIsSleepingNode(node_id)) {
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Postpone Association Get - device is not always listening");
		sendFunctionSleeping(node_id, mybuf , 7, REQUEST, 1);
	} else {
		sendFunction( mybuf , 7, REQUEST, 1);

	}

}

bool ZWApi::ZWApi::zwAssociationSet(int node_id, int group, int target_node_id) {
	char mybuf[1024];

        mybuf[0] = FUNC_ID_ZW_SEND_DATA;
	mybuf[1] = node_id;
	mybuf[2] = 4;
	mybuf[3] = COMMAND_CLASS_ASSOCIATION;
	mybuf[4] = ASSOCIATION_SET;
	mybuf[5] = group;
	mybuf[6] = target_node_id;
	mybuf[7] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;

	if (zwIsSleepingNode(node_id)) {
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Postpone Association Set - device is not always listening");
		sendFunctionSleeping(node_id, mybuf , 8, REQUEST, 1);
	} else {
		sendFunction( mybuf , 8, REQUEST, 1);

	}

}

bool ZWApi::ZWApi::zwAssignReturnRoute(int src_node_id, int dst_node_id) {
	char mybuf[1024];

        mybuf[0] = FUNC_ID_ZW_ASSIGN_RETURN_ROUTE;
	mybuf[1] = src_node_id;
	mybuf[2] = dst_node_id;
	//mybuf[7] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
//        sendFunction( mybuf , 3, REQUEST, 1);

	// suc test code
        mybuf[0] = 0x51;
	mybuf[1] = src_node_id;
	mybuf[2] = 0x00;
        sendFunction( mybuf , 3, REQUEST, 1);

} 

bool ZWApi::ZWApi::zwReplicateController(int mode) {
	char mybuf[1024];
	
	if (mode) {
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Replicate controller start");
		mybuf[0] = FUNC_ID_ZW_SET_LEARN_MODE;
		mybuf[1] = 0x01;
		sendFunction( mybuf , 2, REQUEST, 0); 

	} else {
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Replicate controller end");
		mybuf[0] = FUNC_ID_ZW_SET_LEARN_MODE;
		mybuf[1] = 0x00;
		sendFunction( mybuf , 2, REQUEST, 0); 

		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Get new init data after replication");
		deviceList = "";
		mybuf[0] = FUNC_ID_SERIAL_API_GET_INIT_DATA;
		sendFunction( mybuf , 1, REQUEST, 0); 
	}

}

bool ZWApi::ZWApi::zwSetDefault() {
	char mybuf[1024];
	
	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Reset controller and erase all node information");
	mybuf[0] = FUNC_ID_ZW_SET_DEFAULT;
	sendFunction( mybuf , 1, REQUEST, 1); 

}

bool ZWApi::ZWApi::zwAddNodeToNetwork(int startstop, bool highpower) {
	char mybuf[1024];

	mybuf[0] = FUNC_ID_ZW_ADD_NODE_TO_NETWORK;
	if (startstop) {	
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Adding new node - start");
		mybuf[1] = highpower ? ADD_NODE_ANY | ADD_NODE_OPTION_HIGH_POWER : ADD_NODE_ANY;
		sendFunction( mybuf , 2, REQUEST, 1); 
	} else {
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Adding new node - end");
		mybuf[1] = ADD_NODE_STOP;
		sendFunction( mybuf , 2, REQUEST, 0); 
	}
}

bool ZWApi::ZWApi::zwRemoveNodeFromNetwork(int startstop) {
	char mybuf[1024];

	mybuf[0] = FUNC_ID_ZW_REMOVE_NODE_FROM_NETWORK;
	if (startstop) {	
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Removing node - start");
		mybuf[1] = REMOVE_NODE_ANY;
		sendFunction( mybuf , 2, REQUEST, 1); 
	} else {
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Removing node - end");
		mybuf[1] = REMOVE_NODE_STOP;
		sendFunction( mybuf , 2, REQUEST, 0); 
	}
}

bool ZWApi::ZWApi::zwConfigurationSet(int node_id,int parameter,int value) {
	char mybuf[1024];
	int len = 0;

        mybuf[0] = FUNC_ID_ZW_SEND_DATA;
	mybuf[1] = node_id;
	mybuf[3] = COMMAND_CLASS_CONFIGURATION;
	mybuf[4] = CONFIGURATION_SET;
	mybuf[5] = parameter; // parameter number
	if (value <= 0xff) {
		// one byte value
		mybuf[6] = 1;
		mybuf[7] = value;
		mybuf[8] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
		mybuf[2] = 5;
		len = 9;
	} else {
		 if (value <= 0xffff) {
			// two byte value
			mybuf[6] = 2;
			mybuf[7] = ( (value >> 8 ) & 0xff);
			mybuf[8] = (value & 0xff);
			mybuf[9] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
			mybuf[2] = 6;
			len = 10;

		} else {
			// four byte value
			mybuf[6] = 4;
			mybuf[7] = ( (value >> 24) & 0xff );
			mybuf[8] = ( (value >> 16) & 0xff );
			mybuf[9] = ( (value >> 8) & 0xff );
			mybuf[10] = (value & 0xff);
			mybuf[11] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
			mybuf[2] = 8;
			len = 12;

		}
	}

	if (zwIsSleepingNode(node_id)) {

		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Postpone Configuration Set - device is not always listening");
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Nodeid: %i Parameter: %i Value: %i",node_id,parameter,value);
		sendFunctionSleeping(node_id, mybuf , len, REQUEST, 1);

	} else {
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Running Configuration Set");
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Nodeid: %i Parameter: %i Value: %i",node_id,parameter,value);
		sendFunction( mybuf , len, REQUEST, 1);

	}

}

bool ZWApi::ZWApi::zwWakeupSet(int node_id,int value, bool multi) {
	char mybuf[1024];
	int len = 0;

	if (multi == false) {
		mybuf[0] = FUNC_ID_ZW_SEND_DATA;
		mybuf[1] = node_id;
		mybuf[2] = 6; // length of command
		mybuf[3] = COMMAND_CLASS_WAKE_UP;
		mybuf[4] = WAKE_UP_INTERVAL_SET;
		// convert to seconds
		value = value * 60;
		mybuf[5] = (value >> 16) & 0xff; // seconds msb
		mybuf[6] = (value >> 8) & 0xff; // 
		mybuf[7] = value & 0xff; // seconds lsb
		mybuf[8] = ournodeid;
		mybuf[9] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
		len = 10;
	} else {
		mybuf[0]=FUNC_ID_ZW_SEND_DATA;
		mybuf[1] = node_id;
		mybuf[2]=10;
		mybuf[3]=COMMAND_CLASS_MULTI_CMD;
		mybuf[4]=MULTI_CMD_ENCAP;
		mybuf[5]=1; // 1 commands
		//mybuf[5]=2; // 2 commands
		mybuf[6]=6;
		mybuf[7]=COMMAND_CLASS_WAKE_UP;
		mybuf[8]=WAKE_UP_INTERVAL_SET;
		value = value * 60;
		mybuf[9] = (value >> 16) & 0xff; // seconds msb
		mybuf[10] = (value >> 8) & 0xff; // 
		mybuf[11] = value & 0xff; // seconds lsb
		mybuf[12]=ournodeid;
		//mybuf[13]=2;
		//mybuf[14]=COMMAND_CLASS_WAKE_UP;
		//mybuf[15]=WAKE_UP_NO_MORE_INFORMATION;
		mybuf[13]=TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
		//mybuf[16]=TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
		len = 14;
		// len = 17;

	}
	if (zwIsSleepingNode(node_id)) {
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Postpone wake up interval set - device is not always listening");
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Nodeid: %i Value: %i",node_id,value);
		sendFunctionSleeping(node_id, mybuf , len, REQUEST, 1);
	} else {
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Running wake up interval set");
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Nodeid: %i Value: %i",node_id,value);
		sendFunction( mybuf , len, REQUEST, 1);
	}
}

bool ZWApi::ZWApi::zwIsSleepingNode(int node_id) {

	bool rvalue = false;
	// verify if device is battery powered
	pthread_mutex_lock (&mutexSendQueue);
	ZWNodeMapIt = ZWNodeMap.find((unsigned int)node_id);
	if (ZWNodeMapIt != ZWNodeMap.end()) {
		if ((*ZWNodeMapIt).second->sleepingDevice == true) {
			rvalue= true;
		}
	}
	pthread_mutex_unlock (&mutexSendQueue);
	return rvalue;
}

bool ZWApi::ZWApi::wakeupHandler(int node_id) {
	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Running wakeupHandler for node %i",node_id);
        std::deque < ZWJob * >::iterator sendQueueIt;

        std::multimap < int, ZWJob * >::iterator wakeupQueueIt;
        std::multimap < int, ZWJob * >::iterator wakeupQueueItLast;

	wakeupQueueIt = ZWWakeupQueue.find(node_id);

	if (wakeupQueueIt == ZWWakeupQueue.end()) {
		// nothing in queue, good bye
		return true;	
	}

	wakeupQueueItLast = ZWWakeupQueue.upper_bound(node_id);
	for ( ; wakeupQueueIt != wakeupQueueItLast; ++wakeupQueueIt) {
		
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Moving job %p from wakup to send queue",(*wakeupQueueIt).second);
		pthread_mutex_lock (&mutexSendQueue);
		ZWSendQueue.push_back((*wakeupQueueIt).second);
		ZWWakeupQueue.erase(wakeupQueueIt);
		pthread_mutex_unlock (&mutexSendQueue);

	}
	return true;
}	


bool ZWApi::ZWApi::zwRequestNodeNeighborUpdate(int node_id) {
	char mybuf[1024];

	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Requesting Neighbour Update for node %i",node_id);
	mybuf[0] = FUNC_ID_ZW_REQUEST_NODE_NEIGHBOR_UPDATE;
	mybuf[1] = node_id;
	sendFunction( mybuf , 2, REQUEST, 1);
}
void ZWApi::ZWApi::zwReadMemory(int offset) {
	char mybuf[1024];

	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Reading eeprom at offset %i",offset);
	mybuf[0] = 0x23;
	mybuf[1] = (offset >> 8) & 0xff;
	mybuf[2] = offset & 0xff;;
	mybuf[3] = 64;
	sendFunction( mybuf , 4, REQUEST, 0);
}


void ZWApi::ZWApi::zwRequestBasicReport(int node_id) {
	char mybuf[1024];

	mybuf[0] = FUNC_ID_ZW_SEND_DATA;
	mybuf[1] = node_id;
	mybuf[2] = 0x02;
	mybuf[3] = COMMAND_CLASS_BASIC;
	mybuf[4] = BASIC_GET;
	mybuf[5] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
	sendFunction (mybuf,6,REQUEST,1);
}
void ZWApi::ZWApi::zwRequestManufacturerSpecificReport(int node_id) {
	char mybuf[1024];

	mybuf[0] = FUNC_ID_ZW_SEND_DATA;
	mybuf[1] = node_id;
	mybuf[2] = 2; // length of command
	mybuf[3] = COMMAND_CLASS_MANUFACTURER_SPECIFIC;
	mybuf[4] = MANUFACTURER_SPECIFIC_GET;
	mybuf[5] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
	sendFunction( mybuf , 6, REQUEST, 0);

}

void ZWApi::ZWApi::zwStatusReport() {
	ZWNodeMapIt = ZWNodeMap.begin();
	while (ZWNodeMapIt!=ZWNodeMap.end()) {
		zwRequestNodeNeighborUpdate((*ZWNodeMapIt).first);
//		zwRequestManufacturerSpecificReport((*ZWNodeMapIt).first);
		ZWNodeMapIt++;
	}
//	for (int i=0;i<512;i++) {
//		zwReadMemory(64*i);
//	}
//	zwRequestBasicReport(NODE_BROADCAST);
}
