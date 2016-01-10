//////////////////////////////////////////////////////////////////////////////
//
// PocketHAL
// Copyright 2004 by Thierry Tremblay
//
//////////////////////////////////////////////////////////////////////////////

/** @file
    Miscellaneous utility classes and functions.
*/


#ifndef POCKETHAL_UTILITY_H
#define POCKETHAL_UTILITY_H

#include <PocketHAL/config.h>



namespace PHAL
{

//////////////////////////////////////////////////////////////////////////////
//
// NonCopyable
//
//////////////////////////////////////////////////////////////////////////////

/// NonCopyable is a simple class you can derive from to prevent your class
/// from being copyable.
class NonCopyable
{
protected:
    NonCopyable() {}                                    ///< Constructor: does nothing.
    ~NonCopyable() {}                                   ///< Destructor: does nothing

private:
    NonCopyable( const NonCopyable& );                  ///< Disabled copy constructor.
    const NonCopyable& operator=( const NonCopyable& ); ///< Disabled assignment operator
};



//////////////////////////////////////////////////////////////////////////////
//
// min/max
//
//////////////////////////////////////////////////////////////////////////////

/// Return the minimum of two values.
template<typename T> inline
const T& min( const T& a, const T& b )
{
    return (a<b) ? a : b;
}

/// Return the maximum of two values.
template<typename T> inline
const T& max( const T& a, const T& b )
{
    return (b<a) ? a : b;
}



//////////////////////////////////////////////////////////////////////////////
//
// Swap
//
//////////////////////////////////////////////////////////////////////////////

/// Swap two variables.
template<typename T>
inline void swap( T& a, T& b )
{
    T t(a);
    a = b;
    b = t;
}



} // end of namespace PHAL



#endif
