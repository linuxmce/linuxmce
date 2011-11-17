/**
 * VideoFrameGeometry - This is used to hold video frame geometries of the target
 * Emulation windows, so that Simulate Mouse Click and Get Video Frame will work
 * correctly.
 * 
 * Author: Thom Cherryhomes <thom.cherryhomes@gmail.com>
 */

#include "VideoFrameGeometry.h"

namespace DCE
{

  /* VideoFrameGeometry Stuff */
  VideoFrameGeometry::VideoFrameGeometry (int iWidth, int iHeight)
  {
    m_iWidth = iWidth;
    m_iHeight = iHeight;
  }
  
  VideoFrameGeometry::~VideoFrameGeometry ()
  {
    // Does nothing.
  }
  
}
