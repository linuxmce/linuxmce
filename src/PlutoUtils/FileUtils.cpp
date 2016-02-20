/*
    FileUtils

    Copyright (C) 2004 Pluto, Inc., a Florida Corporation

    www.plutohome.com

    Phone: +1 (877) 758-8648

    This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    See the GNU General Public License for more details.
*/

#include "FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#ifndef WIN32
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <fnmatch.h>
#ifndef __APPLE_CC__
#include <sys/vfs.h>
#else
#include <sys/param.h>
#include <sys/mount.h>
#endif
#include <libgen.h>
#ifdef USE_HTTP_FETCHER
extern "C"
{
#include <http_fetcher.h>
}
#endif
#else
//	#include <shellapi.h>
#endif

#ifndef SYMBIAN
#include <stdio.h>
#include <time.h>
#include <cctype>
#include <algorithm>
#include <stdarg.h>
#include <pthread.h>

#ifdef WIN32
#include <windows.h>
#ifndef WINCE
#include <direct.h>
#include <io.h>
#endif
#else
#include <unistd.h>
#include <fcntl.h>
#endif

#ifndef WINCE
#include "md5.h"
#include <sys/types.h>
#include <sys/stat.h>
#endif
#endif //#ifndef SYMBIAN

#include "sha1.h"

char *FileUtils::ReadFileIntoBuffer( string sFileName, size_t &Size )
{
    Size=0;
    if( sFileName.length()==0 )
        return NULL;

    FILE *pFile = fopen( sFileName.c_str(), "rb" );
    if ( pFile == NULL )
        return NULL;

    fseek( pFile, 0L, SEEK_END );
    Size = ftell( pFile );
    fseek( pFile, 0L, SEEK_SET );

    char *pChr = new char[Size+1];
    fread( pChr, Size, 1, pFile );
    pChr[Size] = 0;
    fclose( pFile );
    return pChr;
}

bool FileUtils::WriteBufferIntoFile( string sFileName, const char *pBuffer, size_t Size )
{
    if( sFileName.length()==0 )
        return false;

    FILE *pFile = fopen( sFileName.c_str(), "wb" );
    if ( pFile == NULL )
        return false;

    fwrite( (const void *) pBuffer, Size, 1, pFile );
    fclose( pFile );

    return true;
}

char *FileUtils::ReadURL(string sUrl, size_t &Size,bool bNullTerminate)
{
    //	Logic:	under Windows platform there is no httpfetcher at all,
    //		so we checking for Windows first, and for httpfetcher then

#ifdef WIN32
    string sDownloadedFile = "/temp/wget.tmp";

    string sCommand = "wget " + sUrl + " -O " + sDownloadedFile;
#ifndef WINCE
    //not working under ce. todo: use ShellExecuteEx instead
    system(sCommand.c_str());
#endif	// WINCE

    char *pData = FileUtils::ReadFileIntoBuffer(sDownloadedFile, Size);
    FileUtils::DelFile(sDownloadedFile);

    return pData; // This always null terminates anyway

#else	// non-Windows platform

#ifdef USE_HTTP_FETCHER

    char * buffer = NULL;
    int iResult = http_fetch(sUrl.c_str(), &buffer);
    if( bNullTerminate && iResult>0 )
    {
        buffer = (char *) realloc(buffer, iResult + 1); // http_fetch doesn't store a \0 in the end
        buffer[iResult] = 0;
        Size = iResult;
    }
    else if( iResult>0 )
        Size = iResult;
    else
        Size = 0;
    return buffer;

#else	// we don't have libhttpfetcher here
    char fileName[] = "/tmp/PlutoFileUtils.XXXXXX";
    if (!mkstemp(fileName))
    {
        Size = 0;
        return NULL;
    }

    string sDownloadedFile = fileName;

    // For some reason, the file is 0 bytes just after the system() call is done, as if the file was not written yet
    // To avoid this, we delete the temp file and wait for it to appear after download
    // Remove file so we can wait for it to appear after the system call
    FileUtils::DelFile(sDownloadedFile);

    string sCommand = "wget -O " + sDownloadedFile + " " + sUrl;
    int iRetCode = system(sCommand.c_str());
    if (iRetCode)
    {
        Size = 0;
        return NULL;
    }

    // wait for file
    int retries = 10;
    FILE *pFile = fopen( sDownloadedFile.c_str(), "rb" );
    while (retries > 0 && pFile == NULL) {
        sleep(1);
        retries--;
        pFile = fopen( sDownloadedFile.c_str(), "rb" );
    }
    fclose(pFile);

    size_t filesize;
    char *pData = FileUtils::ReadFileIntoBuffer(sDownloadedFile, filesize);
    Size = filesize;
    FileUtils::DelFile(sDownloadedFile);

    return pData;
#endif	// USE_HTTP_FETCHER

#endif	// WIN32
}

bool FileUtils::ReadTextFile(string sFileName, string& sData)
{
    size_t nSize = 0;
    char *pData = FileUtils::ReadFileIntoBuffer(sFileName, nSize);
    bool bResult = NULL != pData;

    if(pData)
        sData = string(pData);

    delete [] pData;
    pData = NULL;
    return bResult;
}

bool FileUtils::WriteTextFile(string sFileName, const string& sData)
{
    return WriteBufferIntoFile(sFileName, sData.c_str(), sData.length());
}

void FileUtils::ReadFileIntoVector( string sFileName, vector<string> &vectString )
{
    size_t s;
    char *Buffer = ReadFileIntoBuffer( sFileName, s );
    if( Buffer )
    {
        string sBuffer(Buffer);
        StringUtils::Tokenize(sBuffer,"\n",vectString);
    }
    // Strip any \r that will be in a Windows file
    for(s=0;s<vectString.size();++s)
        vectString[s] = StringUtils::Replace(&vectString[s],"\r","");

    delete[] Buffer;
}

bool FileUtils::WriteVectorToFile( string sFileName, vector<string> &vectString )
{
    FILE *file = fopen(sFileName.c_str(),"wb");
    if( !file )
    {
        MakeDir( FileUtils::BasePath(sFileName) );
        file = fopen(sFileName.c_str(),"wb");
        if( !file )
            return false;
    }

    for(size_t s=0;s<vectString.size();++s)
    {
        fputs(vectString[s].c_str(),file);
#ifdef WIN32
        fputc('\r',file);
#endif
        fputc('\n',file);
    }
    fclose(file);
    return true;
}

bool FileUtils::FileExists( string sFile )
{
#ifndef WINCE
    struct stat buf;
    int iResult;

    if( sFile.length() && sFile[sFile.length()-1]=='/' )
        iResult = stat( sFile.substr(0, sFile.length()-1).c_str(), &buf );
    else
        iResult = stat( sFile.c_str(), &buf );

    return iResult == 0;
#else
    FILE *file = fopen(sFile.c_str(), "r");

    if(!file)
        return false;

    fclose(file);

    return true;
#endif
}

long FileUtils::FileSize(string sFile)
{
#ifndef WINCE
    struct stat buf;
    int iResult;

    iResult = stat( sFile.c_str(), &buf );

    if( iResult != 0 ) // checks if statistics are valid
        return 0;
    else
        return buf.st_size;
#else

    FILE *file = fopen(sFile.c_str(), "rb");

    if(!file)
        return 0;

    long dwSize;

    fseek(file, 0, SEEK_END);
    dwSize = ftell(file);
    fclose(file);

    return dwSize;

#endif
}

string FileUtils::FindExtension( string sFileName )
{
    // This function is a bit primitive because
    // Linux doesn't have a reverse_iterator for
    // the string class, and CE doesn't have a strlwr
    // fucntion.

    string sResult = "";
    const char *pcBeginning = sFileName.c_str();
    const char *pcPtr = pcBeginning + sFileName.length();

    while( pcPtr-- > pcBeginning )
    {
        if ( *pcPtr == '.' )
        {
            while( *(++pcPtr) )
            {
                sResult += ( *pcPtr >= 'A' && *pcPtr <= 'Z' ) ? (*pcPtr) + 32 : *pcPtr;
            }
            break;
        }
    }
    return sResult;
}

void FileUtils::MakeDir(string sDirectory)
{
    // Windows understands both
    sDirectory = StringUtils::Replace( &sDirectory, "\\", "/" );

    if( !StringUtils::EndsWith(sDirectory,"/") )
        sDirectory += "/";

    // Run it on each Directory in the path just to be sure
    string::size_type pos = 0; // string::size_type is UNSIGNED

    while( (pos = sDirectory.find( '/', pos+1 )) != string::npos )
    {
#ifdef WIN32
#ifdef WINCE

        wchar_t pDirectoryW[256];
        mbstowcs(pDirectoryW, sDirectory.c_str(), 256);
        ::CreateDirectory(pDirectoryW, NULL);

#else
        mkdir( sDirectory.substr(0,pos).c_str() );
        mkdir( sDirectory.c_str() );
#endif
#else
        mkdir( sDirectory.substr(0,pos).c_str(),0777 );
        mkdir( sDirectory.c_str(),0777 );
#endif
    }
}

#ifdef WIN32
#define S_IFDIR _S_IFDIR  // Why Microsoft?
#endif

bool FileUtils::DirExists( string sFile )
{
    int iResult = 0;

#ifndef WINCE
    struct stat buf;

    if( sFile.length() && sFile[sFile.length()-1]=='/' )
        iResult = stat( sFile.substr(0, sFile.length()-1).c_str(), &buf );
    else
        iResult = stat( sFile.c_str(), &buf );
    return iResult == 0 && buf.st_mode & S_IFDIR ;
#else

    wchar_t pDirectoryW[256];
    mbstowcs(pDirectoryW, sFile.c_str(), 256);

    WIN32_FIND_DATA find_data;
    HANDLE hFindFile = FindFirstFile(pDirectoryW, &find_data);

    if(hFindFile == INVALID_HANDLE_VALUE)
        return false;

    FindClose(hFindFile);
    return true;
#endif
}

/**
 * Check the sanity of a path name, used to prevent malicious path writing.
 * This needs to be augmented to prevent breaking out of the sanity checks
 * with a symlink.
 */
bool FileUtils::CheckPathSanity(string sPath)
{
    bool bIsHome = (sPath.find("/home/") != string::npos);
    bool bIsMntDevice = (sPath.find("/mnt/device/") != string::npos);
    bool bIsVoiceMail = (sPath.find("/var/spool/asterisk/voicemail/") != string::npos);
    return (bIsHome || bIsMntDevice || bIsVoiceMail);
}

#ifndef WIN32
bool FileUtils::MoveFile(string sSourceFileName, string sDestFileName, bool bOverwrite)
{
    bool bRet = false;
    if (!FileUtils::CheckPathSanity(sSourceFileName))
    {
        return false;
    }

    if (!FileUtils::CheckPathSanity(sDestFileName))
    {
        return false;
    }

    bool bSourceExists = (FileUtils::FileExists(sSourceFileName));
    bool bDestExists = (FileUtils::FileExists(sDestFileName));

    if (bSourceExists && bDestExists && !bOverwrite)
    {
        // Do not overwrite if bOverwrite is false.
        return false;
    }

    string sCommand = "mv " + sSourceFileName + " " + sDestFileName;

    int iRet = system(sCommand.c_str());

    if (iRet)
    {
        bRet = true;
    }
    else
    {
        bRet = false;
    }
    // FIXME implement WIN32
    return bRet;
}
#endif

bool FileUtils::DelFile(string sFileName)
{
#ifndef WINCE //no findfiles implemented under CE yet
    if( sFileName.find('*')!=string::npos || sFileName.find('?')!=string::npos )
    {
        list<string> listFiles;
        FileUtils::FindFiles(listFiles,FileUtils::BasePath(sFileName),FileUtils::FilenameWithoutPath(sFileName),false,true);
        for(list<string>::iterator it=listFiles.begin();it!=listFiles.end();++it)
            if( (*it).find('*')==string::npos && (*it).find('?')==string::npos )
                FileUtils::DelFile(*it);
    }
#endif

#ifdef WIN32
#ifdef WINCE
    wchar_t pFileNameW[256];
    mbstowcs(pFileNameW, sFileName.c_str(), 256);
    return ::DeleteFile(pFileNameW)!=0;
#else
//    wchar_t pFileNameW[256];
//    mbstowcs(pFileNameW, sFileName.c_str(), 256);
//    return ::DeleteFile(pFileNameW)!=0;
    return ::DeleteFile((LPCSTR)sFileName.c_str())!=0;
#endif

#else
    return unlink(sFileName.c_str())==0;
#endif
}

bool FileUtils::DelDir(string sDirectory)
{
#ifdef WIN32

    string sDir = sDirectory + "/*";

    WIN32_FIND_DATA findData;
    ::ZeroMemory(&findData, sizeof(findData));

//    wchar_t pDirectoryW[256];
//    mbstowcs(pDirectoryW, sDir.c_str(), 256);
//    HANDLE findFileHandle = FindFirstFile(pDirectoryW, &findData);
    HANDLE findFileHandle = FindFirstFile((LPCSTR)sDir.c_str(), &findData);

    do
    {
        string sPath = sDirectory + "/";
        string sCurrentItem;

//        char pPath[256];
//        wcstombs(pPath, findData.cFileName, 256);
//		sPath += pPath;
//		sCurrentItem = string(pPath);

		sCurrentItem = findData.cFileName;
		sPath += sCurrentItem;


        if(sCurrentItem == "." || sCurrentItem == "..")
            continue;

        if(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            FileUtils::DelDir(sPath);
        else
            FileUtils::DelFile(sPath);
    }
    while(FindNextFile(findFileHandle, &findData));

    FindClose(findFileHandle);
//    mbstowcs(pDirectoryW, sDirectory.c_str(), 256);
//    return ::RemoveDirectory(pDirectoryW)!=0;
    return ::RemoveDirectory((LPCSTR)sDirectory.c_str())!=0;
#else
    system(("rm -rf " + sDirectory).c_str());
    return true; // Don't know if it was successful or not
#endif
}

string FileUtils::BasePath( string sInput )
{
#ifdef WIN32
    sInput = StringUtils::Replace(sInput, "\\", "/"); // replacing all the \ in a windows path with /
#endif

    while ( sInput.length() > 0 && sInput[sInput.length() - 1] == '/')
        sInput = sInput.substr( 0, sInput.length() - 1 );

    string::size_type pos = sInput.rfind("/");
    if (pos == string::npos)
        sInput = "";
    else
        sInput = sInput.substr(0, pos);

#ifdef WIN32
    sInput = StringUtils::Replace(sInput, "/", "\\"); //switch back to windows format
#endif

    return sInput;
}

string FileUtils::FileWithoutExtension ( string sFileName )
{
    string sExtension = FindExtension(sFileName);
    if( sExtension.length() )
        return sFileName.substr(0, sFileName.length()-sExtension.length()-1);
    return sFileName;
}

string FileUtils::FilenameWithoutPath( string sFullPath, bool bIncludeExtension )
{
    if( sFullPath.length()<3 ) // cannot be anything but the file name
        return sFullPath;

    string::size_type iPos=0, iLastSlash=string::npos; // this way +1 will be 0.  Last slash will be -1, so if there is none, all the +1's below will start with the first character

#ifdef WIN32
    string::size_type s;
    while( ( s = sFullPath.find('\\') ) != string::npos ) // replacing all the \ in a windows path with /
        sFullPath.replace( s, 1, "/" );
#endif

    iPos--; // We start with a +1 below
    while( ( iPos = sFullPath.find( "/", iPos+1 ) ) != string::npos && iPos < sFullPath.length()-1 ) // finding the last slash
        iLastSlash = iPos;

    if( bIncludeExtension ) // @ask
    {
        if( sFullPath[sFullPath.length()-1]=='/' )
            return sFullPath.substr( iLastSlash+1, sFullPath.length()- iLastSlash-2 );
        else
            return sFullPath.substr( iLastSlash+1 );
    }

    if( sFullPath[sFullPath.length()-1]=='/' )
        sFullPath = sFullPath.substr( iLastSlash+1, sFullPath.length() - iLastSlash - 2 );
    else
        sFullPath = sFullPath.substr( iLastSlash+1 );

    iPos=0; --iPos;// We start with a +1 below
    while( ( iPos = sFullPath.find( ".", iPos+1 )) != string::npos && iPos < sFullPath.length()-1 )
        iLastSlash = iPos;

    if( iLastSlash != string::npos) // 10/30/04 aaron -- what was this for??? ->  it means a long extension and short filename don't work && iLastSlash >= sFullPath.length()-5 )
        return sFullPath.substr( 0, iLastSlash );
    else
        return sFullPath;
}

string FileUtils::IncludeTrailingSlash(string sDirectoryPath)
{
    char cLastChar = sDirectoryPath[sDirectoryPath.size() - 1];
    if(sDirectoryPath.size() && cLastChar != '/' && cLastChar != '\\')
        sDirectoryPath = sDirectoryPath + "/";

    return sDirectoryPath;
}

string FileUtils::ExcludeTrailingSlash(string sDirectoryPath)
{
    char cLastChar = sDirectoryPath[sDirectoryPath.size() - 1];
    if(sDirectoryPath.size() && (cLastChar == '/' || cLastChar == '\\'))
        sDirectoryPath = sDirectoryPath.substr(0, sDirectoryPath.size() - 1);

    return sDirectoryPath;
}

string FileUtils::ValidFileName(string sInput,bool bAllowSlashes,bool bStrict)
{
    StringUtils::TrimSpaces(sInput); // eliminting the spaces
    StringUtils::Replace(&sInput,"\\"," ");
    if( !bAllowSlashes )
        StringUtils::Replace(&sInput,"/"," ");
    StringUtils::Replace(&sInput,"`","'");
    StringUtils::Replace(&sInput,"|"," ");
    StringUtils::Replace(&sInput,">"," ");
    StringUtils::Replace(&sInput,"<"," ");
    StringUtils::Replace(&sInput,"\"","'");
    StringUtils::Replace(&sInput,"*","'");
    StringUtils::Replace(&sInput,"?","");
    if( bStrict )
    {
        StringUtils::Replace(&sInput,"'"," ");
        StringUtils::Replace(&sInput,":"," ");
    }
    return sInput;
}

#ifndef WINCE

string FileUtils::ValidCPPName(string sInput)
{
    StringUtils::TrimSpaces(sInput); // eliminting the spaces

    // replacing all the invalid characters
    StringUtils::Replace(&sInput," ","_");
    StringUtils::Replace(&sInput,":","_");
    StringUtils::Replace(&sInput,"=","_");
    StringUtils::Replace(&sInput,",","_");
    StringUtils::Replace(&sInput,"@","_");
    StringUtils::Replace(&sInput,"-","");
    StringUtils::Replace(&sInput,"/","");
    StringUtils::Replace(&sInput,"#","Num");
    StringUtils::Replace(&sInput,"*","");
    StringUtils::Replace(&sInput,"$","");
    StringUtils::Replace(&sInput,".","");
    StringUtils::Replace(&sInput,"\"","");
    StringUtils::Replace(&sInput,"(","_");
    StringUtils::Replace(&sInput,")","_");
    StringUtils::Replace(&sInput,"?","_");
    StringUtils::Replace(&sInput,"!","_");
    StringUtils::Replace(&sInput,"<","_");
    StringUtils::Replace(&sInput,">","");
    StringUtils::Replace(&sInput,"__","_");
    StringUtils::Replace(&sInput,"__","_");
    StringUtils::Replace(&sInput,"%","");
    StringUtils::Replace(&sInput,"&","");
    StringUtils::Replace(&sInput,"+","");
    StringUtils::Replace(&sInput,"'","");
    StringUtils::Replace(&sInput,"�","");

    // taking out the underscores from the begining of the file name
    while(sInput.length() && sInput[0]=='_')
        sInput=sInput.substr(1);

    // taking out the underscores from the end of the file name
    while(sInput.length() && sInput[ sInput.length()-1 ]=='_')
        sInput=sInput.substr(0,sInput.length()-1);

    return sInput;
}

time_t FileUtils::FileDate(string sFile)
{
    struct stat buf;
    int iResult;

    iResult = stat( sFile.c_str(), &buf );

    if( iResult != 0 ) // checks if statistics are valid
        return 0;
    else
        return buf.st_mtime;

}

/*
 * Blacklist filesystems
 */
bool FileUtils::BlackListedFileSystem(string sDirectory)
{
#ifndef WIN32
    struct statfs s;
    if (statfs(sDirectory.c_str(), &s) == -1)
    {
        printf("Error doing statfs on '%s'.\n", sDirectory.c_str());
        return true;
    }

    switch (s.f_type)
    {
    case 0x9fa0 /*PROC_SUPER_MAGIC*/:
        return true;
        break;
    }
#endif
    // if we don't know anything about it, it means it's not on the black list
    return false;
}

#include <iostream>
using namespace std;

bool FileUtils::FindFiles(list<string> &listFiles,string sDirectory,string sFileSpec_TabSep,bool bRecurse,bool bFullyQualifiedPath, int iMaxFileCount, string PrependedPath
                          #ifndef WIN32
                          ,map<u_int64_t,bool> *pMapInodes
                          #endif
                          )
{
    if( !StringUtils::EndsWith(sDirectory,"/") )
        sDirectory += "/";

#ifdef WIN32
    intptr_t ptrFileList;
    _finddata_t finddata;

    ptrFileList = _findfirst((sDirectory + "*.*").c_str(), & finddata);
    while (ptrFileList != -1)
    {
        if (!(finddata.attrib & _A_SUBDIR) && finddata.name[0] != '.')
        {
            string::size_type pos = 0;
            while(true)
            {
                string s = StringUtils::Tokenize(sFileSpec_TabSep, "\t", pos);
                if (s.size() && s.substr(0,1) == "*")
                    s = s.substr(1);  // If it's a *.ext drop the *

                if ( sFileSpec_TabSep.size()==0 || (s.size() && (s == ".*" || s == "*" || StringUtils::EndsWith(finddata.name, s.c_str(),true) )) ||
                     (sFileSpec_TabSep[sFileSpec_TabSep.size()-1]=='*' && StringUtils::StartsWith(finddata.name,sFileSpec_TabSep.substr(0,sFileSpec_TabSep.size()-1),true)) )
                {
                    if( bFullyQualifiedPath )
                        listFiles.push_back(sDirectory + finddata.name);
                    else
                        listFiles.push_back(PrependedPath + finddata.name);
                    break;
                }
                if (pos>=sFileSpec_TabSep.size())
                    break;
            }

            if ( iMaxFileCount && iMaxFileCount < int(listFiles.size()) )
            {
                _findclose(ptrFileList);
                return true; // max depth hit
            }
        }
        else if (bRecurse && (finddata.attrib & _A_SUBDIR) && finddata.name[0] != '.')
        {
            if ( FindFiles(listFiles, sDirectory + finddata.name, sFileSpec_TabSep, true, bFullyQualifiedPath, iMaxFileCount, PrependedPath + finddata.name + "/") )
            {
                _findclose(ptrFileList);
                return true; // if one recursive call hit the maximum depth then return now.
            }
        }
        if (_findnext(ptrFileList, & finddata) < 0)
            break;
    }

    _findclose(ptrFileList);
#else // Linux
    if (BlackListedFileSystem(sDirectory))
        return false;

    bool bCreatedTempMap=false;
    map<u_int64_t,bool>::iterator itInode;
    if( bRecurse && pMapInodes==NULL )
    {
        bCreatedTempMap=true;
        pMapInodes = new map<u_int64_t,bool>;
    }
    DIR * dirp = opendir(sDirectory.c_str());
    struct dirent entry;
    struct dirent * direntp = & entry;

    if (dirp == NULL)
    {
        return false;
    }

    while (dirp != NULL && (readdir_r(dirp, direntp, & direntp) == 0) && direntp)
    {
        if( pMapInodes )
        {
            if( (itInode=pMapInodes->find(entry.d_ino))!=pMapInodes->end() )
                continue;
            else
                (*pMapInodes)[entry.d_ino]=true;
        }

        struct stat s;
        lstat((sDirectory + entry.d_name).c_str(), &s);

        if ( S_ISLNK(s.st_mode) )
        {
            char pLinkContents[4096];
            memset(pLinkContents, 0, 4096);
            if ( -1 == readlink((sDirectory + entry.d_name).c_str(),  pLinkContents, 4095) ) // does not append NULL character
            {
                printf("Error reading link contents for symlink %s (%s).\n", (sDirectory + entry.d_name).c_str(), strerror(errno));
                continue;
            }
            if ( pLinkContents[0] == '.' && s.st_size == 1 )
            {
                continue;
            }

            stat((sDirectory + entry.d_name).c_str(), &s);
        }

        if (!S_ISDIR(s.st_mode) && entry.d_name[0] != '.' )
        {
            size_t pos = 0;
            while(true)
            {
                string sShellPattern = StringUtils::BashPatternEscape(StringUtils::Tokenize(sFileSpec_TabSep, "\t", pos), "*?");

                /* somebody didn't test fnmatch well.  file: "The Patriot [Special Edition].dvd" doesn't match "The Patriot [Special Edition].*"
                so I added the StringUtils::StartsWith */
                if (sFileSpec_TabSep.size()==0 || fnmatch(sShellPattern.c_str(), entry.d_name, FNM_CASEFOLD) == 0 || StringUtils::EndsWith(entry.d_name, sShellPattern.c_str(), true)
                        || (sFileSpec_TabSep[sFileSpec_TabSep.size() - 1] == '*' && StringUtils::StartsWith(entry.d_name, sFileSpec_TabSep.substr(0, sFileSpec_TabSep.size() - 1))))
                {
                    if( bFullyQualifiedPath )
                        listFiles.push_back(sDirectory + entry.d_name);
                    else
                        listFiles.push_back(PrependedPath + entry.d_name);
                    break;
                }
                if (pos>=sFileSpec_TabSep.size())
                    break;
            }

            if ( iMaxFileCount && iMaxFileCount < (int)listFiles.size() )
            {
                if( bCreatedTempMap )
                    delete pMapInodes;
                closedir(dirp);
                return true;
            }
        }
        else if (bRecurse && S_ISDIR(s.st_mode) && entry.d_name[0] != '.')
        {
            if ( FindFiles( listFiles, sDirectory + entry.d_name, sFileSpec_TabSep, true, bFullyQualifiedPath, iMaxFileCount, PrependedPath + entry.d_name + "/",pMapInodes ) )
            {
                if( bCreatedTempMap )
                    delete pMapInodes;
                closedir(dirp);
                return true;
            }
        }
    }
    closedir (dirp);

    if( bCreatedTempMap )
        delete pMapInodes;
#endif

    return false; // if we are here it means we didn't hit the bottom return false.
}

bool FileUtils::FindDirectories(list<string> &listDirectories,string sDirectory,bool bRecurse,bool bFullyQualifiedPath, int iMaxFileCount, string PrependedPath
                                #ifndef WIN32
                                ,map<u_int64_t,bool> *pMapInodes
                                #endif
                                )
{
    if( !StringUtils::EndsWith(sDirectory,"/") )
        sDirectory += "/";

#ifdef WIN32
    intptr_t ptrFileList;
    _finddata_t finddata;

    ptrFileList = _findfirst((sDirectory + "*.*").c_str(), & finddata);
    while (ptrFileList != -1)
    {
        if ( finddata.attrib & _A_SUBDIR && finddata.name[0] != '.')
        {
            if( bFullyQualifiedPath )
                listDirectories.push_back(sDirectory + finddata.name);
            else
                listDirectories.push_back(PrependedPath + finddata.name);

            if ( iMaxFileCount && iMaxFileCount < int(listDirectories.size()) )
            {
                _findclose(ptrFileList);
                return true; // max depth hit
            }
            if (bRecurse && FindDirectories(listDirectories, sDirectory + finddata.name, true, bFullyQualifiedPath, iMaxFileCount, PrependedPath + finddata.name + "/") )
            {
                _findclose(ptrFileList);
                return true; // if one recursive call hit the maximum depth then return now.
            }
        }
        if (_findnext(ptrFileList, & finddata) < 0)
            break;
    }

    _findclose(ptrFileList);
#else // Linux
    if (BlackListedFileSystem(sDirectory))
        return false;

    bool bCreatedTempMap=false;
    map<u_int64_t,bool>::iterator itInode;
    if( bRecurse && pMapInodes==NULL )
    {
        bCreatedTempMap=true;
        pMapInodes = new map<u_int64_t,bool>;
    }

    DIR * dirp = opendir(sDirectory.c_str());
    struct dirent entry;
    struct dirent * direntp = & entry;

    if (dirp == NULL)
    {
        return false;
    }

    while (dirp != NULL && (readdir_r(dirp, direntp, & direntp) == 0) && direntp)
    {
        if( pMapInodes )
        {
            if ( (itInode=pMapInodes->find(entry.d_ino))!=pMapInodes->end() )
                continue;
            else
                (*pMapInodes)[entry.d_ino]=true;
        }

        struct stat s;
        lstat((sDirectory + entry.d_name).c_str(), &s);

        if ( S_ISLNK(s.st_mode) )
        {
            char *pLinkContents = new char[s.st_size + 1];
            if ( -1 == readlink((sDirectory + entry.d_name).c_str(),  pLinkContents, s.st_size) )
            {
                printf("Error reading link contents for symlink %s (%s).\n", (sDirectory + entry.d_name).c_str(), strerror(errno));
                delete[] pLinkContents;
                continue;
            }
            if ( pLinkContents[0] == '.' && s.st_size == 1 )
            {
                delete[] pLinkContents;
                continue;
            }

            delete[] pLinkContents;
            stat((sDirectory + entry.d_name).c_str(), &s);
        }

        if (S_ISDIR(s.st_mode) && entry.d_name[0] != '.' )
        {
            if( bFullyQualifiedPath )
                listDirectories.push_back(sDirectory + entry.d_name);
            else
                listDirectories.push_back(PrependedPath + entry.d_name);

            if ( iMaxFileCount && iMaxFileCount < (int)listDirectories.size() )
            {
                if( bCreatedTempMap )
                    delete pMapInodes;
                closedir(dirp);
                return true;
            }

            if (bRecurse && FindDirectories( listDirectories, sDirectory + entry.d_name, true, bFullyQualifiedPath, iMaxFileCount, PrependedPath + entry.d_name + "/",pMapInodes ) )
            {
                if( bCreatedTempMap )
                    delete pMapInodes;
                closedir(dirp);
                return true;
            }
        }
    }
    if( bCreatedTempMap )
        delete pMapInodes;
    closedir (dirp);
#endif

    return false; // if we are here it means we didn't hit the bottom return false.
}
// Use a 1 meg buffer
#define BUFFER_SIZE		1000000
#include <iostream>
bool FileUtils::PUCopyFile(string sSource,string sDestination)
{
#ifndef WIN32
    system(("mkdir -p \"" + FileUtils::BasePath(sDestination) + "\"").c_str());

    struct stat statinfo;
    if( lstat(sSource.c_str(), &statinfo) < 0)
        return false;

    if( S_ISLNK(statinfo.st_mode)) // Symlinks should not be binary copied.
    {
        system(("cp -a \"" + sSource + "\" \"" + sDestination + "\"").c_str());
        return true;
    }

#endif

    FILE *fileSource = fopen(sSource.c_str(),"rb");
    if( !fileSource )
        return false;

    FILE *fileDest = fopen(sDestination.c_str(),"wb");
    if( !fileDest )
    {
        fclose(fileSource);
        return false;
    }

#ifndef WIN32
    {
        struct stat srcStat;
        fstat(fileno(fileSource), &srcStat);
        fchmod(fileno(fileDest), srcStat.st_mode);
    }
#endif

    void *Buffer = malloc(BUFFER_SIZE);
    size_t BytesRead;
    while((BytesRead = fread(Buffer,1,BUFFER_SIZE,fileSource)) > 0)
    {
        size_t BytesWritten = fwrite(Buffer,1,BytesRead,fileDest);
        if( BytesWritten!=BytesRead )
        {
            fclose(fileSource);
            fclose(fileDest);
            free(Buffer);
            return false;
        }
    }
    fclose(fileSource);
    fclose(fileDest);
    free(Buffer);
    return true;
}

bool FileUtils::LaunchProcessInBackground(string sCommandLine)
{
    if (sCommandLine == "")
    {
        return false;
    }

#ifndef WIN32
    //parse the args
    const int MaxArgs = 32;

    char * ptr;

    ptr = (char *) sCommandLine.c_str();
    char * args[MaxArgs];
    int i = 0;

    // this looks too complicated but i don't have time to make it cleaner :-( mtoader@gmail.com)
    args[0] = (char *) sCommandLine.c_str();
    while ( *ptr && i < MaxArgs - 1)
    {
        if ( *ptr == ' ' || *ptr == '\t' )
            *ptr++ = 0;
        while ( *ptr && (*ptr == ' ' || *ptr == '\t') ) ptr++;  // skip the white spaces.
        if ( *ptr )
        {
            args[++i] = ptr; 			// put the next thing as a parameter
            while ( *ptr && *ptr != ' ' && *ptr != '\t' ) ptr++;  // skip to the next white space. (this doesn't take into account quoted parameters) )
        }
    }

    args[++i] = 0;
    fprintf(stderr, "Found %d arguments\n", i);

    for (int x = 1 ; x < i; x++)
        fprintf(stderr, "Argument %d: %s\n", x, args[x]);

    pid_t pid = fork();
    switch (pid)
    {
    case 0: //child
    {
        fprintf(stderr, "FileUtils::LaunchProcessInBackground(), Fork was succesfull.\n");

        // set the Close on Exec flag on all the descriptors bigger than 3.
        for ( int i = 3; i < 255; i++ )
            close(i);

        fprintf(stderr, "File descriptors closed.\n");

        if ( execvp(args[1], args + 1) == -1)
        {
            fprintf(stderr, "Exec failed %s\n", strerror(errno));
            exit(99);
        }
    }

    case -1:
        fprintf(stderr, "Error starting %s, err: %s\n", sCommandLine.c_str(), strerror(errno));
        return false;

    default:
        return true;
    }
#else
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory (&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory (&pi, sizeof(pi));
//	CreateProcess(L"C:\\WINDOWS\\system32\\cmd.exe", L"/c bogus.bat", NULL, NULL, false, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	CreateProcess("C:\\WINDOWS\\system32\\cmd.exe", "/c bogus.bat", NULL, NULL, false, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	return true;
#endif
}

string FileUtils::FileChecksum( string sFileName)
{
    size_t iSize = 0;
    char *pData = ReadFileIntoBuffer(sFileName, iSize);
    if( pData == NULL )
        return "";

    string sChecksum = FileChecksum(pData, iSize);
    delete[] pData;

    return sChecksum;
}

string FileUtils::FileChecksum( char *pData, size_t iSize)
{
    MD5_CTX ctx;
    MD5Init(&ctx);
    MD5Update(&ctx, (unsigned char *)pData, (unsigned int)iSize);
    unsigned char digest[16];
    MD5Final(digest, &ctx);

    char tmp[3];
    string md5;
    for (int i = 0; i < 16; i++)
    {
        sprintf(tmp, "%02x", digest[i]);
        md5 += tmp;
    }

    return md5;
}

string FileUtils::GetMidFileChecksum( string sFileName, int Bytes )
{
    long FileSize = FileUtils::FileSize(sFileName);
    if( FileSize<0 )
        return "";

    int StartPosition=0;

    if( FileSize>Bytes*2 )
        StartPosition = (FileSize / 2) - (Bytes / 2);

    if( FileSize<Bytes )
        Bytes=FileSize;

    FILE *file = fopen(sFileName.c_str(), "rb");

    if(!file)
        return "";

    char *pBuffer = new char[Bytes];
    int iReturn = fseek(file, StartPosition, SEEK_SET);
    size_t read = fread(pBuffer,1,Bytes,file);
    if( read!=Bytes )
    {
        fclose(file);
        return ""; // Shouldn't happen
    }

    fclose(file);

    string sMd5 = FileUtils::FileChecksum(pBuffer,read);
    delete[] pBuffer;
    return sMd5;
}

string FileUtils::GetLastModifiedDateStr(string sFile)
{
#ifndef ANDROID
    struct stat buf;
    memset(&buf, 0, sizeof(buf));

    string sItemWithoutTrailingSlash = FileUtils::ExcludeTrailingSlash(sFile);
    int result = stat(sItemWithoutTrailingSlash.c_str(), &buf);
    if(!result && (buf.st_mode & S_IFDIR))
    {
        struct tm tm;
#ifdef WIN32
		_localtime64_s(&tm, &buf.st_mtime);
#else
		localtime_r(&buf.st_mtime,&tm);   // Convert time to struct tm form
#endif
		char acDateTime[100];
        sprintf( acDateTime, "%04d%02d%02d%02d%02d%02d",
                 tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec );

        return acDateTime;
    }
#endif
    return "";
}

time_t FileUtils::GetLastModifiedDate(string sFile)
{
    struct stat buf;
    memset(&buf, 0, sizeof(buf));

    string sItemWithoutTrailingSlash = FileUtils::ExcludeTrailingSlash(sFile);
    int result = stat(sItemWithoutTrailingSlash.c_str(), &buf);
    if(!result && buf.st_mode)
        return buf.st_mtime;

    return -1;
}

string FileUtils::GetSymlincDest(string sFile)
{
#ifdef WIN32
    return "";
#else
    char pLinkContents[4096];
    memset(pLinkContents, 0, 4096);

    if ( -1 == readlink(sFile.c_str(),  pLinkContents, 4095) ) // does not append NULL character
        return "";
    else
    {
        char *str = strdup( sFile.c_str() );
        char *dir = dirname(str);
        string sCombined = string(dir) + "/" + pLinkContents;
        free(str);
        return sCombined;
    }
#endif
}

int FileUtils::GetLinuxDeviceId(string sFilename)
{
#ifdef WIN32
    return -1;
#else
    struct stat st;
    stat(sFilename.c_str(), &st);
    return st.st_rdev;
#endif
}

int FileUtils::GetInode(string sFilename)
{
#ifdef WIN32
    return -1;
#else
    struct stat st;
    if( stat(sFilename.c_str(), &st)!=0 )
        return -1;
    return st.st_ino;
#endif
}

long long FileUtils::FileSize64(string sFile)
{
#ifdef WIN32
    return 0;
#else
    struct stat sb;
    if ( stat(sFile.c_str(), &sb) == -1 )
        return 0;
    else
    {
        return sb.st_size;
    }
#endif
}

string FileUtils::FileSHA1(string sFileName)
{
  size_t iSize=0;
  char *pData = ReadFileIntoBuffer(sFileName,iSize);

  if (pData == NULL)
    return "";

  string sSHA1 = FileSHA1(pData,iSize);
  delete[] pData;

  return sSHA1;
}

string FileUtils::FileSHA1(char* pData, size_t iSize)
{
  sha1nfo s;
  char tmp[3];
  uint8_t* hash_result;
  string sha1;

  if (!pData || iSize<1)
    {
      return "";
    }

  /* calculate sha1 */
  sha1_init(&s);
  sha1_write(&s,pData,iSize);
  hash_result = sha1_result(&s);
  for (int i=0; i<20; i++)
    {
      sprintf(tmp,"%02x",hash_result[i]);
      sha1 += tmp;
    }

  return sha1;
}

#endif //not WINCE
