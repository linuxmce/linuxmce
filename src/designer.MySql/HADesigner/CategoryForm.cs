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
	/// Summary description for NewCategory.
	/// </summary>
	

	public enum CategoryMode{DesignObjTree,TextTree}
	
	public class CategoryForm : System.Windows.Forms.Form
	{
		bool existingCategory = false;
		CategoryMode mode;
		TreeView tree;
		private System.Windows.Forms.Button btnOK;
		private System.Windows.Forms.Button btnCancel;
		private System.Windows.Forms.Label lblNewCategory;
		private System.Windows.Forms.TextBox tbCategory;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		private MyDataSet mds
		{
			get	{return HADataConfiguration.m_mdsCache;}
		}


		public CategoryForm(TreeView tree, CategoryMode mode)
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			this.tree = tree;
			this.mode = mode;
		}
		public CategoryForm(TreeView tree, CategoryMode mode, bool existingCategory):this(tree,mode)
		{
			this.existingCategory = existingCategory;
			if (existingCategory)
			{
				this.Text = "Edit Category";
				this.tbCategory.Text = this.tree.SelectedNode.Text;
			}
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
			this.tbCategory = new System.Windows.Forms.TextBox();
			this.btnOK = new System.Windows.Forms.Button();
			this.btnCancel = new System.Windows.Forms.Button();
			this.lblNewCategory = new System.Windows.Forms.Label();
			this.SuspendLayout();
			// 
			// tbCategory
			// 
			this.tbCategory.Location = new System.Drawing.Point(120, 16);
			this.tbCategory.Name = "tbCategory";
			this.tbCategory.TabIndex = 0;
			this.tbCategory.Text = "";
			// 
			// btnOK
			// 
			this.btnOK.Location = new System.Drawing.Point(120, 56);
			this.btnOK.Name = "btnOK";
			this.btnOK.TabIndex = 1;
			this.btnOK.Text = "OK";
			this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
			// 
			// btnCancel
			// 
			this.btnCancel.Location = new System.Drawing.Point(32, 56);
			this.btnCancel.Name = "btnCancel";
			this.btnCancel.TabIndex = 2;
			this.btnCancel.Text = "Cancel";
			this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
			// 
			// lblNewCategory
			// 
			this.lblNewCategory.Location = new System.Drawing.Point(8, 16);
			this.lblNewCategory.Name = "lblNewCategory";
			this.lblNewCategory.TabIndex = 3;
			this.lblNewCategory.Text = "New Category:";
			// 
			// CategoryForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(232, 101);
			this.ControlBox = false;
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.lblNewCategory,
																		  this.btnCancel,
																		  this.btnOK,
																		  this.tbCategory});
			this.Name = "CategoryForm";
			this.Text = "New Category";
			this.ResumeLayout(false);

		}
		#endregion


		private void btnCancel_Click(object sender, System.EventArgs e)
		{
            this.DialogResult = DialogResult.Cancel;		
		}

		private bool showException(Exception e)
		{
			MessageBox.Show(e.ToString());
			return false;
		}

		private bool saveDBchanges()
		{
			if (this.mode == CategoryMode.DesignObjTree)
			{
				try {mds.tDesignObjCategory.Update(1,mds.m_conn,mds.m_trans);}
				catch (Exception e) {return this.showException(e);}
			}
			else if (this.mode == CategoryMode.TextTree)
			{
				try	{mds.tTextCategory.Update(1,mds.m_conn,mds.m_trans);}
				catch (Exception e) {return this.showException(e);}
			}
			return true;
		}

		private bool saveDesignObjCategory() // called only in DesignObjTree mode
		{
			DesignObjTreeNode otnSelected = (DesignObjTreeNode) this.tree.SelectedNode;
			if (this.existingCategory)
			{
				otnSelected.DesignObjCategoryDataRow.fDescription = this.tbCategory.Text;
				if (this.saveDBchanges()) otnSelected.Text = this.tbCategory.Text;
				else return false;
			}
			else
			{
				DesignObjCategoryDataRow drOC = new DesignObjCategoryDataRow(mds.tDesignObjCategory.NewRow());

				if (otnSelected!=null) drOC.fFK_DesignObjCategory_Parent = otnSelected.DesignObjCategoryDataRow.fPK_DesignObjCategory;
				drOC.fDescription = this.tbCategory.Text;
				mds.tDesignObjCategory.Rows.Add(drOC.dr);

				if (this.saveDBchanges())
				{
					DesignObjTreeNode newNode = new DesignObjTreeNode(null,drOC,drOC.fDescription);
					if (otnSelected==null) this.tree.Nodes.Add(newNode);
					else otnSelected.Nodes.Add(newNode);
					this.tree.SelectedNode = newNode;
				}
				else return false;
			}
			return true;
		}

		private bool saveTextCategory() // called only in TextTree mode
		{
			TextTreeNode ttnSelected = (TextTreeNode) this.tree.SelectedNode;
			if (this.existingCategory)
			{
				ttnSelected.TextCategoryDataRow.fDescription = this.tbCategory.Text;
				if (this.saveDBchanges()) ttnSelected.Text = this.tbCategory.Text;
				else return false;
			}
			else
			{
				TextCategoryDataRow drTC = new TextCategoryDataRow(mds.tTextCategory.NewRow());
			
				if (ttnSelected!=null) drTC.fFK_TextCategory_Parent = ttnSelected.TextCategoryDataRow.fPK_TextCategory;
				drTC.fDescription = this.tbCategory.Text;
				mds.tTextCategory.Rows.Add(drTC.dr);

				if (this.saveDBchanges())
				{
					TextTreeNode newNode = new TextTreeNode(null,drTC,drTC.fDescription);
					if (ttnSelected==null) this.tree.Nodes.Add(newNode);
					else ttnSelected.Nodes.Add(newNode);
					this.tree.SelectedNode = newNode;
				}
				else return false;
			}
			return true;
		}
        
		private void btnOK_Click(object sender, System.EventArgs e)
		{
			if (this.tbCategory.Text == "") MessageBox.Show("Please Enter a Description");
			else
			{
				switch (this.mode)
				{
					case CategoryMode.DesignObjTree:
						if (this.saveDesignObjCategory())
						{
							this.DialogResult = DialogResult.OK;
						}
						else MessageBox.Show("Error Saving DesignObj Category");
						break;
					case CategoryMode.TextTree:
						if (this.saveTextCategory()) 
						{
							this.DialogResult = DialogResult.OK;
						}
						else MessageBox.Show("Error Saving Text Category");
						break;
				}
			}
		}
	}
}
