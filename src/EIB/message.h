/***************************************************************************
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

/** @file message.h
Header for all the Message classes.
*/
#ifndef EIBMESSAGE_H
#define EIBMESSAGE_H

/** @namespace EIBBUS
For ???
*/
namespace EIBBUS {

class BusConnector;


/** @class Message
For Messages.
*/
class Message {
	friend class MessagePool;
public:
    Message() {};
    virtual ~Message() {};

public:
	/* recv error codes */
	enum {
		RECV_INVALID = -1,
		RECV_OK,
		RECV_UNKNOWN
	};

	virtual int Send(BusConnector *pbusconn)
	{ return RECV_INVALID; };
	virtual int Recv(BusConnector *pbusconn)
	{ return RECV_INVALID; };

protected:
	int SendBuffer(BusConnector *pbusconn,
		const unsigned char* msg, unsigned int length);
	int RecvBuffer(BusConnector *pbusconn,
		unsigned char* msg, unsigned int length);
	int UndoRecvBuffer(BusConnector *pbusconn,
		const unsigned char* msg, unsigned int length);
};


#define MAX_USERDATA_LENGTH		20

/** @class PeiMessage
For Messages.
*/
class PeiMessage : public Message {
protected:
	int SendPeiBuffer(BusConnector *pbusconn,
		unsigned char type, unsigned char code, const unsigned char* msg, unsigned int length);
	int ReceivePeiBuffer(BusConnector *pbusconn,
		unsigned char type, unsigned char& code, unsigned char* msg, unsigned int& length);
};

};

#endif
