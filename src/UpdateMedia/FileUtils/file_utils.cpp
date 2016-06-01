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
#include "file_utils.h"

#include <sys/stat.h>
#include <limits.h>
#include <sys/types.h>
#include <iostream>
#include <string.h>
#include <cstdlib>
#include <errno.h>
#include <unistd.h>

using namespace std;

namespace UpdateMediaFileUtils
{
	bool IsSymlink(string sFilePath)
	{
#ifdef WIN32
		return false;
#else
		const int cnDummyBufferSize = 4;
		char *pDummyBuffer = new char[cnDummyBufferSize];
		memset(pDummyBuffer, 0, cnDummyBufferSize);

		ssize_t link_length = readlink (sFilePath.c_str(), pDummyBuffer, cnDummyBufferSize); 	

		delete [] pDummyBuffer;

		return link_length > 0;
#endif
	}

	string SymlinkSource(string sFilePath)
	{
#ifdef WIN32
		return "";
#else
		const int cnBufferSize = 1024;
		char *pBuffer = new char[cnBufferSize];
		memset(pBuffer, 0, cnBufferSize);

		ssize_t link_length = readlink (sFilePath.c_str(), pBuffer, cnBufferSize); 	

		string sResult(link_length > 0 ? pBuffer : "");
		delete [] pBuffer;

		return sResult;
#endif
	}

	int PlutoDeviceForFile(string sFilePath, const map<int, int>& mapMountedDevices)
	{
		map<int, int>::const_iterator it = mapMountedDevices.find(GetDeviceID(sFilePath.c_str()));
		if(it != mapMountedDevices.end())
			return it->second;

		return 0;
	}

	void MapMountedDevicesToPlutoDevices(const list<string>& listFiles, map<int, int>& mapMountedDevices)
	{
		const string csMntDevicePath("/mnt/device");
		for(list<string>::const_iterator it = listFiles.begin(), end = listFiles.end(); it != end; ++it)
		{
			string sFilePath = *it;

			int nMountedDevice = GetDeviceID(sFilePath.c_str());
			if(nMountedDevice != 0 && sFilePath.find(csMntDevicePath) == 0)
			{
                string sDeviceId = sFilePath.substr(csMntDevicePath.length() + 1);
				int nPlutoDevice = atoi(sDeviceId.c_str());
				if(nPlutoDevice != 0)
				{		
					mapMountedDevices[nMountedDevice] = nPlutoDevice;
#ifdef UPDATEMEDIA_DEBUG
					cout << "Mapped file " << sFilePath << " : " << nMountedDevice << " -> " << nPlutoDevice << endl; 
#endif
				}
			}
		}
	}

	bool IsDirectory(const char *pFilePath)
	{
		bool bIsDir = false;

	#ifdef WIN32
		struct __stat64 buf;
		if(!_stat64(pFilePath, &buf))
			bIsDir = (0 != (buf.st_mode & _S_IFDIR)); 
	#else
		struct stat64 buf;
		if(!stat64(pFilePath, &buf))
			bIsDir = S_ISDIR(buf.st_mode);
	#endif

		return bIsDir;
	}

	int GetDeviceID(const char *pFilePath)
	{
		int nDeviceID = 0;

#ifdef WIN32
		struct __stat64 buf;
		if(!_stat64(pFilePath, &buf))
			nDeviceID = buf.st_dev;
#else
		struct stat64 buf;
		if(!stat64(pFilePath, &buf))
			nDeviceID = buf.st_dev;
#endif

		return nDeviceID;
	}

	
	time_t ModificationDateForFile(const char *pFilePath)
	{
#ifdef WIN32
		struct __stat64 buf;
#else
		struct stat64 buf;
#endif

#ifdef WIN32
		if(!_stat64(pFilePath, &buf))
#else
		if(!stat64(pFilePath, &buf))
#endif
			return static_cast<time_t>(buf.st_mtime);

		cout << "ERROR: " << errno << " - desc: " << strerror(errno) << endl;

		return 0;
	}

}

