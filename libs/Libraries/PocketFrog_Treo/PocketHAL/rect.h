//////////////////////////////////////////////////////////////////////////////
//
// PocketHAL
// Copyright 2004 by Thierry Tremblay
//
//////////////////////////////////////////////////////////////////////////////

/** @file
    Point and Rect classes.
*/

#ifndef POCKETHAL_RECT_H
#define POCKETHAL_RECT_H



namespace PHAL
{

//////////////////////////////////////////////////////////////////////////////
//
// Point
//
//////////////////////////////////////////////////////////////////////////////

/// A Point represents a 2D coordinate using integers (x and y).
class Point
{
public:

    /// Default constructor: the Point is NOT initialized.
    Point()                                     {}

    /// Constructor
    Point( int ax, int ay )                     { x = ax; y = ay; }

    /// Set the point coordinates
    void Set( int ax, int ay )                  { x = ax; y = ay; }
    
    /// Translate the point by the specified amount
    void Translate( int dx, int dy )            { x += dx; y += dy; }
    
    /// Add a point to this one.
    inline Point& operator+=( const Point& p )  { x += p.x; y += p.y; return *this; }

    /// Substract a point from this one.
    inline Point& operator-=( const Point& p )  { x -= p.x; y -= p.y; return *this; }

    // Data
    int x;          ///< Horizontal coordinate
    int y;          ///< Vertical coordinate
};



/// Add two points together
inline Point operator+( const Point& a, const Point& b ) { Point p(a); p += b; return p; }

/// Subtract a point from another
inline Point operator-( const Point& a, const Point& b ) { Point p(a); p -= b; return p; }




//////////////////////////////////////////////////////////////////////////////
//
// Rect
//
//////////////////////////////////////////////////////////////////////////////

/// A Rect represents a rectangle using integer coordinates.
class Rect
{
public:
    
    /// Default constructor: the Rect is NOT initialized.
    Rect()                                    {}
    
    /// Constructor    
    Rect( int l, int t, int r, int b)         { left = l; top = t; right = r; bottom = b; }

    /// Constructor    
    Rect( const Point& p, int w, int h )      { left = p.x; top = p.y; right = p.x + w; bottom = p.y + h; }

    /// Return the width of the rectangle
    int   GetWidth() const                    { return right - left; }

    /// Return the height of the rectangle
    int   GetHeight() const                   { return bottom - top; }
    
    /// Return whether or not the rectangle is empty (width <= 0 or height <= 0)
    bool  IsEmpty() const                     { return (right <= left) || (bottom <= top); }
    
    /// Check if a point is inside the rectangle
    bool  Contains( const Point& point ) const { return point.x >= left && point.x < right && point.y >= top && point.y < bottom; }

    /// Move the rectangle so that the top-left corner is at the specified (x, y) coordinates.
    void  Move( int x, int y )                { Translate( x-left, y-top ); }

    /// Move the rectangle so that the top-left corner is at the specified point.
    void  Move( const Point& p )              { Translate( p.x-left, p.y-top ); }
    
    /// Set the rectangle values
    void  Set( int l, int t, int r, int b )   { left = l; top = t; right = r; bottom = b; }
    
    /// Translate the rectangle by the specified amount.
    void  Translate( int dx, int dy )         { left += dx; right += dx; top += dy; bottom += dy; }

    /// Translate the rectangle by the specified amount.
    void  Translate( const Point& p )         { left += p.x; right += p.x; top += p.y; bottom += p.y; }

    // Data
    int left;           ///< Left coordinate
    int top;            ///< Top coordinate
    int right;          ///< Right coordinate
    int bottom;         ///< Left coordinate
};



} // end of namespace PHAL



#endif
