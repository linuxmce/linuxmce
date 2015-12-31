using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

using HAData.DataAccess;
using HAData.Common.TreeBuilder;


namespace HADesigner
{
	/// <summary>
	/// Summary description for ChooseDesignObj.
	/// </summary>
	public class ChooseDesignObj : System.Windows.Forms.Form
	{


		private MDIForm MainForm 
		{
			get	{return (MDIForm)this.MdiParent;}
		}
		private int id_DesignObj_Selected = 0;

		private System.Windows.Forms.TreeView tvDesignObj;
		private System.Windows.Forms.Button btnChoose;
		private System.Windows.Forms.Button btnCancel;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public ChooseDesignObj()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
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
			this.tvDesignObj = new System.Windows.Forms.TreeView();
			this.btnChoose = new System.Windows.Forms.Button();
			this.btnCancel = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// tvDesignObj
			// 
			this.tvDesignObj.AllowDrop = true;
			this.tvDesignObj.Dock = System.Windows.Forms.DockStyle.Top;
			this.tvDesignObj.ImageIndex = -1;
			this.tvDesignObj.Name = "tvDesignObj";
			this.tvDesignObj.SelectedImageIndex = -1;
			this.tvDesignObj.Size = new System.Drawing.Size(248, 416);
			this.tvDesignObj.TabIndex = 1;
			this.tvDesignObj.DoubleClick += new System.EventHandler(this.tvDesignObj_DoubleClick);
			this.tvDesignObj.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.tvAfterSelect);
			// 
			// btnChoose
			// 
			this.btnChoose.Enabled = false;
			this.btnChoose.Location = new System.Drawing.Point(112, 424);
			this.btnChoose.Name = "btnChoose";
			this.btnChoose.Size = new System.Drawing.Size(120, 23);
			this.btnChoose.TabIndex = 2;
			this.btnChoose.Text = "Select";
			this.btnChoose.Click += new System.EventHandler(this.btnChoose_Click);
			// 
			// btnCancel
			// 
			this.btnCancel.Location = new System.Drawing.Point(16, 424);
			this.btnCancel.Name = "btnCancel";
			this.btnCancel.TabIndex = 3;
			this.btnCancel.Text = "Cancel";
			this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
			// 
			// ChooseDesignObj
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(248, 454);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.btnCancel,
																		  this.btnChoose,
																		  this.tvDesignObj});
			this.Name = "ChooseDesignObj";
			this.Text = "Choose DesignObj";
			this.Load += new System.EventHandler(this.ChooseDesignObj_Load);
			this.ResumeLayout(false);

		}
		#endregion

		private void ChooseDesignObj_Load(object sender, System.EventArgs e)
		{
			DesignObjTree.CreateTree(this.tvDesignObj);
		}

		private void btnCancel_Click(object sender, System.EventArgs e)
		{
			this.Close();
		}

		private void btnChoose_Click(object sender, System.EventArgs e)
		{
			//MainForm.newDesignObjDesignerWindow(this.id_DesignObj_Selected);
			//MessageBox.Show(Convert.ToString(this.id_DesignObj_Selected));

			DesignObjDesigner owner = (DesignObjDesigner) this.Owner;
			owner.ChooseGotoDesignObj(this.id_DesignObj_Selected);
			this.Close();

		}

		private void tvDesignObj_DoubleClick(object sender, System.EventArgs e)
		{
            if (this.btnChoose.Enabled) this.btnChoose_Click(sender,null);		
		}



		private void tvAfterSelect(object sender, System.Windows.Forms.TreeViewEventArgs e)
		{
			DesignObjDataRow odr = null;
			DesignObjCategoryDataRow ocdr = null;

			bool isDesignObj = false;
			//bool isCategory = false;
			
			DesignObjTreeNode tn = (DesignObjTreeNode) tvDesignObj.SelectedNode;
			
			if ( tn!=null )
			{
				odr = tn.DesignObjDataRow;
				if( odr==null ) // not object, check if category (& get category row)
				{
					ocdr = tn.DesignObjCategoryDataRow;
					//if (ocdr!=null) isCategory = true;
				}
				else isDesignObj = true; // odr !null => is object
			}

			if (isDesignObj)
			{
				this.btnChoose.Enabled = true;
				this.id_DesignObj_Selected = odr.fPK_DesignObj;
			}
			else
			{
				this.btnChoose.Enabled = false;
				this.id_DesignObj_Selected = 0;
			}


		}
	}
}
