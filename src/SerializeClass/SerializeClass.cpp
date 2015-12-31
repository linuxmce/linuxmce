/*
	SerializeClass

	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#ifndef SYMBIAN
#include "PlutoUtils/CommonIncludes.h"
#include "SerializeClass/ShapesColors.h"
#endif

#include "SerializeClass.h"

bool SerializeClass::Serialize( bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition, void *pExtraSerializationData )
{
	// Save the starting points
	m_pcDataBlock=pcDataBlock; m_dwAllocatedSize=dwAllocatedSize; m_pcCurrentPosition=pcCurrentPosition;

	// In some cases a class may need extra data to properly serialize in UnknownSerialize.  This is an extra void pointer that can be cast to something useful
	m_pExtraSerializationData=pExtraSerializationData;

	if( !m_bManuallySetupSerialization )
	{
		// For Symbian compatibility
		MYSTL_ITERATE_VECT( m_vectItemToSerialize, ItemToSerialize, pItem_del, it_del )
		{
			delete pItem_del;
		}
		MYSTL_CLEAR_LIST(m_vectItemToSerialize);
		if( bWriting )
		{
			Write_unsigned_long(m_iSC_Version);
			SetupSerialization(m_iSC_Version);
		}
		else
		{
			unsigned long iSC_Version = Read_unsigned_long();
			if( !OkayToDeserialize(iSC_Version) )
			{
	#ifndef SYMBIAN
	#ifndef WINCE //no 'cerr' under CE
				cerr << "It's not okay to deserialize version " << iSC_Version << endl;
	#endif
	#endif
				return false;
			}
			SetupSerialization(iSC_Version);
		}
	}
#ifdef DEBUG_SERIALIZATION
	cout << "Schema for: " << SerializeClassClassName();
	MYSTL_ITERATE_VECT(m_vectItemToSerialize,ItemToSerialize,pItem_cout,it_cout)
	{
		cout << " " << pItem_cout->m_iSerializeDataType;
	}
	cout << endl;
#endif
	MYSTL_ITERATE_VECT(m_vectItemToSerialize,ItemToSerialize,pItem,it)
	{

		// These are self-serializing, we don't need the if( bWriting ) split.  However, these classes are
		// derived from SerializeClass.  Therefore, we can't include the class definitions before binary
		// serialize.  We do a forward declaration, and have to put the actual switch block in a .cpp
		// file to avoid the cylcical dependencies
		if( SelfSerializing(bWriting,pItem) )
			continue;

		if( bWriting )
		{
#ifdef DEBUG_SERIALIZATION
			cout << "Writing type " << pItem->m_iSerializeDataType << " from class " << SerializeClassClassName() << " at pos: " << (int) CurrentSize() << endl;
#endif
			switch(pItem->m_iSerializeDataType)
			{
			case SERIALIZE_DATA_TYPE_CHAR:
				Write_char( *((char *) pItem->m_pItem) );
				break;
			case SERIALIZE_DATA_TYPE_FLOAT:
				Write_float( *((float *) pItem->m_pItem) );
				break;
			case SERIALIZE_DATA_TYPE_DOUBLE:
				Write_double( *((double *) pItem->m_pItem) );
				break;
			case SERIALIZE_DATA_TYPE_SHORT:
				Write_short( *((short *) pItem->m_pItem) );
				break;
			case SERIALIZE_DATA_TYPE_LONG:
				Write_long( *((long *) pItem->m_pItem) );
				break;
			case SERIALIZE_DATA_TYPE_UNSIGNED_LONG:
				Write_unsigned_long( *((unsigned long *) pItem->m_pItem) );
				break;
#if !defined(SYMBIAN) && !defined(WINCE)
			case SERIALIZE_DATA_TYPE_INT64:
				Write_int64( *((u_int64_t *) pItem->m_pItem) );
				break;
#endif

			case SERIALIZE_DATA_TYPE_STRING:
				Write_string( *((string *) pItem->m_pItem) );
				break;
#ifndef SYMBIAN
			case SERIALIZE_DATA_TYPE_VECT_STRING:
				{
					vector<string> *pVect = (vector<string> *) pItem->m_pItem;
					Write_unsigned_long((unsigned long) pVect->size());
#ifdef DEBUG_SERIALIZATION
					cout << "Writing " << (unsigned long) pVect->size() << " vector strings" << endl;
#endif
					for(size_t s=0; s < pVect->size(); ++s)
					{
						Write_string( (*pVect)[s] );
					}
				}
				break;
			case SERIALIZE_DATA_TYPE_BLOCK:
				{
					PlutoDataBlock *pBlock = (PlutoDataBlock *) pItem->m_pItem;
					Write_unsigned_long(pBlock->m_dwSize);
					Write_block(pBlock->m_pBlock,pBlock->m_dwSize);
				}
				break;

			case SERIALIZE_DATA_TYPE_VECT_INT:
				{
					vector<int> *pVect = (vector<int> *) pItem->m_pItem;
					Write_unsigned_long((unsigned long) pVect->size());
#ifdef DEBUG_SERIALIZATION
					cout << "Writing " << (unsigned long) pVect->size() << " vector ints" << endl;
#endif
					for(size_t s=0; s < pVect->size(); ++s)
					{
						Write_unsigned_long( (*pVect)[s] );
					}
				}
				break;
			case SERIALIZE_DATA_TYPE_VECT_PAIR_INT:
				{
					vector< pair<int,int> > *pVect = (vector< pair<int,int> > *) pItem->m_pItem;
					Write_unsigned_long((unsigned long) pVect->size());
#ifdef DEBUG_SERIALIZATION
					cout << "Writing " << (unsigned long) pVect->size() << " vector pair ints" << endl;
#endif
					for(size_t s=0; s < pVect->size(); ++s)
					{
						Write_unsigned_long( (*pVect)[s].first );
						Write_unsigned_long( (*pVect)[s].second );
					}
				}
				break;
			case SERIALIZE_DATA_TYPE_MAP_STRING_PAIR_INT:
				{
					map<string, pair<int,int> > *pMap = (map<string, pair<int,int> > *) pItem->m_pItem;
					Write_unsigned_long((unsigned long) pMap->size());
#ifdef DEBUG_SERIALIZATION
					cout << "Writing " << (unsigned long) pMap->size() << " map string pair ints" << endl;
#endif
					for(map<string, pair<int,int> >::iterator it=pMap->begin();it!=pMap->end();++it)
					{
						string s = it->first;
						Write_string( s );
						Write_unsigned_long( it->second.first );
						Write_unsigned_long( it->second.second );
					}
				}
				break;
			case SERIALIZE_DATA_TYPE_INT_STRING:
				{
					map<int,string> *pMap = (map<int,string> *) pItem->m_pItem;
					Write_unsigned_long((unsigned long) pMap->size());
#ifdef DEBUG_SERIALIZATION
					cout << "Writing " << (unsigned long) pMap->size() << " map int strings" << endl;
#endif
					map<int,string>::iterator it;
					for(it=pMap->begin(); it!=pMap->end(); ++it)
					{
						Write_long( (*it).first );
						Write_string( (*it).second );
					}
				}
				break;
			case SERIALIZE_DATA_TYPE_INT64_STRING:
				{
					map<u_int64_t,string> *pMap = (map<u_int64_t,string> *) pItem->m_pItem;
					Write_unsigned_long((unsigned long) pMap->size());
#ifdef DEBUG_SERIALIZATION
					cout << "Writing " << (unsigned long) pMap->size() << " map int strings" << endl;
#endif
					map<u_int64_t,string>::iterator it;
					for(it=pMap->begin(); it!=pMap->end(); ++it)
					{
#ifndef WINCE
						Write_int64( (*it).first );
#endif
						Write_string( (*it).second );
					}
				}
				break;
			case SERIALIZE_DATA_TYPE_INT_INT:
				{
					map<int,int> *pMap = (map<int,int> *) pItem->m_pItem;
					Write_unsigned_long((unsigned long) pMap->size());
#ifdef DEBUG_SERIALIZATION
					cout << "Writing " << (unsigned long) pMap->size() << " map int int" << endl;
#endif
					map<int,int>::iterator it;
					for(it=pMap->begin(); it!=pMap->end(); ++it)
					{
						Write_long( (*it).first );
						Write_long( (*it).second );
					}
				}
				break;
			case SERIALIZE_DATA_TYPE_STRING_STRING:
				{
					map<string,string> *pMap = (map<string,string> *) pItem->m_pItem;
					Write_unsigned_long((unsigned long) pMap->size());
#ifdef DEBUG_SERIALIZATION
					cout << "Writing " << (unsigned long) pMap->size() << " map string strings" << endl;
#endif
					map<string,string>::iterator it;
					for(it=pMap->begin(); it!=pMap->end(); ++it)
					{
						string key=(*it).first;
						Write_string( key );
						Write_string( (*it).second );
					}
				}
				break;

#endif

			default:
				{
					if( !UnknownSerialize(pItem,bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition) )
					{

#ifdef DEBUG_SERIALIZATION
						cout << "Unknown Serialize failed" << endl;
#else

#ifndef SYMBIAN

						throw ("Don't know how to serialize data type" + StringUtils::itos(pItem->m_iSerializeDataType)).c_str();
#else
						/** @todo show a popup */
#endif
#endif
						return false; // We don't know how to serialize this type
					}
				}
			};
		}
		else
		{

#ifdef DEBUG_SERIALIZATION
			cout << "Reading type " << pItem->m_iSerializeDataType << " from class " << SerializeClassClassName() << " at pos:" << (int) CurrentSize() << endl;
#endif
			switch(pItem->m_iSerializeDataType)
			{
			case SERIALIZE_DATA_TYPE_CHAR:
				*((char *) pItem->m_pItem)=Read_char();
				break;
			case SERIALIZE_DATA_TYPE_FLOAT:
				*((float *) pItem->m_pItem)=Read_float();
				break;
			case SERIALIZE_DATA_TYPE_DOUBLE:
				*((double *) pItem->m_pItem)=Read_double();
				break;
			case SERIALIZE_DATA_TYPE_SHORT:
				*((short *) pItem->m_pItem)=Read_short();
				break;
			case SERIALIZE_DATA_TYPE_LONG:
				*((int *) pItem->m_pItem)=Read_long();
				break;
			case SERIALIZE_DATA_TYPE_UNSIGNED_LONG:
				*((unsigned int *) pItem->m_pItem)=Read_unsigned_long();
				break;
#ifndef SYMBIAN
			case SERIALIZE_DATA_TYPE_INT64:
				*((u_int64_t *) pItem->m_pItem)=Read_int64();
				break;
#endif
			case SERIALIZE_DATA_TYPE_STRING:
				Read_string( *((string *) pItem->m_pItem) );
				break;
#ifndef SYMBIAN
			case SERIALIZE_DATA_TYPE_VECT_STRING:
				{
					vector<string> *pVect = (vector<string> *) pItem->m_pItem;
					unsigned long count = Read_unsigned_long();
#ifdef DEBUG_SERIALIZATION
					cout << "Reading " << count << " vector strings" << endl;
#endif
					for(size_t s=0;s<count;++s)
					{
						string value;
						Read_string(value);
						pVect->push_back(value);
					}
				}
				break;
			case SERIALIZE_DATA_TYPE_VECT_INT:
				{
					vector<int> *pVect = (vector<int> *) pItem->m_pItem;
					unsigned long count = Read_unsigned_long();
#ifdef DEBUG_SERIALIZATION
					cout << "Reading " << count << " vector ints" << endl;
#endif
					for(size_t s=0;s<count;++s)
					{
						pVect->push_back(Read_unsigned_long());
					}
				}
				break;
			case SERIALIZE_DATA_TYPE_VECT_PAIR_INT:
				{
					vector< pair<int,int> > *pVect = (vector< pair<int,int> > *) pItem->m_pItem;
					unsigned long count = Read_unsigned_long();
#ifdef DEBUG_SERIALIZATION
					cout << "Reading " << count << " vector pair ints" << endl;
#endif
					for(size_t s=0;s<count;++s)
					{
						long l1 = Read_unsigned_long();
						long l2 = Read_unsigned_long();
						pVect->push_back( pair<int,int> (l1,l2) );
					}
				}
				break;
			case SERIALIZE_DATA_TYPE_MAP_STRING_PAIR_INT:
				{
					map<string, pair<int,int> > *pMap = (map<string, pair<int,int> > *) pItem->m_pItem;
					unsigned long count = Read_unsigned_long();
#ifdef DEBUG_SERIALIZATION
					cout << "Reading " << count << " map string pair ints" << endl;
#endif
					for(size_t s=0;s<count;++s)
					{
						string str;
						Read_string(str);
						int i1 = Read_unsigned_long();
                        int i2 = Read_unsigned_long();
#ifdef QT_VERSION
#if QT_VERSION <= 0x050000
                         (*pMap)[str] = make_pair<int,int> (int(i1),int(i2));
#else
                        (*pMap)[str] = make_pair<int,int> (i1,i2);
#endif
#endif
					}
				}
				break;
			case SERIALIZE_DATA_TYPE_INT_STRING:
				{
					map<int,string> *pMap = (map<int,string> *) pItem->m_pItem;
					unsigned long count=Read_unsigned_long();
#ifdef DEBUG_SERIALIZATION
					cout << "Reading " << count << " map int strings" << endl;
#endif
					for(size_t s=0;s<count;++s)
					{
						int Key = Read_long();
						string value;
						Read_string(value);
						(*pMap)[Key]=value;
					}
				}
				break;
				case SERIALIZE_DATA_TYPE_INT64_STRING:
				{
					map<u_int64_t,string> *pMap = (map<u_int64_t,string> *) pItem->m_pItem;
					unsigned long count=Read_unsigned_long();
#ifdef DEBUG_SERIALIZATION
					cout << "Reading " << count << " map int strings" << endl;
#endif
					for(size_t s=0;s<count;++s)
					{
						u_int64_t Key = Read_int64();
						string value;
						Read_string(value);
						(*pMap)[Key]=value;
					}
				}
				break;
			case SERIALIZE_DATA_TYPE_INT_INT:
				{
					map<int,int> *pMap = (map<int,int> *) pItem->m_pItem;
					unsigned long count=Read_unsigned_long();
#ifdef DEBUG_SERIALIZATION
					cout << "Reading " << count << " map int int" << endl;
#endif
					for(size_t s=0;s<count;++s)
					{
						int Key = Read_long();
						int value = Read_long();
						(*pMap)[Key]=value;
					}
				}
				break;
			case SERIALIZE_DATA_TYPE_STRING_STRING:
				{
					map<string,string> *pMap = (map<string,string> *) pItem->m_pItem;
					unsigned long count=Read_unsigned_long();
#ifdef DEBUG_SERIALIZATION
					cout << "Reading " << count << " map string strings" << endl;
#endif
					for(size_t s=0;s<count;++s)
					{
						string key,value;
						Read_string(key);
						Read_string(value);
						(*pMap)[key]=value;
					}
				}
				break;
#endif
			case SERIALIZE_DATA_TYPE_BLOCK:
				{
					PlutoDataBlock *pBlock = (PlutoDataBlock *) pItem->m_pItem;
					pBlock->m_dwSize = Read_unsigned_long();
					pBlock->m_pBlock=Read_block(pBlock->m_dwSize);
				}
				break;
			default:
				{
					if( !UnknownSerialize(pItem,bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition) )
					{
#ifdef DEBUG_SERIALIZATION
						cout << "Unknown Serialize failed" << endl;
#else

#ifndef SYMBIAN
						throw ("Don't know how to serialize data type" + StringUtils::itos(pItem->m_iSerializeDataType)).c_str();
#else
						/** @todo show a popup */
#endif

#endif
						return false;
					}
				}
			};
		}
	}

	// We may have re-allocated the memory block and size, and the position will have changed
	pcDataBlock=m_pcDataBlock; dwAllocatedSize=m_dwAllocatedSize; pcCurrentPosition=m_pcCurrentPosition;
	return true;
}


bool SerializeClass::SerializeRead( string sFilename, void *pExtraSerializationData )
{
#ifndef SYMBIAN
	long lSize = FileUtils::FileSize(sFilename.c_str());
	char *pBuf = new char[lSize];

	FILE *pFile = fopen(sFilename.c_str(), "rb");
	if( !pFile )
	{
		delete[] pBuf;
		return false;
	}

	fread((void*)pBuf, 1, lSize, pFile);
	fclose(pFile);
#else
	RFs aFs;
	aFs.Connect();

	RFile file;
	file.Open(aFs, sFilename.Des(), EFileShareReadersOnly | EFileStream | EFileRead);

	TInt lSize;
	file.Size(lSize);

	HBufC8* Buffer = HBufC8::NewL(lSize);
	TPtr8 pStr = Buffer->Des();

	if( KErrNone != file.Read(pStr, lSize))
		return false;

	char *pBuf = new char[lSize];

	for (int i = 0; i < lSize; i++)
		pBuf[i] = (char)pStr[i];

	file.Close();
	aFs.Close();

	delete Buffer;
	Buffer = NULL;
#endif

	return SerializeRead(lSize, pBuf, pExtraSerializationData);
}


bool SerializeClass::SelfSerializing(bool bWriting,ItemToSerialize *pItem)
{
#ifndef SYMBIAN
	switch(pItem->m_iSerializeDataType)
	{
	case SERIALIZE_DATA_TYPE_COLOR:
		{
			PlutoColor *pColor = (PlutoColor *) pItem->m_pItem;
			pColor->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
		}
		return true;
	case SERIALIZE_DATA_TYPE_POINT:
		{
			PlutoPoint *pPoint = (PlutoPoint *) pItem->m_pItem;
			pPoint->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
		}
		return true;
	case SERIALIZE_DATA_TYPE_SIZE:
		{
			PlutoSize *pSize = (PlutoSize *) pItem->m_pItem;
			pSize->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
		}
		return true;
	case SERIALIZE_DATA_TYPE_RECTANGLE:
		{
			PlutoRectangle *pRectangle = (PlutoRectangle *) pItem->m_pItem;
			pRectangle->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
		}
		return true;
	};
#endif

	return false;
}
