/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/** @file message.h
 *@brief Handle message of CM11A.
 */
//
// C++ Interface: message
//
// Description:
//
//
// Author: igor <igor@dexx>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CM11ADEVMESSAGE_H
#define CM11ADEVMESSAGE_H

#include <string>

/**
@namespace CM11ADEV
The X10 CM11A Controller device namespace.
*/
namespace CM11ADEV {

/**
@class CM11ADEV::Message
This class is used to build a X10 message.
*/
class Message
{
public:
    /** Standard constructor */
    Message();
    /** Standard destructor */
    ~Message();

public:
    /** setAddress
    @param houseCode is the X10 house code (A..P) of the module.
    @param deviceCode is the device address (0..15) of the module.
    */
	void setAddress(unsigned char houseCode, unsigned char deviceCode) {
		houseCode_ = houseCode;
		deviceCode_ = deviceCode;
	}

    /** setAddress
    @param addressis a string containing the combined address
    The address is in the form of A1, B2,...
    */
	void setAddress(std::string address);

    /** getHouseCode
    @param x10addr is a byte contianing the House Code and Device Address
    @returns a string pointer of the house code.
    The house code is 'A'..'P'
    */
	static char* getHouseCode(unsigned char x10addr);

    /** getAddress
    @param x10addr is a byte contianing the House Code and Device Code
    @returns a string pointer of the device address.
    The Address is 00..15
    */
	static char* getAddress(unsigned char x10addr);

	unsigned char getHouseCode() const {
		return houseCode_; }
	unsigned char getDeviceCode() const {
		return deviceCode_; }

	void setFunctionCode(unsigned char functionCode) {
		functionCode_ = functionCode; }
	unsigned char getFunctionCode() const {
		return functionCode_; }

	void setDimmLevel(unsigned char dimmLevel) {
		dimmLevel_ = dimmLevel; }
	unsigned char getDimmLevel() const {
		return dimmLevel_; }


private:
    /** a byte containing the house code */
	unsigned char houseCode_;

    /** A byte containing the device code */
	unsigned char deviceCode_;

    /** A byte containing the function code */
	unsigned char functionCode_;

    /** a byte containing the dim level.
    The values are 0 for off up to 22 for full on.
    */
	unsigned char dimmLevel_;

private:
	static unsigned char s_MapHouseDeviceCodes[];
};

};

#endif
