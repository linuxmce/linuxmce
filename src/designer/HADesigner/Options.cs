using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using Microsoft.Win32;

namespace HADesigner
{
	/// <summary>
	/// Summary description for Options.
	/// </summary>
	
	public class Options : System.Windows.Forms.Form
	{
        private string strRegKey = "Software\\Pluto\\OrbiterDesigner";
		private string strDefaultDirectory = @"C:\PlutoSkins";
        private string strDefaultDirectoryKey = "GraphicsDirectory";
		private string strDefaultConnection = @"driver={MySQL ODBC 3.51 Driver};server=localhost;uid=;password=;database=pluto_dce;";
		private string strDefaultConnectionKey = "ConnectionKey";

		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.TextBox tbGraphicsDirectory;
		private System.Windows.Forms.Button btnOK;
		private System.Windows.Forms.Button btnCancel;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.TextBox tbConnection;

		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		// private RegistryKey m_RegistryKey;

		public Options()
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
			this.label1 = new System.Windows.Forms.Label();
			this.tbGraphicsDirectory = new System.Windows.Forms.TextBox();
			this.btnOK = new System.Windows.Forms.Button();
			this.btnCancel = new System.Windows.Forms.Button();
			this.label2 = new System.Windows.Forms.Label();
			this.tbConnection = new System.Windows.Forms.TextBox();
			this.SuspendLayout();
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(8, 16);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(136, 32);
			this.label1.TabIndex = 0;
			this.label1.Text = "Base Graphics Directory: (Case-Sensitive)";
			// 
			// tbGraphicsDirectory
			// 
			this.tbGraphicsDirectory.Location = new System.Drawing.Point(144, 16);
			this.tbGraphicsDirectory.Name = "tbGraphicsDirectory";
			this.tbGraphicsDirectory.Size = new System.Drawing.Size(216, 20);
			this.tbGraphicsDirectory.TabIndex = 1;
			this.tbGraphicsDirectory.Text = "";
			// 
			// btnOK
			// 
			this.btnOK.Location = new System.Drawing.Point(280, 112);
			this.btnOK.Name = "btnOK";
			this.btnOK.TabIndex = 2;
			this.btnOK.Text = "OK";
			this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
			// 
			// btnCancel
			// 
			this.btnCancel.Location = new System.Drawing.Point(192, 112);
			this.btnCancel.Name = "btnCancel";
			this.btnCancel.TabIndex = 3;
			this.btnCancel.Text = "Cancel";
			this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
			// 
			// label2
			// 
			this.label2.Location = new System.Drawing.Point(32, 56);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(100, 32);
			this.label2.TabIndex = 4;
			this.label2.Text = "Database Connection:";
			this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// tbConnection
			// 
			this.tbConnection.Location = new System.Drawing.Point(144, 56);
			this.tbConnection.Name = "tbConnection";
			this.tbConnection.Size = new System.Drawing.Size(216, 20);
			this.tbConnection.TabIndex = 5;
			this.tbConnection.Text = "";
			// 
			// Options
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(400, 174);
			this.ControlBox = false;
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.tbConnection,
																		  this.label2,
																		  this.btnCancel,
																		  this.btnOK,
																		  this.tbGraphicsDirectory,
																		  this.label1});
			this.Name = "Options";
			this.Text = "Options";
			this.Load += new System.EventHandler(this.Options_Load);
			this.ResumeLayout(false);

		}
		#endregion

		private void Options_Load(object sender, System.EventArgs e)
		{
			RegistryKey rk = Registry.CurrentUser.CreateSubKey(this.strRegKey); 
			tbGraphicsDirectory.Text = rk.GetValue(this.strDefaultDirectoryKey,this.strDefaultDirectory).ToString();
			tbConnection.Text = rk.GetValue(this.strDefaultConnectionKey,this.strDefaultConnection).ToString();
			rk.Close();
		}

		private void btnOK_Click(object sender, System.EventArgs e)
		{
			RegistryKey rk = Registry.CurrentUser.CreateSubKey(this.strRegKey);
			rk.SetValue(this.strDefaultDirectoryKey,tbGraphicsDirectory.Text);
			rk.SetValue(this.strDefaultConnectionKey,tbConnection.Text);
			rk.Close();
			this.DialogResult = DialogResult.OK;
		}

		private void btnCancel_Click(object sender, System.EventArgs e)
		{
			this.DialogResult = DialogResult.Cancel;
		}
	}
}
