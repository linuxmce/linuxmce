/*
 * Frontend Status Monitor plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * AC3 Audio Header: http://www.atsc.org/standards/a_52a.pdf
 */

#include "femontools.h"
#include "femonac3.h"

unsigned int cFemonAC3::s_Bitrates[32] =
{
  32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384, 448, 512, 576, 640, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

unsigned int cFemonAC3::s_Frequencies[4] =
{
  480, 441, 320, 0
};

unsigned int cFemonAC3::s_Frames[3][32] =
{
  {64, 80,  96,  112, 128, 160, 192, 224, 256, 320, 384, 448, 512, 640, 768,  896,  1024, 1152, 1280, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {69, 87,  104, 121, 139, 174, 208, 243, 278, 348, 417, 487, 557, 696, 835,  975,  1114, 1253, 1393, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {96, 120, 144, 168, 192, 240, 288, 336, 384, 480, 576, 672, 768, 960, 1152, 1344, 1536, 1728, 1920, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

cFemonAC3::cFemonAC3(cFemonAC3If *audiohandler)
: m_AudioHandler(audiohandler)
{
}

cFemonAC3::~cFemonAC3()
{
}

bool cFemonAC3::processAudio(const uint8_t *buf, int len)
{
  int fscod, frmsizcod, bsmod, acmod;
  int centermixlevel = AUDIO_CENTER_MIX_LEVEL_INVALID;
  int surroundmixlevel = AUDIO_SURROUND_MIX_LEVEL_INVALID;
  int dolbysurroundmode = AUDIO_DOLBY_SURROUND_MODE_INVALID;
  cBitStream bs(buf, len * 8);

  if (!m_AudioHandler)
     return false;

  // skip PES header
  if (!PesLongEnough(len))
      return false;
  bs.skipBits(8 * PesPayloadOffset(buf));

  // http://rmworkshop.com/dvd_info/related_info/ac3hdr.htm

  // AC3 audio detection
  if (bs.getU16() != 0x0B77)        // syncword
     return false;

  bs.skipBits(16);                  // CRC1

  fscod = bs.getBits(2);            // sampling rate values
  frmsizcod = bs.getBits(6);        // frame size code

  bs.skipBits(5);                   // bitstream id
  bsmod = bs.getBits(3);            // bitstream mode
  acmod = bs.getBits(3);            // audio coding mode

  // 3 front channels
  if ((acmod & 0x01) && (acmod != 0x01))
     centermixlevel = bs.getBits(2);

  // if a surround channel exists
  if (acmod & 0x04)
     surroundmixlevel = bs.getBits(2);

  // if in 2/0 mode
  if (acmod == 0x02)
      dolbysurroundmode = bs.getBits(2);

  m_AudioHandler->SetAC3Bitrate(1000 * s_Bitrates[frmsizcod >> 1]);
  m_AudioHandler->SetAC3SamplingFrequency(100 * s_Frequencies[fscod]);
  m_AudioHandler->SetAC3Bitstream(bsmod);
  m_AudioHandler->SetAC3AudioCoding(acmod);
  m_AudioHandler->SetAC3CenterMix(centermixlevel);
  m_AudioHandler->SetAC3SurroundMix(surroundmixlevel);
  m_AudioHandler->SetAC3DolbySurround(dolbysurroundmode);

  m_AudioHandler->SetAC3LFE(bs.getBit());       // low frequency effects on
  m_AudioHandler->SetAC3Dialog(bs.getBits(5));  // dialog normalization

  return true;
}
