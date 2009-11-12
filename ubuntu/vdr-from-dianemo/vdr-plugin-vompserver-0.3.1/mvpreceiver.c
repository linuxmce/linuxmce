#include "mvpreceiver.h"

int MVPReceiver::numMVPReceivers = 0;

MVPReceiver* MVPReceiver::create(cChannel* channel, int priority)
{
#if VDRVERSNUM < 10500
  bool NeedsDetachReceivers;
  cDevice* device = cDevice::GetDevice(channel, priority, &NeedsDetachReceivers);
#else
  cDevice* device = cDevice::GetDevice(channel, priority, true); // last param is live-view
#endif

  if (!device)
  {
    Log::getInstance()->log("MVPReceiver", Log::INFO, "No device found to receive this channel at this priority");
    return NULL;
  }

#if VDRVERSNUM < 10500
  if (NeedsDetachReceivers)
  {
    Log::getInstance()->log("MVPReceiver", Log::WARN, "Needs detach receivers");

    // Need to detach other receivers or VDR will shut down??
  }
#endif

  MVPReceiver* m = new MVPReceiver(channel, device);

  numMVPReceivers++;
  Log::getInstance()->log("MVPReceiver", Log::DEBUG, "num mvp receivers now up to %i", numMVPReceivers);
  
  return m;
}

MVPReceiver::MVPReceiver(cChannel* channel, cDevice* device)
#if VDRVERSNUM < 10300
: cReceiver(channel->Ca(), 0, 7, channel->Vpid(), channel->Ppid(), channel->Apid1(), channel->Apid2(), channel->Dpid1(), channel->Dpid2(), channel->Tpid())
#elif VDRVERSNUM < 10500
: cReceiver(channel->Ca(), 0, channel->Vpid(), channel->Apids(), channel->Dpids(), mergeSpidsTpid(channel->Spids(),channel->Tpid()))
#else
: cReceiver(channel->GetChannelID(), 0, channel->Vpid(), channel->Apids(), channel->Dpids(), mergeSpidsTpid(channel->Spids(),channel->Tpid()))
#endif
{
  logger = Log::getInstance();
  vdrActivated = false;
  inittedOK = 0;
  streamID = 0;
  tcp = NULL;

//  logger->log("MVPReceiver", Log::DEBUG, "Channel has VPID %i APID %i", channel->Vpid(), channel->Apid(0));

  if (!processed.init(1000000)) return;
  pthread_mutex_init(&processedRingLock, NULL);

  // OK
  
  // Detect whether this is video or radio and set an appropriate stream chunk size
  // 50k for video, 5k for radio
  // Perhaps move this client side?
  if (channel->Vpid()) streamChunkSize = 50000;
  else streamChunkSize = 5000;
  
  inittedOK = 1;
  device->SwitchChannel(channel, false);
  device->AttachReceiver(this);
}

int MVPReceiver::init(TCP* ttcp, ULONG tstreamID)
{
  tcp = ttcp;
  streamID = tstreamID;
  return inittedOK;
}

MVPReceiver::~MVPReceiver()
{
  Detach();
  threadStop();

  numMVPReceivers--;
  Log::getInstance()->log("MVPReceiver", Log::DEBUG, "num mvp receivers now down to %i", numMVPReceivers);
}

void MVPReceiver::Activate(bool on)
{
  vdrActivated = on;
  if (on) 
  {
    logger->log("MVPReceiver", Log::DEBUG, "VDR active");
    threadStart();
  }
  else
  {
    logger->log("MVPReceiver", Log::DEBUG, "VDR inactive, sending stream end message");
    threadStop();
    sendStreamEnd();
  }
}

bool MVPReceiver::isVdrActivated()
{
  return vdrActivated;
}

void MVPReceiver::Receive(UCHAR* data, int length)
{
  pthread_mutex_lock(&processedRingLock);
  processed.put(data, length);
  if (processed.getContent() > streamChunkSize) threadSignal();
  pthread_mutex_unlock(&processedRingLock);
}

void MVPReceiver::threadMethod()
{
  ULONG headerLength = sizeof(ULONG) * 4;
  UCHAR buffer[streamChunkSize + headerLength];
  int amountReceived;

//   threadSetKillable(); ??

  while(1)
  {
    threadLock();
    threadWaitForSignal();
    threadUnlock();
    threadCheckExit();
    
    do
    {
      pthread_mutex_lock(&processedRingLock);
      amountReceived = processed.get(buffer+headerLength, streamChunkSize);
      pthread_mutex_unlock(&processedRingLock);
    
      *(ULONG*)&buffer[0] = htonl(2); // stream channel
      *(ULONG*)&buffer[4] = htonl(streamID);
      *(ULONG*)&buffer[8] = htonl(0); // here insert flag: 0 = ok, data follows
      *(ULONG*)&buffer[12] = htonl(amountReceived);

      tcp->sendPacket(buffer, amountReceived + headerLength);
    } while(processed.getContent() >= streamChunkSize);
  }  
}

void MVPReceiver::sendStreamEnd()
{
  ULONG bufferLength = sizeof(ULONG) * 4;
  UCHAR buffer[bufferLength];
  *(ULONG*)&buffer[0] = htonl(2); // stream channel
  *(ULONG*)&buffer[4] = htonl(streamID);
  *(ULONG*)&buffer[8] = htonl(1); // stream end
  *(ULONG*)&buffer[12] = htonl(0); // zero length, no more data
  tcp->sendPacket(buffer, bufferLength);
}


int *MVPReceiver::mergeSpidsTpid(const int *spids,int tpid)
{
  int *destpids;
  const int *runspid=spids;
  for (runspid=spids,destpids=mergedSpidsTpid;*runspid;runspid++,destpids++) {
       *destpids=*runspid;
  }
  *destpids=tpid;
  destpids++;
  *destpids=0;
  return mergedSpidsTpid;
}  
