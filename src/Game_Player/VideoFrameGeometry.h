/** 
 * VideoFrameGeomtery - Holds window geometry information for Simulate Mouse Click and 
 * Get Video Frame commands.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 */

#ifndef VIDEOFRAMEGEOMETRY_H
#define VIDEOFRAMEGEOMETRY_H

namespace DCE
{
   class VideoFrameGeometry
  {

  public:
    int m_iWidth;
    int m_iHeight;

      VideoFrameGeometry (int iWidth, int iHeight);
      virtual ~ VideoFrameGeometry ();
  };
}

#endif /* VideoFrameGeometry */
