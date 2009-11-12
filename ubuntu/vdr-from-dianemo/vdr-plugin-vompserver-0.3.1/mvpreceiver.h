/*
    Copyright 2004-2005 Chris Tallon

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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef MVPRECEIVER_H
#define MVPRECEIVER_H

#include <vdr/channels.h>
#include <vdr/device.h>
#include <vdr/receiver.h>

#include "log.h"
#include "thread.h"
#include "ringbuffer.h"
#include "tcp.h"
#include "thread.h"

class MVPReceiver : public cReceiver, public Thread
{
  public:
    static MVPReceiver* create(cChannel*, int priority);
    virtual ~MVPReceiver();
    int init(TCP* tcp, ULONG streamID);
    bool isVdrActivated();

  private:
    MVPReceiver(cChannel* channel, cDevice* device);

    Log* logger;
    bool vdrActivated;
    int inittedOK;
    Ringbuffer processed;    // A simpler deleting ringbuffer for processed data
    pthread_mutex_t processedRingLock; // needs outside locking

    TCP* tcp;
    ULONG streamID;
    ULONG streamDataCollected;
    int streamChunkSize;

    // cReciever stuff
    void Activate(bool On);
    void Receive(UCHAR *Data, int Length);
    void sendStreamEnd();

    static int numMVPReceivers;
    
  protected:
    void threadMethod();
    int *mergeSpidsTpid(const int *spids,int tpid);
    int  mergedSpidsTpid[MAXSPIDS+2];
};

#endif


/*
    cReceiver docs from the header file

    void Activate(bool On);
      // This function is called just before the cReceiver gets attached to
      // (On == true) or detached from (On == false) a cDevice. It can be used
      // to do things like starting/stopping a thread.
      // It is guaranteed that Receive() will not be called before Activate(true).
    void Receive(uchar *Data, int Length);
      // This function is called from the cDevice we are attached to, and
      // delivers one TS packet from the set of PIDs the cReceiver has requested.
      // The data packet must be accepted immediately, and the call must return
      // as soon as possible, without any unnecessary delay. Each TS packet
      // will be delivered only ONCE, so the cReceiver must make sure that
      // it will be able to buffer the data if necessary.

*/

/*

  cDevice docs

(VDR 1.4)
  static cDevice *GetDevice(const cChannel *Channel, int Priority = -1, bool *NeedsDetachReceivers = NULL);
     ///< Returns a device that is able to receive the given Channel at the
     ///< given Priority.
     ///< See ProvidesChannel() for more information on how
     ///< priorities are handled, and the meaning of NeedsDetachReceivers.

(VDR >1.5)
  static cDevice *GetDevice(const cChannel *Channel, int Priority, bool LiveView);
     ///< Returns a device that is able to receive the given Channel at the
     ///< given Priority, with the least impact on active recordings and
     ///< live viewing. The LiveView parameter tells whether the device will
     ///< be used for live viewing or a recording.
     ///< If the Channel is encrypted, a CAM slot that claims to be able to
     ///< decrypt the channel is automatically selected and assigned to the
     ///< returned device. Whether or not this combination of device and CAM
     ///< slot is actually able to decrypt the channel can only be determined
     ///< by checking the "scrambling control" bits of the received TS packets.
     ///< The Action() function automatically does this and takes care that
     ///< after detaching any receivers because the channel can't be decrypted,
     ///< this device/CAM combination will be skipped in the next call to
     ///< GetDevice().
     ///< See also ProvidesChannel().

*/
