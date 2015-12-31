using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

using HAData.Common;
using HAData.DataAccess;
using System.Data;


namespace HADesigner
{
	/// <summary>
	/// Summary description for SkinsForm.
	/// </summary>
	public class SkinsForm : System.Windows.Forms.Form
	{
		private MyDataSet mds
		{
			get {return HADataConfiguration.m_mdsCache;}
		}
		private StringPair spSkin
		{
			get {return (StringPair)this.cbSkin.SelectedItem;}
		}
		private StringPair spStyle
		{
			get {return (StringPair)this.cbStyle.SelectedItem;}
		}
		private Hashtable htStylePKToIndex = new Hashtable();
		private StringPair spStylePK(int pk)
		{
			return (StringPair)this.cbStyle.Items[(int)this.htStylePKToIndex[pk]];
		}

		private System.Windows.Forms.Button btnDelete;
		private System.Windows.Forms.Button btnUpdate;
		private System.Windows.Forms.CheckBox cbDrawTextBefore;
		private System.Windows.Forms.ComboBox cbSkin;
		private System.Windows.Forms.ComboBox cbStyle;
		private System.Windows.Forms.Label lblDescription;
		private System.Windows.Forms.Label lblSkin;
		private System.Windows.Forms.Label lblSubDirectory;
		private System.Windows.Forms.Label lblStyle;
		private System.Windows.Forms.TextBox tbDescription;
		private System.Windows.Forms.TextBox tbSubDirectory;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public SkinsForm()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			foreach (DataRow dr in mds.tSkin.Rows)
			{
				SkinDataRow drS = new SkinDataRow(dr);
				this.cbSkin.Items.Add(new StringPair(drS.fPK_Skin,drS.fDescription));
			}
			this.cbSkin.Items.Add(new StringPair(-1,"(new skin)"));

			int index = 0;
			this.cbStyle.Items.Add(new StringPair(-1,"No Style"));
			this.htStylePKToIndex[-1] = index++;
			foreach (DataRow dr in mds.tStyle.Rows)
			{
				StyleDataRow drS = new StyleDataRow(dr);
				this.cbStyle.Items.Add(new StringPair(drS.fPK_Style,drS.fDescription));
				this.htStylePKToIndex[drS.fPK_Style] = index++;
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
			this.btnDelete = new System.Windows.Forms.Button();
			this.btnUpdate = new System.Windows.Forms.Button();
			this.lblDescription = new System.Windows.Forms.Label();
			this.lblSkin = new System.Windows.Forms.Label();
			this.tbDescription = new System.Windows.Forms.TextBox();
			this.cbSkin = new System.Windows.Forms.ComboBox();
			this.lblSubDirectory = new System.Windows.Forms.Label();
			this.tbSubDirectory = new System.Windows.Forms.TextBox();
			this.lblStyle = new System.Windows.Forms.Label();
			this.cbStyle = new System.Windows.Forms.ComboBox();
			this.cbDrawTextBefore = new System.Windows.Forms.CheckBox();
			this.SuspendLayout();
			// 
			// btnDelete
			// 
			this.btnDelete.Enabled = false;
			this.btnDelete.Location = new System.Drawing.Point(64, 192);
			this.btnDelete.Name = "btnDelete";
			this.btnDelete.TabIndex = 6;
			this.btnDelete.Text = "Delete";
			this.btnDelete.Click += new System.EventHandler(this.btnDelete_Click);
			// 
			// btnUpdate
			// 
			this.btnUpdate.Enabled = false;
			this.btnUpdate.Location = new System.Drawing.Point(176, 192);
			this.btnUpdate.Name = "btnUpdate";
			this.btnUpdate.TabIndex = 5;
			this.btnUpdate.Text = "Update";
			this.btnUpdate.Click += new System.EventHandler(this.btnUpdate_Click);
			// 
			// lblDescription
			// 
			this.lblDescription.Location = new System.Drawing.Point(16, 56);
			this.lblDescription.Name = "lblDescription";
			this.lblDescription.TabIndex = 8;
			this.lblDescription.Text = "Description";
			this.lblDescription.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// lblSkin
			// 
			this.lblSkin.Location = new System.Drawing.Point(16, 16);
			this.lblSkin.Name = "lblSkin";
			this.lblSkin.TabIndex = 7;
			this.lblSkin.Text = "Skin";
			this.lblSkin.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// tbDescription
			// 
			this.tbDescription.Enabled = false;
			this.tbDescription.Location = new System.Drawing.Point(128, 56);
			this.tbDescription.Name = "tbDescription";
			this.tbDescription.Size = new System.Drawing.Size(120, 20);
			this.tbDescription.TabIndex = 1;
			this.tbDescription.Text = "";
			// 
			// cbSkin
			// 
			this.cbSkin.DisplayMember = "Description";
			this.cbSkin.Location = new System.Drawing.Point(128, 16);
			this.cbSkin.Name = "cbSkin";
			this.cbSkin.Size = new System.Drawing.Size(121, 21);
			this.cbSkin.TabIndex = 0;
			this.cbSkin.ValueMember = "ID";
			this.cbSkin.SelectedIndexChanged += new System.EventHandler(this.cbSkin_SelectedIndexChanged);
			// 
			// lblSubDirectory
			// 
			this.lblSubDirectory.Location = new System.Drawing.Point(16, 88);
			this.lblSubDirectory.Name = "lblSubDirectory";
			this.lblSubDirectory.TabIndex = 9;
			this.lblSubDirectory.Text = "Sub-Directory";
			this.lblSubDirectory.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// tbSubDirectory
			// 
			this.tbSubDirectory.Enabled = false;
			this.tbSubDirectory.Location = new System.Drawing.Point(128, 88);
			this.tbSubDirectory.Name = "tbSubDirectory";
			this.tbSubDirectory.Size = new System.Drawing.Size(120, 20);
			this.tbSubDirectory.TabIndex = 2;
			this.tbSubDirectory.Text = "";
			// 
			// lblStyle
			// 
			this.lblStyle.Location = new System.Drawing.Point(16, 120);
			this.lblStyle.Name = "lblStyle";
			this.lblStyle.TabIndex = 10;
			this.lblStyle.Text = "Style";
			this.lblStyle.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// cbStyle
			// 
			this.cbStyle.DisplayMember = "Description";
			this.cbStyle.Enabled = false;
			this.cbStyle.Location = new System.Drawing.Point(128, 120);
			this.cbStyle.Name = "cbStyle";
			this.cbStyle.Size = new System.Drawing.Size(121, 21);
			this.cbStyle.TabIndex = 3;
			this.cbStyle.ValueMember = "ID";
			// 
			// cbDrawTextBefore
			// 
			this.cbDrawTextBefore.Enabled = false;
			this.cbDrawTextBefore.Location = new System.Drawing.Point(128, 152);
			this.cbDrawTextBefore.Name = "cbDrawTextBefore";
			this.cbDrawTextBefore.Size = new System.Drawing.Size(152, 24);
			this.cbDrawTextBefore.TabIndex = 4;
			this.cbDrawTextBefore.Text = "DrawTextBeforeChildren";
			// 
			// SkinsForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(328, 245);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.cbDrawTextBefore,
																		  this.cbStyle,
																		  this.lblStyle,
																		  this.lblSubDirectory,
																		  this.tbSubDirectory,
																		  this.btnDelete,
																		  this.btnUpdate,
																		  this.lblDescription,
																		  this.lblSkin,
																		  this.tbDescription,
																		  this.cbSkin});
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "SkinsForm";
			this.Text = "SkinsForm";
			this.ResumeLayout(false);

		}
		#endregion

		private void cbSkin_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			if (this.spSkin.intID == -1)
			{
				this.tbDescription.Text = "";
				this.tbSubDirectory.Text = "";
				this.cbStyle.SelectedItem = this.spStylePK(-1);
				this.cbDrawTextBefore.Checked = false;
			}
			else
			{
				SkinDataRow sdr = mds.tSkin[this.spSkin.intID];
				this.tbDescription.Text = sdr.fDescription;
				this.tbSubDirectory.Text = sdr.fDataSubdirectory;
				this.cbDrawTextBefore.Checked = sdr.fDrawTextBeforeChildren;
				this.cbStyle.SelectedItem = (sdr.fFK_StyleIsNull)? this.spStylePK(-1): this.spStylePK(sdr.fFK_Style);

			}
			this.btnUpdate.Enabled = this.tbDescription.Enabled = this.tbSubDirectory.Enabled = this.cbStyle.Enabled = this.cbDrawTextBefore.Enabled = true;
			this.btnDelete.Enabled = (this.spSkin.intID != -1);
			this.btnUpdate.Text = (this.btnDelete.Enabled)? "Update": "Create";
		}

		private void setFields(SkinDataRow sdr)
		{
			sdr.fDescription = this.tbDescription.Text;
			sdr.fDataSubdirectory = this.tbSubDirectory.Text;
			sdr.fDrawTextBeforeChildren = this.cbDrawTextBefore.Checked;
				
			if (this.spStyle.intID == -1) sdr.fFK_StyleSetNull();
			else sdr.fFK_Style = spStyle.intID;
		}

		private void btnUpdate_Click(object sender, System.EventArgs e)
		{
			int skinIndex = this.cbSkin.SelectedIndex;

			SkinDataRow sdr = (spSkin.intID == -1)? new SkinDataRow(mds.tSkin.NewRow()) : mds.tSkin[spSkin.intID];
			this.setFields(sdr);

			if (spSkin.intID == -1) mds.tSkin.Rows.Add(sdr.dr);
			else this.cbSkin.Items.RemoveAt(skinIndex);

			mds.tSkin.Update(1,mds.m_conn,mds.m_trans);
			this.cbSkin.Items.Insert(skinIndex, new StringPair(sdr.fPK_Skin, sdr.fDescription));
			this.cbSkin.SelectedIndex = skinIndex;
		}

		private void btnDelete_Click(object sender, System.EventArgs e)
		{
			SkinDataRow sdr = mds.tSkin[this.spSkin.intID];
			if (sdr.dr.GetChildRows(MyDataSet.MyRelations.DesignObjVariation_Text_Skin_Language_FK_Skin).Length != 0)
			{
				MessageBox.Show("You Can't Delete \"" + this.spSkin.Description + "\" because there is Text associated with it");
			}
				/*
			else if (sdr.dr.GetChildRows(MyDataSet.MyRelations.Controller_FK_Skin).Length != 0)
			{
				MessageBox.Show("You Can't Delete \"" + this.spSkin.Description + "\" because there is a Controller associated with it");
			}
			*/
			else if (MessageBox.Show("Are you sure you want to delete \"" + this.spSkin.Description + "\"?","Are You Sure?",MessageBoxButtons.YesNo) == DialogResult.Yes)
			{
				foreach (DataRow dr in sdr.dr.GetChildRows(MyDataSet.MyRelations.Skin_FK_Skin_TextPlacement))
				{
					SkinDataRow other_sdr = new SkinDataRow(dr);
					other_sdr.fFK_Skin_TextPlacementSetNull();
				}
				sdr.dr.Delete();
				mds.tSkin.Update(1,mds.m_conn,mds.m_trans);
				this.cbSkin.Items.RemoveAt(this.cbSkin.SelectedIndex--);
			}
		}
	}
}
