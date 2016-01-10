/***************************************************************************
 *   Copyright (C) 2005 by igor                                            *
 *   igor@dexx                                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "message.h"
#include <string.h>
#include "busconnector.h"
#include "timetracker.h"

#include "DCE/Logger.h"

#define MAX_FRAME_LENGTH	30

// Definitions of EIB messages
#define EIB_FIRST_TYPE_BYTE 0
#define EIB_FIRST_LENGTH_BYTE 1
#define EIB_SECOND_LENGTH_BYTE 2
#define EIB_SECOND_TYPE_BYTE 3
#define EIB_CONTROL_FIELD_BYTE 4
#define EIB_MESSAGE_CODE_BYTE 5

#define EIB_STOP_BYTE_VALUE 0x16
#define EIB_TYPE_BYTE_VALUE 0x68

#define EIB_SIZE_OF_PEI_HEADER 4
#define EIB_SIZE_OF_PEI_TAIL 2
#define EIB_SIZE_OF_PEI_FRAME 6
#define EIB_SIZE_OF_PEI_CONTROL_FIELD 1
#define EIB_SIZE_OF_LINK_LAYER_FIELD 3

// only valid for received messages
#define EIB_STATUS_BYTE 6
#define EIB_FIRST_TIMESTAMP_BYTE 7
#define EIB_SECOND_TIMESTAMP_BYTE 8
#define EIB_RECEIVED_MESSAGE_CODE_VALUE 0x2B

// only valid for sent messages
#define EIB_SENT_APCI_BYTE 13
#define EIB_SENT_MESSAGE_CODE_VALUE 0x11
#define EIB_SENT_FIRST_SOURCE_ADDRESS_BYTE 7
#define EIB_SENT_SOURCE_ADDRESS_SHORT_VALUE 0x0000



using namespace DCE;

namespace EIBBUS {

std::string 
FormatHexBuffer(const char* buff, unsigned int size);

int 
Message::SendBuffer(BusConnector *pbusconn, 
		const unsigned char* msg, unsigned int length)
{
	TimeTracker tk;

	tk.Start();
	pbusconn->Send((unsigned char*)msg, length);
	tk.Stop();
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Buffer <%s> sent in %.5f seconds", 
			FormatHexBuffer((const char*)msg, length).c_str(), tk.getTotalPeriodInSecs());
		
	pbusconn->incFrameBit();
	return 0;
}

int 
Message::RecvBuffer(BusConnector *pbusconn, 
		unsigned char* msg, unsigned int length)
{
	unsigned int nread, totalread = 0;
	while(totalread < length) {
		nread = pbusconn->Recv(msg + totalread, length - totalread, 100);
		if(nread <= 0) {
			return totalread;
		}
		totalread += nread;
	}
	return totalread;
}
	
int 
Message::UndoRecvBuffer(BusConnector *pbusconn, 
		const unsigned char* msg, unsigned int length)
{
	return pbusconn->UndoRecv(msg, length);
}

int
PeiMessage::SendPeiBuffer(BusConnector *pbusconn, 
		unsigned char type, unsigned char code, const unsigned char* msg, unsigned int length) 
{
	unsigned char peibuff[MAX_FRAME_LENGTH];
	
	peibuff[EIB_FIRST_TYPE_BYTE] = type;
	peibuff[EIB_SECOND_TYPE_BYTE] = type;
	
	peibuff[EIB_MESSAGE_CODE_BYTE] = code;
	
	peibuff[EIB_FIRST_LENGTH_BYTE] =
		length + EIB_SIZE_OF_PEI_CONTROL_FIELD + 1;
	peibuff[EIB_SECOND_LENGTH_BYTE] =
		length + EIB_SIZE_OF_PEI_CONTROL_FIELD + 1;

	// Set ControlField
	if(pbusconn->getFrameBit()) {
		peibuff[EIB_CONTROL_FIELD_BYTE]= 0x73;
	} else {
		peibuff[EIB_CONTROL_FIELD_BYTE]= 0x53;
	}

	memcpy(&peibuff[EIB_STATUS_BYTE], msg, length);
	
	// Calculate PEI-CHKSUM,
	unsigned char chksum = peibuff[EIB_CONTROL_FIELD_BYTE];
	
	int laenge = ((int)peibuff[EIB_FIRST_LENGTH_BYTE])
						+ EIB_SIZE_OF_PEI_HEADER;
	for ( int i = EIB_MESSAGE_CODE_BYTE ; i < laenge ; i++) {
			chksum += peibuff[i];
	}
	// Set PEI-CHKSUM,
	peibuff[laenge] = chksum;
	
	// Set Stop Byte
	peibuff[laenge + 1] = EIB_STOP_BYTE_VALUE;
	return SendBuffer(pbusconn, peibuff, sizeof(peibuff));
}

int 
PeiMessage::ReceivePeiBuffer(BusConnector *pbusconn, 
		unsigned char type, unsigned char& code, unsigned char* msg, unsigned int& length) 
{
	int totalread = 0;
	unsigned char peibuff[MAX_FRAME_LENGTH];
	
	/*receive header*/
	int nread = RecvBuffer(pbusconn, &peibuff[0], 1);
	if(nread < 1) {
		return RECV_INVALID;
 	}
	totalread += nread;

	if(type != peibuff[0]) {
	} else {
		nread = RecvBuffer(pbusconn, &peibuff[1], EIB_SIZE_OF_PEI_HEADER - 1);
		if(nread >= 1) {
			totalread += nread;
		} 
		if(nread < EIB_SIZE_OF_PEI_HEADER - 1) {
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Could not receive complete Message Header, only %d bytes.", nread);
		} else {	
			/*get message length including TAIL length*/
			if(peibuff[EIB_FIRST_LENGTH_BYTE] != peibuff[EIB_SECOND_LENGTH_BYTE]) {
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "Length fields set incorrectly");
			} else {
				int mlength = peibuff[EIB_FIRST_LENGTH_BYTE];
				nread = RecvBuffer(pbusconn, &peibuff[EIB_SIZE_OF_PEI_HEADER], mlength + EIB_SIZE_OF_PEI_TAIL);
				if(nread >= 1) {
					totalread += nread;
				} 
				if(nread < (int)mlength + EIB_SIZE_OF_PEI_TAIL) {
					LoggerWrapper::GetInstance()->Write(LV_WARNING, "Could not receive message body, expected: %d but received: %d", 
																		mlength + EIB_SIZE_OF_PEI_TAIL, nread);
				
				} else {
					/* check tail */	
					unsigned char chksum = peibuff[EIB_CONTROL_FIELD_BYTE];
					
					if(mlength > EIB_SIZE_OF_LINK_LAYER_FIELD + EIB_SIZE_OF_PEI_CONTROL_FIELD + 2) {
						int dlength = mlength - EIB_SIZE_OF_PEI_CONTROL_FIELD;
						
						/* calculate checksum */
						for ( int i = 0 ; i < dlength; i++)	{
							chksum += peibuff[ i + EIB_MESSAGE_CODE_BYTE];
						}
						
						if (( chksum != peibuff[ dlength + EIB_MESSAGE_CODE_BYTE] )
							|| ( EIB_STOP_BYTE_VALUE != peibuff[dlength + EIB_MESSAGE_CODE_BYTE+1] ))	{
							LoggerWrapper::GetInstance()->Write(LV_WARNING, "Invalid CHECKSUM");
							return RECV_INVALID;
						};
						
						code = peibuff[EIB_MESSAGE_CODE_BYTE];
						
						/*copy msg*/
						unsigned int ulength = mlength - EIB_SIZE_OF_LINK_LAYER_FIELD - EIB_SIZE_OF_PEI_CONTROL_FIELD - 2;
						if(ulength <= length) {
							length = ulength;
							memcpy(msg, &peibuff[EIB_STATUS_BYTE + EIB_SIZE_OF_LINK_LAYER_FIELD], length);
							return RECV_OK;
						} else {
							return RECV_INVALID;
						}
					} else {
						length = 0;
						return RECV_OK;
					}
				}
			}
		}
	}
	
	if(totalread > 0) {
		UndoRecvBuffer(pbusconn, peibuff, totalread);
	}
	return RECV_UNKNOWN;
}

std::string 
FormatHexBuffer(const char* buff, unsigned int size) {
	std::string logstr;
	if(size <= 0) {
		logstr = "EMPTY BUFFER";
	} else {
		char hxbuff[5];
		for(unsigned int i = 0; i < size; i++) {
			sprintf(hxbuff, "0x%0hhx", buff[i]);
			logstr += ((i > 0) ? " " : "");
			logstr += hxbuff;
		}
	}
	return logstr;
}

};
