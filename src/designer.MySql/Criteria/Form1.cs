using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using HAData.Common;
using HAData.DataAccess;

namespace Criteria
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class Form1 : System.Windows.Forms.Form
	{
		private CriteriaList m_CriteriaList;
		private System.Windows.Forms.ComboBox cbCriteria;
		private System.Windows.Forms.ComboBox cbOperator;
		private System.Windows.Forms.ComboBox cbValue;
		private System.Windows.Forms.TextBox tbText;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		//NestingParameter m_NestingParameter;

		private System.Windows.Forms.Button Add;
		private System.Windows.Forms.Button btnAnd;
		private System.Windows.Forms.Button btnOr;
		private System.Windows.Forms.TextBox tbResult;
		private System.Windows.Forms.Button btnDown;
		private System.Windows.Forms.Button btnNot;
		private System.Windows.Forms.Button btnAddNestingParm;
		private System.Windows.Forms.Button btnEdit;
		private System.Windows.Forms.Button btnDelete;
		private System.Windows.Forms.Button btnUp;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.TextBox tbPK_Criteria;
		private System.Windows.Forms.TextBox tbDescription;
		private System.Windows.Forms.TextBox tbDefine;
		private System.Windows.Forms.Button btnSave;
		private System.Windows.Forms.Button btnLoad;
		private System.Windows.Forms.Button button1;
		bool bLoading=true;

		public Form1(CriteriaList criteriaList)
		{
			m_CriteriaList = criteriaList;
			//m_NestingParameter = m_CriteriaList.m_NestingParameter;
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
				if (components != null) 
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
			this.Add = new System.Windows.Forms.Button();
			this.cbCriteria = new System.Windows.Forms.ComboBox();
			this.cbOperator = new System.Windows.Forms.ComboBox();
			this.cbValue = new System.Windows.Forms.ComboBox();
			this.tbText = new System.Windows.Forms.TextBox();
			this.btnAnd = new System.Windows.Forms.Button();
			this.btnOr = new System.Windows.Forms.Button();
			this.tbResult = new System.Windows.Forms.TextBox();
			this.btnDown = new System.Windows.Forms.Button();
			this.btnNot = new System.Windows.Forms.Button();
			this.btnAddNestingParm = new System.Windows.Forms.Button();
			this.btnEdit = new System.Windows.Forms.Button();
			this.btnDelete = new System.Windows.Forms.Button();
			this.btnUp = new System.Windows.Forms.Button();
			this.label1 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.tbPK_Criteria = new System.Windows.Forms.TextBox();
			this.tbDescription = new System.Windows.Forms.TextBox();
			this.tbDefine = new System.Windows.Forms.TextBox();
			this.btnSave = new System.Windows.Forms.Button();
			this.btnLoad = new System.Windows.Forms.Button();
			this.button1 = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// Add
			// 
			this.Add.Location = new System.Drawing.Point(528, 16);
			this.Add.Name = "Add";
			this.Add.TabIndex = 0;
			this.Add.Text = "Add";
			this.Add.Click += new System.EventHandler(this.button1_Click);
			// 
			// cbCriteria
			// 
			this.cbCriteria.Location = new System.Drawing.Point(16, 16);
			this.cbCriteria.Name = "cbCriteria";
			this.cbCriteria.Size = new System.Drawing.Size(192, 21);
			this.cbCriteria.TabIndex = 1;
			this.cbCriteria.SelectedIndexChanged += new System.EventHandler(this.cbCriteria_SelectedIndexChanged);
			// 
			// cbOperator
			// 
			this.cbOperator.Location = new System.Drawing.Point(216, 16);
			this.cbOperator.Name = "cbOperator";
			this.cbOperator.Size = new System.Drawing.Size(80, 21);
			this.cbOperator.TabIndex = 2;
			// 
			// cbValue
			// 
			this.cbValue.Location = new System.Drawing.Point(304, 16);
			this.cbValue.Name = "cbValue";
			this.cbValue.Size = new System.Drawing.Size(216, 21);
			this.cbValue.TabIndex = 3;
			// 
			// tbText
			// 
			this.tbText.Location = new System.Drawing.Point(16, 48);
			this.tbText.Multiline = true;
			this.tbText.Name = "tbText";
			this.tbText.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
			this.tbText.Size = new System.Drawing.Size(592, 272);
			this.tbText.TabIndex = 4;
			this.tbText.Text = "";
			// 
			// btnAnd
			// 
			this.btnAnd.Location = new System.Drawing.Point(24, 336);
			this.btnAnd.Name = "btnAnd";
			this.btnAnd.TabIndex = 5;
			this.btnAnd.Text = "And";
			this.btnAnd.Click += new System.EventHandler(this.btnAnd_Click);
			// 
			// btnOr
			// 
			this.btnOr.Location = new System.Drawing.Point(112, 336);
			this.btnOr.Name = "btnOr";
			this.btnOr.TabIndex = 6;
			this.btnOr.Text = "Or";
			this.btnOr.Click += new System.EventHandler(this.btnOr_Click);
			// 
			// tbResult
			// 
			this.tbResult.Location = new System.Drawing.Point(512, 336);
			this.tbResult.Name = "tbResult";
			this.tbResult.TabIndex = 9;
			this.tbResult.Text = "";
			// 
			// btnDown
			// 
			this.btnDown.Location = new System.Drawing.Point(304, 368);
			this.btnDown.Name = "btnDown";
			this.btnDown.TabIndex = 11;
			this.btnDown.Text = "Down";
			this.btnDown.Click += new System.EventHandler(this.btnDown_Click);
			// 
			// btnNot
			// 
			this.btnNot.Location = new System.Drawing.Point(24, 368);
			this.btnNot.Name = "btnNot";
			this.btnNot.TabIndex = 12;
			this.btnNot.Text = "Not";
			this.btnNot.Click += new System.EventHandler(this.btnNot_Click);
			// 
			// btnAddNestingParm
			// 
			this.btnAddNestingParm.Location = new System.Drawing.Point(112, 368);
			this.btnAddNestingParm.Name = "btnAddNestingParm";
			this.btnAddNestingParm.Size = new System.Drawing.Size(136, 23);
			this.btnAddNestingParm.TabIndex = 13;
			this.btnAddNestingParm.Text = "Add Sub Parenthesis";
			this.btnAddNestingParm.Click += new System.EventHandler(this.btnAddNestingParm_Click);
			// 
			// btnEdit
			// 
			this.btnEdit.Location = new System.Drawing.Point(424, 336);
			this.btnEdit.Name = "btnEdit";
			this.btnEdit.TabIndex = 14;
			this.btnEdit.Text = "Edit";
			// 
			// btnDelete
			// 
			this.btnDelete.Location = new System.Drawing.Point(424, 368);
			this.btnDelete.Name = "btnDelete";
			this.btnDelete.TabIndex = 15;
			this.btnDelete.Text = "Delete";
			// 
			// btnUp
			// 
			this.btnUp.Location = new System.Drawing.Point(304, 336);
			this.btnUp.Name = "btnUp";
			this.btnUp.TabIndex = 16;
			this.btnUp.Text = "Up";
			this.btnUp.Click += new System.EventHandler(this.btnUp_Click);
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(24, 400);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(64, 16);
			this.label1.TabIndex = 17;
			this.label1.Text = "Description:";
			// 
			// label2
			// 
			this.label2.Location = new System.Drawing.Point(264, 400);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(40, 16);
			this.label2.TabIndex = 18;
			this.label2.Text = "Define:";
			// 
			// label3
			// 
			this.label3.Location = new System.Drawing.Point(56, 432);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(72, 16);
			this.label3.TabIndex = 19;
			this.label3.Text = "Criteria Num:";
			// 
			// tbPK_Criteria
			// 
			this.tbPK_Criteria.Location = new System.Drawing.Point(128, 432);
			this.tbPK_Criteria.Name = "tbPK_Criteria";
			this.tbPK_Criteria.TabIndex = 20;
			this.tbPK_Criteria.Text = "*new*";
			// 
			// tbDescription
			// 
			this.tbDescription.Location = new System.Drawing.Point(88, 398);
			this.tbDescription.Name = "tbDescription";
			this.tbDescription.Size = new System.Drawing.Size(160, 20);
			this.tbDescription.TabIndex = 21;
			this.tbDescription.Text = "";
			// 
			// tbDefine
			// 
			this.tbDefine.Location = new System.Drawing.Point(304, 398);
			this.tbDefine.Name = "tbDefine";
			this.tbDefine.TabIndex = 22;
			this.tbDefine.Text = "";
			// 
			// btnSave
			// 
			this.btnSave.Location = new System.Drawing.Point(416, 400);
			this.btnSave.Name = "btnSave";
			this.btnSave.Size = new System.Drawing.Size(104, 24);
			this.btnSave.TabIndex = 23;
			this.btnSave.Text = "Save";
			this.btnSave.Click += new System.EventHandler(this.btnSave_Click);
			// 
			// btnLoad
			// 
			this.btnLoad.Location = new System.Drawing.Point(240, 432);
			this.btnLoad.Name = "btnLoad";
			this.btnLoad.TabIndex = 24;
			this.btnLoad.Text = "Load";
			this.btnLoad.Click += new System.EventHandler(this.btnLoad_Click);
			// 
			// button1
			// 
			this.button1.Location = new System.Drawing.Point(544, 432);
			this.button1.Name = "button1";
			this.button1.TabIndex = 25;
			this.button1.Text = "button1";
			this.button1.Click += new System.EventHandler(this.button1_Click_1);
			// 
			// Form1
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(632, 470);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.button1,
																		  this.btnLoad,
																		  this.btnSave,
																		  this.tbDefine,
																		  this.tbDescription,
																		  this.tbPK_Criteria,
																		  this.label3,
																		  this.label2,
																		  this.label1,
																		  this.btnUp,
																		  this.btnDelete,
																		  this.btnEdit,
																		  this.btnAddNestingParm,
																		  this.btnNot,
																		  this.btnDown,
																		  this.tbResult,
																		  this.btnOr,
																		  this.btnAnd,
																		  this.tbText,
																		  this.cbValue,
																		  this.cbOperator,
																		  this.cbCriteria,
																		  this.Add});
			this.Name = "Form1";
			this.Text = "Form1";
			this.Load += new System.EventHandler(this.Form1_Load);
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			HADataConfiguration.m_mdsCache = new MyDataSet(null,null,true,true);
			HADataConfiguration.m_mdsCache.SetRelations();
			HAObjectFilter haof = new HAObjectFilter();
			Application.Run(new Form1(haof));
		}

		private void Form1_Load(object sender, System.EventArgs e)
		{
			//cbCriteria.DataSource = m_CriteriaList.alCriteriaParmList;
			//cbCriteria.DisplayMember = "Name";
			//cbCriteria.ValueMember = "ID";
			//bLoading=false;
			//updateOperatorsAndValues();
			//UpdateView();


		}

		private void cbCriteria_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			updateOperatorsAndValues();
		}

		private void updateOperatorsAndValues()
		{
			if( bLoading )
				return;

			cbOperator.Items.Clear();

			cbOperator.Items.Add(Operator.String(Operator.iEquals));
			cbOperator.Items.Add(Operator.String(Operator.iNotEquals));

			CriteriaParmList c = m_CriteriaList.GetFromID(Convert.ToInt32(cbCriteria.SelectedValue));

			if( c.m_bAllowText )
			{
				if( c.m_bAllowGLComparisson )
				{
					cbOperator.Items.Add(Operator.String(Operator.iGreaterThan));
					cbOperator.Items.Add(Operator.String(Operator.iLessThan));
				}
				// todo cbOperator.LimitToList=false;

			}
			cbOperator.SelectedIndex = 0;
			// else
			// todo cbOperator.LimitToList=true;

			cbValue.DataSource = c.m_alCriteriaValues;
			cbValue.DisplayMember = "Name";
			cbValue.ValueMember = "ID";
		}



		private void button1_Click(object sender, System.EventArgs e)
		{
			CriteriaParmList c = m_CriteriaList.GetFromID(Convert.ToInt32(cbCriteria.SelectedValue));
			CriteriaParm cp = new CriteriaParm(c,cbOperator.SelectedIndex+1,cbValue.SelectedValue.ToString());
			cp.m_ParentNestingParameter = m_CriteriaList.SelectedNestingParameter;
			m_CriteriaList.SelectedNestingParameter.alCriteriaParm.Add(cp);
			UpdateView();

			//goto the new line number.  bummer that we have to call UpdateView again :(  This could be better
			selectLineNumber(cp.m_DisplayLine.iLineNumber);
			UpdateView();

		}

		private void btnAnd_Click(object sender, System.EventArgs e)
		{
			m_CriteriaList.SelectedNestingParameter.bAnd=true;
			UpdateView();
		}

		private void btnOr_Click(object sender, System.EventArgs e)
		{
			m_CriteriaList.SelectedNestingParameter.bAnd=false;
			UpdateView();
		}






		private void btnAddNestingParm_Click(object sender, System.EventArgs e)
		{
			NestingParameter newNP = new NestingParameter(m_CriteriaList.SelectedNestingParameter);
			m_CriteriaList.SelectedNestingParameter.alNestingParameter.Add(newNP);
			newNP.iIndexOfParent = m_CriteriaList.SelectedNestingParameter.alNestingParameter.Count - 1;
			m_CriteriaList.SelectedNestingParameter = newNP;

			
			UpdateView();
			
			//goto the new line number.  bummer that we have to call UpdateView again :(  This could be better
			selectLineNumber(newNP.m_DisplayLine.iLineNumber);
			UpdateView();

		}


		private void btnUp_Click(object sender, System.EventArgs e)
		{
			if(PrevLineNumber()) UpdateView();
		}



		private void btnDown_Click(object sender, System.EventArgs e)
		{
			if(NextLineNumber()) UpdateView();
		}

		private void btnNot_Click(object sender, System.EventArgs e)
		{
			m_CriteriaList.SelectedNestingParameter.bNot = !m_CriteriaList.SelectedNestingParameter.bNot;
			UpdateView();
		}


		private void UpdateView()
		{
			DisplayLine oNextLine;

			int iCount;

			string sOut = "";
			m_CriteriaList.BuildLines();
			iCount = -1;
			foreach (DisplayLine oLine in m_CriteriaList.alLines)
			{
				iCount++;
				oNextLine = (DisplayLine) oLine;

				//if(oNextLine.bSelected)
				if(iCount == m_CriteriaList.iSelectedLineNumber)
				{
					sOut += "   ->\t";

					//enable or disable buttons depending on what type of line it is.
					if (oNextLine.iType == DisplayLineType.Criteria)
					{
						btnEdit.Enabled = true;
					}
					else
					{
						btnEdit.Enabled = false;
					}

				}
				else
				{
					sOut += "\t";
				}

				sOut += oNextLine.sText + "\r\n";
			}
			tbText.Text = sOut;
			
			tbResult.Text = m_CriteriaList.Evaluate().ToString();
		}



		public bool selectLineNumber(int iLineNumber)
		{
			if(iLineNumber >= 0 && iLineNumber <= m_CriteriaList.alLines.Count - 1)
			{
				m_CriteriaList.iSelectedLineNumber = iLineNumber;

				
				DisplayLine oDisplayLine = (DisplayLine) m_CriteriaList.alLines[m_CriteriaList.iSelectedLineNumber];

				if(oDisplayLine.iType == DisplayLineType.Criteria)
				{
					m_CriteriaList.SelectedNestingParameter = oDisplayLine.oCriteriaParm.m_ParentNestingParameter;
				}
				else
				{
					m_CriteriaList.SelectedNestingParameter = oDisplayLine.oNestingParameter;
				}

				return true;
			}
			else
			{
				return false;
			}
		}

		public bool NextLineNumber()
		{
			return selectLineNumber(m_CriteriaList.iSelectedLineNumber+1);
		}

		public bool PrevLineNumber()
		{
			return selectLineNumber(m_CriteriaList.iSelectedLineNumber-1);
		}

		private void btnSave_Click(object sender, System.EventArgs e)
		{
			if( tbDefine.Text.Length==0 )
				m_CriteriaList.SaveToDatabase(tbDescription.Text);
			else
				m_CriteriaList.SaveToDatabase(tbDescription.Text,tbDefine.Text);

			tbPK_Criteria.Text=m_CriteriaList.m_iPK_Criteria.ToString();
		}

		private void btnLoad_Click(object sender, System.EventArgs e)
		{
			int CriteriaNum = Convert.ToInt32(tbPK_Criteria.Text);
			m_CriteriaList.LoadFromDatabase(CriteriaNum);
			updateOperatorsAndValues();
			UpdateView();

		}

		private void button1_Click_1(object sender, System.EventArgs e)
		{
			EventCriteriaList objEventCriteriaBuilder;
			objEventCriteriaBuilder = new EventCriteriaList(10);
			objEventCriteriaBuilder.LoadFromDatabase(128);
			MessageBox.Show(objEventCriteriaBuilder.GetSerial());

		}
	}
}
