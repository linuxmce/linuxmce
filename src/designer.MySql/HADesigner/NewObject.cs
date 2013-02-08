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
	/// Summary description for NewDesignObj.
	/// </summary>
	public class NewDesignObj : System.Windows.Forms.Form
	{
		private ObjsBrowser2 ObjectBrowser = null;
		private MDIForm MainForm
		{
			get {return (MDIForm)this.Owner;}
		}

		private System.Windows.Forms.ComboBox cbDesignObjTypes;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;


		private MyDataSet mds
		{
			get {return HADataConfiguration.m_mdsCache;}
		}

		private ArrayList alParameters = new ArrayList();
		private string defaultDirectory;

		
		private int m_intDesignObjCategoryID = -1;
		private System.Windows.Forms.Button btnCancel;
		private System.Windows.Forms.Button btnOK;
		private System.Windows.Forms.Panel pnParameters;
		private System.Windows.Forms.TextBox tbDescription;
		private System.Windows.Forms.Label lblDescription;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.NumericUpDown nudPriority;
		private System.Windows.Forms.CheckBox cbAnimate;
	
		private int DesignObjCategoryID
		{
			get	{return m_intDesignObjCategoryID;}
			set	{m_intDesignObjCategoryID = value;}
		}


	
		private int m_intDesignObjType = -1;
		private int DesignObjType
		{
			get	{return m_intDesignObjType;}
			set	{m_intDesignObjType = value;}
		}


		public NewDesignObj(int intDesignObjCategoryID, ObjsBrowser2 ObjBrowser)
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//

			this.DesignObjCategoryID = intDesignObjCategoryID;
			this.ObjectBrowser = ObjBrowser;
			UpdateDesignObjTypes();
		}

		private void NewDesignObj_Load(object sender, System.EventArgs e)
		{
			this.defaultDirectory = MainForm.GraphicsDirectory + "/" + mds.tSkin[1].fDataSubdirectory;
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
			this.cbDesignObjTypes = new System.Windows.Forms.ComboBox();
			this.btnCancel = new System.Windows.Forms.Button();
			this.btnOK = new System.Windows.Forms.Button();
			this.pnParameters = new System.Windows.Forms.Panel();
			this.tbDescription = new System.Windows.Forms.TextBox();
			this.lblDescription = new System.Windows.Forms.Label();
			this.label1 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.nudPriority = new System.Windows.Forms.NumericUpDown();
			this.cbAnimate = new System.Windows.Forms.CheckBox();
			((System.ComponentModel.ISupportInitialize)(this.nudPriority)).BeginInit();
			this.SuspendLayout();
			// 
			// cbDesignObjTypes
			// 
			this.cbDesignObjTypes.Location = new System.Drawing.Point(112, 24);
			this.cbDesignObjTypes.Name = "cbDesignObjTypes";
			this.cbDesignObjTypes.Size = new System.Drawing.Size(144, 21);
			this.cbDesignObjTypes.TabIndex = 0;
			this.cbDesignObjTypes.SelectedIndexChanged += new System.EventHandler(this.cbDesignObjTypes_SelectedIndexChanged);
			// 
			// btnCancel
			// 
			this.btnCancel.Location = new System.Drawing.Point(80, 408);
			this.btnCancel.Name = "btnCancel";
			this.btnCancel.TabIndex = 5;
			this.btnCancel.Text = "Cancel";
			this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
			// 
			// btnOK
			// 
			this.btnOK.Location = new System.Drawing.Point(208, 408);
			this.btnOK.Name = "btnOK";
			this.btnOK.Size = new System.Drawing.Size(168, 23);
			this.btnOK.TabIndex = 4;
			this.btnOK.Text = "Create New DesignObj";
			this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
			// 
			// pnParameters
			// 
			this.pnParameters.AutoScroll = true;
			this.pnParameters.Location = new System.Drawing.Point(16, 192);
			this.pnParameters.Name = "pnParameters";
			this.pnParameters.Size = new System.Drawing.Size(456, 192);
			this.pnParameters.TabIndex = 3;
			// 
			// tbDescription
			// 
			this.tbDescription.Location = new System.Drawing.Point(112, 64);
			this.tbDescription.Name = "tbDescription";
			this.tbDescription.Size = new System.Drawing.Size(224, 20);
			this.tbDescription.TabIndex = 1;
			this.tbDescription.Text = "";
			// 
			// lblDescription
			// 
			this.lblDescription.Location = new System.Drawing.Point(24, 64);
			this.lblDescription.Name = "lblDescription";
			this.lblDescription.Size = new System.Drawing.Size(80, 23);
			this.lblDescription.TabIndex = 7;
			this.lblDescription.Text = "Description";
			this.lblDescription.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(24, 24);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(80, 23);
			this.label1.TabIndex = 6;
			this.label1.Text = "DesignObj Type";
			this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// label2
			// 
			this.label2.Location = new System.Drawing.Point(24, 168);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(80, 23);
			this.label2.TabIndex = 9;
			this.label2.Text = "Parameters";
			// 
			// label3
			// 
			this.label3.Location = new System.Drawing.Point(24, 104);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(80, 23);
			this.label3.TabIndex = 8;
			this.label3.Text = "Priority";
			this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// nudPriority
			// 
			this.nudPriority.Location = new System.Drawing.Point(112, 104);
			this.nudPriority.Maximum = new System.Decimal(new int[] {
																		10,
																		0,
																		0,
																		0});
			this.nudPriority.Minimum = new System.Decimal(new int[] {
																		1,
																		0,
																		0,
																		0});
			this.nudPriority.Name = "nudPriority";
			this.nudPriority.Size = new System.Drawing.Size(56, 20);
			this.nudPriority.TabIndex = 2;
			this.nudPriority.Value = new System.Decimal(new int[] {
																	  5,
																	  0,
																	  0,
																	  0});
			// 
			// cbAnimate
			// 
			this.cbAnimate.CheckAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.cbAnimate.Location = new System.Drawing.Point(200, 104);
			this.cbAnimate.Name = "cbAnimate";
			this.cbAnimate.Size = new System.Drawing.Size(72, 24);
			this.cbAnimate.TabIndex = 10;
			this.cbAnimate.Text = "Animate";
			this.cbAnimate.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// NewDesignObj
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(488, 445);
			this.ControlBox = false;
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.cbAnimate,
																		  this.nudPriority,
																		  this.label3,
																		  this.label2,
																		  this.label1,
																		  this.lblDescription,
																		  this.tbDescription,
																		  this.pnParameters,
																		  this.btnOK,
																		  this.btnCancel,
																		  this.cbDesignObjTypes});
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "NewDesignObj";
			this.ShowInTaskbar = false;
			this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
			this.Text = "Create New DesignObj";
			this.Load += new System.EventHandler(this.NewDesignObj_Load);
			((System.ComponentModel.ISupportInitialize)(this.nudPriority)).EndInit();
			this.ResumeLayout(false);

		}
		#endregion


		
		private void UpdateDesignObjTypes()
		{

			cbDesignObjTypes.Items.Clear();
			cbDesignObjTypes.DisplayMember = "fDescription";
			cbDesignObjTypes.ValueMember = "fPK_DesignObjType";

			DataRow[] drDesignObjTypes = this.mds.tDesignObjType.Select(null, DesignObjTypeData.DESCRIPTION_FIELD);
			foreach(DataRow dr in drDesignObjTypes)
			{
				DesignObjTypeDataRow drDesignObjType = new DesignObjTypeDataRow(dr);
				cbDesignObjTypes.Items.Add(drDesignObjType);
			}
		}



		private void UpdateParameters()
		{

			if(this.DesignObjType == -1)
			{
				pnParameters.Controls.Clear();
			}
			else
			{

				pnParameters.Controls.Clear();
				alParameters.Clear();

				DataRow[] drParameters = this.mds.tDesignObjType_DesignObjParameter.Select(DesignObjType_DesignObjParameterData.FK_DESIGNOBJTYPE_FIELD + "=" + this.DesignObjType, null);
			
				int intCount = -1;
				foreach(DataRow dr in drParameters)
				{
					intCount++;

					DesignObjType_DesignObjParameterDataRow drParameter = new DesignObjType_DesignObjParameterDataRow(dr);

					ParameterControl objParameter = new ParameterControl(this.defaultDirectory);
					objParameter.FK_DesignObjParameter = drParameter.fFK_DesignObjParameter;
					objParameter.Description = drParameter.fFK_DesignObjParameter_DataRow.fDescription;

					objParameter.TextBoxValue = new TextBox();
					objParameter.TextBoxValue.Top = 25 * intCount;
					objParameter.TextBoxValue.Left = 140;
					objParameter.TextBoxValue.Width = 200;

					alParameters.Add(objParameter);

					Label objLabel = new Label();
					objLabel.Width = 120;
					objLabel.Top = 25 * intCount;
					objLabel.Left = 5;
					objLabel.Text = objParameter.Description;


					Button objButton = new Button();
					objButton.Text = "File";
					objButton.Top = 25 * intCount;
					objButton.Left = 350;

					// objParameter.FileButton = objButton;

					objButton.Click += new System.EventHandler(objParameter.OpenFile);
					

					pnParameters.Controls.Add(objLabel);
					pnParameters.Controls.Add(objParameter.TextBoxValue);
					
					if(drParameter.fFK_DesignObjParameter == DesignObjParameterData.GRAPHIC_FILENAME_CONST)
					{
						pnParameters.Controls.Add(objButton);
					}

				}
			

			}
		}

		private void cbDesignObjTypes_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			int intOldDesignObjType = this.DesignObjType;

			DesignObjTypeDataRow drDesignObjType = (DesignObjTypeDataRow) cbDesignObjTypes.SelectedItem;
			if(drDesignObjType == null)
			{
				this.DesignObjType = -1;
			}
			else
			{
				this.DesignObjType = drDesignObjType.fPK_DesignObjType;
			}

			if(this.DesignObjType != intOldDesignObjType)
			{
				UpdateParameters();
			}
		}

		private void btnCancel_Click(object sender, System.EventArgs e)
		{
			this.Close();
		}

		private void btnOK_Click(object sender, System.EventArgs e)
		{
			//make sure data is sufficient
			if(tbDescription.Text == "" || this.DesignObjCategoryID == -1)
			{
				MessageBox.Show("Please complete the form.");
			}
			else
			{
				//create the object
				UIDesignObj objUIDesignObj = new UIDesignObj(null, -1, -1, this.MainForm.GraphicsDirectory);
			
				objUIDesignObj.UIDesignObjType = this.DesignObjType;
				objUIDesignObj.DesignObjCategory = this.DesignObjCategoryID;
				objUIDesignObj.Description = tbDescription.Text;
				objUIDesignObj.Priority = (sbyte)this.nudPriority.Value;
				objUIDesignObj.Animate = this.cbAnimate.Checked;
				objUIDesignObj.NeedsInsert = true;

				//create the standard variation
				UIDesignObjVariation objVariation = objUIDesignObj.AddNewVariation(-1, false, 1);

				//add the parameters
				foreach(ParameterControl objParameter in alParameters)
				{
					switch(objParameter.FK_DesignObjParameter)
					{
						case DesignObjParameterData.WIDTH_CONST:
						case DesignObjParameterData.HEIGHT_CONST:
							if (objParameter.TextBoxValue.Text=="0") objParameter.TextBoxValue.Text="";
							break;
					}
					objVariation.AddNewParameter(objParameter.FK_DesignObjParameter, objParameter.TextBoxValue.Text, -1);
				}

				objUIDesignObj.SaveToDatabase();

				ObjectBrowser.insertNewDesignObjToTree(objUIDesignObj.ID);
				this.Close();
			}
		}
	}
}
