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

DCE::ZWave *DCEcallback;

#include <math.h>

extern "C" void *start( void* );

void *start( void *p )
{
	ZWApi::ZWApi *base = static_cast<ZWApi::ZWApi*>(p);
	base->receiveFunction();
	return NULL;
}

ZWApi::ZWApi::ZWApi(void *myZWave) {
	this->myZWave = myZWave;
	DCEcallback = static_cast<DCE::ZWave*>(myZWave);
}

ZWApi::ZWApi::~ZWApi() {
}

void *ZWApi::ZWApi::init(const char *device) {
	char mybuf[1024];

 	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Initialization...");


        callbackpool = 1;
        ournodeid = -1;
        await_ack = 0;
	poll_state = 1;
	memory_dump_offset = -1;
	memory_dump_counter = 0;
	memory_dump_len = -1;
	maxnodeid = 0;

 	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Open serial port %s",device);
	OpenSerialPortEx(device,&serialPort);
        mybuf[0]=NAK;
        WriteSerialStringEx(serialPort,mybuf,1);

	pthread_mutexattr_t mutexAttrRecursive;
	pthread_mutexattr_init(&mutexAttrRecursive);
	pthread_mutexattr_settype(&mutexAttrRecursive, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&mutexSendQueue, &mutexAttrRecursive);

	static pthread_t readThread;
	pthread_create(&readThread, NULL, start, (void*)this);

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

	return NULL;
}

void SendPopup(void *ref, char *message) {

	DCEcallback->SendOrbiterPopup(message);
}

char ZWApi::ZWApi::checksum(char *buf, int len) {
	int ret = 0xff;
	for (int i=0;i<len;i++) ret ^= buf[i];
	return ret;

}

void ZWApi::ZWApi::handleCommandSensorMultilevelReport(int nodeid, int instance_id, int sensortype, int metadata,
							int val1, int val2, int val3, int val4) {
        DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got sensor report from node %i/%i",nodeid, instance_id);
	int scale = ( (unsigned char)metadata & SENSOR_MULTILEVEL_REPORT_SCALE_MASK ) >> SENSOR_MULTILEVEL_REPORT_SCALE_SHIFT;
	int precision = ( (unsigned char)metadata & SENSOR_MULTILEVEL_REPORT_PRECISION_MASK ) >> SENSOR_MULTILEVEL_REPORT_PRECISION_SHIFT;
	int size = (unsigned char)metadata & SENSOR_MULTILEVEL_REPORT_SIZE_MASK;
	int value;
	short tmpval;
	float fValue;

	switch(size) {
	case 1:
	        value = (signed char)val1;
		;;
		break;
	case 2:
	        tmpval = ((unsigned char)val1 << 8) + (unsigned char)val2;
		value = (signed short)tmpval;
		;;
		break;
	default:
	        value = ( (unsigned char)val1 << 24 ) + ( (unsigned char)val2 << 16 ) + ( (unsigned char)val3 << 8 ) + (unsigned char)val4;
		value = (signed int)value;
		;;
		break;
	}
	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"MULTILEVEL DEBUG: precision: %i scale: %i size: %i value: %i",precision,scale,size,value);

	if (precision > 0) {
		fValue = value / pow(10,precision);
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"MULTILEVEL DEBUG: float value: %2f",fValue);
	}  else {
		fValue = value;
	}
	if (precision > 0) { value = value / pow(10 , precision) ; }  // we only take the integer part for now

	switch(sensortype) { // sensor type

	case SENSOR_MULTILEVEL_REPORT_GENERAL_PURPOSE_VALUE:
	        if (scale == 0) {
		        DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"General purpose measurement value received: %d %%",value);
		} else {
		        DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"General purpose measurement value received: %d (dimensionless)",value);
		}
		;;
		break;
	case SENSOR_MULTILEVEL_REPORT_LUMINANCE:
	        if (scale == 0) {
		        DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Luminance measurement received: %d %%",value);
		} else {
		        DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Luminance measurement received: %d Lux",value);
		}
		DCEcallback->SendBrightnessChangedEvent ((unsigned char)nodeid, instance_id, value);
		;;
		break;
	case SENSOR_MULTILEVEL_REPORT_POWER:
	        DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"MULTILEVEL DEBUG: precision: %i scale: %i size: %i value: %i",precision,scale,size,value);
		if (scale == 0) {
		        DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Power level measurement received: %d W",value);
			DCEcallback->SendPowerUsageChangedEvent((unsigned char)nodeid, instance_id, value);
		} else {
		        DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Power level measurement received: %d",value);
		}
		;;
		break;
	case SENSOR_MULTILEVEL_REPORT_CO2_LEVEL:
	        DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"CO2 level measurement received: %d ppm",value);
		DCEcallback->SendCO2LevelChangedEvent ((unsigned char)nodeid, instance_id, value);

		;;
		break;
	case SENSOR_MULTILEVEL_REPORT_RELATIVE_HUMIDITY:
	        DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Relative humidity measurement received: %.2f percent",fValue);
		DCEcallback->SendHumidityChangedEvent ((unsigned char)nodeid, instance_id, fValue);

		;;
		break;
	case SENSOR_MULTILEVEL_REPORT_TEMPERATURE:
	        if (scale == 0) {
		        DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Temperature level measurement received: %.2f C",fValue);
			DCEcallback->SendTemperatureChangedEvent ((unsigned char)nodeid, instance_id, fValue);
		} else {
		        DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Temperature level measurement received: %.2f F",fValue);
			DCEcallback->SendTemperatureChangedEvent ((unsigned char)nodeid, instance_id, (fValue-32) *5 / 9);
		}
		;;
		break;
	default:
	        DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Sensor type 0x%x not handled",(unsigned char)sensortype);
		;;
		break;
	}
}

void *ZWApi::ZWApi::decodeFrame(char *frame, size_t length) {
	ZWNode *newNode = NULL;
	int k; // int helper
	char tempbuf[512];
	char tempbuf2[512];
	if (frame[0] == RESPONSE) {
		switch ((unsigned char)frame[1]) {
			case FUNC_ID_ZW_GET_SUC_NODE_ID:
				DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING,"Got reply to GET_SUC_NODE_ID, node: %d",frame[2]);
				if ((unsigned char)frame[2] == 0) {
					DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING,"No SUC, we become SUC");
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
				} else if ((unsigned char)frame[2] != ournodeid) {
					DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING,"requesting network update from SUC");
					tempbuf[0]=FUNC_ID_ZW_REQUEST_NETWORK_UPDATE;
					sendFunction(tempbuf, 1, REQUEST, 1);

				}
				break;
			;;
			case ZW_GET_ROUTING_INFO:
				DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got reply to ZW_GET_ROUTING_INFO:");
				k = getIntent(2);
				sprintf(tempbuf,"Node %3i: ",k);
				routingtable.append(tempbuf);
				// printf("RINFO: %i",getIntent(2));
				for (int i=1; i<=maxnodeid; i++) {
					bool nodebit;
					nodebit = (unsigned char)frame[2+i/8] & (1<<((i%8)-1));
					// printf("%c", nodebit ? 'Y' : 'N');
					if (nodebit) {
						routingtable.append("Y");
					} else {
						routingtable.append("N");
					}
				}
				// printf("\n");
				routingtable.append("\n");
				if (k==maxnodeid) {

					DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"\n%s\n",routingtable.c_str());
				}
				break;
			;;
			case ZW_MEMORY_GET_ID:
				DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got reply to ZW_MEMORY_GET_ID:");
				DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Home id: 0x%02x%02x%02x%02x, our node id: %d",(unsigned char) frame[2],(unsigned char) frame[3],(unsigned char)frame[4],(unsigned char)frame[5],(unsigned char)frame[6]);
				ournodeid = (unsigned char)frame[6];
				break;
			;;
			case ZW_MEM_GET_BUFFER:
				DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING,"Got reply to ZW_MEM_GET_BUFFER");
				for (int i=2;i<length;i++) {
					// printf("MEM:%x",(unsigned char)frame[i]);
					memory_dump[memory_dump_offset+i-2]=(unsigned char)frame[i];
					memory_dump_counter++;
				}
				if ((memory_dump_offset > -1) && (memory_dump_offset < 16320)) {
					// we are dumping the complete memory
					memory_dump_offset+=60;
					zwReadMemory(memory_dump_offset,60);
				} else if ((memory_dump_offset == 16320)) {
					// last frame
					memory_dump_offset+=60;
					DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING,"Memory dump fetch last 4 bytes");
					zwReadMemory(memory_dump_offset,4);
				} else if (memory_dump_offset > 16320) {
					int fd, writesize;
					DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING,"Memory dump complete");
					for (int i=0;i<16384;i++) {
						if (i%32==0) { printf("\n"); }
						printf("%2x ",memory_dump[i]);
					}
					printf("\n");
					fd = open("/tmp/zwave-controller-backup.dump",O_WRONLY|O_CREAT);
					writesize = write(fd, memory_dump, 16384);
					close(fd);
					DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING,"dumped %i bytes, offset %i. written %i bytes",memory_dump_counter, memory_dump_offset, writesize);
					poll_state=1;

				}
				break;
			;;
			case ZW_MEM_PUT_BUFFER:
				DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING,"Got reply to ZW_MEM_PUT_BUFFER");
				if (memory_dump_offset < 16380) {
					memory_dump_offset+=60;
					zwWriteMemory(memory_dump_offset, memory_dump_len, memory_dump+memory_dump_offset);
				} else if (memory_dump_offset == 16380) {
					memory_dump_len=4;
					zwWriteMemory(memory_dump_offset, memory_dump_len, memory_dump+memory_dump_offset);
					memory_dump_offset+=4;
				} else {
					DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING,"Memory restore complete %i",memory_dump_offset);
					memory_dump_offset=65535;

				}


				break;
			;;
			case FUNC_ID_SERIAL_API_GET_INIT_DATA:
				DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got reply to FUNC_ID_SERIAL_API_GET_INIT_DATA:");
				if (frame[4] == MAGIC_LEN) {
					for (int i=5;i<5+MAGIC_LEN;i++) {
						for (int j=0;j<8;j++) {
							// printf("test node %d\n",(i-5)*8+j+1);
							if ((unsigned char)frame[i] & (0x01 << j)) {
								// DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"found node %d",(i-5)*8+j+1);
								// requesting node protocol information
								// tempbuf[0]=0x60; // req. node info frame - test
								addIntent((i-5)*8+j+1,1);
								tempbuf[0]=FUNC_ID_ZW_GET_NODE_PROTOCOL_INFO;
								tempbuf[1]=(i-5)*8+j+1;
								sendFunction( tempbuf , 2, REQUEST, 0);
							} else {
								sprintf(tempbuf,"%i",(i-5)*8+j+1);
								// DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Node id %i not in node mask, deleting",(i-5)*8+j+1);
								DCEcallback->DeleteDevicesForNode(tempbuf);
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
					if (tmp_nodeid > maxnodeid) { maxnodeid = tmp_nodeid; }
					newNode = new ZWNode(tmp_nodeid);
					newNode->plutoDeviceTemplateConst=0;
					newNode->typeBasic = (unsigned char) frame[5];
					newNode->typeGeneric = (unsigned char) frame[6];
					newNode->typeSpecific = (unsigned char) frame[7];
					newNode->stateBasic = -1;
					newNode->associationList[0]="";
					newNode->associationList[1]="";
					newNode->associationList[2]="";
					newNode->associationList[3]="";

					if (((unsigned char)frame[2]) & (0x01 << 7)) {
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"listening node");
						newNode->sleepingDevice = false;
						// request version from the device
						//zwRequestVersion(tmp_nodeid);
						//zwRequestManufacturerSpecificReport(tmp_nodeid);
					} else {
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"sleeping node");
						newNode->sleepingDevice = true;
					}
					if (((unsigned char)frame[3]) & (0x01 << 7)) {
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"optional functionality");
					}
					if (((unsigned char)frame[3]) & (0x01 << 6)) {
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"1000ms frequent listening slave");
						newNode->sleepingDevice = false;
					}
					if (((unsigned char)frame[3]) & (0x01 << 5)) {
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"250ms frequent listening slave");
						newNode->sleepingDevice = false;
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
					switch ((unsigned char)frame[6]) {
						case GENERIC_TYPE_GENERIC_CONTROLLER:
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"GENERIC TYPE: Generic Controller");
							break;
						;;
						case GENERIC_TYPE_STATIC_CONTROLLER:
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"GENERIC TYPE: Static Controller");
							break;
						;;
						case GENERIC_TYPE_THERMOSTAT:
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"GENERIC TYPE: Thermostat");
							break;
						;;
						case GENERIC_TYPE_SWITCH_MULTILEVEL:
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"GENERIC TYPE: Multilevel Switch");
							break;
						;;
						case GENERIC_TYPE_SWITCH_REMOTE:
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"GENERIC TYPE: Remote Switch");
							// TODO: saner approach
							//  we read the associations from groups one and two (left and right paddle on the ACT remote switch)
							//zwAssociationGet(tmp_nodeid,1);
							//zwAssociationGet(tmp_nodeid,2);
							break;
						;;
						case GENERIC_TYPE_SWITCH_BINARY:
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"GENERIC TYPE: Binary Switch");
							break;
						;;
						case GENERIC_TYPE_SENSOR_BINARY:
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"GENERIC TYPE: Sensor Binary");
							break;
						case GENERIC_TYPE_WINDOW_COVERING:
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"GENERIC TYPE: Window Covering");
							break;
						;;
						case GENERIC_TYPE_SENSOR_MULTILEVEL:
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"GENERIC TYPE: Sensor Multilevel");
							break;
						;;
						case GENERIC_TYPE_SENSOR_ALARM:
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"GENERIC TYPE: Sensor Alarm");
							break;
						;;
						default:
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"GENERIC TYPE: 0x%x",frame[6]);
							break;
						;;

					}
					DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"SPECIFIC TYPE: 0x%x",frame[7]);
					newNode->plutoDeviceTemplateConst = getDeviceTemplate((unsigned char)frame[5],(unsigned char)frame[6],(unsigned char)frame[7],NULL,0);

					ZWNodeMap.insert(std::map < int, ZWNode * >::value_type(tmp_nodeid,newNode));
					if (newNode->plutoDeviceTemplateConst != 0) {
						sprintf(tempbuf2, "%d", tmp_nodeid);
						// DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Trying to create device: %i %i",tmp_nodeid, newNode->plutoDeviceTemplateConst);
						newNode->iPKDevice = DCEcallback->AddDevice(0, tempbuf2, 0, newNode->plutoDeviceTemplateConst);
					}

					// check if we have fetched the node's command classes/capabilities
					string capa = DCEcallback->GetCapabilities(tmp_nodeid, 0);
					if (!capa.empty()) {
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Device capabilities: %s",capa.c_str());
						resetNodeInstanceCount(newNode, capa);
						// Is this a MULTI_INSTANCE node?
						if (capa.find(StringUtils::itos(COMMAND_CLASS_MULTI_INSTANCE)) != string::npos) {
							// load capabilities for instances and store in node
							map<int, int> mapCCInstanceCount;
							mapCCInstanceCount = DCEcallback->FindCCInstanceCountForNode(tempbuf2);
							map<int, int>::iterator ccIterator;
							for ( ccIterator=newNode->mapCCInstanceCount.begin() ; ccIterator != newNode->mapCCInstanceCount.end(); ccIterator++ ) {
								DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Command class: %d (hex: %x)", (*ccIterator).first, (*ccIterator).first);
								if (mapCCInstanceCount.find((*ccIterator).first) != mapCCInstanceCount.end()) {
									DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Device data matches: instance count: %d", mapCCInstanceCount[(*ccIterator).first]);
									newNode->mapCCInstanceCount[(*ccIterator).first] = mapCCInstanceCount[(*ccIterator).first];
								}
							}
						}
					} else {
						// No capabilities on record, request node info
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"No device capabilities for node: %d",newNode->id);
						zwRequestNodeInfo(newNode->id);
					}
				} else {
					sprintf(tempbuf2, "%d", tmp_nodeid);
					DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Invalid generic class (0x%x), ignoring device",(unsigned char)frame[6]);
					DCEcallback->DeleteDevicesForNode(tempbuf2);
				}
				if (getIntentSize() == 0) {
					// we got all protcol info responses
					DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING,"Finished building node list:");
					ZWNodeMapIt = ZWNodeMap.begin();
					while (ZWNodeMapIt!=ZWNodeMap.end()) {
					        DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING,"Node: %i basic: 0x%x generic: 0x%x specific: 0x%x pluto: %i",(*ZWNodeMapIt).first,(*ZWNodeMapIt).second->typeBasic,(*ZWNodeMapIt).second->typeGeneric,(*ZWNodeMapIt).second->typeSpecific,(*ZWNodeMapIt).second->plutoDeviceTemplateConst);
						zwGetRoutingInfo((*ZWNodeMapIt).first);
						addIntent((*ZWNodeMapIt).first,2);
						ZWNodeMapIt++;

					}

					// DCEcallback->CMD_Report_Child_Devices();

				}
				break;
			;;
			case FUNC_ID_ZW_REQUEST_NODE_INFO:
				DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got reply to FUNC_ID_ZW_REQUEST_NODE_INFO:");
				break;
			;;
			case FUNC_ID_ZW_SEND_DATA:
				switch(frame[2]) {
					case 1:
						DCE::LoggerWrapper::GetInstance()->Write(LV_DEBUG,"ZW_SEND delivered to Z-Wave stack");
						break;
					case 0:
						DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING,"ERROR: ZW_SEND could not be delivered to Z-Wave stack");
						break;
					default:
						DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING,"ERROR: ZW_SEND Response is invalid!");
				}

				break;
			case FUNC_ID_SERIAL_API_GET_CAPABILITIES:
				DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got reply to FUNC_ID_SERIAL_API_GET_CAPABILITIES:");
				DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"SerAppV:%i,r%i,Manf %i,Typ %i,Prod %i",(unsigned char)frame[2],(unsigned char)frame[3], ((unsigned char)frame[4]<<8) + (unsigned char)frame[5],((unsigned char)frame[6]<<8) + (unsigned char)frame[7],((unsigned char)frame[8]<<8) + (unsigned char)frame[9]);
				break;
			case ZW_GET_VERSION:
				DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got reply to ZW_VERSION:");
				DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ZWave Version: %c.%c%c",(unsigned char)frame[9],(unsigned char)frame[11],(unsigned char)frame[12]);
				break;
			default:
				DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"TODO: handle response for 0x%x ",(unsigned char)frame[1]);
				break;
			;;
		}

	} else if (frame[0] == REQUEST) {

		switch (frame[1]) {
			case FUNC_ID_ZW_SEND_DATA:
				DCE::LoggerWrapper::GetInstance()->Write(LV_DEBUG,"ZW_SEND Response with callback %i received",(unsigned char)frame[2]);
				if ((unsigned char)frame[2] != await_callback) {
					// wrong callback id
					DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING,"ERROR: callback id is invalid!");

				} else {
					// callback id matches our expectation
					switch(frame[3]) {
						case 1:
							// zwsend failed
							pthread_mutex_lock (&mutexSendQueue);
							if (ZWSendQueue.front()->sendcount > 3) {
								// can't deliver frame, abort
								DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING,"Error: ZW_SEND failed, removing job after three tries");
								dropSendQueueJob();
							} else {
								DCE::LoggerWrapper::GetInstance()->Write(LV_DEBUG,"Error: ZW_SEND failed, retrying");
								// trigger resend
								await_ack = 0;
							}
							await_callback = 0;
							pthread_mutex_unlock (&mutexSendQueue);
							break;
						case 0:
							// command reception acknowledged by node
							DCE::LoggerWrapper::GetInstance()->Write(LV_DEBUG,"ZW_SEND was successful, removing job");
							pthread_mutex_lock (&mutexSendQueue);
							ZWSendQueue.pop_front();
							pthread_mutex_unlock (&mutexSendQueue);
							await_callback = 0;
							dropped_jobs = 0;

							break;
						default:
							DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING,"ERROR: ZW_SEND Response is invalid!");

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
				switch ((unsigned char)frame[5]) {
					case COMMAND_CLASS_SECURITY:
						if ((unsigned char)frame[6] == SECURITY_SCHEME_REPORT) {
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got SECURITY_SCHEME_REPORT from node %i, scheme: %x - requesting nonce",(unsigned char)frame[3],(unsigned char)frame[7]);
							zwSecurityNonceGet((unsigned char)frame[3]);

						} else if ((unsigned char)frame[6] == SECURITY_NONCE_REPORT) {
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got SECURITY_NONCE_REPORT from node %i, nonce: %x %x %x %x %x %x %x %x ",(unsigned char)frame[3],(unsigned char)frame[7],(unsigned char)frame[8],(unsigned char)frame[9],(unsigned char)frame[10],(unsigned char)frame[11],(unsigned char)frame[12],(unsigned char)frame[13],(unsigned char)frame[14]);
							ZWNodeMapIt = ZWNodeMap.find((unsigned int)frame[3]);
							if (ZWNodeMapIt != ZWNodeMap.end()) {
								(*ZWNodeMapIt).second->nonce[0] = (unsigned char)frame[7];
								(*ZWNodeMapIt).second->nonce[1] = (unsigned char)frame[8];
								(*ZWNodeMapIt).second->nonce[2] = (unsigned char)frame[9];
								(*ZWNodeMapIt).second->nonce[3] = (unsigned char)frame[10];
								(*ZWNodeMapIt).second->nonce[4] = (unsigned char)frame[11];
								(*ZWNodeMapIt).second->nonce[5] = (unsigned char)frame[12];
								(*ZWNodeMapIt).second->nonce[6] = (unsigned char)frame[13];
								(*ZWNodeMapIt).second->nonce[7] = (unsigned char)frame[14];
							}
							zwSecurityKeySet((unsigned char)frame[3]);
						}
						break;
					;;
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
					case COMMAND_CLASS_MULTI_INSTANCE:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_MULTI_INSTANCE");
						if (frame[6] == MULTI_INSTANCE_REPORT) {
						        int instanceCount = (unsigned char)frame[8];
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got MULTI_INSTANCE_REPORT from node %i: Command Class 0x%x, instance count: %i",(unsigned char)frame[3],(unsigned char)frame[7], instanceCount);
							// instance count == 1 -> assume instance 1 is "main" device and don't add new device
							if (instanceCount > 1) {
							        ZWNodeMapIt = ZWNodeMap.find((unsigned int)frame[3]);
								if (ZWNodeMapIt != ZWNodeMap.end()) {
								        (*ZWNodeMapIt).second->mapCCInstanceCount[(unsigned int)frame[7]] = instanceCount;
								}
								// add new devices for instances
								char tmpstr[30];
								sprintf(tmpstr, "%d", (unsigned char)frame[3]);
								for (int i = 2; i <= instanceCount; i++) {
								        int PKDevice = DCEcallback->AddDevice(0, tmpstr, i, (*ZWNodeMapIt).second->plutoDeviceTemplateConst);
									DCEcallback->AddCapability(PKDevice, (unsigned char)frame[7]);
									newNode->iPKDevice = PKDevice;
								}
							}
						} else if (frame[6] == MULTI_INSTANCE_CMD_ENCAP) {
						        DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got MULTI_INSTANCE_CMD_ENCAP from node %i: instance %i Command Class 0x%x type 0x%x",(unsigned char)frame[3],(unsigned char)frame[7],(unsigned char)frame[8],(unsigned char)frame[9]);
						        if (frame[8] == COMMAND_CLASS_SENSOR_MULTILEVEL) {
							        handleCommandSensorMultilevelReport(frame[3], //nodeid
												    (unsigned int)frame[7], // instance id
												    (unsigned int)frame[10], // sensor type
												    (unsigned int)frame[11], // value metadata
												    (unsigned int)frame[12], // value
												    (unsigned int)frame[13],
												    (unsigned int)frame[14],
												    (unsigned int)frame[15]);
						        } else if ((frame[8] == COMMAND_CLASS_BASIC) && (frame[9] == BASIC_REPORT)) {
								// 41	07/22/10 12:05:17.485		0x1 0xc 0x0 0x4 0x0 0x7 0x6 0x60 0x6 0x1 0x20 0x3 0x0 0xb2 (#######`## ###) <0xb795fb90>
								// 36	07/22/10 12:05:17.485		FUNC_ID_APPLICATION_COMMAND_HANDLER: <0xb795fb90>
								// 36	07/22/10 12:05:17.485		COMMAND_CLASS_MULTI_INSTANCE <0xb795fb90>
								// 36	07/22/10 12:05:17.485		Got MULTI_INSTANCE_CMD_ENCAP from node 7: instance 1 Command Class 0x20 type 0x3 <0xb795fb90>

								DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got basic report from node %i, instance %i, value: %i",(unsigned char)frame[3],(unsigned char)frame[7], (unsigned char) frame[10]);
								DCEcallback->SendLightChangedEvents ((unsigned char)frame[3], (unsigned char)frame[7], (unsigned char) frame[10]);

						        } else if ((frame[8] == COMMAND_CLASS_BASIC) && (frame[9] == BASIC_SET)) {
								DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got basic set from node %i, instance %i, value: %i",(unsigned char)frame[3],(unsigned char)frame[7], (unsigned char) frame[10]);
								DCEcallback->SendOnOffEvent ((unsigned char)frame[3],(unsigned char)frame[7],(unsigned char) frame[10]);
							}
						}
						break;
					;;
					case COMMAND_CLASS_VERSION:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_VERSION");
						if (frame[6] == VERSION_REPORT) {
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"REPORT: Lib.typ: 0x%x, Prot.Ver: 0x%x, Sub: 0x%x App.Ver: 0x%x, Sub: 0x%x",
								(unsigned char)frame[7], (unsigned char)frame[8], (unsigned char)frame[9], (unsigned char)frame[10], (unsigned char)frame[11]);
						}

						break;
					;;
					case COMMAND_CLASS_METER:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_METER");
						if (frame[6] == METER_REPORT) {
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got meter report from node %i",(unsigned char)frame[3]);
							int scale = ( (unsigned char)frame[8] & METER_REPORT_SCALE_MASK ) >> METER_REPORT_SCALE_SHIFT;
							int precision = ( (unsigned char)frame[8] & METER_REPORT_PRECISION_MASK ) >> METER_REPORT_PRECISION_SHIFT;
							int size = (unsigned char)frame[8] & METER_REPORT_SIZE_MASK;
							int value;
							short tmpval;
							switch(size) {
								case 1:
									value = (signed char)frame[9];
									;;
								break;
								case 2:
									tmpval = ((unsigned char)frame[9] << 8) + (unsigned char)frame[10];
									value = (signed short)tmpval;
									;;
								break;
								default:
									value = ( (unsigned char)frame[9] << 24 ) + ( (unsigned char)frame[10] << 16 ) + ( (unsigned char)frame[11] << 8 ) + (unsigned char)frame[12];
									value = (signed int)value;
									;;
								break;
							}
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"METER DEBUG: precision: %i scale: %i size: %i value: %i",precision,scale,size,value);
							if (precision > 0) { value = value / pow(10 , precision) ; }  // we only take the integer part for now
							switch(((unsigned char)frame[7]) & 0x1f) { // meter type
								case METER_REPORT_ELECTRIC_METER:
									switch (scale) {
										case 0:
											DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Electric meter measurement received: %d kWh",value);
											break;
										case 1:
											DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Electric meter measurement received: %d kVAh",value);
											break;
										case 2:
											DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Electric meter measurement received: %d W",value);
											DCEcallback->SendPowerUsageChangedEvent((unsigned char)frame[3], -1, value);
											break;
										case 3:
											DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Electric meter measurement received: %d pulses",value);
											break;
									}
									break;
								case METER_REPORT_GAS_METER:
									switch (scale) {
										case 0:
											DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Gas meter measurement received: %d m2",value);
											break;
										case 1:
											DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Gas meter measurement received: %d cubic feet",value);
											break;
										case 3:
											DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Gas meter measurement received: %d pulses",value);
											break;
									}
									break;
								case METER_REPORT_WATER_METER:
									switch (scale) {
										case 0:
											DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Water meter measurement received: %d m2",value);
											break;
										case 1:
											DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Water meter measurement received: %d cubic feet",value);
											break;
										case 2:
											DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Water meter measurement received: %d US gallons",value);
											break;
										case 3:
											DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Water meter measurement received: %d pulses",value);
											break;
									}
									break;
								default:
									DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"unknown METER_REPORT received: %i",(unsigned char)frame[7]);
									break;
							}
						}
						break;
					;;
					case COMMAND_CLASS_MANUFACTURER_SPECIFIC:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_MANUFACTURER_SPECIFIC");
						if (frame[6] == MANUFACTURER_SPECIFIC_REPORT) {
							int manuf=0;int prod=0; int type=0;

							manuf = ((unsigned char)frame[7]<<8) + (unsigned char)frame[8];
							type = ((unsigned char)frame[9]<<8) + (unsigned char)frame[10];
							prod = ((unsigned char)frame[11]<<8) + (unsigned char)frame[12];

							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"REPORT: Manuf: 0x%x, Prod Typ: 0x%x, Prod 0x%x", manuf,type,prod);
							parseManufacturerSpecific((unsigned char)frame[3],manuf,type,prod);

							/* switch ( manuf ) {
								case 0x64: printf("Popp\n");
								break;;
								default: printf("none\n");
							}	*/

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
								if ((*ZWNodeMapIt).second->typeGeneric == GENERIC_TYPE_SENSOR_MULTILEVEL) {
								        // use multi instance get for value > 0, else use normal sensor report
								        if ((*ZWNodeMapIt).second->mapCCInstanceCount.find(COMMAND_CLASS_SENSOR_MULTILEVEL) != (*ZWNodeMapIt).second->mapCCInstanceCount.end() &&
									    (*ZWNodeMapIt).second->mapCCInstanceCount[COMMAND_CLASS_SENSOR_MULTILEVEL] > 0) {
									  DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Requesting SENSOR_REPORT for instances 1 to %d, node %d",(*ZWNodeMapIt).second->mapCCInstanceCount[COMMAND_CLASS_SENSOR_MULTILEVEL],frame[3]);
									        for (int i = 1; i <= (*ZWNodeMapIt).second->mapCCInstanceCount[COMMAND_CLASS_SENSOR_MULTILEVEL]; i++) {
										        zwRequestMultilevelSensorReportInstance((unsigned int)frame[3], i);
										}
									} else {
									        zwRequestMultilevelSensorReport((unsigned int)frame[3]);
									}
								}
							}

							// read battery level
							zwGetBatteryLevel((unsigned char) frame[3]);

							// inject commands from the sleeping queue for this nodeid
							wakeupHandler((unsigned char) frame[3]);

							// handle broadcasts from unconfigured devices
							if (frame[2] & RECEIVE_STATUS_TYPE_BROAD ) {
								DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got broadcast wakeup from node %i, doing WAKE_UP_INTERVAL_SET",frame[3]);
								if (ournodeid != -1) {
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
							if ((unsigned char)frame[7] == 0xff) {
							        DCEcallback->SendSensorTrippedEvents (frame[3], -1, true);
							} else {
							        DCEcallback->SendSensorTrippedEvents (frame[3], -1, false);
							}

						}
						break;
					;;
					case COMMAND_CLASS_SENSOR_MULTILEVEL:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_SENSOR_MULTILEVEL - ");
						if (frame[6] == SENSOR_MULTILEVEL_REPORT) {
						        handleCommandSensorMultilevelReport((unsigned int)frame[3], // node id
											    -1, // instance id
											    (unsigned int)frame[7], // sensor type
											    (unsigned int)frame[8], // value metadata
											    (unsigned int)frame[9], // value
											    (unsigned int)frame[10],
											    (unsigned int)frame[11],
											    (unsigned int)frame[12]);
						}
						break;
					;;
					case COMMAND_CLASS_SENSOR_ALARM:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_SENSOR_ALARM - ");
						if ((unsigned char)frame[6] == SENSOR_ALARM_REPORT) {
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got sensor report from node %i, source node ID: %i, sensor type: %i, sensor state: %i",(unsigned char)frame[3],(unsigned char)frame[7],(unsigned char)frame[8],(unsigned char)frame[9]);
							if ((unsigned char)frame[9]!=0) { // ALARM!!!
								/* if ((unsigned char)frame[0]==0xff) {

								} else { // percentage of severity

								} */
								switch ((unsigned char)frame[8]) {
									case 0x0: // general purpose
										break;
									case 0x1: // smoke alarm
										DCEcallback->SendFireAlarmEvent (frame[3], -1);
										break;
									case 0x2: // co alarm
										DCEcallback->SendAirQualityEvent (frame[3], -1);
										break;
									case 0x3: // co2 alarm
										DCEcallback->SendAirQualityEvent (frame[3], -1);
										break;
									case 0x4: // heat alarm
										DCEcallback->SendFireAlarmEvent (frame[3], -1);
										break;
									case 0x5: // water leak alarm
										break;
								}
							}

						}
					case COMMAND_CLASS_SWITCH_MULTILEVEL:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_SWITCH_MULTILEVEL - ");
						if (frame[6] == SWITCH_MULTILEVEL_REPORT) {
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got switch multilevel report from node %i, level: %i",(unsigned char)frame[3],(unsigned char)frame[7]);
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Send light changed event");
							DCEcallback->SendLightChangedEvents ((unsigned char)frame[3], -1, (unsigned char)frame[7]);
						} else if ((unsigned char)frame[6] == SWITCH_MULTILEVEL_SET) {
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got switch multilevel set from node %i, level: %i",(unsigned char)frame[3],(unsigned char)frame[7]);
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Send light changed event");
							DCEcallback->SendLightChangedEvents ((unsigned char)frame[3], -1, (unsigned char)frame[7]);

						}
						break;
					case COMMAND_CLASS_SWITCH_ALL:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_SWITCH_ALL - ");
						if (frame[6] == SWITCH_ALL_ON) {
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got switch all ON from node %i",(unsigned char)frame[3]);
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Send light changed event");
							DCEcallback->SendLightChangedEvents (0, -1, 99);
						}
						if (frame[6] == SWITCH_ALL_OFF) {
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got switch all OFF from node %i",(unsigned char)frame[3]);
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Send light changed event");
							DCEcallback->SendLightChangedEvents (0, -1, 0);
						}
						break;
					case COMMAND_CLASS_ALARM:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_ALARM - ");
						if (frame[6] == ALARM_REPORT) {
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got ALARM from node %i, type: %i, level: %i",(unsigned char)frame[3],(unsigned char)frame[7],(unsigned char)frame[8]);
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Send sensor tripped changed event");
							DCEcallback->SendSensorTrippedEvents ((unsigned char)frame[3], -1, true);
							DCEcallback->SendSensorTrippedEvents ((unsigned char)frame[3], -1, false);
						}
						break;
					case COMMAND_CLASS_BASIC:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_BASIC - ");
						if (frame[6] == BASIC_REPORT) {

							// char tmp[32];
							// sprintf(&tmp,"%d",frame[7]);
							//
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got basic report from node %i, value: %i",(unsigned char)frame[3],(unsigned char)frame[7]);
							ZWNodeMapIt = ZWNodeMap.find((unsigned int)frame[3]);
							if (ZWNodeMapIt != ZWNodeMap.end()) {
								if ((*ZWNodeMapIt).second->stateBasic != (unsigned char)frame[7]) {
									(*ZWNodeMapIt).second->stateBasic = (unsigned char)frame[7];
									DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"State changed, send light changed event");
									DCEcallback->SendLightChangedEvents ((unsigned char)frame[3], -1, (unsigned char)frame[7]);

								}
							}

						} else if ((unsigned char)frame[6] == BASIC_SET) {
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got BASIC_SET from node %i, value %i",(unsigned char)frame[3],(unsigned char)frame[7]);
							ZWNodeMapIt = ZWNodeMap.find((unsigned int)frame[3]);
							if (ZWNodeMapIt != ZWNodeMap.end()) {

								// send OnOff event for remote switches and controllers to support scenarios
								if  ( ( (*ZWNodeMapIt).second->typeGeneric == GENERIC_TYPE_SWITCH_REMOTE) ||
									( (*ZWNodeMapIt).second->typeBasic == BASIC_TYPE_CONTROLLER) ) {
									if ((unsigned char)frame[7] == 0xff) {
									        DCEcallback->SendOnOffEvent ((unsigned char)frame[3],-1, 1);
									} else {
									        DCEcallback->SendOnOffEvent ((unsigned char)frame[3],-1,0);
									}
								}

								if (((*ZWNodeMapIt).second->typeGeneric == GENERIC_TYPE_SWITCH_REMOTE) && ((*ZWNodeMapIt).second->typeBasic == BASIC_TYPE_ROUTING_SLAVE)) {
									DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"This is a powered remote switch, we will traverse the association list and request reports");
									const char *tmp_str = (*ZWNodeMapIt).second->associationList[1].c_str();
									char *pch = strtok((char*)tmp_str,",");
									while (pch != NULL) {
										zwRequestBasicReport(atoi(pch),0);
										pch = strtok (NULL, ",");
									}


								} else if (((*ZWNodeMapIt).second->typeGeneric == GENERIC_TYPE_SENSOR_BINARY)||((*ZWNodeMapIt).second->typeGeneric == GENERIC_TYPE_SENSOR_MULTILEVEL)) {
									DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"This is a binary sensor, so we send sensor tripped events");
									// if ((unsigned char)frame[7] == 0xff) {
									if ((unsigned char)frame[7] > 0) {
									        DCEcallback->SendSensorTrippedEvents ((unsigned char)frame[3],-1,true);
									} else {
									        DCEcallback->SendSensorTrippedEvents ((unsigned char)frame[3],-1,false);
									}

								} else if (((*ZWNodeMapIt).second->typeGeneric == GENERIC_TYPE_SWITCH_BINARY) || ((*ZWNodeMapIt).second->typeGeneric == GENERIC_TYPE_SWITCH_MULTILEVEL)) {
									(*ZWNodeMapIt).second->stateBasic = (unsigned char)frame[7];
									DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"State changed, send light changed event");
									DCEcallback->SendLightChangedEvents ((unsigned char)frame[3], -1, (unsigned char)frame[7]);

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
							// format; version,2;1,15:00,1;1,22:00,0;2,15:00,1;2,22:00,0;3,11:00,1;3,22:00,0;4,11:00,1;4,22:00,0;5,11:00,1;5,23:00,0;6,07:00,1;6,23:00,0;7,07:00,1;7,22:00,0
							string sSchedule = DCEcallback->GetSchedule((unsigned char) frame[3]);
							int iScheduleVersion = 0;
							int i=0;
							tempbuf[i++]=FUNC_ID_ZW_SEND_DATA;
							// node id
							tempbuf[i++]=frame[3];
							tempbuf[i++]=5;
							tempbuf[i++]=COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE;
							tempbuf[i++]=SCHEDULE_SET;
							tempbuf[i++]=frame[7]; // weekday, 1==mon, 7==sun
							if (sSchedule.find(";") != string::npos) {
								vector<string> vectSchedule;
								vector<string>::const_iterator vectScheduleIt;
								StringUtils::Tokenize(sSchedule, ";", vectSchedule);
								for (vectScheduleIt = vectSchedule.begin() ; vectScheduleIt != vectSchedule.end() ; vectScheduleIt++ ) {
									string sSchedElem = *vectScheduleIt;
									if (sSchedElem.find(",") !=string::npos) {
										vector<string> vectScheduleElem;
										StringUtils::Tokenize(sSchedElem, ",", vectScheduleElem);
										if ((unsigned char)frame[7] == atoi(vectScheduleElem[0].c_str())) {

											DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Schedule for day: %s",sSchedElem.c_str());
											vector<string> vectHourMin;
											if (vectScheduleElem[1].find(":") !=string::npos) {
												StringUtils::Tokenize(vectScheduleElem[1], ":", vectHourMin);
												tempbuf[i++] = atoi(vectHourMin[0].c_str());
												tempbuf[i++] = atoi(vectHourMin[1].c_str());
											} else {
												tempbuf[i++] = 0;
												tempbuf[i++] = 0;
											}
											if (atoi(vectScheduleElem[2].c_str()) == 1)  {
												tempbuf[i++] = 0;
											} else {
												tempbuf[i++] = 0x7a;

											}

										}
									}
								}
							}
							tempbuf[i++]=0;
							tempbuf[i++]=0;
							tempbuf[i++]=0x7f; // unused state, end setpoints
							tempbuf[i++]=TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
							sendFunction( tempbuf , i, REQUEST, 1);

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
								for (unsigned int i=0;i<(length-10);i++) {
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
					case COMMAND_CLASS_BATTERY:
						if ((unsigned char)frame[6] == BATTERY_REPORT) {
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_BATTERY:BATTERY_REPORT: Battery level: %d",(unsigned char)frame[7]);

							if ((unsigned char)frame[7] == 0xff) {
								DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING,"Battery low warning from node %d",(unsigned char)frame[3]);
							}
							DCEcallback->ReportBatteryStatus((unsigned char) frame[3], (unsigned char) frame[7]);
						}
						break;
						;;
					case COMMAND_CLASS_THERMOSTAT_SETPOINT:
						if ((unsigned char)frame[6] == THERMOSTAT_SETPOINT_GET) {
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_THERMOSTAT_SETPOINT:THERMOSTAT_SETPOINT_GET received from node %d",(unsigned char)frame[3]);
						}
						if ((unsigned char)frame[6] == THERMOSTAT_SETPOINT_REPORT) {
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_THERMOSTAT_SETPOINT:THERMOSTAT_SETPOINT_REPORT received from node %d",(unsigned char)frame[3]);
							// 22 = 001 00 010A
							string scale;
							float ftemp;
							int temp;
							int precision = ( (unsigned char)frame[8] & THERMOSTAT_SETPOINT_REPORT_PRECISION_MASK ) >> THERMOSTAT_SETPOINT_REPORT_PRECISION_SHIFT;
							if ((((unsigned char)frame[8]) & THERMOSTAT_SETPOINT_REPORT_SIZE_MASK) == 2) {
								temp = ((unsigned char)frame[9] << 8) + (unsigned char)frame[10];
							} else {
								temp = (unsigned char)frame[9];
							}
							if (precision > 0) {
								ftemp = temp / pow(10,precision);
							}  else {
								ftemp = temp;
							}
							if (((((unsigned char)frame[8]) & THERMOSTAT_SETPOINT_REPORT_SCALE_MASK) >> THERMOSTAT_SETPOINT_REPORT_SCALE_SHIFT) == 0) {
								// celcius
								scale = "degC";
							} else {
								// fahrenheit
								scale = "F";
							}
							switch (((unsigned char)frame[7]) & THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_MASK) {
								case 1:
									DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"heating, %.1f%s",ftemp,scale.c_str());
									break;
								case 2:
									DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"cooling, %.1f%s",ftemp,scale.c_str());
									break;
								case 10:
									DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"auto changeover, %.1f%c",ftemp,scale.c_str());
									break;
							}

							// Send setpoint changed back to router.
							if (scale == "F")
							  {
							    DCEcallback->SendSetpointChangedEvent(frame[3], -1, (ftemp - 32) *5 / 9);
							  }
							else if (scale == "degC")
							  {
							    DCEcallback->SendSetpointChangedEvent(frame[3], -1, ftemp);
							  }


						}
					case COMMAND_CLASS_SWITCH_BINARY:
						if ((unsigned char)frame[6] == SWITCH_BINARY_SET) {
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_SWITCH_BINARY:SWITCH_BINARY_SET received from node %d value %d",(unsigned char)frame[3],(unsigned char)frame[7]);
						}

					case COMMAND_CLASS_CLOCK:
						if ((unsigned char)frame[6] == CLOCK_GET) {
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_CLOCK:CLOCK_GET received from node %d",(unsigned char)frame[3]);
							zwSetClock((unsigned char)frame[3]);
						} else if ((unsigned char)frame[6] == CLOCK_REPORT) {
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_CLOCK:CLOCK_REPORT received from node %d",(unsigned char)frame[3]);
						} else if ((unsigned char)frame[6] == CLOCK_SET) {
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_CLOCK:CLOCK_SET received from node %d",(unsigned char)frame[3]);
						}
						break;
						;;
					case COMMAND_CLASS_THERMOSTAT_MODE:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_THERMOSTAT_MODE - ");
						if (((unsigned char)frame[6] == THERMOSTAT_MODE_REPORT)||((unsigned char)frame[6] == THERMOSTAT_MODE_SET)) {
							switch((unsigned char)frame[7]) {
								case 0:
									DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Thermostat node %d Mode Off",(unsigned char)frame[3]);
									break;
								case 1:
									DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Thermostat node %d Mode Heat",(unsigned char)frame[3]);
									break;
								case 2:
									DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Thermostat node %d Mode Cool",(unsigned char)frame[3]);
									break;
								case 3:
									DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Thermostat node %d Mode Auto",(unsigned char)frame[3]);
									break;
								case 4:
									DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Thermostat node %d Mode Aux/Emergency Heat",(unsigned char)frame[3]);
									break;
								case 5:
									DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Thermostat node %d Mode Resume",(unsigned char)frame[3]);
									break;
								case 6:
									DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Thermostat node %d Mode Fan Only",(unsigned char)frame[3]);
									break;
								case 7:
									DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Thermostat node %d Mode Furnace",(unsigned char)frame[3]);
									break;
								case 8:
									DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Thermostat node %d Mode Dry Air",(unsigned char)frame[3]);
									break;
								case 9:
									DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Thermostat node %d Mode Moist Air",(unsigned char)frame[3]);
									break;
								case 10:
									DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Thermostat node %d Mode Auto Changeover",(unsigned char)frame[3]);
									break;

							}
						}
						break;
						;;
					case COMMAND_CLASS_MULTI_CMD:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_MULTI_CMD - ");
						if ((unsigned char)frame[6] == MULTI_CMD_ENCAP) {
							time_t timestamp;
							struct tm *timestruct;
							int offset = 0;
							// int cmd_length = 0;
							// int cmd_count = 0;

							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Got encapsulated multi command from node %i",(unsigned char)frame[3]);
							timestamp = time(NULL);
							timestruct = localtime(&timestamp);
							// printf("Time: %i %i %i\n",timestruct->tm_wday, timestruct->tm_hour, timestruct->tm_min);
							// iterate over commands
							offset = 8;
							for (int i=0; i<frame[7]; i++) {
								DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND LENGTH: %d, CLASS: %x",frame[offset],(unsigned char) frame[offset+1]);
								switch ((unsigned char)frame[offset+1]) {
									case COMMAND_CLASS_BATTERY:
										if (BATTERY_REPORT == (unsigned char) frame[offset+2]) {
											DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"COMMAND_CLASS_BATTERY:BATTERY_REPORT: Battery level: %d",(unsigned char)frame[offset+3]);
											if ((unsigned char)frame[offset+3] == 0xff) {
												DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING,"Battery low warning from node %d",(unsigned char)frame[3]);

											}
											DCEcallback->ReportBatteryStatus((unsigned char) frame[3], (unsigned char) frame[offset+3]);
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
							string sSchedule = DCEcallback->GetSchedule((unsigned char) frame[3]);
							int iScheduleVersion = 0;
							if (sSchedule.find(";") != string::npos) {
								vector<string> vectSchedule;
								StringUtils::Tokenize(sSchedule, ";", vectSchedule);
								if (vectSchedule[0].find(",") !=string::npos) {
									vector<string> vectScheduleVersion;
									StringUtils::Tokenize(vectSchedule[0], ",", vectScheduleVersion);
									iScheduleVersion = atoi(vectScheduleVersion[1].c_str());
									DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Schedule Version: %i",iScheduleVersion);
								}
							}


							tempbuf[14]=iScheduleVersion; // dummy schedule version for now
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
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Function not implemented - unhandled command class: %x",(unsigned char)frame[5]);
						break;
					;;


				}
				break;
			;;
			case FUNC_ID_ZW_APPLICATION_UPDATE:
				switch((unsigned char)frame[2]) {
					case UPDATE_STATE_NODE_INFO_RECEIVED:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"FUNC_ID_ZW_APPLICATION_UPDATE:UPDATE_STATE_NODE_INFO_RECEIVED received from node %d - ",(unsigned int)frame[3]);

						ZWNodeMapIt = ZWNodeMap.find((unsigned int)frame[3]);
						if (ZWNodeMapIt != ZWNodeMap.end()) {
							ZWNode *node = (*ZWNodeMapIt).second;
							if (node->sleepingDevice) {
								wakeupHandler((unsigned char) frame[3]);
							}
							parseNodeInfo((unsigned char)frame[3],&(frame[8]),(unsigned char)frame[4]-3);
							updateNodeCapabilities(node, &(frame[8]), (unsigned char)frame[4]-3);
						}
						switch((unsigned char)frame[5]) {
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
													zwRequestBasicReport(atoi(pch),0);
													pch = strtok (NULL, ",");
												}


											}
										}
										break;
									case GENERIC_TYPE_SENSOR_MULTILEVEL:
										DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"GENERIC_TYPE_SENSOR_MULTILEVEL");
										break;
									;;
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
				        case UPDATE_STATE_NEW_ID_ASSIGNED:
					{
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"** Network change **: ID %d was assigned to a new Z-Wave node",(unsigned char)frame[3]);

						newNode = new ZWNode((unsigned char)frame[3]);
						newNode->plutoDeviceTemplateConst=0;
						newNode->typeBasic = (unsigned char) frame[5];
						newNode->typeGeneric = (unsigned char) frame[6];
						newNode->typeSpecific = (unsigned char) frame[7];
						newNode->stateBasic = -1;
						newNode->associationList[0]="";
						newNode->associationList[1]="";
						newNode->associationList[2]="";
						newNode->associationList[3]="";
						newNode->plutoDeviceTemplateConst = getDeviceTemplate((unsigned char)frame[5],(unsigned char)frame[6],(unsigned char)frame[7],NULL,0);


						if ((unsigned char)frame[5] == BASIC_TYPE_ROUTING_SLAVE) {
							zwAssignSUCReturnRoute((unsigned char)frame[3]);
						}
						parseNodeInfo((unsigned char)frame[3],&(frame[8]),(unsigned char)frame[4]-3);

						ZWNodeMap.insert(std::map < int, ZWNode * >::value_type((unsigned char)frame[3],newNode));
						char tmpstr2[1024];
						if (newNode->plutoDeviceTemplateConst != 0) {
							sprintf(tmpstr2, "%d", (unsigned char)frame[3]);
							int iPKChildDevice = -1;
							iPKChildDevice =  DCEcallback->AddDevice(0, tmpstr2, 0, newNode->plutoDeviceTemplateConst);
							if (iPKChildDevice != -1) {
								DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"** Network change **: Created LMCE device %d",iPKChildDevice);
								updateNodeCapabilities(newNode, &(frame[8]), (unsigned char)frame[4]-3);
							}
						}

						if (((unsigned int)frame[6] == 8) && ((unsigned int)frame[7] == 3)) {
							DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Setback schedule thermostat detected, triggering configuration");
							zwWakeupSet((unsigned char)frame[3],30,true);
						}
						sprintf(tmpstr2,"Added new Z-Wave device: %d",(unsigned char)frame[3]);
						SendPopup((void *)myZWave, tmpstr2);
					}
						break;
					case UPDATE_STATE_DELETE_DONE:
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"** Network change **: Z-Wave node %d was removed",(unsigned char)frame[3]);
						char tmpstr[1024];
						sprintf(tmpstr, "%d", (unsigned char)frame[3]);
						DCEcallback->DeleteDevicesForNode(tmpstr);
						sprintf(tmpstr,"Removed Z-Wave device: %d",(unsigned char)frame[3]);
						SendPopup((void *)myZWave, tmpstr);
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
				dropped_jobs = 0;
			} else {
				DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Generic callback handling for command %i, ERROR: wrong callback type: %i",(unsigned char)frame[1],callback_type);

			}

		}


	} else {
		// should not happen
	}
	return NULL;
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

					if (len2>0) DCE::LoggerWrapper::GetInstance()->Write(LV_RECEIVE_DATA, "%s", DCE::IOUtils::FormatHexAsciiBuffer(mybuf2, len2,"33").c_str());
					// verify checksum
					if ((unsigned char) mybuf2[len2-1] == (unsigned char) checksum(mybuf2+1,mybuf2[1])) {
						// printf("Checksum correct - sending ACK\n");
						retbuf[0]=ACK;
						WriteSerialStringEx(serialPort,retbuf,1);
						decodeFrame(mybuf2+2,len2-3);
					} else {
						DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Checksum incorrect %x - sending NAK",(unsigned char) checksum(mybuf2+1,mybuf2[1]));
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
					// DCE::LoggerWrapper::GetInstance()->Write(LV_RECEIVE_DATA, "ACK RECEIVED");
					// if we await an ack pop the command, it got an ACK
					if (await_ack) {
						await_ack = 0;
					}
					if (await_callback == 0) {
						pthread_mutex_lock (&mutexSendQueue);
						ZWSendQueue.pop_front();
						pthread_mutex_unlock (&mutexSendQueue);
						dropped_jobs = 0;
					}
					break;
				;;
				default:
					DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "ERROR! Out of frame flow!!");
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
						DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING, "No callback received: await_callback: %i timer: %li",await_callback,timer);
						timer = 0;
						// resend, we got no final callback
						await_ack = 0;
						await_callback = 0;
						if (ZWSendQueue.front()->sendcount > 2) {
							DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "ERROR: Dropping command, no callback received after three resends");
							dropSendQueueJob();

						}
					}


				}

			} else {
				idletimer++;
				//if ((idletimer % 100) == 0) {
				//	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "IDLE timer: %i",idletimer);

				//}

			}
			pthread_mutex_unlock (&mutexSendQueue);

			// we have been idle for 30 seconds, let's poll the devices
			if ((idletimer > 300) && poll_state) {
				DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "We have been idle for %li seconds, polling device states", idletimer / 10);
				zwRequestBasicReport(NODE_BROADCAST,0);
				zwRequestBasicReport(NODE_BROADCAST,2);
				zwRequestMultilevelSensorReport(NODE_BROADCAST);

				// hack for dianemo
				// multiInstanceGetAllCCsForNode(16);
			}

		}
	}
}

bool ZWApi::ZWApi::sendFunction(char *buffer, size_t length, int type, bool response) {
	ZWJob *newJob;
	newJob = new ZWJob;
	int index;
	unsigned int i;
	index = 0;
	i = 0;

//	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Adding job: %p",newJob);
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
	// DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Setting callback type: %i",newJob->callback_type);

	newJob->buffer[index] = checksum(newJob->buffer+1,length+2+( response ? 1 : 0) );
	ZWSendQueue.push_back(newJob);
	pthread_mutex_unlock (&mutexSendQueue);
	return true;
}

bool ZWApi::ZWApi::sendFunctionSleeping(int nodeid, char *buffer, size_t length, int type, bool response) {
	ZWJob *newJob;
	newJob = new ZWJob;
	unsigned int i;
	int index;
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

void ZWApi::ZWApi::resetNodeInstanceCount(ZWNode *node, string capa) {
        // Reset instance count for CCs
        vector<string> vectCCs;
	StringUtils::Tokenize(capa, ",", vectCCs);
	for (int i = 0; i < vectCCs.size(); i++) {
	        node->mapCCInstanceCount[atoi(vectCCs[i].c_str())] = 0;
	}
}

void ZWApi::ZWApi::multiInstanceGetAllCCsForNode(unsigned int node_id) {
	string capa = DCEcallback->GetCapabilities(node_id, 0);
	vector<string> vectCapa;
	StringUtils::Tokenize(capa, ",", vectCapa);
	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"MultiInstanceGet, node %d CC : %s ", node_id, capa.c_str());
	for (int i = 0; i < vectCapa.size(); i++) {
	        unsigned char cc = atoi(vectCapa[i].c_str());
		zwMultiInstanceGet(node_id, cc);
	}
}

std::string ZWApi::ZWApi::getDeviceList() {
	return deviceList;
}

bool ZWApi::ZWApi::zwMultilevelSet(int node_id, int level, int duration, int instance) {
	char mybuf[1024];

	ZWNodeMapIt = ZWNodeMap.find(node_id);
	if (ZWNodeMapIt != ZWNodeMap.end()) {
			if (instance == 0) {
				mybuf[0] = FUNC_ID_ZW_SEND_DATA;
				mybuf[1] = node_id;
				mybuf[2] = 3;
				mybuf[3] = COMMAND_CLASS_SWITCH_MULTILEVEL;
				mybuf[4] = SWITCH_MULTILEVEL_SET;
				mybuf[5] = level;
				mybuf[6] = duration;
				mybuf[7] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
				sendFunction( mybuf , 8, REQUEST, 1);
			} else {
				mybuf[0] = FUNC_ID_ZW_SEND_DATA;
				mybuf[1] = node_id;
				mybuf[2] = 6;
				mybuf[3] = COMMAND_CLASS_MULTI_INSTANCE;
				mybuf[4] = MULTI_INSTANCE_CMD_ENCAP;
				mybuf[5] = instance;
				mybuf[6] = COMMAND_CLASS_SWITCH_MULTILEVEL;
				mybuf[7] = SWITCH_MULTILEVEL_SET;
				mybuf[8] = level;
				mybuf[9] = duration;
				mybuf[10] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
				sendFunction( mybuf , 11, REQUEST, 1);

			}

	}

	return true;
}

bool ZWApi::ZWApi::zwBasicSet(int node_id, int level, int instance) {
	char mybuf[1024];

	ZWNodeMapIt = ZWNodeMap.find(node_id);
	if (ZWNodeMapIt != ZWNodeMap.end()) {
		// check if it is a setback schedule thermostat
		if ( ((*ZWNodeMapIt).second->typeGeneric == 8) && ((*ZWNodeMapIt).second->typeSpecific == 3) ) {
			// only set the stateBasic, later gets sent as multi command on wakeup
			DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Setback schedule override prepared");
			(*ZWNodeMapIt).second->stateBasic = level == 0 ? 0 : 0xff;

		} else {
			if (instance == 0) {
				mybuf[0] = FUNC_ID_ZW_SEND_DATA;
				mybuf[1] = node_id;
				mybuf[2] = 3;
				mybuf[3] = COMMAND_CLASS_BASIC;
				mybuf[4] = BASIC_SET;
				mybuf[5] = level;
				mybuf[6] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
				sendFunction( mybuf , 7, REQUEST, 1);
			} else {
				mybuf[0] = FUNC_ID_ZW_SEND_DATA;
				mybuf[1] = node_id;
				mybuf[2] = 6;
				mybuf[3] = COMMAND_CLASS_MULTI_INSTANCE;
				mybuf[4] = MULTI_INSTANCE_CMD_ENCAP;
				mybuf[5] = instance;
				mybuf[6] = COMMAND_CLASS_BASIC;
				mybuf[7] = BASIC_SET;
				mybuf[8] = level;
				mybuf[9] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
				sendFunction( mybuf , 10, REQUEST, 1);

			}

		}
	}

	return true;
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
	return true;

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
	return true;

}

bool ZWApi::ZWApi::zwAssociationSetMulti(int node_id, int group, int target_node_id, int instance) {

	char mybuf[1024];

        mybuf[0] = FUNC_ID_ZW_SEND_DATA;
	mybuf[1] = node_id;
	mybuf[2] = 4;
	mybuf[3] = COMMAND_CLASS_MULTI_INSTANCE_ASSOCIATION;
	mybuf[4] = MULTI_INSTANCE_ASSOCIATION_SET;
	mybuf[5] = group;
	mybuf[6] = MULTI_INSTANCE_ASSOCIATION_SET_MARKER;
	mybuf[7] = target_node_id;
	mybuf[8] = instance;
	mybuf[9] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;

	if (zwIsSleepingNode(node_id)) {
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Postpone Association Set - device is not always listening");
		sendFunctionSleeping(node_id, mybuf , 10, REQUEST, 1);
	} else {
		sendFunction( mybuf , 10, REQUEST, 1);

	}
	return true;

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
	return true;

}

bool ZWApi::ZWApi::zwAssociationRemove(int node_id, int group, int target_node_id) {
	char mybuf[1024];

        mybuf[0] = FUNC_ID_ZW_SEND_DATA;
	mybuf[1] = node_id;
	mybuf[2] = 4;
	mybuf[3] = COMMAND_CLASS_ASSOCIATION;
	mybuf[4] = ASSOCIATION_REMOVE;
	mybuf[5] = group;
	mybuf[6] = target_node_id;
	mybuf[7] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;

	if (zwIsSleepingNode(node_id)) {
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Postpone Association Remove - device is not always listening");
		sendFunctionSleeping(node_id, mybuf , 8, REQUEST, 1);
	} else {
		sendFunction( mybuf , 8, REQUEST, 1);

	}
	return true;

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

	return true;
}

bool ZWApi::ZWApi::zwSetDefault() {
	char mybuf[1024];

	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Reset controller and erase all node information");
	mybuf[0] = FUNC_ID_ZW_SET_DEFAULT;
	sendFunction( mybuf , 1, REQUEST, 1);
	return true;
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
	return true;
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
	return true;
}
bool ZWApi::ZWApi::zwRemoveFailedNodeId(int nodeid) {
	char mybuf[1024];

	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Removing failed node %i",nodeid);
	mybuf[0] = FUNC_ID_ZW_REMOVE_FAILED_NODE_ID;
	mybuf[1] = (unsigned char)nodeid;
	sendFunction( mybuf , 2, REQUEST, 1);

	return true;
}
bool ZWApi::ZWApi::zwConfigurationGet(int node_id,int parameter) {
        char mybuf[1024];
        int len = 0;

        mybuf[0] = FUNC_ID_ZW_SEND_DATA;
        mybuf[1] = node_id;
        mybuf[2] = 3;
        mybuf[3] = COMMAND_CLASS_CONFIGURATION;
        mybuf[4] = CONFIGURATION_GET;
        mybuf[5] = parameter; // parameter number
        mybuf[6] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;

        if (zwIsSleepingNode(node_id)) {

                DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Postpone Configuration Get - device is not always listening");
                sendFunctionSleeping(node_id, mybuf , 7, REQUEST, 1);

        } else {
                DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Running Configuration Get");
                sendFunction( mybuf , 7, REQUEST, 1);

        }

        return true;
}


bool ZWApi::ZWApi::zwConfigurationSet(int node_id,int parameter,int value, int size) {
	char mybuf[1024];
	int len = 0;

        mybuf[0] = FUNC_ID_ZW_SEND_DATA;
	mybuf[1] = node_id;
	mybuf[3] = COMMAND_CLASS_CONFIGURATION;
	mybuf[4] = CONFIGURATION_SET;
	mybuf[5] = parameter; // parameter number
	switch (size) {
		case 1:
			mybuf[6] = 1;
			mybuf[7] = value;
			mybuf[8] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
			mybuf[2] = 5;
			len = 9;
			break;
		case 2:
			mybuf[6] = 2;
			mybuf[7] = ( (value >> 8 ) & 0xff);
			mybuf[8] = (value & 0xff);
			mybuf[9] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
			mybuf[2] = 6;
			len = 10;
			break;
		case 4:
			mybuf[6] = 4;
			mybuf[7] = ( (value >> 24) & 0xff );
			mybuf[8] = ( (value >> 16) & 0xff );
			mybuf[9] = ( (value >> 8) & 0xff );
			mybuf[10] = (value & 0xff);
			mybuf[11] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
			mybuf[2] = 8;
			len = 12;
			break;
		case 0: // no size specified, try to auto-detect
		default:
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
			break;
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

	return true;
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
	return true;
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
	pthread_mutex_lock (&mutexSendQueue);

	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Running wakeupHandler for node %i",node_id);

	std::multimap < int, ZWJob * >::iterator wakeupQueueIt;
	for (wakeupQueueIt = ZWWakeupQueue.lower_bound(node_id) ; wakeupQueueIt != ZWWakeupQueue.upper_bound(node_id); ++wakeupQueueIt) {

		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Moving job %p from wakeup to send queue (node %i)",(*wakeupQueueIt).second, (*wakeupQueueIt).first);
		ZWSendQueue.push_back((*wakeupQueueIt).second);
	}
	ZWWakeupQueue.erase(node_id);

	pthread_mutex_unlock (&mutexSendQueue);

	return true;
}

string ZWApi::ZWApi::commandClassToString(char nodeinfo) {
	switch((unsigned char)nodeinfo) {
	case COMMAND_CLASS_WAKE_UP:
		return "COMMAND_CLASS_WAKE_UP";
	case COMMAND_CLASS_MANUFACTURER_SPECIFIC:
		return "COMMAND_CLASS_MANUFACTURER_SPECIFIC";
	case COMMAND_CLASS_ASSOCIATION:
		return "COMMAND_CLASS_ASSOCIATION";
	case COMMAND_CLASS_MULTI_CMD:
		return "COMMAND_CLASS_MULTI_CMD";
	case COMMAND_CLASS_MARK:
		return "* COMMAND_CLASS_MARK *";
	case COMMAND_CLASS_ALARM:
		return "COMMAND_CLASS_ALARM";
	case COMMAND_CLASS_APPLICATION_STATUS:
		return "COMMAND_CLASS_APPLICATION_STATUS";
	case COMMAND_CLASS_ASSOCIATION_COMMAND_CONFIGURATION:
		return "COMMAND_CLASS_ASSOCIATION_COMMAND_CONFIGURATION";
	case COMMAND_CLASS_AV_CONTENT_DIRECTORY_MD:
		return "COMMAND_CLASS_AV_CONTENT_DIRECTORY_MD";
	case COMMAND_CLASS_AV_CONTENT_SEARCH_MD:
		return "COMMAND_CLASS_AV_CONTENT_SEARCH_MD";
	case COMMAND_CLASS_AV_RENDERER_STATUS:
		return "COMMAND_CLASS_AV_RENDERER_STATUS";
	case COMMAND_CLASS_AV_TAGGING_MD:
		return "COMMAND_CLASS_AV_TAGGING_MD";
	case COMMAND_CLASS_BASIC_WINDOW_COVERING:
		return "COMMAND_CLASS_BASIC_WINDOW_COVERING";
	case COMMAND_CLASS_BASIC:
		return "COMMAND_CLASS_BASIC";
	case COMMAND_CLASS_BATTERY:
		return "COMMAND_CLASS_BATTERY";
	case COMMAND_CLASS_CHIMNEY_FAN:
		return "COMMAND_CLASS_CHIMNEY_FAN";
	case COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE:
		return "COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE";
	case COMMAND_CLASS_CLOCK:
		return "COMMAND_CLASS_CLOCK";
	case COMMAND_CLASS_COMPOSITE:
		return "COMMAND_CLASS_COMPOSITE";
	case COMMAND_CLASS_CONFIGURATION:
		return "COMMAND_CLASS_CONFIGURATION";
	case COMMAND_CLASS_CONTROLLER_REPLICATION:
		return "COMMAND_CLASS_CONTROLLER_REPLICATION";
	case COMMAND_CLASS_DOOR_LOCK:
		return "COMMAND_CLASS_DOOR_LOCK";
	case COMMAND_CLASS_ENERGY_PRODUCTION:
		return "COMMAND_CLASS_ENERGY_PRODUCTION";
	case COMMAND_CLASS_FIRMWARE_UPDATE_MD:
		return "COMMAND_CLASS_FIRMWARE_UPDATE_MD";
	case COMMAND_CLASS_GEOGRAPHIC_LOCATION:
		return "COMMAND_CLASS_GEOGRAPHIC_LOCATION";
	case COMMAND_CLASS_GROUPING_NAME:
		return "COMMAND_CLASS_GROUPING_NAME";
	case COMMAND_CLASS_HAIL:
		return "COMMAND_CLASS_HAIL";
	case COMMAND_CLASS_INDICATOR:
		return "COMMAND_CLASS_INDICATOR";
	case COMMAND_CLASS_IP_CONFIGURATION:
		return "COMMAND_CLASS_IP_CONFIGURATION";
	case COMMAND_CLASS_LANGUAGE:
		return "COMMAND_CLASS_LANGUAGE";
	case COMMAND_CLASS_LOCK:
		return "COMMAND_CLASS_LOCK";
	case COMMAND_CLASS_MANUFACTURER_PROPRIETARY:
		return "COMMAND_CLASS_MANUFACTURER_PROPRIETARY";
	case COMMAND_CLASS_METER_PULSE:
		return "COMMAND_CLASS_METER_PULSE";
	case COMMAND_CLASS_METER:
		return "COMMAND_CLASS_METER";
	case COMMAND_CLASS_MTP_WINDOW_COVERING:
		return "COMMAND_CLASS_MTP_WINDOW_COVERING";
	case COMMAND_CLASS_MULTI_INSTANCE_ASSOCIATION:
		return "COMMAND_CLASS_MULTI_INSTANCE_ASSOCIATION";
	case COMMAND_CLASS_MULTI_INSTANCE:
		return "COMMAND_CLASS_MULTI_INSTANCE";
	case COMMAND_CLASS_NO_OPERATION:
		return "COMMAND_CLASS_NO_OPERATION";
	case COMMAND_CLASS_NODE_NAMING:
		return "COMMAND_CLASS_NODE_NAMING";
	case COMMAND_CLASS_NON_INTEROPERABLE:
		return "COMMAND_CLASS_NON_INTEROPERABLE";
	case COMMAND_CLASS_POWERLEVEL:
		return "COMMAND_CLASS_POWERLEVEL";
	case COMMAND_CLASS_PROPRIETARY:
		return "COMMAND_CLASS_PROPRIETARY";
	case COMMAND_CLASS_PROTECTION:
		return "COMMAND_CLASS_PROTECTION";
	case COMMAND_CLASS_REMOTE_ASSOCIATION_ACTIVATE:
		return "COMMAND_CLASS_REMOTE_ASSOCIATION_ACTIVATE";
	case COMMAND_CLASS_REMOTE_ASSOCIATION:
		return "COMMAND_CLASS_REMOTE_ASSOCIATION";
	case COMMAND_CLASS_SCENE_ACTIVATION:
		return "COMMAND_CLASS_SCENE_ACTIVATION";
	case COMMAND_CLASS_SCENE_ACTUATOR_CONF:
		return "COMMAND_CLASS_SCENE_ACTUATOR_CONF";
	case COMMAND_CLASS_SCENE_CONTROLLER_CONF:
		return "COMMAND_CLASS_SCENE_CONTROLLER_CONF";
	case COMMAND_CLASS_SCREEN_ATTRIBUTES:
		return "COMMAND_CLASS_SCREEN_ATTRIBUTES";
	case COMMAND_CLASS_SCREEN_MD:
		return "COMMAND_CLASS_SCREEN_MD";
	case COMMAND_CLASS_SECURITY:
		return "COMMAND_CLASS_SECURITY";
	case COMMAND_CLASS_SENSOR_ALARM:
		return "COMMAND_CLASS_SENSOR_ALARM";
	case COMMAND_CLASS_SENSOR_BINARY:
		return "COMMAND_CLASS_SENSOR_BINARY";
	case COMMAND_CLASS_SENSOR_CONFIGURATION:
		return "COMMAND_CLASS_SENSOR_CONFIGURATION";
	case COMMAND_CLASS_SENSOR_MULTILEVEL:
		return "COMMAND_CLASS_SENSOR_MULTILEVEL";
	case COMMAND_CLASS_SILENCE_ALARM:
		return "COMMAND_CLASS_SILENCE_ALARM";
	case COMMAND_CLASS_SIMPLE_AV_CONTROL:
		return "COMMAND_CLASS_SIMPLE_AV_CONTROL";
	case COMMAND_CLASS_SWITCH_ALL:
		return "COMMAND_CLASS_SWITCH_ALL";
	case COMMAND_CLASS_SWITCH_BINARY:
		return "COMMAND_CLASS_SWITCH_BINARY";
	case COMMAND_CLASS_SWITCH_MULTILEVEL:
		return "COMMAND_CLASS_SWITCH_MULTILEVEL";
	case COMMAND_CLASS_SWITCH_TOGGLE_BINARY:
		return "COMMAND_CLASS_SWITCH_TOGGLE_BINARY";
	case COMMAND_CLASS_SWITCH_TOGGLE_MULTILEVEL:
		return "COMMAND_CLASS_SWITCH_TOGGLE_MULTILEVEL";
	case COMMAND_CLASS_THERMOSTAT_FAN_MODE:
		return "COMMAND_CLASS_THERMOSTAT_FAN_MODE";
	case COMMAND_CLASS_THERMOSTAT_FAN_STATE:
		return "COMMAND_CLASS_THERMOSTAT_FAN_STATE";
	case COMMAND_CLASS_THERMOSTAT_HEATING:
		return "COMMAND_CLASS_THERMOSTAT_HEATING";
	case COMMAND_CLASS_THERMOSTAT_MODE:
		return "COMMAND_CLASS_THERMOSTAT_MODE";
	case COMMAND_CLASS_THERMOSTAT_OPERATING_STATE:
		return "COMMAND_CLASS_THERMOSTAT_OPERATING_STATE";
	case COMMAND_CLASS_THERMOSTAT_SETBACK:
		return "COMMAND_CLASS_THERMOSTAT_SETBACK";
	case COMMAND_CLASS_THERMOSTAT_SETPOINT:
		return "COMMAND_CLASS_THERMOSTAT_SETPOINT";
	case COMMAND_CLASS_TIME_PARAMETERS:
		return "COMMAND_CLASS_TIME_PARAMETERS";
	case COMMAND_CLASS_TIME:
		return "COMMAND_CLASS_TIME";
	case COMMAND_CLASS_USER_CODE:
		return "COMMAND_CLASS_USER_CODE";
	case COMMAND_CLASS_VERSION:
		return "COMMAND_CLASS_VERSION";
	case COMMAND_CLASS_ZIP_ADV_CLIENT:
		return "COMMAND_CLASS_ZIP_ADV_CLIENT";
	case COMMAND_CLASS_ZIP_ADV_SERVER:
		return "COMMAND_CLASS_ZIP_ADV_SERVER";
	case COMMAND_CLASS_ZIP_ADV_SERVICES:
		return "COMMAND_CLASS_ZIP_ADV_SERVICES";
	case COMMAND_CLASS_ZIP_CLIENT:
		return "COMMAND_CLASS_ZIP_CLIENT";
	case COMMAND_CLASS_ZIP_SERVER:
		return "COMMAND_CLASS_ZIP_SERVER";
	case COMMAND_CLASS_ZIP_SERVICES:
		return "COMMAND_CLASS_ZIP_SERVICES";
	default:
		return "<unknown command class>";
	}
}

void ZWApi::ZWApi::parseNodeInfo(int nodeid, char *nodeinfo, size_t length) {
	bool wakeup = false;
	bool multicommand = false;
	bool security = false;
	bool multiinstance = false;
	bool association = false;
	bool manufacturerspecific = false;
	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Supported command classes:");
	for (unsigned int i=0;i<length;i++) {
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "%s", commandClassToString(nodeinfo[i]).c_str());
		switch ((unsigned char)nodeinfo[i]) {
		case COMMAND_CLASS_WAKE_UP:
			wakeup = true;
			break;
		case COMMAND_CLASS_MANUFACTURER_SPECIFIC:
			manufacturerspecific = true;
			break;
		case COMMAND_CLASS_ASSOCIATION:
			association = true;
			break;
		case COMMAND_CLASS_MULTI_CMD:
			multicommand = true;
			break;
		case COMMAND_CLASS_MULTI_INSTANCE:
			multiinstance = true;
			break;
		case COMMAND_CLASS_SECURITY:
			security = true;
			break;
		case COMMAND_CLASS_MARK:
			DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Can control the following command classes:");
			break;
		default:
			break;
		}
	}

	// TODO: this should really go into a seperate function with device detection

	if (multicommand) {
		if (wakeup) zwWakeupSet(nodeid,15,true); // wakeup interval

	} else {
		if (security) zwSecuritySchemeGet(nodeid);
		if (association) zwAssociationSet(nodeid, 1, ournodeid); // associate to group 1
		if (wakeup) zwWakeupSet(nodeid,60,false); // wakeup interval
		if (manufacturerspecific) zwRequestManufacturerSpecificReport(nodeid);
	}
}

int ZWApi::ZWApi::getDeviceTemplate(int basic, int generic, int specific, char *nodeinfo, size_t len) {
	int devicetemplate = 0;
	switch(generic) {
		case GENERIC_TYPE_GENERIC_CONTROLLER:
		case GENERIC_TYPE_STATIC_CONTROLLER:
#ifdef DEVICETEMPLATE_ZWave_Controller_CONST
			devicetemplate = DEVICETEMPLATE_ZWave_Controller_CONST;
#endif
			break;
		case GENERIC_TYPE_THERMOSTAT:
			devicetemplate = DEVICETEMPLATE_Standard_Thermostat_CONST;
			break;
		case GENERIC_TYPE_SWITCH_MULTILEVEL:
			switch (specific) {
				case SPECIFIC_TYPE_MOTOR_MULTIPOSITION:
				case SPECIFIC_TYPE_CLASS_A_MOTOR_CONTROL:
				case SPECIFIC_TYPE_CLASS_B_MOTOR_CONTROL:
				case SPECIFIC_TYPE_CLASS_C_MOTOR_CONTROL:
					devicetemplate = DEVICETEMPLATE_Drapes_Switch_CONST;
					break;
				case SPECIFIC_TYPE_NOT_USED:
				case SPECIFIC_TYPE_POWER_SWITCH_MULTILEVEL:
				case SPECIFIC_TYPE_SCENE_SWITCH_MULTILEVEL:
					devicetemplate = DEVICETEMPLATE_Light_Switch_dimmable_CONST;
					break;
			}

			break;
		case GENERIC_TYPE_SWITCH_REMOTE:
#ifdef DEVICETEMPLATE_Remote_Switch_CONST
			devicetemplate = DEVICETEMPLATE_Remote_Switch_CONST;
#else
#ifdef DEVICETEMPLATE_ZWave_Controller_CONST
			devicetemplate = DEVICETEMPLATE_ZWave_Controller_CONST;
#endif
#endif
			break;
		case GENERIC_TYPE_SWITCH_BINARY:
			devicetemplate = DEVICETEMPLATE_Light_Switch_onoff_CONST;
			break;
		case GENERIC_TYPE_SENSOR_BINARY:
			devicetemplate = DEVICETEMPLATE_Generic_Sensor_CONST;
			break;
		case GENERIC_TYPE_WINDOW_COVERING:
			devicetemplate = DEVICETEMPLATE_Drapes_Switch_CONST;
			break;
		case GENERIC_TYPE_SENSOR_MULTILEVEL:
#ifdef DEVICETEMPLATE_Multilevel_Sensor_CONST
			devicetemplate = DEVICETEMPLATE_Multilevel_Sensor_CONST;
#else
			devicetemplate = DEVICETEMPLATE_Generic_Sensor_CONST;
#endif
			break;
		case GENERIC_TYPE_SENSOR_ALARM:
			// DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "alarm sensor found, specific: %i",specific);
			switch(specific) {
				case SPECIFIC_TYPE_BASIC_ROUTING_SMOKE_SENSOR:
				case SPECIFIC_TYPE_ROUTING_SMOKE_SENSOR:
				case SPECIFIC_TYPE_BASIC_ZENSOR_NET_SMOKE_SENSOR:
				case SPECIFIC_TYPE_ZENSOR_NET_SMOKE_SENSOR:
				case SPECIFIC_TYPE_ADV_ZENSOR_NET_SMOKE_SENSOR:
					devicetemplate = DEVICETEMPLATE_Smoke_Detector_CONST;
					// DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "smoke detector found");
				break;
			}
		break;  // hari, shame on you, forgetting a break here. ;) -tschak
		case GENERIC_TYPE_AV_CONTROL_POINT:
			devicetemplate = DEVICETEMPLATE_Generic_Tuner_CONST;
			break;
	}
	return devicetemplate;

}

bool ZWApi::ZWApi::zwRequestNodeNeighborUpdate(int node_id) {
	char mybuf[1024];

	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Requesting Neighbour Update for node %i",node_id);
	mybuf[0] = FUNC_ID_ZW_REQUEST_NODE_NEIGHBOR_UPDATE;
	mybuf[1] = node_id;
	sendFunction( mybuf , 2, REQUEST, 0);
	return true;
}

bool  ZWApi::ZWApi::zwAssignReturnRoute(int node_id, int target_node_id){
	char mybuf[1024];

	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Assigning return route for node %i, targetr %i",node_id,target_node_id);
	mybuf[0] = FUNC_ID_ZW_ASSIGN_RETURN_ROUTE;
	mybuf[1] = node_id;
	mybuf[2] = target_node_id;
	sendFunction( mybuf , 3, REQUEST, 1);
	return true;
}

void ZWApi::ZWApi::zwReadMemory(int offset, int len) {
	char mybuf[1024];

	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Reading eeprom at offset %i",offset);
	mybuf[0] = 0x23;
	mybuf[1] = (offset >> 8) & 0xff;
	mybuf[2] = offset & 0xff;;
	mybuf[3] = len & 0xff;
	sendFunction( mybuf , 4, REQUEST, 0);
}

void ZWApi::ZWApi::zwWriteMemory(int offset, int len, unsigned char *data) {
	char mybuf[1024];

	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Writing eeprom at offset %i",offset);
	mybuf[0] = 0x24; // MemoryPutBuffer
	mybuf[1] = (offset >> 8) & 0xff;
	mybuf[2] = offset & 0xff;;
	mybuf[3] = (len >> 8) & 0xff;
	mybuf[4] = len & 0xff;
	if (len > 100) {
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "length too long");
	} else {
		for (int i=0;i<len;i++) {
			mybuf[5+i]=data[i];
		}
	}
	sendFunction( mybuf , 5+len, REQUEST, 0);
}

void ZWApi::ZWApi::zwControllerBackup() {
	// we need to dump everything from 0-16383

	//for (int i=0;i<273;i++) {
	//	zwReadMemory(60*i,60);
	// }
	// zwReadMemory(16380,3);
	memory_dump_offset = 0;
	memory_dump_len = 60;
	zwReadMemory(memory_dump_offset,memory_dump_len);
	poll_state=0;
}
void ZWApi::ZWApi::zwControllerRestore() {
	int fd,readsize;
	fd = open("/tmp/zwave-controller-backup.dump",O_RDONLY);
	readsize = read(fd, memory_dump, 16384);
	close(fd);
	if (readsize == 16384) {
		poll_state=0;
		DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING,"reading memory dump from file");
		for (int i=0;i<16384;i++) {
			if (i%32==0) { printf("\n"); }
			printf("%2x ",memory_dump[i]);
		}
		printf("\n");
		memory_dump_offset = 0;
		memory_dump_len = 60;
		zwWriteMemory(memory_dump_offset, memory_dump_len, memory_dump);
	}

}


void ZWApi::ZWApi::zwSoftReset() {
	char mybuf[1024];

	DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Soft-resetting the Z-Wave chip");
	mybuf[0] = FUNC_ID_SERIAL_API_SOFT_RESET;
	sendFunction( mybuf , 1, REQUEST, 0);
}


void ZWApi::ZWApi::zwRequestBasicReport(int node_id, int instance) {
	char mybuf[1024];

	if (instance == 0) {
		mybuf[0] = FUNC_ID_ZW_SEND_DATA;
		mybuf[1] = node_id;
		mybuf[2] = 0x02;
		mybuf[3] = COMMAND_CLASS_BASIC;
		mybuf[4] = BASIC_GET;
		mybuf[5] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
		sendFunction (mybuf,6,REQUEST,1);
	} else {

		mybuf[0] = FUNC_ID_ZW_SEND_DATA;
		mybuf[1] = node_id;
		mybuf[2] = 5;
		mybuf[3] = COMMAND_CLASS_MULTI_INSTANCE;
		mybuf[4] = MULTI_INSTANCE_CMD_ENCAP;
		mybuf[5] = instance;
		mybuf[6] = COMMAND_CLASS_BASIC;
		mybuf[7] = BASIC_GET;
		mybuf[8] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
		sendFunction( mybuf , 9, REQUEST, 1);
	}
}
void ZWApi::ZWApi::zwRequestManufacturerSpecificReport(int node_id) {
	char mybuf[1024];

	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Requesting manufacturer specific information from node %d", node_id);
	mybuf[0] = FUNC_ID_ZW_SEND_DATA;
	mybuf[1] = node_id;
	mybuf[2] = 2; // length of command
	mybuf[3] = COMMAND_CLASS_MANUFACTURER_SPECIFIC;
	mybuf[4] = MANUFACTURER_SPECIFIC_GET;
	mybuf[5] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
	sendFunction( mybuf , 6, REQUEST, 1);

}

void ZWApi::ZWApi::zwRequestMultilevelSensorReport(int node_id) {
	char mybuf[1024];

	mybuf[0] = FUNC_ID_ZW_SEND_DATA;
	mybuf[1] = node_id;
	mybuf[2] = 2; // length of command
	mybuf[3] = COMMAND_CLASS_SENSOR_MULTILEVEL;
	mybuf[4] = SENSOR_MULTILEVEL_GET;
	mybuf[5] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
	sendFunction( mybuf , 6, REQUEST, 1);
}

void ZWApi::ZWApi::zwRequestNodeInfo(int node_id) {
	char mybuf[1024];
	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Requesting node info from node %d", node_id);
	mybuf[0] = FUNC_ID_ZW_REQUEST_NODE_INFO;
	mybuf[1] = node_id;
	sendFunction( mybuf , 2, REQUEST, 0);
}

void ZWApi::ZWApi::zwRequestVersion(int node_id) {
	char mybuf[1024];

	mybuf[0] = FUNC_ID_ZW_SEND_DATA;
	mybuf[1] = node_id;
	mybuf[2] = 2; // length of command
	mybuf[3] = COMMAND_CLASS_VERSION;
	mybuf[4] = VERSION_GET;
	mybuf[5] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
	sendFunction( mybuf , 6, REQUEST, 1);
}

void ZWApi::ZWApi::zwStatusReport() {
	ZWNodeMapIt = ZWNodeMap.begin();
	while (ZWNodeMapIt!=ZWNodeMap.end()) {
		ZWNode *node = (*ZWNodeMapIt).second;
		if (!(node->sleepingDevice)) {
			zwRequestNodeNeighborUpdate((*ZWNodeMapIt).first);
		//	zwRequestManufacturerSpecificReport((*ZWNodeMapIt).first);
		}
		ZWNodeMapIt++;
	}
}

void ZWApi::ZWApi::zwPollDevices(bool onoff) {
	poll_state = onoff;
}

std::string ZWApi::ZWApi::nodeInfo2String(char *nodeInfo, size_t len, bool bInclControlCC) {

	string tmp = "";
	char tmp2[1000];

	for (unsigned int i=0;i<len;i++) {
		if ((unsigned char)nodeInfo[i] == COMMAND_CLASS_MARK && !bInclControlCC) {
			return tmp;
		}
		sprintf(tmp2,"%d",(unsigned char)nodeInfo[i]);
		if (tmp == "") {
			tmp += tmp2;
		} else {
			tmp += ",";
			tmp  += tmp2;
		}
	}

	return tmp;
}

void ZWApi::ZWApi::updateNodeCapabilities(ZWNode* node, char *nodeinfo, size_t length) {
	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Updating node capabilities for node %d",node->id);
	string cclasses = nodeInfo2String(nodeinfo, length, false);
	resetNodeInstanceCount(node, cclasses);
	if (DCEcallback->SetCapabilities(node->id, 0, cclasses)) {
		// Check multi instance
		if ( cclasses.find(COMMAND_CLASS_MULTI_INSTANCE) != string::npos ) {
			multiInstanceGetAllCCsForNode(node->id);
		}
	}
}

bool ZWApi::ZWApi::zwMeterGet(int node_id) {
	char mybuf[1024];

	mybuf[0] = FUNC_ID_ZW_SEND_DATA;
	mybuf[1] = node_id;
	mybuf[2] = 2; // length of command
	mybuf[3] = COMMAND_CLASS_METER;
	mybuf[4] = METER_GET;
	mybuf[5] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
	sendFunction( mybuf , 6, REQUEST, 1);

}

void ZWApi::ZWApi::zwMultiInstanceGet(int node_id, int command_class){

        char mybuf[1024];

        mybuf[0] = FUNC_ID_ZW_SEND_DATA;
        mybuf[1] = node_id;
        mybuf[2] = 3; // length of command
        mybuf[3] = COMMAND_CLASS_MULTI_INSTANCE;
        mybuf[4] = MULTI_INSTANCE_GET;
        mybuf[5] = command_class;
        mybuf[6] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
        sendFunction( mybuf , 7, REQUEST, 1);


}
void ZWApi::ZWApi::zwRequestMultilevelSensorReportInstance(int node_id,int instance){
        char mybuf[1024];

        mybuf[0] = FUNC_ID_ZW_SEND_DATA;
        mybuf[1] = node_id;
        mybuf[2] = 5; // length of command
	mybuf[3] = COMMAND_CLASS_MULTI_INSTANCE;
	mybuf[4] = MULTI_INSTANCE_CMD_ENCAP;
	mybuf[5] = instance;
        mybuf[6] = COMMAND_CLASS_SENSOR_MULTILEVEL;
        mybuf[7] = SENSOR_MULTILEVEL_GET;
        mybuf[8] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
        sendFunction( mybuf , 9, REQUEST, 1);

}

bool ZWApi::ZWApi::zwThermostatFanModeSet(int node_id, int fan_mode) {
        char mybuf[1024];

        mybuf[0] = FUNC_ID_ZW_SEND_DATA;
        mybuf[1] = node_id;
        mybuf[2] = 3; // length of command
	mybuf[3] = COMMAND_CLASS_THERMOSTAT_FAN_MODE;
	mybuf[4] = THERMOSTAT_FAN_MODE_SET;
	mybuf[5] = fan_mode & 0xf; // zero high nibble conf. to specs
        mybuf[6] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
        sendFunction( mybuf , 7, REQUEST, 1);

}

bool ZWApi::ZWApi::zwThermostatModeSet(int node_id, int mode) {
        char mybuf[1024];

        mybuf[0] = FUNC_ID_ZW_SEND_DATA;
        mybuf[1] = node_id;
        mybuf[2] = 3; // length of command
	mybuf[3] = COMMAND_CLASS_THERMOSTAT_MODE;
	mybuf[4] = THERMOSTAT_MODE_SET;
	mybuf[5] = mode & 0x1f; // zero highest 3 bit
        mybuf[6] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
        sendFunction( mybuf , 7, REQUEST, 1);

}

void ZWApi::ZWApi::zwThermostatModeGet(int node_id) {
        char mybuf[1024];

        mybuf[0] = FUNC_ID_ZW_SEND_DATA;
        mybuf[1] = node_id;
        mybuf[2] = 2; // length of command
	mybuf[3] = COMMAND_CLASS_THERMOSTAT_MODE;
	mybuf[4] = THERMOSTAT_MODE_GET;
        mybuf[5] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
        sendFunction( mybuf , 6, REQUEST, 1);

}
bool ZWApi::ZWApi::zwThermostatSetpointSet(int node_id, int type, int value) {
        char mybuf[1024];

        mybuf[0] = FUNC_ID_ZW_SEND_DATA;
        mybuf[1] = node_id;
        mybuf[2] = 5; // length of command
	mybuf[3] = COMMAND_CLASS_THERMOSTAT_SETPOINT;
	mybuf[4] = THERMOSTAT_MODE_SET;
	mybuf[5] = type & 0xf; // zero high-nibble
	mybuf[6] = 1; // 3 bit precision, 2 bit scale (0 = C,1=F), 3 bit size
	mybuf[7] = value; // TODO: proper negative values..
        mybuf[8] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
	if (zwIsSleepingNode(node_id)) {
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Postpone thermostat setpoint set - device is not always listening");
		sendFunctionSleeping( node_id, mybuf , 9, REQUEST, 1);
	} else {
		sendFunction( mybuf , 9, REQUEST, 1);
	}

}
void ZWApi::ZWApi::zwThermostatSetpointGet(int node_id,int type) {
        char mybuf[1024];

        mybuf[0] = FUNC_ID_ZW_SEND_DATA;
        mybuf[1] = node_id;
        mybuf[2] = 3; // length of command
	mybuf[3] = COMMAND_CLASS_THERMOSTAT_SETPOINT;
	mybuf[4] = THERMOSTAT_SETPOINT_GET;
	mybuf[5] = type & 0xf; // zero high nibble
        mybuf[6] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
        sendFunction( mybuf , 7, REQUEST, 1);

}

void ZWApi::ZWApi::zwAVControlSet(int node_id, int sequence, int command) {
	char mybuf[1024];

	mybuf[0] = FUNC_ID_ZW_SEND_DATA;
	mybuf[1] = node_id;
	mybuf[2] = 8; // length
	mybuf[3] = COMMAND_CLASS_SIMPLE_AV_CONTROL;
	mybuf[4] = SIMPLE_AV_CONTROL_SET;
	mybuf[5] = sequence & 0xff; // needs to be increased so that the device can detect duplicates
	mybuf[6] = 0; // 0 = key down, 1 = key up, 2 = keep alive
	mybuf[7] = 0; // item id, not used in the simple class
	mybuf[8] = 0; // item id lsb
	mybuf[9] = (command & 0xff00) >> 8; // command 1 msb
	mybuf[10] = command & 0xff; // command 1 lsb
//	mybuf[xxx] = command n ...
	mybuf[11] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
        sendFunction( mybuf , 12, REQUEST, 1);
}


void ZWApi::ZWApi::dropSendQueueJob() {
	// pthread_mutex_lock (&mutexSendQueue);
	ZWSendQueue.pop_front();
	// pthread_mutex_unlock (&mutexSendQueue);
	// await_callback = 0;
	dropped_jobs++;
	if( dropped_jobs >= 6 ) {
		// If you get this error, take a look at Trac ticket #874
		DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "ERROR: Three dropped commands in a row, soft-resetting controller");
		pthread_mutex_unlock (&mutexSendQueue);
		//zwSoftReset();
		pthread_mutex_lock (&mutexSendQueue);
		dropped_jobs = 0;
	}
}

void ZWApi::ZWApi::zwGetBatteryLevel(int node_id){
        char mybuf[1024];

        mybuf[0] = FUNC_ID_ZW_SEND_DATA;
	mybuf[1] = node_id;
	mybuf[2] = 2;
	mybuf[3] = COMMAND_CLASS_BATTERY;
	mybuf[4] = BATTERY_GET;
	mybuf[7] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;

	if (zwIsSleepingNode(node_id)) {
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Postpone readout of battery level - device is not always listening");
		sendFunctionSleeping(node_id, mybuf , 6, REQUEST, 1);
	} else {
		sendFunction( mybuf , 6, REQUEST, 1);

	}
}

bool ZWApi::ZWApi::zwAssignSUCReturnRoute(int node_id){

	char mybuf[1024];

	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Assigning SUC return route for node %i",node_id);
	mybuf[0] = FUNC_ID_ZW_ASSIGN_SUC_RETURN_ROUTE;
	mybuf[1] = node_id;
	mybuf[2] = 0;
	sendFunction( mybuf , 2, REQUEST, 1);
	return true;
}

bool ZWApi::ZWApi::zwSetPromiscMode(bool promisc){
	char mybuf[1024];

	DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Setting promisc mode");
	mybuf[0] = 0xd0;
	mybuf[1] = promisc;
	sendFunction( mybuf , 2, REQUEST, 0);
	return true;
}

void ZWApi::ZWApi::zwSetClock(int node_id) {
	char mybuf[1024];
	time_t timestamp;
	struct tm *timestruct;

	timestamp = time(NULL);
	timestruct = localtime(&timestamp);

        mybuf[0] = FUNC_ID_ZW_SEND_DATA;
        mybuf[1] = node_id;
        mybuf[2] = 4; // length of command
	mybuf[3] = COMMAND_CLASS_CLOCK;
//	mybuf[4] = CLOCK_SET;
	mybuf[4]=CLOCK_REPORT;
	mybuf[5]=((timestruct->tm_wday == 0 ? 7 : timestruct->tm_wday) << 5 ) | timestruct->tm_hour;
	mybuf[6]=timestruct->tm_min == 60 ? 0 : timestruct->tm_min;
        mybuf[7] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
        sendFunction( mybuf , 8, REQUEST, 1);
}

void ZWApi::ZWApi::zwGetRoutingInfo(int node_id){
	char mybuf[1024];

        DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Fetching neighbor list from node %i",node_id);
	mybuf[0] = ZW_GET_ROUTING_INFO;
	mybuf[1] = node_id;
	mybuf[2] = 0;
	mybuf[3] = 0;
	sendFunction( mybuf , 4, REQUEST, 0);


}

void ZWApi::ZWApi::parseManufacturerSpecific(int nodeid, int manuf, int type, int prod){
	string sManufacturer;
	string sProduct;

	switch(manuf) {
		case 0x1: sManufacturer = "ACT";
			switch (type) {
				case 0x4952: switch(prod) {
						case 0x3030: sProduct = "ZIR000 PIR Motion Sensor";
							// zwAssociationSet(nodeid, 1, ournodeid); // receive group 1 events (motion)
							zwAssociationSet(nodeid, 2, ournodeid); // receive group 2 events (casing opened)
							zwAssociationSet(nodeid, 3, ournodeid); // receive group 3 events (battery reports)
							zwConfigurationSet(nodeid,17,2,0); // sensor mode
							zwConfigurationSet(nodeid,18,0,0); // no delay
							zwConfigurationSet(nodeid,19,1,0); // send unsolicited events
							zwConfigurationSet(nodeid,22,30,0); // wakeup duration
							break;
						case 0x3330: sProduct = "ZIR010 PIR Motion Sensor";
							zwAssociationSet(nodeid, 1, ournodeid); // receive group 1 events (motion)
							zwAssociationSet(nodeid, 2, ournodeid); // receive group 2 events (casing opened)
							zwAssociationSet(nodeid, 3, ournodeid); // receive group 3 events (battery reports)
							zwConfigurationSet(nodeid,17,2,0); // sensor mode
							zwConfigurationSet(nodeid,18,0,0); // no delay
							zwConfigurationSet(nodeid,19,1,0); // send unsolicited events
							zwConfigurationSet(nodeid,22,30,0); // wakeup duration
							break;
					}
					break;
				case 0x4450: switch(prod) {
						case 0x3030: sProduct = "ZDP100 Plugin Lamp Module";
							break;
						case 0x3330: sProduct = "ZDP200 Plugin Lamp Module";
                                                        break;
					}
					break;
				case 0x4457: switch(prod) {
						case 0x3330: sProduct = "ZDW230 Wall Dimmer Module";
							break;
						case 0x3332: sProduct = "ZDW232 Wall Dimmer Module";
							break;
					}
					break;
				case 0x5250: switch(prod) {
						case 0x3030: sProduct = "ZRP100 Plugin Appliance Module";
							break;
						case 0x3330: sProduct = "ZRP200 Plugin Appliance Module";
							break;
					}
					break;
				case 0x5457: switch(prod) {
						case 0x3330: sProduct = "ZTW230 Wall Transmitter Module";
							break;
					}
					break;
				case 0x544d: switch(prod) {
						case 0x3330: sProduct = "ZTM230 Wall Transmitter Module";
							break;
					}
					break;
			}
			break;
		case 0x2: sManufacturer = "Danfoss";
			switch (type) {
				case 0x64: switch(prod) {
						case 0x1: sProduct = "RA PLUS-w Radiator Thermostat";
							break;
					}
					break;
			}
			break;
		case 0x5: sManufacturer = "Intermatic";
			switch (type) {
				case 0x3: switch(prod) {
						case 0x3: sProduct = "HA-03WD Lamp Module";
							break;
					}
					break;
			}
			break;
		case 0x8: sManufacturer = "Wayne Dalton";
			switch (type) {
				case 0x7: switch(prod) {
						case 0x20: sProduct = "WDTC-20 Thermostat";
							break;
					}
					break;
			}
			break;
		case 0x12: sManufacturer = "E Housekeeper";
			switch (type) {
				case 0x1: switch(prod) {
						case 0x1: sProduct = "Switchkeeper Plug-in Appliance Module";
							break;
					}
					break;
			}
			break;
		case 0x1a: sManufacturer = "Cooper";
			switch (type) {
				case 0x4449: switch(prod) {
						case 0x2: sProduct = "RF9534DS Wall Dimmer Module";
							break;
					}
					break;
			}
			break;
		case 0x1d: sManufacturer = "Leviton";
			break;
		case 0x1e: sManufacturer = "Express Controls";
			switch (type) {
				case 0x2: switch(prod) {
						case 0x1: sProduct = "EZMotion Wireless Multi-Sensor (aka Homeseer HSM100)";
							break;
					}
					break;
			}
			break;
		case 0x33: sManufacturer = "Electronic Solutions Inc";
			switch (type) {
				case 0x5250: switch(prod) {
						case 0x2: sProduct = "DBMZ Motor Control";
							break;
					}
					break;
			}
			break;
		case 0x40: sManufacturer = "MK Honeywell Astral";
			switch (type) {
				case 0x5: switch(prod) {
						case 0x102: sProduct = "LSM12U C 2 GANG 10AX SWITCH MODULE";
							break;
					}
					break;
			}
			break;
		case 0x59: sManufacturer = "Horstmann";
			switch (type) {
				case 0x1: switch(prod) {
						case 0x1: sProduct = "ASR-RF Thermostat Receiver";
							break;
						case 0x2: sProduct = "AS2-RF Thermostat Transmitter";
							break;
						case 0x3: sProduct = "HRT4-ZW Thermostat";
							zwWakeupSet(nodeid,10,false);
							zwConfigurationSet(nodeid,1,128,1); // enable temperature sensor
							zwConfigurationSet(nodeid,2,0,1); // enable celsius readings
							zwConfigurationSet(nodeid,3,5,1); // enable 0.5degC stepping
							zwAssociationSet(nodeid, 1, ournodeid);  // receive thermostat mode set
							zwAssociationSet(nodeid, 3, ournodeid);  // receive battery reports
							zwAssociationSet(nodeid, 4, ournodeid);  // receive setpoint reports
							zwAssociationSet(nodeid, 5, ournodeid);  // receive temperature reports
							break;
					}
					break;
			}
			break;
		case 0x60: sManufacturer = "Everspring";
			switch (type) {
				case 0x1: switch(prod) {
						case 0x1: sProduct = "SP103 PIR Motion Sensor";
							break;
					}
					break;
				case 0x202:
				case 0x2: switch(prod) {
						case 0x1: sProduct = "SM103 Door/Window Magnetic Sensor";
							break;
					}
					break;
				case 0xd: switch(prod) {
						case 0x1: sProduct = "SF812 Smoke Detector";
							break;
					}
					break;
			}
			break;
		case 0x63: sManufacturer = "GE";
			switch (type) {
				case 0x5250: switch(prod) {
						case 0x3030: sProduct = "GE 45603 Plugin Appliance Module";
							break;
					}
					break;
			}
			break;
		case 0x64: sManufacturer = "Popp/Duewi";
			switch (type) {
				case 0x1: switch(prod) {
						case 0x0: sProduct = "DUW_064336 Dimmer insert";
							break;
					}
					break;
			}
			break;
		case 0x69: sManufacturer = "Seluxit";
			switch (type) {
				case 0x3: switch(prod) {
						case 0x2: sProduct = "Multisensor";
							zwAssociationSet(nodeid, 3, ournodeid);
							zwAssociationSet(nodeid, 6, ournodeid);
							zwAssociationSet(nodeid, 9, ournodeid);
							zwAssociationSet(nodeid, 14, ournodeid);
							break;
					}
					break;
			}
			break;
		case 0x6f: sManufacturer = "Home Manageables";
			switch (type) {
				case 0x102: switch(prod) {
						case 0x1: sProduct = "Door/Window Sensor";
							break;
					}
					break;
			}
			break;
		case 0x77: sManufacturer = "Innovus";
			switch (type) {
				case 0x1: switch(prod) {
						case 0x1: sProduct = "RAone SmartDimmer Wall Dimmer Module";
							break;
						case 0x2: sProduct = "RAone SmartPower Wall Appliance Module";
							break;
					}
					break;
			}
			break;
		case 0x7a: sManufacturer = "Merten";
			switch (type) {
				case 0x1: switch(prod) {
						case 0x2: sProduct = "Transmitter 1-Gang Switch";
							break;
						case 0x4: sProduct = "Transmitter 2-Gang Switch";
							break;
					}
					break;
				case 0x2: switch(prod) {
						case 0x2: sProduct = "Transmitter Move";
							break;
					}
					break;
				case 0x3: switch(prod) {
						case 0x4: sProduct = "Transmitter Flush-Mounted 4-Gang Switch";
							break;
					}
					break;
				case 0x4002: switch(prod) {
						case 0x1: sProduct = "Wall Appliance Module";
							break;
					}
					break;
				case 0x4003: switch(prod) {
						case 0x1: sProduct = "Wall Dimmer Module";
							break;
					}
					break;
				case 0x4004: switch(prod) {
						case 0x1: sProduct = "Wall Roller Shutter Module";
							break;
					}
					break;
				case 0x8001: switch(prod) {
						case 0x1: sProduct = "Plug-In Appliance Module";
							break;
						case 0x8001: sProduct = "Receiver Flush-Mounted 1-Gang Switch";
							break;
					}
					break;
				case 0x8002: switch(prod) {
						case 0x1: sProduct = "Plug-In Dimmer Module";
							break;
					}
					break;
			}
			break;
		case 0x86: sManufacturer = "Aeon Labs";
			switch (type) {
				case 0x1: switch(prod) {
						case 0x3: sProduct = "Minimote";
							break;
					}
					break;
				case 0x2: switch(prod) {
						case 0x9: sProduct = "Home Energy Meter";
							zwConfigurationSet(nodeid,101,2,4); // send multilevel sensor report for group 1
							zwConfigurationSet(nodeid,111,120,4); // every 2 minutes
							zwConfigurationSet(nodeid,102,1,4); // send battery report for group 2
							zwConfigurationSet(nodeid,112,86400,4); // every day
							zwAssociationSet(nodeid, 2, ournodeid);
							break;
					}
					break;
			}
			break;
		case 0x109: sManufacturer = "Vision";
			switch (type) {
				case 0x2006: switch(prod) {
						case 0x620: sProduct = "ZM 1702 Door Lock";
							break;
					}
					break;
			}
			break;
		default: sManufacturer = "unknown";
	}
        DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Manufacturer Specific: Node %i is a %s %s",nodeid,sManufacturer.c_str(),sProduct.c_str());
	sManufacturer.append(" ");
	sManufacturer.append(sProduct);
	ZWNodeMapIt = ZWNodeMap.find(nodeid);

	int iPKDevice = 0;
	if (ZWNodeMapIt != ZWNodeMap.end()) {
		iPKDevice = (*ZWNodeMapIt).second->iPKDevice;
	}
	DCEcallback->SetManufacturerSpecificString(iPKDevice,sManufacturer);
}

void ZWApi::ZWApi::zwSecuritySchemeGet(int node_id) {
	char mybuf[1024];

        mybuf[0] = FUNC_ID_ZW_SEND_DATA;
        mybuf[1] = node_id;
        mybuf[2] = 3; // length of command
	mybuf[3] = COMMAND_CLASS_SECURITY;
	mybuf[4] = SECURITY_SCHEME_GET;
	mybuf[5]= 0; // security scheme 0
        mybuf[6] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
        sendFunction( mybuf , 7, REQUEST, 1);
}

void ZWApi::ZWApi::zwSecurityNonceGet(int node_id) {
	char mybuf[1024];

        mybuf[0] = FUNC_ID_ZW_SEND_DATA;
        mybuf[1] = node_id;
        mybuf[2] = 2; // length of command
	mybuf[3] = COMMAND_CLASS_SECURITY;
	mybuf[4] = SECURITY_NONCE_GET;
        mybuf[5] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
        sendFunction( mybuf , 6, REQUEST, 1);
}

void ZWApi::ZWApi::zwSecurityKeySet(int node_id) {
	char mybuf[1024];
	ZWNodeMapIt = ZWNodeMap.find(node_id);
	if (ZWNodeMapIt != ZWNodeMap.end()) {
		mybuf[0] = FUNC_ID_ZW_SEND_DATA;
		mybuf[1] = node_id;
		mybuf[2] = 30; // length of command
		mybuf[3] = COMMAND_CLASS_SECURITY;
		mybuf[4] = SECURITY_MESSAGE_ENCAPSULATION;
		mybuf[5] = (*ZWNodeMapIt).second->nonce[0];
		mybuf[6] = (*ZWNodeMapIt).second->nonce[0];
		mybuf[7] = (*ZWNodeMapIt).second->nonce[0];
		mybuf[8] = (*ZWNodeMapIt).second->nonce[0];
		mybuf[9] = (*ZWNodeMapIt).second->nonce[0];
		mybuf[10] = (*ZWNodeMapIt).second->nonce[0];
		mybuf[11] = (*ZWNodeMapIt).second->nonce[0];
		mybuf[12] = (*ZWNodeMapIt).second->nonce[0];
		mybuf[13] = 0; // no sequence
		mybuf[14] = COMMAND_CLASS_SECURITY;
		mybuf[15] = NETWORK_KEY_SET;
		mybuf[16] = 0x01;
		mybuf[17] = 0x02;
		mybuf[18] = 0x03;
		mybuf[19] = 0x04;
		mybuf[20] = 0x05;
		mybuf[21] = 0x06;
		mybuf[22] = 0x07;
		mybuf[23] = 0x08;
		mybuf[24] = 0; // receiver nonce id
		mybuf[25] = 0; // mac
		mybuf[26] = 0; // mac
		mybuf[27] = 0; // mac
		mybuf[28] = 0; // mac
		mybuf[29] = 0; // mac
		mybuf[30] = 0; // mac
		mybuf[31] = 0; // mac
		mybuf[32] = 0; // mac
		mybuf[33] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
		sendFunction( mybuf , 34, REQUEST, 1);
	}
}

void ZWApi::ZWApi::zwIsFailedNode(int node_id) {
	char mybuf[1024];

	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Removing failed node %i",node_id);
	mybuf[0] = FUNC_ID_ZW_IS_FAILED_NODE_ID;
	mybuf[1] = (unsigned char)node_id;
	sendFunction( mybuf , 2, REQUEST, 1);

}

