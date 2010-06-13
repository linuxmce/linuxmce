/*
 * Frontend Status Monitor plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include "femontools.h"
#include "femonmpeg.h"

#define IS_EXTENSION_START(buf) (((buf)[0] == 0x00) && ((buf)[1] == 0x00) && ((buf)[2] == 0x01) && ((buf)[3] == 0xB5))

unsigned int cFemonMPEG::s_Bitrates[2][3][16] =
{
  {
    {0,  32,  48,  56,  64,  80,  96, 112, 128, 144, 160, 176, 192, 224, 256, -1}, // MPEG-2 Layer I
    {0,   8,  16,  24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160, -1}, // MPEG-2 Layer II/III
    {0,   8,  16,  24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160, -1}  // MPEG-2 Layer II/III
  },
  {
    {0,  32,  64,  96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, -1}, // MPEG-1 Layer I
    {0,  32,  48,  56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, 384, -1}, // MPEG-1 Layer II
    {0,  32,  40,  48,  56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, -1}  // MPEG-1 Layer III
  }
};

unsigned int cFemonMPEG::s_Samplerates[2][4] =
{
  {22050, 24000, 16000, -1}, // MPEG-2
  {44100, 48000, 32000, -1}  // MPEG-1
};

eAudioCodec cFemonMPEG::s_Formats[2][4] =
{
  {AUDIO_CODEC_MPEG2_I, AUDIO_CODEC_MPEG2_II, AUDIO_CODEC_MPEG2_III, AUDIO_CODEC_UNKNOWN}, // MPEG-2
  {AUDIO_CODEC_MPEG1_I, AUDIO_CODEC_MPEG1_II, AUDIO_CODEC_MPEG1_III, AUDIO_CODEC_UNKNOWN}  // MPEG-1
};

cFemonMPEG::cFemonMPEG(cFemonVideoIf *videohandler, cFemonAudioIf *audiohandler)
: m_VideoHandler(videohandler),
  m_AudioHandler(audiohandler)
{
}

cFemonMPEG::~cFemonMPEG()
{
}

bool cFemonMPEG::processAudio(const uint8_t *buf, int len)
{
  cBitStream bs(buf, len * 8);

  if (!m_AudioHandler)
     return false;

  // skip PES header
  if (!PesLongEnough(len))
      return false;
  bs.skipBits(8 * PesPayloadOffset(buf));

  // MPEG audio detection
  if (bs.getBits(12) != 0xFFF)              // syncword
     return false;

  int id = bs.getBit();                     // id: MPEG-2=0, MPEG-1=1
  int layer = 3 - bs.getBits(2);            // layer: I=11, II=10, III=01
  bs.skipBit();                             // protection bit
  int bit_rate_index = bs.getBits(4);       // bitrate index
  int sampling_frequency = bs.getBits(2);   // sampling frequency
  bs.skipBit();                             // padding bit
  bs.skipBit();                             // private pid
  int mode = bs.getBits(2);                 // mode

  m_AudioHandler->SetAudioCodec(s_Formats[id][layer]);

  switch (mode) {
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

  switch (bit_rate_index) {
    case 0:
         m_AudioHandler->SetAudioBitrate(AUDIO_BITRATE_FREE);
         break;

    case 0xF:
         m_AudioHandler->SetAudioBitrate(AUDIO_BITRATE_RESERVED);
         break;

    default:
         m_AudioHandler->SetAudioBitrate(1000 * s_Bitrates[id][layer][bit_rate_index]);
         break;
  }

  switch (sampling_frequency) {
    case 3:
         m_AudioHandler->SetAudioSamplingFrequency(AUDIO_SAMPLING_FREQUENCY_RESERVED);
         break;

    default:
         m_AudioHandler->SetAudioSamplingFrequency(s_Samplerates[id][sampling_frequency]);
         break;
  }

  return true;
}

bool cFemonMPEG::processVideo(const uint8_t *buf, int len)
{
  cBitStream bs(buf, len * 8);

  if (!m_VideoHandler)
     return false;

  // skip PES header
  if (!PesLongEnough(len))
      return false;
  bs.skipBits(8 * PesPayloadOffset(buf));

  // MPEG-2 video detection, search for start code
  if (bs.getU32() != 0x000001B3)            // sequence header
     return false;

  int scan = VIDEO_SCAN_UNKNOWN;
  int format = VIDEO_FORMAT_UNKNOWN;
  int aspect = VIDEO_ASPECT_RATIO_RESERVED;

  int horizontal_size = bs.getBits(12);     // horizontal size value
  int vertical_size = bs.getBits(12);       // vertical size value

  switch (bs.getBits(4)) {                  // aspect ratio information
    case 1:
         aspect = VIDEO_ASPECT_RATIO_1_1;
         break;

    case 2:
         aspect = VIDEO_ASPECT_RATIO_4_3;
         break;

    case 3:
         aspect = VIDEO_ASPECT_RATIO_16_9;
         break;

    case 4:
         aspect = VIDEO_ASPECT_RATIO_2_21_1;
         break;

    case 5 ... 15:
    default:
         aspect = VIDEO_ASPECT_RATIO_RESERVED;
         break;
    }

  double frame_rate = 0;
  switch (bs.getBits(4)) {                  // frame rate code
    case 1:
         frame_rate = 24000 / 1001.0;
         format = VIDEO_FORMAT_UNKNOWN;
         break;

    case 2:
         frame_rate = 24.0;
         format = VIDEO_FORMAT_UNKNOWN;
         break;

    case 3:
         frame_rate = 25.0;
         format = VIDEO_FORMAT_PAL;
         break;

    case 4:
         frame_rate = 30000 / 1001.0;
         format = VIDEO_FORMAT_NTSC;
         break;

    case 5:
         frame_rate = 30.0;
         format = VIDEO_FORMAT_NTSC;
         break;

    case 6:
         frame_rate = 50.0;
         format = VIDEO_FORMAT_PAL;
         break;

    case 7:
         frame_rate = 60.0;
         format = VIDEO_FORMAT_NTSC;
         break;

    case 8:
         frame_rate = 60000 / 1001.0;
         format = VIDEO_FORMAT_NTSC;
         break;

    case 9 ... 15:
    default:
         frame_rate = 0;
         format = VIDEO_FORMAT_UNKNOWN;
         break;
    }

  int bit_rate = bs.getBits(18);            // bit rate value

  bs.skipBit();             // marker bit
  bs.skipBits(10);          // vbv buffer size value
  bs.skipBit();             // constrained parameters value
  if (bs.getBit())          // load intra quantizer matrix
     bs.skipBits(8 * 64);   // intra quantizer matrix
  if (bs.getBit())          // load non-intra quantizer matrix
     bs.skipBits(8 * 64);   // non-intra quantizer matrix

  if (bs.getU32() != 0x000001B5) {                  // extension start
     bs.skipBits(4);                                // extension start code identifier
     bs.skipBits(8);                                // profile and level indicator
     scan = bs.getBit() ? VIDEO_SCAN_PROGRESSIVE :
                          VIDEO_SCAN_INTERLACED;    // progressive sequence
     bs.skipBits(2);                                // chroma format
     horizontal_size |= (bs.getBits(2) << 12);      // horizontal size extension
     vertical_size |= (bs.getBits(2) << 12);        // vertical size extension
     bit_rate |= (bs.getBits(12) << 18);            // bit rate extension
     bs.skipBit();                                  // marker bit
     bs.skipBits(8);                                // vpv buffer size extension
     bs.skipBit();                                  // low delay
     bs.skipBits(2);                                // frame rate extension n
     bs.skipBits(5);                                // frame rate extension d

     if ((bs.getU32() != 0x000001B5) &&             // extension start code
         (bs.getBits(4) == 0x0010)) {               // sequence display extension id
        switch (bs.getBits(3)) {                    // video format
          case 0x000:
               format = VIDEO_FORMAT_COMPONENT;
               break;
          case 0x001:
               format = VIDEO_FORMAT_PAL;
               break;
          case 0x010:
               format = VIDEO_FORMAT_NTSC;
               break;
          case 0x011:
               format = VIDEO_FORMAT_SECAM;
               break;
          case 0x100:
               format = VIDEO_FORMAT_MAC;
               break;
          case 0x101:
               format = VIDEO_FORMAT_UNKNOWN;
               break;
          case 0x110:
          case 0x111:
               format = VIDEO_FORMAT_RESERVED;
               break;
          default:
               format = VIDEO_FORMAT_INVALID;
               break;
          }
        }
     }

  m_VideoHandler->SetVideoCodec(VIDEO_CODEC_MPEG2);
  m_VideoHandler->SetVideoSize(horizontal_size, vertical_size);
  m_VideoHandler->SetVideoBitrate(400.0 * (double)(bit_rate));
  m_VideoHandler->SetVideoFramerate(frame_rate);
  m_VideoHandler->SetVideoScan(eVideoScan(scan));
  m_VideoHandler->SetVideoAspectRatio(eVideoAspectRatio(aspect));
  m_VideoHandler->SetVideoFormat(eVideoFormat(format));

  return true;
}
