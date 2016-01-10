/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
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
#ifndef __OrbiterRenderer_PocketFrog_H__
#define __OrbiterRenderer_PocketFrog_H__
//-----------------------------------------------------------------------------------------------------
#include "../OrbiterRenderer.h"
#include "../DesignObj_Orbiter.h"
#include "../PocketFrog/PocketFrogGraphic.h"
#include "PocketFrogWrapper.h"
#include "PlutoUtils/MultiThreadIncludes.h"
//-----------------------------------------------------------------------------------------------------
namespace DCE
{
	class Message;
}
//-----------------------------------------------------------------------------------------------------
#include <PocketFrog.h>
using namespace Frog;
//-----------------------------------------------------------------------------------------------------
class TextToRenderInfo
{
public:
	string TextToDisplay;
	PlutoRectangle rPosition;
	int iPK_HorizAlignment,iPK_VertAlignment;
	PlutoPoint point;

	string sFont;
	PlutoColor ForeColor;
	int iPixelHeight;
	bool bBold, bItalic, bUnderline;
};
//-----------------------------------------------------------------------------------------------------
class OrbiterRenderer_PocketFrog : public OrbiterRenderer, public PlutoGame
{
protected:

	bool m_bUpdating;

public:
	DisplayDevice* GetOrbiterDisplay() { return GetDisplay(); }

	OrbiterRenderer_PocketFrog(Orbiter *pOrbiter);
	~OrbiterRenderer_PocketFrog();

	void Configure() {}
	void EventLoop();

	void GetWindowPosition(PlutoPoint& point);

	//from socket class
	void PingFailed();

	// Public methods (from Game)
	void GameSuspend(); 
	void GameResume(); 

	bool IsFullScreen();
	int GetWindowWidth();
	int GetWindowHeight();

    bool GameInit();
    void GameEnd();
    void GameLoop();
	void ShowWindowCustom();

	void StylusDown( Point p, MouseButton aMouseButton );
	void StylusMove( Point p );
	void StylusUp( Point p, MouseButton aMouseButton );
	void StylusWheel( Point p, int delta);

	bool PocketFrogButtonDown(int button);
	bool PocketFrogButtonUp(int button); 
	void HandleKeyEvents(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void TryToUpdate();

	// Drawing routines
	void SolidRectangle(int x, int y, int width, int height, PlutoColor color, string ParentObjectID = "", 
		string ObjectID = "");
	void HollowRectangle(int X, int Y, int Width, int Height, PlutoColor color, string ParentObjectID = "", 
		string ObjectID = "");
	virtual void UnSelectObject(string ParentObjectID = "", string ObjectID = "") {}
	void DrawLine(int x1, int y1, int x2, int y2, PlutoColor color);
	void DrawArrow(PlutoPoint p1, PlutoPoint p2, PlutoSize sizeArrow = PlutoSize(16, 12), PlutoColor color = PlutoColor::Blue(), string ParentObjectID = "", string ObjectID = "");
	void ReplaceColorInRectangle(int x, int y, int width, int height, PlutoColor ColorToReplace, 
		PlutoColor ReplacementColor, DesignObj_Orbiter *pObj = NULL);
	void FloodFill(int x, int y, PlutoColor ColorToReplace, PlutoColor ReplacementColor) {};
	void RenderText(string &sTextToDisplay,class DesignObjText *Text,class TextStyle *pTextStyle, PlutoPoint point = PlutoPoint(0, 0), string ObjectID = "");
	void RenderText(HDC hdc,string &sTextToDisplay,PlutoRectangle &rPosition,int iPK_HorizAlignment,int iPK_VertAlignment,
		string &sFont,PlutoColor &ForeColor,int iPixelHeight,bool bBold,bool bItalic,bool bUnderline, PlutoPoint point, RECT &rectLocation);
	void BatchedTextRendering(bool bValue);
	void SaveBackgroundForDeselect(DesignObj_Orbiter *pObj, PlutoPoint point);
	PlutoGraphic *GetBackground( PlutoRectangle &rect ) ;
	PlutoGraphic *CreateGraphic();

	// Rendering
	void RenderScreen( bool bRenderGraphicsOnly );
	void RenderGraphic(class PlutoGraphic *pPlutoGraphic, PlutoRectangle rectTotal, 
		bool bDisableAspectRatio = false, PlutoPoint point = PlutoPoint(0, 0), int nAlphaChannel = 255,
		string ParentObjectID = "", string ObjectID = "", string ObjectHash = "");

	void BeginPaint();
	void EndPaint();
	void UpdateRect(PlutoRectangle rect, PlutoPoint point=PlutoPoint(0,0));

	int PromptUser(string sPrompt,int iTimeoutSeconds=10,map<int,string> *p_mapPrompts=NULL);
	bool DisplayProgress(string sMessage, int nProgress);

	void SetTime(char *ServerTimeString) {};

	static Pixel GetColor16(PlutoColor color);
	static BYTE GetRedColor(Pixel pixel);
	static BYTE GetGreenColor(Pixel pixel);
	static BYTE GetBlueColor(Pixel pixel);

	void WriteStatusOutput(const char* pMessage);

	bool m_bConnectionLost;
    void OnUnexpectedDisconnect();

	void CMD_On(int iPK_Pipe,string sPK_Device_Pipes,string &sCMD_Result,Message *pMessage);
	void CMD_Off(int iPK_Pipe,string &sCMD_Result,Message *pMessage);

    void AdjustWindowSize(int iWidth, int iHeight);
	void CalcTextRectangle(RECT &rectLocation,PlutoRectangle &rPosition,int iRotation,int iHeight,int iVertAlignment);
	void OnQuit();

protected:

	void DumpLocks();
    string FormatMutexMessage(pluto_pthread_mutex_t& PlutoMutex);
	bool m_bPoolRendering;
	vector<TextToRenderInfo *> m_vectPooledTextToRender;
};

#endif