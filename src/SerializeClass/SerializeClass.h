/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/** @file SerializeClass.h
 Header file for the SerializeClass
 */
#ifndef SERIALIZECLASS_H
#define SERIALIZECLASS_H

#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/MyStl.h"

#ifndef SYMBIAN
	#include "PlutoUtils/CommonIncludes.h"
	#include <iostream>
	#include <map>
	#include <list>
	using namespace std;
#else
	#include <f32file.h>
	#include <eikenv.h>
	#include <e32std.h>

	#define u_int64_t TInt64
	#define memcpy Mem::Copy
#endif

/** @todo check comment */
// Include this define to get debug information if the serialization is ever off
//#define DEBUG_SERIALIZATION


/**
 *
 * @see SerializeClass.txt before using !!!
 *
 */

class PlutoColor;
class PlutoPoint;
class PlutoSize;
class PlutoRectangle;

/** @class PlutoDataBlock
For serializing data.
@see SerializeClass.txt before using !!!
*/
class PlutoDataBlock
{
public:
	unsigned long m_dwSize;  char *m_pBlock;
	PlutoDataBlock(unsigned long dwSize, char *pBlock) { m_dwSize=dwSize; m_pBlock=pBlock; }
	PlutoDataBlock() { m_dwSize=0; m_pBlock=NULL; }
};

#define BLOCK_SIZE 50000

// serializing constants for the ellementary types
// values over 2000 will be reserved for user objects
#define SERIALIZE_DATA_TYPE_CHAR			1
#define SERIALIZE_DATA_TYPE_SHORT			2
#define SERIALIZE_DATA_TYPE_LONG			3
#define SERIALIZE_DATA_TYPE_UNSIGNED_LONG	4
#define SERIALIZE_DATA_TYPE_INT64			5
#define SERIALIZE_DATA_TYPE_STRING			6
#define SERIALIZE_DATA_TYPE_VECT_STRING		7
#define SERIALIZE_DATA_TYPE_INT_STRING		8
#define SERIALIZE_DATA_TYPE_VECT_INT		9
#define SERIALIZE_DATA_TYPE_STRING_STRING	10
#define SERIALIZE_DATA_TYPE_VECT_PAIR_INT	11
#define SERIALIZE_DATA_TYPE_FLOAT			12
#define SERIALIZE_DATA_TYPE_INT_INT			13
#define SERIALIZE_DATA_TYPE_DOUBLE			14
#define SERIALIZE_DATA_TYPE_MAP_STRING_PAIR_INT	15
#define SERIALIZE_DATA_TYPE_INT64_STRING		16

#define SERIALIZE_DATA_TYPE_COLOR			50
#define SERIALIZE_DATA_TYPE_POINT			51
#define SERIALIZE_DATA_TYPE_SIZE			52
#define SERIALIZE_DATA_TYPE_RECTANGLE		53
#define SERIALIZE_DATA_TYPE_BLOCK			54

/** @class ItemToSerialize
 Class containing info about the item to serialize
@see SerializeClass.txt before using !!!

Derive a class you want to serialize from this class, and then use it in a combination of 2 ways.
You can call StartReading/StartWriting and then call all the Read_/Write_
functions directly.  This will allocate a block of memory, pointed to by m_pcDataBlock, and CurrentSize() will always have the size.

Or, this can be done automatically.  Add the members that you want to have serialized when the object is created, either
by calling to AddToAutoSerializeList(...), or with the syntax:
StartSerializeList() + m_iMember1 + m_vectorMember2, etc.  (see operator +)

Call SerializeWrite and SerializeRead to serialize the data members.  Pass in a filename to have them save to the file.

If your derived class has objects that are not of a standard type that SerializeClass knows how to handle, you must create
your own methods to add them to the serialize list, like this a typedef list<DesignObj_Data *> DesignObj_DataList object:

#define SERIALIZE_DATA_TYPE_INT_DESIGNOBJCHILDLIST  1000    // Be sure this doesn't conflict with any other class in the tree
void AddToAutoSerializeList(DesignObj_DataList &i) { m_listItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_INT_DESIGNOBJCHILDLIST,(void *) &i)); }
DesignObj_Data &operator+ (DesignObjCommandList &i) { AddToAutoSerializeList(i); return (*this); }

Then, implement the UnknownSerialize method, like this:

@code
virtual bool UnknownSerialize(ItemToSerialize *pItem,bool bWriting,char *&pDataBlock,unsigned long &iAllocatedSize,char *&pCurrentPosition) s
{
    m_pcDataBlock=pDataBlock; m_dwAllocatedSize=iAllocatedSize; m_pcCurrentPosition=pCurrentPosition;
    if( bWriting )
    {
        switch(pItem->m_iSerializeDataType)
        {
        case SERIALIZE_DATA_TYPE_INT_DESIGNOBJACTIONLIST:
            {
                DesignObjCommandList *pList = (DesignObjCommandList *) pItem->m_pItem;
                Write_unsigned_long((unsigned long) pList->size());
                for(DesignObjCommandList::iterator it=pList->begin();it!=pList->end();++it)
                {
                    (*it)->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
                }
                return true;  // We handled it
            }
            break;
        .
        .
        .
        .
    }
    else
    {
        switch(pItem->m_iSerializeDataType)
        {
        case SERIALIZE_DATA_TYPE_INT_DESIGNOBJACTIONLIST:
            {
                DesignObjCommandList *pList = (DesignObjCommandList *) pItem->m_pItem;
                unsigned long count = Read_unsigned_long();
                for(unsigned long i=0;i<count;++i)
                {
                    DesignObjCommand *pAction = new DesignObjCommand();
                    pAction->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
                    pList->push_back(pAction);
                }
                return true;  // We handled it
            }
            break;
        .
        .
        .
        .
    }
@endcode

*/

/** @class ItemToSerialize
Info about the item.
@see SerializeClass.txt before using !!!
*/
class ItemToSerialize
{

public:

    /** Ttype of item to serialize.
    Can be user defined (use number over 2000) or one of the predefined types.
    */
	int m_iSerializeDataType;

    /** Pointer to the item to be serialized
    */
	void *m_pItem;

	/**
	 * @brief basic constructor
	 * @param iSerializeDataType used to initialize the member
	 * @param pItem used to initialize the member
	 */
	ItemToSerialize(int iSerializeDataType, void *pItem)
	{
		m_iSerializeDataType = iSerializeDataType;
		m_pItem = pItem;
	}
};


/** @class SerializeClass
The main class.
@see SerializeClass.txt before using !!!

Defined in one of two different ways depending if compiling for SYMBIAN.
*/

#ifdef SYMBIAN

#include <e32std.h>
#include "../PlutoUtils/MyString.h"

class SerializeClass : public CBase

#else

class SerializeClass

#endif

{

public:

	char *m_pcDataBlock; /**< the data to be serialized */
	unsigned long m_dwAllocatedSize; /**< how much is allocated for the data */
	char *m_pcCurrentPosition; /**< current position in the DataBlock */
	void *m_pExtraSerializationData;  /**< this is only used in the Serialize method to store misc data */
	unsigned long m_iSC_Version; /**< A schema version.  Set this in your constructor to support multiple schemas. */

    /** If the following is true, then it won't call SetupSerialization, it will assume it's done manually.
	This allows the class to be used in general serialization, like this:
	SerializeClass sc;  sc + myMapIntStrings; sc.SerializeWrite(); */
	bool m_bManuallySetupSerialization;

	/**
	 * @brief basic constructor
	 */
	SerializeClass(bool bManuallySetupSerialization=false)
	{
		m_pcDataBlock = m_pcCurrentPosition = NULL;
		m_iSC_Version = 1;
		m_bManuallySetupSerialization = bManuallySetupSerialization;
	}

	/**
	 * @brief destructor
	 * freeing allocated resources
	 */
	virtual ~SerializeClass()
	{
		MYSTL_ITERATE_VECT(m_vectItemToSerialize,ItemToSerialize,pItem,it)
		{
			delete pItem;
		}
		// We should not delete m_pcDataBlock because if there are several devices nested that
		// were serialized, then will all have the same datablock.  It is the responsibility
		// of whoever called SerializeWrite to be sure they delete the datablock
	}

	/**
	 * @brief free datablock - complements the destructor
	 */
	void FreeSerializeMemory() { delete[] m_pcDataBlock; m_pcDataBlock=m_pcCurrentPosition=NULL; }

	/**
	 * @brief computes the current size of processed data
	 * @return how much of the data block was processed
	 */
	unsigned long CurrentSize() { return (unsigned long)(m_pcCurrentPosition - m_pcDataBlock); }

	/**
	 * @brief name of the class to be serialized
	 * @return "**not specified" if not overriten
	 * @warning it's recommended that all classes derived from this override this function so that the debug output will be meaningful
	 */
	virtual string SerializeClassClassName() { return "**not specified"; }

	/**
	 * @brief when the class wants to start writing to a new data block it will call this
	 * used when you want to serialize a class manually
	 */
	void StartWriting()
	{
		// We should not delete m_pcDataBlock because if there are several devices nested that
		// were serialized, then will all have the same datablock.  It is the responsibility
		// of whoever called SerializeWrite to be sure they delete the datablock.  Just start
		// with a new one
		m_pcDataBlock = new char[BLOCK_SIZE];

#ifndef SYMBIAN
		if(NULL == m_pcDataBlock)
			throw "Cannot allocate memory for m_pcDataBlock";
#endif

		m_dwAllocatedSize = BLOCK_SIZE;
		m_pcCurrentPosition = m_pcDataBlock;
	}


	/**
	 * @brief when the class wants to start reading will call this
	 * used when you want to serialize a class manually
	 */
	void StartReading(unsigned long Size,char *DataPtr)
	{
		m_pcDataBlock = m_pcCurrentPosition = DataPtr;
		m_dwAllocatedSize = Size;
	}

	// The various AddToAutoSerializeList functions for all the supported types

	/**
	 * @brief starts the list of items to be serialized
	 * should be used like: StartSerializeList() + ....
	 * @return the conversion is used in order to get the right methods called and not the overloaded ones
	 */
	SerializeClass &StartSerializeList() { return (*( (SerializeClass *) this)); };

	// Create these for easily adding items to the list.  Now a constructor can just
	// do (*this) + m_iMyInt + m_sMyString + m_bMyBool, etc.

	SerializeClass &operator+ (int &i) {  MYSTL_ADDTO_LIST(m_vectItemToSerialize, new ItemToSerialize(SERIALIZE_DATA_TYPE_LONG, (void *) &i)); return (*this); } /** < @brief overloading + to take an int */
	SerializeClass &operator+ (long &i) { MYSTL_ADDTO_LIST(m_vectItemToSerialize, new ItemToSerialize(SERIALIZE_DATA_TYPE_LONG,(void *) &i));  return (*this); } /** < @brief overloading + to take an long */
	SerializeClass &operator+ (unsigned long &i) { MYSTL_ADDTO_LIST(m_vectItemToSerialize, new  ItemToSerialize(SERIALIZE_DATA_TYPE_UNSIGNED_LONG,(void *) &i)); return (*this); } /** < @brief overloading + to take an unsigned int */
	SerializeClass &operator+ (bool &i) { MYSTL_ADDTO_LIST(m_vectItemToSerialize, new  ItemToSerialize(SERIALIZE_DATA_TYPE_CHAR,(void *) &i)); return (*this); } /** < @brief overloading + to take an bool */
	SerializeClass &operator+ (char &i) { MYSTL_ADDTO_LIST(m_vectItemToSerialize, new  ItemToSerialize(SERIALIZE_DATA_TYPE_CHAR,(void *) &i)); return (*this); } /** < @brief overloading + to take an char */
	SerializeClass &operator+ (float &i) { MYSTL_ADDTO_LIST(m_vectItemToSerialize, new  ItemToSerialize(SERIALIZE_DATA_TYPE_FLOAT,(void *) &i)); return (*this); } /** < @brief overloading + to take an float */
	SerializeClass &operator+ (double &i) { MYSTL_ADDTO_LIST(m_vectItemToSerialize, new  ItemToSerialize(SERIALIZE_DATA_TYPE_DOUBLE,(void *) &i)); return (*this); } /** < @brief overloading + to take an double */
	SerializeClass &operator+ (short &i) { MYSTL_ADDTO_LIST(m_vectItemToSerialize, new  ItemToSerialize(SERIALIZE_DATA_TYPE_SHORT,(void *) &i)); return (*this); } /** < @brief overloading + to take an short */
	SerializeClass &operator+ (string &i) { MYSTL_ADDTO_LIST(m_vectItemToSerialize, new  ItemToSerialize(SERIALIZE_DATA_TYPE_STRING,(void *) &i)); return (*this); } /** < @brief overloading + to take a string */

#if !defined(WINCE) && !defined(SYMBIAN)
	SerializeClass &operator+ (unsigned long long &i) { MYSTL_ADDTO_LIST(m_vectItemToSerialize, new  ItemToSerialize(SERIALIZE_DATA_TYPE_INT64,(void *) &i)); return (*this); } /** < @brief overloading + to take an u_int64_t */
#endif

#ifndef SYMBIAN
	SerializeClass &operator+ (vector<string> &i) { m_vectItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_VECT_STRING,(void *) &i)); return (*this); } /** < @brief overloading + to take a vector of strings */
	SerializeClass &operator+ (map<int,string> &i) { m_vectItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_INT_STRING,(void *) &i)); return (*this); } /** < @brief overloading + to take a map<int,string> */
	SerializeClass &operator+ (map<u_int64_t,string> &i) { m_vectItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_INT64_STRING,(void *) &i)); return (*this); } /** < @brief overloading + to take a map<u_int64_t,string> */
	SerializeClass &operator+ (map<int,int> &i) { m_vectItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_INT_INT,(void *) &i)); return (*this); } /** < @brief overloading + to take a map<int,string> */
	SerializeClass &operator+ (map<string,string> &i) { m_vectItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_STRING_STRING,(void *) &i)); return (*this); } /** < @brief overloading + to take a map<string,string> */
	SerializeClass &operator+ (vector<int> &i) { m_vectItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_VECT_INT,(void *) &i)); return (*this); } /** < @brief overloading + to take a map<int,string> */
	SerializeClass &operator+ (vector< pair<int,int> > &i) { m_vectItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_VECT_PAIR_INT,(void *) &i)); return (*this); } /** < @brief overloading + to take a map<int,string> */
	SerializeClass &operator+ (map<string, pair<int,int> > &i) { m_vectItemToSerialize.push_back(new ItemToSerialize(SERIALIZE_DATA_TYPE_MAP_STRING_PAIR_INT,(void *) &i)); return (*this); } /** < @brief overloading + to take a map<int,string> */
#endif

	SerializeClass &operator+ (PlutoColor &i) { MYSTL_ADDTO_LIST(m_vectItemToSerialize, new  ItemToSerialize(SERIALIZE_DATA_TYPE_COLOR,(void *) &i)); return (*this); } /** < @brief overloading + to take a PlutoColor object */
	SerializeClass &operator+ (PlutoPoint &i) { MYSTL_ADDTO_LIST(m_vectItemToSerialize, new  ItemToSerialize(SERIALIZE_DATA_TYPE_POINT,(void *) &i)); return (*this); } /** < @brief overloading + to take a PlutoPoint object */
	SerializeClass &operator+ (PlutoSize &i) { MYSTL_ADDTO_LIST(m_vectItemToSerialize, new  ItemToSerialize(SERIALIZE_DATA_TYPE_SIZE,(void *) &i)); return (*this); } /** < @brief overloading + to take an PlutoSize object */
	SerializeClass &operator+ (PlutoRectangle &i) { MYSTL_ADDTO_LIST(m_vectItemToSerialize, new  ItemToSerialize(SERIALIZE_DATA_TYPE_RECTANGLE,(void *) &i)); return (*this); } /** < @brief overloading + to take a PlutoRectangle object*/
	SerializeClass &operator+ (PlutoDataBlock &i) { MYSTL_ADDTO_LIST(m_vectItemToSerialize, new  ItemToSerialize(SERIALIZE_DATA_TYPE_BLOCK,(void *) &i)); return (*this); } /** < @brief overloading + to take a PlutoDataBlock object*/

	/**
	 * @brief serializes data to be written
	 * @todo write something about the params
	 *
	 */
	virtual bool Serialize(bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition, void *pExtraSerializationData=NULL);

	/**
	 * @brief serialize writes the data to memory block
	 */
	bool SerializeWrite(void *pExtraSerializationData=NULL)
	{
		StartWriting();
		return Serialize(true, m_pcDataBlock, m_dwAllocatedSize, m_pcCurrentPosition, pExtraSerializationData);
	}

	/**
	 * @brief serialize reads data from memory block
	 */
	bool SerializeRead(unsigned long Size,char *DataPtr,void *pExtraSerializationData=NULL)
	{
		StartReading(Size,DataPtr);
		return Serialize(false, m_pcDataBlock, m_dwAllocatedSize, m_pcCurrentPosition, pExtraSerializationData);
	}

	/**
	 * @brief serialize writes the data to disk file
	 */
	bool SerializeWrite(string sFilename, void *pExtraSerializationData=NULL)
	{
		bool bResult;

#ifndef SYMBIAN
		FILE *pFile = fopen(sFilename.c_str(),"wb");
		if( !pFile )
			return false;

		bResult = SerializeWrite( pExtraSerializationData );
		if( fwrite(m_pcDataBlock, 1, CurrentSize(), pFile) != CurrentSize() )
			bResult=false;

		fclose( pFile );
#else
		RFs aFs;
		aFs.Connect();

		RFile file;
		file.Open(aFs, sFilename.Des(), EFileShareReadersOnly | EFileStream | EFileWrite);

		bResult=SerializeWrite(pExtraSerializationData);

		char *Buff = (char *)pExtraSerializationData;

		HBufC8 *base_str = HBufC8::NewL(CurrentSize());
		TPtr8 pStr = base_str->Des();

		for (unsigned long i = 0; i < CurrentSize(); i++)
			pStr.Append(TChar(Buff[i]));

		if ( KErrNone !=  file.Write(pStr, CurrentSize()) )
			bResult = false;

		delete base_str;
		base_str = NULL; /** @todo comment */

		file.Close();
		aFs.Close();
#endif
		return bResult;
	}

	/**
	 * @brief serialize reads data from disk file
	 */
	bool SerializeRead( string sFilename, void *pExtraSerializationData=NULL );

	/**
	 * @brief override this if you are adding other objects in your derived class
	 */
	virtual bool UnknownSerialize(ItemToSerialize * /*pItem*/, bool /*bWriting*/, char *&/*pcDataBlock*/, unsigned long & /*dwAllocatedSize*/, char *& /*pcCurrentPosition*/) { return false; }

	/**
	 * @brief override this with your own list of members to serialize.  Don't forget to call your base class too
	 */
	virtual void SetupSerialization(int /*iSC_Version*/) {};

	/**
	 * @brief override this if you want to be sure you only Deserialize a particular schema version
	 */
	virtual bool OkayToDeserialize(int /*iSC_Version*/) { return true; }

	/**
	 * @brief Used for classes that know how to serialize themselves.  Only available in the base class
	 * @see the source file SerialiseClass.cpp
	 */
	bool SelfSerializing(bool bWriting, ItemToSerialize *pItem);

	/**
	 * @brief when the class wants to continue reading/writing it will call this
	 */
	void ResumeReadWrite(char *pcDataBlock, unsigned long dwAllocatedSize, char *pcCurrentPosition)
	{
		m_pcDataBlock = pcDataBlock;
		m_dwAllocatedSize = dwAllocatedSize;
		m_pcCurrentPosition = pcCurrentPosition;
	}

	/**
	 * @todo ask Aaron
	 */
	void CheckWrite(unsigned long dwSize)
	{
		unsigned long dwCurrentSize = (unsigned long) (m_pcCurrentPosition - m_pcDataBlock);
		if( m_dwAllocatedSize - dwCurrentSize < dwSize )
		{
			char *pcDataBlock_new = new char[m_dwAllocatedSize + dwSize + BLOCK_SIZE];
			memcpy(pcDataBlock_new,m_pcDataBlock,m_dwAllocatedSize);
			delete[] m_pcDataBlock;
			m_pcDataBlock = pcDataBlock_new;
//			m_pcDataBlock = (char *) realloc(m_pcDataBlock, m_dwAllocatedSize + dwSize + BLOCK_SIZE);
			m_dwAllocatedSize += dwSize + BLOCK_SIZE;
			m_pcCurrentPosition = m_pcDataBlock + dwCurrentSize;
		}
	}

	/**
	 * @brief makes sure the reading was succesfull
	 */
	bool CheckRead(unsigned long dwSize)
	{
		unsigned long dwCurrentSize = (unsigned long) (m_pcCurrentPosition - m_pcDataBlock);
		return  m_dwAllocatedSize > dwCurrentSize && !(m_dwAllocatedSize - dwCurrentSize < dwSize);
	}

	void Write_unsigned_int64(u_int64_t v) {
		CheckWrite(sizeof(u_int64_t));
		u_int64_t *pl = (u_int64_t *) m_pcCurrentPosition;
		*pl = v;
		m_pcCurrentPosition += sizeof(u_int64_t);
	}

#if !defined(WINCE) && !defined(SYMBIAN)
	void Write_int64(unsigned long long v) 
	{
		CheckWrite(sizeof(unsigned long long));
		unsigned long long *pl = (unsigned long long *) m_pcCurrentPosition;
		*pl = v;
		m_pcCurrentPosition += sizeof(unsigned long long);
	}
#endif

	void Write_unsigned_long(unsigned long v) 
	{
		//WARNING: harcoded to 4 bytes!!!
		CheckWrite(sizeof(unsigned int));
		unsigned int *pl = (unsigned int *) m_pcCurrentPosition;

#if !defined(WINCE) && !defined(MAEMO_NOKIA770)
		*pl = v;
#else
		//try to avoid "Datatype misalignment" exception
		((unsigned char *)pl)[3] = (v >> 24)	& 0x000000FF;
		((unsigned char *)pl)[2] = (v >> 16)	& 0x000000FF;
		((unsigned char *)pl)[1] = (v >> 8)		& 0x000000FF;
		((unsigned char *)pl)[0] = v			& 0x000000FF;
#endif
		m_pcCurrentPosition += sizeof(unsigned int);
	}

	void Write_unsigned_short(unsigned short v) {
		CheckWrite(sizeof(unsigned short));
		unsigned short *ps = (unsigned short *) m_pcCurrentPosition;
		*ps = v;
		m_pcCurrentPosition += sizeof(unsigned short);
	}

	void Write_unsigned_char(unsigned char v) {
		CheckWrite(sizeof(unsigned char));
		unsigned char *ps = (unsigned char *) m_pcCurrentPosition;
		*ps = v;
		m_pcCurrentPosition += sizeof(unsigned char);
	}

	void Write_char(char v) {
		CheckWrite(sizeof(char));
		char *ps = (char *) m_pcCurrentPosition;
		*ps = v;
		m_pcCurrentPosition += sizeof(char);
	}

	void Write_long(long v) {
#ifdef DEBUG_SERIALIZATION
		cout << "Writing long " << v << " size: " << (int) CurrentSize() << endl;
#endif

		//WARNING: hardcoded to 4!!!
		CheckWrite(sizeof(int));
		 int *pl = (int *) m_pcCurrentPosition;

#if !defined(WINCE) && !defined(MAEMO_NOKIA770)
		*pl = v;
#else
		//try to avoid "Datatype misalignment" exception
		((unsigned char *)pl)[3] = (v >> 24)	& 0x000000FF;
		((unsigned char *)pl)[2] = (v >> 16)	& 0x000000FF;
		((unsigned char *)pl)[1] = (v >> 8)		& 0x000000FF;
		((unsigned char *)pl)[0] = v			& 0x000000FF;
#endif


		m_pcCurrentPosition += sizeof(int);
	}

	void Write_float(float v) {
		CheckWrite(sizeof(float));
		float *ps = (float *) m_pcCurrentPosition;
		*ps = v;
		m_pcCurrentPosition += sizeof(float);
	}

	void Write_double(double v) {
		CheckWrite(sizeof(double));
		double *ps = (double *) m_pcCurrentPosition;
		*ps = v;
		m_pcCurrentPosition += sizeof(double);
	}

	void Write_short(short v) {
		CheckWrite(sizeof(short));
		 short *ps = (short *) m_pcCurrentPosition;
		*ps = v;
		m_pcCurrentPosition += sizeof(short);
	}

	void Write_string(string &str)
	{
#ifdef DEBUG_SERIALIZATION
		cout << "Writing string " << str << " size: " << (int) CurrentSize() << endl;
#endif
#ifndef SYMBIAN
		CheckWrite((unsigned long) str.length()+1);
		strcpy(m_pcCurrentPosition,str.c_str());
		m_pcCurrentPosition += str.length()+1;
#else
		CheckWrite((unsigned long) str.length()+1);
	    int i = 0;
		int len = str.length();
		while ( i < len )
		{
		    *(m_pcCurrentPosition++)= (char) str[i];
			 i++;
		}
	   *(m_pcCurrentPosition++) = 0;
#endif
	}

	void Write_block(char *ptr,unsigned long size)
	{
		CheckWrite((unsigned long) size);

		memcpy(m_pcCurrentPosition,ptr,size);
		m_pcCurrentPosition += size;
	}

	u_int64_t Read_unsigned_int64() {
		if( !CheckRead(sizeof(u_int64_t)) )
			return 0;
		u_int64_t *pl = (u_int64_t *) m_pcCurrentPosition;
		m_pcCurrentPosition += sizeof(u_int64_t);
		return *pl;
	}

	 u_int64_t Read_int64() {
		if( !CheckRead(sizeof( u_int64_t)) )
			return 0;
		 u_int64_t *pl = ( u_int64_t *) m_pcCurrentPosition;
		m_pcCurrentPosition += sizeof( u_int64_t);
		return *pl;
	}

	unsigned long Read_unsigned_long() {

                //WARNING: hardcoded to 4 bytes
                if( !CheckRead(sizeof(unsigned int)) )
                        return 0;
                unsigned int *pl = (unsigned int *) m_pcCurrentPosition;
                m_pcCurrentPosition += sizeof(unsigned int);

                /** @todo this */
                unsigned char v1 = ((unsigned char *)pl)[0];
                unsigned char v2 = ((unsigned char *)pl)[1];
                unsigned char v3 = ((unsigned char *)pl)[2];
                unsigned char v4 = ((unsigned char *)pl)[3];

                unsigned int myval2 = (v4 << 24) + (v3 << 16) + (v2 << 8) + v1;

                /** @todo check comment */
                //long myval = long(*pl);

#ifdef DEBUG_SERIALIZATION
                cout << "Reading unsigned long " << ((int) myval2) << " size: " << (int) CurrentSize() << endl;
#endif
                //return (long)*pl;
                return myval2;
	}

	unsigned short Read_unsigned_short() {
		if( !CheckRead(sizeof(unsigned short)) )
			return 0;
		unsigned short *pl = (unsigned short *) m_pcCurrentPosition;
		m_pcCurrentPosition += sizeof(unsigned short);
		return *pl;
	}

	unsigned char Read_unsigned_char() {
		if( !CheckRead(sizeof(unsigned char)) )
			return 0;
		unsigned char *pl = (unsigned char *) m_pcCurrentPosition;
		m_pcCurrentPosition += sizeof(unsigned char);
		return *pl;
	}

	char Read_char() {
		if( !CheckRead(sizeof(char)) )
			return 0;
		char *pl = (char *) m_pcCurrentPosition;
		m_pcCurrentPosition += sizeof(char);
		return *pl;
	}

	long Read_long() 
	{
		//WARNING: hardcoded to 4 bytes
		if( !CheckRead(sizeof(int)) )
			return 0;
		int *pl = (int *) m_pcCurrentPosition;
		m_pcCurrentPosition += sizeof(int);

		/** @todo this */
		unsigned char v1 = ((unsigned char *)pl)[0];
		unsigned char v2 = ((unsigned char *)pl)[1];
		unsigned char v3 = ((unsigned char *)pl)[2];
		unsigned char v4 = ((unsigned char *)pl)[3];

		int myval2 = (v4 << 24) + (v3 << 16) + (v2 << 8) + v1;

		/** @todo check comment */
		//long myval = long(*pl);

#ifdef DEBUG_SERIALIZATION
		cout << "Reading long " << ((int) myval2) << " size: " << (int) CurrentSize() << endl;
#endif
		//return (long)*pl;
		return myval2;
	}

	float Read_float() {
		if( !CheckRead(sizeof(float)) )
			return 0;
		float *pl = (float *) m_pcCurrentPosition;
		m_pcCurrentPosition += sizeof(float);
		return *pl;
	}

	double Read_double() {
                if( !CheckRead(sizeof(double)) )
                        return 0;
                double *pl = (double *) m_pcCurrentPosition;
                m_pcCurrentPosition += sizeof(double);
                return *pl;
	}

	short Read_short() {
		if( !CheckRead(sizeof(short)) )
			return 0;
		short *pl = (short *) m_pcCurrentPosition;
		m_pcCurrentPosition += sizeof(short);
		return *pl;
	}

	void Read_string(string &str)
	{
#ifndef SYMBIAN
		str.clear();
#else
		str.SetLength(0);
#endif
		while ( m_pcCurrentPosition < m_pcDataBlock + m_dwAllocatedSize && *m_pcCurrentPosition) {
#ifndef SYMBIAN
			 str.append(1, *m_pcCurrentPosition++);
#else
			 str.Append(*m_pcCurrentPosition++);
#endif
		}

		m_pcCurrentPosition++; //also skip '\0'

/*

#ifndef SYMBIAN
		// Don't bother checking
		str = m_pcCurrentPosition;
		m_pcCurrentPosition += str.length()+1;
#else
		str.SetLength(0);
		while ( *m_pcCurrentPosition )
		{
			 str.Append(*m_pcCurrentPosition++);
		}

		m_pcCurrentPosition++; //also skip '\0'
#endif

*/

#ifdef DEBUG_SERIALIZATION
		cout << "Reading string " << str << " size: " << (int) CurrentSize() << endl;
#endif
	}

	char *Read_block(unsigned long size)
	{
		if( !CheckRead(size) )
			return NULL;

        if(size)
        {
            char *ptr = new char[size];
            memcpy(ptr,m_pcCurrentPosition,size);
            m_pcCurrentPosition += size;
            return ptr;
        }
        else
            return NULL;
    }

protected:

	/**
     * @brief for Symbian compatibility; see Symbian.txt for detailes
     */
	MYSTL_CREATE_VECT(m_vectItemToSerialize, ItemToSerialize);
};

#endif
