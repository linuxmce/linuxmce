/**
 * RadarFrame - A simple container for still images from Radar frames.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 * Copyright (c) 2013 - LinuxMCE - Licensed under the GNU Public License Version 3
 * See COPYING for details.
 */

#include <string>

#ifndef RADARFRAME_H
#define RADARFRAME_H

#define PLEASEWAIT_JPG_SIZE 43099

using namespace std;

namespace DCE
{
  /**
   * RadarFrame - An Image Container 
   */
  class RadarFrame
  {
  private:
  public: 
    /** 
     * Image Width in pixels 
     */
    int m_iWidth;
    /** 
     * Image Height in pixels
     */
    int m_iHeight;
    /**
     * Image Format, 1=JPEG, 2=PNG, 3=GIF
     */
    int m_iImageFormat;
    /**
     * Image data buffer
     */
    string m_sImageData;
    
    RadarFrame(int iWidth, int iHeight, int iImageFormat, string sImageData);
    ~RadarFrame();

    string data() { return m_sImageData; };
    size_t size() { return m_sImageData.size(); };

  };
}

#endif /* RadarFrame */
