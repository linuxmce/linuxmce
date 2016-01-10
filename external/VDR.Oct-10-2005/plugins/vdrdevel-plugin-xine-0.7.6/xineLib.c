
#include "xineCommon.h"
#include <endian.h>

#include <vdr/plugin.h>

#include "xineLib.h"
#include "xineOsd.h"
#include "xineSettings.h"



#define ASSERT_PALETTE(x)
//#define ASSERT_PALETTE(x) x

#define PROFILE_SCALING(x)
//#define PROFILE_SCALING(x) x

#define DLOG(x)
//#define DLOG(x) cDlog dlog(x) 



namespace PluginXine
{
  class cDlog
  {
    char m_s[ 100 ];
    
  public:
    cDlog(char *s)
    {
      ::strncpy(m_s, s, sizeof(m_s));
      
      ::fprintf(stderr, ">>> %s\n", m_s);
    }
    
    ~cDlog()
    {
      ::fprintf(stderr, "<<< %s\n", m_s);
    }
  }; 

 
#if VDRVERSNUM < 10307
  
  bool cXineLib::OpenWindow(cXineOsd *const xineOsd, cWindow *Window)
  {
    execFuncOsdNew(Window->Handle(), xineOsd->X0() + Window->X0(), xineOsd->Y0() + Window->Y0(), Window->Width(), Window->Height());

    CommitWindow(xineOsd, Window, false);
    
    return true;
  }

  void cXineLib::CommitWindow(cXineOsd *const xineOsd, cWindow *Window, const bool optimize /* = true */)
  {
    int firstColorIndex = -1;
    int lastColorIndex = -1;
    const eDvbColor *modifiedColors = (optimize ? Window->NewColors(firstColorIndex, lastColorIndex) : Window->AllColors(lastColorIndex));

    if (!optimize)
    {
      firstColorIndex = 0;
      lastColorIndex--;
    }
    
    if (modifiedColors)
    {
      do
      {
	for (int colorIndex = firstColorIndex
	       ; colorIndex <= lastColorIndex
	       ; colorIndex++, modifiedColors++)
	{
          execFuncSetColor(Window->Handle(), colorIndex, filterAlpha(*modifiedColors));
        }
      }
      while (0 != (modifiedColors = Window->NewColors(firstColorIndex, lastColorIndex)));
    }
    
//    xine_osd_draw_bitmap(m_osdWindow[ Window->Handle() ], (uint8_t *)Window->Data(0, 0), 0, 0, Window->Width(), Window->Height(), &m_mapToXinePalette[ Window->Handle() ][ 0 ]);

    int x1 = 0, y1 = 0, x2 = Window->Width() - 1, y2 = Window->Height() - 1;

    if (!optimize
        || modifiedColors
        || Window->Dirty(x1, y1, x2, y2))
    {
//      fprintf(stderr, "dirty area: (%d, %d) - (%d, %d), window size: (%d, %d)\n", x1, y1, x2, y2, Window->Width(), Window->Height());

      execFuncOsdDrawBitmap(Window->Handle(), (uint8_t *)Window->Data(x1, y1), x1, y1, x2 - x1 + 1, y2 - y1 + 1, Window->Width());

      if (m_osdWindowVisible[ Window->Handle() ])
        execFuncOsdShow(Window->Handle());
    }
  }

  void cXineLib::ShowWindow(cXineOsd *const xineOsd, cWindow *Window)
  {
    execFuncOsdShow(Window->Handle());

    m_osdWindowVisible[ Window->Handle() ] = true;
  }

  void cXineLib::HideWindow(cXineOsd *const xineOsd, cWindow *Window, bool Hide)
  {
    if (Hide)
    {
      execFuncOsdHide(Window->Handle());
    }
    else
    {
      execFuncOsdShow(Window->Handle());
    }
    
    m_osdWindowVisible[ Window->Handle() ] = !Hide;
  }

  void cXineLib::MoveWindow(cXineOsd *const xineOsd, cWindow *Window, int x, int y)
  {
    execFuncOsdSetPosition(Window->Handle(), xineOsd->X0() + x, xineOsd->Y0() + y);

    if (m_osdWindowVisible[ Window->Handle() ])
      execFuncOsdShow(Window->Handle());
  }

  void cXineLib::CloseWindow(cXineOsd *const xineOsd, cWindow *Window)
  {
    CloseWindow(xineOsd, Window->Handle());
  }

  void cXineLib::CloseWindow(cXineOsd *const xineOsd, int Window)
  {
    if (m_osdWindowVisible[ Window ])
      execFuncOsdHide(Window);

    execFuncOsdFree(Window);

    m_osdWindowVisible[ Window ] = false;
  }

#else

#define STD_VIDEO_WIDTH  720
#define STD_VIDEO_HEIGHT 576  

  bool cXineLib::execFuncOsdNew(const eNeedsScaling needsScaling, const int videoLeft, const int videoTop, const int videoWidth, const int videoHeight, int window, int x, int y, int width, int height)
  {
    m_osdFlushRequired = true;
    
    if (!needsScaling)
      return execFuncOsdNew(window, videoLeft + x, videoTop + y, width, height);

    int x0 =   x           * videoWidth                          / STD_VIDEO_WIDTH;
    int w0 = ((x + width)  * videoWidth  - 1 + STD_VIDEO_WIDTH)  / STD_VIDEO_WIDTH  - x0;
    int y0 =   y           * videoHeight                         / STD_VIDEO_HEIGHT;
    int h0 = ((y + height) * videoHeight - 1 + STD_VIDEO_HEIGHT) / STD_VIDEO_HEIGHT - y0;
    
    return execFuncOsdNew(window, videoLeft + x0, videoTop + y0, w0, h0);
  }

  static inline int intersection(int a0, int a1, int b0, int b1)
  {
    if (a0 < b0)
      a0 = b0;

    if (a1 > b1)
      a1 = b1;
    
    int d = a1 - a0;
    if (d <= 0)
      return 0;

    return d;
  }

//#define CRT_GAMMA (2.2)
#define FIX_POINT_BITS (6)   // * 3 + 8 + ceil(ld(ceil(1/scale_x) * ceil(1/scale_y))) < 32 !!!
#define FIX_POINT_FACTOR (1 << FIX_POINT_BITS)
  
  static uint16_t data_delinearize[ 255 * FIX_POINT_FACTOR + 1 ];

  static int init_delinearize(int crtGamma)
  {
    for (int i = 0; i <= 255 * FIX_POINT_FACTOR; i++)
    {
      data_delinearize[ i ] = (int)(::exp(::log(i / (255.0 * FIX_POINT_FACTOR)) / (crtGamma / (double)cXineSettings::monitorGammaBase)) * (255.0 * FIX_POINT_FACTOR) + .5);

//      ::fprintf(stderr, "de_lin: %d => %d\n", i, data_delinearize[ i ]);

      ASSERT_PALETTE(assert(data_delinearize[ i ] <= 255 * FIX_POINT_FACTOR);)
    }

    return 0;
  }
  
  static inline uint16_t delinearize(uint16_t i)
  {
//    static int init = init_delinearize();
//    (void)init;
    
    ASSERT_PALETTE(assert(i <= 255 * FIX_POINT_FACTOR);)
    
    return data_delinearize[ i ];
  }
  
  static uint16_t data_linearize[ 255 * FIX_POINT_FACTOR + 1 ];

  static int init_linearize(int crtGamma)
  {
    for (int i = 0; i <= 255 * FIX_POINT_FACTOR; i++)
    {
      data_linearize[ i ] = (int)(::exp(::log(i / (255.0 * FIX_POINT_FACTOR)) * (crtGamma / (double)cXineSettings::monitorGammaBase)) * (255.0 * FIX_POINT_FACTOR) + .5);

//      ::fprintf(stderr, "lin: %d => %d\n", i, data_linearize[ i ]);
    
      ASSERT_PALETTE(assert(data_linearize[ i ] <= 255 * FIX_POINT_FACTOR);)
    }

    return 0;
  }
  
  static inline uint16_t linearize(uint16_t i)
  {
//    static int init = init_linearize();
//    (void)init;

    ASSERT_PALETTE(assert(i <= 255 * FIX_POINT_FACTOR);)
    
    return data_linearize[ i ];
  }

  typedef uint64_t tColor16;
  
  static inline void mkLinearColor(const tColor &color, tColor16 &color16)
  {
    const uint8_t *src = (const uint8_t *)&color;
    uint16_t *dst = (uint16_t *)&color16;

#if __BYTE_ORDER == LITTLE_ENDIAN      
    *dst++ = linearize(FIX_POINT_FACTOR * *src++);
    *dst++ = linearize(FIX_POINT_FACTOR * *src++);
    *dst++ = linearize(FIX_POINT_FACTOR * *src++);
    *dst++ =          (FIX_POINT_FACTOR * *src++);
#else
    *dst++ =          (FIX_POINT_FACTOR * *src++);
    *dst++ = linearize(FIX_POINT_FACTOR * *src++);
    *dst++ = linearize(FIX_POINT_FACTOR * *src++);
    *dst++ = linearize(FIX_POINT_FACTOR * *src++);
#endif
//    ::fprintf(stderr, "c: %08x, c16: %016llx\n", color, color16);
  }

  static inline void mkDelinearColor(const tColor16 &color16, tColor &color)
  {
    const uint16_t *src = (const uint16_t *)&color16;
    uint8_t *dst = (uint8_t *)&color;
      
#if __BYTE_ORDER == LITTLE_ENDIAN      
    *dst++ = delinearize(*src++) / FIX_POINT_FACTOR;
    *dst++ = delinearize(*src++) / FIX_POINT_FACTOR;
    *dst++ = delinearize(*src++) / FIX_POINT_FACTOR;
    *dst++ =            (*src++) / FIX_POINT_FACTOR;
#else
    *dst++ =            (*src++) / FIX_POINT_FACTOR;
    *dst++ = delinearize(*src++) / FIX_POINT_FACTOR;
    *dst++ = delinearize(*src++) / FIX_POINT_FACTOR;
    *dst++ = delinearize(*src++) / FIX_POINT_FACTOR;
#endif
//    ::fprintf(stderr, "c16: %016llx, c: %08x\n", color16, color);
  }

  class cXinePalette
  {
#define maxSize 65521
  public:
    class cEntry
    {
      friend class cXinePalette;
      
      int m_index;
    public:
      tColor m_color;
      uint32_t m_count;
    private:
      
      cEntry *m_prev, *m_next;

      static int comparePointersByCountDesc(const void *lhs, const void *rhs)
      {
        const cEntry *cLhs = *(const cEntry **)lhs;
        const cEntry *cRhs = *(const cEntry **)rhs;
        
        if (cLhs->m_count < cRhs->m_count)
          return 1;

        if (cLhs->m_count > cRhs->m_count)
          return -1;
    
        return 0;
      }

      static long colorDistance(const tColor c1, const tColor c2)
      {
        int c1a = (c1 & 0xff000000) >> 0x18;
        int c1r = (c1 & 0x00ff0000) >> 0x10;
        int c1g = (c1 & 0x0000ff00) >> 0x08;
        int c1b = (c1 & 0x000000ff) >> 0x00;

        int c2a = (c2 & 0xff000000) >> 0x18;
        int c2r = (c2 & 0x00ff0000) >> 0x10;
        int c2g = (c2 & 0x0000ff00) >> 0x08;
        int c2b = (c2 & 0x000000ff) >> 0x00;
        
        long r,g,b,a;
        long rmean;
        
        rmean = (c1r + c2r) / 2;
        r = c1r - c2r;
        g = c1g - c2g;
        b = c1b - c2b;
	a = c1a - c2a;
        
        return (((512 + rmean) * r * r) >> 8) + 4 * g * g + (((767 - rmean) * b * b) >> 8) + a * a;
      }
      
    public:
      int getIndex() const
      {
        return m_index;
      }
    };

  private:
    cEntry m_palette[ maxSize ];
    int m_numEntries;

    cEntry *m_lut[ 257 ][ 257 ];
    
    bool m_warned;
    
    cEntry *m_lookup[ maxSize ];
    const cEntry *const *const m_lookupLimit;
    
    const int m_numPalColors;
    
    cEntry m_head, m_tail;

    cEntry *(cXinePalette::*m_add)(const tColor &color);

    cEntry *addHash(const tColor &color)
    {
      cEntry **lookup = m_lookup + (color % maxSize);
      cEntry *entry = *lookup;
      
      while (entry)
      {        
        if (color == entry->m_color)
        {
          entry->m_count++;
          return entry;
        }

        if (++lookup >= m_lookupLimit)
          lookup = m_lookup;

        entry = *lookup;
      }

//      ::fprintf(stderr, "i: %d, c: %08x\n", m_numEntries, color);
      
      entry = m_palette + m_numEntries++;
      *lookup = entry;
        
      entry->m_color = color;
      entry->m_count = 1;

      return entry;
    }
    
    void addHead(cEntry *const node)
    {
      node->m_next = m_head.m_next;
      node->m_prev = &m_head;
      
      node->m_next->m_prev = node;
      node->m_prev->m_next = node;
    }
          
    void remNode(cEntry *const node)
    {
      node->m_next->m_prev = node->m_prev;
      node->m_prev->m_next = node->m_next;
    }
    
    cEntry *addLru(const tColor &color)
    {
      cEntry *entry = m_head.m_next;

      while (entry != &m_tail)
      {
        if (color == entry->m_color)
        {
          if (entry->m_prev != &m_head)
          {
            remNode(entry);
            addHead(entry);
          }
          
          entry->m_count++;
          return entry;
        }

        entry = entry->m_next;
      }

//      ::fprintf(stderr, "i: %d, c: %08x\n", m_numEntries, color);
      
      entry = m_palette + m_numEntries++;
      addHead(entry);
        
      entry->m_color = color;
      entry->m_count = 1;

      return entry;
    }

  public:
    uint8_t m_vdrMapping[ 256 ];
    
    cXinePalette(cXineLib *xineLib, const int numVdrColors, const tColor *const colors = 0, const int numPalColors = 0, const tColor *const palColors = 0, const int transparentIndex = -1)
      : m_numEntries(0)
      , m_warned(false)
      , m_lookupLimit(m_lookup + sizeof (m_lookup) / sizeof (*m_lookup))
      , m_numPalColors(numPalColors)
    {
//      fprintf(stderr, "numVdrColors: %d, numPalColors: %d\n", numVdrColors, numPalColors);
      
      if (numVdrColors >= 16)
      {
        PROFILE_SCALING(::fprintf(stderr, "hash: ");)
        
        ::memset(m_lookup, 0, sizeof (m_lookup));

        m_add = &cXinePalette::addHash;
      }
      else
      {
        PROFILE_SCALING(::fprintf(stderr, "lru: ");)
        
        m_head.m_prev = 0;
        m_head.m_next = &m_tail;
        
        m_tail.m_prev = &m_head;
        m_tail.m_next = 0;

        m_add = &cXinePalette::addLru;
      }

      ::memset(m_lut, 0, sizeof (m_lut));

      if (colors)
      {
        for (int i = 0; i < numVdrColors; i++)
        {
          cEntry *e = addUnused(xineLib->filterAlpha(colors[ i ]));
          m_vdrMapping[ i ] = e - m_palette;
        }
      }

      if (transparentIndex != -1)
      {
        ASSERT_PALETTE(assert(transparentIndex == m_numEntries);)

        addUnused(clrTransparent);
      }
      
      if (palColors)
      {
        for (int i = 0; i < numPalColors; i++)
          addUnused(palColors[ i ]);
      }      
    }

    cEntry *add(const tColor &color)
    {
      if (m_numEntries < maxSize)
        return (this->*m_add)(color);

      ASSERT_PALETTE(
      {
        if (!m_warned)
        {
          m_warned = true;
          
          ::fprintf(stderr, "warning: cXinePalette is full!\n");
        }
      })

      return 0;
    }
    
    void addPersistent(const tColor &color)
    {
      cEntry *const entry = add(color);
      assert(entry);
      
      entry->m_count = 0x7fffffff;
    }

    cEntry *addUnused(const tColor &color)
    {
      cEntry *const entry = add(color);
      assert(entry);
      
      entry->m_count = 0;

      return entry;
    }

    cEntry *add(const uint16_t lutIndex)
    {
      ASSERT_PALETTE(assert(lutIndex <= 256);)
      ASSERT_PALETTE(assert(lutIndex < m_numEntries);)
      
      cEntry *const entry = &m_palette[ lutIndex ];
      entry->m_count++;
      
      return entry;
    }

    cEntry *add(const uint16_t lutIndexA, const uint16_t lutIndexB)
    {
      ASSERT_PALETTE(assert(lutIndexA <= 256);)
      ASSERT_PALETTE(assert(lutIndexB <= 256);)

      cEntry *&entry = m_lut[ lutIndexA ][ lutIndexB ];
      
      if (!entry)
      {
        if (lutIndexA == lutIndexB)
        {
          entry = add(lutIndexA);
          return entry;
        }
        
        tColor16 colorA, colorB;

        ASSERT_PALETTE(assert(lutIndexA < m_numEntries);)
        ASSERT_PALETTE(assert(lutIndexB < m_numEntries);)
      
        mkLinearColor(m_palette[ lutIndexA ].m_color, colorA);
        mkLinearColor(m_palette[ lutIndexB ].m_color, colorB);

        tColor16 colorC;
        tColor color;
        
        //colorC = (((colorA ^ colorB) & 0xfffefffefffefffeULL) >> 1) + (colorA & colorB); // doesn't work with ULL and -O2: buggy compiler???
        {
          uint16_t *pA = (uint16_t *)&colorA, *pB = (uint16_t *)&colorB, *pC = (uint16_t *)&colorC;
          for (int i = 0; i < 4; i++)
          {
            *pC = (((*pA ^ *pB) & 0xfffe) >> 1) + (*pA & *pB);
            
            pA++;
            pB++;
            pC++;
          }
        }

//        fprintf(stderr, "a: 0x%016llx, b: 0x%016llx, c: 0x%016llx\n", colorA, colorB, colorC);

        ASSERT_PALETTE(
        {
          uint16_t *c = (uint16_t *)&colorC;

          if (c[ 0 ] > (255 * FIX_POINT_FACTOR)
              || c[ 1 ] > (255 * FIX_POINT_FACTOR)
              || c[ 2 ] > (255 * FIX_POINT_FACTOR)
              || c[ 3 ] > (255 * FIX_POINT_FACTOR))
          {
            ::fprintf(stderr, "%d, 0x%08x, %d, 0x%08x\n, A: 0x%016llx\n, B: 0x%016llx\n, C: 0x%016llx\n, %d\n", lutIndexA, m_palette[ lutIndexA ].m_color, lutIndexB, m_palette[ lutIndexB ].m_color, colorA, colorB, colorC, -1);
          }
        })
        
        mkDelinearColor(colorC, color);
/*        
        fprintf(stderr, "a: 0x%08lx, b: 0x%08lx, c: 0x%08lx\n"
                , m_palette[ lutIndexA ].m_color
                , m_palette[ lutIndexB ].m_color
                , color);
*/        
        m_lut[ lutIndexB ][ lutIndexA ] = entry = add(color);
        return entry;
      }

      entry->m_count++;      
      return entry;
    }

    int getNum() const
    {
      return m_numEntries;
    }
    
    tColor *getVdrColors(int &numColors)
    {
      int usedColors = numColors = m_numEntries;
      int usedIndex = numColors - 1;

      if (numColors > 256)
      {
        usedColors = 0;
        usedIndex = 0;
        
        {
          cEntry *src = m_palette;
          
          for (int i = 0; i < m_numEntries; i++)
          {
            if ((src++)->m_count > 0)
            {
              usedColors++;
              usedIndex = i;
            }
          }
        }

        if (usedColors > 256)
        {
          numColors = 256;
        }
        else
        {
          numColors = usedColors;

          if (usedIndex < 256)
          {
            if (numColors <= usedIndex)
              numColors = usedIndex + 1;
          }
        }
      }

      if (0 == numColors)
        return 0;

      tColor *const colors = new tColor[ numColors ];

//      ASSERT_PALETTE(::fprintf(stderr, "used: %d, nc: %d, index: %d, ", usedColors, numColors, usedIndex);)
      
      if (usedColors > numColors)
      {
        cEntry *sorted[ maxSize ];

        {
          cEntry *src = m_palette;
          cEntry **dst = sorted;
          
          for (int i = 0; i < m_numEntries; i++)
          {
            if (src->m_count > 0)
              *dst++ = src;

            src++;
          }
          
          ASSERT_PALETTE(assert((dst - sorted) == usedColors);)
        }

        ::qsort(sorted, usedColors, sizeof (*sorted), cEntry::comparePointersByCountDesc);
        
        cEntry **src = sorted;

        {
          tColor *dst = colors;
          
          for (int i = 0; i < numColors; i++)
          {
            *dst++ = (*src)->m_color;

            (*src++)->m_index = i;            
          }
        }
        
        for (int i = numColors; i < usedColors; i++)
        {
          cEntry *const entry = *src++;
          
          {
            cEntry **src = sorted;

            tColor bestMatchDelta = cEntry::colorDistance((*src)->m_color, entry->m_color);
            entry->m_index = 0;
            
            for (int k = 1; k < numColors; k++)
            {
              ++src;
              
              tColor delta = cEntry::colorDistance((*src)->m_color, entry->m_color);
              
              if (bestMatchDelta > delta)
              {
                bestMatchDelta = delta;
                entry->m_index = k;
              }
            }
          }
        }
      }
      else
      {
        cEntry *src = m_palette;
        tColor *dst = colors;

        int index = 0;

        if (usedIndex > m_numPalColors)
        {
          int num = m_numEntries;
          if (num > 256)
            num = usedIndex + 1;
          
          for (int i = 0; i < num; i++)
          {
            if (src->m_count > 0
              || m_numEntries < 256)
            {
              *dst++ = src->m_color;
              src->m_index = index++;

//              fprintf(stderr, "%3d: 0x%08x\n", index - 1, src->m_color);
              
              ASSERT_PALETTE(assert(index <= numColors);)
            }
            
            src++;
          }
        }
        else
        {
          ASSERT_PALETTE(assert(usedIndex < numColors);)
            
          for (int i = 0; i < numColors; i++)
          {
            *dst++ = src->m_color;
            (src++)->m_index = i;
          }
        }
      }

      return colors;
    }
  };

  static inline double tt(const timeval &tb, const timeval &ta)
  {
    return (tb.tv_sec + tb.tv_usec * 1e-6) - (ta.tv_sec + ta.tv_usec * 1e-6);
  }

  template <const int yInc = 1, class T = int>
    class cBresenham
  {
    const int m_dx;
    const int m_dy;
    
    int m_eps;    
    T m_y;
    
  public:
    cBresenham(const int dy, const int dx, const int eps, T const y0 = 0)
      : m_dx(dx)
      , m_dy(dy)
      , m_eps(eps - m_dx)
      , m_y(y0)
    {
    }

    int eps() const
    {
      return m_eps;
    }
    
    T step()
    {
      m_eps += m_dy;

      while (m_eps >= 0)
      {
        m_eps -= m_dx;

        m_y += yInc;
      }

      return m_y;
    }

    T step(int n)
    {
      if (n <= 0)
        return m_y;
      
      while (--n > 0)
        step();

      return step();
    }

    T stepRelative(int n = 1)
    {
      T const y = m_y;
      
      return step(n) - y;
    }
  };
  
  static uint8_t *ScaleBitmapSHQ(const uint8_t *src, int x0, int y0, int w, int h, int ws, int hs, int x1, int y1, int w1, int h1, const uint8_t transparentIndex, const tColor *const colors, const int numColors, tColor *&palette2, int &paletteSize, cXineLib *xineLib)
  {
    timeval t0, t1, t2, t3, t4;
    (void)t0, (void)t1, (void)t2, (void)t3, (void)t4;

    PROFILE_SCALING(::gettimeofday(&t0, 0);)
    
    tColor16 *const screen2 = new tColor16[ STD_VIDEO_HEIGHT * STD_VIDEO_WIDTH ];
    {
      tColor16 *const colors16 = new tColor16[ numColors ];

      for (int i = 0; i < numColors; i++)
        mkLinearColor(xineLib->filterAlpha(colors[ i ]), colors16[ i ]);

      tColor16 clrTransparent16;
      mkLinearColor(clrTransparent, clrTransparent16);
      
      int x1 = x0 + w;
      int y1 = y0 + h;
      int x2 = STD_VIDEO_WIDTH;
      int y2 = STD_VIDEO_HEIGHT;

      if (x1 > x2)
        x1 = x2;

      if (y1 > y2)
        y1 = y2;
      
      tColor16 *dst = screen2;
      
      for (int y = 0; y < y0; y++)
      {
        for (int x = 0; x < x2; x++) 
          *dst++ = clrTransparent16;
      }
      
      for (int y = y0; y < y1; y++)
      {
        for (int x = 0; x < x0; x++) 
          *dst++ = clrTransparent16;
        
        for (int x = x0; x < x1; x++) 
          *dst++ = colors16[ *src++ ];
        
        for (int x = x1; x < x2; x++) 
          *dst++ = clrTransparent16;
      }
      
      for (int y = y1; y < y2; y++)
      {
        for (int x = 0; x < x2; x++) 
          *dst++ = clrTransparent16;
      }

      delete [] colors16;
    }

    uint8_t *const scaled = new uint8_t[ hs * ws ];

    PROFILE_SCALING(::gettimeofday(&t1, 0);)

//    cXinePalette xinePalette(numColors); // fails in getVdrColors(): stack???
    cXinePalette *const pXinePalette = new cXinePalette(xineLib, numColors);
    cXinePalette &xinePalette = *pXinePalette;
    xinePalette.addPersistent(clrTransparent);
    xinePalette.addPersistent(clrGray50);
    xinePalette.addPersistent(clrBlack);
    xinePalette.addPersistent(clrRed);
    xinePalette.addPersistent(clrGreen);
    xinePalette.addPersistent(clrYellow);
    xinePalette.addPersistent(clrMagenta);
    xinePalette.addPersistent(clrBlue);
    xinePalette.addPersistent(clrCyan);
    xinePalette.addPersistent(clrWhite);
    
    cXinePalette::cEntry **const scaled2 = new cXinePalette::cEntry*[ hs * ws ];
    {
      int x2 = x1 + w1;
      int y2 = y1 + h1;

      if (x2 > ws)
      {
        x2 = ws;

        w1 = x2 - x1;
        if (w1 < 0)
          w1 = 0;
      }

      if (y2 > hs)
      {
        y2 = hs;

        h1 = y2 - y1;
        if (h1 < 0)
          h1 = 0;
      }

      cXinePalette::cEntry **scaled2dst0 = scaled2 + y1 * ws + x1;

      cBresenham<> yBh(FIX_POINT_FACTOR * STD_VIDEO_HEIGHT, hs, 0);
      int yf0 = yBh.step(y1); //FIX_POINT_FACTOR * y1 * STD_VIDEO_HEIGHT / hs;
      
      cBresenham<> xBh0(FIX_POINT_FACTOR * STD_VIDEO_WIDTH, ws, 0);
      xBh0.step(x1); //FIX_POINT_FACTOR * x1 * STD_VIDEO_WIDTH / ws;
        
      cBresenham<STD_VIDEO_WIDTH, tColor16 *> yyBh(STD_VIDEO_HEIGHT, hs, 0, screen2);
      tColor16 *screen20 = yyBh.step(y1); //y1 * STD_VIDEO_HEIGHT / hs;
      
      cBresenham<> xxBh0(STD_VIDEO_WIDTH, ws, 0);
      xxBh0.step(x1); //x1 * STD_VIDEO_WIDTH / ws;
        
      for (int y = y1; y < y2; y++)
      {
        const int yf1 = yBh.step(); //FIX_POINT_FACTOR * (y + 1) * STD_VIDEO_HEIGHT / hs;
        const int yi00 = yf0 & ~(FIX_POINT_FACTOR - 1);
        const int yi10 = yi00 + FIX_POINT_FACTOR;
          
        cXinePalette::cEntry **scaled2dst = scaled2dst0;

        cBresenham<> xBh(xBh0);
        int xf0 = xBh.step(0); //FIX_POINT_FACTOR * x1 * STD_VIDEO_WIDTH / ws;
        
        cBresenham<> xxBh(xxBh0);
        int xxx = xxBh.step(0); //x1 * STD_VIDEO_WIDTH / ws;
      
        for (int x = x1; x < x2; x++)
        {
          const int xf1 = xBh.step(); //FIX_POINT_FACTOR * (x + 1) * STD_VIDEO_WIDTH / ws;
          const int xi00 = xf0 & ~(FIX_POINT_FACTOR - 1);
          const int xi10 = xi00 + FIX_POINT_FACTOR;          

//          ::fprintf(stderr, "(%d, %d) => (%d, %d)\n", x, y, xf0, yf0);
          
          tColor16 *screen2src0 = screen20 + xxx; //&screen2[ (yi00 / FIX_POINT_FACTOR) * STD_VIDEO_WIDTH + (xi00 / FIX_POINT_FACTOR) ];
                
          int sai = 0;
          int c0 = 0;
          int c1 = 0;
          int c2 = 0;
          int c3 = 0;

          for (int yi0 = yi00, yi1 = yi10; yi0 < yf1; yi0 = yi1, yi1 += FIX_POINT_FACTOR)
          {
            int aiy = intersection(yi0, yi1, yf0, yf1);

            int saix = 0;
            int c0x  = 0;
            int c1x  = 0;
            int c2x  = 0;
            int c3x  = 0;

            tColor16 *screen2src = screen2src0;
            
            for (int xi0 = xi00, xi1 = xi10; xi0 < xf1; xi0 = xi1, xi1 += FIX_POINT_FACTOR)
            {
//              ::fprintf(stderr, "-(%d, %d)\n", xi / 256, yi / 256);
          
              int aix = intersection(xi0, xi1, xf0, xf1);

              saix += aix;

              uint16_t *c = (uint16_t *)screen2src++;
              
              c0x += aix * *c++;
              c1x += aix * *c++;
              c2x += aix * *c++;
              c3x += aix * *c++;
            }

            screen2src0 += STD_VIDEO_WIDTH;
            
            sai += aiy * saix;
            
            c0  += aiy * c0x;
            c1  += aiy * c1x;
            c2  += aiy * c2x;
            c3  += aiy * c3x;
          }

          tColor color;
          uint8_t *c = (uint8_t *)&color;

#if __BYTE_ORDER == LITTLE_ENDIAN      
          *c++ = delinearize(c0 / sai) / FIX_POINT_FACTOR;
          *c++ = delinearize(c1 / sai) / FIX_POINT_FACTOR;
          *c++ = delinearize(c2 / sai) / FIX_POINT_FACTOR;
          *c++ =            (c3 / sai) / FIX_POINT_FACTOR;
#else
          *c++ =            (c0 / sai) / FIX_POINT_FACTOR;
          *c++ = delinearize(c1 / sai) / FIX_POINT_FACTOR;
          *c++ = delinearize(c2 / sai) / FIX_POINT_FACTOR;
          *c++ = delinearize(c3 / sai) / FIX_POINT_FACTOR;
#endif
          *scaled2dst++ = xinePalette.add(color);

          xxx = xxBh.step(); //x1 * STD_VIDEO_WIDTH / ws;
        
          xf0 = xf1;
        }

        scaled2dst0 += ws;
        
        screen20 = yyBh.step(); //y1 * STD_VIDEO_HEIGHT / hs;
        
        yf0 = yf1;
      }

      PROFILE_SCALING(::gettimeofday(&t2, 0);)
        
      palette2 = xinePalette.getVdrColors(paletteSize);

      PROFILE_SCALING(::gettimeofday(&t3, 0);)

      {
        cXinePalette::cEntry **src = scaled2;
        uint8_t *dst = scaled;
        
        for (int y = 0; y < hs; y++)
        {
          for (int x = 0; x < ws; x++)
          {
            *dst++ = (*src) ? (*src)->getIndex() : 0;
            src++;
          }
        }
      }
      
      PROFILE_SCALING(
      {
        ::gettimeofday(&t4, 0);

        ::fprintf(stderr, "num: %d, new: %d, ", numColors, xinePalette.getNum());

        ::fprintf(stderr, "t1: %.5lf, t2: %.5lf, t3: %.5lf, t4: %.5lf, to: %.5lf, sc: %.5lf %%\n", tt(t1, t0), tt(t2, t1), tt(t3, t2), tt(t4, t3), tt(t4, t0), tt(t2, t1) / tt(t4, t0) * 100);
      })
    }

/*    
    char fName[ 50 ];
    static int osdCnt = 0;
    
    ::sprintf(fName, "/tmp/osd_%05da.ppm", osdCnt);
    
    FILE *f = ::fopen(fName, "wb");

    ::fprintf(f, "P6\n%d %d\n255\n", STD_VIDEO_WIDTH, STD_VIDEO_HEIGHT);

    for (int y = 0; y < STD_VIDEO_HEIGHT; y++)
    {
      for (int x = 0; x < STD_VIDEO_WIDTH; x++)
      {
        ::fwrite(&((uint8_t *)&screen2[ y * STD_VIDEO_WIDTH + x ])[ 2 ], 1, 1, f);
        ::fwrite(&((uint8_t *)&screen2[ y * STD_VIDEO_WIDTH + x ])[ 1 ], 1, 1, f);
        ::fwrite(&((uint8_t *)&screen2[ y * STD_VIDEO_WIDTH + x ])[ 0 ], 1, 1, f);
      }
    }

    ::fclose(f);
    
    ::sprintf(fName, "/tmp/osd_%05db.ppm", osdCnt++);
    
    f = ::fopen(fName, "wb");

    ::fprintf(f, "P6\n%d %d\n255\n", ws, hs);

    for (int y = 0; y < hs; y++)
    {
      for (int x = 0; x < ws; x++)
      {
        ::fwrite(&((uint8_t *)&scaled2[ y * ws + x ])[ 2 ], 1, 1, f);
        ::fwrite(&((uint8_t *)&scaled2[ y * ws + x ])[ 1 ], 1, 1, f);
        ::fwrite(&((uint8_t *)&scaled2[ y * ws + x ])[ 0 ], 1, 1, f);
      }
    }

    ::fclose(f);
*/    
    delete pXinePalette;
    
    delete [] screen2;    
    delete [] scaled2;
      
    return scaled;
  }

  static int deduceEps(const int dst, const int src)
  {
    int eps = (int)(dst * (2 - ((dst > src) ? 1 : -1) * ::log(dst / src) / ::log(1.5)) / 2);

    if (eps < (dst / 2))
      eps = dst / 2;

    return -eps;
  }
  
  static uint8_t *ScaleBitmapHQ(const uint8_t *src, int x0, int y0, int w, int h, int ws, int hs, int x1, int y1, int w1, int h1, const uint16_t transparentIndex, const tColor *const colors, const int numColors, tColor *&palette2, int &paletteSize, int currentPaletteSize, tColor *currentPalette, cXineLib *xineLib)
  {
    timeval t0, t1, t2, t3, t4;
    (void)t0, (void)t1, (void)t2, (void)t3, (void)t4;

    PROFILE_SCALING(::gettimeofday(&t0, 0);)
    
//    cXinePalette xinePalette(numColors, colors, currentPaletteSize, currentPalette); // fails in getVdrColors(): stack???
    cXinePalette *const pXinePalette = new cXinePalette(xineLib, numColors, colors, currentPaletteSize, currentPalette, (transparentIndex < numColors) ? -1 : transparentIndex);
    cXinePalette &xinePalette = *pXinePalette;
    
    uint16_t *const screen = new uint16_t[ STD_VIDEO_HEIGHT * STD_VIDEO_WIDTH ];
    {
      int x1 = x0 + w;
      int y1 = y0 + h;
      int x2 = STD_VIDEO_WIDTH;
      int y2 = STD_VIDEO_HEIGHT;

      if (x1 > x2)
        x1 = x2;

      if (y1 > y2)
        y1 = y2;
      
      uint16_t *dst = screen;
      
      for (int y = 0; y < y0; y++)
      {
        for (int x = 0; x < x2; x++) 
          *dst++ = transparentIndex;
      }
      
      for (int y = y0; y < y1; y++)
      {
        for (int x = 0; x < x0; x++) 
          *dst++ = transparentIndex;
        
        for (int x = x0; x < x1; x++) 
          *dst++ = xinePalette.m_vdrMapping[ *src++ ];
        
        for (int x = x1; x < x2; x++) 
          *dst++ = transparentIndex;
      }
      
      for (int y = y1; y < y2; y++)
      {
        for (int x = 0; x < x2; x++) 
          *dst++ = transparentIndex;
      }
    }

    uint8_t *const scaled = new uint8_t[ hs * ws ];

    PROFILE_SCALING(::gettimeofday(&t1, 0);)

    cXinePalette::cEntry **const scaled2 = new cXinePalette::cEntry*[ hs * ws ];
    {
      int x2 = x1 + w1;
      int y2 = y1 + h1;

      if (x2 > ws)
      {
        x2 = ws;

        w1 = x2 - x1;
        if (w1 < 0)
          w1 = 0;
      }

      if (y2 > hs)
      {
        y2 = hs;

        h1 = y2 - y1;
        if (h1 < 0)
          h1 = 0;
      }

      cXinePalette::cEntry **scaled2dst0 = scaled2 + y1 * ws + x1;

      cBresenham<STD_VIDEO_WIDTH, uint16_t *> yyBh(STD_VIDEO_HEIGHT, hs, 0, screen);
      uint16_t *screen0 = yyBh.step(y1); //y1 * STD_VIDEO_HEIGHT / hs;

      const int eps0 = deduceEps(ws, STD_VIDEO_WIDTH); 
      const int eps1 = 0; 
      
      cBresenham<> xxBh0(STD_VIDEO_WIDTH, ws, 0);
      xxBh0.step(x1); //x1 * STD_VIDEO_WIDTH / ws;
        
      for (int y = y1; y < y2; y++)
      {
        cXinePalette::cEntry **scaled2dst = scaled2dst0;

        cBresenham<> xxBh(xxBh0);
        uint16_t *screen2src0 = screen0 + xxBh.step(0); //&screen2[ (yi00 / FIX_POINT_FACTOR) * STD_VIDEO_WIDTH + (xi00 / FIX_POINT_FACTOR) ];
      
        for (int x = x1; x < x2; x++)
        {
//          ::fprintf(stderr, "eps: %d\n", xxBh.eps());

          if (eps0 <= xxBh.eps() && xxBh.eps() <= eps1)
            *scaled2dst++ = xinePalette.add(screen2src0[ 0 ], screen2src0[ 1 ]);
          else
            *scaled2dst++ = xinePalette.add(*screen2src0);
      
          screen2src0 += xxBh.stepRelative(); //x1 * STD_VIDEO_WIDTH / ws;
        }

        scaled2dst0 += ws;
        
        screen0 = yyBh.step(); //y1 * STD_VIDEO_HEIGHT / hs;
      }

      PROFILE_SCALING(::gettimeofday(&t2, 0);)
      
      palette2 = xinePalette.getVdrColors(paletteSize);

      PROFILE_SCALING(::gettimeofday(&t3, 0);)

      {
        cXinePalette::cEntry **src = scaled2;
        uint8_t *dst = scaled;
        
        for (int y = 0; y < hs; y++)
        {
          for (int x = 0; x < ws; x++)
          {
            uint8_t idx = (*src) ? (*src)->getIndex() : 0;

//            fprintf(stderr, "%02x ", idx);
            
            *dst++ = idx;
            src++;
          }

//          fprintf(stderr, "\n");
        }
      }
      
      PROFILE_SCALING(
      {
        ::gettimeofday(&t4, 0);

        ::fprintf(stderr, "num: %d, new: %d, ", numColors, xinePalette.getNum());
      
        ::fprintf(stderr, "t1: %.5lf, t2: %.5lf, t3: %.5lf, t4: %.5lf, to: %.5lf, sc: %.5lf %%\n", tt(t1, t0), tt(t2, t1), tt(t3, t2), tt(t4, t3), tt(t4, t0), tt(t2, t1) / tt(t4, t0) * 100); 
      })
    }

/*    
    char fName[ 50 ];
    static int osdCnt = 0;
*/    
/*
    {
      ::sprintf(fName, "/tmp/osd_%05da.ppm", osdCnt);
      
      FILE *f = ::fopen(fName, "wb");
      
      ::fprintf(f, "P6\n%d %d\n255\n", STD_VIDEO_WIDTH, STD_VIDEO_HEIGHT);
      
      for (int y = 0; y < STD_VIDEO_HEIGHT; y++)
      {
        for (int x = 0; x < STD_VIDEO_WIDTH; x++)
        {
          ::fwrite(&((uint8_t *)&screen2[ y * STD_VIDEO_WIDTH + x ])[ 2 ], 1, 1, f);
          ::fwrite(&((uint8_t *)&screen2[ y * STD_VIDEO_WIDTH + x ])[ 1 ], 1, 1, f);
          ::fwrite(&((uint8_t *)&screen2[ y * STD_VIDEO_WIDTH + x ])[ 0 ], 1, 1, f);
        }
      }
      
      ::fclose(f);
    }
    
    {
      ::sprintf(fName, "/tmp/osd_%05db.ppm", osdCnt++);
      
      FILE *f = ::fopen(fName, "wb");
      
      ::fprintf(f, "P6\n%d %d\n255\n", ws, hs);
      
      for (int y = 0; y < hs; y++)
      {
        for (int x = 0; x < ws; x++)
        {
          ::fwrite(&((uint8_t *)&scaled2[ y * ws + x ])[ 2 ], 1, 1, f);
          ::fwrite(&((uint8_t *)&scaled2[ y * ws + x ])[ 1 ], 1, 1, f);
          ::fwrite(&((uint8_t *)&scaled2[ y * ws + x ])[ 0 ], 1, 1, f);
        }
      }
      
      ::fclose(f);
    }
*/    
/*     
    {
      ::sprintf(fName, "/tmp/osd_%05db.ppm", osdCnt++);
      
      FILE *f = ::fopen(fName, "wb");
      
      ::fprintf(f, "P6\n%d %d\n255\n", ws, hs);
      
      for (int y = 0; y < hs; y++)
      {
        for (int x = 0; x < ws; x++)
        {
          uint8_t *c = (uint8_t *)&palette2[ scaled[ y * ws + x ] ];
          
          ::fwrite(&c[ 2 ], 1, 1, f);
          ::fwrite(&c[ 1 ], 1, 1, f);
          ::fwrite(&c[ 0 ], 1, 1, f);
        }
      }
      
      ::fclose(f);
    }
*/  
    delete pXinePalette;
    
    delete [] screen;    
    delete [] scaled2;
      
    return scaled;
  }

  static uint8_t *ScaleBitmapLQ(const uint8_t *src, int x0, int y0, int w, int h, int ws, int hs, int x1, int y1, int w1, int h1, const uint8_t transparentIndex)
  {
    uint8_t *const screen = new uint8_t[ STD_VIDEO_HEIGHT * STD_VIDEO_WIDTH ];
    {
      int x1 = x0 + w;
      int y1 = y0 + h;
      int x2 = STD_VIDEO_WIDTH;
      int y2 = STD_VIDEO_HEIGHT;

      if (x1 > x2)
        x1 = x2;

      if (y1 > y2)
        y1 = y2;

      uint8_t *dst = screen;

      for (int y = 0; y < y0; y++)
      {
        for (int x = 0; x < x2; x++)
          *dst++ = transparentIndex;
      }

      for (int y = y0; y < y1; y++)
      {
        for (int x = 0; x < x0; x++)
          *dst++ = transparentIndex;

        for (int x = x0; x < x1; x++)
          *dst++ = *src++;

        for (int x = x1; x < x2; x++)
          *dst++ = transparentIndex;
      }

      for (int y = y1; y < y2; y++)
      {
        for (int x = 0; x < x2; x++)
          *dst++ = transparentIndex;
      }
    }

    uint8_t *const scaled = new uint8_t[ hs * ws ];
    {
      int x2 = x1 + w1;
      int y2 = y1 + h1;

      if (x2 > ws)
      {
        x2 = ws;

        w1 = x2 - x1;
        if (w1 < 0)
          w1 = 0;
      }

      if (y2 > hs)
      {
        y2 = hs;

        h1 = y2 - y1;
        if (h1 < 0)
          h1 = 0;
      }

      cBresenham<STD_VIDEO_WIDTH, uint8_t *> yyBh(2 * STD_VIDEO_HEIGHT, 2 * hs, hs, screen);
      uint8_t *screen0 = yyBh.step(y1); //(((2 * y1 + 1) * STD_VIDEO_HEIGHT / hs) / 2);

      cBresenham<> xxBh0(2 * STD_VIDEO_WIDTH, 2 * ws, ws);
      xxBh0.step(x1); //(((2 * x1 + 1) * STD_VIDEO_WIDTH / ws) / 2);

      uint8_t *scaled0 = scaled + y1 * ws;
      
      for (int y = y1; y < y2; y++)
      {
        cBresenham<> xxBh(xxBh0);
        int xxx = xxBh.step(0); //(((2 * x1 + 1) * STD_VIDEO_WIDTH / ws) / 2);

        uint8_t *screen00 = screen0 + xxx;

        uint8_t *scaled00 = scaled0 + x1;
        
        for (int x = x1; x < x2; x++)
        {
          *scaled00++ = *screen00;

          screen00 += xxBh.stepRelative();
        }

        scaled0 += ws;

        screen0 = yyBh.step();
      }
    }

    delete [] screen;

    return scaled;
  }

  bool cXineLib::execFuncOsdDrawBitmap(const eNeedsScaling needsScaling, const int videoWidth, const int videoHeight, cXineOsd *const xineOsd, int window, cBitmap *const bitmap, int x, int y, int width, int height, int stride)
  {
    if (!needsScaling)
      return execFuncOsdDrawBitmap(window, (uint8_t *)bitmap->Data(x, y), x, y, width, height, stride);

    int numColors = 0;
    const tColor *colors = bitmap->Colors(numColors);

    if (!numColors)
      return true;


    static int crtGamma = 0;
    if (crtGamma != m_settings.getCrtGamma())
    {
      crtGamma = m_settings.getCrtGamma();
      init_linearize(crtGamma);
      init_delinearize(crtGamma);
    }
    
    int transparentIndex = numColors;
    
    for (int i = 0; i < numColors; i++)
    {
      if (clrTransparent == colors[ i ])
      {
        transparentIndex = i;
        break;
      }
    }

//    ::fprintf(stderr, "ti: %d\t", transparentIndex);

    if (m_settings.OsdMode() == cXineSettings::osdBlendScaledLQ)
    {
      if (transparentIndex == numColors
          && transparentIndex < 256)
      {
        execFuncSetColor(window, transparentIndex, clrTransparent);
      }
    }
    
    int xo = xineOsd->Left() + bitmap->X0();
    int yo = xineOsd->Top()  + bitmap->Y0();
    int wo = bitmap->Width();
    int ho = bitmap->Height();
    
    int xs =   xo       * videoWidth                          / STD_VIDEO_WIDTH;
    int ws = ((xo + wo) * videoWidth  - 1 + STD_VIDEO_WIDTH)  / STD_VIDEO_WIDTH  - xs;
    int ys =   yo       * videoHeight                         / STD_VIDEO_HEIGHT;
    int hs = ((yo + ho) * videoHeight - 1 + STD_VIDEO_HEIGHT) / STD_VIDEO_HEIGHT - ys;

    if (m_settings.OsdMode() > cXineSettings::osdBlendScaledLQ)
    {
      if (numColors > 22
          || shq == needsScaling)
      {
        x = 0;
        y = 0;
        width = wo;
        height = ho;
      }
    }
    
    int x0 =   (xo + x)           * videoWidth                          / STD_VIDEO_WIDTH;
    int w0 = (((xo + x) + width)  * videoWidth  - 1 + STD_VIDEO_WIDTH)  / STD_VIDEO_WIDTH  - x0;
    int y0 =   (yo + y)           * videoHeight                         / STD_VIDEO_HEIGHT;
    int h0 = (((yo + y) + height) * videoHeight - 1 + STD_VIDEO_HEIGHT) / STD_VIDEO_HEIGHT - y0;

    x0 -= xs;
    y0 -= ys;
    (void)ws;
    (void)hs;

    (void)ScaleBitmapSHQ;
    (void)ScaleBitmapHQ;
    (void)ScaleBitmapLQ;

    uint8_t *scaledScreen = 0;
    
    if (m_settings.OsdMode() > cXineSettings::osdBlendScaledLQ)
    {
      tColor *palette = 0;
      int paletteSize = 0;
      
      scaledScreen = (shq == needsScaling)
        ? ScaleBitmapSHQ(bitmap->Data(0, 0), xo, yo, wo, ho, videoWidth, videoHeight, x0 + xs, y0 + ys, w0, h0, transparentIndex, colors, numColors, palette, paletteSize, this)
        : ScaleBitmapHQ(bitmap->Data(0, 0), xo, yo, wo, ho, videoWidth, videoHeight, x0 + xs, y0 + ys, w0, h0, transparentIndex, colors, numColors, palette, paletteSize, m_scaledWindowColorsNum[ window ], &m_scaledWindowColors[ window ][ 0 ], this);
      
      if (shq == needsScaling)
      {
        execFuncSetColor(window, 0, palette, paletteSize);
      }
      else
      {
//    ::fprintf(stderr, "cached: ");
        
        if (m_scaledWindowColorsNum[ window ] < paletteSize
            || 0 != ::memcmp(&m_scaledWindowColors[ window ][ 0 ], palette, paletteSize * sizeof (*palette)))
        {
          m_scaledWindowColorsNum[ window ] = paletteSize;
          ::memcpy(&m_scaledWindowColors[ window ][ 0 ], palette, paletteSize * sizeof (*palette));
          
          execFuncSetColor(window, 0, palette, paletteSize);
          
//      ::fprintf(stderr, "no\n");
        }
        else
        {
//      ::fprintf(stderr, "yes\n");
        }
      }   
      
      delete [] palette;
    }
    else
    {
      scaledScreen = ScaleBitmapLQ(bitmap->Data(0, 0), xo, yo, wo, ho, videoWidth, videoHeight, x0 + xs, y0 + ys, w0, h0, transparentIndex);

    }
      
//    ::fprintf(stderr, "N: (%d,%d):(%d,%d)-(%d,%d)\n", xo, yo, x , y , width, height);
//    ::fprintf(stderr, "S: (%d,%d):(%d,%d)-(%d,%d)\n", xs, ys, x0, y0, w0   , h0    );
    
    if (w0 > videoWidth - (x0 + xs))
    {
      w0 = videoWidth - (x0 + xs);
      
      if (w0 < 0)
        w0 = 0;
    }
    
    if (h0 > videoHeight - (y0 + ys))
    {
      h0 = videoHeight - (y0 + ys);
      
      if (h0 < 0)
        h0 = 0;
    }
    
    bool retVal = execFuncOsdDrawBitmap(window, scaledScreen + (y0 + ys) * videoWidth + (x0 + xs), x0, y0, w0, h0, videoWidth);

    delete [] scaledScreen;

    return retVal;
  }

  cXineLib::eNeedsScaling cXineLib::NeedsScaling(const int videoWidth, const int videoHeight)
  {
    if (m_settings.OsdMode() < cXineSettings::osdBlendScaledLQ)
      return cXineLib::no;
    
    if (videoWidth > 0
        && videoHeight > 0
        && (videoWidth != STD_VIDEO_WIDTH
            || videoHeight != STD_VIDEO_HEIGHT))
    {
      if (m_settings.OsdMode() == cXineSettings::osdBlendScaledAuto)
      {
        if ((2 * videoWidth) < STD_VIDEO_WIDTH
            || (2 * videoHeight) < STD_VIDEO_HEIGHT)
        {
          return cXineLib::shq;
        }
      }

      if (m_settings.OsdMode() == cXineSettings::osdBlendScaledSHQ)
        return cXineLib::shq;

      return cXineLib::yes;
    }

    return cXineLib::no;
  }

  void cXineLib::SendWindow(cXineOsd *const xineOsd, const int windowNum, cBitmap *bitmap /* = 0 */, const int videoLeft /* = -1 */, const int videoTop /* = -1 */, const int videoWidth /* = -1 */, const int videoHeight /* = -1 */, const bool dontOptimize /* = false */)
  {
    const cXineSettings::eOsdMode mode = m_settings.OsdMode();
    const bool supportTransparency = m_settings.SupportTransparency();
    const eNeedsScaling needsScaling = NeedsScaling(videoWidth, videoHeight);
    const int crtGamma = m_settings.getCrtGamma();

    assert(0 <= windowNum && windowNum < MAXNUMWINDOWS);
    
    if (!bitmap
        || dontOptimize
        || m_osdWindowVideoLeft[ windowNum ] != videoLeft
        || m_osdWindowVideoTop[ windowNum ] != videoTop
        || m_osdWindowVideoWidth[ windowNum ] != videoWidth
        || m_osdWindowVideoHeight[ windowNum ] != videoHeight
        || m_osdWindowMode[ windowNum ] != mode
        || m_osdWindowSupportTransparency[ windowNum ] != supportTransparency
        || m_osdWindowGamma[ windowNum ] != crtGamma)
    {
      if (m_osdWindowVisible[ windowNum ]
          || dontOptimize)
      {
        m_osdWindowVisible[ windowNum ] = false;
        m_osdWindowVideoLeft[ windowNum ] = videoLeft;
        m_osdWindowVideoTop[ windowNum ] = videoTop;
        m_osdWindowVideoWidth[ windowNum ] = videoWidth;
        m_osdWindowVideoHeight[ windowNum ] = videoHeight;
        m_osdWindowMode[ windowNum ] = mode;
        m_osdWindowSupportTransparency[ windowNum ] = supportTransparency;
        m_osdWindowGamma[ windowNum ] = crtGamma;
        
        execFuncOsdHide(windowNum);
        execFuncOsdFree(windowNum);
      }

      if (!bitmap)
        return;
    }
    
    if (!m_osdWindowVisible[ windowNum ]
	|| dontOptimize)
    {
      m_osdWindowVideoLeft[ windowNum ] = videoLeft;
      m_osdWindowVideoTop[ windowNum ] = videoTop;
      m_osdWindowVideoWidth[ windowNum ] = videoWidth;
      m_osdWindowVideoHeight[ windowNum ] = videoHeight;

      m_scaledWindowColorsNum[ windowNum ] = 0;
/*      
      fprintf(stderr, "Bitmap[ %d ]: (%d,%d)-(%d,%d)\n"
              , windowNum
              , bitmap->X0()
              , bitmap->Y0()
              , bitmap->Width()
              , bitmap->Height());
*/      
      assert(0x0000 <= bitmap->Width()  && bitmap->Width()  <= 0x0fff);
      assert(0x0000 <= bitmap->Height() && bitmap->Height() <= 0x0fff);

      int vx = m_osdWindowVideoLeft[ windowNum ];
      int vy = m_osdWindowVideoTop[ windowNum ];

      if (mode < cXineSettings::osdBlendScaledLQ)
      {
        vx = 0;
        vy = 0;
      }
      else
      {
        if (vx < 0)
          vx = 0;
        
        if (vy < 0)
          vy = 0;
      }
        
//    fprintf(stderr, "n: %d: f: %d: v: (%d, %d) - (%d, %d): (%d, %d)\n", windowNum, dontOptimize, videoLeft, videoTop, videoWidth, videoHeight, vx, vy);
    
      execFuncOsdNew(needsScaling, vx, vy, videoWidth, videoHeight, windowNum, xineOsd->Left() + bitmap->X0(), xineOsd->Top() + bitmap->Y0(), bitmap->Width(), bitmap->Height());
      
    }

    int numColors = 0;
    const tColor *colors = bitmap->Colors(numColors);

    int &numWindowColors = m_osdWindowColorsNum[ windowNum ];
    tColor *windowColors = &m_osdWindowColors[ windowNum ][ 0 ];

    bool colorsModified = false;

    const bool setColor = !needsScaling
      || (m_settings.OsdMode() <= cXineSettings::osdBlendScaledLQ);
    
    for (int i = 0; i < numColors; i++)
    {
      if (dontOptimize
          || !m_osdWindowVisible[ windowNum ]
          || numWindowColors <= i
          || *windowColors != *colors)
      {
        if (setColor)
          execFuncSetColor(windowNum, i, filterAlpha(*colors));
        
        *windowColors = *colors;
        colorsModified = true;
      }

      windowColors++;
      colors++;
    }

    numWindowColors = numColors;
    
    int x1 = 0, y1 = 0, x2 = bitmap->Width() - 1, y2 = bitmap->Height() - 1;

    if (colorsModified
        || bitmapDiffers(windowNum, bitmap, x1, y1, x2, y2))
    {
//      fprintf(stderr, "dirty area: (%d, %d) - (%d, %d), bitmap size: (%d, %d) %d %d\n", x1, y1, x2, y2, bitmap->Width(), bitmap->Height(), colorsModified, !m_osdWindowVisible[ windowNum ]);

      cloneBitmap(windowNum, bitmap, x1, y1, x2, y2);
      execFuncOsdDrawBitmap(needsScaling, videoWidth, videoHeight, xineOsd, windowNum, bitmap, x1, y1, x2 - x1 + 1, y2 - y1 + 1, bitmap->Width());

      execFuncOsdShow(windowNum);
      
      bitmap->Clean();
    }
    else
    {
//      fprintf(stderr, "not dirty\n");
    }
    
    m_osdWindowVisible[ windowNum ] = true;
  }

  void cXineLib::cloneBitmap(const int windowNum, cBitmap *bitmap, int x1, int y1, int x2, int y2)
  {
    int w = bitmap->Width();
    int h = bitmap->Height();
    int s = w * h;

    if (m_osdWindowBufferSize[ windowNum ] < s)
    {
      if (m_osdWindowBuffer[ windowNum ])
        delete [] m_osdWindowBuffer[ windowNum ];

      m_osdWindowBufferSize[ windowNum ] = 0;

      m_osdWindowBuffer[ windowNum ] = new uchar[ s ];
      if (!m_osdWindowBuffer[ windowNum ])
        return;

      m_osdWindowBufferSize[ windowNum ] = s;

      x1 = 0;
      y1 = 0;
      x2 = w - 1;
      y2 = h - 1;
    }

    const tIndex *src = bitmap->Data(0, 0) + w * y1 + x1;
    tIndex *dst = m_osdWindowBuffer[ windowNum ] + w * y1 + x1;

    int skip = w - (x2 - x1 + 1);
    
    for (int y = y1; y <= y2; y++)
    {
      for (int x = x1; x <= x2; x++)
        *dst++ = *src++;

      src += skip;
      dst += skip;
    }
  }
  
  bool cXineLib::bitmapDiffers(const int windowNum, cBitmap *bitmap, int &x1, int &y1, int &x2, int &y2)
  {
    if (!bitmap->Dirty(x1, y1, x2, y2))
      return false;

    int w = bitmap->Width();
    int h = bitmap->Height();
    int s = w * h;
    
    if (m_osdWindowBufferSize[ windowNum ] < s)
      return true;

    if (!m_osdWindowBuffer[ windowNum ])
      return true;
    
    const tIndex *src = bitmap->Data(0, 0) + w * y1 + x1;
    const tIndex *dst = m_osdWindowBuffer[ windowNum ] + w * y1 + x1;

    int skip = w - (x2 - x1 + 1);
    
    for (int y = y1; y <= y2; y++)
    {
      for (int x = x1; x <= x2; x++)
      {
        if (*dst++ != *src++)
          return true;
      }
      
      src += skip;
      dst += skip;
    }

//    fprintf(stderr, "bitmap dirty, but no difference!\n");
    return false;
  }
    
  void cXineLib::SetVideoWindow(tArea vidWin, const bool dontOptimize /* = false */)
  {
    if (0 == vidWin.bpp)
    {
      vidWin.x1 = 0;
      vidWin.y1 = 0;
      vidWin.x2 = -1 + STD_VIDEO_WIDTH;
      vidWin.y2 = -1 + STD_VIDEO_HEIGHT;
    }
    else
    {
      m_vidWin.bpp = vidWin.bpp;
    }

    if (dontOptimize
        || (0 != ::memcmp(&m_vidWin, &vidWin, sizeof (m_vidWin))))
    {
      m_vidWin = vidWin;
      
      execFuncSetVideoWindow(m_vidWin.x1, m_vidWin.y1, m_vidWin.Width(), m_vidWin.Height(), STD_VIDEO_WIDTH, STD_VIDEO_HEIGHT);
    }
  }
  
#endif

  extern int GetInstanceNo(cPlugin *const plugin);
  extern cXineLib *&GetXineLib(cPlugin *const plugin);
  
  cXineLib::cXineLib(cPlugin *const plugin, const cXineSettings &settings, cMutex &osdMutex, cXineRemote *const remote)
    : cThread()
      , m_plugin(plugin)
      , m_settings(settings)
      , m_osdFlushRequired(false)
      , fd_fifo0(-1)
      , fd_result(-1)
      , fd_control(-1)
      , fd_remote(-1)
      , m_osdMutex(osdMutex)
      , m_paused(false)
      , m_frozen(false)
      , m_ignore(false)
      , m_shutdown(false)
      , m_muted(false)
      , m_volume(0)
      , m_audioChannel(0)
      , m_trickSpeedMode(false)
      , m_noSignalStreamSize(0)
      , m_eventSink(0)
  {
    m_fifoDir = FIFO_DIR;

    if (GetInstanceNo(plugin) >= 0)
    {
      char s[ 20 ];
      ::sprintf(s, "%d", GetInstanceNo(plugin));
      
      m_fifoDir += s;
    }

    m_fifoNameControl    = m_fifoDir + "/stream.control";
    m_fifoNameResult     = m_fifoDir + "/stream.result";
    m_fifoNameRemote     = m_fifoDir + "/stream.event";
    m_fifoNameStream     = m_fifoDir + "/stream";
    m_fifoNameExtControl = m_fifoDir + FIFO_NAME_EXT_CONTROL;
    m_fifoNameExtResult  = m_fifoDir + FIFO_NAME_EXT_RESULT;

    m_external.setXineLib(this);
      
    ::memset(m_osdWindowVisible, 0, sizeof (m_osdWindowVisible));

#if VDRVERSNUM >= 10307
    ::memset(m_osdWindowBufferSize, 0, sizeof (m_osdWindowBufferSize));
    ::memset(m_osdWindowBuffer, 0, sizeof (m_osdWindowBuffer));
    ::memset(m_osdWindowColorsNum, 0, sizeof (m_osdWindowColorsNum));
    ::memset(m_scaledWindowColorsNum, 0, sizeof (m_scaledWindowColorsNum));

    ::memset(&m_vidWin, 0, sizeof (m_vidWin));
#endif    

    string noSignalFileName = plugin->ConfigDirectory(PLUGIN_NAME_I18N);
    noSignalFileName += "/noSignal.pes";
        
    FILE *const f = ::fopen(noSignalFileName.c_str(), "rb");
    if (f)
    {
      m_noSignalStreamSize = ::fread(m_noSignalStreamData, 1, sizeof (m_noSignalStreamData), f);
      ::fclose(f);
    }
    else
    {
      ::fprintf(stderr, ("vdr-xine: error: couldn't open '" + noSignalFileName + "'!\n").c_str());
      esyslog(("vdr-xine: error: couldn't open '" + noSignalFileName + "'!\n").c_str());
    }

    assert(remote);
    remote->setXineLib(this);
    GetXineLib(m_plugin) = this;
  }

  cXineLib::~cXineLib()
  {
    Close();

    GetXineLib(m_plugin) = 0;

#if VDRVERSNUM >= 10307
    for (int i = 0; i < MAXNUMWINDOWS; i++)
    {
      if (m_osdWindowBuffer[ i ])
        delete [] m_osdWindowBuffer[ i ];
    }
#endif
  }

  void cXineLib::SetEventSink(cXineLibEvents *const eventSink)
  {
    m_eventSink = eventSink;
  }

  bool cXineLib::Open()
  {
    ::unlink(m_fifoNameExtControl.c_str());
    ::unlink(m_fifoNameExtResult.c_str());
    ::unlink(m_fifoNameControl.c_str());
    ::unlink(m_fifoNameResult.c_str());
    ::unlink(m_fifoNameRemote.c_str());
    ::unlink(m_fifoNameStream.c_str());
    ::rmdir(m_fifoDir.c_str());

    const mode_t origUmask = ::umask(0);
    
    if (::mkdir(m_fifoDir.c_str(), 0755) < 0)
    {
      perror(("vdr-xine: error: couldn't create directory '" + m_fifoDir + "'").c_str());
      esyslog(("vdr-xine: error: couldn't create directory '" + m_fifoDir + "'").c_str());

      ::umask(origUmask);
      return false;
    }
    
    if (::mknod(m_fifoNameExtControl.c_str(), 0666 | S_IFIFO, 0) < 0)
    {
      perror(("vdr-xine: error: couldn't create fifo '" + m_fifoNameExtControl + "'").c_str());
      esyslog(("vdr-xine: error: couldn't create fifo '" + m_fifoNameExtControl + "'").c_str());

      ::umask(origUmask);
      return false;
    }

    if (::mknod(m_fifoNameExtResult.c_str(), 0644 | S_IFIFO, 0) < 0)
    {
      perror(("vdr-xine: error: couldn't create fifo '" + m_fifoNameExtResult + "'").c_str());
      esyslog(("vdr-xine: error: couldn't create fifo '" + m_fifoNameExtResult + "'").c_str());

      ::umask(origUmask);
      return false;
    }

    if (::mknod(m_fifoNameControl.c_str(), 0644 | S_IFIFO, 0) < 0)
    {
      perror(("vdr-xine: error: couldn't create fifo '" + m_fifoNameControl + "'").c_str());
      esyslog(("vdr-xine: error: couldn't create fifo '" + m_fifoNameControl + "'").c_str());

      ::umask(origUmask);
      return false;
    }

    if (::mknod(m_fifoNameResult.c_str(), 0666 | S_IFIFO, 0) < 0)
    {
      perror(("vdr-xine: error: couldn't create fifo '" + m_fifoNameResult + "'").c_str());
      esyslog(("vdr-xine: error: couldn't create fifo '" + m_fifoNameResult + "'").c_str());

      ::umask(origUmask);
      return false;
    }

    if (::mknod(m_fifoNameRemote.c_str(), 0666 | S_IFIFO, 0) < 0)
    {
      perror(("vdr-xine: error: couldn't create fifo '" + m_fifoNameRemote + "'").c_str());
      esyslog(("vdr-xine: error: couldn't create fifo '" + m_fifoNameRemote + "'").c_str());

      ::umask(origUmask);
      return false;
    }

    if (::mknod(m_fifoNameStream.c_str(), 0644 | S_IFIFO, 0) < 0)
    {
      perror(("vdr-xine: error: couldn't create fifo '" + m_fifoNameStream + "'").c_str());
      esyslog(("vdr-xine: error: couldn't create fifo '" + m_fifoNameStream + "'").c_str());

      ::umask(origUmask);
      return false;
    }
    
    ::umask(origUmask);
    
    if (!Start())
      return false;

    m_external.Start();
    
    return true;
  }

  void cXineLib::Close()
  {
    m_external.Stop();
    
    {
      cMutexLock shutdownMutexLock(&m_shutdownMutex);
      m_shutdown = true;
      m_shutdownCondVar.Broadcast();
    }

    {
      DLOG("cXineLib::Close");
      cMutexLock ioLock(&m_ioMutex);
      disconnect();
    }
    
    ::unlink(m_fifoNameExtControl.c_str());
    ::unlink(m_fifoNameExtResult.c_str());
    ::unlink(m_fifoNameControl.c_str());
    ::unlink(m_fifoNameResult.c_str());
    ::unlink(m_fifoNameRemote.c_str());
    ::unlink(m_fifoNameStream.c_str());
    ::rmdir(m_fifoDir.c_str());
  }

  void cXineLib::internalPaused(const bool paused)
  {
    cMutexLock pausedMutexLock(&m_pausedMutex);

    m_paused = paused;

    if (!paused)
      m_pausedCondVar.Broadcast();
  }
  
  bool cXineLib::Poll(cPoller &Poller, int TimeoutMs /* = 0 */, const bool special /* = false */)
  {
    if (m_paused)
    {
      if (TimeoutMs > 0)
      {
        cMutexLock pausedMutexLock(&m_pausedMutex);
        
        if (m_paused)
          m_pausedCondVar.TimedWait(m_pausedMutex, TimeoutMs);
      }
      
//      fprintf(stderr, "p");
      return false;
    }
   
    if (-1 == fd_fifo0)
      return true;

    Poller.Add(fd_fifo0, true);
    
    if (Poller.Poll(TimeoutMs))
    {
//      fprintf(stderr, "_");
      return true;
    }
    
    xfprintf(stderr, (special ? "p" : "P"));
    return false;
  }

  void cXineLib::flush()
  {
    ::fsync(fd_fifo0);
  }

  int cXineLib::xread(int f, void *b, int n)
  {
    int t = 0;

    while (t < n)
    {
      void (* const sigPipeHandler)(int) = ::signal(SIGPIPE, SIG_IGN);

      errno = 0;
      int r = ::read(f, ((char *)b) + t, n - t);
      int myErrno = errno;
      
      ::signal(SIGPIPE, sigPipeHandler);
      
      if (r <= 0)
      {
        if (EAGAIN == myErrno || EINTR == myErrno)
          continue;

        xfprintf(stderr, "read(%d) returned %d, error %d: ", n, r, myErrno);
        errno = myErrno;
        if (!m_settings.ShallBeQuiet())
          perror("");

        disconnect();
                
        return r;
      }
        
      t += r;
    }

    return t;
  }

  int cXineLib::xwrite(int f, const void *b, int n)
  {
    char *yyy[] = { "i", "I", "d", "D" };
//    char *yyy[] = { "", "", "d", "D" };
    char **xxx = yyy;
    if (f == fd_fifo0)
      xxx += 2;

    int t = 0;

    while (t < n)
    {
//      fprintf(stderr, "%s", xxx[ 0 ]);
      
//      fprintf(stderr, " xwrite ");

      void (* const sigPipeHandler)(int) = ::signal(SIGPIPE, SIG_IGN);

      errno = 0;
      int r = ::write(f, ((char *)b) + t, n - t);
      int myErrno = errno;
      
      ::signal(SIGPIPE, sigPipeHandler);
      
      if (r <= 0)
      {
        if (EAGAIN == myErrno || EINTR == myErrno)
          break;

        xfprintf(stderr, "::write(%d) returned %d, error %d: ", n, r, myErrno);
        errno = myErrno;
        if (!m_settings.ShallBeQuiet())
          perror("");

        disconnect();
                
//        fprintf(stderr, "%s", xxx[ 1 ]);
        return r;
      }
/*
      if (t != 0 || r != n)
        fprintf(stderr, "::write(%d): %d\n", n - t, r);
*/        
      t += r;
    }

//    fprintf(stderr, "%s", xxx[ 1 ]);
    return t;
  }

  bool cXineLib::showNoSignal()
  {
//    fprintf(stderr, "showNoSignal: enter\n");
    
    bool x = execFuncStream0((uchar *)m_noSignalStreamData, m_noSignalStreamSize);
//    fprintf(stderr, "showNoSignal: leave\n");
    
    return x;
  }

  bool cXineLib::isConnected()
  {
    return (-1 != fd_fifo0);
  }

  static bool m_connectionEstablished = false;
  
  void cXineLib::disconnect()
  {
    cMutexLock disconnectLock(&m_disconnectMutex);

    const bool wasConnected = m_connectionEstablished;
    m_connectionEstablished = false;

    if (-1 != fd_control)
    {
//      ::fprintf(stderr, "close: fd_control\n");      
      ::close(fd_control);
      fd_control = -1;
    }

    if (-1 != fd_result)
    {
//      ::fprintf(stderr, "close: fd_result\n");      
      ::close(fd_result);
      fd_result = -1;
    }

    if (-1 != fd_remote)
    {
//      ::fprintf(stderr, "close: fd_remote\n");
      ::close(fd_remote);
      fd_remote = -1;
    }

    if (-1 != fd_fifo0)
    {
//      ::fprintf(stderr, "close: fd_fifo0\n");
      ::close(fd_fifo0);
      fd_fifo0 = -1;
    }
    
    m_external.disconnect();
   
    if (wasConnected)
    { 
      if (m_eventSink)
        m_eventSink->OnClientDisconnect();

      xfprintf(stderr, "vdr-xine: Client disconnected!\n");
    }
  }

  bool cXineLib::execFuncNop()
  {
    if (!isConnected())
      return false;
      
    DLOG("cXineLib::execFuncNop");
    cMutexLock ioLock(&m_ioMutex);
    if (!isConnected())
      return false;
          
    data_nop_t data;
    data.header.func = func_nop;
    data.header.len = sizeof (data);
    
    if (sizeof (data) == xwrite(fd_control, &data, sizeof (data)))
      return true;
    
    return false;
  }
  
  void cXineLib::Action(void)
  {
    while (!m_shutdown)
    {
      if (!isConnected())
      {
//        fprintf(stderr, "a");

        DLOG("cXineLib::Action");
        cMutexLock osdLock(&m_osdMutex); 
        cMutexLock ioLock(&m_ioMutex);
        cMutexLock dataLock(&m_dataMutex);
        
        checkConnect();
      }
      else
      {
//        fprintf(stderr, "A");

        execFuncNop();

//        fprintf(stderr, "n");
      }

      if (!m_shutdown)
      {
        cMutexLock shutdownMutexLock(&m_shutdownMutex);
        if (!m_shutdown)
          m_shutdownCondVar.TimedWait(m_shutdownMutex, 100);
      }
    }

//    fprintf(stderr, "Action done\n");
  }
  
  bool cXineLib::checkConnect()
  {
//    bool reinit = false;
      
//    if (-1 == fd_fifo0)
//    {
      fd_fifo0 = ::open(m_fifoNameStream.c_str(), O_WRONLY | O_NONBLOCK);
      if (-1 == fd_fifo0)
        return false;

      xfprintf(stderr, "vdr-xine: Client connecting ...\n");

      char zero = 0;
      xwrite(fd_fifo0, &zero, sizeof (zero));

      fd_remote = ::open(m_fifoNameRemote.c_str(), O_RDONLY | O_NONBLOCK);
      fd_control = ::open(m_fifoNameControl.c_str(), O_WRONLY);
      fd_result  = ::open(m_fifoNameResult.c_str() , O_RDONLY);

      ::fcntl(fd_fifo0 , F_SETFL, ~O_NONBLOCK & ::fcntl(fd_fifo0 , F_GETFL, 0));
      ::fcntl(fd_remote, F_SETFL, ~O_NONBLOCK & ::fcntl(fd_remote, F_GETFL, 0));
      
      internalPaused(false);
      
      m_frozen = false;
      m_ignore = false;
//      reinit = true;
//      doBufferAndStart();
//    }

//    if (reinit)
//    {
      
      execFuncSetup();
//      execFuncMute(m_muted);
      execFuncSetVolume(m_volume);
      execFuncSelectAudio(m_audioChannel);
      execFuncTrickSpeedMode(m_trickSpeedMode);
      execFuncClear(-1);
      execFuncSetPrebuffer(0);
      execFuncStart();
      execFuncWait();

      for (int i = 0; i < 2; i++)
        showNoSignal();

      execFuncFlush();
      execFuncResetAudio();
      execFuncWait();
      execFuncSetPrebuffer(0); //m_settings.GetModeParams()->m_prebufferFrames);
      execFuncStart();
      execFuncWait();
      
//      m_setPrebufferRequired = true;
/*      
    if (m_setPrebufferRequired)
    {
      fprintf(stderr, "Prebuffer required\n");
      
      m_setPrebufferRequired = false;
      
      if (!isConnected())
        return false;
    }
    
*/
      if (m_eventSink
        && isConnected())
      {
        m_eventSink->OnClientConnect();
      }

      m_connectionEstablished = true;
      
//      execFuncWait();

//      execFuncSetSpeed(-1);
//      execFuncWait();
      
      {
//        if (!execFuncSetSpeed(-1))
//          return false;

#if 0
        if (execFuncOsdNew(0xf, 320, 16, 16, 16))
        {
          if (execFuncSetColor(0xf, 0, clrBlack))
          {
            if (execFuncSetColor(0xf, 1, clrCyan))
            {
              if (execFuncSetColor(0xf, 2, clrRed))
              {
                uint8_t data[ 16 ][ 16 ] =
                  {
                    { 2, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 2 },
                    { 1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1 },
                    { 1, 1, 0, 0,  0, 0, 1, 1,  1, 1, 0, 0,  0, 0, 1, 1 },
                    { 1, 1, 0, 0,  0, 0, 1, 1,  1, 1, 0, 0,  0, 0, 1, 1 },
                    
                    { 1, 1, 0, 0,  0, 0, 1, 1,  1, 1, 0, 0,  0, 0, 1, 1 },
                    { 1, 1, 0, 0,  0, 0, 1, 1,  1, 1, 0, 0,  0, 0, 1, 1 },
                    { 1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1 },
                    { 1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1 },
                    
                    { 1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1 },
                    { 1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1 },
                    { 1, 1, 0, 0,  0, 0, 1, 1,  1, 1, 0, 0,  0, 0, 1, 1 },
                    { 1, 1, 0, 0,  0, 0, 1, 1,  1, 1, 0, 0,  0, 0, 1, 1 },
                    
                    { 1, 1, 0, 0,  0, 0, 1, 1,  1, 1, 0, 0,  0, 0, 1, 1 },
                    { 1, 1, 0, 0,  0, 0, 1, 1,  1, 1, 0, 0,  0, 0, 1, 1 },
                    { 1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1 },
                    { 2, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 2 },
                  };
                
                if (execFuncOsdDrawBitmap(0xf, &data[ 0 ][ 0 ], 0, 0, 16, 16, 16))
                {
                  if (execFuncOsdShow(0xf))
                  {
                    return true;
                  }
                }
              }
            }
          }
        }
#endif        
      }
//      execFuncSetPrebuffer(31);
//      execFuncStart();
//      execFuncWait();

      if (isConnected())
      {
        xfprintf(stderr, "vdr-xine: Client connected!\n");
        return true;
      }
      
//    }
//    else
//    {
//      dataNop_t dataNop;
//      dataNop.header.func = funcNop;
//      dataNop.header.len = sizeof (dataNop);
//    
//      if (sizeof (dataNop) == xwrite(fd_control, &dataNop, sizeof (dataNop)))
//        return true;
//    }

//    ::close(fd_control);
//    ::close(fd_fifo0);
//    fd_fifo0 = -1;

      xfprintf(stderr, "vdr-xine: Client connect failed!\n");
      
    return false;
  }
/*
  void cXineLib::doBufferAndStart()
  {
    m_doBufferAndStart = 64000;
  }
*/

  bool cXineLib::execFuncSetPrebuffer(int frames)
  {
    if (!isConnected())
      return false;
      
    DLOG("cXineLib::execFuncSetPrebuffer");
    cMutexLock ioLock(&m_ioMutex);
    
    if (!isConnected())
      return false;
      
//    if (!checkConnect())
//      return false;
    
    data_set_prebuffer_t data;
    data.header.func = func_set_prebuffer;
    data.header.len = sizeof (data);
    data.prebuffer = frames * 90000 / 25;
    
    if (sizeof (data) == xwrite(fd_control, &data, sizeof (data)))
      return true;
    
//    ::close(fd_control);
//    ::close(fd_fifo0);
//    fd_fifo0 = -1;
    
    return false;
  }
        
  int cXineLib::execFuncStream1(const uchar *Data, int Length)
  {
    if (!m_connectionEstablished)
      return Length;

    return execFuncStream(Data, Length);
  }
  
  int cXineLib::execFuncStream(const uchar *Data, int Length)
  {
    if (!isConnected())
      return Length;
    
//    return true;
    
//    {
//      cMutexLock ioLock(&m_ioMutex);
//      
//      if (!checkConnect())
//        return false;
//    }

    if (m_paused)
      return 0;
    
    if (m_frozen)
      return -1;

    if (m_ignore)
      return Length;

    cMutexLock dataLock(&m_dataMutex);

    if (!isConnected())
      return Length;      

    int r = xwrite(fd_fifo0, Data, Length);
    if (Length == r)
//      fprintf(stderr, ".");

    if (r < 0)
      xfprintf(stderr, "X");
    
    return r;

#if 0    
    cMutexLock ioLock(&m_ioMutex);
    
    if (!checkConnect())
      return false;

    if (m_frozen)
      return false;

    if (m_ignore)
      return true;

    dataStream_t dataStream;
    dataStream.header.func = funcStream;
    dataStream.header.len = sizeof (dataStream) + Length;
    
    assert((sizeof (dataStream) + Length) == dataStream.header.len);
    
    if (sizeof (dataStream) == xwrite(fd_fifo, &dataStream, sizeof (dataStream)))
    {
      if (Length == xwrite(fd_fifo, Data, Length))
      {
/*        if (m_doBufferAndStart > 0)
        {
          m_doBufferAndStart -= Length;

          if (m_doBufferAndStart <= 0)
          {
            if (!execFuncSetSpeed(0)
                || !execFuncMute(false))
            {
              return false;
            }              
          }
        }
*/    
        return true;
      }
    }

    ::close(fd_control);
    ::close(fd_fifo0);
    fd_fifo0 = -1;
    
    return false;
#endif    
  }

  bool cXineLib::execFuncStream0(const uchar *Data, int Length)
  {
    if (!isConnected())
      return true;
      
//    return true;
    
//    {
//      cMutexLock ioLock(&m_ioMutex);
//      
//      if (!checkConnect())
//        return false;
//    }

    cMutexLock dataLock(&m_dataMutex);

    int done = 0;

    while (done < Length)
    {    
      if (!isConnected())
        return true;

      int r = xwrite(fd_fifo0, Data + done, Length - done);
      if (r < 0)
        return false;

      done += r;
    }

    return Length == done;
  }

  bool cXineLib::execFuncStart()
  {
    if (!isConnected())
      return false;
      
    DLOG("cXineLib::execFuncStart");
    cMutexLock ioLock(&m_ioMutex);
    
//    if (!checkConnect())
//      return false;

    if (!isConnected())
      return false;
          
    data_start_t data;
    data.header.func = func_start;
    data.header.len = sizeof (data);
    
    if (sizeof (data) == xwrite(fd_control, &data, sizeof (data)))
      return true;
    
//    ::close(fd_control);
//    ::close(fd_fifo0);
//    fd_fifo0 = -1;
    
    return false;
  }

  bool cXineLib::execFuncOsdNew(int window, int x, int y, int width, int height)
  {
    if (!isConnected())
      return false;
      
    DLOG("cXineLib::execFuncOsdNew");
    cMutexLock ioLock(&m_ioMutex);
    
//    if (!checkConnect())
//      return false;
    
    if (!isConnected())
      return false;
      
    data_osd_new_t data;
    data.header.func = func_osd_new;
    data.header.len = sizeof (data);
    data.window = window;
    data.x = x;
    data.y = y;
    data.width = width;
    data.height = height;
    
    if (sizeof (data) == xwrite(fd_control, &data, sizeof (data)))
      return true;
    
//    ::close(fd_control);
//    ::close(fd_fifo0);
//    fd_fifo0 = -1;
    
    return false;
  }


  
#if VDRVERSNUM < 10307
  
  bool cXineLib::execFuncSetColor(int window, int index, eDvbColor color)
  {
    return execFuncSetColor(window, index, &color, 1);
  }
  
  bool cXineLib::execFuncSetColor(int window, int index, eDvbColor *const colors, int numColors)
  {
    for (int i = 0; i < numColors; i++)
    {
      uint8_t *const c = (uint8_t *)&colors[ i ];

      uint8_t h = c[ 0 ];
      c[ 0 ] = c[ 2 ];
      c[ 2 ] = h;
    }

    return execFuncSetColor(window, index, numColors, (uint32_t *)colors);
  }

  eDvbColor cXineLib::filterAlpha(eDvbColor color)
  {
    if (!m_settings.SupportTransparency())
    {
      uint8_t *const c = (uint8_t *)&color;
      if (c[ 3 ])
        c[ 3 ] = 0xff;
    }
    return color;
  }
  
#else

  bool cXineLib::execFuncSetColor(int window, int index, tColor color)
  {
    return execFuncSetColor(window, index, &color, 1);
  }
  
  bool cXineLib::execFuncSetColor(int window, int index, tColor *const colors, int numColors)
  {
    return execFuncSetColor(window, index, numColors, (uint32_t *)colors);
  }

  tColor cXineLib::filterAlpha(tColor color)
  {
//    fprintf(stderr, "color: 0x%08x", color);
    
    if (!m_settings.SupportTransparency())
    {
      uint8_t *const c = (uint8_t *)&color;
      if (c[ 3 ])
        c[ 3 ] = 0xff;
    }

//    fprintf(stderr, ", 0x%08x\n", color);
    
    return color;
  }

#endif


  bool cXineLib::execFuncSetColor(int window, int index, int numColors, uint32_t *const colors)
  {
    assert(0 <= index && index < 256);
    assert(0 < numColors && numColors <= 256);
    assert((index + numColors) <= 256);
    
    m_osdFlushRequired = true;
    
    if (!isConnected())
      return false;
      
    DLOG("cXineLib::execFuncSetColor");
    cMutexLock ioLock(&m_ioMutex);
    
    if (!isConnected())
      return false;

    const int sizeofColors = sizeof (*colors) * numColors;
    
    data_set_color_t data;
    data.header.func = func_set_color;
    data.header.len = sizeof (data) + sizeofColors;
    data.window = window;
    data.index = index;
    data.num = numColors - 1;
    
    if (sizeof (data) != xwrite(fd_control, &data, sizeof (data)))
      return false;

    if (sizeofColors != xwrite(fd_control, colors, sizeofColors))
      return false;
    
    return true;
  }
  
  bool cXineLib::execFuncOsdDrawBitmap(int window, uint8_t *pData, int x, int y, int width, int height, int stride)
  {
    m_osdFlushRequired = true;
    
    if (!isConnected())
      return false;
      
    DLOG("cXineLib::execFuncOsdDrawBitmap");
    cMutexLock ioLock(&m_ioMutex);
    
    if (!isConnected())
      return false;
      
//    if (!checkConnect())
//      return false;

    int bitmapLen = width * height;
    
    data_osd_draw_bitmap_t data;
    data.header.func = func_osd_draw_bitmap;
    data.header.len = sizeof (data) + bitmapLen;
    data.window = window;
    data.x = x;
    data.y = y;
    data.width = width;
    data.height = height;
    
    if (sizeof (data) == xwrite(fd_control, &data, sizeof (data)))
    {
      bool failed = false;
      uint8_t *row = pData;
      
      for (int y = 0; !failed && y < height; y++)
      {
        if (width != xwrite(fd_control, row, width))
        {
          failed = true;
          break;
        }

        row += stride;
      }

      if (!failed)
        return true;      
    }
    
//    ::close(fd_control);
//    ::close(fd_fifo0);
//    fd_fifo0 = -1;
    
    return false;
  }

  bool cXineLib::execFuncOsdSetPosition(int window, int x, int y)
  {
    m_osdFlushRequired = true;
    
    if (!isConnected())
      return false;
      
    DLOG("cXineLib::execFuncOsdSetPosition");
    cMutexLock ioLock(&m_ioMutex);
    
    if (!isConnected())
      return false;
      
//    if (!checkConnect())
//      return false;
    
    data_osd_set_position_t data;
    data.header.func = func_osd_set_position;
    data.header.len = sizeof (data);
    data.window = window;
    data.x = x;
    data.y = y;
    
    if (sizeof (data) == xwrite(fd_control, &data, sizeof (data)))
      return true;
    
//    ::close(fd_control);
//    ::close(fd_fifo0);
//    fd_fifo0 = -1;
    
    return false;
  }

  bool cXineLib::execFuncOsdShow(int window)
  {
    m_osdFlushRequired = true;
    
    if (!isConnected())
      return false;
      
    DLOG("cXineLib::execFuncOsdShow");
    cMutexLock ioLock(&m_ioMutex);
    
    if (!isConnected())
      return false;
      
//    if (!checkConnect())
//      return false;
    
    data_osd_show_t data;
    data.header.func = func_osd_show;
    data.header.len = sizeof (data);
    data.window = window;
    
    if (sizeof (data) == xwrite(fd_control, &data, sizeof (data)))
      return true;
    
//    ::close(fd_control);
//    ::close(fd_fifo0);
//    fd_fifo0 = -1;
    
    return false;
  }

  bool cXineLib::execFuncOsdHide(int window)
  {
    m_osdFlushRequired = true;
    
    if (!isConnected())
      return false;
      
    DLOG("cXineLib::execFuncOsdHide");
    cMutexLock ioLock(&m_ioMutex);
    
    if (!isConnected())
      return false;
      
//    if (!checkConnect())
//      return false;
    
    data_osd_hide_t data;
    data.header.func = func_osd_hide;
    data.header.len = sizeof (data);
    data.window = window;
    
    if (sizeof (data) == xwrite(fd_control, &data, sizeof (data)))
      return true;
    
//    ::close(fd_control);
//    ::close(fd_fifo0);
//    fd_fifo0 = -1;
    
    return false;
  }
  
  bool cXineLib::execFuncOsdFlush()
  {
    if (!m_osdFlushRequired)
      return true;

    m_osdFlushRequired = false;
      
    if (!isConnected())
      return false;
      
    DLOG("cXineLib::execFuncOsdFlush");
    cMutexLock ioLock(&m_ioMutex);
    
    if (!isConnected())
      return false;
      
    data_osd_flush_t data;
    data.header.func = func_osd_flush;
    data.header.len = sizeof (data);

    if (sizeof (data) == xwrite(fd_control, &data, sizeof (data)))
      return true;
    
    return false;
  }
  
  bool cXineLib::execFuncOsdFree(int window)
  {
    m_osdFlushRequired = true;
    
    if (!isConnected())
      return false;
      
    DLOG("cXineLib::execFuncOsdFree");
    cMutexLock ioLock(&m_ioMutex);
    
    if (!isConnected())
      return false;
      
//    if (!checkConnect())
//      return false;
    
    data_osd_free_t data;
    data.header.func = func_osd_free;
    data.header.len = sizeof (data);
    data.window = window;
    
    if (sizeof (data) == xwrite(fd_control, &data, sizeof (data)))
      return true;
    
//    ::close(fd_control);
//    ::close(fd_fifo0);
//    fd_fifo0 = -1;
    
    return false;
  }
  
  static uchar dataPadding[ 6 + 0xffff ] = { 0x00, 0x00, 0x01, 0xbe, 0xff, 0xff, 0x00 };

  bool cXineLib::execFuncClear(int n)
  {
    if (!isConnected())
      return false;
      
    DLOG("cXineLib::execFuncClear");

    cMutexLock dataLock(&m_dataMutex);
    cMutexLock ioLock(&m_ioMutex);
    
    if (!isConnected())
      return false;

    if (sizeof (dataPadding) != xwrite(fd_fifo0, &dataPadding, sizeof (dataPadding)))
      return false;

    if (!isConnected())
      return false;

//    if (!checkConnect())
//      return false;
    
    data_clear_t data;
    data.header.func = func_clear;
    data.header.len = sizeof (data);
    data.n = n;
    
    if (sizeof (data) == xwrite(fd_control, &data, sizeof (data)))
      return true;
    
//    ::close(fd_control);
//    ::close(fd_fifo0);
//    fd_fifo0 = -1;
    
    return false;
  }
  
  bool cXineLib::execFuncFirstFrame()
  {
    if (!isConnected())
      return false;
      
    DLOG("cXineLib::execFuncFirstFrame");
    cMutexLock ioLock(&m_ioMutex);
    
    if (!isConnected())
      return false;
      
    data_first_frame_t data;
    data.header.func = func_first_frame;
    data.header.len = sizeof (data);
    
    if (sizeof (data) == xwrite(fd_control, &data, sizeof (data)))
      return true;
    
//    ::close(fd_control);
//    ::close(fd_fifo0);
//    fd_fifo0 = -1;
    
    return false;
  }

  bool cXineLib::execFuncStillFrame()
  {
    if (!isConnected())
      return false;
      
    DLOG("cXineLib::execFuncStillFrame");
    cMutexLock ioLock(&m_ioMutex);
    
    if (!isConnected())
      return false;
      
    data_still_frame_t data;
    data.header.func = func_still_frame;
    data.header.len = sizeof (data);
    
    if (sizeof (data) == xwrite(fd_control, &data, sizeof (data)))
      return true;
    
//    ::close(fd_control);
//    ::close(fd_fifo0);
//    fd_fifo0 = -1;
    
    return false;
  }

  bool cXineLib::execFuncResetAudio()
  {
    if (!isConnected())
      return false;

    DLOG("cXineLib::execFuncResetAudio");
    cMutexLock ioLock(&m_ioMutex);
    
    if (!isConnected())
      return false;
      
    data_reset_audio_t data;
    data.header.func = func_reset_audio;
    data.header.len = sizeof (data);
    
    if (sizeof (data) == xwrite(fd_control, &data, sizeof (data)))
      return true;
    
    return false;
  }

  bool cXineLib::execFuncFlush(int TimeoutMs /* = -1 */, const bool justWait /* = false */)
  {
    if (!isConnected())
      return true;
      
    DLOG("cXineLib::execFuncFlush");
    cMutexLock ioLock(&m_ioMutex);
    
    if (!isConnected())
      return true;
      
//    if (!checkConnect())
//      return false;
    
    data_flush_t data;
    data.header.func = func_flush;
    data.header.len = sizeof (data);
    data.ms_timeout = TimeoutMs;
    data.just_wait = justWait;
    
    if (sizeof (data) == xwrite(fd_control, &data, sizeof (data)))
    {
      result_union_t resultUnion;
   
      off_t n = xread(fd_result, (char *)&resultUnion, sizeof (resultUnion.header));
      if (n != sizeof (resultUnion.header))
        return true;

      if (data.header.func != resultUnion.header.func)
        return true;
      
      result_flush_t *result = &resultUnion.flush;
        
      n = xread(fd_result, (char *)result + sizeof (result->header), sizeof (*result) - sizeof (result->header));
      if (n != sizeof (*result) - sizeof (result->header))
        return true;

      return !result->timed_out;
    }
    
    return true;
  }
  
  bool cXineLib::execFuncMute(bool mute /* = true */)
  {
    m_muted = mute;

    return true;
    
    if (!isConnected())
      return false;
      
    DLOG("cXineLib::execFuncMute");
    cMutexLock ioLock(&m_ioMutex);
    
    if (!isConnected())
      return false;
      
//    if (!checkConnect())
//      return false;
    
    data_mute_t data;
    data.header.func = func_mute;
    data.header.len = sizeof (data);
    data.mute = mute;
    
    if (sizeof (data) == xwrite(fd_control, &data, sizeof (data)))
      return true;
    
//    ::close(fd_control);
//    ::close(fd_fifo0);
//    fd_fifo0 = -1;
    
    return false;
  }
  
  bool cXineLib::execFuncSetVolume(int volume)
  {
    m_volume = volume;
    
    if (!isConnected())
      return false;
      
    DLOG("cXineLib::execFuncSetVolume");
    cMutexLock ioLock(&m_ioMutex);
    
    if (!isConnected())
      return false;
      
//    if (!checkConnect())
//      return false;
    
    data_set_volume_t data;
    data.header.func = func_set_volume;
    data.header.len = sizeof (data);
    data.volume = volume * 100 / 0xff;
    
    if (sizeof (data) == xwrite(fd_control, &data, sizeof (data)))
      return true;
    
//    ::close(fd_control);
//    ::close(fd_fifo0);
//    fd_fifo0 = -1;
    
    return false;
  }
  
  bool cXineLib::execFuncSelectAudio(int audioChannel)
  {
    m_audioChannel = audioChannel;
    
    if (!isConnected())
      return false;
      
    DLOG("cXineLib::execFuncSelectAudio");
    cMutexLock ioLock(&m_ioMutex);
    
    if (!isConnected())
      return false;
      
//    if (!checkConnect())
//      return false;
    
    data_select_audio_t data;
    data.header.func = func_select_audio;
    data.header.len = sizeof (data);
    data.channels = audioChannel;

    switch (audioChannel)
    {
    case -1:
      data.channels = 0; // don't touch audio data
      break;

    case 0:
      data.channels = 3; // left + right channel
      break;
    } 
    
    if (sizeof (data) == xwrite(fd_control, &data, sizeof (data)))
      return true;
    
//    ::close(fd_control);
//    ::close(fd_fifo0);
//    fd_fifo0 = -1;
    
    return false;
  }

  bool cXineLib::execFuncSetSpeed(double speed)
  {
    if (!isConnected())
      return false;
      
    DLOG("cXineLib::execFuncSetSpeed");
    cMutexLock ioLock(&m_ioMutex);
    
    if (!isConnected())
      return false;
      
    data_set_speed_t data;
    data.header.func = func_set_speed;
    data.header.len = sizeof (data);
    data.speed = (int)(XINE_FINE_SPEED_NORMAL * speed / 100.0 + 0.5);
    
    if (sizeof (data) == xwrite(fd_control, &data, sizeof (data)))
      return true;
    
    return false;
  }

  bool cXineLib::execFuncTrickSpeedMode(bool on)
  {
    m_trickSpeedMode = on;
    
    if (!isConnected())
      return false;
      
    DLOG("cXineLib::execFuncTrickSpeedMode");
    cMutexLock ioLock(&m_ioMutex);
    
    if (!isConnected())
      return false;
      
    data_trick_speed_mode_t data;
    data.header.func = func_trick_speed_mode;
    data.header.len = sizeof (data);
    data.on = on;
    
    if (sizeof (data) == xwrite(fd_control, &data, sizeof (data)))
      return true;
    
    return false;
  }
/*
  bool cXineLib::execFuncDelay(int usdelay)
  {
    cMutexLock ioLock(&m_ioMutex);
    
    if (!checkConnect())
      return false;
    
    dataDelay_t dataDelay;
    dataDelay.header.func = funcDelay;
    dataDelay.header.len = sizeof (dataDelay);
    dataDelay.usdelay = usdelay;

    if (sizeof (dataDelay) == xwrite(fd_control, &dataDelay, sizeof (dataDelay)))
      return true;
    
    ::close(fd_control);
    ::close(fd_fifo0);
    fd_fifo0 = -1;
    
    return false;
  }
*/
  bool cXineLib::execFuncMetronom(int64_t pts, uint32_t flags /* = 0 */)
  {
    if (!isConnected())
      return false;
      
    DLOG("cXineLib::execFuncMetronom");
    cMutexLock ioLock(&m_ioMutex);
    
    if (!isConnected())
      return false;
      
//    if (!checkConnect())
//      return false;
    
    data_metronom_t data;
    data.header.func = func_metronom;
    data.header.len = sizeof (data);
    data.pts = pts;
    data.flags = flags;
    
    if (sizeof (data) == xwrite(fd_control, &data, sizeof (data)))
      return true;
    
//    ::close(fd_control);
//    ::close(fd_fifo0);
//    fd_fifo0 = -1;
    
    return false;
  }

  bool cXineLib::execFuncWait()
  {
    if (!isConnected())
      return false;
      
    DLOG("cXineLib::execFuncWait");
    cMutexLock ioLock(&m_ioMutex);
    
    if (!isConnected())
      return false;
      
//    if (!checkConnect())
//      return false;
    
    data_wait_t data;
    data.header.func = func_wait;
    data.header.len = sizeof (data);

    if (sizeof (data) == xwrite(fd_control, &data, sizeof (data)))
      return true;
    
//    ::close(fd_control);
//    ::close(fd_fifo0);
//    fd_fifo0 = -1;
    
    return false;
  }

  bool cXineLib::execFuncSetup()
  {
    if (!isConnected())
      return false;
      
    DLOG("cXineLib::execFuncSetup");
    cMutexLock ioLock(&m_ioMutex);
    
    if (!isConnected())
      return false;
      
//    if (!checkConnect())
//      return false;
    
    data_setup_t data;
    data.header.func = func_setup;
    data.header.len = sizeof (data);

    data.osd_unscaled_blending = (m_settings.OsdMode() == cXineSettings::osdOverlay);
    data.dont_change_xine_volume = !m_settings.ChangeVolume();

    switch (m_settings.MuteMode())
    {
    case cXineSettings::muteIgnore:    data.mute_mode = INPUT_VDR_MUTE_IGNORE;    break;
    case cXineSettings::muteExecute:   data.mute_mode = INPUT_VDR_MUTE_EXECUTE;    break;
    case cXineSettings::muteSimulate:  data.mute_mode = INPUT_VDR_MUTE_SIMULATE;  break;
    default:
      assert(false);
    }
    
    if (sizeof (data) == xwrite(fd_control, &data, sizeof (data)))
      return true;
    
//    ::close(fd_control);
//    ::close(fd_fifo0);
//    fd_fifo0 = -1;
    
    return false;
  }

  void cXineLib::freeze(bool doFreeze /* = true */)
  {
//    m_frozen = doFreeze;
  }

  void cXineLib::ignore(bool doIgnore /* = true */)
  {
//    m_ignore = doIgnore;
  }

  void cXineLib::pause(bool doPause /* = true */)
  {
    cMutexLock dataLock(&m_dataMutex);
    
    internalPaused(doPause);
  }

#ifndef Y4MSCALER
#define Y4MSCALER "y4mscaler"
#endif

#ifndef Y4MTOPPM
#define Y4MTOPPM  "y4mtoppm"
#endif

#ifndef PNMCUT
#define PNMCUT    "pnmcut"
#endif
 
#ifndef PNMTOJPEG
#define PNMTOJPEG "pnmtojpeg"
#endif
 
  bool cXineLib::execFuncGrabImage(const char *FileName, bool Jpeg, int Quality, int SizeX, int SizeY)
  {
    if (!isConnected())
      return false;
      
    DLOG("cXineLib::execFuncGrabImage");
    cMutexLock ioLock(&m_ioMutex);
    
    if (!isConnected())
      return false;
    
    data_grab_image_t data;
    data.header.func = func_grab_image;
    data.header.len = sizeof (data);

    if (sizeof (data) == xwrite(fd_control, &data, sizeof (data)))
    {
      result_union_t resultUnion;
   
      off_t n = xread(fd_result, (char *)&resultUnion, sizeof (resultUnion.header));
      if (n != sizeof (resultUnion.header))
        return false;

      if (data.header.func != resultUnion.header.func)
        return false;
      
      result_grab_image_t *result = &resultUnion.grab_image;
        
      n = xread(fd_result, (char *)result + sizeof (result->header), sizeof (*result) - sizeof (result->header));
      if (n != sizeof (*result) - sizeof (result->header))
        return false;

      const size_t frameSize = result->header.len - sizeof (*result);

//      ::fprintf(stderr, "frameSize: %d\n", frameSize);
      
      if (frameSize <= 0)
        return false;
      
      uint8_t *img = (uint8_t *)::malloc(frameSize);
      if (!img)
        return false;
      
      if (frameSize != (size_t)xread(fd_result, img, frameSize))
      {
        ::free(img);
        return false;
      }

      if (XINE_IMGFMT_YUY2 == result->format)
      {
        uint8_t *img2 = (uint8_t *)::malloc(frameSize);
        if (!img2)
        {
          ::free(img);
          return false;
        }

        ::memset(img2, 0x80, frameSize);
        
        uint8_t *src = img;
        uint8_t *dstY = img2;
        uint8_t *dstU = dstY + result->height * result->width;
        uint8_t *dstV = dstU + result->height * ((result->width + 1) / 2);
        
        for (int y = 0; y < result->height; y++)
        {
          for (int x = 0; x < (result->width + 1) / 2; x++)
          {
            *dstY++ = *src++;
            *dstU++ = *src++;
            *dstY++ = *src++;
            *dstV++ = *src++;
          }
        }

        ::free(img);

        img = img2;
      }

      int videoX = -1;
      int videoY = -1;
      int videoW = -1;
      int videoH = -1;
      execFuncVideoSize(videoX, videoY, videoW, videoH);
      
      bool success = false;
        
      int outfd = ::open(FileName, O_CREAT /* | O_EXCL */ | O_TRUNC | O_RDWR, 0644);
      if (-1 != outfd)
      {
        int sizeX = SizeX;
        int sizeY = SizeY;
        
        if (-1 == videoX || -1 == videoY || videoW <= 0 || videoH <= 0)
        {
          videoX = 0;
          videoY = 0;
          videoW = SizeX;
          videoH = SizeY;          
        }
        else
        {
          sizeX = SizeX * result->width / videoW;
          sizeY = SizeY * result->height / videoH;

          videoX = SizeX * videoX / videoW;
          videoY = SizeY * videoY / videoH;

          videoW = SizeX;
          videoH = SizeY;
        }
        
        const char *const chromass = (XINE_IMGFMT_YV12 == result->format) ? "420_MPEG2" : "422";
        char *cmd = 0;
        
        if (Jpeg)
        {              
          ::asprintf(&cmd, Y4MSCALER " -I chromass=%s -O chromass=420_JPEG -O size=%dx%d -O sar=1:1 | " Y4MTOPPM " | " PNMCUT " %d %d %d %d | " PNMTOJPEG " -quality=%d >&%d", chromass, sizeX, sizeY, videoX, videoY, videoW, videoH, Quality, outfd);
        }
        else
        {              
          ::asprintf(&cmd, Y4MSCALER " -I chromass=%s -O chromass=420_JPEG -O size=%dx%d -O sar=1:1 | " Y4MTOPPM " | " PNMCUT " %d %d %d %d >&%d", chromass, sizeX, sizeY, videoX, videoY, videoW, videoH, outfd);
        }                
        
//      ::fprintf(stderr, "ratio: %d\n", result->ratio);
        xfprintf(stderr, "cmd: %s\n", cmd);
        
        int ratioX = result->width;
        int ratioY = result->height;
        
        double r = result->ratio / 13333.0 * (ratioY * SizeX) / (ratioX * SizeY);
        
        if (r <= 0)
          r = 1;
        
        if (result->ratio >= 1)
          ratioX = (int)(ratioY * r + .5);
        else
          ratioY = (int)(ratioX / r + .5);
        
//      ::fprintf(stderr, "ratio: %d, %lg, %d:%d\n", result->ratio, r, ratioX, ratioY);
        
        FILE *f = ::popen(cmd, "w");
        if (f)
        {
          ::fprintf(f, "YUV4MPEG2 W%d H%d F%d:%d Ip A%d:%d\nFRAME\n"
                    , result->width, result->height
                    , 25, 1
                    , ratioX, ratioY); 
          
          success = (frameSize == ::fwrite(img, 1, frameSize, f));
          
          ::pclose(f);
        }

        ::free(cmd);
        ::close(outfd);
      }
      
      ::free(img);

      return success;
    }

    return false;
  }
  
  bool cXineLib::execFuncGetPTS(int64_t &pts)
  {
    if (!isConnected())
      return false;
      
    DLOG("cXineLib::execFuncGetPTS");
    cMutexLock ioLock(&m_ioMutex);
    
    if (!isConnected())
      return false;
          
    data_get_pts_t data;
    data.header.func = func_get_pts;
    data.header.len = sizeof (data);
    
    if (sizeof (data) == xwrite(fd_control, &data, sizeof (data)))
    {
      result_union_t resultUnion;
   
      off_t n = xread(fd_result, (char *)&resultUnion, sizeof (resultUnion.header));
      if (n != sizeof (resultUnion.header))
        return false;

      if (data.header.func != resultUnion.header.func)
        return false;
      
      result_get_pts_t *result = &resultUnion.get_pts;
        
      n = xread(fd_result, (char *)result + sizeof (result->header), sizeof (*result) - sizeof (result->header));
      if (n != sizeof (*result) - sizeof (result->header))
        return false;

      pts = result->pts;

      return true;
    }
    
    return false;
  }
  
  bool cXineLib::execFuncVideoSize(int &videoLeft, int &videoTop, int &videoWidth, int &videoHeight)
  {
    if (!isConnected())
      return false;
      
    DLOG("cXineLib::execFuncVideoSize");
    cMutexLock ioLock(&m_ioMutex);
    
    if (!isConnected())
      return false;
          
    data_video_size_t data;
    data.header.func = func_video_size;
    data.header.len = sizeof (data);
    
    if (sizeof (data) == xwrite(fd_control, &data, sizeof (data)))
    {
      result_union_t resultUnion;
   
      off_t n = xread(fd_result, (char *)&resultUnion, sizeof (resultUnion.header));
      if (n != sizeof (resultUnion.header))
        return false;

      if (data.header.func != resultUnion.header.func)
        return false;
      
      result_video_size_t *result = &resultUnion.video_size;
        
      n = xread(fd_result, (char *)result + sizeof (result->header), sizeof (*result) - sizeof (result->header));
      if (n != sizeof (*result) - sizeof (result->header))
        return false;

      videoLeft   = result->left;
      videoTop    = result->top;
      videoWidth  = result->width;
      videoHeight = result->height;

      return true;
    }
    
    return false;
  }
  
  bool cXineLib::execFuncSetVideoWindow(int x, int y, int w, int h, int wRef, int hRef)
  {
    if (!isConnected())
      return false;
      
    DLOG("cXineLib::execFuncSetVideoWindow");
    cMutexLock ioLock(&m_ioMutex);
    
    if (!isConnected())
      return false;
      
    data_set_video_window_t data;
    data.header.func = func_set_video_window;
    data.header.len = sizeof (data);
    data.x = x;
    data.y = y;
    data.w = w;
    data.h = h;
    data.w_ref = wRef;
    data.h_ref = hRef;
    
    if (sizeof (data) == xwrite(fd_control, &data, sizeof (data)))
      return true;
    
    return false;
  }
  
  void cXineLib::ReshowCurrentOSD(const int frameWidth, const int frameHeight)
  {
    if (m_eventSink)
      m_eventSink->ReshowCurrentOSD(frameWidth, frameHeight);
  }

  bool cXineLib::execFuncPlayExternal(const char *const fileName /* = 0 */)
  {
    if (!isConnected())
      return false;
      
    DLOG("cXineLib::execFuncPlayExternal");
    cMutexLock ioLock(&m_ioMutex);
    
    if (!isConnected())
      return false;

    int dataSize = (fileName ? (1 + ::strlen(fileName)) : 0);
    
    data_play_external_t data;
    data.header.func = func_play_external;
    data.header.len = sizeof (data) + dataSize;
    
    if (sizeof (data) != xwrite(fd_control, &data, sizeof (data)))
      return false;
    
    if (dataSize != xwrite(fd_control, fileName, dataSize))
      return false;
    
    return true;
  }

  void cXineLib::enableExternal(const bool enable /* = true */)
  {
    m_external.enable(enable);
  }

  void cXineLib::ExternalStreamFinished()
  {
    m_external.externalStreamFinished();
  }

};
