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
	/// Summary description for ChooseCommand.
	/// </summary>
	public class ChooseCommand : System.Windows.Forms.Form
	{

		private MDIForm MainForm 
		{
			get	{return (MDIForm)this.MdiParent;}
		}
		private int id_Command_Selected = 0;
		//private int id_Category_Selected = 0;


		private System.Windows.Forms.Button btnCancel;
		private System.Windows.Forms.Button btnChoose;
		private System.Windows.Forms.TreeView tvCommand;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public ChooseCommand()
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
			this.btnCancel = new System.Windows.Forms.Button();
			this.btnChoose = new System.Windows.Forms.Button();
			this.tvCommand = new System.Windows.Forms.TreeView();
			this.SuspendLayout();
			// 
			// btnCancel
			// 
			this.btnCancel.Location = new System.Drawing.Point(16, 424);
			this.btnCancel.Name = "btnCancel";
			this.btnCancel.TabIndex = 6;
			this.btnCancel.Text = "Cancel";
			this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
			// 
			// btnChoose
			// 
			this.btnChoose.Enabled = false;
			this.btnChoose.Location = new System.Drawing.Point(112, 424);
			this.btnChoose.Name = "btnChoose";
			this.btnChoose.Size = new System.Drawing.Size(120, 23);
			this.btnChoose.TabIndex = 5;
			this.btnChoose.Text = "Select";
			this.btnChoose.Click += new System.EventHandler(this.btnChoose_Click);
			// 
			// tvCommand
			// 
			this.tvCommand.AllowDrop = true;
			this.tvCommand.Dock = System.Windows.Forms.DockStyle.Top;
			this.tvCommand.ImageIndex = -1;
			this.tvCommand.Name = "tvCommand";
			this.tvCommand.SelectedImageIndex = -1;
			this.tvCommand.Size = new System.Drawing.Size(248, 416);
			this.tvCommand.TabIndex = 4;
			this.tvCommand.DoubleClick += new System.EventHandler(this.tvCommand_DoubleClick);
			this.tvCommand.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.tvAfterSelect);
			// 
			// ChooseCommand
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(248, 454);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.btnCancel,
																		  this.btnChoose,
																		  this.tvCommand});
			this.Name = "ChooseCommand";
			this.Text = "ChooseCommand";
			this.Load += new System.EventHandler(this.ChooseCommand_Load);
			this.ResumeLayout(false);

		}
		#endregion


		private void ChooseCommand_Load(object sender, System.EventArgs e)
		{
			CommandTree.CreateTree(this.tvCommand);
		}

		private void btnCancel_Click(object sender, System.EventArgs e)
		{
			this.Close();
		}

		private void btnChoose_Click(object sender, System.EventArgs e)
		{
			DesignObjDesigner owner = (DesignObjDesigner) this.Owner;
			owner.ChooseCommand(this.id_Command_Selected);
			this.Close();
		}

		private void tvCommand_DoubleClick(object sender, System.EventArgs e)
		{
			if (this.btnChoose.Enabled) this.btnChoose_Click(sender,null);
		}

		private void tvAfterSelect(object sender, System.Windows.Forms.TreeViewEventArgs e)
		{
			CommandDataRow odr = null;
			CommandCategoryDataRow ocdr = null;

			bool isCommand = false;
			//bool isCategory = false;
			
			CommandTreeNode tn = (CommandTreeNode) tvCommand.SelectedNode;
			
			if ( tn!=null )
			{
				odr = tn.CommandDataRow;
				if( odr==null ) // not Command, check if category (& get category row)
				{
					ocdr = tn.CommandCategoryDataRow;
					//if (ocdr!=null) isCategory = true;
				}
				else isCommand = true; // odr !null => is Command
			}

			if (isCommand)
			{
				this.btnChoose.Enabled = true;
				this.id_Command_Selected = odr.fPK_Command;
			}
			else
			{
				this.btnChoose.Enabled = false;
				this.id_Command_Selected = 0;
			}


		}
	}
}
