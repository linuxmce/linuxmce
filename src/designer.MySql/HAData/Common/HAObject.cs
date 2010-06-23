using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.IO;
using System.Diagnostics;
using System.Runtime.InteropServices; 
using System.Threading;
using System.Data.SqlClient;

using HAData.DataAccess;

namespace HAData.Common
{
	/// <summary>
	/// Summary description for HADesignObj.
	/// </summary>
	public class HADesignObj
	{
		public ArrayList m_alHADesignObjs = new ArrayList();
		public ArrayList m_alZones = new ArrayList();
		public ArrayList m_alCommands = new ArrayList();
		public Rectangle m_rect;
		public ScreenGraphic m_sg=null;
		public ArrayList m_alText = new ArrayList();
		public int m_Layer=0;
		public bool m_bHidden=false;
		public int m_Parm=-1;
		public HADesignObj m_haoParent;
		public int m_iArrayCount=0;
		public int m_FK_C_Mode=0;
		public int m_iPK_DesignObjHeader=0;
		public ArrayContinue m_ArrayContinueIn=null;
		public ArrayContinue m_ArrayContinueOut=null;

		public DesignObjHeaderDataRow m_drDesignObjHeader;
		public DesignObjDetailDataRow m_drDesignObjDetail;

		public DesignObjDetail_DesignObjHeaderDataRow m_drDesignObjDetail_DesignObjHeader;
		public DesignObjHeader_DesignObjHeaderDataRow m_drDesignObjHeader_DesignObjHeader;

		public int m_iFK_System=0,m_iFK_Installation=0,m_iFK_Controller=0,m_iFK_Skin=0,m_iFK_Size,m_iFK_Version=0,m_iFK_Language=0,m_iFK_Style;
		public int m_OffsetX,m_OffsetY;
		public bool m_bContainsGraphicVariables=false;
		
		Hashtable m_htVariables;
		private MyDataSet m_mds = HADataConfiguration.m_mdsCache;

		public int PK_DesignObjHeader { get { return m_iPK_DesignObjHeader; } }
		public string Description { get { return m_drDesignObjHeader.fDescription; } }

		public HADesignObj(HADesignObj Parent,DesignObjHeaderDataRow drOH,DesignObjDetail_DesignObjHeaderDataRow drOD_OH,DesignObjHeader_DesignObjHeaderDataRow drOH_OH,int OffsetX,int OffsetY,int Layer,Hashtable htVariables,int System,int Installation,int Controller,int Skin,int Size,int Version,int Language,int Style,int Array,int FK_C_Mode)
		{
			m_FK_C_Mode = FK_C_Mode;
			m_iArrayCount = Array;
			m_haoParent = Parent;
			m_drDesignObjDetail_DesignObjHeader = drOD_OH;
			m_drDesignObjHeader_DesignObjHeader = drOH_OH;
			m_drDesignObjHeader=drOH;
			m_Layer=Layer;
			m_OffsetX=OffsetX;
			m_OffsetY=OffsetY;
			m_htVariables=htVariables;
			m_iPK_DesignObjHeader = m_drDesignObjHeader.fPK_DesignObjHeader;

			m_iFK_System = System;
			m_iFK_Installation = Installation;
			m_iFK_Controller = Controller;
			m_iFK_Skin = Skin;
			m_iFK_Size = Size;
			m_iFK_Version = Version;
			m_iFK_Language = Language;
			m_iFK_Style = Style;
			BuildDesignObj();
		}

		public HADesignObj(HADesignObj Parent,DesignObjHeaderDataRow drOH,DesignObjDetail_DesignObjHeaderDataRow drOD_OH,DesignObjHeader_DesignObjHeaderDataRow drOH_OH,int OffsetX,int OffsetY,int Layer,Hashtable htVariables,int System,int Installation,int Controller,int Skin,int Size,int Version,int Language,int Style,int FK_C_Mode,ArrayContinue arrayContinue)
		{
			m_FK_C_Mode = FK_C_Mode;
			m_haoParent = Parent;
			m_drDesignObjDetail_DesignObjHeader = drOD_OH;
			m_drDesignObjHeader_DesignObjHeader = drOH_OH;
			m_drDesignObjHeader=drOH;
			m_Layer=Layer;
			m_OffsetX=OffsetX;
			m_OffsetY=OffsetY;
			m_htVariables=htVariables;
			m_iPK_DesignObjHeader = m_drDesignObjHeader.fPK_DesignObjHeader;
			m_ArrayContinueIn = arrayContinue;

			m_iFK_System = System;
			m_iFK_Installation = Installation;
			m_iFK_Controller = Controller;
			m_iFK_Skin = Skin;
			m_iFK_Size = Size;
			m_iFK_Version = Version;
			m_iFK_Language = Language;
			m_iFK_Style = Style;
			BuildDesignObj();
		}

		public HADesignObj(HADesignObj Parent,DesignObjHeaderDataRow drOH,DesignObjDetail_DesignObjHeaderDataRow drOD_OH,DesignObjHeader_DesignObjHeaderDataRow drOH_OH,int OffsetX,int OffsetY,int Layer,Hashtable htVariables,int System,int Installation,int Controller,int Skin,int Size,int Version,int Language,int Style,int FK_C_Mode)
		{
			m_FK_C_Mode = FK_C_Mode;
			m_haoParent = Parent;
			m_drDesignObjDetail_DesignObjHeader = drOD_OH;
			m_drDesignObjHeader_DesignObjHeader = drOH_OH;
			m_drDesignObjHeader=drOH;
			m_Layer=Layer;
			m_OffsetX=OffsetX;
			m_OffsetY=OffsetY;
			m_htVariables=htVariables;
			m_iPK_DesignObjHeader = m_drDesignObjHeader.fPK_DesignObjHeader;

			m_iFK_System = System;
			m_iFK_Installation = Installation;
			m_iFK_Controller = Controller;
			m_iFK_Skin = Skin;
			m_iFK_Size = Size;
			m_iFK_Version = Version;
			m_iFK_Language = Language;
			m_iFK_Style = Style;
			BuildDesignObj();
		}

		public HADesignObj(HADesignObj Parent,DesignObjHeaderDataRow drOH,DesignObjDetail_DesignObjHeaderDataRow drOD_OH,DesignObjHeader_DesignObjHeaderDataRow drOH_OH,int OffsetX,int OffsetY,int Layer,Hashtable htVariables,int FK_C_Mode)
		{
			m_FK_C_Mode = FK_C_Mode;
			m_haoParent = Parent;
			m_drDesignObjDetail_DesignObjHeader = drOD_OH;
			m_drDesignObjHeader_DesignObjHeader = drOH_OH;
			m_drDesignObjHeader=drOH;
			m_Layer=Layer;
			m_OffsetX=OffsetX;
			m_OffsetY=OffsetY;
			m_htVariables=htVariables;
			m_iPK_DesignObjHeader = m_drDesignObjHeader.fPK_DesignObjHeader;

			m_iFK_System = HADataConfiguration.iFK_System;
			m_iFK_Installation = HADataConfiguration.iFK_Installation;
			m_iFK_Controller = HADataConfiguration.iFK_Controller;
			m_iFK_Skin = HADataConfiguration.iFK_Skin;
			m_iFK_Size = HADataConfiguration.iFK_Size;
			m_iFK_Version = HADataConfiguration.iFK_Version;
			m_iFK_Language = HADataConfiguration.iFK_Language;
			m_iFK_Style = HADataConfiguration.iFK_Style;
			BuildDesignObj();
		}

		public HADesignObj(HADesignObj Parent,DesignObjHeaderDataRow drOH,DesignObjDetail_DesignObjHeaderDataRow drOD_OH,DesignObjHeader_DesignObjHeaderDataRow drOH_OH,ControllerDataRow drController,int OffsetX,int OffsetY,int Layer,Hashtable htVariables,int FK_C_Mode)
		{
			m_FK_C_Mode = FK_C_Mode;
			m_haoParent = Parent;
			m_drDesignObjDetail_DesignObjHeader = drOD_OH;
			m_drDesignObjHeader_DesignObjHeader = drOH_OH;
			m_drDesignObjHeader=drOH;
			m_Layer=Layer;
			m_OffsetX=OffsetX;
			m_OffsetY=OffsetY;
			m_htVariables=htVariables;
			m_iPK_DesignObjHeader = m_drDesignObjHeader.fPK_DesignObjHeader;

			m_iFK_System = drController.fFK_System;
			m_iFK_Installation = drController.fFK_Installation;
			m_iFK_Controller = drController.fPK_Controller;
			m_iFK_Skin = drController.fFK_Skin;
			m_iFK_Size = drController.fFK_Size;
			m_iFK_Version = drController.fFK_Version;
			m_iFK_Language = drController.fFK_Language;
			m_iFK_Style = drController.fFK_StyleHeader_Default;
			BuildDesignObj();
		}

		public HADesignObj(HADesignObj Parent,DesignObjHeaderDataRow drOH,DesignObjDetail_DesignObjHeaderDataRow drOD_OH,DesignObjHeader_DesignObjHeaderDataRow drOH_OH,ControllerDataRow drController,int OffsetX,int OffsetY,int Layer,Hashtable htVariables,int FK_C_Mode,ArrayContinue arrayContinue)
		{
			m_FK_C_Mode = FK_C_Mode;
			m_haoParent = Parent;
			m_drDesignObjDetail_DesignObjHeader = drOD_OH;
			m_drDesignObjHeader_DesignObjHeader = drOH_OH;
			m_drDesignObjHeader=drOH;
			m_Layer=Layer;
			m_OffsetX=OffsetX;
			m_OffsetY=OffsetY;
			m_htVariables=htVariables;
			m_iPK_DesignObjHeader = m_drDesignObjHeader.fPK_DesignObjHeader;
			m_ArrayContinueIn = arrayContinue;

			m_iFK_System = drController.fFK_System;
			m_iFK_Installation = drController.fFK_Installation;
			m_iFK_Controller = drController.fPK_Controller;
			m_iFK_Skin = drController.fFK_Skin;
			m_iFK_Size = drController.fFK_Size;
			m_iFK_Version = drController.fFK_Version;
			m_iFK_Language = drController.fFK_Language;
			m_iFK_Style = drController.fFK_StyleHeader_Default;
			BuildDesignObj();
		}

		private DataRow GetTextSkinLanguage(DataRow[] drc,out StyleDetailDataRow drSD)
		{
			DataRow drMatch=null;
			foreach(DataRow dr in drc)
			{
				// We match everything perfectly
				if( dr["FK_Skin"]!=DBNull.Value && dr["FK_Language"]!=DBNull.Value && 
					Convert.ToInt32(dr["FK_Skin"])==m_iFK_Skin && 
					Convert.ToInt32(dr["FK_Language"])==m_iFK_Language )
				{
					drMatch=dr;
					break;
				}

				// We match the skin directly 
				else if( dr["FK_Skin"]!=DBNull.Value && Convert.ToInt32(dr["FK_Skin"])==m_iFK_Skin)
					drMatch = dr;
				
				// We match the skin indirectly
				else if( dr["FK_Skin"]!=DBNull.Value && 
					m_mds.tSkin[m_iFK_Skin].fFK_Skin_TextPlacementIsNull==false && 
					Convert.ToInt32(dr["FK_Skin"])==m_mds.tSkin[m_iFK_Skin].fFK_Skin_TextPlacement &&
					(drMatch==null || drMatch["FK_Skin"]==DBNull.Value) )
						drMatch = dr;
				
				// We match the language directly
				else if( dr["FK_Language"]!=DBNull.Value && 
					Convert.ToInt32(dr["FK_Language"])==m_iFK_Language && 
					dr["FK_Skin"]==DBNull.Value && 
					(drMatch==null || drMatch["FK_Skin"]==DBNull.Value) )
						drMatch = dr;
				
				// We match the language indirectly
				else if( dr["FK_Language"]!=DBNull.Value && 
					m_mds.tLanguage[m_iFK_Language].fFK_Language_TextPlacementIsNull==false && 
					Convert.ToInt32(dr["FK_Language"])==m_mds.tLanguage[m_iFK_Language].fFK_Language_TextPlacement &&
					dr["FK_Skin"]==DBNull.Value && 
					(drMatch==null || (drMatch["FK_Skin"]==DBNull.Value && drMatch["FK_Language"]==DBNull.Value)) )
						drMatch = dr;
	
				// We found the standard record
				else if( dr["FK_Language"]==DBNull.Value && dr["FK_Skin"]==DBNull.Value && drMatch==null )
					drMatch = dr;
			}

			Debug.Assert(drMatch!=null);
			int Style = drMatch["FK_StyleHeader"]==DBNull.Value ? m_iFK_Style : Convert.ToInt32(drMatch["FK_StyleHeader"]);
			drSD = HADataConfiguration.ObjMgr.GetStyle(Style,
				m_iFK_System,m_iFK_Installation,m_iFK_Controller,
				m_iFK_Skin,m_iFK_Size,m_iFK_Version,m_iFK_Language);

			return drMatch;
		}
		
		private void BuildDesignObj()
		{
			// First get the object
			m_drDesignObjDetail = HADataConfiguration.ObjMgr.GetDesignObj(m_drDesignObjHeader.fPK_DesignObjHeader,
				m_iFK_System,m_iFK_Installation,m_iFK_Controller,
				m_iFK_Skin,m_iFK_Size,m_iFK_Version,m_iFK_Language);

			// Get my graphic
            object oFileName=HADataConfiguration.GetParm(m_drDesignObjDetail,DesignObjParameterData.GRAPHIC_FILENAME_CONST,m_htVariables,ref m_bContainsGraphicVariables);
			if( m_bContainsGraphicVariables )
			{
				HADesignObj haotmp = m_haoParent;
				while( haotmp!=null )
				{
					haotmp.m_bContainsGraphicVariables=true;
					haotmp = haotmp.m_haoParent;
				}
			}

			if( oFileName!=null && oFileName.ToString().Length>0 )
			{
				object oX = HADataConfiguration.GetParm(m_drDesignObjDetail,DesignObjParameterData.XPOS_CONST,m_htVariables);
				object oY = HADataConfiguration.GetParm(m_drDesignObjDetail,DesignObjParameterData.YPOS_CONST,m_htVariables);
				int iX = m_OffsetX + (oX==null ? 0 : Convert.ToInt32(oX));
				int iY = m_OffsetY + (oY==null ? 0 : Convert.ToInt32(oY));
				m_sg = new ScreenGraphic(this,HADataConfiguration.GetFileName(m_iFK_Skin,oFileName.ToString()),iX,iY);
			}

			// Get my text
			// Add the text that belongs to this object's header, then footer
			foreach(DataRow d in m_drDesignObjDetail.fFK_DesignObjHeader_DataRow.dr.GetChildRows(HARelation.rDesignObjHeader__DesignObjHeader_Text))
			{
				DesignObjHeader_TextDataRow drOHT = new DesignObjHeader_TextDataRow(d);
				StyleDetailDataRow drSD;
				DesignObjHeader_Text_Skin_LanguageDataRow drOHTSL = new DesignObjHeader_Text_Skin_LanguageDataRow(
					GetTextSkinLanguage(drOHT.dr.GetChildRows(HARelation.rDesignObjHeader_Text__DesignObjHeader_Text_Skin_Language),out drSD));

				ScreenText st = new ScreenText(this,drOHT.fFK_Text_DataRow,drOHT,null,drOHTSL,null,drSD,m_iFK_Language,m_Layer);
				st.m_rect = new Rectangle(drOHTSL.fX+m_OffsetX,drOHTSL.fY+m_OffsetY,drOHTSL.fWidth,drOHTSL.fHeight);
				m_alText.Add(st);
			}
			foreach(DataRow d in m_drDesignObjDetail.dr.GetChildRows(HARelation.rDesignObjDetail__DesignObjDetail_Text))
			{
				DesignObjDetail_TextDataRow drODT = new DesignObjDetail_TextDataRow(d);
				StyleDetailDataRow drSD;
				DesignObjDetail_Text_Skin_LanguageDataRow drODTSL = new DesignObjDetail_Text_Skin_LanguageDataRow(
					GetTextSkinLanguage(drODT.dr.GetChildRows(HARelation.rDesignObjDetail_Text__DesignObjDetail_Text_Skin_Language),out drSD));

				ScreenText st = new ScreenText(this,drODT.fFK_Text_DataRow,null,drODT,null,drODTSL,drSD,m_iFK_Language,m_Layer);
				st.m_rect = new Rectangle(drODTSL.fX+m_OffsetX,drODTSL.fY+m_OffsetY,drODTSL.fWidth,drODTSL.fHeight);
				m_alText.Add(st);
			}

			// Get my zones and actions

			// First add the actions that belongs to this object's header, then footer directly (no zones)
			// Do the header, and then the detail
			DataView dv = new DataView(m_mds.tDesignObjDetail_Command,DesignObjDetail_CommandData.FK_OBJECTDETAIL_FIELD + "=" + m_drDesignObjDetail.fPK_DesignObjDetail + " AND " + DesignObjDetail_CommandData.FK_OBJECTDETAIL_ZONE_FIELD + " Is Null",DesignObjDetail_CommandData.EXECUTEORDER_FIELD,DataViewRowState.CurrentRows);
			for(int i=0;i<dv.Count;i++)
			{
				DesignObjDetail_CommandDataRow drODA = new DesignObjDetail_CommandDataRow(dv[i].Row);
                m_alCommands.Add(new ScreenCommand(this,null,drODA));
			}
			dv = new DataView(m_mds.tDesignObjHeader_Command,DesignObjHeader_CommandData.FK_OBJECTHEADER_FIELD + "=" + m_drDesignObjDetail.fFK_DesignObjHeader + " AND " + DesignObjHeader_CommandData.FK_OBJECTHEADER_ZONE_FIELD + " Is Null",DesignObjHeader_CommandData.EXECUTEORDER_FIELD,DataViewRowState.CurrentRows);
			for(int i=0;i<dv.Count;i++)
			{
				DesignObjHeader_CommandDataRow drOHA = new DesignObjHeader_CommandDataRow(dv[i].Row);
				m_alCommands.Add(new ScreenCommand(this,drOHA,null));
			}

			// Now do the same thing, but start with each zone
			foreach(DataRow d in m_drDesignObjDetail.dr.GetChildRows(HARelation.rDesignObjDetail__DesignObjDetail_Zone))
			{
				DesignObjDetail_ZoneDataRow drOHZ = new DesignObjDetail_ZoneDataRow(d);
				ScreenZone sz = new ScreenZone(this,null,drOHZ,drOHZ.fX,drOHZ.fY,drOHZ.fWidth,drOHZ.fHeight,m_Layer);
				dv = new DataView(m_mds.tDesignObjDetail_Command,DesignObjDetail_CommandData.FK_OBJECTDETAIL_ZONE_FIELD + "=" + drOHZ.fPK_DesignObjDetail_Zone,DesignObjDetail_CommandData.EXECUTEORDER_FIELD,DataViewRowState.CurrentRows);
				for(int i=0;i<dv.Count;i++)
				{
					DesignObjDetail_CommandDataRow drODA = new DesignObjDetail_CommandDataRow(dv[i].Row);
					sz.m_alScreenCommands.Add(new ScreenCommand(this,null,drODA));
				}
				m_alZones.Add(sz);
			}
			foreach(DataRow d in m_drDesignObjDetail.fFK_DesignObjHeader_DataRow.dr.GetChildRows(HARelation.rDesignObjHeader__DesignObjHeader_Zone))
			{
				DesignObjHeader_ZoneDataRow drOHZ = new DesignObjHeader_ZoneDataRow(d);
				ScreenZone sz = new ScreenZone(this,drOHZ,null,drOHZ.fX,drOHZ.fY,drOHZ.fWidth,drOHZ.fHeight,m_Layer);
				dv = new DataView(m_mds.tDesignObjHeader_Command,DesignObjHeader_CommandData.FK_OBJECTHEADER_ZONE_FIELD + "=" + drOHZ.fPK_DesignObjHeader_Zone,DesignObjHeader_CommandData.EXECUTEORDER_FIELD,DataViewRowState.CurrentRows);
				for(int i=0;i<dv.Count;i++)
				{
					DesignObjHeader_CommandDataRow drOHA = new DesignObjHeader_CommandDataRow(dv[i].Row);
					sz.m_alScreenCommands.Add(new ScreenCommand(this,drOHA,null));
				}
				m_alZones.Add(sz);
			}

			// Get my children
			DataRow[] drc = m_drDesignObjDetail.fFK_DesignObjHeader_DataRow.dr.GetChildRows(HARelation.rDesignObjHeader__DesignObjHeader_DesignObjHeader_Parent);
			foreach(DataRow d in drc)
			{
				DesignObjHeader_DesignObjHeaderDataRow drOHOH = new DesignObjHeader_DesignObjHeaderDataRow(d);
				DesignObjDetailDataRow drODChild = HADataConfiguration.ObjMgr.GetDesignObj(drOHOH.fFK_DesignObjHeader,
					m_iFK_System,m_iFK_Installation,m_iFK_Controller,
					m_iFK_Skin,m_iFK_Size,m_iFK_Version,m_iFK_Language);

				m_alHADesignObjs.Add(new HADesignObj(this,drODChild.fFK_DesignObjHeader_DataRow,null,drOHOH,drOHOH.fX+m_OffsetX,drOHOH.fY+m_OffsetY,m_Layer,m_htVariables,m_iFK_System,m_iFK_Installation,m_iFK_Controller,m_iFK_Skin,m_iFK_Size,m_iFK_Version,m_iFK_Language,m_iFK_Style,m_FK_C_Mode,m_ArrayContinueIn));
			}

			// Now of the detail
			drc = m_drDesignObjDetail.dr.GetChildRows(HARelation.rDesignObjDetail__DesignObjDetail_DesignObjHeader);
			foreach(DataRow d in drc)
			{
				DesignObjDetail_DesignObjHeaderDataRow drODOH = new DesignObjDetail_DesignObjHeaderDataRow(d);
				DesignObjDetailDataRow drODChild = HADataConfiguration.ObjMgr.GetDesignObj(drODOH.fFK_DesignObjHeader,
					m_iFK_System,m_iFK_Installation,m_iFK_Controller,
					m_iFK_Skin,m_iFK_Size,m_iFK_Version,m_iFK_Language);

				m_alHADesignObjs.Add(new HADesignObj(this,drODChild.fFK_DesignObjHeader_DataRow,drODOH,null,drODOH.fX+m_OffsetX,drODOH.fY+m_OffsetY,m_Layer,m_htVariables,m_iFK_System,m_iFK_Installation,m_iFK_Controller,m_iFK_Skin,m_iFK_Size,m_iFK_Version,m_iFK_Language,m_iFK_Style,m_FK_C_Mode,m_ArrayContinueIn));
			}

			// Duplicate my children if I'm an array
			if( m_htVariables!=null && m_drDesignObjHeader.fFK_DesignObjType==DesignObjTypeData.ARRAY_CONST )
			{
				int RowSpacing=0,ColumnSpacing=0,NumRows=0,NumColumns=0,FixedRowHeight=0,FixedColumnWidth=0,PK_DesignObjHeader_More_Fwd=0,PK_DesignObjHeader_More_Back=0;
				bool bDownAcross=true;

				try { FixedRowHeight = Convert.ToInt32(HADataConfiguration.GetParm(this.m_drDesignObjDetail,DesignObjParameterData.FIXEDROWHEIGHT_CONST,m_htVariables)); } 
				catch(Exception) {}
				try { FixedColumnWidth = Convert.ToInt32(HADataConfiguration.GetParm(this.m_drDesignObjDetail,DesignObjParameterData.FIXEDCOLUMNWIDTH_CONST,m_htVariables)); } 
				catch(Exception) {}
				try { RowSpacing = Convert.ToInt32(HADataConfiguration.GetParm(this.m_drDesignObjDetail,DesignObjParameterData.ROW_SPACING_CONST,m_htVariables)); } 
				catch(Exception) {}
				try { ColumnSpacing = Convert.ToInt32(HADataConfiguration.GetParm(this.m_drDesignObjDetail,DesignObjParameterData.COLUMN_SPACING_CONST,m_htVariables)); } 
				catch(Exception) {}
				try { NumRows = Convert.ToInt32(HADataConfiguration.GetParm(this.m_drDesignObjDetail,DesignObjParameterData.NUMROWS_CONST,m_htVariables)); } 
				catch(Exception) {}
				try { NumColumns = Convert.ToInt32(HADataConfiguration.GetParm(this.m_drDesignObjDetail,DesignObjParameterData.NUMCOLUMNS_CONST,m_htVariables)); } 
				catch(Exception) {}
				try { bDownAcross = HADataConfiguration.GetParm(this.m_drDesignObjDetail,DesignObjParameterData.DOWN_ACROSS_CONST,m_htVariables).ToString()=="1"; } 
				catch(Exception) {}
				try { PK_DesignObjHeader_More_Fwd = Convert.ToInt32(HADataConfiguration.GetParm(this.m_drDesignObjDetail,DesignObjParameterData.PK_OBJHDR_MORE_FWD_CONST,m_htVariables)); } 
				catch(Exception) {}
				try { PK_DesignObjHeader_More_Back = Convert.ToInt32(HADataConfiguration.GetParm(this.m_drDesignObjDetail,DesignObjParameterData.PK_OBJHDR_MORE_BAK_CONST,m_htVariables)); } 
				catch(Exception) {}

				int RowNum=1,ColumnNum=1;

				ArrayList alNewDesignObjs = new ArrayList(),alNewText = new ArrayList(),alNewZones = new ArrayList();

				int PK_Array=Convert.ToInt32(HADataConfiguration.GetParm(m_drDesignObjDetail,DesignObjParameterData.PK_ARRAY_CONST,m_htVariables));

				int iVariableToStore;
				int iArrayIndex=0;
				ArrayList alArrayEntries;
				
				if( m_ArrayContinueIn == null || PK_DesignObjHeader_More_Back==0 )
					alArrayEntries = GetArray(PK_Array,0,m_FK_C_Mode,out iVariableToStore);  // Later on figure the controller issue
				else
				{
					alArrayEntries = m_ArrayContinueIn.alArrayEntries;
					iArrayIndex = m_ArrayContinueIn.iArrayIndex;
					iVariableToStore = m_ArrayContinueIn.iVariableToStore;
				}


//				ArrayEntry aeFirst=null;
				bool bContainsIcons=false;
				string sOriginalIconFile="";

				// Skip the first one, because we're using it as a template, so we have to leave everything in tact.
				// We'll take care of this afterwards.  For now, just store the value and key
				if( alArrayEntries.Count>0 )
				{
//					aeFirst = (ArrayEntry) alArrayEntries[iArrayIndex++];

					Size sSize = new Size();
					// We've got to get the total width of the objects in the array
					if( FixedColumnWidth==0 || FixedRowHeight==0 || NumRows==0 || NumColumns==0 )
					{
						bool bFirstHA=true;
						Rectangle r = new Rectangle();
						foreach(HADesignObj hao in m_alHADesignObjs)
						{
							if( hao.m_bContainsGraphicVariables )
								bContainsIcons=true;

							if( hao.m_rect.Width>0 && hao.m_rect.Height>0 )
							{
								if( bFirstHA )
								{
									bFirstHA=false;
									r = hao.m_rect;
								}
								else
									r = Rectangle.Union(r,hao.m_rect);
							}
						}
						sSize.Width = r.Width;
						sSize.Height = r.Height;

						Debug.Assert(r.Width>0 && r.Height>0);

						int Scale = HADataConfiguration.m_mdsCache.tSize[m_iFK_Size].fScale;

						if( NumRows==0 )
						{
							if( FixedRowHeight>0 )
								NumRows = ((m_mds.tSize[m_iFK_Size].fHeight*1000/Scale)-r.Y) / FixedRowHeight;
							else
								NumRows = ((m_mds.tSize[m_iFK_Size].fHeight*1000/Scale)-r.Y) / (r.Height+RowSpacing);
						}
						if( NumColumns==0 )
						{
							if( FixedColumnWidth>0 )
								NumColumns = ((m_mds.tSize[m_iFK_Size].fWidth*1000/Scale)-r.X) / FixedColumnWidth;
							else
								NumColumns = ((m_mds.tSize[m_iFK_Size].fWidth*1000/Scale)-r.X) / (r.Width+ColumnSpacing);
						}
					}

					if( bContainsIcons )
					{
						if( !m_htVariables.ContainsKey(VariableData.ICON_FILE_CONST) )
							m_htVariables.Add(VariableData.ICON_FILE_CONST,"");

						sOriginalIconFile = m_htVariables[VariableData.ICON_FILE_CONST]==null ? "" : m_htVariables[VariableData.ICON_FILE_CONST].ToString();
					}

					int arrayOffsetX=0,arrayOffsetY=0,iArrayCount=1;

					if( m_ArrayContinueIn != null && PK_DesignObjHeader_More_Back!=0 )
					{
						DesignObjDetailDataRow drODChild = HADataConfiguration.ObjMgr.GetDesignObj(PK_DesignObjHeader_More_Back,
							m_iFK_System,m_iFK_Installation,m_iFK_Controller,
							m_iFK_Skin,m_iFK_Size,m_iFK_Version,m_iFK_Language);

						HADesignObj newhao = new HADesignObj(this,drODChild.fFK_DesignObjHeader_DataRow,null,null,
							m_OffsetX + arrayOffsetX,m_OffsetY + arrayOffsetY,m_Layer,
							m_htVariables,m_iFK_System,m_iFK_Installation,m_iFK_Controller,m_iFK_Skin,m_iFK_Size,
							m_iFK_Version,m_iFK_Language,m_iFK_Style,0);

						ScreenCommand sa = new ScreenCommand(newhao,null,null);
						sa.m_iPK_Command = CommandData.NAV_GOTO_CONST;
						sa.m_iPK_C_ExecTime = C_ExecTimeData.TOUCH_CONST;
						ScreenCommandParm sap = new ScreenCommandParm(CommandParameterData.PK_OBJECT_HEADER_CONST,
							-1,false,m_ArrayContinueIn.iPK_DesignObjHeader_Prior);
						sa.m_alParms.Add(sap);
//						newhao.m_alCommands.Add(sa);
						HADataConfiguration.htPatheticBackPath.Add(newhao,sa);
//						m_ArrayContinueIn.haoBack = newhao; // add the screen action to go back later, otherwise the build routin
//						m_ArrayContinueIn.saBack = sa;  // will get stuck in loop regenerating the prior screen
						alNewDesignObjs.Add(newhao);
						if( bDownAcross )
						{
							RowNum++;
							if( FixedRowHeight>0 )
								arrayOffsetY+=FixedRowHeight;
							else
								arrayOffsetY+=RowSpacing + sSize.Height;
						}
						else
						{
							ColumnNum++;
							if( FixedColumnWidth>0 )
								arrayOffsetX+=FixedColumnWidth;
							else
								arrayOffsetX+=ColumnSpacing + sSize.Width;
						}
					}

					while(iArrayIndex<alArrayEntries.Count)
					{
						if( RowNum>=NumRows && ColumnNum>=NumColumns && iArrayIndex<alArrayEntries.Count-1 )
						{
							// We won't have enough space, if we've got a more button, we're going to continue
							if( PK_DesignObjHeader_More_Fwd>0 )
							{
								DesignObjDetailDataRow drODChild = HADataConfiguration.ObjMgr.GetDesignObj(PK_DesignObjHeader_More_Fwd,
									m_iFK_System,m_iFK_Installation,m_iFK_Controller,
									m_iFK_Skin,m_iFK_Size,m_iFK_Version,m_iFK_Language);

								HADesignObj newhao = new HADesignObj(this,drODChild.fFK_DesignObjHeader_DataRow,null,null,
									m_OffsetX + arrayOffsetX,m_OffsetY + arrayOffsetY,m_Layer,
									m_htVariables,m_iFK_System,m_iFK_Installation,m_iFK_Controller,m_iFK_Skin,m_iFK_Size,
									m_iFK_Version,m_iFK_Language,m_iFK_Style,0);

								alNewDesignObjs.Add(newhao);

								ArrayContinue ac = new ArrayContinue();
								ac.iArrayIndex = iArrayIndex;
								ac.alArrayEntries = alArrayEntries;
								ac.haoForward = newhao;
								ac.iVariableToStore = iVariableToStore;
								m_ArrayContinueOut = ac;
								break;
							}
						}

						ArrayEntry ae = (ArrayEntry) alArrayEntries[iArrayIndex++];
						// Duplicate all DesignObjs, Graphics, Text, & Zones that were added to this
						foreach(HADesignObj hao in m_alHADesignObjs)
						{
							if( bContainsIcons )
							{
								if( ae.iIcon>0 )
									m_htVariables[VariableData.ICON_FILE_CONST] = m_mds.tC_Icon[ae.iIcon].fFileName;
								else
									m_htVariables[VariableData.ICON_FILE_CONST] = "";
							}

							HADesignObj newhao = new HADesignObj(this,hao.m_drDesignObjHeader,hao.m_drDesignObjDetail_DesignObjHeader,hao.m_drDesignObjHeader_DesignObjHeader,hao.m_OffsetX + arrayOffsetX,hao.m_OffsetY + arrayOffsetY,hao.m_Layer,
								m_htVariables,m_iFK_System,m_iFK_Installation,m_iFK_Controller,m_iFK_Skin,m_iFK_Size,
								m_iFK_Version,m_iFK_Language,m_iFK_Style,iArrayCount++);
//							newhao.m_ArrayCount=iArrayCount++;

//							if( hao.m_sg.m_sFileName.ToUpper().IndexOf(".PSD")>0 )
//								newhao.m_sg = new ScreenGraphic(this,HADataConfiguration.GetFileName(m_iFK_Skin,oFileName.ToString()),iX,iY);

							// Do the substitutions on this new object, first adding the set variable command, then substituting the text
							ScreenCommand sa = new ScreenCommand(this,null,null);
							sa.m_iPK_Command = CommandData.GEN_VAR_SET_CONST;
							sa.m_iPK_C_ExecTime=C_ExecTimeData.TOUCH_CONST;
							sa.m_alParms.Add(new ScreenCommandParm(CommandParameterData.PK_VARIABLE_CONST,-1,false,iVariableToStore));
							sa.m_alParms.Add(new ScreenCommandParm(CommandParameterData.VALUE_ASSIGN_CONST,-1,false,ae.iValue));
							newhao.m_alCommands.Add(sa);
							newhao.m_alCommands.Sort(new CommandComparer());

							foreach(ScreenText st in newhao.m_alText )
								st.m_sText = st.m_sText.Replace("<%=" + VariableData.ARRAY_DESCRIPTION_CONST + ":1%>",ae.sValue).Replace("<%=" + VariableData.ARRAY_DESCRIPTION_CONST + ":0%>",ae.sValue);

							alNewDesignObjs.Add(newhao);
						}

						if( RowNum>=NumRows && ColumnNum>=NumColumns )
							break;  // Nothing more will fit on this screen
						
						if( bDownAcross )
						{
							if( RowNum>=NumRows )
							{
								RowNum=1;
								ColumnNum++;
								if( FixedColumnWidth>0 )
									arrayOffsetX+=FixedColumnWidth;
								else
									arrayOffsetX+=ColumnSpacing + sSize.Width;
								arrayOffsetY=0;
							}
							else
							{
								RowNum++;
								if( FixedRowHeight>0 )
									arrayOffsetY+=FixedRowHeight;
								else
									arrayOffsetY+=RowSpacing + sSize.Height;
							}
						}
						else
						{
							if( ColumnNum>=NumColumns )
							{
								ColumnNum=1;
								RowNum++;
								arrayOffsetX=0;
								if( FixedRowHeight>0 )
									arrayOffsetY+=FixedRowHeight;
								else
									arrayOffsetY+=RowSpacing + sSize.Height;
							}
							else
							{
								ColumnNum++;
								if( FixedColumnWidth>0 )
									arrayOffsetX+=FixedColumnWidth;
								else
									arrayOffsetX+=ColumnSpacing + sSize.Width;
							}
						}
					}
					if( iArrayIndex>=alArrayEntries.Count )
						m_ArrayContinueOut=null;
				}
/*
				if( aeFirst==null )
				{
					return; // Delete yourself first??
				}
				else
				{
					m_alHADesignObjs.Clear();
//					ArrayList deleteThese = new ArrayList();
					// Now do the substitution on the original objects which served as templates
//					foreach(HADesignObj hao in m_alHADesignObjs )
					{
						deleteThese.Add(hao);
						ScreenCommand sa = new ScreenCommand(this,null,null);
						sa.m_iPK_Command = CommandData.GEN_VAR_SET_CONST;
						sa.m_iPK_C_ExecTime=C_ExecTimeData.TOUCH_CONST;
						sa.m_alParms.Add(new ScreenCommandParm(CommandParameterData.PK_VARIABLE_CONST,-1,false,iVariableToStore));
						sa.m_alParms.Add(new ScreenCommandParm(CommandParameterData.VALUE_ASSIGN_CONST,-1,false,aeFirst.iValue));
						hao.m_alCommands.Add(sa);
						hao.m_alCommands.Sort(new CommandComparer());

						foreach(ScreenText st in hao.m_alText )
							st.m_sText = st.m_sText.Replace("<%=" + VariableData.ARRAY_DESCRIPTION_CONST + ":1%>",aeFirst.sValue).Replace("<%=" + VariableData.ARRAY_DESCRIPTION_CONST + ":0%>",aeFirst.sValue);

						if( bContainsIcons )
						{
							if( aeFirst.iIcon>0 )
								m_htVariables[VariableData.ICON_FILE_CONST] = m_mds.tC_Icon[aeFirst.iIcon].fFileName;
							else
								m_htVariables[VariableData.ICON_FILE_CONST] = "";

							CheckForIcon(hao);
						}
					}
//					foreach(HADesignObj hao in 
*/
				if( bContainsIcons )
					m_htVariables[VariableData.ICON_FILE_CONST] = sOriginalIconFile;

//				foreach(HADesignObj hao in alNewDesignObjs)
				m_alHADesignObjs = alNewDesignObjs;
			}


			m_alHADesignObjs.Sort(new HADesignObjComparer());

			// Now get my dimensions as a union of my graphic, my text objects, and my child objects
			bool bFirst=true;

			if( m_sg!=null && m_sg.m_bValid )
			{
				m_rect = m_sg.m_rect;
				bFirst=false;
			}

			foreach(ScreenText st in m_alText)
			{
				if( st.m_rect.Width>0 && st.m_rect.Height>0 )
				{
					if( bFirst )
					{
						m_rect = st.m_rect;
						bFirst=false;
					}
					else
						m_rect = Rectangle.Union(m_rect,st.m_rect);
				}
			}

			foreach(HADesignObj hao in m_alHADesignObjs)
			{
				if( hao.m_rect.Width>0 && hao.m_rect.Height>0 )
				{
					if( bFirst )
					{
						m_rect = hao.m_rect;
						bFirst=false;
					}
					else
						m_rect = Rectangle.Union(m_rect,hao.m_rect);
				}
			}
			return;
		}

		public void CheckForIcon(HADesignObj hao)
		{
			if( hao.m_sg==null || !hao.m_sg.m_bValid )
			{
				// We've got a graphic that didn't load, maybe it's an icon, try again
				bool bContainsGraphicVariables=false;
				object oFileName=HADataConfiguration.GetParm(hao.m_drDesignObjDetail,DesignObjParameterData.GRAPHIC_FILENAME_CONST,m_htVariables,ref bContainsGraphicVariables);
				if( bContainsGraphicVariables )
				{
					object oX = HADataConfiguration.GetParm(m_drDesignObjDetail,DesignObjParameterData.XPOS_CONST,m_htVariables);
					object oY = HADataConfiguration.GetParm(m_drDesignObjDetail,DesignObjParameterData.YPOS_CONST,m_htVariables);
					int iX = m_OffsetX + (oX==null ? 0 : Convert.ToInt32(oX));
					int iY = m_OffsetY + (oY==null ? 0 : Convert.ToInt32(oY));
					hao.m_sg = new ScreenGraphic(hao,HADataConfiguration.GetFileName(m_iFK_Skin,oFileName.ToString()),iX,iY);
				}
			}
			foreach(HADesignObj hao2 in hao.m_alHADesignObjs)
				CheckForIcon(hao2);
		}


		public ArrayList GetArray(int PK_Array,int Controllr,int iMode,out int iVariableToStore)
		{
			int iController = Controllr<1 ? m_iFK_Controller : Controllr;

			iVariableToStore=0;

			ArrayList alValues = new ArrayList();
			if( PK_Array==ArrayData.LIGHTING_SCENARIOS_CONST ||
				PK_Array==ArrayData.CLIMATE_SCENARIOS_CONST ||
				PK_Array==ArrayData.ENTERTAINMENT_SCENARIOS_CONST ||
				PK_Array==ArrayData.SECURITY_SCENARIOS_CONST )
			{
				iVariableToStore = VariableData.PK_SCENARIO_HEADER_CONST;
	
				DataView dv = new DataView();/* = new DataView(HADataConfiguration.m_mdsCache.tScenario_Controller,
					Scenario_ControllerData.FK_CONTROLLER_FIELD + "=" + iController,
					Scenario_ControllerData.SORT_FIELD,DataViewRowState.CurrentRows);*/

				ArrayEntry ae=null;
/*
				for(int i=0; i<dv.Count; i++ )
				{
					ScenarioDataRow drS = m_mds.tScenario[Convert.ToInt32(dv[i][Scenario_ControllerData.FK_SCENARIO_FIELD])];
					Scenario_ControllerDataRow drSC = new Scenario_ControllerDataRow(dv[i].Row);

					int Icon=0;
					if( !drSC.fFK_C_IconIsNull )
						Icon = drSC.fFK_C_Icon;

					if( drS.fFK_Array==PK_Array )
					{
						// If we've already added an entry with this sort order that is an exact match, skip this
						// and add only if the mode is 0, or it's an exact match
						if( !(ae!=null && ae.iMode!=0 && ae.iSortOrder==drSC.fSort) && 
							drS.fFK_C_Mode_ShowWhenInIsNull || drS.fFK_C_Mode_ShowWhenIn==iMode )
								alValues.Add(new ArrayEntry(drS.fPK_Scenario,drS.fDescription,Icon,drSC.fSort,
									drS.fFK_C_Mode_ShowWhenInIsNull ? 1 : drS.fFK_C_Mode_ShowWhenIn ));
					}
				}
			}

			else if( PK_Array==ArrayData.ROOMS_CONST )
			{
				iVariableToStore = VariableData.PK_ROOM_CONST;
	
				DataView dv = new DataView(HADataConfiguration.m_mdsCache.tRoom,
					RoomData.FK_INSTALLATION_FIELD + "=" + m_mds.tController[iController].fFK_Installation,
					RoomData.DESCRIPTION_FIELD,DataViewRowState.CurrentRows);

				for(int i=0; i<dv.Count; i++ )
				{
					RoomDataRow dr = new RoomDataRow(dv[i].Row);
					int Icon = dr.fFK_C_IconIsNull ? 0 : dr.fFK_C_Icon;
					alValues.Add(new ArrayEntry(dr.fPK_Room,dr.fDescription,Icon,i,0));
				}
			}

			else if( PK_Array==ArrayData.LIGHTS_IN_ROOM_CONST || PK_Array==ArrayData.DEVICES_IN_ROOM_CONST )
			{
				string DeviceFilter = "";
				if( PK_Array==ArrayData.LIGHTS_IN_ROOM_CONST )
					DeviceFilter = DeviceData.FK_DEVICECATEGORY_FIELD + "=" + DeviceCategoryData.LIGHTS_CONST;
				else
					DeviceFilter = DeviceData.FK_DEVICECATEGORY_FIELD + "<>" + DeviceCategoryData.LIGHTS_CONST + " AND " +
						DeviceData.FK_OBJECTHEADER_FIELD + " IS NOT NULL";

				iVariableToStore = VariableData.PK_DEVICE_CONST;
				int Room = Convert.ToInt32(m_htVariables[VariableData.PK_ROOM_CONST]);
	
				DataView dv = new DataView(HADataConfiguration.m_mdsCache.tDevice,
					DeviceData.FK_ROOM_FIELD + "=" + Room + " AND " +
					DeviceFilter,
					DeviceData.DESCRIPTION_FIELD,DataViewRowState.CurrentRows);

				for(int i=0; i<dv.Count; i++ )
				{
					DeviceDataRow dr = new DeviceDataRow(dv[i].Row);
					alValues.Add(new ArrayEntry(dr.fPK_Device,dr.fDescription,0,i,0));
				}
			}

			else if( PK_Array==ArrayData.ROOMS_LIGHTS_CONST || PK_Array==ArrayData.ROOMS_DEVICES_CONST )
			{
				iVariableToStore = VariableData.PK_DEVICE_CONST;
				
				SqlCommand cmd = new SqlCommand("SELECT * FROM " + 
					(PK_Array==ArrayData.ROOMS_LIGHTS_CONST ? "v_RoomsAndLights" : "v_RoomsAndDevices") +
					" WHERE FK_Installation=" + m_mds.tController[iController].fFK_Installation +
					" ORDER BY Description",
					m_mds.m_conn,null);

				SqlDataReader dr = cmd.ExecuteReader();
				int i=0;

				while(dr.Read())
					alValues.Add(new ArrayEntry(Convert.ToInt32(dr["PK_Device"]),dr["Description"].ToString(),0,i++,0));
*/
			}
			return alValues;
		}

		private class ArrayEntry
		{
			public int iValue;
			public string sValue;
			public int iIcon;
			public int iMode;
			public int iSortOrder;

			public ArrayEntry(int Value,string str,int Icon,int sortOrder,int mode)
			{
				iValue=Value;
				sValue=str;
				iIcon=Icon;
				iSortOrder=sortOrder;
				iMode = mode;
			}
		}
	}

/*
	public class ChildDesignObj
	{
		public DesignObjDetailDataRow m_drDesignObjDetail;
		public Rectangle m_rect;
		public ScreenGraphic m_sg=null;
		public int m_Layer,m_Parm=-1;
		public DesignObjHeader_DesignObjHeaderDataRow m_drDesignObjHeader_DesignObjHeader;
		public DesignObjDetail_DesignObjHeaderDataRow m_drDesignObjDetail_DesignObjHeader;
		public bool m_bAssignedDimensions;
		public bool m_bHidden=false;

		public int PK_DesignObjHeader { get { return m_drDesignObjDetail.fFK_DesignObjHeader; } }
		public string Description { get { return m_drDesignObjDetail.fFK_DesignObjHeader_DataRow.fDescription; } }

		public ChildDesignObj(DesignObjDetailDataRow dr,DesignObjHeader_DesignObjHeaderDataRow drOHOH,DesignObjDetail_DesignObjHeaderDataRow drODOH,ArrayList alGraphics,int X,int Y,int Width,int Height,int Layer)
		{
			m_Layer=Layer;
			m_drDesignObjDetail=dr;
			m_rect=new Rectangle(X,Y,Width,Height);
			m_drDesignObjHeader_DesignObjHeader = drOHOH;
			m_drDesignObjDetail_DesignObjHeader = drODOH;

			object oFileName=HADataConfiguration.GetParm(dr,DesignObjParameterData.GRAPHIC_FILENAME_CONST);

			if( oFileName!=null )
			{
				object oX = HADataConfiguration.GetParm(dr,DesignObjParameterData.XPOS_CONST);
				object oY = HADataConfiguration.GetParm(dr,DesignObjParameterData.YPOS_CONST);

				if( oX==null )
					oX=0;

				if( oY==null )
					oY=0;

				m_sg = new ScreenGraphic(this,oFileName.ToString(),Convert.ToInt32(oX)+X,Convert.ToInt32(oY)+Y);
				if( m_sg.m_bValid )
					alGraphics.Add(m_sg);

				m_rect = Rectangle.Union(m_rect,m_sg.m_rect);
				m_bAssignedDimensions=true;
			}
			if( m_drDesignObjHeader_DesignObjHeader!=null )
			{
				if( !m_drDesignObjHeader_DesignObjHeader.fWidthIsNull )
				{
					m_rect.Width = Math.Max(m_rect.Width,m_drDesignObjHeader_DesignObjHeader.fWidth);
					m_bAssignedDimensions=true;
				}
				if( !m_drDesignObjHeader_DesignObjHeader.fHeightIsNull )
				{
					m_rect.Height = Math.Max(m_rect.Height,m_drDesignObjHeader_DesignObjHeader.fHeight);
					m_bAssignedDimensions=true;
				}
			}
			else if( m_drDesignObjDetail_DesignObjHeader!=null )
			{
				if( !m_drDesignObjDetail_DesignObjHeader.fWidthIsNull )
				{
					m_rect.Width = Math.Max(m_rect.Width,m_drDesignObjDetail_DesignObjHeader.fWidth);
					m_bAssignedDimensions=true;
				}
				if( !m_drDesignObjDetail_DesignObjHeader.fHeightIsNull )
				{
					m_rect.Height = Math.Max(m_rect.Height,m_drDesignObjDetail_DesignObjHeader.fHeight);
					m_bAssignedDimensions=true;
				}
			}
		}
	}
*/

	public class HAScreen
	{
		public HADesignObj m_hao;
		public int m_iPK_Language;
		public int m_iPK_C_Mode;
		public int m_iPK_DesignObjHeader;

		public HAScreen(HADesignObj hao,int Language,int Mode)
		{
			m_hao = hao;
			m_iPK_Language=Language;
			m_iPK_C_Mode=Mode;
			m_iPK_DesignObjHeader = m_hao.m_drDesignObjHeader.fPK_DesignObjHeader;
		}

		public string Description {	get	{ return m_hao.m_drDesignObjHeader.fDescription; } }
		public int PK_DesignObjHeader { get { return m_iPK_DesignObjHeader; } }
	}

	public class ScreenText
	{
		public string m_sText;
		public Rectangle m_rect;
		public TextDataRow m_drText;
		public StyleDetailDataRow m_drStyleDetail;
		public int m_Layer,m_Parm=-1;
		public int m_PK_Language;
		public HADesignObj m_hao;
		public DesignObjDetail_TextDataRow m_drDesignObjDetail_Text;
		public DesignObjHeader_TextDataRow m_drDesignObjHeader_Text;
		public DesignObjHeader_Text_Skin_LanguageDataRow m_drDesignObjHeader_Text_Skin_Language;
		public DesignObjDetail_Text_Skin_LanguageDataRow m_drDesignObjDetail_Text_Skin_Language;

		public ScreenText(HADesignObj hao,TextDataRow drT,DesignObjHeader_TextDataRow OHT,DesignObjDetail_TextDataRow ODT,DesignObjHeader_Text_Skin_LanguageDataRow drOHTSL,DesignObjDetail_Text_Skin_LanguageDataRow drODTSL,StyleDetailDataRow drS,int PK_Language,int Layer)
		{
			m_hao = hao;
			m_PK_Language=PK_Language;
			m_Layer = Layer;
			m_drText = drT;
			m_drStyleDetail = drS;
			m_drDesignObjDetail_Text = ODT;
			m_drDesignObjHeader_Text = OHT;
			m_sText = m_drText.fDescription_LS(PK_Language);
			m_drDesignObjHeader_Text_Skin_Language = drOHTSL;
			m_drDesignObjDetail_Text_Skin_Language = drODTSL;
		}
	}

	public class ScreenZone
	{
		public ArrayList m_alScreenCommands = new ArrayList();
		public Rectangle m_rect;
		public int m_Layer,m_Parm=-1;
		public DesignObjDetail_ZoneDataRow m_drDesignObjDetail_Zone;
		public DesignObjHeader_ZoneDataRow m_drDesignObjHeader_Zone;
		public HADesignObj m_hao;

		public ScreenZone(HADesignObj hao,DesignObjHeader_ZoneDataRow drOHZ,DesignObjDetail_ZoneDataRow drODZ,int X,int Y,int Width,int Height,int Layer)
		{
			m_hao=hao;
			m_drDesignObjHeader_Zone = drOHZ;
			m_drDesignObjDetail_Zone = drODZ;
			m_rect = new Rectangle(X,Y,Width,Height);
			m_Layer=Layer;
		}
		public ScreenZone(Rectangle rect,int Layer)
		{
			m_rect = rect;
			m_Layer=Layer;
		}
	}

	public class ScreenCommandParm
	{
		public int m_iPK_CommandParameter,m_iPK_Variable=-1;
		public bool m_bEvaluateRuntime;
		public object m_oValue;

		public ScreenCommandParm(int CommandParameter,int Variable,bool EvaluateRuntime,object Value)
		{
			m_iPK_CommandParameter=CommandParameter;
			m_iPK_Variable=Variable;
			m_bEvaluateRuntime=EvaluateRuntime;
			m_oValue=Value;
		}
	}

	public class ScreenCommand
	{
		public DesignObjHeader_CommandDataRow m_drDesignObjHeader_Command;
		public DesignObjDetail_CommandDataRow m_drDesignObjDetail_Command;

		public int m_iPK_Command=0,m_iPK_C_ExecTime,m_iTimeout;
		public ArrayList m_alParms = new ArrayList();
		public HADesignObj m_hao;

		public ScreenCommand(HADesignObj hao,DesignObjHeader_CommandDataRow drOHA,DesignObjDetail_CommandDataRow drODA)
		{
			m_hao = hao;
			DataRow[] drcParms=null;
			if( drOHA!=null )
			{
				m_drDesignObjHeader_Command=drOHA;
				m_iPK_Command = drOHA.fFK_Command;
				m_iPK_C_ExecTime=drOHA.fFK_C_ExecTime;
				m_iTimeout=drOHA.fTimeout;
				drcParms = drOHA.dr.GetChildRows(HARelation.rDesignObjHeader_Command__DesignObjHeader_Command_CommandParameter);
			}
			else if( drODA!=null )
			{
				m_drDesignObjDetail_Command=drODA;
				m_iPK_Command = drODA.fFK_Command;
				m_iPK_C_ExecTime=drODA.fFK_C_ExecTime;
				m_iTimeout=drODA.fTimeout;
				drcParms = drODA.dr.GetChildRows(HARelation.rDesignObjDetail_Command__DesignObjDetail_Command_CommandParameter);
			}
				
			if( drcParms!=null )
			{
				foreach(DataRow dr in drcParms)
				{
					m_alParms.Add(new ScreenCommandParm(
					Convert.ToInt32(dr[DesignObjDetail_Command_CommandParameterData.FK_C_ACTIONPARAMETER_FIELD]),
					dr[DesignObjDetail_Command_CommandParameterData.FK_VARIABLE_FIELD]==DBNull.Value ? -1 : Convert.ToInt32(dr[DesignObjDetail_Command_CommandParameterData.FK_VARIABLE_FIELD]),
					Convert.ToBoolean(dr[DesignObjDetail_Command_CommandParameterData.EVALUATERUNTIME_FIELD]),
					dr[DesignObjDetail_Command_CommandParameterData.VALUE_FIELD]));
				}
			}
		}
	}

	public class ScreenGraphic
	{
		public string m_sFileName;
		public Rectangle m_rect = new Rectangle();
		public bool m_bValid=false;
		public Bitmap m_Bitmap;
		public HADesignObj m_hao;

		[DllImport("User32.dll")]
		public static extern System.Int32 FindWindow(string sClass, string sName);
		[DllImport("User32.dll")]
		public static extern System.Int32 FindWindowEx(int hParent,int hChildAfter,string sClass, string sName);
		[DllImport("User32.dll")]
		public static extern bool PostMessage(System.Int32 hWnd,System.UInt16 Msg,System.UInt16 wParam,System.Int32 lParam);

		public ScreenGraphic(HADesignObj hao,string FileName,int X,int Y)
		{
			m_hao=hao;
			m_sFileName = FileName;
			m_rect.X = X;
			m_rect.Y = Y;

			try
			{
				m_sFileName = HADataConfiguration.GetFileName(HADataConfiguration.iFK_Skin,FileName);

				if( !File.Exists(m_sFileName) )
				{
					if( File.Exists(m_sFileName+".PSD") )
						m_sFileName += ".PSD";
					else if( File.Exists(m_sFileName+".PNG") )
						m_sFileName += ".PNG";
					else if( File.Exists(m_sFileName+".BMP") )
						m_sFileName += ".BMP";
				}

				if( m_sFileName.ToUpper().EndsWith(".PSD") )
				{
					if( !File.Exists(m_sFileName) )
					{
						MessageBox.Show("File: " + m_sFileName + " not found");
						return;
					}

					ArrayList alDesignObjs = new ArrayList();
					string PSDSuffix="";
					DateTime dtDesignObjModified = hao.m_drDesignObjDetail.fModified;

					HADesignObj haoTmp = m_hao;
					while(haoTmp!=null)
					{
						if( haoTmp.m_drDesignObjDetail_DesignObjHeader==null && haoTmp.m_drDesignObjHeader_DesignObjHeader==null )
						{
							if( haoTmp.m_drDesignObjDetail.fModified>dtDesignObjModified )
								dtDesignObjModified = haoTmp.m_drDesignObjDetail.fModified;
							PSDSuffix += "-" + haoTmp.m_drDesignObjDetail.fPK_DesignObjDetail;
						}
						else if( hao.m_drDesignObjDetail_DesignObjHeader!=null )
						{
							if( haoTmp.m_drDesignObjDetail_DesignObjHeader.fModified>dtDesignObjModified )
								dtDesignObjModified = haoTmp.m_drDesignObjDetail_DesignObjHeader.fModified;
							PSDSuffix += "-D" + haoTmp.m_drDesignObjDetail_DesignObjHeader.fPK_DesignObjDetail_DesignObjHeader;
						}
						else if( haoTmp.m_drDesignObjHeader_DesignObjHeader!=null )
						{
							if( haoTmp.m_drDesignObjHeader_DesignObjHeader.fModified>dtDesignObjModified )
								dtDesignObjModified = haoTmp.m_drDesignObjHeader_DesignObjHeader.fModified;
							PSDSuffix += "-H" + haoTmp.m_drDesignObjHeader_DesignObjHeader.fPK_DesignObjHeader_DesignObjHeader;
						}

						if( haoTmp.m_iArrayCount>0 )
							PSDSuffix += "_" + haoTmp.m_iArrayCount;
					
						if( haoTmp.m_haoParent!=null )
							alDesignObjs.Add(haoTmp.m_haoParent);
						haoTmp = haoTmp.m_haoParent;
					}

					if( alDesignObjs.Count==0 )
						return; // Nothing to merge, since there's no background

					string m_sBmpFile = m_sFileName.ToUpper().Replace(".PSD","-" + PSDSuffix + ".BMP");
					// If the file was written after the photoshop file and after the last modification to the object, we're cool

					if( File.Exists(m_sBmpFile) && File.GetLastWriteTime(m_sBmpFile) > File.GetLastWriteTime(m_sFileName) && File.GetLastWriteTime(m_sBmpFile) > dtDesignObjModified )
						m_sFileName = m_sBmpFile;
					else
					{
						// Gotta build it
			bool b1 = File.Exists(m_sBmpFile);
			DateTime dt1=b1 ? File.GetLastWriteTime(m_sBmpFile) : DateTime.Now;
			DateTime dt2=File.GetLastWriteTime(m_sFileName);

/*
						// We've got to get the position, and create an image of the background
						int absX=0,absY=0;  // Absolute offsets from the top
						Size s = new Size(0,0);
						for(int i = alDesignObjs.Count-1;i>=0;i--)
						{
							haoTmp = (HADesignObj) alDesignObjs[i];
							if( haoTmp.m_sg!=null && haoTmp.m_sg.m_bValid )
							{
								s.Width = Math.Max(s.Width,haoTmp.m_sg.m_Bitmap.Width + haoTmp.m_OffsetX);
								s.Height = Math.Max(s.Height,haoTmp.m_sg.m_Bitmap.Height + haoTmp.m_OffsetY);
							}
							absX+=haoTmp.m_OffsetX;
							absY+=haoTmp.m_OffsetY;
						}
*/
						int Scale = HADataConfiguration.m_mdsCache.tSize[m_hao.m_iFK_Size].fScale;
						Bitmap bBackground = new Bitmap(
							HADataConfiguration.m_mdsCache.tSize[m_hao.m_iFK_Size].fWidth * 1000 / Scale,
							HADataConfiguration.m_mdsCache.tSize[m_hao.m_iFK_Size].fHeight * 1000 / Scale);

						Graphics g = Graphics.FromImage(bBackground);

						int OffsetX=0,OffsetY=0;

						for(int i = alDesignObjs.Count-1;i>=0;i--)
						{
							haoTmp = (HADesignObj) alDesignObjs[i];
							OffsetX += haoTmp.m_OffsetX;
							OffsetY += haoTmp.m_OffsetY;
							if( haoTmp.m_sg!=null && haoTmp.m_sg.m_bValid )
								g.DrawImage(haoTmp.m_sg.m_Bitmap,new Rectangle(OffsetX+haoTmp.m_sg.m_rect.X,OffsetY+haoTmp.m_sg.m_rect.Y,haoTmp.m_sg.m_Bitmap.Width,haoTmp.m_sg.m_Bitmap.Height));
						}

						// How do I set the width???
						m_rect.Width=275; m_rect.Height=290;

						Bitmap bPiece = bBackground.Clone(m_rect,System.Drawing.Imaging.PixelFormat.DontCare);
						Clipboard.SetDataObject(bPiece);

						if( File.Exists(@"C:\HA.BMP") )
							File.Delete(@"C:\HA.BMP");
						if( File.Exists(@"C:\TEMP.PSD") )
							File.Delete(@"C:\TEMP.PSD");
/*
//						PhotoshopApplicationClass c = new PhotoshopApplicationClass();
//						c.Open(m_sFileName);
//						c.PlayCommand(@"Import Background");

						if( !File.Exists(@"C:\HA.BMP") )
						{
							MessageBox.Show("Error creating file with Photoshop");
							return;
						}

						if( File.Exists(m_sBmpFile) )
							File.Delete(m_sBmpFile);

						File.Move(@"C:\HA.BMP",m_sBmpFile);

						HADataConfiguration.m_MainForm.Activate();
						HADataConfiguration.m_MainForm.Refresh();
//						if( HADataConfiguration.m_MainForm.Focus()==false )
//							MessageBox.Show("unable to focus");
//						HADataConfiguration.m_MainForm.Refresh();

						m_sFileName=m_sBmpFile;
*/
						/*
						int h1 = FindWindow("Photoshop","Adobe Photoshop");
						int h2 = FindWindowEx(h1,0,"MDIClient",null);
						int h = FindWindowEx(h2,0,"PSDocC",null);

						string x = h.ToString("x");
						bool b=PostMessage(h,0x111,(System.UInt16) 0xF060,0x006A0240);  // control o

						MessageBox.Show("ok?");
						return;
						if( MessageBox.Show("ctrlo","",MessageBoxButtons.OKCancel)==DialogResult.Cancel )
							return;
						b=PostMessage(h,0x100,0x11,0x401D0001);  // control o
						b=PostMessage(h,0x100,0x4F,0x00180001);  // control o
						b=PostMessage(h,0x102,0x0F,0x00180001);  // control o


//						int h2 = FindWindowEx(h1,0,"#32770 (Dialog)","Open");

						if( h2<1 )
						{
							MessageBox.Show("No open");
							return;
						}

//						b=PostMessage(h,0x104,0x12,0x20380001);  // alt-f
//						b=PostMessage(h,0x104,0x46,0x20210001);  // alt-f
//						b=PostMessage(h,0x100,0x4F,0x10180001); // o

						if( MessageBox.Show("circle","",MessageBoxButtons.OKCancel)==DialogResult.Cancel )
							return;

						b=PostMessage(h2,0x100,0x43,0x002E0001); // c
						b=PostMessage(h2,0x100,0x49,0x00170001); // i
						b=PostMessage(h2,0x100,0x52,0x00130001); // r
						b=PostMessage(h2,0x100,0x43,0x002E0001); // c
						b=PostMessage(h2,0x100,0x4c,0x00260001); // l
						b=PostMessage(h2,0x100,0x45,0x00120001); // e
						b=PostMessage(h2,0x100,0xBE,0x00340001); // .
						b=PostMessage(h2,0x100,0x50,0x00190001); // p
						b=PostMessage(h2,0x100,0x53,0x001F0001); // s
						b=PostMessage(h2,0x100,0x44,0x00200001); // d
						if( MessageBox.Show("enter","",MessageBoxButtons.OKCancel)==DialogResult.Cancel )
							return;
						b=PostMessage(h2,0x100,0x0D,0x001C0001); // enter

						MessageBox.Show("done");
						return;
						*/

					}
				}

				m_Bitmap = new Bitmap(Image.FromFile(m_sFileName));
				m_rect.Width = m_Bitmap.Width;
				m_rect.Height = m_Bitmap.Height;
			}
			catch(Exception) {}

			if( m_Bitmap!=null )
				m_bValid=true;
		}
	}
	public class CommandComparer : System.Collections.IComparer
	{
		public int Compare(object obj1,object obj2)
		{
			ScreenCommand sa1 = (ScreenCommand) obj1,sa2 = (ScreenCommand) obj2;

			// sa1 isn't attached to anything.  It must be an assign var added to an array.  Return -1 so it's always put at the top
			if( sa1.m_drDesignObjDetail_Command==null && sa1.m_drDesignObjHeader_Command==null && sa2.m_drDesignObjDetail_Command==null && sa2.m_drDesignObjHeader_Command==null )
				return 0;
			if( sa1.m_drDesignObjDetail_Command==null && sa1.m_drDesignObjHeader_Command==null )
				return -1;
			if( sa2.m_drDesignObjDetail_Command==null && sa2.m_drDesignObjHeader_Command==null )
				return 1;

			int Val1 = sa1.m_drDesignObjHeader_Command==null ? sa1.m_drDesignObjDetail_Command.fExecuteOrder : sa1.m_drDesignObjHeader_Command.fExecuteOrder;
			int Val2 = sa2.m_drDesignObjHeader_Command==null ? sa2.m_drDesignObjDetail_Command.fExecuteOrder : sa2.m_drDesignObjHeader_Command.fExecuteOrder;

			if( Val1 == Val2 )
				return 0;

			if( Val1 < Val2 )
				return -1;

			return 1;
		}
	}
	public class HADesignObjComparer : System.Collections.IComparer
	{
		public int Compare(object obj1,object obj2)
		{
			HADesignObj ha1 = (HADesignObj) obj1,ha2 = (HADesignObj) obj2;

			// sa1 isn't attached to anything.  It must be an assign var added to an array.  Return -1 so it's always put at the top
			if( ha1.m_drDesignObjDetail_DesignObjHeader==null && ha1.m_drDesignObjHeader_DesignObjHeader==null && 
				ha2.m_drDesignObjDetail_DesignObjHeader==null && ha2.m_drDesignObjHeader_DesignObjHeader==null )
				return 0;
			if( ha1.m_drDesignObjDetail_DesignObjHeader==null && ha1.m_drDesignObjHeader_DesignObjHeader==null )
				return -1;
			if( ha2.m_drDesignObjDetail_DesignObjHeader==null && ha2.m_drDesignObjHeader_DesignObjHeader==null )
				return 1;

			int Val1 = ha1.m_drDesignObjHeader_DesignObjHeader==null ? ha1.m_drDesignObjDetail_DesignObjHeader.fDisplayOrder : ha1.m_drDesignObjHeader_DesignObjHeader.fDisplayOrder;
			int Val2 = ha2.m_drDesignObjHeader_DesignObjHeader==null ? ha2.m_drDesignObjDetail_DesignObjHeader.fDisplayOrder : ha2.m_drDesignObjHeader_DesignObjHeader.fDisplayOrder;

			if( Val1 == Val2 )
				return 0;

			if( Val1 < Val2 )
				return -1;

			return 1;
		}
	}
	public class ArrayContinue 
	{
		public int iArrayIndex=0,iPK_DesignObjHeader_Prior=0,iPK_DesignObjHeader_Next=0,iVariableToStore=0;
		public ArrayList alArrayEntries=null;
		public HADesignObj haoForward=null;
		public HADesignObj haoBack=null;
		public ScreenCommand saBack=null;
	}
}
