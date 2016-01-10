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

#include "rect.h"

namespace Frog {



//////////////////////////////////////////////////////////////////////////////
//
// Rect
//
//////////////////////////////////////////////////////////////////////////////

void Rect::Extend( const Point& point )
{
   if (point.x < left) left = point.x;
   else if (point.x >= right) right = point.x + 1;

   if (point.y < top) top = point.y;
   else if (point.y >= bottom) bottom = point.y + 1;
}



void Rect::Extend( const Rect& rect )
{
   if (rect.left < left) left = rect.left;
   if (rect.right > right) right = rect.right;
   if (rect.top < top) top = rect.top;
   if (rect.bottom > bottom) bottom = rect.bottom;
}



void Rect::Intersect( const Rect& rect )
{
   if (left < rect.left) left = rect.left;
   if (top < rect.top) top = rect.top;
   if (right > rect.right) right = rect.right;
   if (bottom > rect.bottom) bottom = rect.bottom;
}



} // end of namespace Frog
