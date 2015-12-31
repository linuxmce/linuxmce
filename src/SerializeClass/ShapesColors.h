/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/** @file ShapesColors.h
 Header file for the PlutoColor class
 */
#ifndef ShapesColor_h
#define ShapesColor_h

#include <math.h>
#include "SerializeClass/SerializeClass.h"
#define DIRECTION_Up_CONST 1
#define DIRECTION_Down_CONST 2
#define DIRECTION_Left_CONST 3
#define DIRECTION_Right_CONST 4

/** @class PlutoColor
For working with colors.

@verbatim
 alpha values:
// 0   : full-transparency
// 255 : opaque
// 128 : optimized semi-transparency
@endverbatim
*/

class PlutoColor : public SerializeClass
{
public:
	PlutoColor() { m_Value = 0; SetAlpha(255); }
	PlutoColor(long Value) { m_Value=Value; }
	PlutoColor(unsigned char R,unsigned char G,unsigned char B) { unsigned char *ucp = (unsigned char *) &m_Value;  ucp[0]=B; ucp[1]=G; ucp[2]=R; ucp[3]=255; };
	PlutoColor(unsigned char R,unsigned char G,unsigned char B, unsigned char A) { unsigned char *ucp = (unsigned char *) &m_Value;  ucp[0]=B; ucp[1]=G; ucp[2]=R; ucp[3]=A; };
	PlutoColor(const PlutoColor &C) : SerializeClass() {m_Value=C.m_Value;  }

	static PlutoColor Black() { return PlutoColor(0,0,0); }
	static PlutoColor Red() { return PlutoColor(255,0,0); }
	static PlutoColor Green() { return PlutoColor(0,255,0); }
	static PlutoColor Blue() { return PlutoColor(0,0,255); }
	static PlutoColor White() { return PlutoColor(255,255,255); }
	static PlutoColor Gray() { return PlutoColor(128,128,128); }

	PlutoColor SetAlpha(unsigned char Alpha)
	{
		A(Alpha);
		return *this;
	}

	long m_Value;
	long AsARGB() { return m_Value; };
	int A() { unsigned char *ucp = (unsigned char *) &m_Value;  return ucp[3]; };
	int R() { unsigned char *ucp = (unsigned char *) &m_Value;  return ucp[2]; };
	int G() { unsigned char *ucp = (unsigned char *) &m_Value;  return ucp[1]; };
	int B() { unsigned char *ucp = (unsigned char *) &m_Value;  return ucp[0]; };

	void A(unsigned char A) { unsigned char *ucp = (unsigned char *) &m_Value; ucp[3]=A; };
	void R(unsigned char R) { unsigned char *ucp = (unsigned char *) &m_Value; ucp[2]=R; };
	void G(unsigned char G) { unsigned char *ucp = (unsigned char *) &m_Value; ucp[1]=G; };
	void B(unsigned char B) { unsigned char *ucp = (unsigned char *) &m_Value; ucp[0]=B; };
	virtual string SerializeClassClassName() { return "PlutoColor"; }
	void SetupSerialization(int iSC_Version)
	{
		StartSerializeList() + m_Value;
	}
};

/** @class PlutoPoint
Info about a point - X,Y coords.
*/
class PlutoPoint : public SerializeClass
{
public:
	int X,Y;
	PlutoPoint() { X=0; Y=0;  }
	PlutoPoint(PlutoPoint *p) { X=p->X; Y=p->Y;  };
	PlutoPoint(int x,int y) {X=x; Y=y;  }
	PlutoPoint(const PlutoPoint &p) : SerializeClass() {X=p.X; Y=p.Y;  }

	void SetupSerialization(int iSC_Version)
	{
		StartSerializeList() + X + Y;
	}
	virtual string SerializeClassClassName() { return "PlutoPoint"; }

	PlutoPoint operator+ (PlutoPoint p) { return PlutoPoint(X + p.X,Y + p.Y); }
	bool operator==(PlutoPoint &p1) { return p1.X==X && p1.Y==Y; }

	// Given another point, what is it's relative direction and distance
	void RelativePosition(PlutoPoint &p,int &Direction_Primary,int &Direction_Secondary,int &Distance)
	{
		if( p.X>X )
		{
			if( p.Y>Y )
			{
				if( p.Y-Y > p.X-X )
				{
					Direction_Primary=DIRECTION_Down_CONST;
					Direction_Secondary=DIRECTION_Right_CONST;
				}
				else
				{
					Direction_Primary=DIRECTION_Right_CONST;
					Direction_Secondary=DIRECTION_Down_CONST;
				}
			}
			else
			{
				if( Y-p.Y > p.X-X )
				{
					Direction_Primary=DIRECTION_Up_CONST;
					Direction_Secondary=DIRECTION_Right_CONST;
				}
				else
				{
					Direction_Primary=DIRECTION_Right_CONST;
					Direction_Secondary=DIRECTION_Up_CONST;
				}
			}
		}
		else
		{
			if( p.Y>Y )
			{
				if( p.Y-Y > X-p.X )
				{
					Direction_Primary=DIRECTION_Down_CONST;
					Direction_Secondary=DIRECTION_Left_CONST;
				}
				else
				{
					Direction_Primary=DIRECTION_Left_CONST;
					Direction_Secondary=DIRECTION_Down_CONST;
				}
			}
			else
			{
				if( Y-p.Y > X-p.X )
				{
					Direction_Primary=DIRECTION_Up_CONST;
					Direction_Secondary=DIRECTION_Left_CONST;
				}
				else
				{
					Direction_Primary=DIRECTION_Left_CONST;
					Direction_Secondary=DIRECTION_Up_CONST;
				}
			}
		}
		Distance = (int) sqrt( (float) (abs(X-p.X)*abs(X-p.X) + abs(Y-p.Y)*abs(Y-p.Y)) );
	}
};


/** @class PlutoSize
Info about the size.
*/
class PlutoSize : public SerializeClass
{
public:
	int Width,Height;
	PlutoSize() {Width=0; Height=0; };
	PlutoSize(const PlutoSize &s) : SerializeClass() {Width=s.Width; Height=s.Height; };
	PlutoSize(PlutoPoint &pt)
	{
		Width=pt.X;
		Height=pt.Y;

	}
	PlutoSize(PlutoPoint *pt)
	{
		Width=pt->X;
		Height=pt->Y;

	}
	PlutoSize(int W,int H) { Width=W; Height=H;  }
	void Rotate(int iDegrees)
	{
		if( iDegrees==90 || iDegrees==270 )
		{
			int W=Width;
			Width=Height;
			Height=W;
		}
	}

	void SetupSerialization(int iSC_Version)
	{
		StartSerializeList() + Width + Height;
	}
	virtual string SerializeClassClassName() { return "PlutoSize"; }
};


/** @class PlutoRectangle
Info about a rectangle.

Define our own rect class since there's no ANSI standard for rectangles.
*/
class PlutoRectangle : public SerializeClass
{
public:
	PlutoRectangle() {X=Y=Width=Height=0; };
	PlutoRectangle(int dX, int dY, int dWidth, int dHeight) { X=dX; Y=dY; Width=dWidth; Height=dHeight;  } ;
	PlutoRectangle(int dX, int dY) { X=dX; Y=dY;  } ;
	PlutoRectangle(PlutoPoint pt,PlutoSize sz) { X=pt.X; Y=pt.Y; Width=sz.Width; Height=sz.Height; }
	PlutoRectangle(PlutoPoint origin,PlutoPoint corner) { X=origin.X; Y=origin.Y; Width=corner.X-origin.X; Height=corner.Y-origin.Y; }
	PlutoRectangle(const PlutoRectangle &Rect) : SerializeClass() { X = Rect.X; Y=Rect.Y; Width=Rect.Width; Height=Rect.Height; }

	bool operator== (const PlutoRectangle &Rect) { return X == Rect.X && Y == Rect.Y && Width == Rect.Width && Height == Rect.Height;}
	bool operator!= (const PlutoRectangle &Rect) { return !(operator==(Rect)); }

	int Top() { return Y; }
	int Bottom() { return Y + Height - 1; }
	int Left() { return X; }
	int Right() { return X + Width - 1; }

	void Bottom(int Bottom) { Height = Bottom-Y; }
	void Right(int Right) { Width = Right-X; }

	PlutoPoint Location() { return PlutoPoint(X,Y); }
	PlutoPoint Point() { return PlutoPoint(X,Y); }
	void Location(PlutoPoint p) { X=p.X; Y=p.Y; }
	PlutoSize Size() { return PlutoSize(Width,Height); }
	void Size(PlutoSize s) { Width=s.Width; Height=s.Height; }
	bool Contains(int dX, int dY)
	{
		return (dX >= X && dY >=Y && dX < (X+Width) && dY < (Y+Height));
	};
	bool Contains(const PlutoRectangle& rect)
	{
		return Contains(rect.X, rect.Y) && Contains(rect.X + rect.Width - 1, rect.Y + rect.Height - 1);
	}
	PlutoRectangle& operator=(const PlutoRectangle& Rect) { X = Rect.X; Y=Rect.Y; Width=Rect.Width; Height=Rect.Height;return *this; };
	PlutoRectangle operator+ (PlutoPoint &p) { return PlutoRectangle(X + p.X,Y + p.Y,Width,Height); }

	void SetupSerialization(int iSC_Version)
	{
		StartSerializeList() + X + Y + Width + Height;
	}

	bool IntersectsWith(PlutoRectangle &r)
	{
		if (((r.X >= X && r.X <= Right()) || (r.Right() >= X && r.Right() <= Right())) &&
			((r.Y >= Y && r.Y <= Bottom()) || (r.Bottom() >= Y && r.Bottom() <= Bottom())))
			return true;

		return false;
	}
	void Union(PlutoRectangle &RectToUnite)
	{
		PlutoRectangle NewRect;

		if (RectToUnite.X < X)
			NewRect.X = RectToUnite.X;
		else
			NewRect.X = X;

		if (RectToUnite.Y < Y)
			NewRect.X = RectToUnite.X;
		else
			NewRect.Y = Y;

		if (RectToUnite.Right() > Right())
			NewRect.Width = RectToUnite.Right() - NewRect.X;
		else
			NewRect.Width = Right() - NewRect.X;

		if (RectToUnite.Bottom() > Bottom())
			NewRect.Height = RectToUnite.Bottom() - NewRect.Y;
		else
			NewRect.Height= Bottom() - NewRect.Y;

		*this = NewRect;
	}

	void Rotate(int iDegrees,PlutoSize sizeTotal)
	{
		// This rotates and resets the origin to be always the upper left corner
		if( iDegrees==90 )
		{
			int x=X;
			X = sizeTotal.Height - Y - Height;
			Y = x;
			int W=Width;
			Width=Height;
			Height=W;
		}
		else if( iDegrees==180 )
		{
			X = sizeTotal.Width - X - Width;
			Y = sizeTotal.Height - Y - Height;
		}
		else if( iDegrees==270 )
		{
			int x=X;
			X = Y;
			Y = sizeTotal.Width - x - Width;
			int W=Width;
			Width=Height;
			Height=W;
		}
	}
	static PlutoRectangle Union(PlutoRectangle &r1,PlutoRectangle &r2)
	{
		return PlutoRectangle(
			PlutoPoint(min(r1.X,r2.X),min(r1.Y,r2.Y)),
			PlutoPoint(max(r1.Right(),r2.Right()),max(r1.Bottom(),r2.Bottom())) );
	}

	int X, Y, Width, Height;
	virtual string SerializeClassClassName() { return "PlutoRectangle"; }
};

#endif
