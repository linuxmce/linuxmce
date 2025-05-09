// -*- Mode: c++ -*-
/**
 *  HDTVRecorder
 *  Copyright (c) 2003-2004 by Brandon Beattie, Doug Larrick, 
 *    Jason Hoos, and Daniel Thor Kristjansson
 *  Device ringbuffer added by John Poet
 *  Distributed as part of MythTV under GPL v2 and later.
 */

#ifndef HDTVRECORDER_H_
#define HDTVRECORDER_H_

#include "dtvrecorder.h"
#include "tsstats.h"

struct AVFormatContext;
struct AVPacket;
class ATSCStreamData;
class ProgramAssociationTable;
class ProgramMapTable;
class VirtualChannelTable;
class MasterGuideTable;

/** \class HDTVRecorder
 *  \brief This is a specialization of DTVRecorder used to 
 *         handle streams from bttv drivers, such as the
 *         vendor drivers for the the HD-2000 and HD-3000.
 *
 *  \sa DTVRecorder, DVBRecorder
 */
class HDTVRecorder : public DTVRecorder
{
    Q_OBJECT
    friend class ATSCStreamData;
    friend class TSPacketProcessor;
  public:
    enum {report_loops = 20000};

    HDTVRecorder(TVRec *rec);
   ~HDTVRecorder();

    void SetOptionsFromProfile(RecordingProfile *profile,
                               const QString &videodev,
                               const QString &audiodev,
                               const QString &vbidev);

    void StartRecording(void);
    void StopRecording(void);

    void Pause(bool clear = false);
    bool IsPaused(void) const;

    void Reset(void);

    bool Open(void);

    void SetStreamData(ATSCStreamData*);
    ATSCStreamData* StreamData(void) { return _atsc_stream_data; }

  public slots:
    void deleteLater(void);

  private:
    void TeardownAll(void);

    int  ProcessData    (const unsigned char *buffer, uint len);
    int  ResyncStream   (const unsigned char *buffer, uint pos, uint len);
    bool ProcessTSPacket(const TSPacket &tspacket);

    static void *boot_ringbuffer(void *);
    void fill_ringbuffer(void);
    int ringbuf_read(unsigned char *buffer, size_t count);

 private slots:
    void WritePAT(ProgramAssociationTable*);
    void WritePMT(ProgramMapTable*);
    void ProcessMGT(const MasterGuideTable*);
    void ProcessVCT(uint, const VirtualChannelTable*);
 private:
    ATSCStreamData* _atsc_stream_data;

    // statistics
    TSStats _ts_stats;
    long long _resync_count;
    size_t loop;

    // Data for managing the device ringbuffer
    struct {
        pthread_t        thread;
        mutable pthread_mutex_t lock;
        mutable pthread_mutex_t lock_stats;

        bool             run;
        bool             eof;
        bool             error;
        bool             request_pause;
        bool             paused;
        size_t           size;
        size_t           used;
        size_t           max_used;
        size_t           avg_used;
        size_t           avg_cnt;
        size_t           dev_read_size;
        size_t           min_read;
        unsigned char  * buffer;
        unsigned char  * readPtr;
        unsigned char  * writePtr;
        unsigned char  * endPtr;
    } ringbuf;
};

#endif
