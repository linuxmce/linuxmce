/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/** @file Generic_Serial_Device_MessageProcessor.h
Core ?
*/
//
// C++ Interface: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DCEGENERIC_SERIAL_DEVICE_MESSAGEPROCESSOR_H
#define DCEGENERIC_SERIAL_DEVICE_MESSAGEPROCESSOR_H

#include "MessageTranslation.h"
#include "DCE/Command_Impl.h"

/** @namespace DCE
The Data Commands and Events (DCE) namespace.
*/
namespace DCE {

/** @class Generic_Serial_Device_MessageProcessor
For ???
*/
class Generic_Serial_Device_MessageProcessor : public MessageProcessorBase {
public:

    /** Constructor.
    */
    Generic_Serial_Device_MessageProcessor();

    /** Destructor.
    */
    ~Generic_Serial_Device_MessageProcessor();

public:
	void setCommandImpl(Command_Impl* pcmdimpl) {
		pcmdimpl_ = pcmdimpl;
	}
	Command_Impl* getCommandImpl() {
		return pcmdimpl_;
	}

public:
	virtual bool Translate(Message* pmsg, MessageTimedQueue* pqueue);

	/*helper funcs*/
private:
	Command_Impl* FindTargetDevice(long devid);
	Command_Impl* FindTargetDevice(long devid, Command_Impl* pfromdev);


private:
	Command_Impl* pcmdimpl_; /**< pointer to Command? */
};

};

#endif
