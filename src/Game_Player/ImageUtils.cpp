/**
 * ImageUtils - Image Utilities
 * Author: Thomas Cherryhomes
 * <thom.cherryhomes@gmail.com>
*/

#include "ImageUtils.h"
#include <wand/MagickWand.h>
#include "DCE/Logger.h"
#include "PlutoUtils/CommonIncludes.h"

using namespace DCE;

bool ImageUtils::ResizeImage(string sFilename, int iWidth, int iHeight)
{
  MagickWand* m_wand = NULL;
  MagickBooleanType status;

  MagickWandGenesis();
  m_wand = NewMagickWand();
  status=MagickReadImage(m_wand,sFilename.c_str());
  if (status == MagickFalse)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ImageUtils::ResizeImage() - could not open image %s",sFilename.c_str());
      return false;
    }
  else
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"XXXX Image read!");
    }

  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"XXXX Image being resized.");
  MagickResizeImage(m_wand,iWidth,iHeight,LanczosFilter,1);
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"XXXX Image resized.");
  MagickWriteImage(m_wand,sFilename.c_str());
  
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ImageUtils::ResizeImage() - Image %s resized to %d x %d",sFilename.c_str(),iWidth,iHeight);

  if (m_wand)
    m_wand = DestroyMagickWand(m_wand);

  MagickWandTerminus();

  return true;
}
