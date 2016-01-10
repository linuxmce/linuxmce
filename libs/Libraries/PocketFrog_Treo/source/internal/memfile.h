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

#ifndef POCKETFROG_MEMFILE_H
#define POCKETFROG_MEMFILE_H

#include "../defs.h"

namespace Frog {
namespace Internal {



//////////////////////////////////////////////////////////////////////////////
//
// MemoryFile - Memory mapping of files
//
//////////////////////////////////////////////////////////////////////////////

class MemoryFile
{
public:
    
    // Create a memory file object without opening any file
    MemoryFile() : m_hFile(INVALID_HANDLE_VALUE) {}
    
    // Attempt to map the specified file in memory
    MemoryFile( const TCHAR* filename ) : m_hFile(INVALID_HANDLE_VALUE) { Open( filename ); }
    
    // Close any opened file
    ~MemoryFile() { if (IsOpen()) Close(); }
    
    // Return true if a file is opened
    bool IsOpen() const { return m_hFile != INVALID_HANDLE_VALUE; }
    
    // Open the specified file, closing any file already opened
    bool Open( const TCHAR* filename );
    
    // Close the currently opened file
    void Close();
    
    // Container access
    const uint8_t* begin() const { return m_pBegin; }
    const uint8_t* end() const   { return m_pEnd; }
    size_t         size() const  { return m_pEnd - m_pBegin; }
    
    
private:
    
    HANDLE         m_hFile;
    HANDLE         m_hMapping;
    const uint8_t* m_pBegin;
    const uint8_t* m_pEnd;
};



} // end of namespace Internal
} // end of namespace Frog



#endif
