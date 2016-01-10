/**
 * INotifyEmulatorModel - Creates a simple model that can
 * be used to track state for emulators controlled by the
 * kernel inotify method.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 * Version: 1.0
 */

#include "INotifyEmulatorModel.h"
#include "pluto_main/Define_Button.h"

namespace DCE
{
  INotifyEmulatorModel::INotifyEmulatorModel()
    : EmulatorModel()
  {
    m_sLastReceivedCommand="";
    m_iDoneDelay=50;
    m_mapActionsToCommandBytes["GET_SNAPSHOT"] = make_pair(0x0c,false);
    m_mapActionsToCommandBytes["STOP"] = make_pair(0xff,false);
    m_mapActionsToCommandBytes["SAVE_STATE"] = make_pair(0x20,false);
    m_mapActionsToCommandBytes["LOAD_STATE"] = make_pair(0x21,false);
  }

  INotifyEmulatorModel::~INotifyEmulatorModel()
  {
  
  }

  /**
   * Update a VideoFrameGeometry for a particular PK_Device
   * Used primarily for Get Video Frame, and Simulate Mouse Click
   *
   * Create a VideoFrameGeometry for a PK_Device if it does not exist,
   * update its width and height if it does.
   */
  void INotifyEmulatorModel::updateVideoFrameGeometry(long int iPK_Device, int iWidth, int iHeight)
  {
    if (!m_mapVideoFrameGeometry_Exists(iPK_Device))
      {
	// make a new videoframegeometry for a device.
	m_mapVideoFrameGeometry[iPK_Device] = new VideoFrameGeometry(iWidth, iHeight);
      }
    else
      {
	// update existing videoframegeometry
	map<long int, VideoFrameGeometry *>::iterator it = m_mapVideoFrameGeometry.find(iPK_Device);
	(*it).second->m_iWidth=iWidth;
	(*it).second->m_iHeight=iHeight;
      }
  }

  /**
   * Returns a pair of width and height for a video frame geometry.
   */
  pair<int, int> INotifyEmulatorModel::getVideoFrameGeometry(long int iPK_Device)
  {
    if (!m_mapVideoFrameGeometry_Exists(iPK_Device))
      {
	return make_pair(-1,-1);
      }

    VideoFrameGeometry* vfg = m_mapVideoFrameGeometry_Find(iPK_Device);
    int iWidth = vfg->m_iWidth;
    int iHeight = vfg->m_iHeight;
    return make_pair(iWidth, iHeight);
  }


}
