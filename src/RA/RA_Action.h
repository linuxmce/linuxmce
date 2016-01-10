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
 * @file RA_Action.h
 * @brief header file for the RA_Action class
 * @author @todo addauthor
 * @todo notcommented
 *
 */

#ifndef RA_ACTION_H
#define RA_ACTION_H

#include "SerializeClass/SerializeClass.h"

class RA_Action : public SerializeClass
{
protected:

    uint32_t m_dwActionSize; /** <the size of the action */
    uint32_t m_dwActionChecksum; /** <the checksum of the action */
    char *m_pcAction; /** <the action data */

public:

    /**
     * @brief basic constructor
     * the one initiating the action calls this constructor, and will call ConvertActionToBinary and Send it
     */
    RA_Action();

    /**
     * @brief virual destructor
     */
	virtual ~RA_Action() {};

    /**
     * @brief Create an action class from a binary block of data
     * the one receiving the action calls this one
     * received a binary action, call ProcessAction
     * @param dwSize the size of the binary data
     * @param pcData the data
     */
    void CreateAction( uint32_t dwSize,const char *pcData); // Received a binary action, call ProcessAction

	/**
     * @brief virtual convertor to binary
     */
    virtual void ConvertActionToBinary();

    /**
     * @brief pure virtual that, in the inheriting classes, will process the action
     */
    virtual void ProcessAction(class RA_Request *pRequest,class RA_Processor *pRA_Processor)=0;

    /**
     * @return private member
     */
    uint32_t ActionSize() { return m_dwActionSize; }

    /**
     * @return private member
     */
    uint32_t ActionChecksum() { return m_dwActionChecksum; }

    /**
     * @return constant private member
     */
    const char *Action() { return m_pcAction; }

    /**
     * @brief pure virtual method... should return the ID of the action in inheriting classes
     */
    virtual uint32_t ID()=0;
};

#endif
