/*
 * Frontend Status Monitor plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include "femontools.h"
#include "femonaac.h"

#define IS_HEAAC_AUDIO(buf) (((buf)[0] == 0xFF) && (((buf)[1] & 0xF6) == 0xF0))

unsigned int cFemonAAC::s_Samplerates[16] =
{
  96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025, 8000, -1, -1, -1, -1
};

cFemonAAC::cFemonAAC(cFemonAudioIf *audiohandler)
: m_AudioHandler(audiohandler)
{
}

cFemonAAC::~cFemonAAC()
{
}

bool cFemonAAC::processAudio(const uint8_t *buf, int len)
{
  cBitStream bs(buf, len * 8);

  if (!m_AudioHandler)
     return false;

  /* ADTS Fixed Header:
   * syncword                 12b  always: '111111111111'
   * id                        1b  0: MPEG-4, 1: MPEG-2
   * layer                     2b  always: '00'
   * protection_absent         1b
   * profile                   2b  0: Main profile AAC MAIN 1: Low Complexity profile (LC) AAC LC 2: Scalable Sample Rate profile (SSR) AAC SSR 3: (reserved) AAC LTP
   * sampling_frequency_index  4b
   * private_bit               1b
   * channel_configuration     3b
   * original/copy             1b
   * home                      1b
   * emphasis                  2b only if ID == 0 (ie MPEG-4)
   */

  // skip PES header
  if (!PesLongEnough(len))
      return false;
  bs.skipBits(8 * PesPayloadOffset(buf));

  // HE-AAC audio detection
  if (bs.getBits(12) != 0xFFF)                  // syncword
     return false;

  bs.skipBit();                                 // id

  // layer must be 0
  if (bs.getBits(2))                            // layer
      return false;

  bs.skipBit();                                 // protection_absent
  bs.skipBits(2);                               // profile
  int sampling_frequency_index = bs.getBits(4); // sampling_frequency_index
  bs.skipBit();                                 // private pid
  int channel_configuration = bs.getBits(3);    // channel_configuration

  m_AudioHandler->SetAudioCodec(AUDIO_CODEC_HEAAC);
  m_AudioHandler->SetAudioBitrate(AUDIO_BITRATE_RESERVED);

  switch (channel_configuration) {
    case 0:
         m_AudioHandler->SetAudioChannel(AUDIO_CHANNEL_MODE_STEREO);
         break;

    case 1:
         m_AudioHandler->SetAudioChannel(AUDIO_CHANNEL_MODE_JOINT_STEREO);
         break;

    case 2:
         m_AudioHandler->SetAudioChannel(AUDIO_CHANNEL_MODE_DUAL);
         break;

    case 3:
         m_AudioHandler->SetAudioChannel(AUDIO_CHANNEL_MODE_SINGLE);
         break;

    default:
         m_AudioHandler->SetAudioChannel(AUDIO_CHANNEL_MODE_INVALID);
         break;
  }

  switch (sampling_frequency_index) {
    case 0xC ... 0xF:
         m_AudioHandler->SetAudioSamplingFrequency(AUDIO_SAMPLING_FREQUENCY_RESERVED);
         break;

    default:
         m_AudioHandler->SetAudioSamplingFrequency(s_Samplerates[sampling_frequency_index]);
         break;
  }

  return true;
}
