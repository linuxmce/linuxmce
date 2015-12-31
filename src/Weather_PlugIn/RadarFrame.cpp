/**
 * RadarFrame - A simple container for still images from Radar frames.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 * Copyright (c) 2013 - LinuxMCE - Licensed under the GNU Public License Version 3
 * See COPYING for details.
 */

#include "RadarFrame.h"

namespace DCE
{

  RadarFrame::RadarFrame(int iWidth, int iHeight, int iImageFormat, string sImageData)
  {
    m_iWidth=iWidth;
    m_iHeight=iHeight;
    m_iImageFormat=iImageFormat;
    m_sImageData=sImageData;
  }

  RadarFrame::~RadarFrame()
  {
    // Does nothing.
  }

}
