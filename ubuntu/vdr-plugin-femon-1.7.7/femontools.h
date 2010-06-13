/*
 * Frontend Status Monitor plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __FEMONTOOLS_H
#define __FEMONTOOLS_H

#include <stdint.h>
#include <vdr/channels.h>
#include <vdr/remux.h>
#include <vdr/tools.h>

#ifdef DEBUG
#define debug(x...) dsyslog("FEMON: " x);
#define error(x...) esyslog("ERROR: " x);
#else
#define debug(x...) ;
#define error(x...) esyslog("ERROR: " x);
#endif

#define ELEMENTS(x) (sizeof(x) / sizeof(x[0]))

#define FRONTEND_DEVICE "/dev/dvb/adapter%d/frontend%d"

cString getFrontendInfo(int cardIndex = 0);
cString getFrontendName(int cardIndex = 0);
cString getFrontendStatus(int cardIndex = 0);

uint16_t getSNR(int cardIndex = 0);
uint16_t getSignal(int cardIndex = 0);

uint32_t getBER(int cardIndex = 0);
uint32_t getUNC(int cardIndex = 0);

cString getApids(const cChannel *channel);
cString getDpids(const cChannel *channel);
cString getSpids(const cChannel *channel);
cString getCAids(const cChannel *channel);
cString getVideoStream(int value);
cString getVideoCodec(int value);
cString getAudioStream(int value, const cChannel *channel);
cString getAudioCodec(int value);
cString getAudioChannelMode(int value);
cString getCoderate(int value);
cString getTransmission(int value);
cString getBandwidth(int value);
cString getInversion(int value);
cString getHierarchy(int value);
cString getGuard(int value);
cString getModulation(int value);
cString getSystem(int value);
cString getRollOff(int value);
cString getResolution(int width, int height, int scan);
cString getAspectRatio(int value);
cString getVideoFormat(int value);
cString getFrameRate(double value);
cString getAC3Stream(int value, const cChannel *channel);
cString getAC3BitStreamMode(int value, int coding);
cString getAC3AudioCodingMode(int value, int stream);
cString getAC3CenterMixLevel(int value);
cString getAC3SurroundMixLevel(int value);
cString getAC3DolbySurroundMode(int value);
cString getAC3DialogLevel(int value);
cString getFrequencyMHz(int value);
cString getAudioSamplingFreq(int value);
cString getAudioBitrate(double value, double stream);
cString getVideoBitrate(double value, double stream);
cString getBitrateMbits(double value);
cString getBitrateKbits(double value);

class cBitStream {
private:
  const uint8_t *data;
  int            count; // in bits
  int            index; // in bits

public:
  cBitStream(const uint8_t *buf, const int len);
  ~cBitStream();

  int            getBit();
  uint32_t       getBits(uint32_t n);
  void           skipBits(uint32_t n);
  uint32_t       getUeGolomb();
  int32_t        getSeGolomb();
  void           skipGolomb();
  void           skipUeGolomb();
  void           skipSeGolomb();
  void           byteAlign();

  void           skipBit()  { skipBits(1); }
  uint32_t       getU8()    { return getBits(8); }
  uint32_t       getU16()   { return ((getBits(8) << 8) | getBits(8)); }
  uint32_t       getU24()   { return ((getBits(8) << 16) | (getBits(8) << 8) | getBits(8)); }
  uint32_t       getU32()   { return ((getBits(8) << 24) | (getBits(8) << 16) | (getBits(8) << 8) | getBits(8)); }
  bool           isEOF()    { return (index >= count); }
  void           reset()    { index = 0; }
  int            getIndex() { return (isEOF() ? count : index); }
  const uint8_t *getData()  { return (isEOF() ? NULL : data + (index / 8)); }
};

#endif // __FEMONTOOLS_H
