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
	/// Summary description for ChooseCommandGroup.
	/// </summary>
	public class ChooseCommandGroup : System.Windows.Forms.Form
	{

		private MDIForm MainForm 
		{
			get	{return (MDIForm)this.MdiParent;}
		}
		private int id_CommandGroup_Selected = 0;
		private System.Windows.Forms.Button btnChoose;
		private System.Windows.Forms.Button btnCancel;
		private System.Windows.Forms.TreeView tvCommandGroup;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public ChooseCommandGroup()
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
			this.tvCommandGroup = new System.Windows.Forms.TreeView();
			this.btnChoose = new System.Windows.Forms.Button();
			this.btnCancel = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// tvCommandGroup
			// 
			this.tvCommandGroup.AllowDrop = true;
			this.tvCommandGroup.Dock = System.Windows.Forms.DockStyle.Top;
			this.tvCommandGroup.ImageIndex = -1;
			this.tvCommandGroup.Name = "tvCommandGroup";
			this.tvCommandGroup.SelectedImageIndex = -1;
			this.tvCommandGroup.Size = new System.Drawing.Size(248, 416);
			this.tvCommandGroup.TabIndex = 1;
			this.tvCommandGroup.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.tvAfterSelect);
			// 
			// btnChoose
			// 
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
			// ChooseCommandGroup
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(248, 454);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.btnCancel,
																		  this.btnChoose,
																		  this.tvCommandGroup});
			this.Name = "ChooseCommandGroup";
			this.Text = "ChooseCommandGroup";
			this.ResumeLayout(false);

		}
		#endregion


		
		private void ChooseCommandGroup_Load(object sender, System.EventArgs e)
		{
			CommandGroupTree.CreateTree(this.tvCommandGroup);
		}

		private void btnCancel_Click(object sender, System.EventArgs e)
		{
			this.Close();
		}

		private void btnChoose_Click(object sender, System.EventArgs e)
		{


			DesignObjDesigner owner = (DesignObjDesigner) this.Owner;
			owner.ChooseCommandGroup(this.id_CommandGroup_Selected);
			this.Close();

		}



		private void tvAfterSelect(object sender, System.Windows.Forms.TreeViewEventArgs e)
		{
			CommandGroup_DDataRow odr = null;
			CommandGroupCategoryDataRow ocdr = null;

			bool isCommandGroup = false;
			//bool isCategory = false;
			
			CommandGroupTreeNode tn = (CommandGroupTreeNode) tvCommandGroup.SelectedNode;
			
			if ( tn!=null )
			{
				odr = tn.CommandGroup_DDataRow;
				if( odr==null ) // not CommandGroup, check if category (& get category row)
				{
					ocdr = tn.CommandGroupCategoryDataRow;
					//if (ocdr!=null) isCategory = true;
				}
				else isCommandGroup = true; // odr !null => is CommandGroup
			}

			if (isCommandGroup)
			{
				this.btnChoose.Enabled = true;
				this.id_CommandGroup_Selected = odr.fPK_CommandGroup_D;
			}
			else
			{
				this.btnChoose.Enabled = false;
				this.id_CommandGroup_Selected = 0;
			}


		}



	}
}
