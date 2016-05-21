/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/** @file FileUtils.h
 Header file for the file utils namespace
 */
#ifndef FILEUTILS
#define FILEUTILS

#ifndef SYMBIAN
	#include <cstdlib>
	#include <string>
	#include <vector>
	#include <map>
	#include <list>
	using namespace std;
#else
	#include "MyString.h" //for Symbian compatibility
	#define abs(x) Abs(x)
	#define atoi(x) SymbianAtoi(x, 0)
	#define itos(x) SymbianItoa(x)
	#define strchr(x, y) (x)
#endif

/**
@namespace FileUtils
Utilities for File access.
*/
namespace FileUtils
{
#ifndef SYMBIAN
    char *ReadFileIntoBuffer( string sFileName, size_t &Size ); /**< reads file into buffer; buffer must be freed by caller */
	bool WriteBufferIntoFile( string sFileName, const char *pBuffer, size_t Size ); /**< writes a buffer into a file */
	char *ReadURL(string sUrl, size_t &Size,bool bNullTerminate=false); /**< download a file into buffer; buffer must be freed by caller */

	bool ReadTextFile(string sFileName, string& sData);
	bool WriteTextFile(string sFileName, const string& sData);

	void ReadFileIntoVector( string sFileName, vector<string> &vectString ); /**< reads file into a vector of strings for each line */
    bool WriteVectorToFile( string sFileName, vector<string> &vectString ); /**< writes a file where the vector of strings are the lines */
    string FileSystemType( string sFileName ); /**< returns the file system type that the file resides on (ext3,nfs,cfs) */
    bool FileExists( string sFileName ); /**< checks if the specified file exists */
    long FileSize(string sFileName); /**< returns the file size or zero if error */
    string FindExtension ( string sFileName ); /**< returns the extension of the file name specified */
    void MakeDir(string sDirectory); /**< makes a directory, including all necessary parent directories, like mkdir -p in Linux */
    bool DirExists(string sDirectory); /**< Returns true if the directory exists */
	bool DelFile(string sFileName);
	bool CheckPathSanity(string sPath); /** Simple Sanity Checks for a Path */
#ifndef WIN32
	bool MoveFile(string sSourceFileName, string sDestFileName, bool bOverwrite=false); /** Moves a file from src to dest */
#endif
	bool DelDir(string sDirectory);
    string BasePath( string sFileName ); /**< returns the path one dir up @todo ask */
    string FileWithoutExtension ( string sFileName ); /**< returns the filename without the extension */
    string FilenameWithoutPath( string sFullPath, bool bIncludeExtension=true ); /**< returnes only the file name from a path/filename string */
    string IncludeTrailingSlash(string sDirectoryPath);
    string ExcludeTrailingSlash(string sDirectoryPath);
	string ValidFileName(string sInput,bool bAllowSlashes=false,bool bStrict=false); /**< converts the input file name to a valid file name (stripping / and \, for example) Strict=true makes it compatable with samba*/
	string GetSymlincDest(string sFile);
	int GetLinuxDeviceId(string sFilename);
	int GetInode(string sFilename);

#ifndef WINCE
    string ValidCPPName( string sInput ); /**< converts the input file name to a valid CPP file name */
    time_t FileDate(string sFileName); /**< returns the date of the last file modification or 0 if error */

	bool BlackListedFileSystem(string sDirectory); /**< check if a directory is on a blacklisted filesystem (ex: proc) */

	/** scan sDirectory, for files matching any of the sFileSpec_TabSep (ie *.jpg\\ta*\\tbcd*.mpg), and store the names in listFiles.
	Optionally recurse into sub-directories up to a level (0 = no limit). If the max depth was hit return true.
	The prepended path will be prepended to any files that are found.  Normally this is for internal use only while recursing.
	if pMapInodes is supplied, it will be filled in with the inodes all all files, and any files that are already in the map
	will be skipped.  This way the calling program can create such a map and pass it in, ensuring that the same files
	will not be included twice, handling self recursive symbolic links.  For Linux, if bRecurse=true and pMapInodes=NULL,
	a temporary map will be created to ensure that we do not get into any infinite recursive loops.  */
	bool FindFiles(list<string> &listFiles,string sDirectory,string sFileSpec_TabSep,bool bRecurse=false,bool bFullyQualifiedPath=false, int iMaxFileCount = 0, string PrependedPath=""
#ifndef WIN32
		,map<u_int64_t,bool> *pMapInodes=NULL
#endif
	);
	/**< same thing as FindFiles, but returns Directories instead */
	bool FindDirectories(list<string> &listDirectories,string sDirectory,bool bRecurse=false,bool bFullyQualifiedPath=false, int iMaxFileCount = 0, string PrependedPath=""
#ifndef WIN32
		,map<u_int64_t,bool> *pMapInodes=NULL
#endif
	);
	bool PUCopyFile(string sSource,string sDestination); /**< Copies a file from sSource to sDestination.  returns false if it fails, true otherwise */
	bool LaunchProcessInBackground(string sCommandLine);
    string FileChecksum( string sFileName); /**< returns file's checksum */
    string FileChecksum( char *pData, size_t iSize); /**< returns file's checksum */
	string GetMidFileChecksum( string sFileName, int Bytes=1024 ); /**< returns the checksum of the Bytes in the middle of the file for a quick check if a file has changed */
	string GetLastModifiedDateStr(string sFile); /**< returns last modified date for a file as a string */
	time_t GetLastModifiedDate(string sFile); /**< returns last modified date for a file as time_t */
	long long FileSize64(string sFileName); /**< returns the file size or zero if error, works for files larger than 2Gb */
#endif

#endif //#ifndef SYMBIAN
}

#endif

