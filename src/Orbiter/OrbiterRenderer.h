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
#ifndef __ORBITER_RENDERER_H__
#define __ORBITER_RENDERER_H__

#include <memory>
#include <string>
//-----------------------------------------------------------------------------------------------------
#include "DesignObj_Orbiter.h"
#include "PlutoGraphic.h"
#include "SerializeClass/ShapesColors.h"
#include "DCE/Logger.h"

namespace DCE
{
	class Orbiter;
	class DataGridCell;
}
class DesignObj_DataGrid;

class BackgroundImage  // Information on an image to load in the background
{
public:
	string m_sPic;
	DesignObj_DataGrid *m_pObj_Grid;
	DataGridCell *m_pCell;
	pair<int,int> m_ColRow;
	int m_iRequestID; // Keep track of which request generated this table so we know if it's changed the cell will have been deleted
	pair<int,int> m_DataGridTable_Cache; // Which datagrid table we were caching, corresponding to m_mapDataGridTable_Cache

	BackgroundImage(string sPic,DesignObj_DataGrid *pObj_Grid,int RequestID,pair<int,int> DataGridTable_Cache,DataGridCell *pCell,pair<int,int> ColRow) 
	{ 
		m_pObj_Grid=pObj_Grid; m_pCell=pCell; m_ColRow=ColRow; m_sPic=sPic; 
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"BackgroundImage::BackgroundImage %p",this);
		m_iRequestID=RequestID;
		m_DataGridTable_Cache=DataGridTable_Cache;
	}
	~BackgroundImage()
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"BackgroundImage::~BackgroundImage %p",this);
	}
};

using namespace DCE;
//-----------------------------------------------------------------------------------------------------
class OrbiterRenderer
{
private:

	friend class OrbiterRendererFactory;

	/**
	* @brief stores objects that need to be redrawned
	* When it's time to redraw some objects without redrawing the whole screen, store them here
	* then call RedrawObjects(), rather than updating the screen over and over if several objects change at once
	*/
	vector<DesignObj_Orbiter *> m_vectObjs_NeedRedraw;
	vector<DesignObjText *> m_vectTexts_NeedRedraw;

	//helper method
	void RelativePosition(const PlutoRectangle& rectCurrentObject, 
		const PlutoRectangle& rectCandidateObject,
		int &Direction_Primary, int &Direction_Secondary, int &Distance);

protected:

	OrbiterRenderer(Orbiter *pOrbiter);

	void ConfigureDefaultTextStyle();
	virtual void UpdateScreen();

	auto_ptr<TextStyle> m_spTextStyle;
	pluto_pthread_mutex_t m_NeedRedrawVarMutex; //this will protect needredraw vectors

	//draw arrow - helper functions
	void CalcArrowValues(PlutoPoint p1, PlutoPoint p2, PlutoSize sizeArrow, PlutoPoint& arrow_p1, PlutoPoint& arrow_p2);
	void CalcArrowValuesQuad(PlutoPoint p1, PlutoPoint p2, PlutoSize sizeArrow, PlutoPoint& arrow_p1, PlutoPoint& arrow_p2);
	void CalcArrowCoords(int x, int y, double dist, double dirn, PlutoPoint& arrow);

public:

	Orbiter *m_pOrbiter;

	list<BackgroundImage *> m_listBackgroundImage;

	virtual ~OrbiterRenderer();

	Orbiter *OrbiterLogic();

	virtual void EventLoop() = 0; 
	virtual void Configure() = 0;
	virtual void Destroy() {}

	/**
	* @brief Adjust the window size
	*/
	virtual void AdjustWindowSize(int iWidth, int iHeight) {}

	/**
	* @brief renders text with the specified style
	*/
	virtual void RenderText(string &sTextToDisplay,class DesignObjText *Text, class TextStyle *pTextStyle, PlutoPoint point = PlutoPoint(0, 0), string ObjectID = "") = 0;
	virtual void BatchedTextRendering(bool bValue) {}

	/**
	* @brief draws an rectangle
	*/
	virtual void SolidRectangle(int iX, int iY, int iWidth, int iHeight, PlutoColor color, string ParentObjectID = "", string ObjectID = "") = 0;

	/**
	* @brief draws an x-or'd rectangle outline.  Used to highlight something on screen
	*/
	virtual void HollowRectangle(int X, int Y, int Width, int Height, PlutoColor color, string ParentObjectID = "", string ObjectID = "") = 0;

	/**
	* @brief draws an line
	*/
	virtual void DrawLine(int x1, int y1, int x2, int y2, PlutoColor color) = 0;

	/**
	* @brief draws an arrow
	*/
	virtual void DrawArrow(PlutoPoint p1, PlutoPoint p2, PlutoSize sizeArrow = PlutoSize(16, 12), PlutoColor color = PlutoColor::Blue(), string ParentObjectID = "", string ObjectID = "");

	/**
	* @brief replaces the specified color from within the specified rectangle with another one
	*/
	virtual void ReplaceColorInRectangle( int iX, int iY, int iWidth, int iHeight, PlutoColor ColorToReplace, PlutoColor ReplacementColor, DesignObj_Orbiter *pObj = NULL) = 0;

	/**
	* @brief floods the region that containes the specified point, replacing one color
	* A graphic is no longer on screen. Maybe remove it from cache.
	*/
	virtual void FloodFill(int iX, int iY, PlutoColor ColorToReplace, PlutoColor ReplacementColor) = 0;

	virtual void RenderGraphic(class PlutoGraphic *pPlutoGraphic, PlutoRectangle rectTotal, 
		bool bDisableAspectRatio = false, PlutoPoint point = PlutoPoint(0, 0), int nAlphaChannel = 255,
		string ParentObjectID = "", string ObjectID = "", string ObjectHash = "") = 0;

	virtual PlutoGraphic *CreateGraphic() = 0;

	/**
	* @brief We're going to be redrawing something on top of this object. Save it's state, so that during the next redraw this will be used
	*/
	virtual void SaveBackgroundForDeselect( DesignObj_Orbiter *pObj, PlutoPoint point ) = 0;
	virtual PlutoGraphic *GetBackground( PlutoRectangle &rect ) = 0;

	virtual void ShowProgress(int nPercent);
	virtual void RenderProgressBar(int nPercent);
	virtual bool DisplayProgress(string sMessage, int nProgress) { return false; }
	virtual bool DisplayProgress(string sMessage, const map<string, bool> &mapChildDevices, int nProgress);
	virtual int PromptUser(string sPrompt,int iTimeoutSeconds=10,map<int,string> *p_mapPrompts = NULL);
	virtual int PromptFor(string sToken);

	/**
	* @brief Render the screen
	*/
	virtual void RenderScreen(bool bRenderGraphicsOnly);

	virtual void BeginPaint() = 0;
	virtual void EndPaint() = 0;
	virtual void UpdateRect(PlutoRectangle rect, PlutoPoint point=PlutoPoint(0,0)) = 0;

	virtual void OnQuit() = 0;
	virtual void OnReload();

	//helpers
	virtual void ClipRectangle(int& x, int& y, int& width, int& height);
	virtual void ClipRectangle(PlutoRectangle &rect);
	virtual void RenderFrame(void *) {}

	virtual void UnSelectObject(string ParentObjectID = "", string ObjectID = "") = 0;

	/**
	* @brief Do the Highlighting of the currently highlighted object, or remove the highlighting
	*/
    virtual void DoHighlightObject();
	virtual void UnHighlightObject(bool bDeleteOnly = false);

	/**
	* @brief Find the first 'tab stop' object on screen and highlight it
	*/
	virtual void HighlightFirstObject();
	/**
	* @brief Find an object from a given direction (ie up = start at the top and move down, down=start at the bottom and move up)
	* finding any datagrid if there is one active if bPreferGrid is true, and that is only a child of pObj_Parent if that is not null,
	* and use pObj_RelativeTo as a starting point if it's specified
	*/
	virtual DesignObj_Orbiter *FindFirstObjectByDirection(char cDirection /* u,d,l,r,1 (ul),2 (ur),3(dl),4(dr) */,bool bPreferGrid,DesignObj_Orbiter *pObj_Parent,DesignObj_Orbiter *pObj_RelativeTo);

	/**
	* @brief This is used by the following function
	*/
	virtual DesignObj_Orbiter *FindObjectToHighlight( DesignObj_Orbiter *pObjCurrent, int PK_Direction );

	/**
	* @brief Given a direction ( UDLR ) find the object.  Return false if we couldn't find anything and left the highlighted object unchanged
	*/
	virtual bool HighlightNextObject( int dwPK_Direction );

	/**
	* @brief Render a shortcut for an object
	*/
	virtual void RenderShortcut(DesignObj_Orbiter *pObj);

	/**
	* @brief Hide a popup
	*/
	virtual void HidePopups(DesignObj_Orbiter *pObj);

	/**
	* @brief These will redraw any objects in m_vectObjsToRedraw. Use this to queue objects to redraw, such as those tht
	*/
	virtual void RedrawObjects();

	/*
	 *	Refreshes the screen as a task of maintenance thread
	 */
	virtual void RefreshScreen(void *data);

	/**
	* @brief renders an object on the screen async; it will add the object to m_vectObjs_NeedRedraw vector to be re-rendered
	*/
	virtual void RenderObjectAsync(DesignObj_Orbiter *pObj);

	/**
	* @brief renders a text object on the screen async; it will add the object to m_vectTexts_NeedRedraw vector to be re-rendered
	*/
	virtual void RenderTextAsync(DesignObjText *pObj);

	/*
	 *	Wrapper for ObjectOnScreen
	 */
	virtual void ObjectOnScreenWrapper();

	/**
	* @brief puts the object on screen
	* @param pObj pointer to the object to put on screen
	*/
	virtual void ObjectOnScreen( VectDesignObj_Orbiter *pVectDesignObj_Orbiter, DesignObj_Orbiter *pObj, PlutoPoint *ptPopup=NULL );

	/**
	* @brief A graphic is no longer on screen. Maybe remove it from cache
	*/
	virtual void GraphicOffScreen(vector<class PlutoGraphic*> *pvectGraphic);

	/**
	 * Any post install actions here	
	 */
	virtual void PostInitializeActions() {}

	/**
	* @brief takes out the specified object from screen
	* @param pObj the object to remove from screen
	*/
	virtual void ObjectOffScreen( DesignObj_Orbiter *pObj );

	// If bDoFirst is true it's added to the front of the list, otherwise to the back
	// Colrow is the location for the picture (the actual cell), DataGridTable_Cache is the first col/row
	// of the table we're caching so we can look it up later with DesignObj_DataGrid::DataGridTable_Get
	void BackgroundImageLoad(const char *Filename, DesignObj_DataGrid *pObj_DataGrid,int RequestID, pair<int,int> DataGridTable_Cache, DataGridCell *pCell, pair<int,int> ColRow, bool bDoFirst);

	/**
	* @brief Setup the window related tasks 
	*/
	virtual void SetupWindow() {}

	virtual bool HandleShowPopup(PlutoPopup* Popup, PlutoPoint Position, int EffectID);
	virtual bool HandleHidePopup(PlutoPopup* Popup);

	virtual void HandleRefreshCommand(string sDataGrid_ID);

	virtual void UpdateObjectImage(string sPK_DesignObj, string sType, char *pData, int iData_Size, 
		string sDisable_Aspect_Lock, bool bRefreshObject = true);

	virtual void RemoveGraphic(string ObjectID) {}

	/*
	 *	Orbiter's internal event - it will be fired as soon as as we've finished rendering an object
	 */
	virtual void ObjectRendered(DesignObj_Orbiter *pObj_Screen, PlutoPoint point){}

	/*
	 *	Orbiter's internal event - it will be fired as soon as the we've finished rendering the screen
	 */
	virtual void ScreenRendered() {}

	/*
	 *	Orbiter is processing an event
	 */
	virtual void ProcessingInputEvent() {}

	virtual void DestroyGraphic(string sObjectID) {}

	/*
	*  Floorplan specific actions
	*/
	virtual void PrepareFloorplan(DesignObj_Orbiter *pObj_Floorplan) {}
	virtual void UpdateFloorplan(DesignObj_Orbiter *pObj_Floorplan) {}
};

#endif //__ORBITER_RENDERER_H__

