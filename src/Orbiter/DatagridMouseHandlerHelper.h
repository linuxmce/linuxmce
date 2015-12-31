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
#ifndef __DatagridMouseHandlerHelper_H__
#define __DatagridMouseHandlerHelper_H__
//-----------------------------------------------------------------------------------------------------
#include <list>
#include <string>
#include <map>
using namespace std;

#include "DesignObj_Orbiter.h"
#include "Orbiter.h"
#include "PlutoUtils/ProcessUtils.h"
#include "MouseBehavior.h"
using namespace DCE;

namespace DCE
{
	//-----------------------------------------------------------------------------------------------------
	/**
	* @brief Handles special mouse behavior for lighting control
	*/
	//-----------------------------------------------------------------------------------------------------

	class DatagridMouseHandlerHelper
	{
		friend class KeyboardMouseHandler;
		friend class MediaBrowserMouseHandler;
		friend class Orbiter;

		class MouseHandler *m_pMouseHandler;
		class MouseBehavior *m_pMouseBehavior;
		int m_dwPK_Direction_ScrollGrid;
		int m_NumNotches, m_Top, m_Bottom, m_iLastNotch, m_iLastX;
		int m_iLeft,m_iRight;
		DesignObj_DataGrid *m_pObj_ScrollingGrid;
		DesignObj_Orbiter *m_pObj_MediaBrowser_Alpha;
		enum CapturingOffscreenMovement
		{
			cosm_NO,
			cosm_UP,
			cosm_DOWN
		};
		CapturingOffscreenMovement m_eCapturingOffscreenMovement;
		bool m_bHitBottom;

        /// relative mouse pointer
        ///   hide when relative motion is wanted
        ///   show the standard pointer whdn relative motion is not wanted
        // speed == 0 : normal pointer
        // speed < 0 : up
        // speed > 0 : down
    public:
	
		DatagridMouseHandlerHelper(MouseHandler *pMouseHandler,int iLeft=0,int iRight=0);
		virtual ~DatagridMouseHandlerHelper();
		void Start(DesignObj_DataGrid *pObj_ScrollingGrid,int NumNotches,int Top,int Bottom);
		void Stop();
		void ScrollGrid(int dwPK_Direction,int X,int Y);

		bool DoIteration();
		bool Move(int X,int Y,int PK_Direction);
		void RelativeMove(int X, int Y);
		bool ReleaseRelative();

		bool CapturingMouse() { return m_eCapturingOffscreenMovement != cosm_NO; }

		// changed algorithm for relative pointer
        // (may not match the current code structure)
        bool RelativePointer_SetStatus(int nSpeedShape);

        // reset variables
        void RelativePointer_Clear();

        // returns the number in the desired range
        int RelativePointer_AdjustSpeedShape(int nSpeedShape);

        // read the image from disk
        bool RelativePointer_ImageLoad(int nSpeedShape);

        // draw the image
        // if it was loaded succesfully
        void RelativePointer_ImageDraw(PlutoGraphic *pImage, const PlutoRectangle &rectFakePointer);

        // remove the image from the list of objects
        void RelativePointer_ImageRemove();

        // compute the image rectangle, based on the pre-loaded image
        // X and Y are the desired pointer coordinates
        PlutoRectangle RelativePointer_ComputeRectangle(int X, int Y, int screenWidth, int screenHeight);

    protected:
        // holds both speed and shape
        int m_nRelativePointer_SpeedShape;
        // path to the fake pointer image
        string m_sImagePath;
        // image for the fake pointer
		class PlutoGraphic * m_pRelativePointer_Image;
	};
}
//-----------------------------------------------------------------------------------------------------
#endif
