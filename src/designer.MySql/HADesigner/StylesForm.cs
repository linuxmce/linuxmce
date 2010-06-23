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
	/// Summary description for StylesForm.
	/// </summary>
	public class StylesForm : System.Windows.Forms.Form
	{
		private MyDataSet mds
		{
			get {return HADataConfiguration.m_mdsCache;}
		}
		private StringPair spStyle
		{
			get {return (StringPair)this.cbStyle.SelectedItem;}
		}
		private StringPair spCriteria
		{
			get {return (StringPair)this.cbCriteria.SelectedItem;}
		}
		private StringPair spSkin
		{
			get {return (StringPair)this.cbSkin.SelectedItem;}
		}
		private int styleVariationID = -1;

		private System.Windows.Forms.Button btnUpdateVariation;
		private System.Windows.Forms.Button btnColor;
		private System.Windows.Forms.Button btnShadowColor;
		private System.Windows.Forms.Button btnUpdate;
		private System.Windows.Forms.Button btnFont;
		private System.Windows.Forms.Button btnDeleteVariation;
		private System.Windows.Forms.Button btnDeleteStyle;

		private System.Windows.Forms.CheckBox cbBold;
		private System.Windows.Forms.CheckBox cbItalic;
		private System.Windows.Forms.CheckBox cbUnderline;

		private System.Windows.Forms.ComboBox cbStyle;
		private System.Windows.Forms.ComboBox cbAlignment;
		private System.Windows.Forms.ComboBox cbCriteria;

		private System.Windows.Forms.FontDialog fontDialog;

		private System.Windows.Forms.GroupBox gbStyleVariation;
		
		private System.Windows.Forms.Label lblStyle;
		private System.Windows.Forms.Label lblVariation;
		private System.Windows.Forms.Label lblDescription;
		private System.Windows.Forms.Label lblFont;
		private System.Windows.Forms.Label lblHeight;
		private System.Windows.Forms.Label lblAlignment;
		private System.Windows.Forms.Label lblColor;
		private System.Windows.Forms.Label lblShadowColor;
		private System.Windows.Forms.Label lblShadowX;
		private System.Windows.Forms.Label lblShadowY;
		
		private System.Windows.Forms.TextBox tbDescription;
		private System.Windows.Forms.TextBox tbFont;
		private System.Windows.Forms.TextBox tbPixelHeight;
		private System.Windows.Forms.TextBox tbShadowX;
		private System.Windows.Forms.TextBox tbShadowY;
		private System.Windows.Forms.ComboBox cbSkin;
		private System.Windows.Forms.Label label1;
		
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public StylesForm()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			foreach (DataRow dr in mds.tStyle.Rows)
			{
				StyleDataRow drS = new StyleDataRow(dr);
				this.cbStyle.Items.Add(new StringPair(drS.fPK_Style,drS.fDescription));
			}
			this.cbStyle.Items.Add(new StringPair(-1,"(new style)"));

			this.cbCriteria.Items.Add(new StringPair(-1,"Default Variation"));
			foreach(DataRow dr in mds.tUI.Select("", UIData.DESCRIPTION_FIELD))
			{
				UIDataRow drC = new UIDataRow(dr);
				this.cbCriteria.Items.Add(new StringPair(drC.fPK_UI, drC.fDescription));
			}

			
			this.cbSkin.Items.Add(new StringPair(-1,""));
			foreach(DataRow dr in mds.tSkin.Select("", SkinData.DESCRIPTION_FIELD))
			{
				SkinDataRow drC = new SkinDataRow(dr);
				this.cbSkin.Items.Add(new StringPair(drC.fPK_Skin, drC.fDescription));
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
			this.cbStyle = new System.Windows.Forms.ComboBox();
			this.btnUpdateVariation = new System.Windows.Forms.Button();
			this.tbDescription = new System.Windows.Forms.TextBox();
			this.lblStyle = new System.Windows.Forms.Label();
			this.cbCriteria = new System.Windows.Forms.ComboBox();
			this.lblVariation = new System.Windows.Forms.Label();
			this.lblDescription = new System.Windows.Forms.Label();
			this.lblFont = new System.Windows.Forms.Label();
			this.fontDialog = new System.Windows.Forms.FontDialog();
			this.gbStyleVariation = new System.Windows.Forms.GroupBox();
			this.tbShadowY = new System.Windows.Forms.TextBox();
			this.tbShadowX = new System.Windows.Forms.TextBox();
			this.lblShadowY = new System.Windows.Forms.Label();
			this.lblShadowX = new System.Windows.Forms.Label();
			this.btnDeleteVariation = new System.Windows.Forms.Button();
			this.btnShadowColor = new System.Windows.Forms.Button();
			this.lblShadowColor = new System.Windows.Forms.Label();
			this.lblColor = new System.Windows.Forms.Label();
			this.btnColor = new System.Windows.Forms.Button();
			this.cbUnderline = new System.Windows.Forms.CheckBox();
			this.cbItalic = new System.Windows.Forms.CheckBox();
			this.cbBold = new System.Windows.Forms.CheckBox();
			this.lblAlignment = new System.Windows.Forms.Label();
			this.lblHeight = new System.Windows.Forms.Label();
			this.tbPixelHeight = new System.Windows.Forms.TextBox();
			this.tbFont = new System.Windows.Forms.TextBox();
			this.btnFont = new System.Windows.Forms.Button();
			this.cbAlignment = new System.Windows.Forms.ComboBox();
			this.btnUpdate = new System.Windows.Forms.Button();
			this.btnDeleteStyle = new System.Windows.Forms.Button();
			this.cbSkin = new System.Windows.Forms.ComboBox();
			this.label1 = new System.Windows.Forms.Label();
			this.gbStyleVariation.SuspendLayout();
			this.SuspendLayout();
			// 
			// cbStyle
			// 
			this.cbStyle.DisplayMember = "Description";
			this.cbStyle.Location = new System.Drawing.Point(120, 16);
			this.cbStyle.Name = "cbStyle";
			this.cbStyle.Size = new System.Drawing.Size(121, 21);
			this.cbStyle.TabIndex = 0;
			this.cbStyle.ValueMember = "ID";
			this.cbStyle.SelectedIndexChanged += new System.EventHandler(this.cbStyle_SelectedIndexChanged);
			// 
			// btnUpdateVariation
			// 
			this.btnUpdateVariation.Location = new System.Drawing.Point(224, 200);
			this.btnUpdateVariation.Name = "btnUpdateVariation";
			this.btnUpdateVariation.TabIndex = 11;
			this.btnUpdateVariation.Text = "Update";
			this.btnUpdateVariation.Click += new System.EventHandler(this.btnUpdateVariation_Click);
			// 
			// tbDescription
			// 
			this.tbDescription.Enabled = false;
			this.tbDescription.Location = new System.Drawing.Point(120, 112);
			this.tbDescription.Name = "tbDescription";
			this.tbDescription.Size = new System.Drawing.Size(120, 20);
			this.tbDescription.TabIndex = 2;
			this.tbDescription.Text = "";
			// 
			// lblStyle
			// 
			this.lblStyle.Location = new System.Drawing.Point(8, 16);
			this.lblStyle.Name = "lblStyle";
			this.lblStyle.TabIndex = 5;
			this.lblStyle.Text = "Style";
			this.lblStyle.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// cbCriteria
			// 
			this.cbCriteria.DisplayMember = "Description";
			this.cbCriteria.Location = new System.Drawing.Point(120, 48);
			this.cbCriteria.Name = "cbCriteria";
			this.cbCriteria.Size = new System.Drawing.Size(121, 21);
			this.cbCriteria.TabIndex = 1;
			this.cbCriteria.SelectedIndexChanged += new System.EventHandler(this.cbCriteria_SelectedIndexChanged);
			// 
			// lblVariation
			// 
			this.lblVariation.Location = new System.Drawing.Point(8, 48);
			this.lblVariation.Name = "lblVariation";
			this.lblVariation.TabIndex = 6;
			this.lblVariation.Text = "Variation";
			this.lblVariation.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// lblDescription
			// 
			this.lblDescription.Location = new System.Drawing.Point(8, 112);
			this.lblDescription.Name = "lblDescription";
			this.lblDescription.TabIndex = 7;
			this.lblDescription.Text = "Description";
			this.lblDescription.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// lblFont
			// 
			this.lblFont.Location = new System.Drawing.Point(8, 32);
			this.lblFont.Name = "lblFont";
			this.lblFont.Size = new System.Drawing.Size(80, 23);
			this.lblFont.TabIndex = 13;
			this.lblFont.Text = "Font";
			this.lblFont.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// gbStyleVariation
			// 
			this.gbStyleVariation.Controls.AddRange(new System.Windows.Forms.Control[] {
																						   this.tbShadowY,
																						   this.tbShadowX,
																						   this.lblShadowY,
																						   this.lblShadowX,
																						   this.btnDeleteVariation,
																						   this.btnShadowColor,
																						   this.lblShadowColor,
																						   this.lblColor,
																						   this.btnColor,
																						   this.cbUnderline,
																						   this.cbItalic,
																						   this.cbBold,
																						   this.lblAlignment,
																						   this.lblHeight,
																						   this.tbPixelHeight,
																						   this.tbFont,
																						   this.btnFont,
																						   this.lblFont,
																						   this.cbAlignment,
																						   this.btnUpdateVariation});
			this.gbStyleVariation.Enabled = false;
			this.gbStyleVariation.Location = new System.Drawing.Point(24, 152);
			this.gbStyleVariation.Name = "gbStyleVariation";
			this.gbStyleVariation.Size = new System.Drawing.Size(320, 240);
			this.gbStyleVariation.TabIndex = 4;
			this.gbStyleVariation.TabStop = false;
			this.gbStyleVariation.Text = "Varies by Variation";
			// 
			// tbShadowY
			// 
			this.tbShadowY.Location = new System.Drawing.Point(224, 168);
			this.tbShadowY.Name = "tbShadowY";
			this.tbShadowY.Size = new System.Drawing.Size(32, 20);
			this.tbShadowY.TabIndex = 10;
			this.tbShadowY.Text = "";
			// 
			// tbShadowX
			// 
			this.tbShadowX.Location = new System.Drawing.Point(224, 136);
			this.tbShadowX.Name = "tbShadowX";
			this.tbShadowX.Size = new System.Drawing.Size(32, 20);
			this.tbShadowX.TabIndex = 9;
			this.tbShadowX.Text = "";
			// 
			// lblShadowY
			// 
			this.lblShadowY.Location = new System.Drawing.Point(160, 168);
			this.lblShadowY.Name = "lblShadowY";
			this.lblShadowY.Size = new System.Drawing.Size(56, 23);
			this.lblShadowY.TabIndex = 19;
			this.lblShadowY.Text = "Shadow Y";
			this.lblShadowY.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// lblShadowX
			// 
			this.lblShadowX.Location = new System.Drawing.Point(160, 136);
			this.lblShadowX.Name = "lblShadowX";
			this.lblShadowX.Size = new System.Drawing.Size(56, 23);
			this.lblShadowX.TabIndex = 18;
			this.lblShadowX.Text = "Shadow X";
			this.lblShadowX.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// btnDeleteVariation
			// 
			this.btnDeleteVariation.Location = new System.Drawing.Point(128, 200);
			this.btnDeleteVariation.Name = "btnDeleteVariation";
			this.btnDeleteVariation.TabIndex = 12;
			this.btnDeleteVariation.Text = "Delete";
			this.btnDeleteVariation.Click += new System.EventHandler(this.btnDeleteVariation_Click);
			// 
			// btnShadowColor
			// 
			this.btnShadowColor.Location = new System.Drawing.Point(96, 168);
			this.btnShadowColor.Name = "btnShadowColor";
			this.btnShadowColor.Size = new System.Drawing.Size(48, 23);
			this.btnShadowColor.TabIndex = 8;
			this.btnShadowColor.Click += new System.EventHandler(this.btnShadowColor_Click);
			// 
			// lblShadowColor
			// 
			this.lblShadowColor.Location = new System.Drawing.Point(8, 168);
			this.lblShadowColor.Name = "lblShadowColor";
			this.lblShadowColor.Size = new System.Drawing.Size(80, 23);
			this.lblShadowColor.TabIndex = 17;
			this.lblShadowColor.Text = "Shadow Color";
			this.lblShadowColor.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// lblColor
			// 
			this.lblColor.Location = new System.Drawing.Point(8, 136);
			this.lblColor.Name = "lblColor";
			this.lblColor.Size = new System.Drawing.Size(80, 23);
			this.lblColor.TabIndex = 16;
			this.lblColor.Text = "Color";
			this.lblColor.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// btnColor
			// 
			this.btnColor.Location = new System.Drawing.Point(96, 136);
			this.btnColor.Name = "btnColor";
			this.btnColor.Size = new System.Drawing.Size(48, 23);
			this.btnColor.TabIndex = 7;
			this.btnColor.Click += new System.EventHandler(this.btnColor_Click);
			// 
			// cbUnderline
			// 
			this.cbUnderline.Location = new System.Drawing.Point(224, 104);
			this.cbUnderline.Name = "cbUnderline";
			this.cbUnderline.Size = new System.Drawing.Size(72, 16);
			this.cbUnderline.TabIndex = 6;
			this.cbUnderline.Text = "Underline";
			// 
			// cbItalic
			// 
			this.cbItalic.Location = new System.Drawing.Point(224, 88);
			this.cbItalic.Name = "cbItalic";
			this.cbItalic.Size = new System.Drawing.Size(72, 16);
			this.cbItalic.TabIndex = 5;
			this.cbItalic.Text = "Italic";
			// 
			// cbBold
			// 
			this.cbBold.Location = new System.Drawing.Point(224, 72);
			this.cbBold.Name = "cbBold";
			this.cbBold.Size = new System.Drawing.Size(72, 16);
			this.cbBold.TabIndex = 4;
			this.cbBold.Text = "Bold";
			// 
			// lblAlignment
			// 
			this.lblAlignment.Location = new System.Drawing.Point(8, 96);
			this.lblAlignment.Name = "lblAlignment";
			this.lblAlignment.Size = new System.Drawing.Size(80, 23);
			this.lblAlignment.TabIndex = 15;
			this.lblAlignment.Text = "Alignment";
			this.lblAlignment.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// lblHeight
			// 
			this.lblHeight.Location = new System.Drawing.Point(8, 64);
			this.lblHeight.Name = "lblHeight";
			this.lblHeight.Size = new System.Drawing.Size(80, 23);
			this.lblHeight.TabIndex = 14;
			this.lblHeight.Text = "Height";
			this.lblHeight.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// tbPixelHeight
			// 
			this.tbPixelHeight.Location = new System.Drawing.Point(96, 64);
			this.tbPixelHeight.Name = "tbPixelHeight";
			this.tbPixelHeight.TabIndex = 2;
			this.tbPixelHeight.Text = "";
			// 
			// tbFont
			// 
			this.tbFont.Location = new System.Drawing.Point(96, 32);
			this.tbFont.Name = "tbFont";
			this.tbFont.TabIndex = 0;
			this.tbFont.Text = "";
			// 
			// btnFont
			// 
			this.btnFont.Location = new System.Drawing.Point(224, 32);
			this.btnFont.Name = "btnFont";
			this.btnFont.TabIndex = 1;
			this.btnFont.Text = "Font Dialog";
			this.btnFont.Click += new System.EventHandler(this.btnFont_Click);
			// 
			// cbAlignment
			// 
			this.cbAlignment.Items.AddRange(new object[] {
															 "Top - Left",
															 "Top - Center",
															 "Top - Right",
															 "Middle - Left",
															 "Middle - Center",
															 "Middle - Right",
															 "Bottom - Left",
															 "Bottom - Center",
															 "Bottom - Right"});
			this.cbAlignment.Location = new System.Drawing.Point(96, 96);
			this.cbAlignment.MaxDropDownItems = 9;
			this.cbAlignment.Name = "cbAlignment";
			this.cbAlignment.Size = new System.Drawing.Size(104, 21);
			this.cbAlignment.TabIndex = 3;
			// 
			// btnUpdate
			// 
			this.btnUpdate.Enabled = false;
			this.btnUpdate.Location = new System.Drawing.Point(256, 112);
			this.btnUpdate.Name = "btnUpdate";
			this.btnUpdate.TabIndex = 3;
			this.btnUpdate.Text = "Update";
			this.btnUpdate.Click += new System.EventHandler(this.btnUpdate_Click);
			// 
			// btnDeleteStyle
			// 
			this.btnDeleteStyle.Enabled = false;
			this.btnDeleteStyle.Location = new System.Drawing.Point(256, 16);
			this.btnDeleteStyle.Name = "btnDeleteStyle";
			this.btnDeleteStyle.TabIndex = 8;
			this.btnDeleteStyle.Text = "Delete";
			this.btnDeleteStyle.Click += new System.EventHandler(this.btnDeleteStyle_Click);
			// 
			// cbSkin
			// 
			this.cbSkin.DisplayMember = "Description";
			this.cbSkin.Location = new System.Drawing.Point(120, 80);
			this.cbSkin.Name = "cbSkin";
			this.cbSkin.Size = new System.Drawing.Size(121, 21);
			this.cbSkin.TabIndex = 9;
			this.cbSkin.SelectedIndexChanged += new System.EventHandler(this.cbSkin_SelectedIndexChanged);
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(8, 80);
			this.label1.Name = "label1";
			this.label1.TabIndex = 10;
			this.label1.Text = "Skin";
			this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// StylesForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(360, 405);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.label1,
																		  this.cbSkin,
																		  this.btnDeleteStyle,
																		  this.btnUpdate,
																		  this.gbStyleVariation,
																		  this.lblDescription,
																		  this.lblVariation,
																		  this.cbCriteria,
																		  this.lblStyle,
																		  this.tbDescription,
																		  this.cbStyle});
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "StylesForm";
			this.Text = "StylesForm";
			this.gbStyleVariation.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion

		private void cbStyle_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			this.tbDescription.Text = (this.spStyle.intID == -1) ? "" : this.spStyle.Description;
			this.btnDeleteStyle.Enabled = (this.spStyle.intID != -1);
			this.tbDescription.Enabled = true;
			this.btnUpdate.Enabled = true;
			this.btnUpdate.Text = (this.btnDeleteStyle.Enabled)? "Update": "Create";

			if (this.cbCriteria.SelectedIndex == 0) this.cbCriteria_SelectedIndexChanged(sender, e); //otherwise will call automatically
			this.cbCriteria.SelectedIndex = 0;
			this.cbSkin.Enabled = this.cbCriteria.Enabled = this.gbStyleVariation.Enabled = (spStyle.intID != -1);

			

		}





		private void cbCriteria_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			if (this.cbSkin.SelectedIndex == 0) this.cbSkin_SelectedIndexChanged(sender, e); //otherwise will call automatically
			this.cbSkin.SelectedIndex = 0;
		}


		private void cbSkin_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			if(this.cbStyle.SelectedIndex != -1)
			{
				updateFormValues(this.spStyle.intID, this.spCriteria.intID, this.spSkin.intID);
				this.cbSkin.Enabled = this.cbCriteria.Enabled = this.gbStyleVariation.Enabled = (spStyle.intID != -1);
			}
			
		}


		private void updateFormValues(int intStyle, int intUI, int intSkin)
		{
			
			string strSelect = "";
			strSelect += "FK_Style="+this.spStyle.ID;

			if(this.spCriteria.intID == -1)
			{
				strSelect += " and FK_Criteria is null";
			}
			else
			{
				strSelect += " and FK_Criteria="+this.spCriteria.ID;
			}

			if(this.spSkin.intID == -1)
			{
				strSelect += " and FK_Skin is null";
			}
			else
			{
				strSelect += " and FK_Skin="+this.spSkin.ID;
			}


			DataRow[] drsVariation =  mds.tStyleVariation.Select(strSelect);
			if (drsVariation.Length > 0)
			{
				StyleVariationDataRow svdr = new StyleVariationDataRow(drsVariation[0]);
				this.styleVariationID = svdr.fPK_StyleVariation;

				this.tbFont.Text = svdr.fFont;
				this.tbPixelHeight.Text = svdr.fPixelHeight.ToString();
				this.setAlignmentSelection(svdr.fFK_HorizAlignment,svdr.fFK_VertAlignment);
				this.cbBold.Checked = svdr.fBold;
				this.cbItalic.Checked = svdr.fItalic;
				this.cbUnderline.Checked = svdr.fUnderline;
			
				this.tbShadowX.Text = (svdr.fShadowXIsNull)?"":svdr.fShadowX.ToString();
				this.tbShadowY.Text = (svdr.fShadowYIsNull)?"":svdr.fShadowY.ToString();

				this.btnColor.BackColor = Color.FromArgb(svdr.fForeColor);
			
				if (svdr.fShadowColorIsNull) this.btnShadowColor.BackColor = SystemColors.Control;
				else this.btnShadowColor.BackColor = Color.FromArgb(svdr.fShadowColor);

				this.btnUpdateVariation.Text = "Update";
				this.btnDeleteVariation.Enabled = true;
			}
			else
			{
				this.styleVariationID = -1;

				this.tbFont.Text = this.tbPixelHeight.Text = this.tbShadowX.Text = this.tbShadowY.Text = "";
				this.cbAlignment.SelectedItem = null;
				this.cbBold.Checked = this.cbItalic.Checked = this.cbUnderline.Checked = false;
				this.btnColor.BackColor = this.btnShadowColor.BackColor = SystemColors.Control;

				this.btnUpdateVariation.Text = "Create";
				this.btnDeleteVariation.Enabled = false;
			}

		}



		private void btnUpdate_Click(object sender, System.EventArgs e)
		{
			StyleDataRow sdr;
			int styleIndex = this.cbStyle.SelectedIndex;

			if (spStyle.intID == -1)
			{
				sdr = new StyleDataRow(mds.tStyle.NewRow());
				sdr.fDescription = this.tbDescription.Text;
				mds.tStyle.Rows.Add(sdr.dr);
			}
			else
			{
				sdr = mds.tStyle[spStyle.intID];
				sdr.fDescription = this.tbDescription.Text;
				this.cbStyle.Items.RemoveAt(styleIndex);
			}

			mds.tStyle.Update(1,mds.m_conn,mds.m_trans);
			this.cbStyle.Items.Insert(styleIndex, new StringPair(sdr.fPK_Style, sdr.fDescription));
			this.cbStyle.SelectedIndex = styleIndex;
		}

		private void btnUpdateVariation_Click(object sender, System.EventArgs e)
		{
			if (this.tbFont.Text=="" || this.tbPixelHeight.Text=="" || this.cbAlignment.SelectedItem == null)
			{
				MessageBox.Show("Font, Pixel Height, & Alignment are Required");
				return;
			}
			if (!StringPair.BothEmptyOrBothSet(this.tbShadowX.Text,this.tbShadowY.Text))
			{
				MessageBox.Show("Shadow X & Y should either both be set or both be empty");
				return;
			}

			StyleVariationDataRow svdr = (this.styleVariationID == -1)? new StyleVariationDataRow(mds.tStyleVariation.NewRow()): mds.tStyleVariation[this.styleVariationID];

			svdr.fFont = this.tbFont.Text;
			svdr.fPixelHeight = Convert.ToInt32(this.tbPixelHeight.Text);
			svdr.fFK_HorizAlignment = this.horizontalID;
			svdr.fFK_VertAlignment = this.verticalID;
            
			// won't be null
			svdr.fForeColor = this.btnColor.BackColor.ToArgb();
			svdr.fBold = this.cbBold.Checked;
			svdr.fItalic = this.cbItalic.Checked;
			svdr.fUnderline = this.cbUnderline.Checked;

			// shadow all 3 set or all 3 null
			if (this.tbShadowX.Text=="") 
			{
				svdr.fShadowXSetNull();
				svdr.fShadowYSetNull();
				svdr.fShadowColorSetNull();
			}
			else
			{
				svdr.fShadowX = Convert.ToInt32(this.tbShadowX.Text);
				svdr.fShadowY = Convert.ToInt32(this.tbShadowY.Text);
				svdr.fShadowColor = this.btnShadowColor.BackColor.ToArgb();
			}

			if (this.styleVariationID == -1)
			{
				svdr.fFK_Style = this.spStyle.intID;
				if (this.spCriteria.intID != -1) svdr.fFK_UI = this.spCriteria.intID;
				if (this.spSkin.intID != -1) svdr.fFK_Skin = this.spSkin.intID;
				mds.tStyleVariation.Rows.Add(svdr.dr);
			}
			mds.tStyleVariation.Update(1,mds.m_conn,mds.m_trans);
			this.cbCriteria_SelectedIndexChanged(null,null);
		}

		private void btnDeleteStyle_Click(object sender, System.EventArgs e)
		{
			StyleDataRow sdr = mds.tStyle[this.spStyle.intID];
			/*
			if (sdr.dr.GetChildRows(MyDataSet.MyRelations.Controller_FK_Style_Default).Length != 0)
			{
				MessageBox.Show("You Can't Delete \"" + this.spStyle.Description + "\" because there is a controller associated with it");
			}
			*/
			if (MessageBox.Show("Are you sure you want to delete \"" + this.spStyle.Description + "\"?","Are You Sure?",MessageBoxButtons.YesNo) == DialogResult.Yes)
			{
				foreach (DataRow dr in sdr.dr.GetChildRows(MyDataSet.MyRelations.DesignObjVariation_Text_Skin_Language_FK_Style))
				{
					DesignObjVariation_Text_Skin_LanguageDataRow ovtsldr = new DesignObjVariation_Text_Skin_LanguageDataRow(dr);
					ovtsldr.fFK_StyleSetNull();
				}
				mds.tDesignObjVariation_Text_Skin_Language.Update(1,mds.m_conn,mds.m_trans);
				foreach (DataRow dr in sdr.dr.GetChildRows(MyDataSet.MyRelations.StyleVariation_FK_Style))
				{
					StyleVariationDataRow svdr = new StyleVariationDataRow(dr);
					svdr.dr.Delete();
				}
				mds.tStyleVariation.Update(1,mds.m_conn,mds.m_trans);
				sdr.dr.Delete();
				mds.tStyle.Update(1,mds.m_conn,mds.m_trans);

				this.cbStyle.Items.RemoveAt(this.cbStyle.SelectedIndex--);
			}
		}

		private void btnDeleteVariation_Click(object sender, System.EventArgs e)
		{
			if (MessageBox.Show("Are you sure you want to delete \"" + this.spStyle.Description + " - " + this.spCriteria.Description + "\"?","Are You Sure?",MessageBoxButtons.YesNo) == DialogResult.Yes)
			{
				mds.tStyleVariation[this.styleVariationID].dr.Delete();
				mds.tStyleVariation.Update(1,mds.m_conn,mds.m_trans);
				this.styleVariationID = -1;
				this.cbCriteria_SelectedIndexChanged(null,null);
			}
		}

		private void setAlignmentSelection(int hAlign, int vAlign)
		{
			this.cbAlignment.SelectedIndex = (--vAlign*3 + --hAlign);
		}
	
		private int horizontalID
		{
			get {return (this.cbAlignment.SelectedIndex % 3) + 1;}
		}

		private int verticalID
		{
			get {return (this.cbAlignment.SelectedIndex / 3) + 1;}
		}

		private void btnColor_Click(object sender, System.EventArgs e)
		{
			object o = ColorPicker.getColor(); //this.getColor();
			if (o!=null) this.btnColor.BackColor = (Color)o;
		}

		private void btnShadowColor_Click(object sender, System.EventArgs e)
		{
			object o = ColorPicker.getColor();
			if (o!=null) this.btnShadowColor.BackColor = (Color)o;
		}
		
		private void btnFont_Click(object sender, System.EventArgs e)
		{
			UIStyleVariation uisv = new UIStyleVariation(); // Just for Font Style
			uisv.Bold = this.cbBold.Checked;
			uisv.Italic = this.cbItalic.Checked;
			uisv.Underline = this.cbUnderline.Checked;

			if (this.tbPixelHeight.Text == "") this.tbPixelHeight.Text = "16";

			this.fontDialog.Font = new Font(this.tbFont.Text,Convert.ToSingle(this.tbPixelHeight.Text),uisv.FontStyle);
			
			if (this.fontDialog.ShowDialog() == DialogResult.OK)
			{
				this.tbFont.Text = this.fontDialog.Font.FontFamily.Name;
				this.cbBold.Checked = this.fontDialog.Font.Bold;
				this.cbItalic.Checked = this.fontDialog.Font.Italic;
				this.cbUnderline.Checked = this.fontDialog.Font.Underline;
				this.tbPixelHeight.Text = Math.Round(this.fontDialog.Font.SizeInPoints).ToString();
			}
		}

		
	}
}
