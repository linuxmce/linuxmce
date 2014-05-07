using System;
using System.Data;
using System.Data.SqlClient;
using MySql;
	using MySql.Data;
	using MySql.Data.MySqlClient;
using System.Collections;
using System.Configuration;
using System.Diagnostics;
using System.Windows.Forms;
using Microsoft.Win32;
using System.IO;
using System.Drawing;

// using HAData.Common;
using HAData.DataAccess;

namespace HAData.Common 
{
 
	public class HADataConfiguration
	{
		//----------------------------------------------------------------
//		public static string ConnectionString=@"C:\HA\dbase.txt";
//		public static string ConnectionString="server=windows-server;User ID=sa;Password=orlando10;database=HA";
//		public static string ConnectionString="server=localhost;User ID=sa;Password=configit;database=HA";
//		public static string ConnectionString="server=localhost;User ID=sa;Password=msvirus;database=HA";
//		public static string ConnectionString="server=192.168.1.116;User ID=sa;Password=orlando10;database=HA";
//		public static string ConnectionString="server=localhost;user id=sa;password=msvirus;database=HA";
//		public static string ConnectionString=@"DRIVER={MySQL ODBC 3.51 Driver};SERVER=localhost;UID=root;PWD=;DATABASE=pluto_dce;";
		public static string ConnectionString=@"SERVER=localhost;UID=root;PWD=;DATABASE=pluto_dce;";
		public static string Company = "Business Advantages";
		public static string AppName = "Home Automation";
		public static string DataPath = @"Y:\graphics";
		public static int[] iCustomColors = null;
		public static RegistryKey rkApp=null;
		public static Form m_MainForm=null;

//		public static DesignObjManager ObjMgr = null;
		public static MyDataSet m_mdsCache;
		public static SqlConnection connection=null;
		public static int iFK_System=0,iFK_Installation=0,iFK_Controller=0,iFK_Skin=0,iFK_Size,iFK_Version=0,iFK_Language=0,iFK_Style;
		public static int iZoom=100;
		public static Hashtable htPatheticBackPath = new Hashtable();

		public static SqlConnection GetSqlConnection()
		{
			return new SqlConnection(ConnectionString);
		}
		
		public static MySqlConnection GetMySqlConnection()
		{
			MySqlConnection oc=new MySqlConnection(ConnectionString);
			try
			{
				oc.Open();
			}
			catch(Exception e)
			{
				MessageBox.Show(e.Message.ToString());
				Application.Exit();
			}
			
			return oc;
		}
		/*
		public static void GetStyle(ScreenText st,int Scale,out Font font,out Brush brush,out Brush brushShadow,out StringFormat sf)
		{
			StyleDetailDataRow dr = st.m_drStyleDetail;

			FontStyle style = FontStyle.Regular;
			if( dr.fBold )
				style |= FontStyle.Bold;
			if( dr.fItalic )
				style |= FontStyle.Italic;
			if( dr.fUnderline )
				style |= FontStyle.Underline;

			font = new Font(dr.fFont, dr.fPixelHeight * Scale * HADataConfiguration.iZoom / 100000,
				style,GraphicsUnit.Pixel);

			Color c = Color.FromArgb(dr.fColor);
			brush = new SolidBrush(c);

			if( dr.fShadowColorIsNull )
				brushShadow=null;
			else
			{
				c = Color.FromArgb(dr.fShadowColor);
				brushShadow = new SolidBrush(c);
			}

			int HorizAlignment = 0, VertAlignment = 0;
			if( st.m_drDesignObjDetail_Text_Skin_Language!=null && !st.m_drDesignObjDetail_Text_Skin_Language.fFK_HorizAlignmentIsNull )
				HorizAlignment = st.m_drDesignObjDetail_Text_Skin_Language.fFK_HorizAlignment;
			else if( st.m_drDesignObjHeader_Text_Skin_Language!=null && !st.m_drDesignObjHeader_Text_Skin_Language.fFK_HorizAlignmentIsNull )
				HorizAlignment = st.m_drDesignObjHeader_Text_Skin_Language.fFK_HorizAlignment;
			else
				HorizAlignment = st.m_drStyleDetail.fFK_HorizAlignment;

			if( st.m_drDesignObjDetail_Text_Skin_Language!=null && !st.m_drDesignObjDetail_Text_Skin_Language.fFK_VertAlignmentIsNull )
				VertAlignment = st.m_drDesignObjDetail_Text_Skin_Language.fFK_VertAlignment;
			else if( st.m_drDesignObjHeader_Text_Skin_Language!=null && !st.m_drDesignObjHeader_Text_Skin_Language.fFK_VertAlignmentIsNull )
				VertAlignment = st.m_drDesignObjHeader_Text_Skin_Language.fFK_VertAlignment;
			else
				VertAlignment = st.m_drStyleDetail.fFK_VertAlignment;

			sf = new StringFormat();

			if( HorizAlignment == HorizAlignmentData.LEFT_CONST )
				sf.Alignment = StringAlignment.Near;
			else if( HorizAlignment == HorizAlignmentData.CENTER_CONST )
				sf.Alignment = StringAlignment.Center;
			else if( HorizAlignment == HorizAlignmentData.RIGHT_CONST)
				sf.Alignment = StringAlignment.Far;

			if( VertAlignment == VertAlignmentData.TOP_CONST)
				sf.LineAlignment = StringAlignment.Near;
			else if( VertAlignment == VertAlignmentData.MIDDLE_CONST)
				sf.LineAlignment = StringAlignment.Center;
			else if( VertAlignment == VertAlignmentData.BOTTOM_CONST)
				sf.LineAlignment = StringAlignment.Far;

		}
*/
		public static object GetParm(DesignObjVariationDataRow dr,int PK_DesignObjParameter,Hashtable htVariable)
		{
			bool bV=false;
			return GetParm(dr,PK_DesignObjParameter,htVariable,ref bV);
		}

		public static object GetParm(DesignObjVariationDataRow dr,int PK_DesignObjParameter,Hashtable htVariable,ref bool ContainsVariable)
		{
			object oValue=null;

			try
			{
				DesignObjVariation_DesignObjParameterDataRow drODP = m_mdsCache.tDesignObjVariation_DesignObjParameter[dr.fPK_DesignObjVariation,PK_DesignObjParameter];
				return drODP.fValue;

			}
			catch(Exception) {}

			return oValue;
		}
/*
		public static object GetCommandParm(ScreenCommand sa,int PK_CommandParameter)
		{
			if( sa.m_drDesignObjDetail_Command!=null )
			{
				DesignObjDetail_Command_CommandParameterDataRow dr = HADataConfiguration.m_mdsCache.tDesignObjDetail_Command_CommandParameter[sa.m_drDesignObjDetail_Command.fPK_DesignObjDetail_Command,PK_CommandParameter];
				return dr.bIsValid ? dr.fValue : null;
			}
			else if( sa.m_drDesignObjHeader_Command!=null )
			{
				DesignObjHeader_Command_CommandParameterDataRow dr = HADataConfiguration.m_mdsCache.tDesignObjHeader_Command_CommandParameter[sa.m_drDesignObjHeader_Command.fPK_DesignObjHeader_Command,PK_CommandParameter];
				return dr.bIsValid ? dr.fValue : null;
			}

			Debug.Assert(false);  // I don't think this will happen
			return null;
		}
*/
		public static string GetFileName(int PK_Skin,string FileName)
		{
			if( FileName.Length<2 )
				return FileName;

			if( FileName.StartsWith(@"\") || FileName.Substring(1,1)==":" )
				return FileName; // It's already fully qualified

			string sFullName;

			SkinDataRow drSkin = m_mdsCache.tSkin[PK_Skin];
			if( !drSkin.fDataSubdirectoryIsNull )
			{
				sFullName = DataPath + @"\" + drSkin.fDataSubdirectory + @"\" + FileName;
				return sFullName;
			}

			sFullName = DataPath + @"\" + FileName;
			return sFullName;
		}

		public static void GetDetaults()
		{
			RegistryKey rkCompany = Registry.CurrentUser.OpenSubKey(HADataConfiguration.Company,true);
			if( rkCompany==null )
				rkCompany = Registry.CurrentUser.CreateSubKey(HADataConfiguration.Company);

			rkApp = rkCompany.OpenSubKey(HADataConfiguration.AppName,true);
			if( rkApp==null )
				rkApp = rkCompany.CreateSubKey(HADataConfiguration.AppName);

			string connect = (string) rkApp.GetValue("ConnectString");
			if( connect==null )
				rkApp.SetValue("ConnectString",HADataConfiguration.ConnectionString);
			else
				HADataConfiguration.ConnectionString=connect;

			try { iFK_System = Convert.ToInt32(rkApp.GetValue("iFK_System")); } 
			catch(Exception) {}
			try { iFK_Installation = Convert.ToInt32(rkApp.GetValue("iFK_Installation")); } 
			catch(Exception) {}
			try { iFK_Controller = Convert.ToInt32(rkApp.GetValue("iFK_Controller")); } 
			catch(Exception) {}
			try { iFK_Skin = Convert.ToInt32(rkApp.GetValue("iFK_Skin")); } 
			catch(Exception) {}
			try { iFK_Size = Convert.ToInt32(rkApp.GetValue("iFK_Size")); } 
			catch(Exception) {}
			try { iFK_Version = Convert.ToInt32(rkApp.GetValue("iFK_Version")); } 
			catch(Exception) {}
			try { iFK_Language = Convert.ToInt32(rkApp.GetValue("iFK_Language")); } 
			catch(Exception) {}
			try { iFK_Style = Convert.ToInt32(rkApp.GetValue("iStyle")); } 
			catch(Exception) {}
			if( iFK_Style==0 )
				iFK_Style=1;
			try { iZoom = Convert.ToInt32(rkApp.GetValue("iZoom")); } 
			catch(Exception) {}
			if( iZoom==0 )
				iZoom=100;
			try 
			{ 
				object oColors = rkApp.GetValue("CustomColors");
				if( oColors!=null && oColors.ToString().Length>0 )
				{
					char[] sc={'|'};
					string[] sColors = oColors.ToString().Split(sc);
					iCustomColors = new int[sColors.Length];
					for(int i=0;i<sColors.Length;i++)
						iCustomColors[i]=Convert.ToInt32(sColors[i]);
				}
			} 
			catch(Exception) {}
		}

		public static void SaveDefaults()
		{
			RegistryKey rkCompany = Registry.CurrentUser.OpenSubKey(HADataConfiguration.Company,true);
			if( rkCompany==null )
				rkCompany = Registry.CurrentUser.CreateSubKey(HADataConfiguration.Company);

			RegistryKey rkApp = rkCompany.OpenSubKey(HADataConfiguration.AppName,true);
			if( rkApp==null )
				rkApp = rkCompany.CreateSubKey(HADataConfiguration.AppName);

			rkApp.SetValue("ConnectString",HADataConfiguration.ConnectionString);

			rkApp.SetValue("iFK_System",iFK_System);
			rkApp.SetValue("iFK_Installation",iFK_Installation);
			rkApp.SetValue("iFK_Controller",iFK_Controller);
			rkApp.SetValue("iFK_Skin",iFK_Skin);
			rkApp.SetValue("iFK_Size",iFK_Size);
			rkApp.SetValue("iFK_Version",iFK_Version);
			rkApp.SetValue("iFK_Language",iFK_Language);
			rkApp.SetValue("iZoom",iZoom);
			if( iCustomColors!=null )
			{
				string sOutput="";
				for(int i=0;i<iCustomColors.Length;i++)
					sOutput+=iCustomColors[i].ToString()+"|";
				rkApp.SetValue("CustomColors",sOutput);

			}
		}
		public string m_ConnectionString { get { return ConnectionString; } set { ConnectionString=value; } }
		public string m_Company { get { return Company; } set { Company=value; } }
		public string m_AppName { get { return AppName; } set { AppName=value; } }
		public string m_DataPath { get { return DataPath; } set { DataPath=value; } }
		public int m_iFK_System { get { return iFK_System; } set { iFK_System=value; } }
		public int m_iFK_Installation{ get { return iFK_Installation; } set { iFK_Installation=value; } }
		public int m_iFK_Controller { get { return iFK_Controller; } set { iFK_Controller=value; } }
		public int m_iFK_Skin { get { return iFK_Skin; } set { iFK_Skin=value; } }
		public int m_iFK_Size { get { return iFK_Size; } set { iFK_Size=value; } }
		public int m_iFK_Version { get { return iFK_Version; } set { iFK_Version=value; } }
		public int m_iFK_Language { get { return iFK_Language; } set { iFK_Language=value; } }
		public int m_iZoom { get { return iZoom; } set { iZoom=value; } }
	} 
}
