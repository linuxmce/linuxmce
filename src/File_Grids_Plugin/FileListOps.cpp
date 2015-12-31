/*
 FileListOps

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

#include "FileListOps.h"

#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"

#ifndef WIN32
#include "dirent.h"
#endif

using namespace DCE;

bool PathEq(string Path1, string Path2)
{
    Path1 += "/";
    Path2 += "/";

    string::size_type pos;

    while ((pos = Path1.find("//")) != string::npos)
        Path1.replace(pos, 2, "/");
    while ((pos = Path2.find("//")) != string::npos)
        Path2.replace(pos, 2, "/");

    return Path1 == Path2;
}

void GetDirContents(list<FileDetails *> &listFileNames,string Path, string sValidExtensions_CSV)
{
    string BasePath;

#ifdef DEBUG
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "get dir contents Base Path: %s",Path.c_str());
#endif
    string::size_type pos=0;
	int iDirNumber=0;
    while( (BasePath=StringUtils::Tokenize(Path,"\t",pos)).length() )
    {
        if (BasePath[BasePath.length()-1] != '/')
            BasePath += '/';

#ifdef WIN32
        intptr_t ptrFileList;
        _finddata_t finddata;

        ptrFileList = _findfirst((BasePath + "*.*").c_str(), & finddata);
        while (ptrFileList != -1)
        {
            if (finddata.attrib & _A_SUBDIR) //if (finddata.attrib == _A_SUBDIR)
            {
                if (finddata.name[0] != '.')
                {
                    FileDetails *fi = new FileDetails(BasePath, finddata.name, true, iDirNumber, finddata.time_write);
                    listFileNames.push_back(fi);
                }
            }
            else if (finddata.name[0] != '.' && FileUtils::FindExtension(finddata.name)!="id3" && FileUtils::FindExtension(finddata.name) != "lock")
            {
                string::size_type pos = 0;
                while(pos<sValidExtensions_CSV.length() || sValidExtensions_CSV.length()==0 )
                {
					char *pPtr;
                    string s = StringUtils::Tokenize(sValidExtensions_CSV, ",", pos);
                    if (s.length()>0 && s.substr(0,1) == "*")
                        s = s.substr(1);
                    if ( s.length()==0 || s == ".*" || 
						(pPtr=strstr(finddata.name, s.c_str())) != NULL && 
						pPtr-finddata.name==strlen(finddata.name)-s.size() )
                    {
                        FileDetails *fi = new FileDetails(BasePath, finddata.name, false, iDirNumber, finddata.time_write);
                        listFileNames.push_back(fi);
                        break;
                    }
                }
            }
            if (_findnext(ptrFileList, & finddata) < 0)
                break;
        }

        _findclose(ptrFileList);
#else // Linux
        DIR * dirp = opendir(BasePath.c_str());
        struct dirent entry;
        struct dirent * direntp = & entry;
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_STATUS, "opened dir %s", BasePath.c_str());
#endif
        if (dirp == NULL)
        {
            LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "opendir1 %s failed: %s", BasePath.c_str(), strerror(errno));
            continue;
        }

        // int x;
		// LSB-generic Large file support standard for accessing large files
        struct stat64 dirEntryStat;
        while (dirp != NULL && (readdir_r(dirp, direntp, & direntp) == 0) && direntp)
        {
			// LSB-generic Large file support standard for accessing large files
            if ( lstat64((BasePath + "/" + entry.d_name).c_str(), &dirEntryStat) != 0 )
            {
                LoggerWrapper::GetInstance()->Write(LV_STATUS, "Could not stat directory entry: %s (%s)", entry.d_name, strerror(errno));
                continue;
            }
			
			if (S_ISLNK(dirEntryStat.st_mode))
			{
				char buffer[1024];
				memset(buffer, 0, 1024);
				readlink((BasePath + "/" + entry.d_name).c_str(), buffer, 1023);
				if (strstr(buffer, "/mnt/device/") == buffer)
				{
					FileDetails * fi = new FileDetails(BasePath, entry.d_name, true, iDirNumber, dirEntryStat.st_mtime);
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_STATUS, "adding autofs symlink as directory");
#endif
					listFileNames.push_back(fi);
				}
				else
				{
					stat64((BasePath + "/" + entry.d_name).c_str(), &dirEntryStat);
				}
			}

            // if (entry.d_type == DT_DIR )
            if ( S_ISDIR(dirEntryStat.st_mode) )
            {
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_STATUS, "found dir entry %s", entry.d_name);
#endif
                if (entry.d_name[0] != '.') // ignore folders starting with . {hidden and the . .. ones}
                {
                    FileDetails *fi = new FileDetails(BasePath, entry.d_name, true, iDirNumber, dirEntryStat.st_mtime);
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_STATUS, "adding dir");
#endif
                    listFileNames.push_back(fi);
                }
            }
            else if ( S_ISREG(dirEntryStat.st_mode) && entry.d_name[0] != '.' && FileUtils::FindExtension(entry.d_name)!="id3" && FileUtils::FindExtension(entry.d_name)!="lock") // ignore hidden files
            {
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_STATUS, "found file entry %s", entry.d_name);
#endif
                size_t pos = 0;
                while(pos<sValidExtensions_CSV.length() || sValidExtensions_CSV.length()==0 )
                {
					char *pPtr;
                    string s = StringUtils::Tokenize(sValidExtensions_CSV, ",", pos);
                    if (s.length()>0 && s.substr(0,1) == "*")
                        s = s.substr(1);
                    if ( s.length()==0 || s == ".*" ||
						FileUtils::FindExtension(entry.d_name)==s )
                    {
                        FileDetails *fi = new FileDetails(BasePath, entry.d_name, false, iDirNumber, dirEntryStat.st_mtime);
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_STATUS, "added file %s", entry.d_name);
#endif
                        listFileNames.push_back(fi);
                        break;
                    }
                }
            }
        }
        closedir (dirp);
#endif
		iDirNumber++;
    }

#ifdef DEBUG
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "get dir contents returning: %d records",(int) listFileNames.size());
#endif
}
