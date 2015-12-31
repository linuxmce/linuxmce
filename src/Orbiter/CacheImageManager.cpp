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
#include "CacheImageManager.h"

#include "PlutoUtils/FileUtils.h"
#include "DCE/Logger.h"

#include <algorithm>

using namespace DCE;
//----------------------------------------------------------------------------------------
bool CompareCacheImageData(const CacheImageData* c1, const CacheImageData* c2)
{
	return c1->m_iPriority < c2->m_iPriority;
}
//----------------------------------------------------------------------------------------
CacheImageManager::CacheImageManager(string sCacheFolder, int iCacheSize)
{
	m_sCacheFolder = sCacheFolder;
	m_iCacheSize = iCacheSize * 1024 * 1024; //size in bytes

	if(FileUtils::DirExists(m_sCacheFolder) && FileUtils::FileExists(m_sCacheFolder + "/CacheTimeStamp.txt"))
	{
		size_t iSize;
		char *pData = FileUtils::ReadFileIntoBuffer(m_sCacheFolder + "/CacheTimeStamp.txt", iSize);
		m_sTimeStamp = pData;
		delete pData;
		pData = NULL;
	}
	else
		m_sTimeStamp = "";
}
//----------------------------------------------------------------------------------------
CacheImageManager::~CacheImageManager()
{
	for(vector<CacheImageData *>::iterator it = m_vectCacheInfo.begin(); it != m_vectCacheInfo.end(); ++it)
		delete *it;

	m_vectCacheInfo.clear();
}
//----------------------------------------------------------------------------------------
bool CacheImageManager::VerifyCache(string sTimeStamp)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Server skins timestamp: %s. Cache skins timestamp: %s",
		sTimeStamp.c_str(), m_sTimeStamp.c_str());
#endif

	if(sTimeStamp != m_sTimeStamp)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "New skins are available. We'll purge the cache");

		if(!ClearCache())
			return false;
	}

	m_sTimeStamp = sTimeStamp;
	FileUtils::WriteBufferIntoFile(m_sCacheFolder + "/CacheTimeStamp.txt", 
		const_cast<char *>(m_sTimeStamp.c_str()), m_sTimeStamp.length());
	return true;
}
//----------------------------------------------------------------------------------------
void CacheImageManager::CacheImage(char *pData, int iSize, string sFileName, 
								   int iPriority)
{
	if(IsImageInCache(sFileName))
	{
		//LoggerWrapper::GetInstance()->Write(LV_STATUS, "The image '%s' in already in cache.", sFileName.c_str());
		return;
	}

	CacheImageData* pCacheImageData = new CacheImageData(sFileName, iSize, iPriority);
	m_vectCacheInfo.push_back(pCacheImageData);
	sort(m_vectCacheInfo.begin(), m_vectCacheInfo.end(), CompareCacheImageData);

	AdjustCacheSize();

	if(IsImageInCache(sFileName))
	{
		if(!FileUtils::WriteBufferIntoFile(GetCacheImageFileName(sFileName), pData, iSize))
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Unable to add image '%s' to cache", sFileName.c_str());
#ifdef DEBUG
		else
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Added image '%s' to cache", sFileName.c_str());
#endif
	}
}
//----------------------------------------------------------------------------------------
bool CacheImageManager::IsImageInCache(string sFileName, int iPriority/*=0*/)
{
	for(vector<CacheImageData *>::iterator it = m_vectCacheInfo.begin(); it != m_vectCacheInfo.end(); ++it)
	{
		CacheImageData* pCacheImageData = *it;

		if(pCacheImageData->m_sFileName == sFileName)
			return true;
	}

	if(iPriority == -1)
		return false;

	//file exists in cache folder 
	string sCacheImageFileName = GetCacheImageFileName(sFileName);
	if(FileUtils::FileExists(GetCacheImageFileName(sFileName))) 
	{
		int iSize = FileUtils::FileSize(sFileName);
		CacheImageData* pCacheImageData = new CacheImageData(sFileName, iSize, iPriority);
		m_vectCacheInfo.push_back(pCacheImageData);
		sort(m_vectCacheInfo.begin(), m_vectCacheInfo.end(), CompareCacheImageData);

		AdjustCacheSize();

#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Image '%s' found in cache folder. Added to our list.", sFileName.c_str());
#endif
		return IsImageInCache(sFileName);
	}

	return false;
}
//----------------------------------------------------------------------------------------
string CacheImageManager::GetCacheImageFileName(string sFileName)
{
	return m_sCacheFolder + "/" + sFileName;
}
//----------------------------------------------------------------------------------------
bool CacheImageManager::ClearCache()
{
	if(FileUtils::DirExists(m_sCacheFolder))
		FileUtils::DelDir(m_sCacheFolder);

	if(FileUtils::DirExists(m_sCacheFolder))
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Unable to purge %s cache folder!", m_sCacheFolder.c_str());

	FileUtils::MakeDir(m_sCacheFolder);
	if(!FileUtils::DirExists(m_sCacheFolder))
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Unable to create cache folder %s", m_sCacheFolder.c_str());
		return false;
	}

	return true;
}
//----------------------------------------------------------------------------------------
int CacheImageManager::GetCacheActualSize()
{
	int iCacheActualSize = 0;

	for(vector<CacheImageData *>::iterator it = m_vectCacheInfo.begin(); it != m_vectCacheInfo.end(); ++it)
		iCacheActualSize += (*it)->m_iImageSize;

	return iCacheActualSize;
}
//----------------------------------------------------------------------------------------
void CacheImageManager::AdjustCacheSize()
{
	while(GetCacheActualSize() > m_iCacheSize)
	{
		CacheImageData* pCacheImageData = m_vectCacheInfo.front();
        m_vectCacheInfo.erase(m_vectCacheInfo.begin());

		FileUtils::DelFile(GetCacheImageFileName(pCacheImageData->m_sFileName));
		if(NULL != pCacheImageData)
		{
			delete pCacheImageData;
			pCacheImageData = NULL;
		}
	}
}
//----------------------------------------------------------------------------------------

