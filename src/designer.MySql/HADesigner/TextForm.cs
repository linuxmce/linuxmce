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
	/// Summary description for TextForm.
	/// </summary>
	public class TextForm : System.Windows.Forms.Form
	{
		private bool updatedText = false;
		public bool UpdatedText { get{return this.updatedText;} }

		public string TreeLabel
		{
			get
			{
				if (this.drText==null) return "";
				else return this.drText.fDescription + "(" + this.drText.fPK_Text.ToString() + ")";
			}
		}

		private bool newText;
		private MyDataSet mds
		{
			get	{return HADataConfiguration.m_mdsCache;}
		}
		private MDIForm MainForm 
		{
			get {return (MDIForm)this.MdiParent;}
		}

        private TextDataRow drText = null;
		public int TextID
		{
			get {return (drText==null)? -1 : drText.fPK_Text;}
		}
        private TextCategoryDataRow drTextCategory = null;
		public int TextCategoryID
		{
			get {return (drTextCategory==null)? -1 : drTextCategory.fPK_TextCategory;}
		}

		private System.Windows.Forms.Panel pLanguages;
		
		private Hashtable htLanguages = new Hashtable();
		
		private System.Windows.Forms.Button btnOK;
		private System.Windows.Forms.Button btnCancel;
		private System.Windows.Forms.Label lblDescription;
		private System.Windows.Forms.Label lblDefine;
		private System.Windows.Forms.TextBox tbDescription;
		private System.Windows.Forms.TextBox tbDefine;

		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public TextForm(int TextID) : this(TextID,false){}

		public TextForm(int TextCategoryID, bool newText)
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			this.newText = newText;
			if (newText) this.drTextCategory = mds.tTextCategory[TextCategoryID];
			else this.drText = mds.tText[TextCategoryID];
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
			this.pLanguages = new System.Windows.Forms.Panel();
			this.btnOK = new System.Windows.Forms.Button();
			this.btnCancel = new System.Windows.Forms.Button();
			this.lblDescription = new System.Windows.Forms.Label();
			this.lblDefine = new System.Windows.Forms.Label();
			this.tbDescription = new System.Windows.Forms.TextBox();
			this.tbDefine = new System.Windows.Forms.TextBox();
			this.SuspendLayout();
			// 
			// pLanguages
			// 
			this.pLanguages.AutoScroll = true;
			this.pLanguages.Location = new System.Drawing.Point(8, 72);
			this.pLanguages.Name = "pLanguages";
			this.pLanguages.Size = new System.Drawing.Size(336, 248);
			this.pLanguages.TabIndex = 0;
			// 
			// btnOK
			// 
			this.btnOK.Location = new System.Drawing.Point(192, 336);
			this.btnOK.Name = "btnOK";
			this.btnOK.TabIndex = 1;
			this.btnOK.Text = "OK";
			this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
			// 
			// btnCancel
			// 
			this.btnCancel.Location = new System.Drawing.Point(56, 336);
			this.btnCancel.Name = "btnCancel";
			this.btnCancel.TabIndex = 2;
			this.btnCancel.Text = "Cancel";
			this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
			// 
			// lblDescription
			// 
			this.lblDescription.Location = new System.Drawing.Point(32, 16);
			this.lblDescription.Name = "lblDescription";
			this.lblDescription.TabIndex = 3;
			this.lblDescription.Text = "Description:";
			this.lblDescription.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// lblDefine
			// 
			this.lblDefine.Location = new System.Drawing.Point(32, 40);
			this.lblDefine.Name = "lblDefine";
			this.lblDefine.TabIndex = 4;
			this.lblDefine.Text = "Define: ";
			this.lblDefine.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// tbDescription
			// 
			this.tbDescription.Location = new System.Drawing.Point(136, 16);
			this.tbDescription.Name = "tbDescription";
			this.tbDescription.TabIndex = 5;
			this.tbDescription.Text = "";
			// 
			// tbDefine
			// 
			this.tbDefine.Location = new System.Drawing.Point(136, 40);
			this.tbDefine.Name = "tbDefine";
			this.tbDefine.TabIndex = 6;
			this.tbDefine.Text = "";
			// 
			// TextForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(360, 381);
			this.ControlBox = false;
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.tbDefine,
																		  this.tbDescription,
																		  this.lblDefine,
																		  this.lblDescription,
																		  this.btnCancel,
																		  this.btnOK,
																		  this.pLanguages});
			this.Name = "TextForm";
			this.Text = "TextForm";
			this.Load += new System.EventHandler(this.TextForm_Load);
			this.ResumeLayout(false);

		}
		#endregion

		private void TextForm_Load(object sender, System.EventArgs e)
		{
			if (this.newText) this.Text = "New Text in category: " + this.drTextCategory.fDescription;
			else
			{
				this.Text = "Text: " + this.drText.fDescription;
				this.tbDefine.Text = this.drText.fDefine;
				this.tbDescription.Text = this.drText.fDescription;
			}
			this.LoadLanguages();
		}

		private void LoadLanguages()
		{
			this.pLanguages.Controls.Clear();
			this.htLanguages.Clear();

			int count = 0;
			foreach(DataRow dr in mds.tLanguage.Rows)
			{
				LanguageDataRow drLanguage = new LanguageDataRow(dr);
				int LanguageID = drLanguage.fPK_Language;
				
				TextBox tbText_LS = new TextBox();
				tbText_LS.Multiline = true;
				if (!this.newText) tbText_LS.Text = drText.fDescription_LS(LanguageID,false);
				tbText_LS.Top = 50*count;
				tbText_LS.Height = 45; // 3 lines
				tbText_LS.Left = 130;
				
				Label lLanguage = new Label();
				lLanguage.Width = 120;
				lLanguage.Top = 50*count;
				lLanguage.Left = 5;
				lLanguage.Text = drLanguage.fDescription;
				lLanguage.TextAlign = ContentAlignment.TopRight;

				this.pLanguages.Controls.Add(lLanguage);
				this.pLanguages.Controls.Add(tbText_LS);
				this.htLanguages[LanguageID] = tbText_LS;

				count++;
        	}
		}

		private bool DuplicateDefine()
		{
			DataRow[] drDefineRows = mds.tText.Select("Define='"+this.tbDefine.Text+"'");
			return (drDefineRows.Length > 0);
		}

		private bool SaveLanguages() // return success
		{
			if( this.tbDefine.Text=="" )
				this.tbDefine.Text=this.tbDescription.Text;

			if (this.tbDescription.Text=="")
			{
				MessageBox.Show("A Description is required");
				return false;
			}

			if (this.newText)
			{
				if (this.DuplicateDefine())
				{
					MessageBox.Show("Duplicate define entered");
					return false;
				}

				this.drText = new TextDataRow(mds.tText.NewRow());
				this.drText.fFK_TextCategory = this.drTextCategory.fPK_TextCategory;
				this.drText.fDefine = this.tbDefine.Text;
				this.drText.fDescription = this.tbDescription.Text;
				mds.tText.Rows.Add(drText.dr);
				mds.tText.Update(1,mds.m_conn,mds.m_trans);
			}
			else if (this.drText.fDefine != this.tbDefine.Text || this.drText.fDescription != this.tbDescription.Text) // Update Text Row
			{
				if (this.drText.fDefine != this.tbDefine.Text && this.DuplicateDefine())
				{
					MessageBox.Show("Duplicate define entered");
					return false;
				}
				this.drText.fDefine = this.tbDefine.Text;
				this.drText.fDescription = this.tbDescription.Text;
				mds.tText.Update(1,mds.m_conn,mds.m_trans);
				this.updatedText = true;
			}

			foreach (object key in htLanguages.Keys)
			{
				TextBox tbDescription = (TextBox)htLanguages[key];
				int languageID = (int)key;
				if (tbDescription.Text=="") 
				{	// Delete row if exists
					if (this.drText.fDescription_LS(languageID,false)!=null) mds.tText_LS[this.TextID,languageID].dr.Delete();
				}
				else this.drText.fDescription_LS(languageID,tbDescription.Text); // Add or Update Row
			}
			mds.tText_LS.Update(1,mds.m_conn,mds.m_trans);
			return true;
		}

		private void btnOK_Click(object sender, System.EventArgs e)
		{
            if (this.SaveLanguages()) this.DialogResult = DialogResult.OK; // Setting DialogResult closes form
		}

		private void btnCancel_Click(object sender, System.EventArgs e)
		{
			this.DialogResult = DialogResult.Cancel;
		}
	}
}
