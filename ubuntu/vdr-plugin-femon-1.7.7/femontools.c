/*
 * Frontend Status Monitor plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/dvb/frontend.h>
#include "femonreceiver.h"
#include "femonosd.h"
#include "femontools.h"

static cString getCA(int value)
{
  // http://www.dvb.org/index.php?id=174
  // http://en.wikipedia.org/wiki/Conditional_access_system
  switch (value) {
    case 0x0000:            return cString::sprintf("%s (%X)", trVDR("Free To Air"), value);  // Reserved
    case 0x0001 ... 0x009F:
    case 0x00A2 ... 0x00FF: return cString::sprintf("%s (%X)", tr("Fixed"),          value); // Standardized systems
    case 0x00A0 ... 0x00A1: return cString::sprintf("%s (%X)", tr("Analog"),         value); // Analog signals
    case 0x0100 ... 0x01FF: return cString::sprintf("%s (%X)", "SECA Mediaguard",    value); // Canal Plus
    case 0x0464:            return cString::sprintf("%s (%X)", "EuroDec",            value); // EuroDec
    case 0x0500 ... 0x05FF: return cString::sprintf("%s (%X)", "Viaccess",           value); // France Telecom
    case 0x0600 ... 0x06FF: return cString::sprintf("%s (%X)", "Irdeto",             value); // Irdeto
    case 0x0900 ... 0x09FF: return cString::sprintf("%s (%X)", "NDS Videoguard",     value); // News Datacom
    case 0x0B00 ... 0x0BFF: return cString::sprintf("%s (%X)", "Conax",              value); // Norwegian Telekom
    case 0x0D00 ... 0x0DFF: return cString::sprintf("%s (%X)", "CryptoWorks",        value); // Philips
    case 0x0E00 ... 0x0EFF: return cString::sprintf("%s (%X)", "PowerVu",            value); // Scientific Atlanta
    case 0x1000:            return cString::sprintf("%s (%X)", "RAS",                value); // Tandberg Television
    case 0x1200 ... 0x12FF: return cString::sprintf("%s (%X)", "NagraVision",        value); // BellVu Express
    case 0x1700 ... 0x17FF: return cString::sprintf("%s (%X)", "BetaCrypt",          value); // BetaTechnik
    case 0x1800 ... 0x18FF: return cString::sprintf("%s (%X)", "NagraVision",        value); // Kudelski SA
    case 0x22F0:            return cString::sprintf("%s (%X)", "Codicrypt",          value); // Scopus Network Technologies
    case 0x2600:            return cString::sprintf("%s (%X)", "BISS",               value); // European Broadcasting Union
    case 0x4347:            return cString::sprintf("%s (%X)", "CryptOn",            value); // CryptOn
    case 0x4800:            return cString::sprintf("%s (%X)", "Accessgate",         value); // Telemann
    case 0x4900:            return cString::sprintf("%s (%X)", "China Crypt",        value); // CryptoWorks
    case 0x4A10:            return cString::sprintf("%s (%X)", "EasyCas",            value); // EasyCas
    case 0x4A20:            return cString::sprintf("%s (%X)", "AlphaCrypt",         value); // AlphaCrypt
    case 0x4A70:            return cString::sprintf("%s (%X)", "DreamCrypt",         value); // Dream Multimedia
    case 0x4A60:            return cString::sprintf("%s (%X)", "SkyCrypt",           value); // @Sky
    case 0x4A61:            return cString::sprintf("%s (%X)", "Neotioncrypt",       value); // Neotion
    case 0x4A62:            return cString::sprintf("%s (%X)", "SkyCrypt",           value); // @Sky
    case 0x4A63:            return cString::sprintf("%s (%X)", "Neotion SHL",        value); // Neotion
    case 0x4A64 ... 0x4A6F: return cString::sprintf("%s (%X)", "SkyCrypt",           value); // @Sky
    case 0x4A80:            return cString::sprintf("%s (%X)", "ThalesCrypt",        value); // TPS
    case 0x4AA1:            return cString::sprintf("%s (%X)", "KeyFly",             value); // SIDSA
    case 0x4ABF:            return cString::sprintf("%s (%X)", "DG-Crypt",           value); // Beijing Compunicate Technology Inc.
    case 0x4AD0 ... 0x4AD1: return cString::sprintf("%s (%X)", "X-Crypt",            value); // XCrypt Inc.
    case 0x4AD4:            return cString::sprintf("%s (%X)", "OmniCrypt",          value); // Widevine Technologies, Inc.
    case 0x4AE0:            return cString::sprintf("%s (%X)", "RossCrypt",          value); // Digi Raum Electronics Co. Ltd.
    case 0x5500:            return cString::sprintf("%s (%X)", "Z-Crypt",            value); // Digi Raum Electronics Co. Ltd.
    case 0x5501:            return cString::sprintf("%s (%X)", "Griffin",            value); // Griffin
    default:                break;
    }
  return cString::sprintf("%X", value);
}

static const char *getUserString(int Value, const tDvbParameterMap *Map)
{
  const tDvbParameterMap *map = Map;
  while (map && map->userValue != -1) {
        if (map->driverValue == Value)
           return map->userString ? trVDR(map->userString) : "---";
        map++;
        }
  return "---";
}

cString getFrontendInfo(int cardIndex)
{
  cString info;
  struct dvb_frontend_info value;
  fe_status_t status;
  uint16_t signal = 0;
  uint16_t snr = 0;
  uint32_t ber = 0;
  uint32_t unc = 0;
  cChannel *channel = Channels.GetByNumber(cDevice::CurrentChannel());
  cString dev = cString::sprintf(FRONTEND_DEVICE, cardIndex, 0);

  int fe = open(dev, O_RDONLY | O_NONBLOCK);
  if (fe < 0)
     return NULL;

  info = cString::sprintf("CARD:%d", cardIndex);

  if (ioctl(fe, FE_GET_INFO, &value) >= 0)
     info = cString::sprintf("%s\nTYPE:%d\nNAME:%s", *info, value.type, value.name);
  if (ioctl(fe, FE_READ_STATUS, &status) >= 0)
     info = cString::sprintf("%s\nSTAT:%02X", *info, status);
  if (ioctl(fe, FE_READ_SIGNAL_STRENGTH, &signal) >= 0)
     info = cString::sprintf("%s\nSGNL:%04X", *info, signal);
  if (ioctl(fe, FE_READ_SNR, &snr) >= 0)
     info = cString::sprintf("%s\nSNRA:%04X", *info, snr);
  if (ioctl(fe, FE_READ_BER, &ber) >= 0)
     info = cString::sprintf("%s\nBERA:%08X", *info, ber);
  if (ioctl(fe, FE_READ_UNCORRECTED_BLOCKS, &unc) >= 0)
     info = cString::sprintf("%s\nUNCB:%08X", *info, unc);
  close(fe);

  if (cFemonOsd::Instance())
     info = cString::sprintf("%s\nVIBR:%.0f\nAUBR:%.0f\nDDBR:%.0f", *info, cFemonOsd::Instance()->GetVideoBitrate(), cFemonOsd::Instance()->GetAudioBitrate(), cFemonOsd::Instance()->GetDolbyBitrate());

  if (channel)
     info = cString::sprintf("%s\nCHAN:%s", *info, *channel->ToText());

  return info;
}

cString getFrontendName(int cardIndex)
{
  struct dvb_frontend_info value;
  cString dev = cString::sprintf(FRONTEND_DEVICE, cardIndex, 0);

  int fe = open(dev, O_RDONLY | O_NONBLOCK);
  if (fe < 0)
     return NULL;
  memset(&value, 0, sizeof(value));
  ioctl(fe, FE_GET_INFO, &value);
  close(fe);

  return (cString::sprintf("%s on device #%d", value.name, cardIndex));
}

cString getFrontendStatus(int cardIndex)
{
  fe_status_t value;
  cString dev = cString::sprintf(FRONTEND_DEVICE, cardIndex, 0);

  int fe = open(dev, O_RDONLY | O_NONBLOCK);
  if (fe < 0)
     return NULL;
  memset(&value, 0, sizeof(value));
  ioctl(fe, FE_READ_STATUS, &value);
  close(fe);

  return (cString::sprintf("Status %s:%s:%s:%s:%s on device #%d", (value & FE_HAS_LOCK) ? "LOCKED" : "-", (value & FE_HAS_SIGNAL) ? "SIGNAL" : "-", (value & FE_HAS_CARRIER) ? "CARRIER" : "-", (value & FE_HAS_VITERBI) ? "VITERBI" : "-", (value & FE_HAS_SYNC) ? "SYNC" : "-", cardIndex));
}

uint16_t getSignal(int cardIndex)
{
  uint16_t value = 0;
  cString dev = cString::sprintf(FRONTEND_DEVICE, cardIndex, 0);

  int fe = open(dev, O_RDONLY | O_NONBLOCK);
  if (fe < 0)
     return (value);
  ioctl(fe, FE_READ_SIGNAL_STRENGTH, &value);
  close(fe);

  return (value);
}

uint16_t getSNR(int cardIndex)
{
  uint16_t value = 0;
  cString dev = cString::sprintf(FRONTEND_DEVICE, cardIndex, 0);

  int fe = open(dev, O_RDONLY | O_NONBLOCK);
  if (fe < 0)
     return (value);
  ioctl(fe, FE_READ_SNR, &value);
  close(fe);

  return (value);
}

uint32_t getBER(int cardIndex)
{
  uint32_t value = 0;
  cString dev = cString::sprintf(FRONTEND_DEVICE, cardIndex, 0);

  int fe = open(dev, O_RDONLY | O_NONBLOCK);
  if (fe < 0)
     return (value);
  ioctl(fe, FE_READ_BER, &value);
  close(fe);

  return (value);
}

uint32_t getUNC(int cardIndex)
{
  uint32_t value = 0;
  cString dev = cString::sprintf(FRONTEND_DEVICE, cardIndex, 0);

  int fe = open(dev, O_RDONLY | O_NONBLOCK);
  if (fe < 0)
     return (value);
  ioctl(fe, FE_READ_UNCORRECTED_BLOCKS, &value);
  close(fe);

  return (value);
}

cString getApids(const cChannel *channel)
{
  int value = 0;
  cString apids = cString::sprintf("%d", channel->Apid(value));
  while (channel->Apid(++value) && (value < MAXAPIDS))
    apids = cString::sprintf("%s, %d", *apids, channel->Apid(value));
  return apids;
}

cString getDpids(const cChannel *channel)
{
  int value = 0;
  cString dpids = cString::sprintf("%d", channel->Dpid(value));
  while (channel->Dpid(++value) && (value < MAXDPIDS))
    dpids = cString::sprintf("%s, %d", *dpids, channel->Dpid(value));
  return dpids;
}

cString getSpids(const cChannel *channel)
{
  int value = 0;
  cString spids = cString::sprintf("%d", channel->Spid(value));
  while (channel->Spid(++value) && (value < MAXSPIDS))
    spids = cString::sprintf("%s, %d", *spids, channel->Spid(value));
  return spids;
}

cString getCAids(const cChannel *channel)
{
  int value = 0;
  cString caids = cString::sprintf("%s", *getCA(channel->Ca(value)));
  while (channel->Ca(++value) && (value < MAXCAIDS))
    caids = cString::sprintf("%s, %s", *caids, *getCA(channel->Ca(value)));
  return caids;
}

cString getVideoStream(int value)
{
  if (value != 0)
     return cString::sprintf("#%d", value);
  return cString::sprintf("---");
}

cString getAudioStream(int value, const cChannel *channel)
{
  int pid = 0;
  if (IS_AUDIO_TRACK(value))
     pid = int(value - ttAudioFirst);
  if (channel && channel->Apid(pid)) {
     if (channel->Alang(pid))
        return cString::sprintf("#%d (%s)", channel->Apid(pid), channel->Alang(pid));
     else
        return cString::sprintf("#%d", channel->Apid(pid));
     }
  return cString::sprintf("---");
}

cString getAC3Stream(int value, const cChannel *channel)
{
  int pid = 0;
  if (IS_DOLBY_TRACK(value))
     pid = int(value - ttDolbyFirst);
  if (channel && channel->Dpid(pid)) {
     if (channel->Dlang(pid))
        return cString::sprintf("#%d (%s)", channel->Dpid(pid), channel->Dlang(pid));
     else
        return cString::sprintf("#%d", channel->Dpid(pid));
     }
  return cString::sprintf("---");
}

cString getVideoCodec(int value)
{
  switch (value) {
    case VIDEO_CODEC_MPEG2: return cString::sprintf("%s", tr("MPEG-2"));
    case VIDEO_CODEC_H264:  return cString::sprintf("%s", tr("H.264"));
    default:                break;
    }
  return cString::sprintf("---");
}

cString getAudioCodec(int value)
{
  switch (value) {
    case AUDIO_CODEC_MPEG1_I:   return cString::sprintf("%s", tr("MPEG-1 Layer I"));
    case AUDIO_CODEC_MPEG1_II:  return cString::sprintf("%s", tr("MPEG-1 Layer II"));
    case AUDIO_CODEC_MPEG1_III: return cString::sprintf("%s", tr("MPEG-1 Layer III"));
    case AUDIO_CODEC_MPEG2_I:   return cString::sprintf("%s", tr("MPEG-2 Layer I"));
    case AUDIO_CODEC_MPEG2_II:  return cString::sprintf("%s", tr("MPEG-2 Layer II"));
    case AUDIO_CODEC_MPEG2_III: return cString::sprintf("%s", tr("MPEG-2 Layer III"));
    case AUDIO_CODEC_HEAAC:     return cString::sprintf("%s", tr("HE-AAC"));
    default:                    break;
    }
  return cString::sprintf("---");
}

cString getAudioChannelMode(int value)
{
  switch (value) {
    case AUDIO_CHANNEL_MODE_STEREO:       return cString::sprintf("%s", tr("stereo"));
    case AUDIO_CHANNEL_MODE_JOINT_STEREO: return cString::sprintf("%s", tr("joint Stereo"));
    case AUDIO_CHANNEL_MODE_DUAL:         return cString::sprintf("%s", tr("dual"));
    case AUDIO_CHANNEL_MODE_SINGLE:       return cString::sprintf("%s", tr("mono"));
    default:                              break;
    }
  return cString::sprintf("---");
}

cString getCoderate(int value)
{
  return cString::sprintf("%s", getUserString(value, CoderateValues));
}

cString getTransmission(int value)
{
  return cString::sprintf("%s", getUserString(value, TransmissionValues));
}

cString getBandwidth(int value)
{
  return cString::sprintf("%s", getUserString(value, BandwidthValues));
}

cString getInversion(int value)
{
  return cString::sprintf("%s", getUserString(value, InversionValues));
}

cString getHierarchy(int value)
{
  return cString::sprintf("%s", getUserString(value, HierarchyValues));
}

cString getGuard(int value)
{
  return cString::sprintf("%s", getUserString(value, GuardValues));
}

cString getModulation(int value)
{
  return cString::sprintf("%s", getUserString(value, ModulationValues));
}

cString getSystem(int value)
{
  return cString::sprintf("%s", getUserString(value, SystemValues));
}

cString getRollOff(int value)
{
  return cString::sprintf("%s", getUserString(value, RollOffValues));
}

cString getResolution(int width, int height, int scan)
{
  if ((width > 0) && (height > 0)) {
     switch (scan) {
       case VIDEO_SCAN_INTERLACED:  return cString::sprintf("%dx%d %s", width, height, tr("interlaced"));
       case VIDEO_SCAN_PROGRESSIVE: return cString::sprintf("%dx%d %s", width, height, tr("progressive"));
       default:                     return cString::sprintf("%dx%d",    width, height);
       }
     }
  return cString::sprintf("---");
}

cString getAspectRatio(int value)
{
  switch (value) {
    case VIDEO_ASPECT_RATIO_RESERVED: return cString::sprintf("%s", tr("reserved"));
    case VIDEO_ASPECT_RATIO_EXTENDED: return cString::sprintf("%s", tr("extended"));
    case VIDEO_ASPECT_RATIO_1_1:      return cString::sprintf("1:1");
    case VIDEO_ASPECT_RATIO_4_3:      return cString::sprintf("4:3");
    case VIDEO_ASPECT_RATIO_16_9:     return cString::sprintf("16:9");
    case VIDEO_ASPECT_RATIO_2_21_1:   return cString::sprintf("2.21:1");
    case VIDEO_ASPECT_RATIO_12_11:    return cString::sprintf("12:11");
    case VIDEO_ASPECT_RATIO_10_11:    return cString::sprintf("10:11");
    case VIDEO_ASPECT_RATIO_16_11:    return cString::sprintf("16:11");
    case VIDEO_ASPECT_RATIO_40_33:    return cString::sprintf("40:33");
    case VIDEO_ASPECT_RATIO_24_11:    return cString::sprintf("24:11");
    case VIDEO_ASPECT_RATIO_20_11:    return cString::sprintf("20:11");
    case VIDEO_ASPECT_RATIO_32_11:    return cString::sprintf("32:11");
    case VIDEO_ASPECT_RATIO_80_33:    return cString::sprintf("80:33");
    case VIDEO_ASPECT_RATIO_18_11:    return cString::sprintf("18:11");
    case VIDEO_ASPECT_RATIO_15_11:    return cString::sprintf("15:11");
    case VIDEO_ASPECT_RATIO_64_33:    return cString::sprintf("64:33");
    case VIDEO_ASPECT_RATIO_160_99:   return cString::sprintf("160:99");
    case VIDEO_ASPECT_RATIO_3_2:      return cString::sprintf("3:2");
    case VIDEO_ASPECT_RATIO_2_1:      return cString::sprintf("2:1");
    default:                          break;
    }
  return cString::sprintf("---");
}

cString getVideoFormat(int value)
{
  switch (value) {
    case VIDEO_FORMAT_UNKNOWN:   return cString::sprintf("%s", tr("unknown"));
    case VIDEO_FORMAT_RESERVED:  return cString::sprintf("%s", tr("reserved"));
    case VIDEO_FORMAT_COMPONENT: return cString::sprintf("%s", tr("component"));
    case VIDEO_FORMAT_PAL:       return cString::sprintf("%s", tr("PAL"));
    case VIDEO_FORMAT_NTSC:      return cString::sprintf("%s", tr("NTSC"));
    case VIDEO_FORMAT_SECAM:     return cString::sprintf("%s", tr("SECAM"));
    case VIDEO_FORMAT_MAC:       return cString::sprintf("%s", tr("MAC"));
    default:                     break;
    }
  return cString::sprintf("---");
}

cString getFrameRate(double value)
{
  if (value > 0)
     return cString::sprintf("%.2f %s", value, tr("Hz"));
  return cString::sprintf("---");
}

cString getAC3BitStreamMode(int value, int coding)
{
  switch (value) {
    case AUDIO_BITSTREAM_MODE_CM:     return cString::sprintf("%s", tr("Complete Main (CM)"));
    case AUDIO_BITSTREAM_MODE_ME:     return cString::sprintf("%s", tr("Music and Effects (ME)"));
    case AUDIO_BITSTREAM_MODE_VI:     return cString::sprintf("%s", tr("Visually Impaired (VI)"));
    case AUDIO_BITSTREAM_MODE_HI:     return cString::sprintf("%s", tr("Hearing Impaired (HI)"));
    case AUDIO_BITSTREAM_MODE_D:      return cString::sprintf("%s", tr("Dialogue (D)"));
    case AUDIO_BITSTREAM_MODE_C:      return cString::sprintf("%s", tr("Commentary (C)"));
    case AUDIO_BITSTREAM_MODE_E:      return cString::sprintf("%s", tr("Emergency (E)"));
    case AUDIO_BITSTREAM_MODE_VO_KAR: return cString::sprintf("%s", (coding == 1) ? tr("Voice Over (VO)") : tr("Karaoke"));
    default:                          break;
    }
  return cString::sprintf("---");
}

cString getAC3AudioCodingMode(int value, int stream)
{
  if (stream != 7) {
     switch (value) {
       case AUDIO_CODING_MODE_1_1: return cString::sprintf("1+1 - %s, %s", tr("Ch1"), tr("Ch2"));
       case AUDIO_CODING_MODE_1_0: return cString::sprintf("1/0 - %s", tr("C"));
       case AUDIO_CODING_MODE_2_0: return cString::sprintf("2/0 - %s, %s", tr("L"), tr("R"));
       case AUDIO_CODING_MODE_3_0: return cString::sprintf("3/0 - %s, %s, %s", tr("L"), tr("C"), tr("R"));
       case AUDIO_CODING_MODE_2_1: return cString::sprintf("2/1 - %s, %s, %s", tr("L"), tr("R"), tr("S"));
       case AUDIO_CODING_MODE_3_1: return cString::sprintf("3/1 - %s, %s, %s, %s", tr("L"), tr("C"), tr("R"), tr("S"));
       case AUDIO_CODING_MODE_2_2: return cString::sprintf("2/2 - %s, %s, %s, %s", tr("L"), tr("R"), tr("SL"), tr("SR"));
       case AUDIO_CODING_MODE_3_2: return cString::sprintf("3/2 - %s, %s, %s, %s, %s", tr("L"), tr("C"), tr("R"), tr("SL"), tr("SR"));
       default:                    break;
       }
     }
  return cString::sprintf("---");
}

cString getAC3CenterMixLevel(int value)
{
  switch (value) {
    case AUDIO_CENTER_MIX_LEVEL_MINUS_3dB:   return cString::sprintf("-3.0 %s", tr("dB"));
    case AUDIO_CENTER_MIX_LEVEL_MINUS_4_5dB: return cString::sprintf("-4.5 %s", tr("dB"));
    case AUDIO_CENTER_MIX_LEVEL_MINUS_6dB:   return cString::sprintf("-6.0 %s", tr("dB"));
    case AUDIO_CENTER_MIX_LEVEL_RESERVED:    return cString::sprintf("%s", tr("reserved"));
    default:                                 break;
    }
  return cString::sprintf("---");
}

cString getAC3SurroundMixLevel(int value)
{
  switch (value) {
    case AUDIO_SURROUND_MIX_LEVEL_MINUS_3dB: return cString::sprintf("-3 %s", tr("dB"));
    case AUDIO_SURROUND_MIX_LEVEL_MINUS_6dB: return cString::sprintf("-6 %s", tr("dB"));
    case AUDIO_SURROUND_MIX_LEVEL_0_dB:      return cString::sprintf("0 %s", tr("dB"));
    case AUDIO_SURROUND_MIX_LEVEL_RESERVED:  return cString::sprintf("%s", tr("reserved"));
    default:                                 break;
    }
  return cString::sprintf("---");
}

cString getAC3DolbySurroundMode(int value)
{
  switch (value) {
    case AUDIO_DOLBY_SURROUND_MODE_NOT_INDICATED:     return cString::sprintf("%s", tr("not indicated"));
    case AUDIO_DOLBY_SURROUND_MODE_NOT_DOLBYSURROUND: return cString::sprintf("%s", trVDR("no"));
    case AUDIO_DOLBY_SURROUND_MODE_DOLBYSURROUND:     return cString::sprintf("%s", trVDR("yes"));
    case AUDIO_DOLBY_SURROUND_MODE_RESERVED:          return cString::sprintf("%s", tr("reserved"));
    default:                                          break;
    }
  return cString::sprintf("---");
}

cString getAC3DialogLevel(int value)
{
  if (value > 0)
     return cString::sprintf("-%d %s", value, tr("dB"));
  return cString::sprintf("---");
}

cString getFrequencyMHz(int value)
{
   while (value > 20000) value /= 1000;
   return cString::sprintf("%d %s", value, tr("MHz"));
}

cString getAudioSamplingFreq(int value)
{
  switch (value) {
    case AUDIO_SAMPLING_FREQUENCY_INVALID:  return cString::sprintf("---");
    case AUDIO_SAMPLING_FREQUENCY_RESERVED: return cString::sprintf("%s", tr("reserved"));
    default:                                break;
    }
  return cString::sprintf("%d %s", value, tr("Hz"));
}

cString getAudioBitrate(double value, double stream)
{
  switch ((int)stream) {
    case AUDIO_BITRATE_INVALID:  return cString::sprintf("---");
    case AUDIO_BITRATE_RESERVED: return cString::sprintf("%s (%s)", tr("reserved"), *getBitrateKbits(value));
    case AUDIO_BITRATE_FREE:     return cString::sprintf("%s (%s)", tr("free"), *getBitrateKbits(value));
    default:                     break;
    }
  return cString::sprintf("%s (%s)", *getBitrateKbits(stream), *getBitrateKbits(value));
}

cString getVideoBitrate(double value, double stream)
{
  return cString::sprintf("%s (%s)", *getBitrateMbits(stream), *getBitrateMbits(value));
}

cString getBitrateMbits(double value)
{
  if (value > 0)
     return cString::sprintf("%.2f %s", value / 1000000.0, tr("Mbit/s"));
  return cString::sprintf("---");
}

cString getBitrateKbits(double value)
{
  if (value > 0)
     return cString::sprintf("%.0f %s", value / 1000.0, tr("kbit/s"));
  return cString::sprintf("---");
}

cBitStream::cBitStream(const uint8_t *buf, const int len)
: data(buf),
  count(len),
  index(0)
{
}

cBitStream::~cBitStream()
{
}

int cBitStream::getBit()
{
  if (index >= count)
     return (1); // -> no infinite colomb's ...

  int r = (data[index >> 3] >> (7 - (index & 7))) & 1;
  ++index;

  return (r);
}

uint32_t cBitStream::getBits(uint32_t n)
{
  uint32_t r = 0;

  while (n--)
    r = (r | (getBit() << n));

  return (r);
}

void cBitStream::skipBits(uint32_t n)
{
  index += n;
}

uint32_t cBitStream::getUeGolomb()
{
  int n = 0;

  while (!getBit() && (n < 32))
    n++;

  return (n ? ((1 << n) - 1) + getBits(n) : 0);
}

int32_t cBitStream::getSeGolomb()
{
  uint32_t r = getUeGolomb() + 1;

  return ((r & 1) ? -(r >> 1) : (r >> 1));
}

void cBitStream::skipGolomb()
{
  int n = 0;

  while (!getBit() && (n < 32))
    n++;

  skipBits(n);
}

void cBitStream::skipUeGolomb()
{
  skipGolomb();
}

void cBitStream::skipSeGolomb()
{
  skipGolomb();
}

void cBitStream::byteAlign()
{
  int n = index % 8;

  if (n > 0)
     skipBits(8 - n);
}
