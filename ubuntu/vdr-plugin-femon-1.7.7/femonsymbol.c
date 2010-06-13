/*
 * Frontend Status Monitor plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include "femonsymbol.h"

#include "symbols/stereo.xpm"
#include "symbols/monoleft.xpm"
#include "symbols/monoright.xpm"
#include "symbols/dolbydigital.xpm"
#include "symbols/dolbydigital20.xpm"
#include "symbols/dolbydigital51.xpm"
#include "symbols/mpeg2.xpm"
#include "symbols/h264.xpm"
#include "symbols/ntsc.xpm"
#include "symbols/pal.xpm"
#include "symbols/encrypted.xpm"
#include "symbols/svdrp.xpm"
#include "symbols/lock.xpm"
#include "symbols/signal.xpm"
#include "symbols/carrier.xpm"
#include "symbols/viterbi.xpm"
#include "symbols/sync.xpm"
#include "symbols/ar11.xpm"
#include "symbols/ar169.xpm"
#include "symbols/ar2211.xpm"
#include "symbols/ar43.xpm"
#include "symbols/device.xpm"
#include "symbols/zero.xpm"
#include "symbols/one.xpm"
#include "symbols/two.xpm"
#include "symbols/three.xpm"
#include "symbols/four.xpm"
#include "symbols/five.xpm"
#include "symbols/six.xpm"
#include "symbols/seven.xpm"
#include "symbols/eight.xpm"
#include "symbols/format1080.xpm"
#include "symbols/format720.xpm"
#include "symbols/format576.xpm"
#include "symbols/format480.xpm"

cBitmap bmSymbol[SYMBOL_MAX_COUNT] =
{
  cBitmap(stereo_xpm),         // SYMBOL_STEREO
  cBitmap(monoleft_xpm),       // SYMBOL_MONO_LEFT
  cBitmap(monoright_xpm),      // SYMBOL_MONO_RIGHT
  cBitmap(dolbydigital_xpm),   // SYMBOL_DD
  cBitmap(dolbydigital20_xpm), // SYMBOL_DD20
  cBitmap(dolbydigital51_xpm), // SYMBOL_DD51
  cBitmap(mpeg2_xpm),          // SYMBOL_MPEG2
  cBitmap(h264_xpm),           // SYMBOL_H264
  cBitmap(pal_xpm),            // SYMBOL_PAL
  cBitmap(ntsc_xpm),           // SYMBOL_NTSC
  cBitmap(encrypted_xpm),      // SYMBOL_ENCRYPTED
  cBitmap(svdrp_xpm),          // SYMBOL_SVDRP
  cBitmap(lock_xpm),           // SYMBOL_LOCK
  cBitmap(signal_xpm),         // SYMBOL_SIGNAL
  cBitmap(carrier_xpm),        // SYMBOL_CARRIER
  cBitmap(viterbi_xpm),        // SYMBOL_VITERBI
  cBitmap(sync_xpm),           // SYMBOL_SYNC
  cBitmap(ar11_xpm),           // SYMBOL_AR_1_1
  cBitmap(ar169_xpm),          // SYMBOL_AR_16_9
  cBitmap(ar2211_xpm),         // SYMBOL_AR_2_21_1
  cBitmap(ar43_xpm),           // SYMBOL_AR_4_3
  cBitmap(device_xpm),         // SYMBOL_DEVICE
  cBitmap(zero_xpm),           // SYMBOL_ZERO
  cBitmap(one_xpm),            // SYMBOL_ONE
  cBitmap(two_xpm),            // SYMBOL_TWO
  cBitmap(three_xpm),          // SYMBOL_THREE
  cBitmap(four_xpm),           // SYMBOL_FOUR
  cBitmap(five_xpm),           // SYMBOL_FIVE
  cBitmap(six_xpm),            // SYMBOL_SIX
  cBitmap(seven_xpm),          // SYMBOL_SEVEN
  cBitmap(eight_xpm),          // SYMBOL_EIGHT
  cBitmap(format1080_xpm),     // SYMBOL_FORMAT_1080
  cBitmap(format720_xpm),      // SYMBOL_FORMAT_720
  cBitmap(format576_xpm),      // SYMBOL_FORMAT_576
  cBitmap(format480_xpm)       // SYMBOL_FORMAT_480
};
