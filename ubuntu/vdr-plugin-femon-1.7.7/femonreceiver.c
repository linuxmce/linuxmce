/*
 * Frontend Status Monitor plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <unistd.h>
#include "femontools.h"
#include "femoncfg.h"
#include "femonreceiver.h"

cFemonReceiver::cFemonReceiver(int Vtype, int Vpid, int Apid, int Dpid)
: cThread("femon receiver"),
  m_Mutex(),
  m_Sleep(),
  m_Active(false),
  m_DetectH264(this),
  m_DetectMPEG(this, this),
  m_DetectAAC(this),
  m_DetectAC3(this),
  m_VideoBuffer(KILOBYTE(512), TS_SIZE, false, "Femon video"),
  m_VideoType(Vtype),
  m_VideoPid(Vpid),
  m_VideoPacketCount(0),
  m_VideoBitrate(0.0),
  m_VideoValid(false),
  m_AudioBuffer(KILOBYTE(256), TS_SIZE, false, "Femon audio"),
  m_AudioPid(Apid),
  m_AudioPacketCount(0),
  m_AudioBitrate(0.0),
  m_AudioValid(false),
  m_AC3Buffer(KILOBYTE(256), TS_SIZE, false, "Femon AC3"),
  m_AC3Pid(Dpid),
  m_AC3PacketCount(0),
  m_AC3Bitrate(0),
  m_AC3Valid(false)
{
  debug("%s()\n", __PRETTY_FUNCTION__);

  AddPid(m_VideoPid);
  AddPid(m_AudioPid);
  AddPid(m_AC3Pid);

  m_VideoBuffer.SetTimeouts(0, 100);
  m_AudioBuffer.SetTimeouts(0, 100);
  m_AC3Buffer.SetTimeouts(0, 100);

  m_VideoInfo.codec = VIDEO_CODEC_INVALID;
  m_VideoInfo.format = VIDEO_FORMAT_INVALID;
  m_VideoInfo.scan = VIDEO_SCAN_INVALID;
  m_VideoInfo.aspectRatio = VIDEO_ASPECT_RATIO_INVALID;
  m_VideoInfo.width = 0;
  m_VideoInfo.height = 0;
  m_VideoInfo.frameRate = 0;
  m_VideoInfo.bitrate = AUDIO_BITRATE_INVALID;
  m_AudioInfo.codec = AUDIO_CODEC_UNKNOWN;
  m_AudioInfo.bitrate = AUDIO_BITRATE_INVALID;
  m_AudioInfo.samplingFrequency = AUDIO_SAMPLING_FREQUENCY_INVALID;
  m_AudioInfo.channelMode = AUDIO_CHANNEL_MODE_INVALID;
  m_AC3Info.bitrate = AUDIO_BITRATE_INVALID;
  m_AC3Info.samplingFrequency = AUDIO_SAMPLING_FREQUENCY_INVALID;
  m_AC3Info.bitstreamMode = AUDIO_BITSTREAM_MODE_INVALID;
  m_AC3Info.audioCodingMode = AUDIO_CODING_MODE_INVALID;
  m_AC3Info.dolbySurroundMode = AUDIO_DOLBY_SURROUND_MODE_INVALID;
  m_AC3Info.centerMixLevel = AUDIO_CENTER_MIX_LEVEL_INVALID;
  m_AC3Info.surroundMixLevel = AUDIO_SURROUND_MIX_LEVEL_INVALID;
  m_AC3Info.dialogLevel = 0;
  m_AC3Info.lfe = false;
}

cFemonReceiver::~cFemonReceiver(void)
{
  debug("%s()\n", __PRETTY_FUNCTION__);
  Deactivate();
}

void cFemonReceiver::Deactivate(void)
{
  debug("%s()\n", __PRETTY_FUNCTION__);
  if (m_Active) {
     m_Active = false;
     m_Sleep.Signal();
     if (Running())
        Cancel(3);
     Detach();
     }
}

void cFemonReceiver::Activate(bool On)
{
  debug("%s(%d)\n", __PRETTY_FUNCTION__, On);
  if (On)
     Start();
  else
     Deactivate();
}

void cFemonReceiver::Receive(uchar *Data, int Length)
{
  // TS packet length: TS_SIZE
  if (Running() && (*Data == TS_SYNC_BYTE) && (Length == TS_SIZE)) {
     int len, pid = TsPid(Data);
     if (pid == m_VideoPid) {
        ++m_VideoPacketCount;
        len = m_VideoBuffer.Put(Data, Length);
        if (len != Length) {
           m_VideoBuffer.ReportOverflow(Length - len);
           m_VideoBuffer.Clear();
           }
        }
     else if (pid == m_AudioPid) {
        ++m_AudioPacketCount;
        len = m_AudioBuffer.Put(Data, Length);
        if (len != Length) {
           m_AudioBuffer.ReportOverflow(Length - len);
           m_AudioBuffer.Clear();
           }
        }
     else if (pid == m_AC3Pid) {
        ++m_AC3PacketCount;
        len = m_AC3Buffer.Put(Data, Length);
        if (len != Length) {
           m_AC3Buffer.ReportOverflow(Length - len);
           m_AC3Buffer.Clear();
           }
        }
     }
}

void cFemonReceiver::Action(void)
{
  debug("%s()\n", __PRETTY_FUNCTION__);
  cTimeMs calcPeriod(0);
  m_Active = true;

  while (Running() && m_Active) {
    uint8_t *Data;
    double timeout;
    int len, Length;
    bool processed = false;

    // process available video data
    while (Data = m_VideoBuffer.Get(Length)) {
      if (!m_Active || (Length < TS_SIZE))
         break;
      Length = TS_SIZE;      
      if (*Data != TS_SYNC_BYTE) {
         for (int i = 1; i < Length; ++i) {
             if (Data[i] == TS_SYNC_BYTE) {
                Length = i;
                break;
                }
             }
         m_VideoBuffer.Del(Length);
         continue;
         }
      processed = true;
      if (TsPayloadStart(Data)) {
         while (const uint8_t *p = m_VideoAssembler.GetPes(len)) {
           if (m_VideoType == 0x1B) { // MPEG4
              if (m_DetectH264.processVideo(p, len)) {
                 m_VideoValid = true;
                 break;
                 }
              }
           else {
              if (m_DetectMPEG.processVideo(p, len)) {
                 m_VideoValid = true;
                 break;
                 }
              }
           }
         m_VideoAssembler.Reset();
         }
      m_VideoAssembler.PutTs(Data, Length);
      m_VideoBuffer.Del(Length);
      }

    // process available audio data
    while (Data = m_AudioBuffer.Get(Length)) {
      if (!m_Active || (Length < TS_SIZE))
         break;
      Length = TS_SIZE;      
      if (*Data != TS_SYNC_BYTE) {
         for (int i = 1; i < Length; ++i) {
             if (Data[i] == TS_SYNC_BYTE) {
                Length = i;
                break;
                }
             }
         m_AudioBuffer.Del(Length);
         continue;
         }
      processed = true;
      if (const uint8_t *p = m_AudioAssembler.GetPes(len)) {
         if (m_DetectAAC.processAudio(p, len) || m_DetectMPEG.processAudio(p, len))
            m_AudioValid = true;
         m_AudioAssembler.Reset();
         }
      m_AudioAssembler.PutTs(Data, Length);
      m_AudioBuffer.Del(Length);
      }

    // process available dolby data
    while (Data = m_AC3Buffer.Get(Length)) {
      if (!m_Active || (Length < TS_SIZE))
         break;
      Length = TS_SIZE;      
      if (*Data != TS_SYNC_BYTE) {
         for (int i = 1; i < Length; ++i) {
             if (Data[i] == TS_SYNC_BYTE) {
                Length = i;
                break;
                }
             }
         m_AC3Buffer.Del(Length);
         continue;
         }
      processed = true;
      if (const uint8_t *p = m_AC3Assembler.GetPes(len)) {
         if (m_DetectAC3.processAudio(p, len))
            m_AC3Valid = true;
         m_AC3Assembler.Reset();
         }
      m_AC3Assembler.PutTs(Data, Length);
      m_AC3Buffer.Del(Length);
      }

    // calculate bitrates
    timeout = double(calcPeriod.Elapsed());
    if (m_Active && (timeout >= (100.0 * femonConfig.calcinterval))) {
       // TS packet 188 bytes - 4 byte header; MPEG standard defines 1Mbit = 1000000bit
       // PES headers should be compensated!
       m_VideoBitrate     = (1000.0 * 8.0 * 184.0 * m_VideoPacketCount) / timeout;
       m_VideoPacketCount = 0;
       m_AudioBitrate     = (1000.0 * 8.0 * 184.0 * m_AudioPacketCount) / timeout;
       m_AudioPacketCount = 0;
       m_AC3Bitrate       = (1000.0 * 8.0 * 184.0 * m_AC3PacketCount)   / timeout;
       m_AC3PacketCount   = 0;
       calcPeriod.Set(0);
       }

    if (!processed)
       m_Sleep.Wait(10); // to avoid busy loop and reduce cpu load
    }
}
