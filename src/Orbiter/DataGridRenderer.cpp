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
#include "DataGridRenderer.h"
#include "../DCE/Logger.h"
#include "Orbiter.h"
#include "OrbiterRenderer.h"
#include "DesignObj_DataGrid.h"
#include "../DCE/DataGrid.h"
#include "../Gen_Devices/AllCommandsRequests.h"
#include "pluto_main/Define_DataGrid.h"
#include "pluto_main/Define_Variable.h"
#include "pluto_main/Define_FloorplanType.h"
#include "pluto_main/Define_HorizAlignment.h"
#include "pluto_main/Define_VertAlignment.h"
#include "pluto_main/Define_DesignObj.h"

using namespace DCE;

DataGridRenderer::DataGridRenderer(DesignObj_Orbiter *pOwner): ObjectRenderer(pOwner)
{
	m_pObj_Owner_DataGrid = dynamic_cast<DesignObj_DataGrid *>(pOwner);
	StartAnimation = 0;
	iPK_Direction = 0;
}

/*virtual*/ DataGridRenderer::~DataGridRenderer(void)
{
}

/*virtual*/ void DataGridRenderer::RenderObject(DesignObj_Orbiter *pObj_Screen, PlutoPoint point)
{
	if(NULL == m_pObj_Owner_DataGrid)
		return;

	if(!PreRenderActions(pObj_Screen, point))
		return;

	PLUTO_SAFETY_LOCK( dg, m_pObj_Owner_DataGrid->m_pOrbiter->m_DatagridMutex );
	string delSelections;
	string sSelectedIndex = "0";
	if(!m_pObj_Owner_DataGrid->sSelVariable.empty())
	{
		PLUTO_SAFETY_LOCK( vm, m_pObj_Owner_DataGrid->m_pOrbiter->m_VariableMutex )
			delSelections = "|" + m_pObj_Owner_DataGrid->m_pOrbiter->m_mapVariable[atoi(m_pObj_Owner_DataGrid->sSelVariable.c_str())] + "|";
		sSelectedIndex = m_pObj_Owner_DataGrid->m_pOrbiter->m_mapVariable[atoi(m_pObj_Owner_DataGrid->sSelVariable.c_str())];
		vm.Release();
	}

#if ( defined( PROFILING_GRID ) )
	clock_t clkStart = clock(  );
#endif

	m_pObj_Owner_DataGrid->PrepareRenderDataGrid(delSelections);

#if ( defined( PROFILING_GRID ) )
	clock_t clkAcquired = clock(  );
#endif

	DataGridTable *pDataGridTable = m_pObj_Owner_DataGrid->DataGridTable_Get();
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"RenderDataGrid %s %p",m_pObj_Owner_DataGrid->m_ObjectID.c_str(),pDataGridTable);
#endif
	if( !pDataGridTable )
		return;

	bool bContainsCells = m_pObj_Owner_DataGrid->m_mapChildDgObjects.size()>1; // Store this as a bool so for efficiency we're not doing a find in a map when the map is empty
	if( bContainsCells )
	{
		for(map< pair<int,int>, DesignObj_Orbiter *>::iterator it=m_pObj_Owner_DataGrid->m_mapChildDgObjects.begin();it!=m_pObj_Owner_DataGrid->m_mapChildDgObjects.end();++it)
		{
//DesignObj_Orbiter *pObj = it->second;
			it->second->m_bHidden=true;  // Set them all to hidden.  We will unhide them if there is data for this cell
		}
	}

	int nAlphaChannel = GetAlphaLevel();

	m_pObj_Owner_DataGrid->m_pOrbiter->Renderer()->SolidRectangle( point.X + m_pObj_Owner_DataGrid->m_rPosition.X, 
		point.Y + m_pObj_Owner_DataGrid->m_rPosition.Y, m_pObj_Owner_DataGrid->m_rPosition.Width, 
		m_pObj_Owner_DataGrid->m_rPosition.Height, PlutoColor(0, 0, 0, nAlphaChannel));

	int i,  j; //indexes

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "DataGridRenderer::RenderObject ===============================================");

	for(map< pair<int,int>, DesignObj_Orbiter *>::iterator itc = m_pObj_Owner_DataGrid->m_mapChildDgObjects.begin();
		itc != m_pObj_Owner_DataGrid->m_mapChildDgObjects.end(); ++itc)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "DataGridRenderer::RenderObject --- Cell <%d, %d>",
			itc->first.first, itc->first.second);
	}

	for ( i = 0; i < pDataGridTable->m_RowCount; i++ )
	{
		for ( j = 0; j < pDataGridTable->m_ColumnCount; j++ )
		{
			int DGRow = ( ( i == 0 && pDataGridTable->m_bKeepRowHeader ) ? 0 : i + pDataGridTable->m_StartingRow );
			int DGColumn = ( j == 0 && pDataGridTable->m_bKeepColumnHeader ) ? 0 : j + pDataGridTable->m_StartingColumn;

			DataGridCell * pCell = pDataGridTable->GetData( DGColumn,  DGRow );

			if ( pCell )
			{
				if( bContainsCells && DGRow!=pDataGridTable->m_iUpRow && DGRow!=pDataGridTable->m_iDownRow)
				{
					map< pair<int,int>, DesignObj_Orbiter *>::iterator it = 
						m_pObj_Owner_DataGrid->m_mapChildDgObjects.find(make_pair<int,int> (j, i));

					if(	it!=m_pObj_Owner_DataGrid->m_mapChildDgObjects.end())
					{
						it->second->m_bHidden=false;

						LoggerWrapper::GetInstance()->Write(LV_WARNING, "DataGridRenderer::RenderObject Need to custom render cell <%d,%d>=%s, cell count %d", 
							j, i, pCell->GetText(), pDataGridTable->m_CellCount);

						continue; // No need to render it here; the object will self-render
					}
				}

				LoggerWrapper::GetInstance()->Write(LV_WARNING, "DataGridRenderer::RenderObject Need to render cell <%d,%d>=%s, cell count %d", 
					j, i, pCell->GetText(), pDataGridTable->m_CellCount);

				if ( !m_pObj_Owner_DataGrid->m_bDontShowSelection )
				{
					int GraphicType = GRAPHIC_NORMAL;
					if(  delSelections.length(  ) > 2 && delSelections.find( "|"+string( pCell->GetValue(  ) )+"|" )!=string::npos  )
					{
						GraphicType = GRAPHIC_SELECTED;
						if( m_pObj_Owner_DataGrid->m_bHighlightSelectedCell )
						{
							m_pObj_Owner_DataGrid->m_iHighlightedRow = m_pObj_Owner_DataGrid->m_sExtraInfo.find( 'C' )==string::npos ? DGRow : -1;
							m_pObj_Owner_DataGrid->m_iHighlightedColumn = m_pObj_Owner_DataGrid->m_sExtraInfo.find( 'R' )==string::npos ? DGColumn : -1;
							m_pObj_Owner_DataGrid->m_bHighlightSelectedCell=false; // Only on the initial draw
						}
					}

					RenderCell(pDataGridTable,  pCell,  j,  i,  GraphicType, point );
				}
				else
					RenderCell(pDataGridTable,  pCell,  j,  i,  GRAPHIC_NORMAL, point );

				if( DGRow==pDataGridTable->m_iUpRow || DGRow==pDataGridTable->m_iDownRow )
					break;  // Only the first column

				pCell = NULL;
			}
		}
	}

#if ( defined( PROFILING_GRID ) )
	clock_t clkFinished = clock(  );

	LoggerWrapper::GetInstance()->Write( LV_CONTROLLER, "Grid: %s took %d ms to acquire and %d ms to render",
		m_pObj_Owner_DataGrid->m_sGridID.c_str(), int(clkAcquired-clkStart), int(clkFinished-clkAcquired));
#endif

	PostRenderActions(pObj_Screen, point);
}

/*virtual*/ void DataGridRenderer::GetGridCellDimensions(int Colspan,  int Rowspan,  int Column,  int Row,  int &x,  int &y,  int &w,  int &h )
{
	if(NULL == m_pObj_Owner_DataGrid)
		return;

	if ( Colspan < 1 )
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING,  "Data grid cell has column span of zero!" );
		x=y=w=h=0;
		return;
	}

	map< pair<int,int>, DesignObj_Orbiter *>::iterator it=m_pObj_Owner_DataGrid->m_mapChildDgObjects.find( make_pair<int,int> (Column,Row) );
	if(	it!=m_pObj_Owner_DataGrid->m_mapChildDgObjects.end() )
	{
		DesignObj_Orbiter *pObj = it->second;
		x = pObj->m_rPosition.X;
		y = pObj->m_rPosition.Y;
		w = pObj->m_rPosition.Width;
		h = pObj->m_rPosition.Height;
		return;
	}

	// These deltas make adjustments to the rest of the coodrinates when there's a first column
	// on the grid that's a different size from the rest.

	int DeltaX = 0,  DeltaY = 0;

	// Is the first row on the screen?
	if ( Row > 0 && m_pObj_Owner_DataGrid->m_FirstRowHeight > 0 && ( m_pObj_Owner_DataGrid->m_GridCurRow == 0 || m_pObj_Owner_DataGrid->m_bKeepRowHeader )  )
	{
		DeltaY = m_pObj_Owner_DataGrid->m_FirstRowHeight - m_pObj_Owner_DataGrid->m_FixedRowHeight;
	}
	// Is the first column on the screen?
	if ( Column > 0 && m_pObj_Owner_DataGrid->m_FirstColumnWidth > 0 && ( m_pObj_Owner_DataGrid->m_GridCurCol == 0 || m_pObj_Owner_DataGrid->m_bKeepColHeader )  )
	{
		DeltaX = m_pObj_Owner_DataGrid->m_FirstColumnWidth - m_pObj_Owner_DataGrid->m_FixedColumnWidth;
	}

	if( m_pObj_Owner_DataGrid->m_pOrbiter->m_iRotation==90 )
	{
		x = m_pObj_Owner_DataGrid->m_rPosition.Right()-( Row*( m_pObj_Owner_DataGrid->m_FixedRowHeight+1 ) ) + DeltaX - m_pObj_Owner_DataGrid->m_FixedRowHeight;
		y = m_pObj_Owner_DataGrid->m_rPosition.Y+( Column *( m_pObj_Owner_DataGrid->m_FixedColumnWidth+1 ) ) + DeltaY;
	}
	else if( m_pObj_Owner_DataGrid->m_pOrbiter->m_iRotation==180 )
	{
		x = m_pObj_Owner_DataGrid->m_rPosition.Right()-( (Column+Colspan-1)*( m_pObj_Owner_DataGrid->m_FixedColumnWidth+1 ) ) + DeltaX - m_pObj_Owner_DataGrid->m_FixedColumnWidth;
		y = m_pObj_Owner_DataGrid->m_rPosition.Bottom()-( (Row+Rowspan-1) *( m_pObj_Owner_DataGrid->m_FixedRowHeight+1 ) ) + DeltaY - m_pObj_Owner_DataGrid->m_FixedRowHeight;
	}
	else if( m_pObj_Owner_DataGrid->m_pOrbiter->m_iRotation==270 )
	{
		x = m_pObj_Owner_DataGrid->m_rPosition.X+( Row*( m_pObj_Owner_DataGrid->m_FixedRowHeight+1 ) ) + DeltaX;
		y = m_pObj_Owner_DataGrid->m_rPosition.Bottom()-( (Column+Colspan-1) *( m_pObj_Owner_DataGrid->m_FixedColumnWidth+1 ) ) + DeltaY - m_pObj_Owner_DataGrid->m_FixedColumnWidth;
	}
	else
	{
		x = m_pObj_Owner_DataGrid->m_rPosition.X+( Column*( m_pObj_Owner_DataGrid->m_FixedColumnWidth+1 ) ) + DeltaX;
		y = m_pObj_Owner_DataGrid->m_rPosition.Y+( Row *( m_pObj_Owner_DataGrid->m_FixedRowHeight+1 ) ) + DeltaY;
	}

	if ( Column == 0 && m_pObj_Owner_DataGrid->m_FirstColumnWidth > 0 && ( m_pObj_Owner_DataGrid->m_GridCurCol == 0 || m_pObj_Owner_DataGrid->m_bKeepColHeader ))
	{
		w = m_pObj_Owner_DataGrid->m_FirstColumnWidth + (Colspan-1) * m_pObj_Owner_DataGrid->m_FixedColumnWidth + ( Colspan-1 );
	}
	else
	{
		w = m_pObj_Owner_DataGrid->m_FixedColumnWidth * Colspan + ( Colspan-1 );
	}
	if ( Row == 0 && m_pObj_Owner_DataGrid->m_FirstRowHeight > 0  && ( m_pObj_Owner_DataGrid->m_GridCurRow == 0 || m_pObj_Owner_DataGrid->m_bKeepRowHeader ) )
	{
		h = m_pObj_Owner_DataGrid->m_FirstRowHeight + (Rowspan-1) * m_pObj_Owner_DataGrid->m_FixedRowHeight + ( Rowspan-1 );
	}
	else
	{
		h = m_pObj_Owner_DataGrid->m_FixedRowHeight * Rowspan+ ( Rowspan-1 );
	}

	if( m_pObj_Owner_DataGrid->m_pOrbiter->m_iRotation==0 )
	{
		if ( x+w > m_pObj_Owner_DataGrid->m_rPosition.X + m_pObj_Owner_DataGrid->m_rPosition.Width )
			// Oops,  width is greater than the size of the object.  Truncate.
			w = ( m_pObj_Owner_DataGrid->m_rPosition.X+m_pObj_Owner_DataGrid->m_rPosition.Width ) - x;
		if ( y+h > m_pObj_Owner_DataGrid->m_rPosition.Y + m_pObj_Owner_DataGrid->m_rPosition.Height )
			// Oops,  height is greater than the size of the object.  Truncate.
			h = ( m_pObj_Owner_DataGrid->m_rPosition.Y+m_pObj_Owner_DataGrid->m_rPosition.Height ) - y;
	}

	if ( w<0 || h<0 )
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING,  "Data grid cell has negative size values."  );
		x=y=w=h=0;
		return;
	}

	if( m_pObj_Owner_DataGrid->m_pOrbiter->m_iRotation==90 || m_pObj_Owner_DataGrid->m_pOrbiter->m_iRotation==270 )
	{
		int ww = w;
		w=h;
		h=ww;
	}
}

/*virtual*/ bool DataGridRenderer::RenderCell(DataGridTable *pT, DataGridCell *pCell, int j, int i, int iGraphicToDisplay, PlutoPoint point)
{
	if(NULL == m_pObj_Owner_DataGrid)
		return false;

	int nAlphaChannel = GetAlphaLevel();

	TextStyle *pTextStyle = m_pObj_Owner_DataGrid->m_pTextStyle;
	bool bTransparentCell = false; // todo,  is transparency in PlutoColor? ( strchr( GetParameterValue( C_PARAMETER_CELL_COLOR_CONST ).c_str(  ),  ', ' )==NULL );

	if ( j==0 && pT->m_bKeepColumnHeader )
	{
		pTextStyle = m_pObj_Owner_DataGrid->m_pTextStyle_FirstCol;
		bTransparentCell = false;
	}
	if ( i==0 && pT->m_bKeepRowHeader )
	{
		pTextStyle = m_pObj_Owner_DataGrid->m_pTextStyle_FirstRow;
		bTransparentCell = false;
	}
	if ( pCell->m_AltColor > 0 && pCell->m_AltColor<( int ) m_pObj_Owner_DataGrid->m_vectTextStyle_Alt.size(  ) )
	{
		pTextStyle = m_pObj_Owner_DataGrid->m_vectTextStyle_Alt[pCell->m_AltColor];
		bTransparentCell = false;
	}
	if( iGraphicToDisplay==GRAPHIC_SELECTED )
	{
		pTextStyle = m_pObj_Owner_DataGrid->m_pTextStyle_Selected;
		bTransparentCell = false;
	}
	else if ( iGraphicToDisplay==GRAPHIC_HIGHLIGHTED )
	{
		pTextStyle = m_pObj_Owner_DataGrid->m_pTextStyle_Highlighted;
		bTransparentCell = false;
	}

	if(  !pTextStyle  )
		pTextStyle = m_pObj_Owner_DataGrid->m_pOrbiter->m_mapTextStyle_Find( 0 );
	if(  !pTextStyle  )
		pTextStyle = m_pObj_Owner_DataGrid->m_pOrbiter->m_mapTextStyle_Find( 1 );

	if(  !pTextStyle  )
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Trying to render cell,  but cannot find any style" );
		return false;
	}
	int x, y, w, h;
	GetGridCellDimensions(pCell->m_Colspan,  pCell->m_Rowspan,  j,  i,  x,  y,  w,  h );
	if ( w>4 && h >4 )
	{
		PlutoColor CellColor = PlutoColor::Gray();

		PLUTO_SAFETY_LOCK(M,m_pObj_Owner_DataGrid->m_pOrbiter->m_VariableMutex);

		const char *pPath = pCell->GetImagePath();
		if (
			pPath && !pCell->m_pGraphicData && !pCell->m_pGraphic && 
			(m_pObj_Owner->m_pOrbiter->m_sSkin == AUDIO_STATION_SKIN || !m_pObj_Owner->m_pOrbiter->m_bLoadDatagridImagesInBackground)
		)
		{
			size_t s=0;
			pCell->m_pGraphicData = m_pObj_Owner_DataGrid->m_pOrbiter->ReadFileIntoBuffer(pPath,s);
			pCell->m_GraphicLength = (unsigned long) s;
			pCell->m_GraphicFormat = GR_JPG;
		} 
		if ( pCell->m_pGraphicData && !pCell->m_pGraphic )
		{
			pCell->m_pGraphic = m_pObj_Owner_DataGrid->m_pOrbiter->Renderer()->CreateGraphic();
			pCell->m_pGraphic->m_Filename = "datagrid cell";
			pCell->m_pGraphic->m_GraphicFormat = pCell->m_GraphicFormat;
			pCell->m_pGraphic->LoadGraphic(pCell->m_pGraphicData,  pCell->m_GraphicLength, m_pObj_Owner_DataGrid->m_pOrbiter->m_iRotation);
			// Todo, since we're leaving the graphic in native format from this point foward,
			// we can probably delete the compressed-format image here.
		}

		if(!bTransparentCell)
		{
			CellColor = pCell->m_AltColor ? pCell->m_AltColor : pTextStyle->m_BackColor;
			CellColor.SetAlpha(nAlphaChannel);
			m_pObj_Owner_DataGrid->m_pOrbiter->Renderer()->SolidRectangle( point.X + x,  point.Y + y,  w,  h,  CellColor);
		}

		if (pCell->m_pGraphic)
		{
			PlutoRectangle rect(x,  y,  w,  h);

			if(m_pObj_Owner->m_pOrbiter->m_sSkin == AUDIO_STATION_SKIN)
			{
				//center graphic

				double zoom = min(rect.Width / double(pCell->m_pGraphic->m_nWidth),
					rect.Height / double(pCell->m_pGraphic->m_nHeight));

				int new_width = static_cast<int>(pCell->m_pGraphic->m_nWidth * zoom);
				int new_height = static_cast<int>(pCell->m_pGraphic->m_nHeight * zoom);

				rect.X += (rect.Width - new_width) / 2;
				rect.Y += (rect.Height - new_height) / 2;
				rect.Width = new_width;
				rect.Height = new_height;
			}

			if(m_pObj_Owner->m_pOrbiter->m_sSkin == AUDIO_STATION_SKIN && string(pCell->GetValue()).find("!A") != 0)
			{
				//don't display coverart for nuforce's non-albums
				delete pCell->m_pGraphicData;
				pCell->m_pGraphicData = NULL;
				pCell->m_ImagePath = NULL;
				pPath = NULL;
			}
			else
			{
				string sCellObjectID, sCellObjectHash;
				sCellObjectID = sCellObjectHash = DATAGRID_THUMB_NAME " - " + StringUtils::ltos(x) + "-" + StringUtils::ltos(y) + 
					(NULL == pCell->m_ImagePath ? "" : string("-pic ") + pCell->m_ImagePath);
				m_pObj_Owner_DataGrid->m_pOrbiter->Renderer()->RenderGraphic(pCell->m_pGraphic, 
					rect, 
					m_pObj_Owner_DataGrid->m_bDisableAspectLock, 
					point,
					255, 
					"datagrid " + m_pObj_Owner_DataGrid->GenerateObjectHash(m_pObj_Owner_DataGrid->m_pPopupPoint, false), //ParentID
					sCellObjectID, //Object ID
					sCellObjectHash  //Object hash
				);
			}
		}

#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Rendering cell with %s",pCell->GetText());        
#endif
		DesignObjText Text(m_pObj_Owner_DataGrid);
		// todo         Text.m_Rect = PlutoRectangle( x+pObj->BorderWidth,  y+pObj->BorderWidth,  w-( 2*pObj->BorderWidth ),  h-( 2*pObj->BorderWidth ) );
		Text.m_rPosition = PlutoRectangle( x,  y,  w,  h );
		//pTextStyle->m_BackColor = PlutoColor( 0, 0, 0, 255 );
		//pTextStyle->m_ForeColor = PlutoColor( 255, 255, 255, 255 );
		//pTextStyle->m_iPK_VertAlignment=2;

		Text.m_iPK_HorizAlignment = pTextStyle->m_iPK_HorizAlignment;
		Text.m_iPK_VertAlignment = pTextStyle->m_iPK_VertAlignment;

		if(m_pObj_Owner->m_pOrbiter->m_sSkin == AUDIO_STATION_SKIN && m_pObj_Owner->m_iBaseObjectID == DESIGNOBJ_dgFileList2_Pics_CONST)
		{
			Text.m_iPK_HorizAlignment = HORIZALIGNMENT_Center_CONST;
			Text.m_iPK_VertAlignment = VERTALIGNMENT_Middle_CONST;
		}

		string sText = m_pObj_Owner_DataGrid->m_pOrbiter->SubstituteVariables(pCell->GetText(), m_pObj_Owner_DataGrid, 0, 0);

		//don't display any text on a cell if it has already a pictures to display
		if(NULL != pCell->m_pGraphicData || NULL != pPath)
			sText = "";

		if(!sText.empty())
		{
			PlutoColor TextColor = pTextStyle->m_ForeColor;
			if(CellColor.R() == TextColor.R() && CellColor.G() == TextColor.G() && CellColor.B() == TextColor.B())
			{
				pTextStyle->m_ForeColor.R( pTextStyle->m_ForeColor.R() ^ 0xff );
				pTextStyle->m_ForeColor.G( pTextStyle->m_ForeColor.G() ^ 0xff );
				pTextStyle->m_ForeColor.B( pTextStyle->m_ForeColor.B() ^ 0xff );
				LoggerWrapper::GetInstance()->Write( LV_WARNING,  "Datagrid foreground is same as background.  Using xor" );
			}

			PlutoColor BackColor = pTextStyle->m_BackColor;
			pTextStyle->m_BackColor = CellColor;
			m_pObj_Owner_DataGrid->m_pOrbiter->Renderer()->RenderText(sText, &Text, pTextStyle, point);
			pTextStyle->m_BackColor = BackColor;
			pTextStyle->m_ForeColor = TextColor;
		}
	}
	else
		LoggerWrapper::GetInstance()->Write( LV_WARNING,  "Datagrid width or height is too small" );


	return bTransparentCell;
}


/*
void *StartDataGridCacheThread(void *p)
{
	DataGridRenderer *dgr = (DataGridRenderer *)p;

	dgr->DataGridCacheThread();
	return NULL;
}

void DataGridRenderer::StartCacheThread()
{
	StopCacheThread();
	m_bStopCaching=false;
	pthread_create(&m_DataGridCacheThread, NULL, StartDataGridCacheThread, (void*)this);
}

void DataGridRenderer::StopCacheThread()
{
	if (m_DataGridCacheThread)
	{
		m_bStopCaching = true;
		pthread_join(m_DataGridCacheThread, NULL);
	}
}

void DataGridRenderer::DataGridCacheThread()
{
	int Direction, Step, MaxSteps = max(m_pObj_Owner_DataGrid->m_iCacheRows,m_pObj_Owner_DataGrid->m_iCacheColumns);
	int DirectionCur[5];
	for(Direction=1;Direction<=4;Direction++)
		DirectionCur[Direction]=(Direction <= 2) ? m_pObj_Owner_DataGrid->m_GridCurRow : m_pObj_Owner_DataGrid->m_GridCurCol;

	for(Step=1;Step<=MaxSteps && !m_bStopCaching;Step++)
	{
		for(Direction=1;Direction<=4 && !m_bStopCaching;Direction++)
		{
			if (DirectionCur[Direction]==-1)
				continue;

			int MaxCache = (Direction <= 2) ? m_pObj_Owner_DataGrid->m_iCacheRows : m_pObj_Owner_DataGrid->m_iCacheColumns;
			if (Step > MaxCache)
			{
				DirectionCur[Direction] = -1;
				continue;
			}

			PLUTO_SAFETY_LOCK( dgc, m_DataGridCacheMutex );

			int CurSize = (int)m_listDataGridCache[Direction].size();
			
			if (CurSize > MaxCache)
			{
				// There's more cache than the specified amount (probably as a result of moving). Delete the back.
				delete m_listDataGridCache[Direction].back();
				m_listDataGridCache[Direction].pop_back();
			}

			if (CurSize > Step) 
			{
				// This step is already cached.
				continue;
			}

			// Starting at the origin, we need to "walk" Step number of steps to request the appropriate grid.
			dgc.Release();

			// CalculateGridMovement is going to try to lock the m_DatagridMutex.  But it's possible a rendering thread
			// is locking it and will try to kill us.  So we're doing a peek to see if the thread is locked manually 
			// using trylock, and if it is, will keep checking if we're supposed to exit.  If it's not,
			// we need to manually unluck it below
			bool bKeepTryingToLock=true;
			while( bKeepTryingToLock )
			{
				int iResult = pthread_mutex_trylock(&m_pObj_Owner_DataGrid->m_pOrbiter->m_DatagridMutex.mutex);
				if( iResult!=0 )
				{
					if( m_bStopCaching )
					{
						m_DataGridCacheThread = NULL;
						return;
					}
					else
						Sleep(50);  // Something else is rendering or something using grids, give it a moment
				}
				else
					bKeepTryingToLock=false;
			}

			if (!CalculateGridMovement( Direction, DirectionCur[Direction],  0 ))
			{				
				pthread_mutex_unlock(&m_pObj_Owner_DataGrid->m_pOrbiter->m_DatagridMutex.mutex);  // See try lock above
				DirectionCur[Direction] = -1;
				continue;
			}
			pthread_mutex_unlock(&m_pObj_Owner_DataGrid->m_pOrbiter->m_DatagridMutex.mutex);  // See try lock above
			dgc.Relock();

			int GridCurRow = (Direction <= 2) ? DirectionCur[Direction] : m_pObj_Owner_DataGrid->m_GridCurRow;
			int GridCurCol = (Direction <= 2) ? m_pObj_Owner_DataGrid->m_GridCurCol : DirectionCur[Direction];
			int size = 0;
			char *data = NULL;

			dgc.Release();

			DCE::CMD_Request_Datagrid_Contents CMD_Request_Datagrid_Contents( m_pObj_Owner_DataGrid->m_pOrbiter->m_dwPK_Device,  m_pObj_Owner_DataGrid->m_pOrbiter->m_dwPK_Device_DatagridPlugIn,
			StringUtils::itos( m_pObj_Owner_DataGrid->m_pOrbiter->m_dwIDataGridRequestCounter ), m_pObj_Owner_DataGrid->m_sGridID,
			m_pObj_Owner_DataGrid->m_MaxRow, m_pObj_Owner_DataGrid->m_MaxCol, m_pObj_Owner_DataGrid->m_bKeepRowHeader, m_pObj_Owner_DataGrid->m_bKeepColHeader, true, m_pObj_Owner_DataGrid->m_sSeek, m_pObj_Owner_DataGrid->m_iSeekColumn, &data, &size, &GridCurRow, &GridCurCol );
		
			if(  !m_pObj_Owner_DataGrid->m_pOrbiter->SendCommand( CMD_Request_Datagrid_Contents )  )
			{
				LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Request datagrid: %s failed", m_pObj_Owner_DataGrid->m_ObjectID.c_str(  ) );
				DirectionCur[Direction] = -1;
			}
			else
			{
				if ( size && data )
				{
					dgc.Relock();
					DataGridTable *pDataGridTable = new DataGridTable( size,  data );
					m_listDataGridCache[Direction].push_back(pDataGridTable);
					delete[] data;
					data = NULL;
					PLUTO_SAFETY_LOCK(M,m_pObj_Owner_DataGrid->m_pOrbiter->m_VariableMutex);
					for(MemoryDataTable::iterator it=pDataGridTable->m_MemoryDataTable.begin();it!=pDataGridTable->m_MemoryDataTable.end();++it)
					{
						DataGridCell *pDataGridCell = it->second;
						const char *pPath = pDataGridCell->GetImagePath();
						if (pPath && !pDataGridCell->m_pGraphicData && !pDataGridCell->m_pGraphic)
 						{
							M.Release();
							m_pObj_Owner_DataGrid->m_pOrbiter->Renderer()->BackgroundImageLoad(pPath, &pDataGridCell->m_pGraphic);              
							M.Relock();
						}
					}
				}
			}
			Sleep(0);
		}
	}
	m_DataGridCacheThread = NULL;
}

*/
