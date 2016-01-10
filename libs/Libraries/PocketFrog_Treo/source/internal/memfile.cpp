//////////////////////////////////////////////////////////////////////////////
//
// PocketFrog - The Game Library for Pocket PC Devices
// Copyright 2002  Thierry Tremblay
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all copies and
// that both that copyright notice and this permission notice appear
// in supporting documentation.  Thierry Tremblay makes no representations
// about the suitability of this software for any purpose.
// It is provided "as is" without express or implied warranty.
//
//////////////////////////////////////////////////////////////////////////////

#include "memfile.h"
#include "../pocketpc.h"

namespace Frog {
namespace Internal {



//////////////////////////////////////////////////////////////////////////////
//
// MemoryFile
//
//////////////////////////////////////////////////////////////////////////////

bool MemoryFile::Open( const TCHAR* filename )
{
    if (IsOpen())
        Close();

    TCHAR pathname[MAX_PATH];
    PocketPC::GetFullPathName( filename, pathname );
    
    // Open the file
#if defined(_WIN32_WCE)
    m_hFile = ::CreateFileForMapping( pathname, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0 );
#else
    m_hFile = ::CreateFile( pathname, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0 );
#endif
    
    // Validate that the file is opened
    if (m_hFile == INVALID_HANDLE_VALUE)
        return false;
    
    // Map the file
    m_hMapping = ::CreateFileMapping( m_hFile, 0, PAGE_READONLY, 0, 0, 0 );
    if (m_hMapping==0)
    {
        ::CloseHandle( m_hFile );
        m_hFile = INVALID_HANDLE_VALUE;
        return false;
    }
    
    // Create view of file mapping
    m_pBegin = (uint8_t*)::MapViewOfFile( m_hMapping, FILE_MAP_READ, 0, 0, 0 );
    if (m_pBegin==0)
    {
        ::CloseHandle( m_hMapping );
        ::CloseHandle( m_hFile );
        m_hFile = INVALID_HANDLE_VALUE;
        return false;
    }
    
    // Finish setting up the MemoryFile object
    m_pEnd = m_pBegin + ::GetFileSize( m_hFile, 0 );
    
    
    return true;
}
    


void MemoryFile::Close()
{
    ::UnmapViewOfFile( m_pBegin );
    ::CloseHandle( m_hMapping );
    ::CloseHandle( m_hFile );
    m_hFile = INVALID_HANDLE_VALUE;
}



} // end of namespace Internal
} // end of namespace Frog
