/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#ifndef __FILE_UTILS_H__
#define __FILE_UTILS_H__

#include <string>
#include <map>
#include <list>

namespace UpdateMediaFileUtils
{
	bool IsSymlink(std::string sFilePath);
	std::string SymlinkSource(std::string sFilePath);
	int PlutoDeviceForFile(std::string sFilePath, const std::map<int, int>& mapMountedDevices);
	void MapMountedDevicesToPlutoDevices(const std::list<std::string>& listFiles, std::map<int, int>& mapMountedDevices);
	bool IsDirectory(const char *pFilePath);
	int GetDeviceID(const char *pFilePath);
	time_t ModificationDateForFile(const char *pFilePath);
}

#endif //__FILE_UTILS_H__
