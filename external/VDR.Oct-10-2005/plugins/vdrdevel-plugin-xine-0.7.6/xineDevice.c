
#include "xineCommon.h"

#include <vdr/remux.h>
#include <vdr/transfer.h>

#include "xineDevice.h"
#include "xineOsd.h"
#include "xineSettings.h"


//#define LOG_ME(x) x
#define LOG_ME(x)


namespace PluginXine
{
  void cXineSpuDecoder::ptsAdjust(uint32_t &pts)
  {
    if (0 == pts
        || -1 == (int32_t)pts)
    {
      return;
    }
    
    const int64_t ptsXine = m_xineDevice->GetSTC();

    if (-1 == ptsXine)
      return;
    
//    ::fprintf(stderr, "ptsAdjust: %ld, %lld, %lld\n", pts, ptsXine, pts - ptsXine);

    pts = (uint32_t)ptsXine;
  }
  
  int cXineSpuDecoder::setTime(uint32_t pts)
  {
    ptsAdjust(pts);
    
    return cDvbSpuDecoder::setTime(pts);
  }

  static cXineDevice *theXineDevice = 0;

  bool cXineDevice::HasDecoder(void) const
  {
    return true;
  }

  cSpuDecoder *cXineDevice::GetSpuDecoder(void)
  {
    if (!m_spuDecoder
        && IsPrimaryDevice())
    {
      m_spuDecoder = new cXineSpuDecoder(this);
    }
    
    return m_spuDecoder;
  }

  bool cXineDevice::CanReplay(void) const
  {
    return true;
  }

  static int ts = 0;
  static bool f = false;
  static bool np = false;
  static bool muted = false;
  static int jumboPESsize = 0;
  static int jumboPEStail = 0;
  static ePlayMode pm = pmNone;
  static bool audioSeen = false;

  static int64_t ptsV = -1, ptsA = -1, ptsP = -1, ptsD = -1;
  
  static enum { sstNone = 0, sstNormal, sstNoMetronom } softStartTrigger = sstNone;
  
//  static int streams[ 256 ];
  
  bool cXineDevice::SetPlayMode(ePlayMode PlayMode)
  {
    ptsV = ptsA = ptsP = ptsD = -1;
    
    ts = 0;
    np = 0;
    f  = 0;
    
    xfprintf(stderr, "SetPlayMode: %d\n", PlayMode);

    m_xineLib.pause(false);
    m_xineLib.execFuncTrickSpeedMode(false);
    m_xineLib.execFuncSetSpeed(100.0);

    if (muted)
    {
      muted = false;
      m_xineLib.execFuncMute(false);
    } 

    if (pmNone == PlayMode)
    {
      if (pmExtern_THIS_SHOULD_BE_AVOIDED == pm)
        m_xineLib.enableExternal(false);
    
      pm = PlayMode;
    
      jumboPESsize = 0;
      jumboPEStail = 0;
/*
      for (unsigned int i = 0; i < sizeof (streams) / sizeof (*streams); i++)
      {
        if (streams[ i ])
          fprintf(stderr, "stream: 0x%02x\n", i);
      }
*/
      
      m_xineLib.ignore();
//      m_xineLib.execFuncMute();
      m_xineLib.execFuncClear(-2);
      m_xineLib.execFuncSetPrebuffer(0);
      m_xineLib.execFuncStart();
//      m_xineLib.execFuncMetronom(0);
      m_xineLib.execFuncWait();

      m_xineLib.showNoSignal();

      softStartTrigger = sstNone;
    }
    else
    {
      audioSeen = false;
      softStartTrigger = sstNone;
      
//      ::memset(&streams, 0, sizeof (streams));
      
      m_xineLib.freeze();
      m_xineLib.ignore(false);

      m_xineLib.freeze(false);

      PushOut();
      m_xineLib.execFuncFlush();
      m_xineLib.execFuncWait();
      
//      m_xineLib.execFuncSetPrebuffer(m_settings.GetModeParams()->m_prebufferFrames);
      m_xineLib.execFuncSetPrebuffer(0);
      m_xineLib.execFuncClear(-4);
      m_xineLib.execFuncStart();
      m_xineLib.execFuncWait();

      m_settings.SelectReplayPrebufferMode(0 == cTransferControl::ReceiverDevice());
      
      if (m_settings.LiveTV())
      {
//        ::fprintf(stderr, "LiveTV\n");
        softStartTrigger = sstNormal;
      }
      else
        np = true;

      cMutexLock pmMutexLock(&m_pmMutex);
      pm = PlayMode;
      m_pmCondVar.Broadcast();
    }
    
    if (pmExtern_THIS_SHOULD_BE_AVOIDED == PlayMode)
      m_xineLib.enableExternal();
    
    return false;
  }

  void cXineDevice::TrickSpeed(int Speed)
  {
    f = false;
    ts = Speed;

    xfprintf(stderr, "TrickSpeed: %d\n", Speed);
    m_xineLib.execFuncTrickSpeedMode(true);
    m_xineLib.execFuncSetSpeed(100.0 / Speed);
    m_xineLib.execFuncWait();
    m_xineLib.freeze(false);
    m_xineLib.pause(false);
  }

  void cXineDevice::Clear(void)
  {
    ptsV = ptsA = ptsP = ptsD = -1;
    
    static int cntClear = 0;

    xfprintf(stderr, "Clear(%d)", cntClear);

    m_xineLib.pause();
    
    jumboPESsize = 0;
    jumboPEStail = 0;

    if (f)
        m_xineLib.execFuncSetSpeed(100.0);

    m_xineLib.execFuncClear(cntClear++);
//    m_xineLib.execFuncStart();
    np = true;
    
    if (f)
        m_xineLib.execFuncSetSpeed(0.0);
    
    m_xineLib.execFuncWait();
    m_xineLib.pause(false);
  xfprintf(stderr, "!\n");
    if (m_settings.LiveTV())
      softStartTrigger = sstNoMetronom;
   
    cDevice::Clear();
  }

  void cXineDevice::Play(void)
  {
    f = false;
    ts = 0;

    xfprintf(stderr, "Play\n");
    m_xineLib.execFuncTrickSpeedMode(false);
    m_xineLib.execFuncSetSpeed(100.0);

    if (muted)
    {
      muted = false;
      m_xineLib.execFuncMute(false);
    }
    
    m_xineLib.execFuncWait();
    m_xineLib.freeze(false);
    m_xineLib.pause(false);
    LOG_ME(::fprintf(stderr, "----\n");)
  }

  void cXineDevice::Freeze(void)
  {
    f = true;
    
    xfprintf(stderr, "Freeze\n");  
    m_xineLib.freeze();
    m_xineLib.pause();
    m_xineLib.execFuncSetSpeed(0.0);
    m_xineLib.execFuncWait();
    LOG_ME(::fprintf(stderr, "------\n");)  
  }

  void cXineDevice::Mute(void)
  {
    xfprintf(stderr, "Mute\n");
    m_xineLib.execFuncMute(true);

    muted = true;
  }

  static void store_frame(const unsigned char *buf, int len, int line)
  {
    if (0)
    {
      static int cnt = 0;
      
      char name[ 100 ];
      ::sprintf(name, "/tmp/frame_%05d_%05d", line, cnt++);
      
      FILE *f = fopen(name, "wb");
      fwrite(buf, 1, len, f);
      fclose(f);
    }
  }
  
#define VERBOSE_NOP() do{ xfprintf(stderr, "FIXME: %s:%d\n", __FILE__, __LINE__); } while (0)
#define VERBOSE_NOP1() do{ store_frame(Data, Length, __LINE__); xfprintf(stderr, "FIXME: %s:%d\n", __FILE__, __LINE__); } while (0)
#define VERBOSE_RETURN0(x) do{ xfprintf(stderr, "FIXME: %s:%d\n", __FILE__, __LINE__); return x; } while (0)
#define VERBOSE_RETURN1(x) do{ store_frame(buf0, len0, __LINE__); xfprintf(stderr, "FIXME: %s:%d\n", __FILE__, __LINE__); return x; } while (0)
#define VERBOSE_RETURN2(x) do{ store_frame(buf, len, __LINE__); xfprintf(stderr, "FIXME: %s:%d\n", __FILE__, __LINE__); return x; } while (0)
#define VERBOSE_RETURN3(x) do{ store_frame(Data, Length, __LINE__); xfprintf(stderr, "FIXME: %s:%d\n", __FILE__, __LINE__); return x; } while (0)
  
  static bool getPTS(const unsigned char *buf0, int len0, int64_t &pts)
  {
    while (len0 > 0)
    {
      while (len0 > 3
             && 0x00 == buf0[ 0 ]
             && 0x00 == buf0[ 1 ]
             && 0x00 == buf0[ 2 ])
      {
        buf0++;
        len0--;
      }
      
      if (3 == len0
          && 0x00 == buf0[ 0 ]
          && 0x00 == buf0[ 1 ]
          && 0x00 == buf0[ 2 ])
      {
        break;
      }
      
      if (len0 < 6)
        VERBOSE_RETURN1(false);
      
      if (0x00 != buf0[ 0 ]
          || 0x00 != buf0[ 1 ]
          || 0x01 != buf0[ 2 ])
      {
        VERBOSE_RETURN1(false);
      }
      
      if (0xe0 != (0xf0 & buf0[ 3 ])      // video
          && 0xc0 != (0xe0 & buf0[ 3 ])   // audio
          && 0xbd != (0xff & buf0[ 3 ])   // dolby
          && 0xbe != (0xff & buf0[ 3 ]))  // padding (DVD)
      {
        VERBOSE_RETURN1(false);
      }
      
      int len = (6 + buf0[ 4 ] * 0x100 + buf0[ 5 ]);
      if (len > len0)
        VERBOSE_RETURN1(false);
      
      const unsigned char *buf = buf0;
      buf0 += len;
      len0 -= len;

//      if (len0 != 0)
//        VERBOSE_NOP();
      
      if (0xbe == (0xff & buf[ 3 ]))  // padding (DVD)
        continue;
      
      if (len < (6 + 3))
        VERBOSE_RETURN2(false);
      
      if (0x80 != (0xc0 & buf[ 6 ]))  // MPEG1
      {
        do // ... while (false);
        {
          int o = 0;

          for (int i = 0; i < 16; i++)
          {
             if (buf[ 6 + o ] != 0xff)
               break;

             if (len < (6 + ++o))
               VERBOSE_RETURN2(false);
          }

          if (0x40 == (0xc0 & buf[ 6 + o ]))
            o += 2;
        
          if (len < (6 + o))
            VERBOSE_RETURN2(false);
          
          if (0x31 == (0xf1 & buf[ 6 + o + 0 ]))
          {
            if (len < (6 + o + 5 + 5))
              VERBOSE_RETURN2(false);
          
            if (0x01 != (0x01 & buf[ 6 + o + 2 ]))
              VERBOSE_RETURN2(false);
            
            if (0x01 != (0x01 & buf[ 6 + o + 4 ]))
              VERBOSE_RETURN2(false);
            
            if (0x11 != (0xf1 & buf[ 6 + o + 5 + 0 ]))
              VERBOSE_RETURN2(false);
            
            if (0x01 != (0x01 & buf[ 6 + o + 5 + 2 ]))
              VERBOSE_RETURN2(false);
            
            if (0x01 != (0x01 & buf[ 6 + o + 5 + 4 ]))
              VERBOSE_RETURN2(false);
            
            int64_t _pts = ((int64_t)(0x0e & buf[ 6 + o + 0 ])) << 29
              |                      (0xff & buf[ 6 + o + 1 ])  << 22
              |                      (0xfe & buf[ 6 + o + 2 ])  << 14
              |                      (0xff & buf[ 6 + o + 3 ])  << 7
              |                      (0xfe & buf[ 6 + o + 4 ])  >> 1;
            
//            ::fprintf(stderr, "pts: %lld\n", _pts);
            
            if (0 == _pts)
              break;
            
            pts = _pts;
            
            return true;
          }
          else if (0x21 == (0xf1 & buf[ 6 + o + 0 ]))
          {
            if (len < (6 + o + 5))
              VERBOSE_RETURN2(false);
          
            if (0x01 != (0x01 & buf[ 6 + o + 2 ]))
              VERBOSE_RETURN2(false);
            
            if (0x01 != (0x01 & buf[ 6 + o + 4 ]))
              VERBOSE_RETURN2(false);
            
            int64_t _pts = ((int64_t)(0x0e & buf[ 6 + o + 0 ])) << 29
              |                      (0xff & buf[ 6 + o + 1 ])  << 22
              |                      (0xfe & buf[ 6 + o + 2 ])  << 14
              |                      (0xff & buf[ 6 + o + 3 ])  << 7
              |                      (0xfe & buf[ 6 + o + 4 ])  >> 1;
            
//            ::fprintf(stderr, "pts: %lld\n", _pts);
            
            if (0 == _pts)
              break;
            
            pts = _pts;
            
            return true;
          }
          else if (0x0f == (0xff & buf[ 6 + o + 0 ]))
          {
            break;
          }
          
          for (int i = 0; i < 30; i++)
            xfprintf(stderr, "%02x ", buf[ i ]);
          xfprintf(stderr, "\n");
          
          VERBOSE_RETURN2(false);
        }
        while (false);

        continue;
      }
      
      if (0x40 == (0xc0 & buf[ 7 ]))
        VERBOSE_RETURN2(false);
      
      if (0x00 == (0xc0 & buf[ 7 ]))
        continue;

// ignore      
//      if (0x00 != (0x3f & buf[ 7 ]))
//        VERBOSE_RETURN2(false);
      
      bool hasPTS = (0 != (0x80 & buf[ 7 ]));
      bool hasDTS = (0 != (0x40 & buf[ 7 ]));
      
      unsigned char hdl = buf[ 8 ];
      
      if (hdl < ((hasPTS + hasDTS) * 5))
        VERBOSE_RETURN2(false);
      
      if (len < (6 + 3 + hdl))
        VERBOSE_RETURN2(false);
      
      if ((0x20 * hasPTS + 0x10 * hasDTS + 0x01) != (0xf1 & buf[ 9 ]))
      {
        if ((0x20 * hasPTS + 0x00 * hasDTS + 0x01) != (0xf1 & buf[ 9 ]))
        {
          // accept streams, that start with '00X0' instead of '00X1'.
        }
        else if ((0x00 * hasPTS + 0x10 * hasDTS + 0x01) != (0xf1 & buf[ 9 ]))
        {
          // accept streams, that start with '000X' instead of '001X'.
        }
        else
        {
          VERBOSE_RETURN2(false);
        }
      }
      
      if (0x01 != (0x01 & buf[ 11 ]))
        VERBOSE_RETURN2(false);
      
      if (0x01 != (0x01 & buf[ 13 ]))
        VERBOSE_RETURN2(false);
      
      if (hasDTS)
      {
        if (0x11 != (0xf1 & buf[ 14 ]))
          VERBOSE_RETURN2(false);
                  
        if (0x01 != (0x01 & buf[ 16 ]))
          VERBOSE_RETURN2(false);
              
        if (0x01 != (0x01 & buf[ 18 ]))
          VERBOSE_RETURN2(false);
      }
/*
      fprintf(stderr, " %02x %02x %02x %02x %02x\n"
              , buf[  9 ]
              , buf[ 10 ]
              , buf[ 11 ]
              , buf[ 12 ]
              , buf[ 13 ]);
*/
      int64_t _pts = ((int64_t)(0x0e & buf[  9 ])) << 29
        |                      (0xff & buf[ 10 ])  << 22
        |                      (0xfe & buf[ 11 ])  << 14
        |                      (0xff & buf[ 12 ])  << 7
        |                      (0xfe & buf[ 13 ])  >> 1;

      if (0 == _pts)
        return false;

      pts = _pts;
      
      return true;
    }

//    VERBOSE_RETURN2(false);
    return false;
  }

  static bool stripPTSandDTS(unsigned char *buf0, int len0)
  {
    while (len0 > 0)
    {
      while (len0 > 3
             && 0x00 == buf0[ 0 ]
             && 0x00 == buf0[ 1 ]
             && 0x00 == buf0[ 2 ])
      {
        buf0++;
        len0--;
      }
      
      if (3 == len0
          && 0x00 == buf0[ 0 ]
          && 0x00 == buf0[ 1 ]
          && 0x00 == buf0[ 2 ])
      {
        break;
      }
      
      if (len0 < 6)
        VERBOSE_RETURN1(false);
      
      if (0x00 != buf0[ 0 ]
          || 0x00 != buf0[ 1 ]
          || 0x01 != buf0[ 2 ])
      {
        VERBOSE_RETURN1(false);
      }
      
      if (0xe0 != (0xf0 & buf0[ 3 ])      // video
          && 0xc0 != (0xe0 & buf0[ 3 ])   // audio
          && 0xbd != (0xff & buf0[ 3 ])   // dolby
          && 0xbe != (0xff & buf0[ 3 ])   // padding (DVD)
          && 0xba != (0xff & buf0[ 3 ])   // system layer: pack header
          && 0xbb != (0xff & buf0[ 3 ])   // system layer: system header
          && 0xb9 != (0xff & buf0[ 3 ]))  // system layer: stream end
      {
//fprintf(stderr, "buf0[ 3 ]: %02x\n", buf0[ 3 ]);
        VERBOSE_RETURN1(false);
      }

      int len = (6 + buf0[ 4 ] * 0x100 + buf0[ 5 ]);
      if (0xba == buf0[ 3 ]) // pack header has fixed length
      {
        if (0x00 == (0xc0 & buf0[ 4 ])) // MPEG 1
          len = 12;
        else // MPEG 2
          len = 14 + (buf0[ 13 ] & 0x07);
      }
      else if (0xb9 == buf0[ 3 ]) // stream end has fixed length
      {
        len = 4;
      }

      if (len > len0)
        VERBOSE_RETURN1(false);
      
      unsigned char *buf = buf0;
      buf0 += len;
      len0 -= len;

//      if (len0 != 0)
//        VERBOSE_NOP();
      
      if (0xbe == (0xff & buf[ 3 ])    // padding (DVD)
        || 0xba == (0xff & buf[ 3 ])   // system layer: pack header
        || 0xbb == (0xff & buf[ 3 ])   // system layer: system header
        || 0xb9 == (0xff & buf[ 3 ]))  // system layer: stream end
      {
        continue;
      }
      
      if (len < (6 + 3))
        VERBOSE_RETURN2(false);
      
      if (0x80 != (0xc0 & buf[ 6 ]))  // MPEG1
      {
        bool ok = false;
        
        do // ... while (false);
        {
          int o = 0;

          for (int i = 0; i < 16; i++)
          {
             if (buf[ 6 + o ] != 0xff)
               break;

             if (len < (6 + ++o))
               VERBOSE_RETURN2(false);
          }

          if (0x40 == (0xc0 & buf[ 6 + o ]))
            o += 2;
        
          if (len < (6 + o))
            VERBOSE_RETURN2(false);
          
          if (0x31 == (0xf1 & buf[ 6 + o + 0 ]))
          {
            if (0x01 != (0x01 & buf[ 6 + o + 2 ]))
              VERBOSE_RETURN2(false);
            
            if (0x01 != (0x01 & buf[ 6 + o + 4 ]))
              VERBOSE_RETURN2(false);
            
            if (0x11 != (0xf1 & buf[ 6 + o + 5 + 0 ]))
              VERBOSE_RETURN2(false);
            
            if (0x01 != (0x01 & buf[ 6 + o + 5 + 2 ]))
              VERBOSE_RETURN2(false);
            
            if (0x01 != (0x01 & buf[ 6 + o + 5 + 4 ]))
              VERBOSE_RETURN2(false);
            
            buf[ 6 + o + 0 ] = 0xff;
            buf[ 6 + o + 1 ] = 0xff;
            buf[ 6 + o + 2 ] = 0xff;
            buf[ 6 + o + 3 ] = 0xff;
            buf[ 6 + o + 4 ] = 0xff;
            
            buf[ 6 + o + 5 + 0 ] = 0xff;
            buf[ 6 + o + 5 + 1 ] = 0xff;
            buf[ 6 + o + 5 + 2 ] = 0xff;
            buf[ 6 + o + 5 + 3 ] = 0xff;
            buf[ 6 + o + 5 + 4 ] = 0x0f;
            
            ok = true;
          }
          else if (0x21 == (0xf1 & buf[ 6 + o + 0 ]))
          {
            if (0x01 != (0x01 & buf[ 6 + o + 2 ]))
              VERBOSE_RETURN2(false);
            
            if (0x01 != (0x01 & buf[ 6 + o + 4 ]))
              VERBOSE_RETURN2(false);
            
            buf[ 6 + o + 0 ] = 0xff;
            buf[ 6 + o + 1 ] = 0xff;
            buf[ 6 + o + 2 ] = 0xff;
            buf[ 6 + o + 3 ] = 0xff;
            buf[ 6 + o + 4 ] = 0x0f;
            
            ok = true;
          }
          else if (0x0f == (0xff & buf[ 6 + o + 0 ]))
          {
            ok = true;
          }

          if (ok)
            break;
            
          for (int i = 0; i < 30; i++)
            xfprintf(stderr, "%02x ", buf[ i ]);
          xfprintf(stderr, "\n");
          
          VERBOSE_RETURN2(false);
        }
        while (false);

        if (ok)
          continue;
      }
      
      if (0x40 == (0xc0 & buf[ 7 ]))
        VERBOSE_RETURN2(false);
      
      if (0x00 == (0xc0 & buf[ 7 ]))
        continue;

// ignore      
//      if (0x00 != (0x3f & buf[ 7 ]))
//        VERBOSE_RETURN2(false);
      
      bool hasPTS = (0 != (0x80 & buf[ 7 ]));
      bool hasDTS = (0 != (0x40 & buf[ 7 ]));
      
      unsigned char hdl = buf[ 8 ];
      
      if (hdl < ((hasPTS + hasDTS) * 5))
        VERBOSE_RETURN2(false);
      
      if (len < (6 + 3 + hdl))
        VERBOSE_RETURN2(false);
      
      if ((0x20 * hasPTS + 0x10 * hasDTS + 0x01) != (0xf1 & buf[ 9 ]))
      {
        if ((0x20 * hasPTS + 0x00 * hasDTS + 0x01) != (0xf1 & buf[ 9 ]))
        {
          // accept streams, that start with '00X0' instead of '00X1'.
        }
        else if ((0x00 * hasPTS + 0x10 * hasDTS + 0x01) != (0xf1 & buf[ 9 ]))
        {
          // accept streams, that start with '000X' instead of '001X'.
        }
        else
        {
          VERBOSE_RETURN2(false);
        }
      }
      
      if (0x01 != (0x01 & buf[ 11 ]))
        VERBOSE_RETURN2(false);
      
      if (0x01 != (0x01 & buf[ 13 ]))
        VERBOSE_RETURN2(false);
      
      if (hasDTS)
      {
        if (0x11 != (0xf1 & buf[ 14 ]))
          VERBOSE_RETURN2(false);
        
        if (0x01 != (0x01 & buf[ 16 ]))
          VERBOSE_RETURN2(false);
                  
        if (0x01 != (0x01 & buf[ 18 ]))
          VERBOSE_RETURN2(false);
      }
      
      buf[ 7 ] &= 0x3f;
      
      for (int i = 9; i < (9 + ((hasPTS + hasDTS) * 5)); i++)
        buf[ i ] = 0xff;
    }
    
    return true;
  }

  static uchar padding[ 6 + 0xffff ] =
  {
    0x00
    , 0x00
    , 0x01
    , 0xbe
    , 0xff
    , 0xff
  };

  int cXineDevice::PushOut()
  {
    uchar *Data = padding;
    int Length = sizeof (padding);
    
    return PlayCommon3(Data, Length);
  }
  
  void cXineDevice::StillPicture(const uchar *Data, int Length)
  {
    xfprintf(stderr, "StillPicture: %p, %d\n", Data, Length);

    if (0)
    {
      for (int i = 0; i < Length - 3; i++)
      {
        if (i != 0
            && Data[ i + 0 ] == 0x00
            && Data[ i + 1 ] == 0x00
            && Data[ i + 2 ] == 0x01)
        {
          xfprintf(stderr, "\n");
        }
        
        xfprintf(stderr, "%02x ", Data[ i ]);
      }
      
      for (int i = Length - 3; i < Length; i++)
      {
        xfprintf(stderr, "%02x ", Data[ i ]);
      }
      
      xfprintf(stderr, "\n");
    }
    
    const int maxPackets = 3;
    uchar pes[ maxPackets * (6 + 0xffff) ];
    static const uchar header[ 6 + 3 ] = { 0x00, 0x00, 0x01, 0xe0, 0x00, 0x00, 0x80, 0x00, 0x00 };
    
    do // ... while (false);
    {
      if (Length < 6)
      {
        VERBOSE_NOP();
        break;
      }
      
      if (0x00 != Data[ 0 ]
          || 0x00 != Data[ 1 ]
          || 0x01 != Data[ 2 ])
      {
        VERBOSE_NOP();
        break;
      }
      
      if (0xe0 != (0xf0 & Data[ 3 ])      // video
          && 0xc0 != (0xe0 & Data[ 3 ])   // audio
          && 0xbd != (0xff & Data[ 3 ])   // dolby
          && 0xbe != (0xff & Data[ 3 ]))  // padding (DVD)
      {
        if (Length > maxPackets * (0xffff - 3))
        {
          VERBOSE_NOP();
          break;
        }
      
	int todo = Length;
        const uchar *src = Data;
        uchar *dst = pes;

        while (todo > 0)
        {
          ::memcpy(dst, header, sizeof (header));
          
          int bite = todo;

          if (bite > 0xffff - 3)
            bite = 0xffff - 3;

          todo -= bite;
          
          dst[ 4 ] = 0xff & ((bite + 3) >> 8);
          dst[ 5 ] = 0xff & (bite + 3);

          ::memcpy(dst + sizeof (header), src, bite);

          Length += sizeof (header);
          dst += sizeof (header) + bite;
          src += bite;
        }
        
        Data = pes;
      }
    }
    while (false);
    
    stripPTSandDTS((uchar *)Data, Length);
    
    ts = 0;

    m_xineLib.execFuncTrickSpeedMode(false);
    m_xineLib.execFuncSetSpeed(100.0);
    m_xineLib.execFuncStillFrame();
    m_xineLib.execFuncWait();

    f = 0;
    
    m_xineLib.pause(false);

    for (int i = 0; i < 1 /* 4 */; i++)
    {
//fprintf(stderr, " (%d) ", i);
      int r = PlayVideo1(Data, Length, true);      
      if (r < 0)
        return;
    }

    PushOut();
    m_xineLib.execFuncFlush();

//    ::fprintf(stderr, "------------\n");      
    LOG_ME(::fprintf(stderr, "------------\n");)
  }

  static bool softStartPoll(cXineLib &xineLib, cPoller &poller, const int timeout, const bool result);

  bool cXineDevice::Poll(cPoller &Poller, int TimeoutMs /* = 0 */)
  {
    if (m_xineLib.Poll(Poller, TimeoutMs))
      return softStartPoll(m_xineLib, Poller, TimeoutMs, true);

    return softStartPoll(m_xineLib, Poller, TimeoutMs, false);
  }

  static bool jw = false;

  bool cXineDevice::Flush(int TimeoutMs /* = 0 */)
  {
    const bool jw0 = jw;
    
    m_xineLib.pause(false);

    if (!jw0)
    {
      int r = PushOut();
      if (r < 0)
        return true;
    }

    bool retVal = m_xineLib.execFuncFlush(TimeoutMs, jw0);

    if (!retVal)
      xfprintf(stderr, jw0 ? "f" : "F");
    
    jw = true;
    
    return retVal;
  }
  
  static bool dumpAudio(const char *proc, const uchar *Data, int Length)
  {
    return false;
    
  nextPacket:
    if (Length == 0)
      return true;
/*    
    fprintf(stderr
            , "%s: "
            , proc);
*/  
    if (Length < 6)
      VERBOSE_RETURN0(false);
        
    if (0x00 != Data[ 0 ]
        || 0x00 != Data[ 1 ]
        || 0x01 != Data[ 2 ])
    {
      VERBOSE_RETURN3(false);
    }

    int l = Data[ 4 ] * 0x0100 + Data[ 5 ];
    if (Length < (6 + l))
      VERBOSE_RETURN0(false);
    
    if (0xe0 == (Data[ 3 ] & 0xf0)     //video
        || 0xc0 == (Data[ 3 ] & 0xe0)  //audio
        || 0xbe == Data[ 3 ])          //padding
    {
      Data += (6 + l);
      Length -= (6 + l);
      goto nextPacket;
    }
    
  //  if (0xbd != Data[ 3 ])             //private (dolby, pcm)
  //    VERBOSE_RETURN0(false);

//    fprintf(stderr, "private ");
    
    if (l < (3 + 0 + 2))
      VERBOSE_RETURN0(false);
    
    int h = Data[ 8 ];
    if (l < (3 + h + 2))
      VERBOSE_RETURN0(false);

    xfprintf(stderr
            , "%s: "
            , proc);

    xfprintf(stderr
            , "0x%02x 0x%02x\n"
            , Data[ 6 + 3 + h + 0 ]
            , Data[ 6 + 3 + h + 1 ]);

    Data += (6 + l);
    Length -= (6 + l);
    goto nextPacket;
  }
  
  static bool IsVideo(const uchar *Data, int Length)
  {
    return (Length >= 4
            && 0x00 == Data[ 0 ]
            && 0x00 == Data[ 1 ]
            && 0x01 == Data[ 2 ]
            && 0xe0 == (0xf0 & Data[ 3 ]));
  }

  int cXineDevice::PlayVideo(const uchar *Data, int Length)
  {
//    static FILE *f = fopen("/tmp/egon1", "wb");
//    fwrite(Data, Length, 1, f);

    return PlayVideo1(Data, Length, false);
  }
  
  int cXineDevice::PlayVideo1(const uchar *Data, int Length, const bool stillImageData)
  {
    LOG_ME(::fprintf(stderr, "V");)

    if (f)
    {      
      LOG_ME(::fprintf(stderr, "<");)
      return 0;
    }

    if (pmNone == pm)
    {
      cMutexLock pmMutexLock(&m_pmMutex);

      if (pmNone == pm)
        m_pmCondVar.Wait(m_pmMutex);
    }
    
    int retVal = PlayVideo2(Data, Length, stillImageData);

    LOG_ME(::fprintf(stderr, "v");)

    return retVal;
  }

#if VDRVERSNUM < 10331

enum ePesHeader {
  phNeedMoreData = -1,
  phInvalid = 0,
  phMPEG1 = 1,
  phMPEG2 = 2
  };

static ePesHeader AnalyzePesHeader(const uchar *Data, int Count, int &PesPayloadOffset, bool *ContinuationHeader = NULL)
{
  if (Count < 7)
     return phNeedMoreData; // too short

  if ((Data[6] & 0xC0) == 0x80) { // MPEG 2
     if (Count < 9)
        return phNeedMoreData; // too short

     PesPayloadOffset = 6 + 3 + Data[8];
     if (Count < PesPayloadOffset)
        return phNeedMoreData; // too short

     if (ContinuationHeader)
        *ContinuationHeader = ((Data[6] == 0x80) && !Data[7] && !Data[8]);

     return phMPEG2; // MPEG 2
     }

  // check for MPEG 1 ...
  PesPayloadOffset = 6;

  // skip up to 16 stuffing bytes
  for (int i = 0; i < 16; i++) {
      if (Data[PesPayloadOffset] != 0xFF)
         break;

      if (Count <= ++PesPayloadOffset)
         return phNeedMoreData; // too short
      }

  // skip STD_buffer_scale/size
  if ((Data[PesPayloadOffset] & 0xC0) == 0x40) {
     PesPayloadOffset += 2;

     if (Count <= PesPayloadOffset)
        return phNeedMoreData; // too short
     }

  if (ContinuationHeader)
     *ContinuationHeader = false;

  if ((Data[PesPayloadOffset] & 0xF0) == 0x20) {
     // skip PTS only
     PesPayloadOffset += 5;
     }
  else if ((Data[PesPayloadOffset] & 0xF0) == 0x30) {
     // skip PTS and DTS
     PesPayloadOffset += 10;
     }
  else if (Data[PesPayloadOffset] == 0x0F) {
     // continuation header
     PesPayloadOffset++;

     if (ContinuationHeader)
        *ContinuationHeader = true;
     }
  else
     return phInvalid; // unknown

  if (Count < PesPayloadOffset)
     return phNeedMoreData; // too short

  return phMPEG1; // MPEG 1
}

#endif 
 
  int cXineDevice::PlayCommon2(const uchar *Data, int Length)
  {
    do // ... while (false);
    {
      if (Length < 6)
      {
        VERBOSE_NOP();
        break;
      }
      
      if (0x00 != Data[ 0 ]
          || 0x00 != Data[ 1 ]
          || 0x01 != Data[ 2 ])
      {
        VERBOSE_NOP();
        break;
      }
      
      int l = 6 + Data[ 4 ] * 0x0100 + Data[ 5 ];
      if (Length < l)
      {
        VERBOSE_NOP();
        break;
      }
      
      if (0xe0 != (Data[ 3 ] & 0xf0)     //video
          && 0xc0 != (Data[ 3 ] & 0xe0)  //audio
          && 0xbd != Data[ 3 ])          //private (dolby, pcm)
      {
        VERBOSE_NOP();
        break;
      }

      int payloadOffset = 0;
      if (AnalyzePesHeader(Data, Length, payloadOffset) <= phInvalid)
      {
        VERBOSE_NOP();
        break;
      }

      if (l <= payloadOffset)
      {
        // drop short frames
//        ::fprintf(stderr, "i");
        return Length;
      }

//      if (0xc0 == (Data[ 3 ] & 0xe0))  //audio
//        return Length;
    }
    while (false);

    return PlayCommon3(Data, Length);
  }

  int cXineDevice::PlayCommon3(const uchar *Data, int Length)
  {
//    if (!m_settings.LiveTV())
//    {
//      VERBOSE_NOP1();
//      return Length;
//    }

    if (0)
    {
      int64_t pts = -1;
      if (getPTS(Data, Length, pts))
      {
        int64_t *pPTS = 0;
        
        if (0xe0 == (Data[ 3 ] & 0xf0))      //video
        {
          pPTS = &ptsV;
        }
        else if (0xc0 == (Data[ 3 ] & 0xe0)) //audio
        {
          pPTS = &ptsA;
        }
        else if (0xbd == Data[ 3 ])          //private (dolby, pcm)
        {
          int h = Data[ 6 + 2 ];
          
          if (0xa0 == (0xf0 & Data[ 6 + 3 + h + 0 ]))  // pcm?
            pPTS = &ptsP;
          else
            pPTS = &ptsD;
        }
        else
        {
          xfprintf(stderr, "0x%02x\t", Data[ 3 ]);
          VERBOSE_NOP();        
        }
        
        if (pPTS
            && *pPTS != pts)
        {
          *pPTS = pts;
          
          int64_t ptsX = -1;
          m_xineLib.execFuncGetPTS(ptsX);

          int64_t dV = (ptsV != -1 && ptsX != -1) ? ptsV - ptsX : 0;
          int64_t dA = (ptsA != -1 && ptsX != -1) ? ptsA - ptsX : 0;
          int64_t dP = (ptsP != -1 && ptsX != -1) ? ptsP - ptsX : 0;
          int64_t dD = (ptsD != -1 && ptsX != -1) ? ptsD - ptsX : 0;

          fprintf(stderr, "ptsVideo: %lld, ptsAudio: %lld, ptsPCM: %lld, ptsDolby: %lld, ptsXine: %lld, dV: %lld, dA: %lld, dP: %lld, dD: %lld\n", ptsV, ptsA, ptsP, ptsD, ptsX, dV, dA, dP, dD);
        }
      }
    }
    
    int done = 0;

    while (done < Length)
    {
      int r = m_xineLib.execFuncStream1(Data + done, Length - done);
      if (r < 0)
        return r;
      
      done += r;
    }

    return done;
  }
  
  int cXineDevice::PlayVideo2(const uchar *Data, int Length, const bool stillImageData)
  {
//    fprintf(stderr, "D");
    
    int done = 0;

    while (done < Length)
    {
      char ch = 'X';
      
      int todo = Length - done;
      
      if (todo >= 6)
      {
        if (0x00 == Data[ done + 0 ]
            && 0x00 == Data[ done + 1 ]
            && 0x01 == Data[ done + 2 ])
        {
          int id  = Data[ done + 3 ];
          int len = 6 + Data[ done + 4 ] * 0x0100 + Data[ done + 5 ];

          if (0xba == id) // pack header has fixed length
          {
            if (0x00 == (0xc0 & Data[ done + 4 ])) // MPEG 1
              len = 12;
            else // MPEG 2
              len = 14 + (Data[ done + 13 ] & 0x07);
          }
          else if (0xb9 == id) // stream end has fixed length
          {
            len = 4;
          }

          if (todo >= len)
          {
            todo = len;

            if (0xbe == id   // padding
              || 0xba == id  // system layer: pack header
              || 0xbb == id  // system layer: system header
              || 0xb9 == id) // system layer: stream end
            {
              done += todo;

//              fprintf(stderr, "x");
              continue;
            }

            ch = '.';            
          }
          else        
          {
//            ::fprintf(stderr, "todo: %d, len: %d\t", todo, len);
            VERBOSE_NOP();
            ch = '3';

//            break;
          }
        }
        else        
        {
          VERBOSE_NOP();
          ch = '2';
        }
      }
      else        
      {
        VERBOSE_NOP();
        ch = '1';
      }
      
//      fprintf(stderr, "%c", ch);
      
      int r = PlayVideo3(Data + done, todo, stillImageData);
      if (r < 0)
        return r;

      done += r;
    }
    
    return done;
  }

  static void resetScramblingControl(const uchar *Data, int Length)
  {
    if (Length < 6)
    {
      VERBOSE_NOP();
      return;
    }
      
    if (0x00 != Data[ 0 ]
        || 0x00 != Data[ 1 ]
        || 0x01 != Data[ 2 ])
    {
      VERBOSE_NOP1();
      return;
    }
      
    if (0xe0 != (0xf0 & Data[ 3 ])      // video
        && 0xc0 != (0xe0 & Data[ 3 ])   // audio
        && 0xbd != (0xff & Data[ 3 ])   // dolby
        && 0xbe != (0xff & Data[ 3 ])   // padding (DVD)
        && 0xba != (0xff & Data[ 3 ])   // system layer: pack header
        && 0xbb != (0xff & Data[ 3 ])   // system layer: system header
        && 0xb9 != (0xff & Data[ 3 ]))  // system layer: stream end
    {
      VERBOSE_NOP();
      return;
    }
        
    if (0xbe == (0xff & Data[ 3 ])    // padding (DVD)
      || 0xba == (0xff & Data[ 3 ])   // system layer: pack header
      || 0xbb == (0xff & Data[ 3 ])   // system layer: system header
      || 0xb9 == (0xff & Data[ 3 ]))  // system layer: stream end
    {
      return;
    }

    int len = (6 + Data[ 4 ] * 0x100 + Data[ 5 ]);
    if (len < 6 + 1
        || Length < 6 + 1)
    {
      VERBOSE_NOP();
      return;
    }

    if (0x00 != (0x30 & Data[ 6 ]))
    {
      if (0x80 == (0xc0 & Data[ 6 ]))  // only touch MPEG2
      {
        xfprintf(stderr, "reseting PES_scrambling_control: 0x%02x\n", Data[ 6 ]);

        ((uchar *)Data)[ 6 ] &= ~0x30;
      }
    }
  }
  
  int cXineDevice::PlayVideo3(const uchar *Data, int Length, const bool stillImageData)
  {
    resetScramblingControl(Data, Length);
    
    dumpAudio("Video", Data, Length);

    return PlayCommon(Data, Length, stillImageData);
  }
  
/*
// Start codes:
#define SC_PICTURE 0x00  // "picture header"
  
  static int GetPacketLength(const uchar *Data, int Count, int Offset)
  {
    // Returns the entire length of the packet starting at offset, or -1 in case of error.
    return (Offset + 5 < Count) ? (Data[Offset + 4] << 8) + Data[Offset + 5] + 6 : -1;
  }
  
  static int ScanVideoPacket(const uchar *Data, int Count, int Offset, uchar &PictureType)
  {
    // Scans the video packet starting at Offset and returns its length.
    // If the return value is -1 the packet was not completely in the buffer.
    
    int Length = GetPacketLength(Data, Count, Offset);
    if (Length > 0 && Offset + Length <= Count)
    {
      int i = Offset + 8; // the minimum length of the video packet header
      i += Data[i] + 1;   // possible additional header bytes
      for (; i < Offset + Length; i++)
      {
        if (Data[i] == 0 && Data[i + 1] == 0 && Data[i + 2] == 1)
        {
          switch (Data[i + 3])
          {
          case SC_PICTURE: PictureType = (Data[i + 5] >> 3) & 0x07;
            return Length;
          }
        }
      }
      PictureType = NO_PICTURE;
      return Length;
    }
    return -1;
  }
  
  static bool IsIframe(const uchar *Data, int Length)
  {
    uchar pictureType = NO_PICTURE;
    
    return ((ScanVideoPacket(Data, Length, 0, pictureType) > 0)
      && (pictureType == I_FRAME));      
  }
*/                             
  static enum
  {
    sIdle
    , sInitiateSequence
    , sStartSequence
    , sContinueSequence
  }
  softStartState = sIdle;
  
  static double softStartTime = 0;
  static const double softStartSpeedStart = 0.75;
  static const double softStartSpeedMin = 0.15;
  static double softStartDuration = 31 / 25.0;
  static const int softStartMaxSpeedChanges = 20;
  static double softStartLastSpeed = -1;
  static double softStartSpeedChangeTime = -1;
  static int64_t softStartPtsVdr = -1;
  static const int64_t softStartLogPtsDelta = 3 * 90000;
  static int softStartHitPoll = 0;
  static bool softStartNoMetronom = false;
  static const double pi = 4 * ::atan(1);

  
  static double softStartCalcSpeed0(const double t)
  {
    return 0;
  }
  
  static double softStartCalcSpeed1(const double t)
  {
    const double p = (1 + ::cos(2 * pi * t)) / 2;    
          
    return softStartSpeedMin + p * ((softStartSpeedStart * (1 - t) + 1.0 * t) - softStartSpeedMin);
  }
  
  static double softStartCalcSpeed2(const double t)
  {
    double p = 2 * t - 1;
    if (p < 0)
      p = -p;

//    p = p * p * p;
    p = p * p;
          
    return softStartSpeedMin + p * ((softStartSpeedStart * (1 - t) + 1.0 * t) - softStartSpeedMin);
  }
  
  static double softStartCalcSpeed3(const double t)
  {
    if (t < 0.25)
      return softStartSpeedStart;

    if (t < 0.50)
      return softStartSpeedMin + (1 - softStartSpeedMin) * 0 / 3;

    if (t < 0.75)
      return softStartSpeedMin + (1 - softStartSpeedMin) * 1 / 3;

    return   softStartSpeedMin + (1 - softStartSpeedMin) * 2 / 3;
  }
  
  static double softStartCalcSpeed(const double t)
  {
    if (t >= 1)
      return 1;
   
    return softStartCalcSpeed1(t);  // choose a method

    (void)softStartCalcSpeed0;
    (void)softStartCalcSpeed2;
    (void)softStartCalcSpeed3;
  }

  bool softStartPoll(cXineLib &xineLib, cPoller &poller, const int timeout, bool result)
  {
    if (softStartState > sIdle)
    {
      if (result)
      {
        softStartHitPoll = 0;
      }
      else if (++softStartHitPoll >= 2)
      {
        do
        {
          softStartState = sIdle;
          
          xineLib.execFuncFirstFrame();
          xineLib.execFuncSetSpeed(100.0);
          xineLib.execFuncWait();
        }
        while (!xineLib.Poll(poller, timeout, true));

        softStartHitPoll = 0;
        result = true;
      }

//      ::fprintf(stderr, "softStartHitPoll: %d, %d\n", result, softStartHitPoll);      
    }

    return result;
  }

  static int64_t vpts, apts, extra0, extra;
  static bool useApts;
  static bool seenAudio = false;
  static bool seenVideo = false;
  static bool seenApts = false;
  static bool seenVpts = false;
  
  static const int64_t extra_max = 50 * 90000 / 25;
  
  int cXineDevice::PlayCommon(const uchar *Data, int Length, const bool stillImageData)
  {
    const bool isAudio = !IsVideo(Data, Length);
    const bool isVideo = !isAudio && !stillImageData;

    if (stillImageData
      && isAudio)
    {
//      ::fprintf(stderr, "x");
      return Length;
    }
/*    
    if (stillImageData)
      fprintf(stderr, "writing: %d\n", Length - 6);
*/
    if (softStartTrigger
      && !stillImageData)
    {
      softStartNoMetronom = (sstNoMetronom == softStartTrigger);
      softStartTrigger = sstNone;

      softStartState = sInitiateSequence;

//      ::fprintf(stderr, "#(%d,%d)", ::getpid(), pthread_self());
    }

    if (softStartState > sIdle)
    {
      timeval tv;
      ::gettimeofday(&tv, 0);

      const double now = (tv.tv_sec + tv.tv_usec / 1.0e+6);

      if (softStartState == sInitiateSequence)
      {
        xfprintf(stderr, "[");
      
        softStartDuration = m_settings.GetModeParams()->m_prebufferFrames / 25.0;
        
        softStartTime = now;        
        
        softStartSpeedChangeTime = -1;
        softStartLastSpeed = -1;
        softStartPtsVdr = -1;
        softStartHitPoll = 0;

        softStartState = sStartSequence;

        vpts = -1;
        apts = -1;
        extra0 = 0;
        extra = 0;

        useApts = 1;
        
        seenAudio = false;
        seenVideo = false;
        seenApts = false;
        seenVpts = false;
      }

      {
        if (isVideo)
        {
          if (!seenVideo)
          {
            seenVideo = true;
//            ::fprintf(stderr, "seen video\n");
            xfprintf(stderr, "v");
          }
        }
        else if (isAudio)
        {
          if (!seenAudio)
          {
            audioSeen = true;
            seenAudio = true;
//            ::fprintf(stderr, "seen audio\n");
            xfprintf(stderr, "a");
          }
        }
        
        int64_t pts;
        if (getPTS(Data, Length, pts))
        {
          if (isVideo)
          {
            if (!seenVpts)
            {
              seenVpts = true;
//              ::fprintf(stderr, "seen video pts\n");
              xfprintf(stderr, "V");
            }
            
            vpts = pts;

            if (apts > -1)
            {
              int64_t delta = vpts - apts;
              if (delta < 0)
                delta = - delta;

              if (extra0 < delta)
              {
                extra0 = delta;

//                ::fprintf(stderr, "max. A/V delta: %lld pts => total extra buffering: %d frames", extra0, (int)(extra0 * 25 / 90000));

                extra = extra0;
                
		if (extra > extra_max)
		{
		  extra = extra_max;

//                  ::fprintf(stderr, ", limited to %d frames", (int)(extra * 25 / 90000));
		}	

//		::fprintf(stderr, "\n");
                xfprintf(stderr, "+%d", (int)(extra * 25 / 90000));
              }
            }

//            ::fprintf(stderr, "video: v: %lld, a: %lld, d: %lld, e: %lld\n", vpts, apts, vpts - apts, extra);
          }
          else if (isAudio)
          {
            if (!seenApts)
            {
              seenApts = true;
//              ::fprintf(stderr, "seen audio pts\n");
              xfprintf(stderr, "A");
            }
            
            apts = pts;
            
            if (vpts > -1)
            {
              int64_t delta = vpts - apts;
              if (delta < 0)
                delta = - delta;

              if (extra0 < delta)
              {
                extra0 = delta;
                
//                ::fprintf(stderr, "max. A/V delta: %lld pts => total extra buffering: %d frames", extra0, (int)(extra0 * 25 / 90000));

                extra = extra0;
                
		if (extra > extra_max)
		{
		  extra = extra_max;

//                  ::fprintf(stderr, ", limited to %d frames", (int)(extra * 25 / 90000));
		}	

//		::fprintf(stderr, "\n");
                xfprintf(stderr, "+%d", (int)(extra * 25 / 90000));
              }
            }

//            ::fprintf(stderr, "audio: v: %lld, a: %lld, d: %lld, e: %lld\n", vpts, apts, vpts - apts, extra);
          }
        }
      }

      if (seenVideo && !seenVpts
          || seenAudio && !seenApts)
      {
        softStartTime = now;
      }
      
      if (softStartState >= sStartSequence)
      {
        if (isVideo)
          useApts = false;
        
        if (useApts
            || isVideo)
        {
          getPTS(Data, Length, softStartPtsVdr);

          if (softStartPtsVdr != -1)
          {
            int64_t ptsXine = -1;
            m_xineLib.execFuncGetPTS(ptsXine);
            
            const int64_t delta = (ptsXine != -1) ? (softStartPtsVdr - ptsXine) : 0;
            if (softStartState == sStartSequence
                || delta < -softStartLogPtsDelta
                || delta > +softStartLogPtsDelta)
            {
//              if (softStartState != sStartSequence)
//                ::fprintf(stderr, "SoftStart: ptsVdr: %lld, ptsXine: %lld, delta: %lld\n", softStartPtsVdr, ptsXine, delta);

              if (!softStartNoMetronom)
              {
                xfprintf(stderr, "M");
                
//ZZZ                m_xineLib.execFuncMetronom(softStartPtsVdr);
//ZZZ                m_xineLib.execFuncWait();
              }
              
              softStartTime = now;
              
              softStartState = sContinueSequence;
            }
          }
        }
      }
        
      if (softStartState <= sStartSequence)
        stripPTSandDTS((uchar *)Data, Length);

      m_xineLib.execFuncFirstFrame();

      const double totalDuration = (softStartDuration + extra / 90000.0);
      const double p = softStartCalcSpeed((now - softStartTime) / totalDuration);
      double speed = (p > 0) ? (100.0 * p) : 12.5;
          
      if (speed >= 100.0)
      {            
        xfprintf(stderr, "]");
        
        speed = 100.0;
            
        softStartState = sIdle;

        int64_t vdrPTS = vpts, xinePTS = -1;
        if (vdrPTS > -1 && apts > -1)
        {
          if (vdrPTS < apts)
            vdrPTS = apts;
        }
        else if (apts > -1)
          vdrPTS = apts;

        m_xineLib.execFuncGetPTS(xinePTS);

        if (vdrPTS > -1 && xinePTS > -1)
          xfprintf(stderr, " buffered %.1lf frames", (vdrPTS - xinePTS) / 90000.0 * 25);
        
        xfprintf(stderr, "\n");
      }
          
      if (100.0 == speed
          || (speed != softStartLastSpeed
              && (now - softStartSpeedChangeTime) > (totalDuration / softStartMaxSpeedChanges)))
      {
        softStartSpeedChangeTime = now;
        
//        fprintf(stderr, "slowstart: %lg, %lg\n", speed, p);

        m_xineLib.execFuncSetSpeed(speed);
        m_xineLib.execFuncWait();

//        if (100.0 == speed)
//          m_xineLib.execFuncSetPrebuffer(m_settings.GetModeParams()->m_prebufferFrames);
        
        softStartLastSpeed = speed;
      }
    }
    
    jw = false;
    
//    fprintf(stderr, "v");
    
    if (ts)
    {
      stripPTSandDTS((uchar *)Data, Length);
    }
    else if (0) //ZZZ
    {
      int64_t pts = 0;
      
      if (np && getPTS(Data, Length, pts))
      {
        np = false;
        
//        fprintf(stderr, "M %lld %llx\n", pts);
        m_xineLib.execFuncMetronom(pts);
        m_xineLib.execFuncWait();
      }
    }
        
    int r = PlayCommon2(Data, Length);

//    fprintf(stderr, "V");

    return r;
  }

  static uchar jumboPESdata[ 6 + 0xffff ];
  static uchar *jumboPEStailData = 0;
  
  static int frameSizes[ 256 ];
  
  static bool mkJumboPES(const uchar *Data, int Length)
  {
    int origJumboPESsize = jumboPESsize;
    jumboPESsize = 0;
    
    if (Length < 9)
      VERBOSE_RETURN0(false);

    if (0x00 != Data[ 0 ])
      VERBOSE_RETURN0(false);
    
    if (0x00 != Data[ 1 ])
      VERBOSE_RETURN0(false);
    
    if (0x01 != Data[ 2 ])
      VERBOSE_RETURN0(false);
    
    if (0xbd != Data[ 3 ])
      VERBOSE_RETURN0(false);

    int l = Data[ 4 ] * 256 + Data[ 5 ];    
    if ((6 + l) != Length)
    {
      const uchar *data = Data + (6 + l);
      int length = Length - (6 + l);
      
      if (length < 6)
        VERBOSE_RETURN3(false);
      
      if (0x00 != data[ 0 ])
        VERBOSE_RETURN3(false);
      
      if (0x00 != data[ 1 ])
        VERBOSE_RETURN3(false);
      
      if (0x01 != data[ 2 ])
        VERBOSE_RETURN3(false);
      
      if (0xbe != data[ 3 ])
        VERBOSE_RETURN3(false);
 
      int L = data[ 4 ] * 256 + data[ 5 ];    
      if ((6 + L) != length)
        VERBOSE_RETURN3(false);

      // ignore padding
      Length -= length;
    }
/*
    for (int i = 0; i < 20; i++)
      fprintf(stderr, "%02x ", Data[ i ]);
    fprintf(stderr, "\n");
*/    
    bool cont = (0x80 == Data[ 6 ]
                 && 0x00 == Data[ 7 ]
                 && 0x00 == Data[ 8 ]);

    if (cont
        && Length >= 6 + 3 + 5
        && Data[  9 ] == 0x0b
        && Data[ 10 ] == 0x77
        && frameSizes[ Data[ 13 ] ] > 0)
    {
      cont = false;
    }
    
    if (!cont
      || 0 == origJumboPESsize)
    {
      if (0 != origJumboPESsize)
        VERBOSE_RETURN0(false);

      if ((origJumboPESsize + Length - 0) > (6 + 0xffff))
        VERBOSE_RETURN0(false);

      if (jumboPEStail > 0)
      {
        int headerSize = 6 + 3 + Data[ 8 ];
        ::memcpy(&jumboPESdata[ origJumboPESsize ], &Data[ 0 ], headerSize);

        ::memmove(&jumboPESdata[ origJumboPESsize + headerSize ], jumboPEStailData, jumboPEStail);
        
        ::memcpy(&jumboPESdata[ origJumboPESsize + headerSize + jumboPEStail ], &Data[ headerSize ], Length - headerSize);
        
        origJumboPESsize += headerSize + jumboPEStail + Length - headerSize;

        jumboPEStail = 0;
        jumboPEStailData = 0;

        //FIXME: PTS should be adjusted to take care of jumboPEStail's duration.
        //       Otherwise there is a certain jitter on audio duration <=> PTS.
      }
      else
      {
        ::memcpy(&jumboPESdata[ origJumboPESsize ], &Data[ 0 ], Length - 0);
        origJumboPESsize += Length - 0;
      }
    }
    else
    {
      if (0 == origJumboPESsize)
        VERBOSE_RETURN0(false);
      
      if ((origJumboPESsize + Length - 9) > (6 + 0xffff))
        VERBOSE_RETURN0(false);
        
      ::memcpy(&jumboPESdata[ origJumboPESsize ], &Data[ 9 ], Length - 9);
      origJumboPESsize += Length - 9;
    }

    if (0 == origJumboPESsize)
      VERBOSE_RETURN0(false);

    jumboPESsize = origJumboPESsize;

    if (2048 == Length)
    {
//      fprintf(stderr, " b %d", jumboPESsize);
      return false;
    }
    
    jumboPESdata[ 4 ] = (jumboPESsize - 6) >> 8;
    jumboPESdata[ 5 ] = (jumboPESsize - 6) & 0xff;
    
//    fprintf(stderr, " B %d", jumboPESsize);
    return true;
  }

  static int initFrameSizes()
  {
    ::memset(frameSizes, 0, sizeof (frameSizes));

    // fs = 48 kHz
    frameSizes[ 0x00 ] =   64;
    frameSizes[ 0x01 ] =   64;
    frameSizes[ 0x02 ] =   80;
    frameSizes[ 0x03 ] =   80;
    frameSizes[ 0x04 ] =   96;
    frameSizes[ 0x05 ] =   96;
    frameSizes[ 0x06 ] =  112;
    frameSizes[ 0x07 ] =  112;
    frameSizes[ 0x08 ] =  128;
    frameSizes[ 0x09 ] =  128;
    frameSizes[ 0x0a ] =  160;
    frameSizes[ 0x0b ] =  160;
    frameSizes[ 0x0c ] =  192;
    frameSizes[ 0x0d ] =  192;
    frameSizes[ 0x0e ] =  224;
    frameSizes[ 0x0f ] =  224;
    frameSizes[ 0x10 ] =  256;
    frameSizes[ 0x11 ] =  256;
    frameSizes[ 0x12 ] =  320;
    frameSizes[ 0x13 ] =  320;
    frameSizes[ 0x14 ] =  384;
    frameSizes[ 0x15 ] =  384;
    frameSizes[ 0x16 ] =  448;
    frameSizes[ 0x17 ] =  448;
    frameSizes[ 0x18 ] =  512;
    frameSizes[ 0x19 ] =  512;
    frameSizes[ 0x1a ] =  640;
    frameSizes[ 0x1b ] =  640;
    frameSizes[ 0x1c ] =  768;
    frameSizes[ 0x1d ] =  768;
    frameSizes[ 0x1e ] =  896;
    frameSizes[ 0x1f ] =  896;
    frameSizes[ 0x20 ] = 1024;
    frameSizes[ 0x21 ] = 1024;
    frameSizes[ 0x22 ] = 1152;
    frameSizes[ 0x23 ] = 1152;
    frameSizes[ 0x24 ] = 1280;
    frameSizes[ 0x25 ] = 1280;
    
    // fs = 44.1 kHz
    frameSizes[ 0x40 ] =   69;
    frameSizes[ 0x41 ] =   70;
    frameSizes[ 0x42 ] =   87;
    frameSizes[ 0x43 ] =   88;
    frameSizes[ 0x44 ] =  104;
    frameSizes[ 0x45 ] =  105;
    frameSizes[ 0x46 ] =  121;
    frameSizes[ 0x47 ] =  122;
    frameSizes[ 0x48 ] =  139;
    frameSizes[ 0x49 ] =  140;
    frameSizes[ 0x4a ] =  174;
    frameSizes[ 0x4b ] =  175;
    frameSizes[ 0x4c ] =  208;
    frameSizes[ 0x4d ] =  209;
    frameSizes[ 0x4e ] =  243;
    frameSizes[ 0x4f ] =  244;
    frameSizes[ 0x50 ] =  278;
    frameSizes[ 0x51 ] =  279;
    frameSizes[ 0x52 ] =  348;
    frameSizes[ 0x53 ] =  349;
    frameSizes[ 0x54 ] =  417;
    frameSizes[ 0x55 ] =  418;
    frameSizes[ 0x56 ] =  487;
    frameSizes[ 0x57 ] =  488;
    frameSizes[ 0x58 ] =  557;
    frameSizes[ 0x59 ] =  558;
    frameSizes[ 0x5a ] =  696;
    frameSizes[ 0x5b ] =  697;
    frameSizes[ 0x5c ] =  835;
    frameSizes[ 0x5d ] =  836;
    frameSizes[ 0x5e ] =  975;
    frameSizes[ 0x5f ] =  976;
    frameSizes[ 0x60 ] = 1114;
    frameSizes[ 0x61 ] = 1115;
    frameSizes[ 0x62 ] = 1253;
    frameSizes[ 0x63 ] = 1254;
    frameSizes[ 0x64 ] = 1393;
    frameSizes[ 0x65 ] = 1394;
    
    // fs = 32 kHz
    frameSizes[ 0x80 ] =   96;
    frameSizes[ 0x81 ] =   96;
    frameSizes[ 0x82 ] =  120;
    frameSizes[ 0x83 ] =  120;
    frameSizes[ 0x84 ] =  144;
    frameSizes[ 0x85 ] =  144;
    frameSizes[ 0x86 ] =  168;
    frameSizes[ 0x87 ] =  168;
    frameSizes[ 0x88 ] =  192;
    frameSizes[ 0x89 ] =  192;
    frameSizes[ 0x8a ] =  240;
    frameSizes[ 0x8b ] =  240;
    frameSizes[ 0x8c ] =  288;
    frameSizes[ 0x8d ] =  288;
    frameSizes[ 0x8e ] =  336;
    frameSizes[ 0x8f ] =  336;
    frameSizes[ 0x90 ] =  384;
    frameSizes[ 0x91 ] =  384;
    frameSizes[ 0x92 ] =  480;
    frameSizes[ 0x93 ] =  480;
    frameSizes[ 0x94 ] =  576;
    frameSizes[ 0x95 ] =  576;
    frameSizes[ 0x96 ] =  672;
    frameSizes[ 0x97 ] =  672;
    frameSizes[ 0x98 ] =  768;
    frameSizes[ 0x99 ] =  768;
    frameSizes[ 0x9a ] =  960;
    frameSizes[ 0x9b ] =  960;
    frameSizes[ 0x9c ] = 1152;
    frameSizes[ 0x9d ] = 1152;
    frameSizes[ 0x9e ] = 1344;
    frameSizes[ 0x9f ] = 1344;
    frameSizes[ 0xa0 ] = 1536;
    frameSizes[ 0xa1 ] = 1536;
    frameSizes[ 0xa2 ] = 1728;
    frameSizes[ 0xa3 ] = 1728;
    frameSizes[ 0xa4 ] = 1920;
    frameSizes[ 0xa5 ] = 1920;
    
    return 0;
  };
  
#if VDRVERSNUM >= 10318

  int cXineDevice::GetAudioChannelDevice(void)
  {
    return m_audioChannel;
  }

  void cXineDevice::SetAudioChannelDevice(int AudioChannel)
  {
    xfprintf(stderr, "SetAudioChannelDevice: %d\n", AudioChannel);

    m_audioChannel = AudioChannel;

    m_xineLib.execFuncSelectAudio(m_audioChannel);
  }

  void cXineDevice::SetDigitalAudioDevice(bool On)
  {
    xfprintf(stderr, "SetDigitalAudioDevice: %d\n", On);

    m_xineLib.execFuncSelectAudio(On ? -1 : m_audioChannel);

    if (pmNone == pm)
      return;

    if (m_settings.LiveTV()
      && !audioSeen)
    {
      if (softStartState == sIdle)
        softStartTrigger = sstNoMetronom;
      
      return;
    }
    
    m_xineLib.pause();
    
    jumboPESsize = 0;
    jumboPEStail = 0;

    if (f)
        m_xineLib.execFuncSetSpeed(100.0);

    m_xineLib.execFuncResetAudio();

    if (m_settings.LiveTV())
    {
      ptsV = ptsA = ptsP = ptsD = -1;
    
      m_xineLib.execFuncClear(-3);
//      m_xineLib.execFuncStart();
//      np = true;
    }
    
    if (f)
        m_xineLib.execFuncSetSpeed(0.0);
    
    m_xineLib.execFuncWait();
    m_xineLib.pause(false);

    if (m_settings.LiveTV())
      softStartTrigger = sstNoMetronom;
    
fprintf(stderr, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");    
  }
  
  int cXineDevice::PlayAudio(const uchar *Data, int Length)
  {
//    fprintf(stderr, " 0x%02x ", Data[ 3 ]);
    return PlayAudioCommon(Data, Length);
  }
#else
  void cXineDevice::PlayAudio(const uchar *Data, int Length)
  {
    cDevice::PlayAudio(Data, Length);

    PlayAudioCommon(Data, Length);
  }
#endif
  
  int cXineDevice::PlayAudioCommon(const uchar *Data, int Length)
  {
    {
      static int i = initFrameSizes();
      (void)i;
    }
    
store_frame(Data, Length, __LINE__);
 
    LOG_ME(::fprintf(stderr, "A");)
    
    if (f)
    {      
      LOG_ME(::fprintf(stderr, "<");)
      return Length;
    }
    
    if (pmNone == pm)
    {
      cMutexLock pmMutexLock(&m_pmMutex);

      if (pmNone == pm)
        m_pmCondVar.Wait(m_pmMutex);
    }

    int retVal = PlayAudio2(Data, Length);
    
    LOG_ME(::fprintf(stderr, "a"));

    return retVal;
  }
  
  int cXineDevice::PlayAudio2(const uchar *Data, int Length)
  {
//    fprintf(stderr, "D");
    
    int done = 0;

    while (done < Length)
    {
      char ch = 'X';

      int id = 0x00;
      
      int todo = Length - done;
      if (todo >= 6)
      {
        if (0x00 == Data[ done + 0 ]
            && 0x00 == Data[ done + 1 ]
            && 0x01 == Data[ done + 2 ])
        {
          id  = Data[ done + 3 ];
          int len = Data[ done + 4 ] * 0x0100 + Data[ done + 5 ];

          if (todo >= (6 + len))
          {
            todo = (6 + len);

            if (0xbe == id)
            {
              done += todo;

//              ::fprintf(stderr, "x");
              continue;
            }

            ch = '.';            
          }
          else        
          {
            VERBOSE_NOP();
            ch = '3';
          }
        }
        else        
        {
          VERBOSE_NOP();
          ch = '2';
        }
      }
      else        
      {
        VERBOSE_NOP();
        ch = '1';
      }
      
//      ::fprintf(stderr, "%c", ch);

      int r;
      
      if (0xbd == id)
        r = PlayAudio3(Data + done, todo);
      else
        r = PlayVideo3(Data + done, todo, false);
        
      if (r < 0)
        return r;
      
      if (r != todo)
        VERBOSE_NOP();
                
      done += r;
    }
    
    return done;
  }
  
  int cXineDevice::PlayAudio3(const uchar *Data, int Length)
  {
    resetScramblingControl(Data, Length);
    
store_frame(Data, Length, __LINE__);    
/*    
    ::fprintf(stderr, "l: %d\t", Length);
    for (int i = 0; i < 20; i++)
      ::fprintf(stderr, "%02x ", Data[ i ]);
    ::fprintf(stderr, "\n");
*/ 
//    fprintf(stderr, "A");
    
    if (mkJumboPES(Data, Length))
    {
      int todo = jumboPESsize;
      jumboPESsize = 0;

      dumpAudio("Audio", jumboPESdata, todo);

      bool dolby = false;
      bool pcm = false;
      
      do
      {
        if (todo < (6 + 3 + 0 + 2))
          break;
        
        if (0x00 != jumboPESdata[ 0 ]
            || 0x00 != jumboPESdata[ 1 ]
            || 0x01 != jumboPESdata[ 2 ]
            || 0xbd != jumboPESdata[ 3 ])
        {
          break;
        }

        int l = jumboPESdata[ 4 ] * 0x0100 + jumboPESdata[ 5 ];
        if (l < (3 + 0 + 2))
          break;

        if (todo < (6 + l))
          break;

        int h = jumboPESdata[ 8 ];
        if (l < (3 + h + 2))
          break;

        if (0x0b == jumboPESdata[ 6 + 3 + h + 0 ]
            && 0x77 == jumboPESdata[ 6 + 3 + h + 1 ])
        {
          if (l < (3 + h + 2 + 2 + 1))
          {
            VERBOSE_NOP();            
            break;
          }
          
          int frameStart = 6 + 3 + h;
          bool failed = false;
          
          while (true)
          {
            int frameSize = 2 * frameSizes[ jumboPESdata[ frameStart + 4 ] ];
            if (frameSize <= 0)
            {
              failed = true;
              
              xfprintf(stderr, "frame_size_code: 0x%02x\n", jumboPESdata[ frameStart + 4 ]);
              VERBOSE_NOP();              
              break;
            }

            if (frameStart + frameSize > todo)
              break;

            frameStart += frameSize;
            
            if (frameStart + 2 + 2 + 1 > todo)
              break;

            if (0x0b != jumboPESdata[ frameStart + 0 ]
                || 0x77 != jumboPESdata[ frameStart + 1 ])
            {
              failed = true;
              
              VERBOSE_NOP();
              break;
            }
          }
          
          if (failed)
            break;
          
          jumboPEStail = todo - frameStart;
          jumboPEStailData = jumboPESdata + frameStart;

          todo = frameStart;

          jumboPESdata[ 4 + 0 ] = (todo - 6) >> 8;
          jumboPESdata[ 4 + 1 ] = (todo - 6) & 0xff;
          
          dolby = true;
store_frame(jumboPESdata, todo, __LINE__);    
          break;
        }

        if (0x80 == (0xf0 & jumboPESdata[ 6 + 3 + h + 0 ]))
        {
          dolby = true;
          break;
        }
        
        if (0xa0 == (0xf0 & jumboPESdata[ 6 + 3 + h + 0 ]))
        {
          pcm = true;
          break;
        }

        for (int i = 6 + 3 + h; i < todo - 2 - 2 - 1; i++)
        {
          if (0x0b == jumboPESdata[ i + 0 ]
              && 0x77 == jumboPESdata[ i + 1 ]
              && 0 != frameSizes[ jumboPESdata[ i + 4 ] ])
          {
            jumboPEStail = todo - i;
            jumboPEStailData = jumboPESdata + i;
          }
        }
      }
      while (false);

      if (pcm
          || (dolby && m_settings.AudioDolbyOn()))
      {
        int done = 0;
        
        while (done < todo)
        {
          int r = PlayCommon(jumboPESdata + done, todo - done, false);
          if (r < 0)
            return r;
          
//          fprintf(stderr, ".");
          
          done += r;
        }

// Don't return done here as the remaining bytes were buffered elsewhere!
//        return done;
      }
    }
    else if (jumboPESsize == Length)
    {
      int todo = jumboPESsize;

      bool dolby = false;
      bool pcm = false;
      
      do
      {
        if (todo < (6 + 3 + 0 + 2))
          break;
        
        if (0x00 != jumboPESdata[ 0 ]
            || 0x00 != jumboPESdata[ 1 ]
            || 0x01 != jumboPESdata[ 2 ]
            || 0xbd != jumboPESdata[ 3 ])
        {
          break;
        }

        int l = jumboPESdata[ 4 ] * 0x0100 + jumboPESdata[ 5 ];
        if (l < (3 + 0 + 2))
          break;

        if (todo < (6 + l))
          break;

        int h = jumboPESdata[ 8 ];
        if (l < (3 + h + 2))
          break;

        if (0x80 == (0xf0 & jumboPESdata[ 6 + 3 + h + 0 ]))
        {
          dolby = true;
          break;
        }
        
        if (0xa0 == (0xf0 & jumboPESdata[ 6 + 3 + h + 0 ]))
        {
          pcm = true;
          break;
        }
      }
      while (false);

      if (dolby || pcm)
      {
        dumpAudio("Audio", jumboPESdata, todo);

        jumboPESsize = 0;
      }

      if (pcm
          || (dolby && m_settings.AudioDolbyOn()))
      {
        int done = 0;
        
        while (done < todo)
        {
          int r = PlayCommon(jumboPESdata + done, todo - done, false);
          if (r < 0)
            return r;
          
//          fprintf(stderr, ".");
          
          done += r;
        }

// Don't return done here as the remaining bytes were buffered elsewhere!
//        return done;
      }
    }

//    fprintf(stderr, "\n");

    return Length;
  }
  
  bool cXineDevice::GrabImage(const char *FileName, bool Jpeg /* = true */, int Quality /* = -1 */, int SizeX /* = -1 */, int SizeY /* = -1 */)
  {
    xfprintf(stderr, "GrabImage ...\n\n");
    
    if (m_xineLib.execFuncGrabImage(FileName, Jpeg, Quality, SizeX, SizeY))
    {
      xfprintf(stderr, "\nGrabImage succeeded.\n");
      return true;
    }

    xfprintf(stderr, "\nGrabImage failed.\n");    
    return false;
  }

  int64_t cXineDevice::GetSTC(void)
  {
//    ::fprintf(stderr, "GetSTC: ");
    
    int64_t pts = -1;

    if (!m_xineLib.execFuncGetPTS(pts))
      pts = cDevice::GetSTC();

//    ::fprintf(stderr, "%lld\n", pts);
    
    return pts;
  }
  
  void cXineDevice::SetVideoFormat(bool VideoFormat16_9)
  {
    xfprintf(stderr, "SetVideoFormat: %d\n", VideoFormat16_9);
    cDevice::SetVideoFormat(VideoFormat16_9); 
  }

  static bool firstCallToSetVolume = true;
  static void switchSkin(const bool restore);
  
  void cXineDevice::SetVolumeDevice(int Volume)
  {
    if (firstCallToSetVolume)
    {
      firstCallToSetVolume = false;

      if (m_settings.ShallSwitchSkin())
        switchSkin(false);
    }
    
    xfprintf(stderr, "SetVolumeDevice: %d\n", Volume);
    m_xineLib.execFuncSetVolume(Volume);
  }
  
#if VDRVERSNUM < 10307
  cOsdBase *cXineDevice::NewOsd(int x, int y)
#else    
  cOsd *cXineDevice::NewOsd(int x, int y)
#endif    
  {
//    ::fprintf(stderr, "NewOsd ---: %s\n", ::ctime(&(const time_t &)::time(0)));
    cMutexLock osdLock(&m_osdMutex);
//    ::fprintf(stderr, "NesOsd +++: %s\n", ::ctime(&(const time_t &)::time(0)));
    
    if (m_currentOsd)
    {
      xfprintf(stderr, "vdr-xine: OSD requested while another OSD active => Returning NULL!\n");
      return 0;
    }
    
    m_currentOsd = new cXineOsd(*this, x, y);

    return m_currentOsd;
  }

#if VDRVERSNUM < 10307
  void cXineDevice::OnFreeOsd(cOsdBase *const osd)
#else    
  void cXineDevice::OnFreeOsd(cOsd *const osd)
#endif    
  {
    cMutexLock osdLock(&m_osdMutex);
    
    assert(osd == m_currentOsd);

    m_currentOsd = 0;
  }


  
  static cDevice *originalPrimaryDevice = 0;
  
#if VDRVERSNUM >= 10307
  
  void cXineDevice::MakePrimaryDevice(bool On)
  {
    if (On)
      new cXineOsdProvider(*this);
    
    originalPrimaryDevice = 0;
  }
  
#endif    



#if VDRVERSNUM >= 10320
  static cSkin *origSkin = 0;
#endif
  
  static void switchSkin(const bool restore)
  {
#if VDRVERSNUM >= 10320
    if (restore)
    {
      Skins.SetCurrent(origSkin->Name());
      cThemes::Load(Skins.Current()->Name(), Setup.OSDTheme, Skins.Current()->Theme());
    }
    else
    {
      origSkin = Skins.Current();
      Skins.SetCurrent("curses");
      cThemes::Load(Skins.Current()->Name(), Setup.OSDTheme, Skins.Current()->Theme());
    }
#endif
  }
  
  void cXineDevice::reshowCurrentOsd(const bool dontOptimize /* = true */, const int frameWidth /* = -1 */, const int frameHeight /* = -1 */)
  {
    cMutexLock osdLock(&m_osdMutex);
    
    if (m_currentOsd)
      m_currentOsd->ReshowCurrentOsd(dontOptimize, frameWidth, frameHeight);
  }

  void cXineDevice::OnClientConnect()
  {
    reshowCurrentOsd();

    if (m_settings.LiveTV())
      softStartTrigger = sstNormal;

    if (m_settings.AutoPrimaryDevice())
    {
      cDevice *primaryDevice = cDevice::PrimaryDevice();
      if (this != primaryDevice)
        SetPrimaryDevice(1 + DeviceNumber());
      originalPrimaryDevice = primaryDevice;
    }

    if (m_settings.ShallSwitchSkin())
      switchSkin(true);
  }

  void cXineDevice::OnClientDisconnect()
  {
    if (m_settings.ShallSwitchSkin())
      switchSkin(false);

    if (m_settings.AutoPrimaryDevice()
      && originalPrimaryDevice)
    {
      if (this != originalPrimaryDevice)
        SetPrimaryDevice(1 + originalPrimaryDevice->DeviceNumber());
    }
  }
  
  void cXineDevice::ReshowCurrentOSD(const int frameWidth, const int frameHeight)
  {
//    ::fprintf(stderr, ">>> cXineDevice::ReshowCurrentOSD()\n");
    reshowCurrentOsd(false, frameWidth, frameHeight);
//    ::fprintf(stderr, "<<< cXineDevice::ReshowCurrentOSD()\n");
  }

  bool cXineDevice::open()
  {
    if (!m_xineLib.Open())
      return false;

    if (m_settings.ShallSwitchSkin())
    {
#if VDRVERSNUM >= 10320
      Skins.SetCurrent(Setup.OSDSkin);
      cThemes::Load(Skins.Current()->Name(), Setup.OSDTheme, Skins.Current()->Theme());
#endif
      switchSkin(false);
    }
    
    return true;
  }

  void cXineDevice::close()
  {
    m_xineLib.Close();

    if (m_settings.ShallSwitchSkin())
      switchSkin(true);
  }

  void cXineDevice::Stop()
  {
    if (!theXineDevice)
      return;

    theXineDevice->close();
  }

  cXineDevice::cXineDevice(cPlugin *const plugin, cXineSettings &settings, cXineRemote *remote)
    : cDevice()
    , m_settings(settings)
    , m_currentOsd(0)
    , m_spuDecoder(0)
    , m_audioChannel(0)
    , m_xineLib(plugin, settings, m_osdMutex, remote)
  {
    m_xineLib.SetEventSink(this);
  }
  
  cXineDevice::~cXineDevice()
  {
#if VDRVERSNUM < 10320
    close();
#endif
    if (m_spuDecoder)
      delete m_spuDecoder;
  }
  
  bool cXineDevice::Create(cPlugin *const plugin, cXineSettings &settings, cXineRemote *remote)
  {
    if (theXineDevice)
      return false;

    theXineDevice = new cXineDevice(plugin, settings, remote);
    
    return 0 != theXineDevice
      && theXineDevice->hasNoSignalStream();
  }

  bool cXineDevice::Open()
  {
    if (!theXineDevice)
      return false;

    return theXineDevice->open();
  }

  cXineDevice *cXineDevice::GetDevice()
  {
    return theXineDevice;
  }

};
