using System;
using System.Collections;
using HAData.Common;
using HAData.DataAccess;
using System.Data;

namespace HADesigner
{
	/// <summary>
	/// Summary description for UIDesignObjVariationCommandParameter.
	/// </summary>
	public class UIDesignObjVariationCommandParameter
	{
		//MEMBER VARIABLES
		//standard
		private bool m_blnNeedsDBInsert = false;
		private bool m_blnNeedsDBDelete = false;
		private bool m_blnDeleted = false;			//this becomes true AFTER the delete

		//specific
		private int m_intParameterID = -1;
		private string m_strDescription = "";
		private UIDesignObjVariationCommand m_objParentUIDesignObjVariationCommand;

		private string m_strValue = "";


		//things that can change
		private string m_strValueOriginal = "";

		//PROPERTIES
		//standard
		public bool WarnToSave
		{
			get {return (NeedsInsert||NeedsDelete||OriginalsChanged);}
		}
		public bool NeedsInsert
		{	
			get
			{
				return m_blnNeedsDBInsert;
			}
			set
			{
				m_blnNeedsDBInsert = value;
			}
		}
		public bool NeedsDelete
		{
			get
			{
				return m_blnNeedsDBDelete;
			}
			set
			{
				m_blnNeedsDBDelete = value;
			}
		}
		public bool Deleted
		{
			get
			{
				return m_blnDeleted;
			}
		}

		public bool Include
		{
			get
			{
				return (!this.Deleted && !this.NeedsDelete);
			}
		}

		public bool OriginalsChanged
		{
			get
			{
				return m_strValueOriginal != m_strValue;
			}
		}

		public MyDataSet mds
		{
			get{return HADataConfiguration.m_mdsCache;}
		}

		
		//specific
		public int ParameterID
		{
			get{ return m_intParameterID;	}
			set{ m_intParameterID = value; }
		}
		public string Description
		{
			get{ return m_strDescription; }
			set{ m_strDescription = value; }
		}
		
		public UIDesignObjVariationCommand ParentUIDesignObjVariationCommand
		{
			get{ return m_objParentUIDesignObjVariationCommand; }
			set{ m_objParentUIDesignObjVariationCommand = value; }
		}


		public string Value
		{
			get{ return m_strValue; }
			set{ m_strValue = value; }
		}	


		public UIDesignObjVariationCommandParameter(UIDesignObjVariationCommand objParentCommand, int intParameterID)
		{
			this.ParentUIDesignObjVariationCommand = objParentCommand;
			this.ParameterID = intParameterID;

			LoadFromDatabase();

			//set the originals
			this.ResetOriginals();

		}



		//METHODS
		public void LoadFromDatabase()
		{
			string strErrorMessage = "";


			CommandParameterDataRow drCommandParameter = mds.tCommandParameter[this.ParameterID];
			this.Description = drCommandParameter.fDescription;


			if(this.ParentUIDesignObjVariationCommand.LinkID == -1)
			{
				//parent action isn't in database, so this parameter isn't in the database either
				this.NeedsInsert = true;
			}
			else
			{
				//see if there is a parameter existant
				try
				{
					CommandGroup_D_Command_CommandParameterDataRow drParameter = this.mds.tCommandGroup_D_Command_CommandParameter[this.ParentUIDesignObjVariationCommand.LinkID, this.ParameterID];
					this.Value = (drParameter.fIK_CommandParameterIsNull) ? "" : drParameter.fIK_CommandParameter;
				}
				catch(Exception e)
				{
					strErrorMessage = e.Message;
					this.NeedsInsert = true;
				}
			}

		}



		public bool SaveToDatabase()
		{
			//returns whether anything has been saved to the database
			bool blnChanged = false;



			if(!this.Deleted)
			{
				if(this.NeedsDelete)
				{
					if(!this.NeedsInsert)
					{
						//delete this parameter
						
						CommandGroup_D_Command_CommandParameterDataRow drParameter = this.mds.tCommandGroup_D_Command_CommandParameter[this.ParentUIDesignObjVariationCommand.LinkID, this.ParameterID];

						drParameter.dr.Delete();

						this.mds.tCommandGroup_D_Command_CommandParameter.Update(1,mds.m_conn,mds.m_trans);

						blnChanged = true;

					}

					
					m_blnDeleted = true;
				}
				else
				{
					if(this.NeedsInsert)
					{
						//insert this parameter

						CommandGroup_D_Command_CommandParameterDataRow drParameter = new CommandGroup_D_Command_CommandParameterDataRow(this.mds.tCommandGroup_D_Command_CommandParameter.NewRow());

						drParameter.fFK_CommandGroup_D_Command = this.ParentUIDesignObjVariationCommand.LinkID;
						drParameter.fFK_CommandParameter = this.ParameterID;

						if(this.Value == "")
						{
							drParameter.fIK_CommandParameterSetNull();
						}
						else
						{	
							drParameter.fIK_CommandParameter = this.Value;
						}
						
						this.mds.tCommandGroup_D_Command_CommandParameter.Rows.Add(drParameter.dr);

						this.mds.tCommandGroup_D_Command_CommandParameter.Update(1,mds.m_conn,mds.m_trans);


						blnChanged = true;

					}
					else
					{

						if(this.OriginalsChanged)
						{
							//update this parameter

							CommandGroup_D_Command_CommandParameterDataRow drParameter = mds.tCommandGroup_D_Command_CommandParameter[this.ParentUIDesignObjVariationCommand.LinkID, this.ParameterID];

							if(this.Value == "")
							{
								drParameter.fIK_CommandParameterSetNull();
							}
							else
							{	
								drParameter.fIK_CommandParameter = this.Value;
							}


							blnChanged = true;

						}
					}

				}


				//set the originals so we know how and when to save
				this.ResetOriginals();


				this.NeedsInsert = false;
				this.NeedsDelete = false;

			}



			return blnChanged;

		}


		private void ResetOriginals()
		{
			m_strValueOriginal = this.Value;
		}

	}
}
