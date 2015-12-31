using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.Diagnostics;
using System.Data.SqlClient;
using System.IO;
using Microsoft.Win32;
using HAData.Common;
using HAData.DataAccess;


//using Designer.Dialogs;

/*
using ONECONTROL.Common;
using ONECONTROL.Common.TreeBuilder;
using ONECONTROL.DataAccess;
*/

namespace HADesigner
{
	/// <summary>
	/// Summary description for MDIForm.
	/// </summary>
	public class MDIForm : System.Windows.Forms.Form
	{
		private string strRegKey = "Software\\Pluto\\OrbiterDesigner";
		private string strDirectoryKey = "GraphicsDirectory";
		private string strConnectionKey = "ConnectionKey";

		private string graphicsDirectory;
		public string GraphicsDirectory
		{ 
			get {return this.graphicsDirectory;} 
		}

		private Hashtable htDesignObjDesignerForms = new Hashtable(); // to keep track of open object designer forms
		private ObjsBrowser2 browser;
		public ObjsBrowser2 Browser
		{
			get {return this.browser;}
		}

		private System.Windows.Forms.MainMenu mainMenu;
		private System.Windows.Forms.MenuItem menuItem1;
		private System.Windows.Forms.MenuItem menuItem8;
		private System.Windows.Forms.MenuItem menuItem9;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		private System.Windows.Forms.MenuItem cmNewInstallation;
		private System.Windows.Forms.MenuItem cmLoadInstallation;
		private System.Windows.Forms.MenuItem cmBuild;
		private System.Windows.Forms.MenuItem menuItem2;
		private System.Windows.Forms.MenuItem menuItem3;
		private System.Windows.Forms.MenuItem menuItem4;
		private System.Windows.Forms.MenuItem menuItem5;
		private System.Windows.Forms.MenuItem menuItem6;
		private System.Windows.Forms.MenuItem menuItem7;
		private System.Windows.Forms.MenuItem menuItem12;
		private System.Windows.Forms.MenuItem menuItem13;
		private System.Windows.Forms.MenuItem menuItem14;
		private System.Windows.Forms.MenuItem menuItem15;
		private System.Windows.Forms.MenuItem menuItem16;
		private System.Windows.Forms.MenuItem menuItem10;
		private System.Windows.Forms.MenuItem miColorPicker;
		private System.Windows.Forms.MenuItem mnuSkinsStyles;
		private System.Windows.Forms.MenuItem miSkins;
		private System.Windows.Forms.MenuItem miStyles;
		private System.Windows.Forms.MenuItem miOptions;

		// private int childCount = 0; never used

		//private DesignObjCategoryDataRow m_drDesignObjCategory=null;
		//private DesignObjDataRow m_drDesignObj=null;
		//private bool m_bDesignObjCut=false;
		//MyDataSet m_mds = null;

		public MDIForm()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//MainDataForm objMainDataForm = new MainDataForm();
			//objMainDataForm.MdiParent = this;
			//objMainDataForm.Show();

			//DocumentsTempl objDocumentsTempl = new DocumentsTempl();
			//objDocumentsTempl.MdiParent = this;
			//objDocumentsTempl.Show();
			//MessageBox.Show("Near: " + ((int)StringAlignment.Near).ToString() + "\nCenter: " + ((int)StringAlignment.Center).ToString() + "\nFar: " + ((int)StringAlignment.Far).ToString());
		}

		public void newDesignObjDesignerWindow(int intDesignObjID)
		{
            this.newDesignObjDesignerWindow(intDesignObjID, null);
		}

		public void newDesignObjDesignerWindow(int intDesignObjID, TreeNode tnDesignObj)
		{
			DesignObjDesigner od;
			if (this.htDesignObjDesignerForms.ContainsKey(intDesignObjID))
			{
				od = (DesignObjDesigner) this.htDesignObjDesignerForms[intDesignObjID];
				od.BringToFront();
			}
			else
			{
				this.htDesignObjDesignerForms[intDesignObjID] = new DesignObjDesigner(intDesignObjID, tnDesignObj);
				od = (DesignObjDesigner) this.htDesignObjDesignerForms[intDesignObjID];
				od.MdiParent = this;
				
				od.Show();
				od.Top = 0;
				od.Left = 0;
			}
		}

		public void removeDesignObjDesignerWindow(int intDesignObjID)
		{
			if (this.htDesignObjDesignerForms.ContainsKey(intDesignObjID))
			{
				DesignObjDesigner od = (DesignObjDesigner) this.htDesignObjDesignerForms[intDesignObjID];
				od.warnSaveChanges = false;
				od.Close();
				this.htDesignObjDesignerForms.Remove(intDesignObjID);
				od.Dispose();
			}
		}

		public void removedDesignObjDesignerWindow(int intDesignObjID)
		{
			DesignObjDesigner od = (DesignObjDesigner) this.htDesignObjDesignerForms[intDesignObjID];
			this.htDesignObjDesignerForms.Remove(intDesignObjID);
			od.Dispose();
		}

		public bool objectDesignerWindowOpen(int intDesignObjID)
		{
			return this.htDesignObjDesignerForms.ContainsKey(intDesignObjID);
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.mainMenu = new System.Windows.Forms.MainMenu();
			this.menuItem1 = new System.Windows.Forms.MenuItem();
			this.cmNewInstallation = new System.Windows.Forms.MenuItem();
			this.cmLoadInstallation = new System.Windows.Forms.MenuItem();
			this.cmBuild = new System.Windows.Forms.MenuItem();
			this.menuItem2 = new System.Windows.Forms.MenuItem();
			this.miOptions = new System.Windows.Forms.MenuItem();
			this.menuItem3 = new System.Windows.Forms.MenuItem();
			this.menuItem4 = new System.Windows.Forms.MenuItem();
			this.menuItem5 = new System.Windows.Forms.MenuItem();
			this.menuItem6 = new System.Windows.Forms.MenuItem();
			this.menuItem7 = new System.Windows.Forms.MenuItem();
			this.menuItem12 = new System.Windows.Forms.MenuItem();
			this.menuItem13 = new System.Windows.Forms.MenuItem();
			this.menuItem14 = new System.Windows.Forms.MenuItem();
			this.menuItem15 = new System.Windows.Forms.MenuItem();
			this.menuItem16 = new System.Windows.Forms.MenuItem();
			this.menuItem8 = new System.Windows.Forms.MenuItem();
			this.menuItem9 = new System.Windows.Forms.MenuItem();
			this.menuItem10 = new System.Windows.Forms.MenuItem();
			this.miColorPicker = new System.Windows.Forms.MenuItem();
			this.mnuSkinsStyles = new System.Windows.Forms.MenuItem();
			this.miSkins = new System.Windows.Forms.MenuItem();
			this.miStyles = new System.Windows.Forms.MenuItem();
			// 
			// mainMenu
			// 
			this.mainMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					 this.menuItem1,
																					 this.menuItem3,
																					 this.menuItem7,
																					 this.menuItem8,
																					 this.mnuSkinsStyles,
																					 this.menuItem10});
			// 
			// menuItem1
			// 
			this.menuItem1.DefaultItem = true;
			this.menuItem1.Index = 0;
			this.menuItem1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					  this.cmNewInstallation,
																					  this.cmLoadInstallation,
																					  this.cmBuild,
																					  this.menuItem2,
																					  this.miOptions});
			this.menuItem1.Text = "&File";
			// 
			// cmNewInstallation
			// 
			this.cmNewInstallation.Index = 0;
			this.cmNewInstallation.Text = "New Installation";
			this.cmNewInstallation.Click += new System.EventHandler(this.cmNewInstallation_Click);
			// 
			// cmLoadInstallation
			// 
			this.cmLoadInstallation.Index = 1;
			this.cmLoadInstallation.Text = "Load Installation";
			// 
			// cmBuild
			// 
			this.cmBuild.Index = 2;
			this.cmBuild.Text = "Build";
			// 
			// menuItem2
			// 
			this.menuItem2.Index = 3;
			this.menuItem2.Text = "Program";
			this.menuItem2.Click += new System.EventHandler(this.menuItem2_Click);
			// 
			// miOptions
			// 
			this.miOptions.Index = 4;
			this.miOptions.Text = "&Options";
			this.miOptions.Click += new System.EventHandler(this.miOptions_Click);
			// 
			// menuItem3
			// 
			this.menuItem3.Index = 1;
			this.menuItem3.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					  this.menuItem4,
																					  this.menuItem5,
																					  this.menuItem6});
			this.menuItem3.Text = "Edit";
			// 
			// menuItem4
			// 
			this.menuItem4.Index = 0;
			this.menuItem4.Text = "Cut";
			// 
			// menuItem5
			// 
			this.menuItem5.Index = 1;
			this.menuItem5.Text = "Copy";
			// 
			// menuItem6
			// 
			this.menuItem6.Index = 2;
			this.menuItem6.Text = "Paste";
			// 
			// menuItem7
			// 
			this.menuItem7.Index = 2;
			this.menuItem7.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					  this.menuItem12,
																					  this.menuItem13,
																					  this.menuItem14,
																					  this.menuItem15,
																					  this.menuItem16});
			this.menuItem7.Text = "Insert";
			// 
			// menuItem12
			// 
			this.menuItem12.Index = 0;
			this.menuItem12.Text = "Image";
			// 
			// menuItem13
			// 
			this.menuItem13.Index = 1;
			this.menuItem13.Text = "Image Button";
			// 
			// menuItem14
			// 
			this.menuItem14.Index = 2;
			this.menuItem14.Text = "Array DesignObj";
			// 
			// menuItem15
			// 
			this.menuItem15.Index = 3;
			this.menuItem15.Text = "Broadcast Video";
			// 
			// menuItem16
			// 
			this.menuItem16.Index = 4;
			this.menuItem16.Text = "Recorded Video";
			// 
			// menuItem8
			// 
			this.menuItem8.Index = 3;
			this.menuItem8.MdiList = true;
			this.menuItem8.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					  this.menuItem9});
			this.menuItem8.Text = "&Windows";
			// 
			// menuItem9
			// 
			this.menuItem9.Index = 0;
			this.menuItem9.Text = "Cascade";
			// 
			// menuItem10
			// 
			this.menuItem10.Index = 5;
			this.menuItem10.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					   this.miColorPicker});
			this.menuItem10.Text = "&Color";
			// 
			// miColorPicker
			// 
			this.miColorPicker.Index = 0;
			this.miColorPicker.Text = "&Color Picker";
			this.miColorPicker.Click += new System.EventHandler(this.miColorPicker_Click);
			// 
			// mnuSkinsStyles
			// 
			this.mnuSkinsStyles.Index = 4;
			this.mnuSkinsStyles.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																						   this.miSkins,
																						   this.miStyles});
			this.mnuSkinsStyles.Text = "&Skins && Styles";
			// 
			// miSkins
			// 
			this.miSkins.Index = 0;
			this.miSkins.Text = "S&kins";
			this.miSkins.Click += new System.EventHandler(this.miSkins_Click);
			// 
			// miStyles
			// 
			this.miStyles.Index = 1;
			this.miStyles.Text = "S&tyles";
			this.miStyles.Click += new System.EventHandler(this.miStyles_Click);
			// 
			// MDIForm
			// 
			this.AutoScale = false;
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(692, 416);
			this.IsMdiContainer = true;
			this.Menu = this.mainMenu;
			this.Name = "MDIForm";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "HA Designer";
			this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
			this.Load += new System.EventHandler(this.MDIForm_Load);

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new MDIForm());
		}

		private void menuItem11_Click(object sender, System.EventArgs e)
		{
			About aboutDialog = new About();
			aboutDialog.ShowDialog();
		}



		private void menuItem2_Click(object sender, System.EventArgs e)
		{
			//MainForm Form1Obj = new MainForm();
			//Form1Obj.Show();

		}

		private void LoadObjsBrowser()
		{
			//ObjsBrowser ObjsBrowserObj = new ObjsBrowser();
			//ObjsBrowserObj.MdiParent = this;
			//ObjsBrowserObj.Show();

			this.browser = new ObjsBrowser2();
			this.browser.MdiParent = this;

			this.browser.Show();
			this.browser.Dock = DockStyle.Right;
			

			// JAG - FOR TESTING
			//(new TestingForm()).Show();
			/*
			Testing testing = new Testing();
			testing.MdiParent = this;
			testing.Show();
			*/
		}

		private void MDIForm_Load(object sender, System.EventArgs e)
		{
			RegistryKey rk = Registry.CurrentUser.CreateSubKey(this.strRegKey);
		
			object oPath = rk.GetValue(this.strDirectoryKey); 
			while(oPath==null)
			{
				if ((new Options()).ShowDialog() == DialogResult.OK)
				{
					oPath = rk.GetValue(this.strDirectoryKey); 
				}
				else
				{
					Application.Exit();
					break; // VERY IMPORTANT !!!
				}
			}
			this.graphicsDirectory = oPath.ToString();

			object oConnection = rk.GetValue(this.strConnectionKey); 
			if( oConnection!=null )
				HADataConfiguration.ConnectionString = oConnection.ToString();
			//bool bConnected = false;
			
			/*
					if( ONECONTROLConfiguration.ConnectionString.Substring(1,2)==@":\" )
					{
						// This will throw an exception since it's looking for a sql server
						try
						{
							m_mds = new MyDataSet(null,null,false,false);
						}
						catch(Exception) {}

						m_mds.BuildTables();

						// We've got a fully qualified file name
						FileStream fs = new FileStream(ONECONTROLConfiguration.ConnectionString,FileMode.Open,FileAccess.Read);
						m_mds.Serialize(false,fs);
					}
					else
					*/



			HADataConfiguration.m_mdsCache = new MyDataSet(null,null,true,true);

			//HADataConfiguration.m_mdsCache.Relations.Add(MyDataSet..rText__Text_LS,m_mds.tText.cPK_Text,m_mds.tText_LS.cFK_Text,true);
/*
			HADataConfiguration.m_mdsCache.tDesignObjCategory.LoadAll(HADataConfiguration.m_mdsCache.m_conn,HADataConfiguration.m_mdsCache.m_trans);
			HADataConfiguration.m_mdsCache.tDesignObjType.LoadAll(HADataConfiguration.m_mdsCache.m_conn,HADataConfiguration.m_mdsCache.m_trans);
			HADataConfiguration.m_mdsCache.tDesignObj.LoadAll(HADataConfiguration.m_mdsCache.m_conn,HADataConfiguration.m_mdsCache.m_trans);
			HADataConfiguration.m_mdsCache.tText.LoadAll(HADataConfiguration.m_mdsCache.m_conn,HADataConfiguration.m_mdsCache.m_trans);
			HADataConfiguration.m_mdsCache.tTextCategory.LoadAll(HADataConfiguration.m_mdsCache.m_conn,HADataConfiguration.m_mdsCache.m_trans);
			HADataConfiguration.m_mdsCache.tDesignObjType_DesignObjParameter.LoadAll(HADataConfiguration.m_mdsCache.m_conn,HADataConfiguration.m_mdsCache.m_trans);
			HADataConfiguration.m_mdsCache.tCriteria_D.LoadAll(HADataConfiguration.m_mdsCache.m_conn,HADataConfiguration.m_mdsCache.m_trans);
			HADataConfiguration.m_mdsCache.tCriteriaParmNesting_D.LoadAll(HADataConfiguration.m_mdsCache.m_conn,HADataConfiguration.m_mdsCache.m_trans);
			HADataConfiguration.m_mdsCache.tCriteria_DList.LoadAll(HADataConfiguration.m_mdsCache.m_conn,HADataConfiguration.m_mdsCache.m_trans);
			HADataConfiguration.m_mdsCache.tCriteria_DParmList.LoadAll(HADataConfiguration.m_mdsCache.m_conn,HADataConfiguration.m_mdsCache.m_trans);
			HADataConfiguration.m_mdsCache.tButton.LoadAll(HADataConfiguration.m_mdsCache.m_conn,HADataConfiguration.m_mdsCache.m_trans);
			HADataConfiguration.m_mdsCache.tParameterType.LoadAll(HADataConfiguration.m_mdsCache.m_conn,HADataConfiguration.m_mdsCache.m_trans);
			HADataConfiguration.m_mdsCache.tDesignObjParameter.LoadAll(HADataConfiguration.m_mdsCache.m_conn,HADataConfiguration.m_mdsCache.m_trans);
			HADataConfiguration.m_mdsCache.tDesignObjType_DesignObjParameter.LoadAll(HADataConfiguration.m_mdsCache.m_conn,HADataConfiguration.m_mdsCache.m_trans);
			HADataConfiguration.m_mdsCache.tLanguage.LoadAll(HADataConfiguration.m_mdsCache.m_conn,HADataConfiguration.m_mdsCache.m_trans);
*/			
			HADataConfiguration.m_mdsCache.SetRelations();


			LoadObjsBrowser();
		}

		private void cmNewInstallation_Click(object sender, System.EventArgs e)
		{
		
		}

		private void miOptions_Click(object sender, System.EventArgs e)
		{
			RegistryKey rk = Registry.CurrentUser.CreateSubKey(this.strRegKey);
			object oPath = null;
			while(oPath==null)
			{
				DialogResult dr = (new Options()).ShowDialog();
				oPath = rk.GetValue(this.strDirectoryKey); 
				if (dr != DialogResult.OK && oPath==null)
				{
					Application.Exit();
					break; // VERY IMPORTANT !!!
				}
			}
			this.graphicsDirectory = oPath.ToString();
		}

		private void miColorPicker_Click(object sender, System.EventArgs e)
		{
			HADataConfiguration.GetDetaults();
			ColorDialog cd = new ColorDialog();
			cd.AllowFullOpen=true;
			cd.AnyColor=true;
			if( HADataConfiguration.iCustomColors!=null ) cd.CustomColors = HADataConfiguration.iCustomColors;
			cd.ShowDialog();
 
			if( cd.CustomColors!=null )
			{
				HADataConfiguration.iCustomColors = cd.CustomColors;
				HADataConfiguration.SaveDefaults();
			}
			Clipboard.SetDataObject(cd.Color.ToArgb().ToString(),true);
		}

		private void miSkins_Click(object sender, System.EventArgs e)
		{
            (new SkinsForm()).Show();
		}

		private void miStyles_Click(object sender, System.EventArgs e)
		{
			(new StylesForm()).Show();
		}
	}
}
