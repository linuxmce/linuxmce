/*
 * Frontend Status Monitor plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __FEMONRECEIVER_H
#define __FEMONRECEIVER_H

#include <vdr/thread.h>
#include <vdr/receiver.h>

#include "femonh264.h"
#include "femonmpeg.h"
#include "femonaac.h"
#include "femonac3.h"
#include "femonaudio.h"
#include "femonvideo.h"
#include "femontools.h"

class cFemonReceiver : public cReceiver, public cThread, public cFemonVideoIf, public cFemonAudioIf, public cFemonAC3If {
private:
  cMutex            m_Mutex;
  cCondWait         m_Sleep;
  bool              m_Active;

  cFemonH264        m_DetectH264;
  cFemonMPEG        m_DetectMPEG;
  cFemonAAC         m_DetectAAC;
  cFemonAC3         m_DetectAC3;

  cRingBufferLinear m_VideoBuffer;
  cTsToPes          m_VideoAssembler;
  int               m_VideoType;
  int               m_VideoPid;
  int               m_VideoPacketCount;
  double            m_VideoBitrate;
  bool              m_VideoValid;
  video_info_t      m_VideoInfo;

  cRingBufferLinear m_AudioBuffer;
  cTsToPes          m_AudioAssembler;
  int               m_AudioPid;
  int               m_AudioPacketCount;
  double            m_AudioBitrate;
  bool              m_AudioValid;
  audio_info_t      m_AudioInfo;

  cRingBufferLinear m_AC3Buffer;
  cTsToPes          m_AC3Assembler;
  int               m_AC3Pid;
  int               m_AC3PacketCount;
  double            m_AC3Bitrate;
  bool              m_AC3Valid;
  ac3_info_t        m_AC3Info;

protected:
  virtual void Activate(bool On);
  virtual void Receive(uchar *Data, int Length);
  virtual void Action(void);

public:
  virtual void SetVideoCodec(eVideoCodec codec)                   { cMutexLock MutexLock(&m_Mutex);
                                                                    m_VideoInfo.codec = codec; }
  virtual void SetVideoFormat(eVideoFormat format)                { cMutexLock MutexLock(&m_Mutex);
                                                                    m_VideoInfo.format = format; }
  virtual void SetVideoScan(eVideoScan scan)                      { cMutexLock MutexLock(&m_Mutex);
                                                                    m_VideoInfo.scan = scan; }
  virtual void SetVideoAspectRatio(eVideoAspectRatio aspectratio) { cMutexLock MutexLock(&m_Mutex);
                                                                    m_VideoInfo.aspectRatio = aspectratio; }
  virtual void SetVideoSize(int width, int height)                { cMutexLock MutexLock(&m_Mutex);
                                                                    m_VideoInfo.width  = width;
                                                                    m_VideoInfo.height = height; }
  virtual void SetVideoFramerate(double framerate)                { cMutexLock MutexLock(&m_Mutex);
                                                                    m_VideoInfo.frameRate = framerate; }
  virtual void SetVideoBitrate(double bitrate)                    { cMutexLock MutexLock(&m_Mutex);
                                                                    m_VideoInfo.bitrate = bitrate; }

  virtual void SetAudioCodec(eAudioCodec codec)        { cMutexLock MutexLock(&m_Mutex);
                                                         m_AudioInfo.codec = codec; }
  virtual void SetAudioBitrate(double bitrate)         { cMutexLock MutexLock(&m_Mutex);
                                                         m_AudioInfo.bitrate = bitrate; }
  virtual void SetAudioSamplingFrequency(int sampling) { cMutexLock MutexLock(&m_Mutex);
                                                         m_AudioInfo.samplingFrequency = sampling; }
  virtual void SetAudioChannel(eAudioChannelMode mode) { cMutexLock MutexLock(&m_Mutex);
                                                         m_AudioInfo.channelMode = mode; }

  virtual void SetAC3Bitrate(int bitrate)            { cMutexLock MutexLock(&m_Mutex);
                                                       m_AC3Info.bitrate = bitrate; }
  virtual void SetAC3SamplingFrequency(int sampling) { cMutexLock MutexLock(&m_Mutex);
                                                       m_AC3Info.samplingFrequency = sampling; }
  virtual void SetAC3Bitstream(int mode)             { cMutexLock MutexLock(&m_Mutex);
                                                       m_AC3Info.bitstreamMode = mode; }
  virtual void SetAC3AudioCoding(int mode)           { cMutexLock MutexLock(&m_Mutex);
                                                       m_AC3Info.audioCodingMode = mode; }
  virtual void SetAC3DolbySurround(int mode)         { cMutexLock MutexLock(&m_Mutex);
                                                       m_AC3Info.dolbySurroundMode = mode; }
  virtual void SetAC3CenterMix(int level)            { cMutexLock MutexLock(&m_Mutex);
                                                       m_AC3Info.centerMixLevel = level; }
  virtual void SetAC3SurroundMix(int level)          { cMutexLock MutexLock(&m_Mutex);
                                                       m_AC3Info.surroundMixLevel = level; }
  virtual void SetAC3Dialog(int level)               { cMutexLock MutexLock(&m_Mutex);
                                                       m_AC3Info.dialogLevel = level; }
  virtual void SetAC3LFE(bool onoff)                 { cMutexLock MutexLock(&m_Mutex);
                                                       m_AC3Info.lfe = onoff; }

public:
  cFemonReceiver(int Vtype, int Vpid, int Apid, int Dpid);
  virtual ~cFemonReceiver();
  void Deactivate(void);

  bool   VideoValid(void)           { cMutexLock MutexLock(&m_Mutex);
                                      return m_VideoValid; };                  // boolean
  double VideoBitrate(void)         { cMutexLock MutexLock(&m_Mutex);
                                      return m_VideoBitrate; };                // bit/s
  int    VideoCodec(void)           { cMutexLock MutexLock(&m_Mutex);
                                      return m_VideoInfo.codec; };             // eVideoCodec
  int    VideoFormat(void)          { cMutexLock MutexLock(&m_Mutex);
                                      return m_VideoInfo.format; };            // eVideoFormat
  int    VideoScan(void)            { cMutexLock MutexLock(&m_Mutex);
                                      return m_VideoInfo.scan; };              // eVideoScan
  int    VideoAspectRatio(void)     { cMutexLock MutexLock(&m_Mutex);
                                      return m_VideoInfo.aspectRatio; };       // eVideoAspectRatio
  int    VideoHorizontalSize(void)  { cMutexLock MutexLock(&m_Mutex);
                                      return m_VideoInfo.width; };             // pixels
  int    VideoVerticalSize(void)    { cMutexLock MutexLock(&m_Mutex);
                                      return m_VideoInfo.height; };            // pixels
  double VideoFrameRate(void)       { cMutexLock MutexLock(&m_Mutex);
                                      return m_VideoInfo.frameRate; };         // Hz
  double VideoStreamBitrate(void)   { cMutexLock MutexLock(&m_Mutex);
                                      return m_VideoInfo.bitrate; };           // bit/s

  bool   AudioValid(void)           { cMutexLock MutexLock(&m_Mutex);
                                      return m_AudioValid; };                  // boolean
  double AudioBitrate(void)         { cMutexLock MutexLock(&m_Mutex);
                                      return m_AudioBitrate; };                // bit/s
  int    AudioCodec(void)           { cMutexLock MutexLock(&m_Mutex);
                                      return m_AudioInfo.codec; };             // eAudioCodec
  int    AudioChannelMode(void)     { cMutexLock MutexLock(&m_Mutex);
                                      return m_AudioInfo.channelMode; };       // eAudioChannelMode
  double AudioStreamBitrate(void)   { cMutexLock MutexLock(&m_Mutex);
                                      return m_AudioInfo.bitrate; };           // bit/s or eAudioBitrate
  int    AudioSamplingFreq(void)    { cMutexLock MutexLock(&m_Mutex);
                                      return m_AudioInfo.samplingFrequency; }; // Hz or eAudioSamplingFrequency

  bool   AC3Valid(void)             { cMutexLock MutexLock(&m_Mutex);
                                      return m_AC3Valid; };                    // boolean
  double AC3Bitrate(void)           { cMutexLock MutexLock(&m_Mutex);
                                      return m_AC3Bitrate; };                  // bit/s
  double AC3StreamBitrate(void)     { cMutexLock MutexLock(&m_Mutex);
                                      return m_AC3Info.bitrate; };             // bit/s or eAudioBitrate
  int    AC3SamplingFreq(void)      { cMutexLock MutexLock(&m_Mutex);
                                      return m_AC3Info.samplingFrequency; };   // Hz or eAudioSamplingFrequency
  int    AC3BitStreamMode(void)     { cMutexLock MutexLock(&m_Mutex);
                                      return m_AC3Info.bitstreamMode; };       // 0..7 or eAudioBitstreamMode
  int    AC3AudioCodingMode(void)   { cMutexLock MutexLock(&m_Mutex);
                                      return m_AC3Info.audioCodingMode; };     // 0..7 or eAudioCodingMode
  bool   AC3_2_0(void)              { cMutexLock MutexLock(&m_Mutex);
                                      return (m_AC3Info.audioCodingMode == AUDIO_CODING_MODE_2_0); }; // boolean
  bool   AC3_5_1(void)	            { cMutexLock MutexLock(&m_Mutex);
                                      return (m_AC3Info.audioCodingMode == AUDIO_CODING_MODE_3_2); }; // boolean
  int    AC3DolbySurroundMode(void) { cMutexLock MutexLock(&m_Mutex);
                                      return m_AC3Info.dolbySurroundMode; };   // eAudioDolbySurroundMode
  int    AC3CenterMixLevel(void)    { cMutexLock MutexLock(&m_Mutex);
                                      return m_AC3Info.centerMixLevel; };      // eAudioCenterMixLevel
  int    AC3SurroundMixLevel(void)  { cMutexLock MutexLock(&m_Mutex);
                                      return m_AC3Info.surroundMixLevel; };    // eAudioSurroundMixLevel
  int    AC3DialogLevel(void)       { cMutexLock MutexLock(&m_Mutex);
                                      return m_AC3Info.dialogLevel; };         // -dB
  bool   AC3Lfe(void)               { cMutexLock MutexLock(&m_Mutex);
                                      return m_AC3Info.lfe; };                 // boolean
  };

#endif //__FEMONRECEIVER_H

