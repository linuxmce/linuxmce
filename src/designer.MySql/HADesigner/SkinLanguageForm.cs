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
	/// Summary description for SkinLanguageForm.
	/// </summary>
	public class SkinLanguageForm : System.Windows.Forms.Form
	{
        private UIText parentUIText = null;
		private UIDesignObj parentUIDesignObj = null;
		private string graphicsDir = null;

		private StringPair defaultStringPair = new StringPair("-1","Default");

		private System.Windows.Forms.Button btnOK;
		private System.Windows.Forms.Button btnCancel;
		private System.Windows.Forms.ComboBox cbLanguage;
		private System.Windows.Forms.ComboBox cbSkin;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		private MyDataSet mds
		{
			get	{return HADataConfiguration.m_mdsCache;}
		}

		public SkinLanguageForm(UIText parentUIText)
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			this.parentUIText = parentUIText;
		}

		public SkinLanguageForm(UIDesignObj parentUIDesignObj, string graphicsDir)
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			this.parentUIDesignObj = parentUIDesignObj;
			this.graphicsDir = graphicsDir;
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
			this.btnOK = new System.Windows.Forms.Button();
			this.btnCancel = new System.Windows.Forms.Button();
			this.cbLanguage = new System.Windows.Forms.ComboBox();
			this.cbSkin = new System.Windows.Forms.ComboBox();
			this.SuspendLayout();
			// 
			// btnOK
			// 
			this.btnOK.Location = new System.Drawing.Point(151, 151);
			this.btnOK.Name = "btnOK";
			this.btnOK.TabIndex = 2;
			this.btnOK.Text = "OK";
			this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
			// 
			// btnCancel
			// 
			this.btnCancel.Location = new System.Drawing.Point(39, 151);
			this.btnCancel.Name = "btnCancel";
			this.btnCancel.TabIndex = 3;
			this.btnCancel.Text = "Cancel";
			this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
			// 
			// cbLanguage
			// 
			this.cbLanguage.DisplayMember = "Description";
			this.cbLanguage.Location = new System.Drawing.Point(72, 87);
			this.cbLanguage.Name = "cbLanguage";
			this.cbLanguage.Size = new System.Drawing.Size(121, 21);
			this.cbLanguage.TabIndex = 1;
			this.cbLanguage.Text = "Language";
			this.cbLanguage.ValueMember = "ID";
			// 
			// cbSkin
			// 
			this.cbSkin.DisplayMember = "Description";
			this.cbSkin.Location = new System.Drawing.Point(72, 39);
			this.cbSkin.Name = "cbSkin";
			this.cbSkin.Size = new System.Drawing.Size(121, 21);
			this.cbSkin.TabIndex = 0;
			this.cbSkin.Text = "Skin";
			this.cbSkin.ValueMember = "ID";
			// 
			// SkinLanguageForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(264, 213);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.cbSkin,
																		  this.cbLanguage,
																		  this.btnCancel,
																		  this.btnOK});
			this.Name = "SkinLanguageForm";
			this.Text = "New Skin Language";
			this.Load += new System.EventHandler(this.SkinLanguageForm_Load);
			this.ResumeLayout(false);

		}
		#endregion


		private void createUITSL()
		{
			UITextSkinLanguage uiTSL = new UITextSkinLanguage(this.parentUIText,-1);

            uiTSL.LanguageID = Convert.ToInt32(((StringPair)this.cbLanguage.SelectedItem).ID);
			uiTSL.SkinID = Convert.ToInt32(((StringPair)this.cbSkin.SelectedItem).ID);

			this.parentUIText.TextSkinLanguages.Add(uiTSL);
		}

		private void createUIDSL()
		{
			UIChildSkinLanguage uiDSL = new UIChildSkinLanguage(
				this.parentUIDesignObj, -1, -1, -1, graphicsDir);

			uiDSL.LanguageID = Convert.ToInt32(((StringPair)this.cbLanguage.SelectedItem).ID);
			uiDSL.SkinID = Convert.ToInt32(((StringPair)this.cbSkin.SelectedItem).ID);

			this.parentUIDesignObj.ChildSkinLanguages.Add(uiDSL);
		}

		private void btnOK_Click(object sender, System.EventArgs e)
		{
			if(null != this.parentUIText)
				createUITSL();
			else if(null != this.parentUIDesignObj)
				createUIDSL();

			this.DialogResult = DialogResult.OK;
			this.Close();
		}

		private void btnCancel_Click(object sender, System.EventArgs e)
		{
			this.DialogResult = DialogResult.Cancel;
			this.Close();
		}

		private void SkinLanguageForm_Load(object sender, System.EventArgs e)
		{
			this.cbLanguage.Items.Add(this.defaultStringPair);
			foreach(DataRow dr in mds.tLanguage.Rows)
			{
				LanguageDataRow ldr =  new LanguageDataRow(dr);
				this.cbLanguage.Items.Add(new StringPair(ldr.fPK_Language.ToString(),ldr.fDescription));
			}
            this.cbLanguage.SelectedIndex = 0;

			this.cbSkin.Items.Add(this.defaultStringPair);
			foreach(DataRow dr in mds.tSkin.Rows)
			{
				SkinDataRow sdr = new SkinDataRow(dr);
				this.cbSkin.Items.Add(new StringPair(sdr.fPK_Skin.ToString(),sdr.fDescription));
			}
			this.cbSkin.SelectedIndex = 0;
		}	 
	}
}
