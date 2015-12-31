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
 * @file BDCommand.h
 * @brief header file for the bidirectional command classs
 * @author
 * @todo notcommented
 *
 */



#ifndef BDCOMMAND_H
#define BDCOMMAND_H

#include "SerializeClass/SerializeClass.h"

//#ifdef SYMBIAN
	//class BDCommandProcessor {};
//#endif


/**
 * @brief class defining a command
 */
class BDCommand : public SerializeClass
{

protected:
	
	unsigned long m_dwCommandSize;  /** < the command size */
	unsigned long m_dwCommandChecksum; /** < checksum for the command */
	char *m_pcCommand; /** < the actual command data */

	unsigned long m_dwAckSize; /** < the acknowledge size */
	unsigned long m_dwAckChecksum; /** < the acknowledge checksum */
	char *m_pcAck; /** < the acknowledge actual data */

public:

	/**
	 * @brief constructor
	 */
	BDCommand();
	
	/** 
	 * @brief destructor
	 */
	virtual ~BDCommand() {};

	/**
	 * @brief returnes how much of the data block was processed
	 * similar to SerializeClass::CurrentSize
	 */
	unsigned long GetCommandOrAckSize() { return (unsigned long)(m_pcCurrentPosition - m_pcDataBlock); }
	
	/**
	 * @brief const acces to the member m_pcDataBlock
	 */
	const char *GetCommandOrAckData() { return m_pcDataBlock; }

	/**
	 * @brief calls SerializeClass::StartWriting() to begin manual serialization
	 * @see SerializeClass/SerializeClass.txt
	 */
	virtual void ConvertCommandToBinary();
	
	/**
	 * @brief calls SerializeClass::StartReading( ... ) with the same parameters
	 * @see SerializeClass/SerializeClass.txt
	 */
	virtual void ParseCommand( unsigned long dwSize, const char *pcData);
	
	/**
	 * @brief calls SerializeClass::StartWriting() to allocate the memory block and then writes the response "OKEY" to it
	 */
	virtual void ConvertAckToBinary();
	
	/**
	 * @brief calls SerializeClass::StartReading( ... ) and then reads the acknowledge response
	 */
	virtual void ParseAck( unsigned long dwSize, const char *pcData );

	// PURE VIRTUAL FUNCTIONS
	
	/**
	 * @brief pure virtual, should be overriden in inheriting classes
	 */
	virtual unsigned long ID()=0;
	
	/**
	 * @brief pure virtual, should be overriden in inheriting classes to describe what the command is
	 */
	virtual const char *Description()=0;

	/**
	 * @brief pure virtual, should be overriden in inheriting classes
	 */

	virtual bool ProcessCommand( class BDCommandProcessor *pProcessor )=0;
};

#endif
