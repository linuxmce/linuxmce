/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU Lesser General Public License for more details.

*/

/** @file DataGrid.h
 DataGrid classes.
 */
#ifndef DATAGRID2_H
#define DATAGRID2_H

#include "../Orbiter/PlutoGraphic.h"
#include "PlutoUtils/GraphicFormat.h"
#include "Message.h"

// REQUEST DATA

#ifdef new
#undef new
#endif

#ifdef delete
#undef delete
#endif

#define INCREMENT_ROW_COLUMN iColumn++; if( iColumn>iMaxColumns ) { iColumn=0; iRow++; }

namespace DCE
{

/**
@class DataGridCellSerializableData
This class is ???
*/
	class DataGridCellSerializableData
	{
	public:
		unsigned int m_GraphicLength, m_MessageLength, m_TextLength, m_ValueLength, m_ImagePathLength, m_NumAttributes, m_AttributesLength;
		int m_AltColor;
		enum eGraphicFormat m_GraphicFormat;
		bool m_bSelectable;
		int m_PK_StyleDetail;
		int m_Colspan, m_Rowspan;
		bool m_bDontFireGridEvent; // usefull for the cells with scroll arrows
	};

/**
@class DataGridCell
This class is ???
*/
	class DataGridCell : public DataGridCellSerializableData
	{
	public:
		Message *m_pMessage;
		PlutoGraphic *m_pGraphic;
		char *m_pGraphicData, *m_Text, *m_Value, *m_ImagePath;
		map<string,string> m_mapAttributes;
		string m_mapAttributes_Find(string Attribute) { map<string,string>::iterator it = m_mapAttributes.find(Attribute); return it==m_mapAttributes.end() ? "" : (*it).second; }

		// Unique value REQUIRED for multi-select lists.
		DataGridCell(string Text, string Value="");
		DataGridCell(int Size, char *Data);
		DataGridCell(DataGridCell *pDataGridCell);
		virtual ~DataGridCell();
		void Initialize();

		// Will display this image in the cell
		void SetImage(char *Data, int Length, enum eGraphicFormat Format);
		void SetImagePath(const char *ImagePath);
		void SetValue(string Value);
		void SetText(string Text);

		// Will fire off this message when touched.
		void SetMessage(Message *pMessage);
		const char *GetText();
		const char *GetValue();
		const char *GetImagePath() { return m_ImagePath; };

		virtual void ToData(unsigned long &Size, char* &Data);
	};

	// We're currently supporting many more columns than rows, because the EPG grid with column span may need quite a bit of granularity, but
	// not much vertically.  We may opt to make this a 64 bit integer, but in the interest of speed we're trying it with 32.

	#define MAKECOLROW(col, row) (((col)  << 14) | ((row) & 0x3FFF))

	typedef unsigned int ColRowType;
	typedef map<ColRowType, DataGridCell *> MemoryDataTable;

/**
@class DataGridTableCellIndex
This class is ???
*/
	class DataGridTableCellIndex
	{
	public:
		ColRowType m_ColRow;
		unsigned int m_Size;
	};

/**
@class DataGridTableSerializableData
This class is ???
*/
	class DataGridTableSerializableData
	{
	protected:
		int m_TotalColumns, m_TotalRows;
	public:
		// Starting = The first row/column relative to the full data in this chunk.
		// Count = The number of rows/columns in this snapshot of the data
		// Total = The number of rows/columns available in the full data.

		bool m_bKeepRowHeader, m_bKeepColumnHeader;
		int m_ColumnCount, m_RowCount;
		int m_StartingColumn, m_StartingRow;
		int m_CellCount;

		const int getTotalRowCount() const { return m_TotalRows; };
	};

/**
@class DataGridTable
This class is ???
*/
	class DataGridTable : public DataGridTableSerializableData
	{
	public:
		// These are only used by a memory grid.
		MemoryDataTable m_MemoryDataTable;

		// These aren't serialized.  Just used internally by the plugin
		int m_iPK_Datagrid;
		string m_sOptions;
		bool m_bRePopulateEachTimeRequested;

		// Used only by Orbiter, these are the rows which have up/down arrows.  If up==-1, there is no up arrow, same for down.  Otherwise it's 0 based
		// Orbiter may shift data around to make room for up/down arrows
		int m_iDownRow,m_iUpRow;

		int m_iRequestID; // The rendering device can keep track of which data grid request id was used to create this grid

		DataGridTable();
		DataGridTable(int Size, char *Data,bool bShiftDown=false);  // If Shift down = true, while deserializing we will shift the cells down by 1
		virtual ~DataGridTable();

		virtual int GetRows();  // The total number of rows in the whole grid
		virtual int GetCols();  // The total number of columns in the whole grid
		virtual DataGridCell *GetData(int Column, int Row);
		//The caller is responsible for deleting the cell returned by this.
		virtual void SetData(int Column, int Row, DataGridCell *Value);
		virtual void ClearData();
		virtual void ToData(string GridID,int &Size, char* &Data);
		// Serialize just a chunk of the data.
		virtual void ToData(string GridID,int &Size, char* &Data, int *ColStart, int *RowStart, int ColCount, int RowCount);

		//used by opengl; the engine will handle cells' graphic objects
		virtual void DetachGraphics();

		static pair<int,int> CovertColRowType(ColRowType colRowType) { return make_pair<int,int> (colRowType >> 14,colRowType & 0x3FFF); };
	};
}

#endif
