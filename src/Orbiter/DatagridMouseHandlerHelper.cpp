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
#include "DatagridMouseHandlerHelper.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/Other.h"
#include "MouseGovernor.h"
#include "MouseIterator.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "pluto_main/Define_Button.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_Screen.h"
#include "OrbiterRenderer.h"
#include "DataGrid.h"
#include "ScreenHistory.h"

#ifdef ORBITER_OPENGL
#include "OpenGL/OrbiterRenderer_OpenGL.h"
#include "OpenGL/DataGridRenderer_OpenGL.h"
#include "OpenGL/OpenGL3DEngine.h"
#endif

using namespace DCE;

#define IMAGE_FAKE_POINTER_NAME "image_fake_pointer"

DatagridMouseHandlerHelper::DatagridMouseHandlerHelper(MouseHandler *pMouseHandler,int iLeft,int iRight)
{ 
	m_pMouseHandler=pMouseHandler;
	m_pMouseBehavior=m_pMouseHandler->m_pMouseBehavior;
	m_dwPK_Direction_ScrollGrid=0; m_pObj_MediaBrowser_Alpha=m_pObj_ScrollingGrid=NULL;
    m_nRelativePointer_SpeedShape=0;
    m_pRelativePointer_Image=NULL;
	m_eCapturingOffscreenMovement=cosm_NO;
	m_bHitBottom=false;
	m_iLeft=iLeft;
	m_iRight=iRight;
}

DatagridMouseHandlerHelper::~DatagridMouseHandlerHelper()
{
	RelativePointer_Clear();
}

void DatagridMouseHandlerHelper::Start(DesignObj_DataGrid *pObj_ScrollingGrid,int NumNotches,int Top,int Bottom)
{
	m_dwPK_Direction_ScrollGrid = 0;
	m_pObj_ScrollingGrid=pObj_ScrollingGrid;
	m_NumNotches=NumNotches; m_Top=Top; m_Bottom=Bottom;
	m_bHitBottom=false;

	m_iLastNotch=-1;

	if(NULL != m_pMouseHandler->m_pObj)
		m_pObj_MediaBrowser_Alpha = m_pMouseBehavior->m_pOrbiter->FindObject(m_pMouseHandler->m_pObj->m_ObjectID + "." + StringUtils::itos(DESIGNOBJ_icoVertAlpha_CONST ));
	else 
		m_pObj_MediaBrowser_Alpha = NULL;

	if( m_pObj_MediaBrowser_Alpha && !m_pObj_MediaBrowser_Alpha->m_bOnScreen )
		m_pObj_MediaBrowser_Alpha = NULL;
}

void DatagridMouseHandlerHelper::Stop()
{
	m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_None,0,"",0,NULL); // In case we're scrolling a grid
}
	
void DatagridMouseHandlerHelper::ScrollGrid(int dwPK_Direction,int X,int Y)
{
	if( dwPK_Direction!=m_dwPK_Direction_ScrollGrid)
	{
LoggerWrapper::GetInstance()->Write(LV_FESTIVAL,"DatagridMouseHandlerHelper::ScrollGrid direction changed from %d to %d",
					  m_dwPK_Direction_ScrollGrid,dwPK_Direction);
		// We moved past the top
		m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_None,0,"",0,NULL); // In case we're scrolling a grid

		if( m_dwPK_Direction_ScrollGrid==DIRECTION_Up_CONST )
			m_pMouseBehavior->SetMousePosition( m_pObj_ScrollingGrid->m_rPosition.X + m_pObj_ScrollingGrid->m_pPopupPoint.X + m_pObj_ScrollingGrid->m_rPosition.Width/2 , m_pObj_ScrollingGrid->m_rPosition.Y + m_pObj_ScrollingGrid->m_pPopupPoint.Y );
		else
		{
			if( m_pObj_ScrollingGrid->DataGridTable_Get() && m_pObj_ScrollingGrid->m_iHighlightedRow!=-1 && m_pObj_ScrollingGrid->m_GridCurRow + m_pObj_ScrollingGrid->m_iHighlightedRow >= m_pObj_ScrollingGrid->DataGridTable_Get()->GetRows() )
				m_pObj_ScrollingGrid->m_iHighlightedRow = m_pObj_ScrollingGrid->DataGridTable_Get()->GetRows() - m_pObj_ScrollingGrid->m_GridCurRow - 1;

			PlutoRectangle r;
			m_pObj_ScrollingGrid->GetGridCellDimensions(
				1, // col span
				1, // row span
				0, // column
				m_pObj_ScrollingGrid->m_iHighlightedRow /* 0 based */, // We only care about the position of the last visible row, whether or not anything is there
				r.X,  r.Y,  r.Width,  r.Height );

			// Move back to the bottom of the grid, since we're done with the iterator
			m_pMouseBehavior->SetMousePosition( m_pObj_ScrollingGrid->m_rPosition.X + m_pObj_ScrollingGrid->m_pPopupPoint.X + m_pObj_ScrollingGrid->m_rPosition.Width/2 , r.Bottom() + m_pObj_ScrollingGrid->m_pPopupPoint.Y );
		}

		// Unhighlight the speed
		m_dwPK_Direction_ScrollGrid = 0;
		m_pMouseHandler->m_iLastNotch = -999;

		return;
	}

	// 11 notches, 0=pause, 5 scroll, 5 pages
	int NotchWidth = m_pObj_ScrollingGrid->m_rPosition.Height/11;
	int Notch = (Y-m_pObj_ScrollingGrid->m_rPosition.Y-m_pObj_ScrollingGrid->m_pPopupPoint.Y)/NotchWidth;
	if( Notch<0 )
		Notch = 0;
	if( Notch>10 )
		Notch = 10;

	if( dwPK_Direction==DIRECTION_Up_CONST )
		Notch = 10 - Notch; // It's reversed if we're moving up

	if( Notch==0 )
	{
		m_pMouseHandler->m_iLastNotch=Notch;
		return;
	}

	if( Notch!=m_pMouseHandler->m_iLastNotch )
	{
LoggerWrapper::GetInstance()->Write(LV_FESTIVAL,"DatagridMouseHandlerHelper::ScrollGrid Notch %d",Notch);
	NeedToRender render( m_pMouseBehavior->m_pOrbiter, "scroll" );
		m_pMouseBehavior->m_pOrbiter->Renderer()->RenderObjectAsync(m_pObj_ScrollingGrid);
		// We know the notch can't go in reverse since that would stop the scrolling
		if( Notch<=5 )
		{
			if( dwPK_Direction==DIRECTION_Up_CONST )
				m_pMouseBehavior->m_pOrbiter->CMD_Scroll_Grid("","",DIRECTION_Up_CONST);
			else
				m_pMouseBehavior->m_pOrbiter->CMD_Scroll_Grid("","",DIRECTION_Down_CONST);

			m_pMouseHandler->m_iLastNotch=Notch;
			return;
		}

		int Frequency;
		//bool bPage=false;
		Frequency = (11-Notch)*100;
		m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_MediaTracks,dwPK_Direction==DIRECTION_Down_CONST ? 2 : -2,"",Frequency,NULL);
		m_pMouseHandler->m_iLastNotch=Notch;
	}
}


void DatagridMouseHandlerHelper::RelativeMove(int X, int Y)
{
	m_iLastX=X;
#ifdef DEBUG
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "DatagridMouseHandlerHelper::RelativeMove(%d, %d, %d) bottom %d top %d direction %s",
		X, Y, m_iLastNotch,m_Bottom,m_Top,m_eCapturingOffscreenMovement == cosm_UP ? "UP" : "DOWN");
#endif
	int Notch;
	if( m_eCapturingOffscreenMovement == cosm_UP )
	{
#ifdef WIN32
		if( Y >= m_Bottom-6 )
#else
		if( Y >= m_Bottom-1 )
#endif
		{
			ReleaseRelative();
			return;
		}
		Notch = (m_pMouseBehavior->m_pOrbiter->m_Height - Y) / (m_pMouseBehavior->m_pOrbiter->m_Height/5) +1;
		if( Notch==m_iLastNotch )
			return;

        RelativePointer_ImageLoad(-Notch);
        if (m_pRelativePointer_Image)
		{
			PlutoRectangle rectFakePointer(X-40,0,m_pRelativePointer_Image->m_nWidth, m_pRelativePointer_Image->m_nHeight);
            RelativePointer_ImageDraw(m_pRelativePointer_Image, rectFakePointer);
		}
	}
	else
	{
#ifdef WIN32
		if (Y <=m_Top+6 )  
#else
		if (Y <=m_Top+1 )  
#endif
		{
			ReleaseRelative();
			return;
		}
		Notch = Y / (m_pMouseBehavior->m_pOrbiter->m_Height/5) +1;
		if( Notch==m_iLastNotch )
			return;

        RelativePointer_ImageLoad(Notch);
		if (m_pRelativePointer_Image)
		{
			PlutoRectangle rectFakePointer(X-40,m_pMouseBehavior->m_pOrbiter->m_Height-m_pRelativePointer_Image->m_nHeight,m_pRelativePointer_Image->m_nWidth, m_pRelativePointer_Image->m_nHeight);
            RelativePointer_ImageDraw(m_pRelativePointer_Image, rectFakePointer);
		}
	}

	if( Notch<0 )
		Notch=0;
	if( Notch>5 )
		Notch=5;

	m_iLastNotch=Notch;

	int Frequency;
	Frequency = (6-Notch)*400;
LoggerWrapper::GetInstance()->Write(LV_ACTION, "Frequency: %d Y: %d notch %d",Frequency,Y,Notch);
	m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_MediaGrid,0,"",Frequency,this->m_pMouseHandler);
	#ifdef ORBITER_OPENGL
		if( m_pObj_ScrollingGrid )
			(dynamic_cast<DataGridRenderer_OpenGL *>(m_pObj_ScrollingGrid->Renderer()))->m_AnimationSpeed_set(int(Frequency*.5));
	#endif
}


bool DatagridMouseHandlerHelper::ReleaseRelative()
{
	if( m_eCapturingOffscreenMovement==cosm_NO )
		return false;

LoggerWrapper::GetInstance()->Write(LV_ACTION, "**stop**");
	m_iLastNotch=-1;
#ifdef DEBUG
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "DatagridMouseHandlerHelper::ReleaseRelative()");
#endif
	m_pMouseBehavior->m_pMouseIterator->SetIterator(MouseIterator::if_None,0,"",0,NULL); // In case we're scrolling a grid
    RelativePointer_Clear();
#ifdef WIN32
	m_pMouseBehavior->SetMousePosition(m_iLastX, m_eCapturingOffscreenMovement == cosm_DOWN ? m_Bottom-15 : m_Top+15);
#else
	m_pMouseBehavior->SetMousePosition(m_iLastX, m_eCapturingOffscreenMovement == cosm_DOWN ? m_Bottom-1 : m_Top+1);
#endif

	ScreenHistory *pCurrentScreen = m_pMouseBehavior->m_pOrbiter->m_pScreenHistory_Current;

	//don't release mouse if we are on media tracks screen
	if(NULL != pCurrentScreen && pCurrentScreen->PK_Screen() != SCREEN_Media_Tracks_CONST)
		m_pMouseBehavior->ConstrainMouse();

	m_pMouseBehavior->ShowMouse(true);
	m_eCapturingOffscreenMovement=cosm_NO;

	return true;
}

bool DatagridMouseHandlerHelper::Move(int X,int Y,int PK_Direction)
{
	if( X<m_iLeft || (m_iRight && X>m_iRight) )
		return false;

LoggerWrapper::GetInstance()->Write(LV_ACTION, "DatagridMouseHandlerHelper::Move Y %d m_Bottom %d m_eCapturingOffscreenMovement %d m_bHitBottom %d currow %d",
					  Y,m_Bottom,(int) m_eCapturingOffscreenMovement,(int) m_bHitBottom,m_pObj_ScrollingGrid->m_GridCurRow);
	if (m_eCapturingOffscreenMovement != cosm_NO)
	{
		RelativeMove(X,Y);
		return true;
	}
	
	// For testing under windows add extra padding of 6 pixels on the top and bottom since it's usually not a full screen app
#ifdef WIN32
	if ( (Y <= m_Top+6 && m_pObj_ScrollingGrid->m_GridCurRow>0) || (Y >= m_Bottom-6 && !m_bHitBottom) )
#else
	if ( (Y <= m_Top+1 && m_pObj_ScrollingGrid->m_GridCurRow>0) || (Y >= m_Bottom-1 && !m_bHitBottom) )
#endif
	{
LoggerWrapper::GetInstance()->Write(LV_ACTION, "DatagridMouseHandlerHelper::Move ****GO**** Y %d m_Bottom %d m_eCapturingOffscreenMovement %d m_bHitBottom %d currow %d",
					  Y,m_Bottom,(int) m_eCapturingOffscreenMovement,(int) m_bHitBottom,m_pObj_ScrollingGrid->m_GridCurRow);

#ifndef WIN32
		m_pMouseBehavior->ShowMouse(false,MouseBehavior::smb_TurnOnRemote);
#endif

#ifdef WIN32
		if( Y <= m_Top+6 ) 
#else
		if( Y <= m_Top+1 ) 
#endif
		{
			m_eCapturingOffscreenMovement = cosm_UP;
#ifdef WIN32
			m_pMouseBehavior->SetMousePosition(X,m_Bottom-15); 
#else
			m_pMouseBehavior->SetMousePosition(X,m_Bottom-1); 
#endif
			m_pMouseBehavior->m_pOrbiter->CMD_Remove_Popup("","coverart");

		}
		else
		{
			m_eCapturingOffscreenMovement = cosm_DOWN;
#ifdef WIN32
			m_pMouseBehavior->SetMousePosition(X,m_Top+15);
#else
			m_pMouseBehavior->SetMousePosition(X,m_Top+1);
#endif
			m_pMouseBehavior->m_pOrbiter->CMD_Remove_Popup("","coverart");
		}

		PlutoRectangle r(m_iLeft,m_Top,m_iRight-m_iLeft,m_Bottom-m_Top);

		if(r.Width != 0 && r.Height != 0)
			m_pMouseBehavior->ConstrainMouse(r);

		return true;
	}
	return false;
}

bool DatagridMouseHandlerHelper::DoIteration()
{
	if(NULL == m_pMouseBehavior)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"m_pMouseBehavior is NULL! Race condition!?");
		return true;
	}
	m_pMouseBehavior->m_pOrbiter->CMD_Remove_Popup("","coverart");
LoggerWrapper::GetInstance()->Write(LV_ACTION,"********SCROLL  --  START***");
	NeedToRender render( m_pMouseBehavior->m_pOrbiter, "iterator grid" );  // Redraw anything that was changed by this command
	bool bResult;

	if( m_eCapturingOffscreenMovement==cosm_UP )
	{
		bResult = m_pMouseBehavior->m_pOrbiter->Scroll_Grid("",m_pObj_ScrollingGrid->m_ObjectID,DIRECTION_Up_CONST);
		m_bHitBottom=false;
	}
	else
	{
		bResult = m_pMouseBehavior->m_pOrbiter->Scroll_Grid("",m_pObj_ScrollingGrid->m_ObjectID,DIRECTION_Down_CONST);
		if( !bResult )
			m_bHitBottom=true;
	}

	if( !bResult )
	{
		ReleaseRelative();
		return false;
	}
	// For the first notch, scroll only 1 time
	if( m_iLastNotch==1 )
		return false;

	return bResult;
}

bool DatagridMouseHandlerHelper::RelativePointer_SetStatus(int nSpeedShape)
{
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "DatagridMouseHandlerHelper::RelativePointer_SetStatus(%d)", nSpeedShape);
    nSpeedShape = RelativePointer_AdjustSpeedShape(nSpeedShape);
    // check range
    // compare with current status
    if (m_nRelativePointer_SpeedShape == nSpeedShape)
        return true;
    m_nRelativePointer_SpeedShape = nSpeedShape;

    // load image from disk
    RelativePointer_ImageLoad(nSpeedShape);
    
    // get current screen size
    PlutoSize oSizeScreen = m_pMouseBehavior->m_pOrbiter->m_sScreenSize;
    // get current mouse position
    PlutoPoint posFakeMouse;
    m_pMouseBehavior->GetMousePosition(&posFakeMouse);
    // pointer move to the middle of the screen, on Y axis only
    m_pMouseBehavior->SetMousePosition(posFakeMouse.X, oSizeScreen.Height/2);

    //delete this debug-only block
    // with a real pointer, not a fake one
    if (0)
    {
        string sPathMask = m_sImagePath + ".msk";
        m_pMouseBehavior->SetMouseCursorImage(m_sImagePath, sPathMask);
        if (nSpeedShape > 0)
            m_pMouseBehavior->SetMousePosition(posFakeMouse.X, posFakeMouse.Y - 2);
        else
            m_pMouseBehavior->SetMousePosition(posFakeMouse.X, posFakeMouse.Y + 2);
    }

    // draw fake pointer
	if( m_pRelativePointer_Image )
	{
	    PlutoRectangle rectFakePointer = RelativePointer_ComputeRectangle(posFakeMouse.X, posFakeMouse.Y, oSizeScreen.Width, oSizeScreen.Height);
        RelativePointer_ImageDraw(m_pRelativePointer_Image, rectFakePointer);
	}
	return true;
}

void DatagridMouseHandlerHelper::RelativePointer_Clear()
{
    m_nRelativePointer_SpeedShape = 0;
    RELEASE_GRAPHIC(m_pRelativePointer_Image);
    RelativePointer_ImageRemove();
}

int DatagridMouseHandlerHelper::RelativePointer_AdjustSpeedShape(int nSpeedShape)
{
    // check range
    const int nPeakSpeed = 5;
    if ( (nSpeedShape > 0) && (nSpeedShape > nPeakSpeed) )
    {
#ifdef DEBUG
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "DatagridMouseHandlerHelper::RelativePointer_AdjustSpeedShape(%d) : decreasing shape to %d", nSpeedShape, nPeakSpeed);
#endif
        nSpeedShape = nPeakSpeed;
    }
    if ( (nSpeedShape < 0) && (nSpeedShape < nPeakSpeed*-1) )
    {
#ifdef DEBUG
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "DatagridMouseHandlerHelper::RelativePointer_AdjustSpeedShape(%d) : increasing shape to %d", nSpeedShape, -nPeakSpeed);
#endif
        nSpeedShape = -nPeakSpeed;
    }
    return nSpeedShape;
}

bool DatagridMouseHandlerHelper::RelativePointer_ImageLoad(int nSpeedShape)
{
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "DatagridMouseHandlerHelper::RelativePointer_ImageLoad(%d)", nSpeedShape);
    nSpeedShape = RelativePointer_AdjustSpeedShape(nSpeedShape);

    // compare with current status
    if (m_nRelativePointer_SpeedShape == nSpeedShape)
        return true;
    m_nRelativePointer_SpeedShape = nSpeedShape;

    // restore the normal behavior in this case
    if (nSpeedShape == 0)
    {
        RelativePointer_Clear();
        return false;
    }
    // compute the image path
    char buffer[100];
    sprintf(buffer, "speed_shape_%d.png", nSpeedShape);
    m_sImagePath = "/usr/pluto/orbiter/skins/Basic/cursors/pointers/";
    m_sImagePath = m_sImagePath + buffer;

    // read the image from disk
	size_t iSize = 0;
    char * pData = FileUtils::ReadFileIntoBuffer(m_sImagePath.c_str(), iSize);
    if (pData == NULL)
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "DatagridMouseHandlerHelper::RelativePointer_ImageLoad(%d) : cannot load graphic file %s", nSpeedShape, m_sImagePath.c_str());
        return false;
    }
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "DatagridMouseHandlerHelper::RelativePointer_ImageLoad(%d) : loaded graphic file %s", nSpeedShape, m_sImagePath.c_str());

	RELEASE_GRAPHIC(m_pRelativePointer_Image);
	m_pRelativePointer_Image = m_pMouseBehavior->m_pOrbiter->m_pOrbiterRenderer->CreateGraphic();
	m_pRelativePointer_Image->m_Filename = "relative pointer image";
    m_pRelativePointer_Image->LoadGraphic(pData, iSize);
    delete [] pData;
    pData = NULL;
    return true;
}

void DatagridMouseHandlerHelper::RelativePointer_ImageDraw(PlutoGraphic *pImage, const PlutoRectangle &rectFakePointer)
{
	RelativePointer_ImageRemove();

#ifdef ORBITER_OPENGL
	LoggerWrapper::GetInstance()->Write(LV_ACTION,"DatagridMouseHandlerHelper::RelativePointer_ImageDraw");
    OrbiterRenderer_OpenGL *pOrbiterRenderer_OpenGL = dynamic_cast<OrbiterRenderer_OpenGL *>(m_pMouseBehavior->m_pOrbiter->Renderer());
    if (pOrbiterRenderer_OpenGL)
    {
        // opengl mode
        // reusing the same image id
		pOrbiterRenderer_OpenGL->DestroyGraphic(IMAGE_FAKE_POINTER_NAME);
        pOrbiterRenderer_OpenGL->RenderGraphic(pImage, rectFakePointer, false, PlutoPoint(), 255,
			"", IMAGE_FAKE_POINTER_NAME);
		pOrbiterRenderer_OpenGL->Engine->AddTopMostObject(IMAGE_FAKE_POINTER_NAME);
        return;
    }
#endif
    // sdl mode, not tested
    m_pMouseBehavior->m_pOrbiter->Renderer()->RenderGraphic(pImage, rectFakePointer);
}

void DatagridMouseHandlerHelper::RelativePointer_ImageRemove()
{
	m_pMouseBehavior->m_pOrbiter->Renderer()->DestroyGraphic(IMAGE_FAKE_POINTER_NAME);
}

PlutoRectangle DatagridMouseHandlerHelper::RelativePointer_ComputeRectangle(int X, int Y, int screenWidth, int screenHeight)
{
    PlutoRectangle rectFakePointer;
    if (m_pRelativePointer_Image == NULL)
        return rectFakePointer;
    //compute fake image position
    rectFakePointer.Width = m_pRelativePointer_Image->m_nWidth;
    rectFakePointer.Height = m_pRelativePointer_Image->m_nHeight;
    // X coordinate : keep the pointer on the screen
    rectFakePointer.X = X;
    if (rectFakePointer.X + rectFakePointer.Width > screenWidth)
        rectFakePointer.X = screenWidth - rectFakePointer.Width;
    // Y coordinate
    if (m_nRelativePointer_SpeedShape > 0)
        rectFakePointer.Y = screenHeight - rectFakePointer.Height;
    else
        rectFakePointer.Y = 0;
    return rectFakePointer;
}
