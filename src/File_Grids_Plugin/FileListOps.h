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
#ifndef FileListOps_h
#define FileListOps_h

#include "PlutoUtils/StringUtils.h"

#include <list>
#include <string>
using namespace std;

class FileDetails
{
	public:
		string m_sBaseName;
		string m_sFileName;
		string m_sDescription;
		bool m_bIsDir;
		int m_iDirNumber;
		time_t m_tDate;

		FileDetails() : m_sBaseName(""), m_sFileName(""), m_sDescription("(not initialized)"), m_bIsDir(false), m_tDate(0) {}
		FileDetails(string sBaseName, string sFileName, bool bIsDir, int iDirNumber, time_t tDate, string sDesc = "")
			: m_sBaseName(sBaseName), m_sFileName(sFileName), m_sDescription(sDesc), m_bIsDir(bIsDir), m_iDirNumber(iDirNumber), m_tDate(tDate) {}
};

void GetDirContents(list<FileDetails *> &listFileName,string Path, string sValidExtensions_CSV);

static bool FileNameComparer(FileDetails *x, FileDetails *y)
{
	return StringUtils::ToUpper(x->m_sFileName)<StringUtils::ToUpper(y->m_sFileName);
}

static bool FileDateComparer(FileDetails *x, FileDetails *y)
{
	return x->m_tDate<y->m_tDate;
}

#endif /* FileListOps_h */
