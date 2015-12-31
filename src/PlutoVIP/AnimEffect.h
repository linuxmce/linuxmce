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
// Written by SiGMan / iO UpG  13-04-2000
// 
//
// AnimEffect.h: interface for the AnimEffect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANIMEFFECT_H__9AED4B93_0A83_4B4E_B6ED_1B3A133E2600__INCLUDED_)
#define AFX_ANIMEFFECT_H__9AED4B93_0A83_4B4E_B6ED_1B3A133E2600__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


enum AnimType {
	AnimInit, AnimDraw, AnimErase, AnimTerm
};

struct AnimData
{
	BOOL	bOpen;
	RECT	rcScreen;
	RECT	rcWnd;
	int		iParameter;
	int		iAfterimages;
	int		iTotalSteps;
	int		iStep;
	POINT	ptRelRightTop;
	POINT	ptCenter;
	HWND	hWnd;
	HDC		hDC;
	enum	AnimType animType;
	BYTE	bBuffer[4000];
};


class AnimEffect  
{
public:
	enum EffectType {
		Random=-1,
		Spin=0,
		Vortex,
		ScatterGather,
		Spike,
		Fireworks
	};

	typedef BOOL (*EFFUNC)( AnimData * );

	AnimEffect();
	virtual ~AnimEffect();

	void Setup( int iSteps, int iAfterimages, int iDelay = 10);
	void Effect( EffectType effect );
	void Open( const CRect &rcWnd );
	void Close( const CRect &rcWnd );

private:
	void Defaults();
	void ChooseFunc();
	void Animate( AnimData &animData );

private:
	RECT	m_rcScreen;
	int		m_iParameter;
	int		m_iAfterimages;
	int		m_iTotalSteps;
	int		m_iDelay;
	EffectType	m_Effect;
	EFFUNC	m_Func;

	RECT	m_rcWnd;
	POINT	m_ptRelRightTop;
	POINT	m_ptCenter;

	HWND	m_hWnd;
	HDC		m_hDC;
	HPEN	m_hPen;
};

#endif // !defined(AFX_ANIMEFFECT_H__9AED4B93_0A83_4B4E_B6ED_1B3A133E2600__INCLUDED_)
