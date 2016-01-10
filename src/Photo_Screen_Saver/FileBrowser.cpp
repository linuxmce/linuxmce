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
#include "FileBrowser.h"

#include "../PlutoUtils/StringUtils.h"
#include "../DCE/Logger.h"

#include<pthread.h>

using namespace DCE;

FileBrowser::FileBrowser(string FileList) : m_BrowserMutex("file browser mutex")
{
	StringUtils::Tokenize(FileList,"\r\n",m_vectFileList);
	srand( (unsigned)time( NULL ) );

	m_BrowserMutex.Init(NULL);
}

FileBrowser::~FileBrowser(void)
{
	pthread_mutex_destroy(&m_BrowserMutex.mutex);
}

void FileBrowser::RefreshFileList(string sFileList)
{
	PLUTO_SAFETY_LOCK(vm, m_BrowserMutex);

	m_vectFileList.clear();
	StringUtils::Tokenize(sFileList,"\r\n",m_vectFileList);
}

string FileBrowser::NextFile()
{
	PLUTO_SAFETY_LOCK(vm, m_BrowserMutex);

	string Result;
	if(!m_vectFileList.size())
		return Result;

	if(m_vectFileList.size() == 1)
		return m_vectFileList[0];
		
	int Position = -1;
	do {
#ifdef DEBUG
		int r = rand();
		Position = int(r % m_vectFileList.size());
		LoggerWrapper::GetInstance()->Write(LV_ALARM, "Purpose file rand %d Position %d", r, Position);
#else
		Position = int(rand() % m_vectFileList.size());
#endif
		Result = m_vectFileList[Position];
	} while(Result == LastFile);
	LastFile = Result;

	LoggerWrapper::GetInstance()->Write(LV_ALARM, "Purpose file Position %d vect size %d file %s", Position, (int) m_vectFileList.size(), Result.c_str());

	return Result;
}
