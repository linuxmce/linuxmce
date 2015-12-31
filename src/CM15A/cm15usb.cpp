/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

     Author: Nosilla99
     Name  : CM15A
     Descr : Active Home CM15A USB X10 PC Interface and RF transceiver
             DCE Device Handler
     Date  : 12.11.2009
     File  : CM15A/cm15usb.cpp
*/

#include "cm15usb.h"

#include <time.h>

#include "DCE/Logger.h"
#include "DCE/DeviceData_Impl.h"
#include "DCE/Message.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "PlutoUtils/StringUtils.h"


using namespace DCE;

namespace CM15ADEV {

CM15usb::CM15usb() {
}

CM15usb::~CM15usb() {
        usb_release_interface(device_handle,0);
        usb_close(device_handle); 
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Closed device CM15A.");
}

int
CM15usb::Locate() {
        usb_init();
        usb_set_debug(0); 
        device_handle = 0;
        char sbuff[8];

        status = usb_find_busses();
        if (status > 0)
	   LoggerWrapper::GetInstance()->Write(LV_STATUS, "Found %d USB buses.",status);
        else
           LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "No USB buses present !!!.");

        usb_find_devices();
        for (bus = usb_busses; bus; bus = bus->next) {
            for (dev = bus->devices; dev; dev = dev->next) {
                if (dev->descriptor.idVendor == 0x0bc7 && dev->descriptor.idProduct == 0x0001) {
                      device_handle = usb_open(dev);
	              LoggerWrapper::GetInstance()->Write(LV_STATUS, "CM15A found @ Addr %s.", dev->filename);
                      usb_set_configuration(device_handle,1);
                      usb_claim_interface(device_handle,0);
                      usb_clear_halt(device_handle, EP_OUT);
                      usb_clear_halt(device_handle, EP_IN);
                      usb_bulk_read(device_handle,EP_IN,(char *)sbuff,8,USB_TIMEOUT);
                      usleep(500);
                      SendClock();
                }
            }     
        }
        if ( device_handle == 0 ) {
	   LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "No CM15A found on this Computer.");
           return false;
        }
        return true;
}

int 
CM15usb::SendRequest(const Message* pMesg) {
	int ret = 0; // ok
	
	mq_.Lock();
	if(msgqueue_.size() < MAX_QUEUE_SIZE) {
		msgqueue_.push_back(*pMesg);
		ret = -1;
	}
	mq_.Unlock();
	return 0;
}

int 
CM15usb::SendPacket(unsigned char highByte, unsigned char lowByte)
{
	unsigned char sendbuff[2] =  { highByte, lowByte };

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sending packet with HighByte=%x, LowByte=%x.", highByte, lowByte);
										
	if (usb_bulk_write(device_handle,EP_OUT,(char *)sendbuff,2,USB_TIMEOUT) == 2) {
	    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Packet Sent to CM15A.");
        }
        else {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to Send Packet to CM15A.");
            return false;
        }

        usb_bulk_read(device_handle,EP_IN,(char *)sendbuff,1,USB_TIMEOUT);

        if (sendbuff[0] == CM15A_ACK)
	    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Received expected ACK from CM15A.");
        else
	    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Whoops did not get ACK from CM15A.");
            
	return true;
}			

int 
CM15usb::SendClock() {
	char sbuff[8];
	time_t ttime = time((long *) NULL);
	
	struct tm *tp;
	tp = localtime(&ttime);

	sbuff[0] = CM15A_SET_TIME;             /* CM15A timer download code */
	sbuff[1] = tp->tm_sec ;
	sbuff[2] = tp->tm_min + (((tp->tm_hour) %2) * 60 ) ;  /* minutes 0 - 119 */
	sbuff[3] = tp->tm_hour / 2 ;                 /* hour / 2         0 - 11 */
	sbuff[4] = tp->tm_yday % 256 ;               /* mantisa of julian date */
	sbuff[5] = ((tp->tm_yday / 256 ) << 7);      /* radix of julian date */
	sbuff[5] |= (0x01 << (tp->tm_wday)); /* bits 0-6 =  single bit mask day of week ( smtwtfs ) */
	sbuff[6] = 0x61; /* monitor house codes A */
        sbuff[7] = 0x00;
        if (usb_bulk_write(device_handle,EP_OUT,sbuff,7,USB_TIMEOUT) == 7)
	    LoggerWrapper::GetInstance()->Write(LV_STATUS, "CM15A Clock Set");
        else
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to set CM15A Clock.");
	return true;
}

const char *
CM15usb::Byte2Binary(int x)
{
        static char binary[9];
        int z;

        binary[0] = 0;
        for (z = 256; z > 0; z >>= 1) {
             strcat(binary, ((x & z) == z) ? "1" : "0");
        }

        return binary;
}

int 
CM15usb::GetStatus() {
	char sbuff[8],sbuff_1[8];
        char hours;
        
        sbuff[0] = CM15A_STATUS;
        if (usb_bulk_write(device_handle,EP_OUT,sbuff,1,USB_TIMEOUT) == 1)
	    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Requested CM15A Status.");
        else
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to request CM15A status.");
        usleep(500);
	if (usb_bulk_read(device_handle,EP_IN,(char *)sbuff, CM15A_READ_SIZE, USB_TIMEOUT) == CM15A_READ_SIZE) {
           usleep(500);
	   usb_bulk_read(device_handle,EP_IN,(char *)sbuff_1, CM15A_READ_SIZE, USB_TIMEOUT);
           hours = sbuff[3] > 59;
           if (sbuff[3] > 59 ) sbuff[3] = sbuff[3] - 60;
	   LoggerWrapper::GetInstance()->Write(LV_STATUS, "CM15A Device Status Report.");
	   LoggerWrapper::GetInstance()->Write(LV_STATUS, "==================================.");
	   LoggerWrapper::GetInstance()->Write(LV_STATUS, "Battery Timer : %02X%02X", sbuff[0],sbuff[1]);
	   LoggerWrapper::GetInstance()->Write(LV_STATUS, "Time          : %02d:%02d:%02d",(sbuff[4]*2)+hours,sbuff[3],sbuff[2]);
	   LoggerWrapper::GetInstance()->Write(LV_STATUS, "Day Number    : %003d",sbuff[5]+((sbuff[6] & 0x80)<<1));
	   LoggerWrapper::GetInstance()->Write(LV_STATUS, "Day of Week   : %s",Byte2Binary(sbuff[6] & 0x7f));
	   LoggerWrapper::GetInstance()->Write(LV_STATUS, "Firmware Rev  : %02d",sbuff[7] & 0x0f);
	   LoggerWrapper::GetInstance()->Write(LV_STATUS, "Monitored HC  : %s",Message::getHouseCode(sbuff[7]));
	   LoggerWrapper::GetInstance()->Write(LV_STATUS, "Monitored Devs: %s%s",Byte2Binary(sbuff_1[0]),Byte2Binary(sbuff_1[1]));
	   LoggerWrapper::GetInstance()->Write(LV_STATUS, "On/Off Status : %s%s",Byte2Binary(sbuff_1[2]),Byte2Binary(sbuff_1[3]));
	   LoggerWrapper::GetInstance()->Write(LV_STATUS, "Dim Status    : %s%s",Byte2Binary(sbuff_1[4]),Byte2Binary(sbuff_1[5]));
	   LoggerWrapper::GetInstance()->Write(LV_STATUS, "==================================.");
        } else
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to get CM15A status response.");
	return true;
}

int 
CM15usb::SendAddress(const Message* pMesg) 
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sending address with HouseCode=%d, DeviceCode=%d.",
                                                        pMesg->getHouseCode(), pMesg->getDeviceCode());
	return SendPacket(X10_ADDRESS, pMesg->getHouseCode() << 4 | pMesg->getDeviceCode());
}

int
CM15usb::SendFunction(const Message* pMesg) 
{
	unsigned char dim = (unsigned char)(pMesg->getDimmLevel() * X10_MAX_DIM_LEVEL * 1.0 / 100);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sending function with Code: %d.", 
							pMesg->getFunctionCode());
	if(dim > 0) {
	    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Using Dim Level: %d.", dim);
	}
	
	return SendPacket((dim << 3) | X10_FUNCTION,pMesg->getHouseCode() << 4 | pMesg->getFunctionCode());
	
}

int 
CM15usb::ReadIncoming() {

        unsigned char recvbuff[255];
        char hexbuff[255];
        int bytecnt, index;
	list<std::string> used_addresses;
	string recv_msg="";

//	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Checking for incoming data from the CM15A.");

	bytecnt = usb_bulk_read(device_handle,EP_IN,(char *)recvbuff, CM15A_READ_SIZE, USB_TIMEOUT);
        hexbuff[0] = 0;
	if(bytecnt > 0) { 
                for (index=0;index<bytecnt;index++) {
                     sprintf(hexbuff+strlen(hexbuff),"%02X ",recvbuff[index] & 0xFF);
                }
                switch (recvbuff[0]) {
                    case CM15A_ACK : LoggerWrapper::GetInstance()->Write(LV_STATUS, "Received ACK <%s>",hexbuff);
                                     break;

                      case CM15A_MT: LoggerWrapper::GetInstance()->Write(LV_STATUS, "Received <%s>: Macro Timer",hexbuff);
                                     break;

                     case CM15A_PLC: LoggerWrapper::GetInstance()->Write(LV_STATUS, "Received PLC data <%s>",hexbuff);
				     for (index=3;index<(recvbuff[1]+2);index++) {
					 gettimeofday(&timestampNow, NULL);						
					 if ((recvbuff[2] & (1 << (index-3))) == 0) { //an address
					    char* addr = Message::getAddress(recvbuff[index]);
					    used_addresses.push_back(addr);
					    if(recv_msg.length()>0) {
					        recv_msg+=",";
					    }
					    recv_msg+=addr;
					    //get timestamp for computing the CM11A_DIM_TIMEOUT
					    gettimeofday(&timestampLast, NULL);	
					    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Address received. Resetting X10_FUNC_TIMEOUT timer.");						
                                         }
                                         else if((timestampNow.tv_sec - timestampLast.tv_sec)*1000 + (timestampNow.tv_usec - timestampLast.tv_usec)/1000 < X10_FUNC_TIMEOUT)
					 { 
					    //This is a function and the X10_FUNC_TIMEOUT has not been reached
					    char func=recvbuff[index] & 0x0F;
					    char data=0;
					    std::string message= Message::getHouseCode(recvbuff[index]);
								
					    if (used_addresses.size() == 0) {
					        used_addresses = last_used_addresses;
					        LoggerWrapper::GetInstance()->Write(LV_STATUS, "No address codes found. Using last used addresses.");
					    }

					    switch(func) {
						case X10_FUNC_ALL_U_OFF : message+=" ALL UNITS OFF"; break;
						case X10_FUNC_ALL_L_ON  : message+=" ALL LIGHTS ON"; break;
						case X10_FUNC_ON        : message+=" ON"; break;
						case X10_FUNC_OFF       : message+=" OFF"; break;
						case X10_FUNC_DIM       : message+=" DIM"; 
									  index++;
									  data=(recvbuff[index]*100)/210; 
									  break;

						case X10_FUNC_BRIGHT    : message+=" BRIGHT"; 
									  index++;
									  data=(recvbuff[index]*100)/210; 
									  break;

						case X10_FUNC_ALL_L_OFF : message+=" ALL LIGHTS OFF"; break;
						case X10_FUNC_EXT       : message+=" EXTENDED"; break;
						case X10_FUNC_HREQ      : message+=" HAIL REQUEST"; break;
						case X10_FUNC_HACK      : message+=" HAIL ACK"; break;
						case X10_FUNC_PSD1      : message+=" PRESET DIM 1"; break;
						case X10_FUNC_PSD2      : message+=" PRESET DIM 2"; break;
						case X10_FUNC_EXTDT     : message+=" EXTENDED DATA"; break;
						case X10_FUNC_STATON    : message+=" STATUS ON"; break;
						case X10_FUNC_STATOFF   : message+=" STATUS OFF"; break;
						case X10_FUNC_STATREQ   : message+=" STATUS REQUEST"; break;
                                                              
                                                default                 : message+=" UNKNOWN FUNCTION"; break;
					    }

					    for(std::list<std::string>::iterator it = used_addresses.begin(); it != used_addresses.end(); it++) {
						DeviceData_Impl *child = kids[inverse_device_map[*it]];
								
						LoggerWrapper::GetInstance()->Write(LV_STATUS, "Looking for device status for %s found %d", (*it).c_str(), device_status.find(*it) != device_status.end() ? 1 : 0);
						if (child->m_dwPK_DeviceCategory == DEVICECATEGORY_Lighting_Device_CONST) {//device_status.find(*it) != device_status.end()) {
						   switch(func) {
							//Need to add status updates for all lights on/off
							case X10_FUNC_ON      	: device_status[*it]=100;
										  pMessageOut = new DCE::Message(child->m_dwPK_Device,
                                                                                                         DEVICETEMPLATE_Lighting_Plugin_CONST,
                                                                                                         PRIORITY_NORMAL, MESSAGETYPE_EVENT, 
                                                                                                         EVENT_Device_OnOff_CONST, 0);
										  pMessageOut->m_mapParameters[EVENTPARAMETER_OnOff_CONST]="1";
										  break;

							case X10_FUNC_OFF     	: device_status[*it]=0; 
										  pMessageOut = new DCE::Message(child->m_dwPK_Device,
                                                                                                         DEVICETEMPLATE_Lighting_Plugin_CONST,
                                                                                                         PRIORITY_NORMAL, MESSAGETYPE_EVENT, 
                                                                                                         EVENT_Device_OnOff_CONST, 0);
										  pMessageOut->m_mapParameters[EVENTPARAMETER_OnOff_CONST]="0";	
										  break;

							case X10_FUNC_ALL_U_OFF : LoggerWrapper::GetInstance()->Write(LV_STATUS, "All Units Off function not yet implemented.");
										  break;

							case X10_FUNC_ALL_L_ON	: LoggerWrapper::GetInstance()->Write(LV_STATUS, "All Lights On function not yet implemented.");
										  break;

							case X10_FUNC_ALL_L_OFF	: LoggerWrapper::GetInstance()->Write(LV_STATUS, "All Lights Off function not yet implemented.");
										  break;

							case X10_FUNC_DIM       : device_status[*it]-=data; 
										  if (device_status[*it] < 0){
									  	      device_status[*it] = 0;
										  }
										  pMessageOut = new DCE::Message(child->m_dwPK_Device, 
                                                                                                         DEVICETEMPLATE_Lighting_Plugin_CONST,
                                                                                                         PRIORITY_NORMAL, MESSAGETYPE_EVENT, 
                                                                                                         EVENT_State_Changed_CONST, 0);
										  pMessageOut->m_mapParameters[EVENTPARAMETER_State_CONST]=StringUtils::itos(device_status[*it]);
										  break;

							case X10_FUNC_BRIGHT    : device_status[*it]+=data; 
										  if (device_status[*it] >100) {
										      device_status[*it] = 100;
										  }
										  pMessageOut = new DCE::Message(child->m_dwPK_Device, 
                                                                                                         DEVICETEMPLATE_Lighting_Plugin_CONST,
                                                                                                         PRIORITY_NORMAL, MESSAGETYPE_EVENT, 
                                                                                                         EVENT_State_Changed_CONST, 0);
										  pMessageOut->m_mapParameters[EVENTPARAMETER_State_CONST]=StringUtils::itos(device_status[*it]);
										  break;

						   }
						   LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sending LIGHTING EVENT that %s is %s",(*it).c_str(),message.c_str());
						   real_cm15a->m_pEvent->SendMessage(pMessageOut);
							
						} else {
						   if (child->m_dwPK_DeviceCategory == DEVICECATEGORY_Security_Device_CONST) {
						       LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sending SECURITY EVENT that %s is %s",(*it).c_str(),message.c_str());
						       DCE::Message *pMessageOut=new DCE::Message(child->m_dwPK_Device,
                                                                                          DEVICETEMPLATE_VirtDev_Security_Plugin_CONST,
                                                                                          PRIORITY_NORMAL,MESSAGETYPE_EVENT,
                                                                                          EVENT_Sensor_Tripped_CONST,0);
						       pMessageOut->m_mapParameters[EVENTPARAMETER_Tripped_CONST]=(func==X10_FUNC_ON?"1":"0");
						       real_cm15a->m_pEvent->SendMessage(pMessageOut);
						   } else {
						       LoggerWrapper::GetInstance()->Write(LV_STATUS, "No devices known to use address %s. No EVENT sent.",(*it).c_str());
						   }
						}								
					    }//end for loop

					    if (recv_msg.length()>0) {
						recv_msg+=" ";
					    }
					    recv_msg+=message;
					 } else {
					    //CM11A_FUNC_TIMEOUT has been reached. The timeout timer will be resed on the next address received.
					    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Whoops Function timeout reached. It will be reset with the next address received.");
					    index++;
					 }//end elseif (is a function)
				     }//end for loop
				     last_used_addresses = used_addresses;
				     if (recv_msg != "") {
					 LoggerWrapper::GetInstance()->Write(LV_STATUS, "Data decoded into [%s]",recv_msg.c_str());
				     }
                                     break;

                      case CM15A_RF: LoggerWrapper::GetInstance()->Write(LV_STATUS, "Received RF Line data <%s>",hexbuff);
                                     LoggerWrapper::GetInstance()->Write(LV_STATUS, "Entering mind reading mode");
                                     LoggerWrapper::GetInstance()->Write(LV_STATUS, "I think you want %s",
                                                                         Message::getRfString((recvbuff[2]*0x100)+recvbuff[4]));
				     used_addresses.push_back(Message::getRfAddress((recvbuff[2]*0x100)+recvbuff[4]));
				     last_used_addresses = used_addresses;
				     gettimeofday(&timestampLast, NULL); // reset timestamp	
                                     break;

                      case CM15A_CR: LoggerWrapper::GetInstance()->Write(LV_STATUS, "Received Clock Reset required <%s>",hexbuff);
                                     SendClock();
                                     break;

                     case CM15A_MOR: LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "POTENTIAL CM15A LOCK PROBLEM <%s>",hexbuff);
                                     break;

                            default: LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Received <%s> UNKNOWN DATA.",hexbuff);
                                     break;
 
                } 
        }
           
	return 0;
}



void* CM15usb::_Run() {

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "CM15 USB thread started.");

	kids = real_cm15a->m_pData->m_vectDeviceData_Impl_Children;

	for(unsigned int kn=0; kn< kids.size(); kn ++)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Child device: #%d(%s)   Category:%d",kids[kn]->m_dwPK_Device,kids[kn]->m_mapParameters[12].c_str(),kids[kn]->m_dwPK_DeviceCategory);
		inverse_device_map[kids[kn]->m_mapParameters[12]]=kn;
		device_status[kids[kn]->m_mapParameters[12].c_str()]=0;
	}
	
	while(!isStopRequested()) {// will be changed with thread end request
		try {

			/* Should init and check usb here ??? using throw statement on error */

			// get the msg from queue
			mq_.Lock();
			if(msgqueue_.size() > 0) {
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Processing message for CM15A.");
				Message msg = msgqueue_.front();
				msgqueue_.pop_front();
				mq_.Unlock();

				if(SendAddress(&msg)) {
					LoggerWrapper::GetInstance()->Write(LV_STATUS, "Address sent successfully.");
				} else  {
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed sending address.");
					continue;
				}
				
                                usleep(500); //Delay required for CM15A to process address

				if(SendFunction(&msg)) {
					LoggerWrapper::GetInstance()->Write(LV_STATUS, "Function sent successfully.");
				} else  {
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed sending function.");
				}
			} else {
				mq_.Unlock();
				ReadIncoming();
				Sleep(CM15A_SLEEP);
			}
		} catch(...) {
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Houston we have a problem on CM15A USB bus.");
			Sleep(CM15A_ERROR_SLEEP);
		}
	}
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "CM15 USB thread ending.");
	return 0;
}

};
