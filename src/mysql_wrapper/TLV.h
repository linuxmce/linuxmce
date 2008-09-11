#ifndef __TLV_H__
#define __TLV_H__

#include <string>
#include <iostream>
#include <cstring>
#include <stdint.h>
#include <sys/types.h>

using namespace std;

struct TLV {
	enum Type {
		// not typed
		typeNone=0,

  		// functions
		typeFunction_RealQuery,
		typeFunction_Query,
		typeFunction_RealEscapeString,
  		typeFunction_Error,
    		typeFunction_AffectedRows,
      		typeFunction_InsertID,
		typeFunction_StoreResult,
  		typeFunction_FetchRow,
    		typeFunction_FetchLengths,
      		typeFunction_FreeResult,
		typeFunction_FieldCount,
		
		// data
		typeData_BOOL,
 		typeData_UINT,
		typeData_UINTArray,
  		typeData_LONG,
  		typeData_ULONG,
  		typeData_ULONGArray,
  		typeData_CHARArray,
    		typeData_ROW,
      
      		// misc
      		typeData_PING,
		typeData_PONG,
  		typeCommand_SHUTDOWN,

  		typeDataset, typeRow, typeField, typeAffectedRows, typeError};
	Type type;
	uint length;
	char *value;
	
	TLV(): type(typeNone), length(0), value(NULL) {};
	
	TLV(Type inType): type(inType), length(0), value(NULL) {};
	
	TLV(Type inType, const string inValue) 
	{
		type  = inType;
		length = inValue.length()+1;
		value = new char[length];
		memcpy( value, inValue.c_str(), length );
	}
	
	TLV(Type inType, const char *inValue) 
	{
		type  = inType;
		length = strlen(inValue);
		if (length)
		{
			value = new char[length];
			memcpy( value, inValue, length );
		}
		else
			value = NULL;
	}
	
	TLV(Type inType, const char *inValue, uint inLength) 
	{
		type  = inType;
		length = inLength;
		if (length)
		{
			value = new char[length];
			memcpy(value, inValue, length);
		}
		else
			value = NULL;
	}
	
	TLV(uint inValue) 
	{
		type  = typeData_UINT;
		length = sizeof(uint);
		value = new char[length];
		memcpy(value, &inValue, length);
	}
	
	TLV(long inValue) 
	{
		type  = typeData_LONG;
		length = sizeof(long);
		value = new char[length];
		memcpy(value, &inValue, length);
	}
	
	TLV(unsigned long inValue) 
	{
		type  = typeData_ULONG;
		length = sizeof(unsigned long);
		value = new char[length];
		memcpy(value, &inValue, length);
	}
	
	TLV(bool inValue) 
	{
		type  = typeData_BOOL;
		length = sizeof(bool);
		value = new char[length];
		memcpy(value, &inValue, length);
	}
	
	~TLV() { 
		delete [] value; 
	};

	char *c_str()
	{
		char *msg = new char[length+1];
		memcpy(msg, value, length);
		msg[length] = 0;
		return msg;
	}
	
	bool to_bool()
	{
		if (length==sizeof(bool))
		{
			bool result;
			memcpy(&result, value, length);
			return result;
		}
		else
		{
			cerr << "TLV::to_bool() : Type size mismatch" << endl;
			return false;
		}
	}

	uint to_uint()
	{
		if (length==sizeof(uint))
		{
			uint result;
			memcpy(&result, value, length);
			return result;
		}
		else
		{
			cerr << "TLV::to_uint() : Type size mismatch" << endl;
			return false;
		}
	}

	unsigned long to_ulong()
	{
		if (length==sizeof(unsigned long))
		{
			unsigned long result;
			memcpy(&result, value, length);
			return result;
		}
		else
		{
			cerr << "TLV::to_ulong() : Type size mismatch" << endl;
			return false;
		}
	}

	long to_long()
	{
		if (length==sizeof(long))
		{
			long result;
			memcpy(&result, value, length);
			return result;
		}
		else
		{
			cerr << "TLV::to_long() : Type size mismatch" << endl;
			return false;
		}
	}
};

#endif
