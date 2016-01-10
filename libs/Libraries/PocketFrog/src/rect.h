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

#ifndef POCKETFROG_RECT_H
#define POCKETFROG_RECT_H

#include "defs.h"

namespace Frog {



//////////////////////////////////////////////////////////////////////////////
//
// Point
//
//////////////////////////////////////////////////////////////////////////////

class Point : public POINT
{
public:
    
   Point()                                {}
   Point( int ax, int ay )                { x = ax; y = ay; }
   Point( const POINT& p )                { *static_cast<POINT*>(this) = p; }

   // Set the point coordinates
   void Set( int ax, int ay )             { x = ax; y = ay; }

   // Translate the point by the specified amount
   void Translate( int dx, int dy )       { x += dx; y += dy; }


   inline Point& operator+=( const Point& p )    { x += p.x; y += p.y; return *this; }
   inline Point& operator-=( const Point& p )    { x -= p.x; y -= p.y; return *this; }
};



inline Point operator+( const Point& a, const Point& b ) { Point p(a); p += b; return p; }
inline Point operator-( const Point& a, const Point& b ) { Point p(a); p -= b; return p; }




//////////////////////////////////////////////////////////////////////////////
//
// Rect
//
//////////////////////////////////////////////////////////////////////////////

class Rect : public RECT
{
public:
    
   Rect()                                    {}
   Rect( int l, int t, int r, int b)         { left = l; top = t; right = r; bottom = b; }
   Rect( const RECT& r )                     { *static_cast<RECT*>(this) = r; }
   Rect( const Point& p, int w, int h )      { left = p.x; top = p.y; right = p.x + w; bottom = p.y + h; }
   Rect( const Point& p1, const Point& p2 )  { left = p1.x; top = p1.y; right = p2.x; bottom = p2.y; }

   // Info
   int   GetWidth() const                    { return right - left; }
   int   GetHeight() const                   { return bottom - top; }
   
   // Is the rectangle empty?
   bool  IsEmpty() const                     { return (right <= left) || (bottom <= top); }

   // Check if a point is inside the rectangle
   bool  Contains( const Point& point ) const { return point.x >= left && point.x < right && point.y >= top && point.y < bottom; }

   // Extend to include the specified point or rect
   void  Extend( const Point& point );
   void  Extend( const Rect& rect );

   // Compute the intersection with the specified rectangle
   void  Intersect( const Rect& rect );

   // Move the rectangle to the specified location
   void  Move( int x, int y )                { Translate( x-left, y-top ); }
   void  Move( const Point& p )              { Translate( p.x-left, p.y-top ); }

   // Set the rectangle values
   void  Set( int l, int t, int r, int b )   { left = l; top = t; right = r; bottom = b; }

   // Translate the rectangle by the specified amount
   void  Translate( int dx, int dy )         { left += dx; right += dx; top += dy; bottom += dy; }
   void  Translate( const Point& p )         { left += p.x; right += p.x; top += p.y; bottom += p.y; }
};



} // end of namespace Frog



#endif
