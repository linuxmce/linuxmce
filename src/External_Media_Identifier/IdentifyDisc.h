/**
 * IdentifyDisc - Routines to determine disc type for identification
 * mostly because we can't directly use the Disk_Drive_Functions..grrr...
 *
 * @Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * @Version: 1.0
 */

#ifndef IDENTIFYDISC_H
#define IDENTIFYDISC_H

#include <string>

using namespace std;

namespace DCE
{
  class IdentifyDisc
  {
  public:

    enum class DiscType {UNKNOWN, CD, DVD, BluRay};

    IdentifyDisc();
    virtual ~IdentifyDisc();
    virtual DiscType Identify(string sDiscDevice);

  private:
    bool IsDiscInDrive(string sDiscDevice);
    bool HasValidTOC(string sDiscDevice);
    DiscType GetDiscType(string sDiscDevice);
    DiscType GetDataTrackType(string sDiscDevice);
    bool IsVideoDVD(string sDiscDevice);
    bool IsBluRayVideo(string sDiscDevice);
    bool DirExists(string sDevice, string sDirectory);

  };
}

#endif /* IDENTIFYDISC_H */
