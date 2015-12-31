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
#ifndef __CACHE_IMAGE_MANAGER_H__
#define __CACHE_IMAGE_MANAGER_H__

#include <string>
#include <vector>
using namespace std;
//------------------------------------------------------------------------
struct CacheImageData
{
	CacheImageData(string sFileName, int iImageSize, int iPriority)
	{
		m_sFileName = sFileName;
		m_iImageSize = iImageSize;
		m_iPriority = iPriority;
	}

	string m_sFileName;
	int m_iImageSize;
	int m_iPriority;
};
//------------------------------------------------------------------------
class CacheImageManager
{
	string m_sTimeStamp;
	string m_sCacheFolder;
	int m_iCacheSize;

	vector<CacheImageData *> m_vectCacheInfo;

public:
	CacheImageManager(string sCacheFolder, int iCacheSize);
	~CacheImageManager();

	bool VerifyCache(string sTimeStamp);
	void CacheImage(char *pData, int iSize, string sFileName, int iPriority);
	bool IsImageInCache(string sFileName, int iPriority = -1);
	string GetCacheImageFileName(string sFileName);

private:
	bool ClearCache();
	int GetCacheActualSize();
	void AdjustCacheSize();
};
//------------------------------------------------------------------------
#endif
