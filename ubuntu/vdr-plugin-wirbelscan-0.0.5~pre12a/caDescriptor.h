/*

these are 1:1 copies of the three classes from <vdr/pat.c> :
	cCaDescriptor 
	cCaDescriptors
	cCaDescriptorHandler

copyright: see original file pat.c (vdr-1.6.0, http://www.cadsoft.de/vdr/)

*/

#ifndef __CA_DESCIPTOR_H_
#define __CA_DESCIPTOR_H_

#include <vdr/tools.h>
#include <vdr/channels.h>
#include <libsi/section.h>
#include <libsi/descriptor.h>
#include <libsi/si.h>

// --- cCaDescriptor ---------------------------------------------------------

class cCaDescriptor : public cListObject {
private:
  int caSystem;
  bool stream;
  int length;
  uchar *data;
public:
  cCaDescriptor(int CaSystem, int CaPid, bool Stream, int Length, const uchar *Data);
  virtual ~cCaDescriptor();
  bool operator== (const cCaDescriptor &arg) const;
  int CaSystem(void) { return caSystem; }
  int Stream(void) { return stream; }
  int Length(void) const { return length; }
  const uchar *Data(void) const { return data; }
  };


// --- cCaDescriptors --------------------------------------------------------

class cCaDescriptors : public cListObject {
private:
  int source;
  int transponder;
  int serviceId;
  int numCaIds;
  int caIds[MAXCAIDS + 1];
  cList<cCaDescriptor> caDescriptors;
  void AddCaId(int CaId);
public:
  cCaDescriptors(int Source, int Transponder, int ServiceId);
  bool operator== (const cCaDescriptors &arg) const;
  bool Is(int Source, int Transponder, int ServiceId);
  bool Is(cCaDescriptors * CaDescriptors);
  bool Empty(void) { return caDescriptors.Count() == 0; }
  void AddCaDescriptor(SI::CaDescriptor *d, bool Stream);
  int GetCaDescriptors(const int *CaSystemIds, int BufSize, uchar *Data, bool &StreamFlag);
  const int *CaIds(void) { return caIds; }
  };


// --- cCaDescriptorHandler --------------------------------------------------

class cCaDescriptorHandler : public cList<cCaDescriptors> {
private:
  cMutex mutex;
public:
  int AddCaDescriptors(cCaDescriptors *CaDescriptors);
      // Returns 0 if this is an already known descriptor,
      // 1 if it is an all new descriptor with actual contents,
      // and 2 if an existing descriptor was changed.
  int GetCaDescriptors(int Source, int Transponder, int ServiceId, const int *CaSystemIds, int BufSize, uchar *Data, bool &StreamFlag);
  };

extern cCaDescriptorHandler CaDescriptorHandler;

#endif
