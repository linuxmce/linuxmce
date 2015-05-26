/*
 * equalizer.c: audio equalizer OSD control
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: equalizer.c,v 1.8 2012-03-18 19:17:07 phintuka Exp $
 *
 */

#include <vdr/config.h>

#include "config.h"
#include "device.h"
#include "equalizer.h"

cEqualizer::cEqualizer(cXinelibDevice *Dev) : cOsdObject()
{
  m_Dev = Dev;
  m_Values = new int[AUDIO_EQ_count];
  memcpy(m_Values, xc.audio_equalizer, sizeof(xc.audio_equalizer));
  m_Osd = NULL;
  m_Current = 0;
}

cEqualizer::~cEqualizer()
{
  delete m_Osd;
  delete[] m_Values;
}

#define OSD_W (220)
#define OSD_H (220)
#define OSD_X (720-50-OSD_W)
#define OSD_Y (576-50-OSD_H)
/* dvbdevice requires bpp*width to be n*8 */

#define ADJUST_MIN  (-100)
#define ADJUST_MAX  (100)
#define ADJUST_STEP (5)

void cEqualizer::Show()
{
  tArea areas [] = { {0, 0, OSD_W - 1, OSD_H - 1, 4} };

  m_Osd = cOsdProvider::NewOsd(OSD_X, OSD_Y, 0);

  if(m_Osd) {
    if (m_Osd->CanHandleAreas(areas, sizeof(areas) / sizeof(tArea) ) == oeOk) {
      m_Osd->SetAreas(areas, sizeof(areas) / sizeof(tArea));
      m_Osd->Flush();
      DrawBackground();
      DrawBar(0,true);
      for(int i=1; i<AUDIO_EQ_count; i++) 
	DrawBar(i);
    }
  }
}

eOSState cEqualizer::ProcessKey(eKeys key)
{
  eOSState state = cOsdObject::ProcessKey(key);
  if (state == osUnknown) {
    switch (key & ~k_Repeat) {
      case kDown:  
	m_Values[m_Current] -= ADJUST_STEP;
	if(m_Values[m_Current] < ADJUST_MIN)
	  m_Values[m_Current] = ADJUST_MIN;
	DrawBar(m_Current,true);
	m_Dev->ConfigurePostprocessing(xc.deinterlace_method, xc.audio_delay, xc.audio_compression, m_Values, xc.audio_surround, xc.speaker_type);
	break;
      case kUp:
	m_Values[m_Current] += ADJUST_STEP;
	if(m_Values[m_Current] > ADJUST_MAX)
	  m_Values[m_Current] = ADJUST_MAX;
	DrawBar(m_Current,true);
	m_Dev->ConfigurePostprocessing(xc.deinterlace_method, xc.audio_delay, xc.audio_compression, m_Values, xc.audio_surround, xc.speaker_type);
	break;
      case kLeft: 
	if(m_Current>0) {
	  DrawBar(m_Current);	  
	  m_Current--;
	  DrawBar(m_Current, true);
	}
	break;
      case kRight:
	if(m_Current+1 < AUDIO_EQ_count) {
	  DrawBar(m_Current);
	  m_Current++;
	  DrawBar(m_Current, true);
	}
	break;
      case kBack:
        m_Dev->ConfigurePostprocessing(xc.deinterlace_method, xc.audio_delay, xc.audio_compression, xc.audio_equalizer, xc.audio_surround, xc.speaker_type);
	return osEnd;
      case kOk:
        memcpy(xc.audio_equalizer, m_Values, sizeof(xc.audio_equalizer));
	return osEnd;
      default:; // all other keys - do nothing.
    }
  }

  return state;
}

#define COL_BORDER     0xffb0b0b0
#define COL_BG         0x7f7f7f7f
#define COL_BAR        0xff000000
#define COL_BAR_SEL    0xffff0000
#define COL_BAR_ON     0xff00FF00
#define COL_BAR_OFF    0xff000000
#define COL_BAR_BORDER 0xff7f7f7f

void cEqualizer::DrawBackground()
{
  // border
  m_Osd->DrawRectangle(0, 0, OSD_W - 1, OSD_H - 1, COL_BORDER);
  m_Osd->DrawRectangle(1, 1, OSD_W - 2, OSD_H - 2, COL_BORDER);
  // background
  m_Osd->DrawRectangle(2, 2, OSD_W - 3, OSD_H - 3, COL_BG);
  // line
  m_Osd->DrawRectangle(5, 10+100-1, OSD_W-6, 10+100, COL_BAR);
  // commit
  m_Osd->Flush();
}

void cEqualizer::DrawBar(int Index, bool Selected)
{
  // bar
  if(Selected)
    m_Osd->DrawRectangle(10+20*Index, 10, 10+20*Index+7, OSD_H - 10, COL_BAR_SEL);
  else
    m_Osd->DrawRectangle(10+20*Index, 10, 10+20*Index+7, OSD_H - 10, COL_BAR);
  // off
  m_Osd->DrawRectangle(12+20*Index, 10, 10+20*Index+5, OSD_H - 10, COL_BAR_OFF);
  // on
  if(m_Values[Index]>0) 
    m_Osd->DrawRectangle(12+20*Index, 10+100-m_Values[Index], 10+20*Index+5, 10+100, COL_BAR_ON);
  else
    m_Osd->DrawRectangle(12+20*Index, 10+100, 10+20*Index+5, 10+100-m_Values[Index], COL_BAR_ON);
  // line
  m_Osd->DrawRectangle(12+20*Index, 10+100-1, 10+20*Index+5, 10+100, COL_BAR_ON);

  m_Osd->Flush();
}


