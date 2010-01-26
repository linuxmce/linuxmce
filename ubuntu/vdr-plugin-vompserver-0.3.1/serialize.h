/*
    Copyright 2004-2005 Chris Tallon, Andreas Vogel

    This file is part of VOMP.

    VOMP is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    VOMP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VOMP; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef SERIALIZE_H
#define SERIALIZE_H
#include <vector>
using namespace std;
#include <stdio.h>
#include <string.h>
#include "defines.h"

class SerializeBuffer {
  public:
    //constructor for send buffers
    SerializeBuffer(ULONG size,bool isMalloc=false,bool autoincrease=false);
    //constructor for SerializeBuffers with external buffers
    SerializeBuffer(UCHAR *buffer,ULONG size,bool owning=false,bool isMalloc=true,bool autoincrease=false);
    ~SerializeBuffer();
    //access to bufferpointer
    UCHAR * getStart(){ return start;}
    UCHAR * getCurrent(){ return current;}
    UCHAR * getEnd(){ return end;}
    //get the buffer and take it away from this object
    UCHAR * steelBuffer();
    //seek relative to current
    int seek(int amount);
    void rewind();

    //encode/decode functions
    //always return != 0 on error
    int encodeLong(ULONG data);
    int encodeLongLong(ULLONG data);
    int encodeShort(USHORT data);
    int encodeString(const char *data);
    int encodeByte(UCHAR data);

    int decodeLong(ULONG & data);
    int decodeLong(int & data);
    int decodeLongLong(ULLONG & data);
    int decodeShort(USHORT & data);
    int decodeString(ULONG &len,char * &data);
    int decodeByte(UCHAR &data);

  private:
    UCHAR * start;
    UCHAR * end;
    UCHAR * current;
    ULONG size;
    bool useMalloc;
    bool owning;
    bool autoincrease;

    //check buffer space and enlarge if allowed
    int checkSpace(int size);

};

class Serializable {
  public:
    Serializable();
    virtual ~Serializable();
    //serialize functions
    //get the #of bytes needed to serialize
    int getSerializedLen();
    //serialize
    //advance buffer, check if >= end
    //return 0 if OK
    int serialize(SerializeBuffer *b);
    //deserialize
    //advance buffer, check if >= end
    //return 0 if OK
    int deserialize(SerializeBuffer *b);
    //or the received version after deserialize
    //can be queried with deserializeImpl
    USHORT getVersion();
    //helper
    static int getSerializedStringLen(const char *str);
  protected:
    //methods to be overwritten by impl
    //those methods can use the version info
    //the length and version is automatically encoded and decoded by the base class
    virtual int getSerializedLenImpl()=0;
    virtual int serializeImpl(SerializeBuffer *b)=0;
    virtual int deserializeImpl(SerializeBuffer *b)=0;
    USHORT version;
 };

/**
  * a class for creating a list of serializable parameters
  * by including a version handling this automatically maintains compatibility
  * by correct usage.
  * usage example:
  * 1. version
  * USHORT myP1=0;
  * ULONG  myP2=0;
  * SerializableList myList();
  * myList.addParam(&myP1);
  * myList.addParam(&myP2);
  * //now serialize/deserialize...
  * //later - second version:
  * USHORT myP1=0;
  * ULONG  myP2=0;
  * char *myString=NULL;
  * SerializableList myList();
  * myList.addParam(&myP1);
  * myList.addParam(&myP2);
  * myList.addParam(&myString,2); //this parameter is new in version 2
  * myList.deserialize(buffer);
  * if (!myList.isDeserialized(&myString)) {
  *   //we got a version 1 message
  *   myString=strcpy(new char[22],"default-for-myString");
  * }
  *
  */
class SerializableList : public Serializable{
  public:
    SerializableList();
    virtual ~SerializableList();
    /**
      * addParam
      * add a parameter to the list
      * no life cycle handling included
      * params should be initialized before!
      * when adding new params after having released a version
      * add them to the end with a new version - this will automatically maintain
      * compatibility
      * will return !=0 if adding is not possible (e.g. adding with a smaller version)
      */
    int addParam(Serializable *p,USHORT version=1);
    int addParam(USHORT *p,USHORT version=1);
    int addParam(ULONG *p,USHORT version=1);
    int addParam(ULLONG *p,USHORT version=1);
    int addParam(char **p,USHORT version=1);

    /**
      * for lists only intended for encoding also
      * const members will be added
      * this is fully compatible to non const addParams on the  other side
      * so on the sender side you can use the addParam(const ... ) methods, on the receiver 
      * the non-const
      * After having called one of this methods deserialize will always fail!
      */
    int addParam(const Serializable *p,USHORT vs=1){
      encodeOnly=true;
      return addParam((Serializable *)p,vs);
    }
    int addParam(const USHORT *p,USHORT vs=1){
      encodeOnly=true;
      return addParam((USHORT *)p,vs);
    }
    int addParam(const ULONG *p,USHORT vs=1){
      encodeOnly=true;
      return addParam((ULONG *)p,vs);
    }
    int addParam(const int *p,USHORT vs=1){
      encodeOnly=true;
      return addParam((ULONG *)p,vs);
    }
    int addParam(const ULLONG *p,USHORT vs=1){
      encodeOnly=true;
      return addParam((ULLONG *)p,vs);
    }
    int addParam(const char **p,USHORT vs=1){
      encodeOnly=true;
      return addParam((char **)p,vs);
    }


    /**
      * check functions to test if a certain parameter has been filled
      * during deserialize
      */
    bool isDeserialized(Serializable *p);
    bool isDeserialized(USHORT *p);
    bool isDeserialized(ULONG *p);
    bool isDeserialized(ULLONG *p);
    bool isDeserialized(char **p);

    //return the highest version after adding params


  protected:
    virtual int getSerializedLenImpl();
    virtual int serializeImpl(SerializeBuffer *b);
    virtual int deserializeImpl(SerializeBuffer *b);
    bool encodeOnly; //if any addParam(const ...) is called this will be set

  private:
    typedef enum{
      TUNKNOWN,
      TSER,
      TUSHORT,
      TULONG,
      TULLONG,
      TCHAR } Ptypes;
    struct Pentry{
      Ptypes ptype;
      bool isDeserialized;
      USHORT version;
      union {
        Serializable *pser;
        USHORT *pshort;
        ULONG *plong;
        ULLONG *pllong;
        char **pchar;
      } ptr;
      Pentry() {
        ptype=TUNKNOWN;
        version=1;
        isDeserialized=false;
        ptr.pser=NULL;
      }
      bool isEqual(void *p,Ptypes t);
    } ;
    vector<struct Pentry>list;
    Pentry *findEntry(void *p,Ptypes t);
};



#endif
