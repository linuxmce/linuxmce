/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//
// C++ Implementation: telegrammessage
//
// Description: 
//
//
// Author:  <>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "telegrammessage.h"
#include "busconnector.h"

#include "DCE/Logger.h"

/* helper functions */
/*static unsigned getunsignedfromint( int value );
static int getintfromeis5( unsigned eis5 );*/
static unsigned short getgroupaddrfromstr(const char *gadstr);
static bool getstrfromgroupaddr(unsigned short gaddr, char *gadstr, int length);
static unsigned short swapushort(unsigned short toswap);

#define MAX_GROUPADDR_STR_LEN	11

/* TelegramMessage */

using namespace DCE;

namespace EIBBUS {

TelegramMessage::TelegramMessage()
	: acttype_(WRITE), length_(0), shortusrdata_(0)
{
}

TelegramMessage::~TelegramMessage()
{
}
	
TelegramMessage::TelegramMessage(const TelegramMessage& tlmsg) {
	*this = tlmsg;
}

bool 
TelegramMessage::operator ==(const TelegramMessage& tlmsg) {
	if(acttype_ == tlmsg.acttype_ && gaddr_ == tlmsg.gaddr_ && length_ == tlmsg.length_ &&
		shortusrdata_ == tlmsg.shortusrdata_) {
			if(tlmsg.length_ > 0) {
				return  !memcmp(usrdata_, tlmsg.usrdata_, tlmsg.length_);
			} else {
				return true;
			}
	} else {
			return false;
	}
}

TelegramMessage& 
TelegramMessage::operator=(const TelegramMessage& tlmsg) {
	acttype_ = tlmsg.acttype_;
	gaddr_ = tlmsg.gaddr_;
	length_ = tlmsg.length_;
	shortusrdata_ = tlmsg.shortusrdata_;
	if(tlmsg.length_ > 0) {
		memcpy(usrdata_, tlmsg.usrdata_, tlmsg.length_);
	}
	
	return *this;
}

int 
TelegramMessage::Recv(BusConnector *pbusconn) {
	int ret = 0;
	
	unsigned char code;
	unsigned char inmsg[MAX_USERDATA_LENGTH];
	unsigned int userlength = MAX_USERDATA_LENGTH;
	if((ret = ReceivePeiBuffer(pbusconn, 0x68, code, inmsg, userlength)) != 0) {
		return ret;
	}
	
	if(code != 0x2B || userlength < 8) {
		return RECV_INVALID;
	}
	
	length_ = userlength - 8;

	/*get destination grou[ address*/
	unsigned short groupaddr = *((unsigned short*)&inmsg[3]);
	char gaddrstr[MAX_GROUPADDR_STR_LEN];
	if(!getstrfromgroupaddr(groupaddr, gaddrstr, sizeof(gaddrstr))) {
		return RECV_INVALID;
	}
	
	setGroupAddress(gaddrstr);
	
	unsigned char acpishortuserdata = inmsg	[7];
	setActionType((ACTIONTYPE)(acpishortuserdata >> 6));
	
	switch(length_) {
	case 0:
		shortusrdata_ = acpishortuserdata & 0x3F;
		break;
	default:
		memcpy(usrdata_, &inmsg[8], length_);
		break;
	}
	
	return RECV_OK;
}

int 
TelegramMessage::Send(BusConnector *pbusconn) {
	if(getActionType() != WRITE) {
		shortusrdata_ = 0;
		length_ = 0;
	}	
		
	unsigned char routingcounterlength = (unsigned short)(length_ + 1) + 0xE0;
	unsigned char acpishortuserdata = (unsigned char)(getActionType() << 6) 
											+ ((length_ == 0) ? shortusrdata_ : 0);
	
	/*control field*/
	int controlfield = 0x9C;
	
	/*group address*/	
	unsigned short groupaddr = getgroupaddrfromstr(gaddr_.c_str());
	

	char outmsg[MAX_USERDATA_LENGTH];
	unsigned outmsglength = 0;
	
	outmsg[0] = controlfield;
	outmsg[1] = outmsg[2] = 0;
	*((unsigned short*)&outmsg[3]) = groupaddr;
	outmsg[5] = routingcounterlength;
	outmsg[6] = 0;
	outmsg[7] = acpishortuserdata;
	
	unsigned char userlength = routingcounterlength & 0x0F; 
	if(userlength > 1) {
		memcpy(&outmsg[8], usrdata_, userlength - 1);
	}
	outmsglength = 8 + userlength - 1;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sending telegram with Group Address: %d(%s), Length: %d, Short User Data: %d", 
														groupaddr, gaddr_.c_str(), length_, shortusrdata_);

													
	return SendPeiBuffer(pbusconn, 0x68, 0x11, (unsigned char*)outmsg, outmsglength);
}

};

/* helper functions */
/*unsigned getunsignedfromint( int value )
{
    if( value >= 0 )
        return value;

    unsigned ret = *((unsigned *)(&value));

    return ret;
}

int getintfromeis5( unsigned eis5 )
{
    eis5 = eis5 & 0xffff; // only 2 bytes
    int sign = (eis5 & 0x8000) ? 1 : 0;  // last bit
    int exponent = (eis5 & 0x7800) >> 11;

    int pow = 1;
    for( int i = 0; i < exponent; i++ )
        pow *= 2;

    int mantissa = eis5 & 0x07ff;
    if( sign ) // two's complement
    {
        mantissa ^= 0x7ff;
        mantissa += 1;
        mantissa &= 0x07ff;
    }

    int result = pow * mantissa;

    if( sign )
        return (-result);
    else
        return result;
}
*/

unsigned short 
getgroupaddrfromstr(const char *gadstr)
{
    char *pFirstSlash, *pSecondSlash;
    char gad[128];
    unsigned short mainGad, middleGad, subGad, middleSubGad;

    if( !gadstr )
        return 0xffff;

    strncpy( gad, gadstr, 127 );
    gad[127] = 0;

    if( !(pFirstSlash = strchr( gad, '/' )) )
        return 0xffff;
    *pFirstSlash = 0;

    if( !(pSecondSlash = strchr( pFirstSlash + 1, '/' )) )
        return 0xffff;
    *pSecondSlash = 0;

    mainGad = (unsigned short)atoi( gad );
    middleGad = (unsigned short)atoi(pFirstSlash + 1);
    subGad = (unsigned short)atoi( pSecondSlash + 1 );

    if( (mainGad > 15) || (subGad > 0x7ff ) )
        return 0xfff;

    middleSubGad = subGad + (middleGad << 8);
    return swapushort( ((subGad + (middleGad << 8)) & 0x7ff) | ( mainGad << 11 ) );
};

bool 
getstrfromgroupaddr(unsigned short gaddr, char *gadstr, int length) {
	if(length < MAX_GROUPADDR_STR_LEN) {
		return false;
	}
	
	unsigned short mainGad, subGad, middleGad;
	if(!gadstr) {
        return false;
	}

    unsigned short gadSwap = swapushort( gaddr );
	mainGad = gadSwap >> 11;
    subGad = gadSwap & 0x07ff;
    middleGad = subGad >> 8;
    subGad = subGad & 0xff;

    sprintf( gadstr, "%d/%d/%d", mainGad, middleGad, subGad );
	return true;
}

unsigned short 
swapushort(unsigned short toswap)
{
    unsigned char res[2], *ptoswap = (unsigned char *)(&toswap);

    res[0] = ptoswap[1];
    res[1] = ptoswap[0];

    return  (*((unsigned short *)res));
}
