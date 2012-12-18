using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

using System.Data;
using HAData.Common;
using HAData.DataAccess;
using HAData.Common.TreeBuilder;

namespace HADesigner
{
	/// <summary>
	/// Summary description for ObjsBrowser2.
	/// </summary>
	public class ObjsBrowser2 : System.Windows.Forms.Form
	{
		private MDIForm MainForm 
		{
			get	{return (MDIForm)this.MdiParent;}
		}
		private MyDataSet mds
		{
			get	{return HADataConfiguration.m_mdsCache;}
		}

		public TreeView DesignObjTreeView
		{
			get {return this.tvDesignObj;}
		}
		public TreeView TextTreeView
		{
			get {return this.tvText;}
		}

		private int id_DesignObj_Selected = 0;
		private int id_Category_Selected = 0;
		private int id_Text_Selected = 0;
		private int id_TextCategory_Selected = 0;
		private System.Windows.Forms.Button btnLoadUIDesignObj;
		private System.Windows.Forms.TabControl tabControl1;
		private System.Windows.Forms.TabPage tpDesignObjs;
		private System.Windows.Forms.TabPage tpText;
		private System.Windows.Forms.TreeView tvDesignObj;
		private System.Windows.Forms.TreeView tvText;
		private System.Windows.Forms.ImageList imageList1;
		private System.Windows.Forms.Button btnDelete;
		private System.Windows.Forms.Button btnCreate;
		private System.Windows.Forms.Button btnNewText;
		private System.Windows.Forms.Button btnDeleteText;
		private System.Windows.Forms.Button btnLoadText;
		private System.Windows.Forms.ContextMenu cmDesignObjTree;
		private System.Windows.Forms.ContextMenu cmTextTree;
		private System.Windows.Forms.MenuItem miNewDesignObjCategory;
		private System.Windows.Forms.MenuItem miNewTextCategory;
		private System.Windows.Forms.MenuItem miEditDesignObjCategory;
		private System.Windows.Forms.MenuItem miEditTextCategory;
		private System.Windows.Forms.TextBox tbSelectByID;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.TextBox tbSelectTextByID;
		private System.Windows.Forms.MenuItem miNewDesignObj;
		private System.Windows.Forms.MenuItem miNewText;
		private System.Windows.Forms.FlowLayoutPanel panelMain_Obj, panelMain_Text;
		private System.Windows.Forms.FlowLayoutPanel panelQuick_Obj, panelQuick_Text;
		private System.ComponentModel.IContainer components;

		public ObjsBrowser2()
		{
			//
			// Required for Windows Form Designer support
			//

			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
		}

		private void objBrowser_Load(object sender, System.EventArgs e)
		{
			// this.tvDesignObj.Sorted = true;
			// this.tvText.Sorted = true;

			DesignObjTree.CreateTree(this.tvDesignObj);
			TextTree.CreateTree(this.tvText);
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
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
			this.components = new System.ComponentModel.Container();
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(ObjsBrowser2));
			this.btnLoadUIDesignObj = new System.Windows.Forms.Button();
			this.tabControl1 = new System.Windows.Forms.TabControl();
			this.tpDesignObjs = new System.Windows.Forms.TabPage();
			this.label1 = new System.Windows.Forms.Label();
			this.tbSelectByID = new System.Windows.Forms.TextBox();
			this.btnCreate = new System.Windows.Forms.Button();
			this.btnDelete = new System.Windows.Forms.Button();
			this.tvDesignObj = new System.Windows.Forms.TreeView();
			this.cmDesignObjTree = new System.Windows.Forms.ContextMenu();
			this.miNewDesignObjCategory = new System.Windows.Forms.MenuItem();
			this.miEditDesignObjCategory = new System.Windows.Forms.MenuItem();
			this.imageList1 = new System.Windows.Forms.ImageList(this.components);
			this.tpText = new System.Windows.Forms.TabPage();
			this.label2 = new System.Windows.Forms.Label();
			this.tbSelectTextByID = new System.Windows.Forms.TextBox();
			this.btnNewText = new System.Windows.Forms.Button();
			this.btnDeleteText = new System.Windows.Forms.Button();
			this.btnLoadText = new System.Windows.Forms.Button();
			this.tvText = new System.Windows.Forms.TreeView();
			this.cmTextTree = new System.Windows.Forms.ContextMenu();
			this.miNewTextCategory = new System.Windows.Forms.MenuItem();
			this.miEditTextCategory = new System.Windows.Forms.MenuItem();
			this.miNewDesignObj = new System.Windows.Forms.MenuItem();
			this.miNewText = new System.Windows.Forms.MenuItem();
			this.panelMain_Obj = new System.Windows.Forms.FlowLayoutPanel();
			this.panelMain_Text = new System.Windows.Forms.FlowLayoutPanel();
			this.panelQuick_Obj = new System.Windows.Forms.FlowLayoutPanel();
			this.panelQuick_Text = new System.Windows.Forms.FlowLayoutPanel();
			this.tabControl1.SuspendLayout();
			this.tpDesignObjs.SuspendLayout();
			this.tpText.SuspendLayout();
			this.SuspendLayout();
			// 
			// btnLoadUIDesignObj
			// 
			this.btnLoadUIDesignObj.Anchor = (System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left);
			this.btnLoadUIDesignObj.Enabled = false;
			this.btnLoadUIDesignObj.Location = new System.Drawing.Point(8, 320);
			this.btnLoadUIDesignObj.Name = "btnLoadUIDesignObj";
			this.btnLoadUIDesignObj.Size = new System.Drawing.Size(40, 23);
			this.btnLoadUIDesignObj.TabIndex = 1;
			this.btnLoadUIDesignObj.Text = "Load";
			this.btnLoadUIDesignObj.Click += new System.EventHandler(this.btnLoadUIDesignObj_Click);
			// 
			// tabControl1
			// 
			this.tabControl1.Controls.AddRange(new System.Windows.Forms.Control[] {
																					  this.tpDesignObjs,
																					  this.tpText});
			this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tabControl1.Name = "tabControl1";
			this.tabControl1.SelectedIndex = 0;
			this.tabControl1.Size = new System.Drawing.Size(232, 413);
			this.tabControl1.TabIndex = 2;
			// 
			// tpDesignObjs
			// 
			this.tpDesignObjs.Controls.AddRange(new System.Windows.Forms.Control[] {
				this.panelMain_Obj
			});
			this.panelMain_Obj.Controls.AddRange(new System.Windows.Forms.Control[] {
				this.tvDesignObj,
				this.panelQuick_Obj
			});
			this.panelQuick_Obj.Controls.AddRange(new System.Windows.Forms.Control[] {
				this.label1,
				this.tbSelectByID,
				this.btnLoadUIDesignObj,
				this.btnCreate,
				this.btnDelete
			});
			this.panelMain_Obj.FlowDirection = FlowDirection.TopDown;
			this.panelMain_Obj.Dock = DockStyle.Fill;
			this.panelQuick_Obj.FlowDirection = FlowDirection.LeftToRight;
			this.panelQuick_Obj.Dock = DockStyle.Bottom;
			
			this.tpDesignObjs.Location = new System.Drawing.Point(4, 22);
			this.tpDesignObjs.Name = "tpDesignObjs";
			this.tpDesignObjs.Size = new System.Drawing.Size(224, 387);
			this.tpDesignObjs.TabIndex = 0;
			this.tpDesignObjs.Text = "DesignObjs";
			this.tpDesignObjs.Dock = DockStyle.Fill;
			// 
			// label1
			// 
			this.label1.Anchor = (System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left);
			this.label1.Location = new System.Drawing.Point(64, 352);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(72, 24);
			this.label1.TabIndex = 5;
			this.label1.Text = "Select by ID:";
			this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// tbSelectByID
			// 
			this.tbSelectByID.Anchor = (System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left);
			//this.tbSelectByID.Location = new System.Drawing.Point(136, 352);
			this.tbSelectByID.Name = "tbSelectByID";
			this.tbSelectByID.Size = new System.Drawing.Size(72, 20);
			this.tbSelectByID.TabIndex = 4;
			this.tbSelectByID.Text = "";
			this.tbSelectByID.KeyDown += new System.Windows.Forms.KeyEventHandler(this.tbSelectByID_KeyDown);
			// 
			// btnCreate
			// 
			this.btnCreate.Anchor = (System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left);
			this.btnCreate.Enabled = false;
			this.btnCreate.Location = new System.Drawing.Point(144, 320);
			this.btnCreate.Name = "btnCreate";
			this.btnCreate.Size = new System.Drawing.Size(72, 23);
			this.btnCreate.TabIndex = 3;
			this.btnCreate.Tag = "";
			this.btnCreate.Text = "New DesignObj";
			this.btnCreate.Click += new System.EventHandler(this.Create_Clicked);
			// 
			// btnDelete
			// 
			this.btnDelete.Anchor = (System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left);
			this.btnDelete.Enabled = false;
			this.btnDelete.Location = new System.Drawing.Point(72, 320);
			this.btnDelete.Name = "btnDelete";
			this.btnDelete.Size = new System.Drawing.Size(48, 23);
			this.btnDelete.TabIndex = 2;
			this.btnDelete.Text = "Delete";
			this.btnDelete.Click += new System.EventHandler(this.Delete_Clicked);
			// 
			// tvDesignObj
			// 
			this.tvDesignObj.AllowDrop = true;
			this.tvDesignObj.Anchor = System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom
				| System.Windows.Forms.AnchorStyles.Left 
				| System.Windows.Forms.AnchorStyles.Right;
			this.tvDesignObj.ContextMenu = this.cmDesignObjTree;
			this.tvDesignObj.ImageList = this.imageList1;
			this.tvDesignObj.Location = new System.Drawing.Point(8, 8);
			this.tvDesignObj.Name = "tvDesignObj";
			this.tvDesignObj.Size = new System.Drawing.Size(208, 304);
			this.tvDesignObj.TabIndex = 0;
			this.tvDesignObj.KeyDown += new System.Windows.Forms.KeyEventHandler(this.tvDesignObj_KeyDown);
			this.tvDesignObj.MouseDown += new System.Windows.Forms.MouseEventHandler(this.tvDesignObj_MouseDown);
			this.tvDesignObj.DragOver += new System.Windows.Forms.DragEventHandler(this.tvDesignObj_DragOver);
			this.tvDesignObj.DoubleClick += new System.EventHandler(this.tvDoubleClick);
			this.tvDesignObj.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.tvAfterSelect);
			this.tvDesignObj.ItemDrag += new System.Windows.Forms.ItemDragEventHandler(this.tvDesignObj_ItemDrag);
			this.tvDesignObj.DragDrop += new System.Windows.Forms.DragEventHandler(this.tvDesignObj_DragDrop);
			// 
			// cmDesignObjTree
			// 
			this.cmDesignObjTree.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																						 this.miNewDesignObjCategory,
																						 this.miEditDesignObjCategory,
																						 this.miNewDesignObj});
			// 
			// miNewDesignObjCategory
			// 
			this.miNewDesignObjCategory.Index = 0;
			this.miNewDesignObjCategory.Text = "New Category";
			this.miNewDesignObjCategory.Click += new System.EventHandler(this.miNewDesignObjCategory_Click);
			// 
			// miEditDesignObjCategory
			// 
			this.miEditDesignObjCategory.Index = 1;
			this.miEditDesignObjCategory.Text = "Edit Category";
			this.miEditDesignObjCategory.Click += new System.EventHandler(this.miEditDesignObjCategory_Click);
			// 
			// imageList1
			// 
			this.imageList1.ColorDepth = System.Windows.Forms.ColorDepth.Depth8Bit;
			this.imageList1.ImageSize = new System.Drawing.Size(16, 16);
			this.imageList1.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
			this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
			// 
			// tpText
			// 
			this.tpText.Controls.AddRange(new System.Windows.Forms.Control[] {
				this.panelMain_Text
			});
			this.panelMain_Text.Controls.AddRange(new System.Windows.Forms.Control[] {
				this.tvText,
				this.panelQuick_Text
			});
			this.panelQuick_Text.Controls.AddRange(new System.Windows.Forms.Control[] {
				this.label2,
				this.tbSelectTextByID,
				this.btnLoadText,
				this.btnNewText,
				this.btnDeleteText
			});
			this.panelMain_Text.FlowDirection = FlowDirection.TopDown;
			this.panelQuick_Text.FlowDirection = FlowDirection.LeftToRight;
			this.panelQuick_Text.Dock = DockStyle.Bottom;
			this.panelMain_Text.Dock = DockStyle.Fill;

			this.tpText.Location = new System.Drawing.Point(4, 22);
			this.tpText.Name = "tpText";
			this.tpText.Size = new System.Drawing.Size(224, 387);
			this.tpText.TabIndex = 1;
			this.tpText.Text = "Text";
			// 
			// label2
			// 
			this.label2.Anchor = (System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left);
			this.label2.Location = new System.Drawing.Point(64, 352);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(72, 24);
			this.label2.TabIndex = 9;
			this.label2.Text = "Select by ID:";
			this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// tbSelectTextByID
			// 
			this.tbSelectTextByID.Anchor = (System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left);
			this.tbSelectTextByID.Location = new System.Drawing.Point(136, 352);
			this.tbSelectTextByID.Name = "tbSelectTextByID";
			this.tbSelectTextByID.Size = new System.Drawing.Size(72, 20);
			this.tbSelectTextByID.TabIndex = 8;
			this.tbSelectTextByID.Text = "";
			this.tbSelectTextByID.KeyDown += new System.Windows.Forms.KeyEventHandler(this.tbSelectTextByID_KeyDown);
			// 
			// btnNewText
			// 
			this.btnNewText.Anchor = (System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left);
			this.btnNewText.Enabled = false;
			this.btnNewText.Location = new System.Drawing.Point(144, 320);
			this.btnNewText.Name = "btnNewText";
			this.btnNewText.Size = new System.Drawing.Size(72, 23);
			this.btnNewText.TabIndex = 7;
			this.btnNewText.Text = "New Text";
			this.btnNewText.Click += new System.EventHandler(this.btnNewText_Click);
			// 
			// btnDeleteText
			// 
			this.btnDeleteText.Anchor = (System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left);
			this.btnDeleteText.Enabled = false;
			this.btnDeleteText.Location = new System.Drawing.Point(72, 320);
			this.btnDeleteText.Name = "btnDeleteText";
			this.btnDeleteText.Size = new System.Drawing.Size(48, 23);
			this.btnDeleteText.TabIndex = 6;
			this.btnDeleteText.Text = "Delete";
			this.btnDeleteText.Click += new System.EventHandler(this.btnDeleteText_Click);
			// 
			// btnLoadText
			// 
			this.btnLoadText.Anchor = (System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left);
			this.btnLoadText.Enabled = false;
			this.btnLoadText.Location = new System.Drawing.Point(8, 320);
			this.btnLoadText.Name = "btnLoadText";
			this.btnLoadText.Size = new System.Drawing.Size(40, 23);
			this.btnLoadText.TabIndex = 5;
			this.btnLoadText.Text = "Load";
			this.btnLoadText.Click += new System.EventHandler(this.btnLoadText_Click);
			// 
			// tvText
			// 
			this.tvText.AllowDrop = true;
			this.tvText.Anchor = (((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
				| System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right);
			this.tvText.ContextMenu = this.cmTextTree;
			this.tvText.ImageList = this.imageList1;
			this.tvText.Location = new System.Drawing.Point(8, 8);
			this.tvText.Name = "tvText";
			this.tvText.Size = new System.Drawing.Size(208, 304);
			this.tvText.TabIndex = 1;
			this.tvText.KeyDown += new System.Windows.Forms.KeyEventHandler(this.tvText_KeyDown);
			this.tvText.MouseDown += new System.Windows.Forms.MouseEventHandler(this.tvText_MouseDown);
			this.tvText.DragOver += new System.Windows.Forms.DragEventHandler(this.tvText_DragOver);
			this.tvText.DoubleClick += new System.EventHandler(this.tvText_DoubleClick);
			this.tvText.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.tvText_AfterSelect);
			this.tvText.ItemDrag += new System.Windows.Forms.ItemDragEventHandler(this.tvText_ItemDrag);
			this.tvText.DragDrop += new System.Windows.Forms.DragEventHandler(this.tvText_DragDrop);
			// 
			// cmTextTree
			// 
			this.cmTextTree.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					   this.miNewTextCategory,
																					   this.miEditTextCategory,
																					   this.miNewText});
			// 
			// miNewTextCategory
			// 
			this.miNewTextCategory.Index = 0;
			this.miNewTextCategory.Text = "New Category";
			this.miNewTextCategory.Click += new System.EventHandler(this.miNewTextCategory_Click);
			// 
			// miEditTextCategory
			// 
			this.miEditTextCategory.Index = 1;
			this.miEditTextCategory.Text = "Edit Category";
			this.miEditTextCategory.Click += new System.EventHandler(this.miEditTextCategory_Click);
			// 
			// miNewDesignObj
			// 
			this.miNewDesignObj.Enabled = false;
			this.miNewDesignObj.Index = 2;
			this.miNewDesignObj.Text = "New DesignObj";
			this.miNewDesignObj.Click += new System.EventHandler(this.miNewDesignObj_Click);
			// 
			// miNewText
			// 
			this.miNewText.Enabled = false;
			this.miNewText.Index = 2;
			this.miNewText.Text = "New Text";
			this.miNewText.Click += new System.EventHandler(this.miNewText_Click);
			// 
			// ObjsBrowser2
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(232, 413);
			this.ControlBox = false;
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.tabControl1});
			this.Name = "ObjsBrowser2";
			this.Text = "DesignObj Browser";
			this.Load += new System.EventHandler(this.objBrowser_Load);
			this.tabControl1.ResumeLayout(false);
			this.tpDesignObjs.ResumeLayout(false);
			this.tpText.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion

		private void btnLoadUIDesignObj_Click(object sender, System.EventArgs e)
		{
			MainForm.newDesignObjDesignerWindow(this.id_DesignObj_Selected, this.tvDesignObj.SelectedNode);
		}

		/// <summary>
		/// Insert a new object to the object browser tree.  DesignObj should already be in the database.
		/// </summary>
		
		public void refreshDesignObjTree()
		{
			this.tvDesignObj.Nodes.Clear();
			DesignObjTree.CreateTree(this.tvDesignObj);
		}
		
		private void tvAfterSelect(object sender, System.Windows.Forms.TreeViewEventArgs e) // could be null
		{
			DesignObjDataRow odr = null;
			DesignObjCategoryDataRow ocdr = null;

			bool isDesignObj = false;
			bool isCategory = false;
		
			DesignObjTreeNode tn = (DesignObjTreeNode) tvDesignObj.SelectedNode;
			
			if ( tn!=null )
			{
				odr = tn.DesignObjDataRow;
				ocdr = tn.DesignObjCategoryDataRow;

                if (odr!=null) isDesignObj = true;
				else if (ocdr!=null) isCategory = true;
			}

			this.btnLoadUIDesignObj.Enabled = isDesignObj;
			this.id_DesignObj_Selected = (isDesignObj) ? odr.fPK_DesignObj : 0;

			this.btnDelete.Enabled = (isDesignObj || (isCategory && tn.Nodes.Count == 0));

			this.btnCreate.Enabled = this.miNewDesignObj.Enabled = isCategory || isDesignObj;
			this.id_Category_Selected = (btnCreate.Enabled)? ocdr.fPK_DesignObjCategory : 0;
		}

		private void tvDoubleClick(object sender, System.EventArgs e)
		{
			if (this.btnLoadUIDesignObj.Enabled)
			{
                btnLoadUIDesignObj_Click(sender,null);
			}
		}

		private void Delete_Clicked(object sender, System.EventArgs e)
		{
			DesignObjTreeNode otn = (DesignObjTreeNode) this.tvDesignObj.SelectedNode;
			DesignObjDataRow drO = otn.DesignObjDataRow;
            
			if (drO == null) // empty category selected
			{
				DesignObjCategoryDataRow drOC = otn.DesignObjCategoryDataRow;
				string confirm = "Are you sure you want to delete \"" + drOC.fDescription + "\"?";
				if (MessageBox.Show(confirm,"Are You Sure?",MessageBoxButtons.YesNo) == DialogResult.Yes)
				{
					drOC.dr.Delete();
					mds.tDesignObjCategory.Update(1,mds.m_conn,mds.m_trans);
					otn.Remove();
					this.tvAfterSelect(sender,null); // find new selection
				}
			}
			else // object selected
			{
				DialogResult dr = MessageBox.Show("Are you sure you want to delete \"" + drO.fDescription + "\"","Delete",MessageBoxButtons.YesNo);
				if (dr == DialogResult.Yes)
				{
					UIDesignObj uiobject = new UIDesignObj(null, -1, drO.fPK_DesignObj, this.MainForm.GraphicsDirectory);
					uiobject.NeedsDelete = true;
					uiobject.SaveToDatabase();

					MainForm.removeDesignObjDesignerWindow(this.id_DesignObj_Selected); // Close window (if open) before removing tree node
					otn.Remove();
					this.tvAfterSelect(sender,null); // find new selection
				}
			}
		}

		private void miNewDesignObj_Click(object sender, System.EventArgs e)
		{
            this.Create_Clicked(sender,e);		
		}

		private void Create_Clicked(object sender, System.EventArgs e)
		{
			NewDesignObj createForm = new NewDesignObj(this.id_Category_Selected, this);
			createForm.ShowDialog();
		}

		/// <summary>
		/// Inserts new object to category of selected node in tree
		/// </summary>
		public void insertNewDesignObjToTree(int id_object)
		{
			DesignObjTreeNode selected = (DesignObjTreeNode) this.tvDesignObj.SelectedNode;
			if (selected.DesignObjDataRow!=null) this.tvDesignObj.SelectedNode = selected.Parent;
            DesignObjTree.InsertDesignObjToTree(this.tvDesignObj, id_object);
		}

		private void tvText_AfterSelect(object sender, System.Windows.Forms.TreeViewEventArgs e)
		{
			TextDataRow tdr = null;
			TextCategoryDataRow tcdr = null;

			bool isText = false;
			bool isCategory = false;

			TextTreeNode tn = (TextTreeNode) tvText.SelectedNode;
		
			if ( tn!=null )
			{
				tdr = tn.TextDataRow;
				tcdr = tn.TextCategoryDataRow;

				if( tdr!=null ) isText = true;
				else if (tcdr!=null) isCategory = true;
			}

			this.btnLoadText.Enabled = isText;
			this.id_Text_Selected = (isText) ? tdr.fPK_Text : 0;

			this.btnDeleteText.Enabled = (isText || (isCategory && tn.Nodes.Count == 0));
            
            this.btnNewText.Enabled = this.miNewText.Enabled = isCategory || isText;
            this.id_TextCategory_Selected = (this.btnNewText.Enabled) ? tcdr.fPK_TextCategory : 0;
		}

		private void tvText_DoubleClick(object sender, System.EventArgs e)
		{
			if (this.btnLoadText.Enabled) this.btnLoadText_Click(sender, e);
		}

		private void btnLoadText_Click(object sender, System.EventArgs e)
		{
			TextForm tf = new TextForm(this.id_Text_Selected);
			if (tf.ShowDialog() == DialogResult.OK && tf.UpdatedText) this.tvText.SelectedNode.Text = tf.TreeLabel;
		}

		private void miNewText_Click(object sender, System.EventArgs e)
		{
            this.btnNewText_Click(sender,e);		
		}

		private void btnNewText_Click(object sender, System.EventArgs e)
		{
			TextForm tf = new TextForm(this.id_TextCategory_Selected,true);
			if (tf.ShowDialog() == DialogResult.OK)
			{
				if (tf.TextID == -1) MessageBox.Show("Error inserting text to db");
				else 
				{
					TextTreeNode selected = (TextTreeNode)this.tvText.SelectedNode;
					if (selected.TextDataRow != null) this.tvText.SelectedNode = selected.Parent;
					TextTree.InsertDesignObjToTree(this.tvText, tf.TextID);
				}
			}
		}

		private void btnDeleteText_Click(object sender, System.EventArgs e)
		{
			TextTreeNode ttn = (TextTreeNode) this.tvText.SelectedNode;

			TextDataRow drT = ttn.TextDataRow;

			if (drT == null) // empty category selected
			{
				TextCategoryDataRow drTC = ttn.TextCategoryDataRow;
				string confirm = "Are you sure you want to delete \"" + drTC.fDescription + "\"?";
				if (MessageBox.Show(confirm,"Are You Sure?",MessageBoxButtons.YesNo) == DialogResult.Yes)
				{
					drTC.dr.Delete();
					mds.tTextCategory.Update(1,mds.m_conn,mds.m_trans);
					ttn.Remove();
					this.tvText_AfterSelect(sender, null); // find new selection
				}
			}
			else // Text Selected
			{
				// Get OVT Rows to delete
				DataRow[] drsOVT_delete = drT.dr.GetChildRows(MyDataSet.MyRelations.DesignObjVariation_Text_FK_Text);
				// Get OVT Rows to Set FK_Text_OverrideFromHeader to NULL
				DataRow[] drsOVT_setNull = drT.dr.GetChildRows(MyDataSet.MyRelations.DesignObjVariation_Text_FK_Text_OverrideFromHeader);
				// Get Corresponding DesignObj Rows
				ArrayList alOWarn = new ArrayList();
				foreach (DataRow drOVT_delete in drsOVT_delete)
				{
					DesignObjVariation_TextDataRow ovtdr = new DesignObjVariation_TextDataRow(drOVT_delete);
					alOWarn.Add(ovtdr.fFK_DesignObjVariation_DataRow.fFK_DesignObj_DataRow);
				}
				foreach (DataRow drOVT_setNull in drsOVT_setNull)
				{
					DesignObjVariation_TextDataRow ovtdr = new DesignObjVariation_TextDataRow(drOVT_setNull);
					alOWarn.Add(ovtdr.fFK_DesignObjVariation_DataRow.fFK_DesignObj_DataRow);
				}

				string warnList = "";
				foreach (DesignObjDataRow odr in alOWarn) // converting object to datarow ???
				{
					warnList += "\n" + odr.fDescription + " (" + odr.fPK_DesignObj + ")";
				}
				if (warnList=="") warnList = "\n(none)";

				string warn = "Are you sure you want to delete \"" + drT.fDescription + "\" ?\n\nThis will also remove it from the following objects:\n" + warnList;

				if (MessageBox.Show(warn,"Delete: ARE YOU SURE?",MessageBoxButtons.YesNo) == DialogResult.Yes)
				{
					bool needToClose = false;
					foreach (DesignObjDataRow odr in alOWarn)
					{
						if (MainForm.objectDesignerWindowOpen(odr.fPK_DesignObj)) needToClose = true;
					}

					if (needToClose)
					{
						MessageBox.Show("Please close designer windows for the following objects: " + warnList);
					}
					else // continue with delete, Delete rows from ovtsl, ovt, text_ls, & text
					{
						foreach (DataRow drOVT in drsOVT_delete)
						{
							DataRow[] drsOVTSL = drOVT.GetChildRows(MyDataSet.MyRelations.DesignObjVariation_Text_Skin_Language_FK_DesignObjVariation_Text);
							foreach (DataRow drOVTSL in drsOVTSL)
							{
								drOVTSL.Delete();
							}
							drOVT.Delete();
						}
						foreach (DataRow drOVT in drsOVT_setNull)
						{
							DesignObjVariation_TextDataRow odrOVT = new DesignObjVariation_TextDataRow(drOVT);
							odrOVT.fFK_Text_OverrideFromHeaderSetNull();
						}

						foreach (DataRow drL in mds.tLanguage.Select())
						{
							LanguageDataRow odrL = new LanguageDataRow(drL);
							if (drT.fDescription_LS(odrL.fPK_Language,false)!=null) mds.tText_LS[drT.fPK_Text,odrL.fPK_Language].dr.Delete();
						}
						drT.dr.Delete();
					
						mds.tDesignObjVariation_Text_Skin_Language.Update(1,mds.m_conn,mds.m_trans);
						mds.tDesignObjVariation_Text.Update(1,mds.m_conn,mds.m_trans);
						mds.tText_LS.Update(1,mds.m_conn,mds.m_trans);
						mds.tText.Update(1,mds.m_conn,mds.m_trans);

						this.tvText.SelectedNode.Remove();
						this.tvText_AfterSelect(sender,null); // find new selection
					}
				}
			}
		}

		private void tvDesignObj_ItemDrag(object sender, System.Windows.Forms.ItemDragEventArgs e)
		{
			DesignObjTreeNode ObjTN = (DesignObjTreeNode)e.Item;
			if(ObjTN.DesignObjDataRow != null) tvDesignObj.DoDragDrop(ObjTN,DragDropEffects.All);
		}

		private void tvText_ItemDrag(object sender, System.Windows.Forms.ItemDragEventArgs e)
		{
			TextTreeNode ttn = (TextTreeNode)e.Item;
			if (ttn.TextDataRow != null) tvText.DoDragDrop(ttn,DragDropEffects.All);
		}

		private void tvDesignObj_DragOver(object sender, System.Windows.Forms.DragEventArgs e)
		{
			if (e.Data.GetDataPresent("HAData.Common.TreeBuilder.DesignObjTreeNode"))
			{
				DesignObjTreeNode otnDrag = (DesignObjTreeNode) e.Data.GetData("HAData.Common.TreeBuilder.DesignObjTreeNode");
				DesignObjTreeNode otnOver = (DesignObjTreeNode) this.tvDesignObj.GetNodeAt(this.tvDesignObj.PointToClient(new Point(e.X,e.Y)));
				if (otnOver.DesignObjDataRow == null) // over category
				{
					if (otnOver == otnDrag.Parent) e.Effect = DragDropEffects.None;
					else 
					{
						this.tvDesignObj.SelectedNode = otnOver;
						e.Effect = DragDropEffects.Move;
					}
				}
				else // over object
				{
					if (otnOver.Parent == otnDrag.Parent) e.Effect = DragDropEffects.None;
					else 
					{
						this.tvDesignObj.SelectedNode = otnOver.Parent;
						e.Effect = DragDropEffects.Move;
					}
				}
			}
			else e.Effect = DragDropEffects.None;

			if (e.Effect==DragDropEffects.None) this.tvDesignObj.SelectedNode = null;
		}

		private void tvText_DragOver(object sender, System.Windows.Forms.DragEventArgs e)
		{
			if(e.Data.GetDataPresent("HAData.Common.TreeBuilder.TextTreeNode"))
			{
				TextTreeNode ttnDrag = (TextTreeNode) e.Data.GetData("HAData.Common.TreeBuilder.TextTreeNode");
				TextTreeNode ttnOver = (TextTreeNode) this.tvText.GetNodeAt(this.tvText.PointToClient(new Point(e.X,e.Y)));
				if (ttnOver.TextDataRow == null) // over category
				{
					if (ttnOver == ttnDrag.Parent) e.Effect = DragDropEffects.None;
					else
					{
						this.tvText.SelectedNode = ttnOver;
						e.Effect = DragDropEffects.Move;
					}
				}
				else // over text
				{
					if (ttnOver.Parent == ttnDrag.Parent) e.Effect = DragDropEffects.None;
					else
					{
						this.tvText.SelectedNode = ttnOver.Parent;
						e.Effect = DragDropEffects.Move;
					}
				}
			}
			else e.Effect = DragDropEffects.None;

			if (e.Effect==DragDropEffects.None) this.tvText.SelectedNode = null;
		}

		private void tvDesignObj_DragDrop(object sender, System.Windows.Forms.DragEventArgs e)
		{            
			if(e.Data.GetDataPresent("HAData.Common.TreeBuilder.DesignObjTreeNode") && e.Effect == DragDropEffects.Move)
			{
				DesignObjTreeNode otnDrag = (DesignObjTreeNode) e.Data.GetData("HAData.Common.TreeBuilder.DesignObjTreeNode");
				DesignObjTreeNode otnDrop = (DesignObjTreeNode) this.tvDesignObj.SelectedNode;

				if (otnDrop.DesignObjCategoryDataRow != null && otnDrop != otnDrag.Parent) // move in DB, move on tree
				{
					otnDrag.DesignObjDataRow.fFK_DesignObjCategory = otnDrop.DesignObjCategoryDataRow.fPK_DesignObjCategory;
					mds.tDesignObj.Update(1,mds.m_conn,mds.m_trans);

					DesignObjTreeNode otnNew = new DesignObjTreeNode(otnDrag.DesignObjDataRow,otnDrop.DesignObjCategoryDataRow,otnDrag.Text);
					otnDrop.Nodes.Add(otnNew);
					otnDrag.Remove();
                    this.tvDesignObj.SelectedNode = otnNew;
				}
			}
		}

		private void tvText_DragDrop(object sender, System.Windows.Forms.DragEventArgs e)
		{
			if(e.Data.GetDataPresent("HAData.Common.TreeBuilder.TextTreeNode") && e.Effect == DragDropEffects.Move)
			{
				TextTreeNode ttnDrag = (TextTreeNode) e.Data.GetData("HAData.Common.TreeBuilder.TextTreeNode");
				TextTreeNode ttnDrop = (TextTreeNode) this.tvText.SelectedNode;

				if (ttnDrop.TextCategoryDataRow != null && ttnDrop != ttnDrag.Parent)
				{
					ttnDrag.TextDataRow.fFK_TextCategory = ttnDrop.TextCategoryDataRow.fPK_TextCategory;
					mds.tText.Update(1,mds.m_conn,mds.m_trans);

					TextTreeNode ttnNew = new TextTreeNode(ttnDrag.TextDataRow,ttnDrop.TextCategoryDataRow,ttnDrag.Text);
					ttnDrop.Nodes.Add(ttnNew);
					ttnDrag.Remove();
					this.tvText.SelectedNode = ttnNew;
				}
			}
		}

		private void tvDesignObj_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			if (e.Button == MouseButtons.Right)
			{
				DesignObjTreeNode otnRightClick = (DesignObjTreeNode) this.tvDesignObj.GetNodeAt(e.X,e.Y);
                if (otnRightClick!=null && otnRightClick.DesignObjDataRow!=null) this.tvDesignObj.SelectedNode = otnRightClick.Parent;	
				else this.tvDesignObj.SelectedNode = otnRightClick;
				if (otnRightClick==null) this.tvAfterSelect(sender,null);
				this.miEditDesignObjCategory.Enabled = (this.tvDesignObj.SelectedNode!=null);
			}
		}

		private void tvText_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			if (e.Button == MouseButtons.Right)
			{
				TextTreeNode ttnRightClick = (TextTreeNode) this.tvText.GetNodeAt(e.X,e.Y);
				if (ttnRightClick!=null && ttnRightClick.TextDataRow!=null) this.tvText.SelectedNode = ttnRightClick.Parent;
				else this.tvText.SelectedNode = ttnRightClick;
				if (ttnRightClick==null) this.tvText_AfterSelect(sender, null);
				this.miEditTextCategory.Enabled = (this.tvText.SelectedNode!=null);
			}
		}

		private void miNewDesignObjCategory_Click(object sender, System.EventArgs e)
		{
			(new CategoryForm(this.tvDesignObj,CategoryMode.DesignObjTree)).ShowDialog();
		}

		private void miEditDesignObjCategory_Click(object sender, System.EventArgs e)
		{
            (new CategoryForm(this.tvDesignObj,CategoryMode.DesignObjTree,true)).ShowDialog();		
		}

		private void miNewTextCategory_Click(object sender, System.EventArgs e)
		{
			(new CategoryForm(this.tvText,CategoryMode.TextTree)).ShowDialog();
		}

		private void miEditTextCategory_Click(object sender, System.EventArgs e)
		{
            (new CategoryForm(this.tvText,CategoryMode.TextTree,true)).ShowDialog();
		}

		private void tvDesignObj_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
		{
            if (e.KeyCode == Keys.Delete && this.btnDelete.Enabled) this.Delete_Clicked(sender,null);
		}

		private void tvText_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
		{
            if (e.KeyCode == Keys.Delete && this.btnDeleteText.Enabled) this.btnDeleteText_Click(sender,null);		
		}

		private void tbSelectByID_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				int id = -1;
				try {id = Convert.ToInt32(this.tbSelectByID.Text);}
				catch {MessageBox.Show("Invalid ID Entered");}
				this.tvDesignObj.SelectedNode = DesignObjTree.GetNodeByID(this.tvDesignObj,id);
			}
		}

		private void tbSelectTextByID_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				int id = -1;
				try	{id = Convert.ToInt32(this.tbSelectTextByID.Text);}
				catch {MessageBox.Show("Invalid ID Entered");}
				this.tvText.SelectedNode = TextTree.GetNodeByID(this.tvText,id);
			}
		}
	}
}
