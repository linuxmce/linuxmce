/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
/**
 *
 * @file BD_HaveNothing.h
 * @brief header file for the HaveNothing command (answer) classs
 * @author
 * @todo notcommented
 *
 */


#ifndef BDHAVENOTHINGH
#define BDHAVENOTHINGH

#include "BD/BDCommand.h"
//#include "VIPShared/PlutoPhoneCommands.h"

#ifndef SYMBIAN
#include "BD/BDCommandProcessor.h"
#else
#include "BD/BDCommandProcessor_Symbian_Base.h"
#endif


/**
 * @brief the response to WhatDoYouHave when there are no commands
 */
class BD_HaveNothing : public BDCommand 
{

public:

	/**
	 * @brief empty constructor (nothing to be done)
	 */
	BD_HaveNothing()  {}
	
	/**
	 * @brief just returns the BD_CP_HAVE_NOTHING defined value
	 */
	virtual unsigned long ID() { return BD_CP_HAVE_NOTHING; }
	
	/**
	 * @brief nothing to be done
	 */
	virtual bool ProcessCommand( class BDCommandProcessor *pProcessor ) { return true; }

	virtual const char *Description() { return "Have nothing"; };
};


#endif
