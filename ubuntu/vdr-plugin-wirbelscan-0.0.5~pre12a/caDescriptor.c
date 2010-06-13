/*

these are 1:1 copies of the three classes from <vdr/pat.c> :
	cCaDescriptor 
	cCaDescriptors
	cCaDescriptorHandler

copyright: see original file pat.c (vdr-1.6.0, http://www.cadsoft.de/vdr/)

*/


#include "caDescriptor.h"


// --- cCaDescriptor ---------------------------------------------------------


cCaDescriptor::cCaDescriptor(int CaSystem, int CaPid, bool Stream, int Length, const uchar *Data)
{
  caSystem = CaSystem;
  stream = Stream;
  length = Length + 6;
  data = MALLOC(uchar, length);
  data[0] = SI::CaDescriptorTag;
  data[1] = length - 2;
  data[2] = (caSystem >> 8) & 0xFF;
  data[3] =  caSystem       & 0xFF;
  data[4] = ((CaPid   >> 8) & 0x1F) | 0xE0;
  data[5] =   CaPid         & 0xFF;
  if (Length)
     memcpy(&data[6], Data, Length);
}

cCaDescriptor::~cCaDescriptor()
{
  free(data);
}

bool cCaDescriptor::operator== (const cCaDescriptor &arg) const
{
  return length == arg.length && memcmp(data, arg.data, length) == 0;
}

// --- cCaDescriptors --------------------------------------------------------

cCaDescriptors::cCaDescriptors(int Source, int Transponder, int ServiceId)
{
  source = Source;
  transponder = Transponder;
  serviceId = ServiceId;
  numCaIds = 0;
  caIds[0] = 0;
}

bool cCaDescriptors::operator== (const cCaDescriptors &arg) const
{
  cCaDescriptor *ca1 = caDescriptors.First();
  cCaDescriptor *ca2 = arg.caDescriptors.First();
  while (ca1 && ca2) {
        if (!(*ca1 == *ca2))
           return false;
        ca1 = caDescriptors.Next(ca1);
        ca2 = arg.caDescriptors.Next(ca2);
        }
  return !ca1 && !ca2;
}

bool cCaDescriptors::Is(int Source, int Transponder, int ServiceId)
{
  return source == Source && transponder == Transponder && serviceId == ServiceId;
}

bool cCaDescriptors::Is(cCaDescriptors *CaDescriptors)
{
  return Is(CaDescriptors->source, CaDescriptors->transponder, CaDescriptors->serviceId);
}

void cCaDescriptors::AddCaId(int CaId)
{
  if (numCaIds < MAXCAIDS) {
     for (int i = 0; i < numCaIds; i++) {
         if (caIds[i] == CaId)
            return;
         }
     caIds[numCaIds++] = CaId;
     caIds[numCaIds] = 0;
     }
}

void cCaDescriptors::AddCaDescriptor(SI::CaDescriptor *d, bool Stream)
{
  cCaDescriptor *nca = new cCaDescriptor(d->getCaType(), d->getCaPid(), Stream, d->privateData.getLength(), d->privateData.getData());
  for (cCaDescriptor *ca = caDescriptors.First(); ca; ca = caDescriptors.Next(ca)) {
      if (*ca == *nca) {
         delete nca;
         return;
         }
      }
  AddCaId(nca->CaSystem());
  caDescriptors.Add(nca);
//#define DEBUG_CA_DESCRIPTORS 1
#ifdef DEBUG_CA_DESCRIPTORS
  char buffer[1024];
  char *q = buffer;
  q += sprintf(q, "CAM: %04X %5d %5d %04X %d -", source, transponder, serviceId, d->getCaType(), Stream);
  for (int i = 0; i < nca->Length(); i++)
      q += sprintf(q, " %02X", nca->Data()[i]);
  dsyslog(buffer);
#endif
}

int cCaDescriptors::GetCaDescriptors(const int *CaSystemIds, int BufSize, uchar *Data, bool &StreamFlag)
{
  if (!CaSystemIds || !*CaSystemIds)
     return 0;
  if (BufSize > 0 && Data) {
     int length = 0;
     int IsStream = -1;
     for (cCaDescriptor *d = caDescriptors.First(); d; d = caDescriptors.Next(d)) {
         const int *caids = CaSystemIds;
         do {
            if (d->CaSystem() == *caids) {
               if (length + d->Length() <= BufSize) {
                  if (IsStream >= 0 && IsStream != d->Stream())
                     dsyslog("CAM: different stream flag in CA descriptors");
                  IsStream = d->Stream();
                  memcpy(Data + length, d->Data(), d->Length());
                  length += d->Length();
                  }
               else
                  return -1;
               }
            } while (*++caids);
         }
     StreamFlag = IsStream == 1;
     return length;
     }
  return -1;
}

// --- cCaDescriptorHandler --------------------------------------------------

int cCaDescriptorHandler::AddCaDescriptors(cCaDescriptors *CaDescriptors)
{
  cMutexLock MutexLock(&mutex);
  for (cCaDescriptors *ca = First(); ca; ca = Next(ca)) {
      if (ca->Is(CaDescriptors)) {
         if (*ca == *CaDescriptors) {
            delete CaDescriptors;
            return 0;
            }
         Del(ca);
         Add(CaDescriptors);
         return 2;
         }
      }
  Add(CaDescriptors);
  return CaDescriptors->Empty() ? 0 : 1;
}

int cCaDescriptorHandler::GetCaDescriptors(int Source, int Transponder, int ServiceId, const int *CaSystemIds, int BufSize, uchar *Data, bool &StreamFlag)
{
  cMutexLock MutexLock(&mutex);
  StreamFlag = false;
  for (cCaDescriptors *ca = First(); ca; ca = Next(ca)) {
      if (ca->Is(Source, Transponder, ServiceId))
         return ca->GetCaDescriptors(CaSystemIds, BufSize, Data, StreamFlag);
      }
  return 0;
}

cCaDescriptorHandler CaDescriptorHandler;

int GetCaDescriptors(int Source, int Transponder, int ServiceId, const int *CaSystemIds, int BufSize, uchar *Data, bool &StreamFlag)
{
  return CaDescriptorHandler.GetCaDescriptors(Source, Transponder, ServiceId, CaSystemIds, BufSize, Data, StreamFlag);
}
