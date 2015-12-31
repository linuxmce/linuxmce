/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#ifndef _PLUTOINC_H_
#define _PLUTOINC_H_

#include <string>
using namespace std;


class PlutoColor
{
public:
	PlutoColor() { m_Value = 0; SetAlpha(255); }
	PlutoColor(long Value) { m_Value=Value; }
	PlutoColor(unsigned char R,unsigned char G,unsigned char B) { unsigned char *ucp = (unsigned char *) &m_Value;  ucp[0]=B; ucp[1]=G; ucp[2]=R; ucp[3]=255; };
	PlutoColor(unsigned char R,unsigned char G,unsigned char B, unsigned char A) { unsigned char *ucp = (unsigned char *) &m_Value;  ucp[0]=B; ucp[1]=G; ucp[2]=R; ucp[3]=A; };
	PlutoColor(const PlutoColor &C) {m_Value=C.m_Value;  }

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
};


// Encapsulate a text style
class TextStyle
{
public:
	string m_sFont;
	int m_iPK_Style,m_iPK_StyleVariation,m_iPK_Style_Selected,m_iPK_Style_Highlighed,m_iPK_Style_Alt;
	int m_iVersion; // This is for the selected, alt, or highlighted versions
	PlutoColor m_ForeColor,m_BackColor,m_ShadowColor;
	int m_iPixelHeight;
	bool m_bBold, m_bItalic, m_bUnderline;
	int m_iShadowX, m_iShadowY;  // Pixels to offset a drop shadow
	int m_iBorderStyle;
	int m_iRotate;
	void *m_pTTF_Font; // Make this a void * for now, since trying to include the headers creates a nightmare.  We'll just cast
	int m_iPK_HorizAlignment,m_iPK_VertAlignment;  // To hold the values so we can store them with the text
	TextStyle(class Row_StyleVariation *pRow_StyleVariation);
	TextStyle() { m_pTTF_Font=NULL; } // Needed for de-serializing

	TextStyle::~TextStyle(){}
};

#define DIRECTION_Up_CONST 1
#define DIRECTION_Down_CONST 2
#define DIRECTION_Left_CONST 3
#define DIRECTION_Right_CONST 4

class PlutoPoint 
{
public:
	int X,Y;
	PlutoPoint() { X=0; Y=0;  }
	PlutoPoint(PlutoPoint *p) { X=p->X; Y=p->Y;  };
	PlutoPoint(int x,int y) {X=x; Y=y;  }
	PlutoPoint(const PlutoPoint &p)  {X=p.X; Y=p.Y;  }

	PlutoPoint operator+ (PlutoPoint &p) { return PlutoPoint(X + p.X,Y + p.Y); }
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




#endif.