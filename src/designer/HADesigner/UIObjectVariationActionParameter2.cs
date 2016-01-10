using System;
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
		private int m_intCCommandParameterID;
		private int m_intVariableID = -1;
		private string m_strValue = "";
		private UIDesignObjVariation m_objParentUIDesignObjVariation;

		private string m_strDescription;		//this doesn't change


		private bool m_blnNeedsDBInsert = false;
		private bool m_blnNeedsDBDelete = false;
		private bool m_blnDeleted = false;			//this becomes true AFTER the delete


		//things that can change
		private int m_intVariableIDOriginal;
		private string m_strValueOriginal;




		//PROPERTIES
		public bool WarnToSave
		{
			get {return (NeedsInsert||NeedsDelete||OriginalsChanged);}
		}

		public int CCommandParameterID
		{	
			get
			{
				return m_intCCommandParameterID;
			}
			set
			{
				m_intCCommandParameterID = value;
			}
		}

		public int VariableID
		{
			get
			{
				return m_intVariableID;
			}
			set
			{
				m_intVariableID = value;
			}
		}

		public string Description
		{
			get
			{
				return m_strDescription;
			}
			set
			{
				m_strDescription = value;
			}
		}

		public string Value
		{
			get
			{
				return m_strValue;
			}
			set
			{
				m_strValue = value;
			}
		}

		public UIDesignObjVariation ParentUIDesignObjVariation
		{
			get
			{
				return m_objParentUIDesignObjVariation;
			}
			set
			{
				m_objParentUIDesignObjVariation = value;
			}
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
				return m_intVariableIDOriginal != m_intVariableID || m_strValueOriginal != m_strValue;
			}
		}




		//CONSTRUCTORS
		public UIDesignObjVariationCommandParameter(UIDesignObjVariation objUIDesignObjVariation, int intCCommandParameterID, bool blnLoadValues)
		{
			m_objParentUIDesignObjVariation = objUIDesignObjVariation;
			m_intCCommandParameterID = intCCommandParameterID;

			MyDataSet mds = HADataConfiguration.m_mdsCache;
			this.Description = mds.tCommandParameter[this.m_intCCommandParameterID].fDescription;

			//if the id is -1, it's not in the database
			if(objUIDesignObjVariation.ID != -1 && blnLoadValues)
			{
				LoadFromDatabase();
			}
		}


		public UIDesignObjVariationCommandParameter(UIDesignObjVariationCommand objUIDesignObjVariationCommand, int intCCommandParameterID, bool blnLoadValues)
		{
			m_objParentUIDesignObjVariation = objUIDesignObjVariation;
			m_intCCommandParameterID = intCCommandParameterID;

			MyDataSet mds = HADataConfiguration.m_mdsCache;
			this.Description = mds.tCommandParameter[this.m_intCCommandParameterID].fDescription;

			//if the id is -1, it's not in the database
			if(objUIDesignObjVariation.ID != -1 && blnLoadValues)
			{
				LoadFromDatabase();
			}
		}


		

		//METHODS
		public void LoadFromDatabase()
		{
			MyDataSet mds = HADataConfiguration.m_mdsCache;
			
/* sean			DesignObjVariation_CommandParameterDataRow drCommandParameter = mds.tDesignObjVariation_CommandParameter[this.ParentUIDesignObjVariation.ID, m_intCCommandParameterID];

			m_intVariableID = (drCommandParameter.fFK_VariableIsNull) ? -1 : drCommandParameter.fFK_Variable;
			m_strValue = (drCommandParameter.fValueIsNull) ? "" : drCommandParameter.fValue;
*/
			//this.Description = mds.tCommandParameter[this.m_intCCommandParameterID].fDescription;
		

			m_blnNeedsDBInsert = false;
			m_blnNeedsDBDelete = false;



			//set the originals
			this.ResetOriginals();

		}



		public bool SaveToDatabase()
		{
			//returns whether anything has been saved to the database
			bool blnChanged = false;

			MyDataSet mds = HADataConfiguration.m_mdsCache;
			

			if(!this.Deleted)
			{

				if(this.NeedsDelete)
				{
					if(!this.NeedsInsert)
					{
						/* sean
						//delete this CommandParameter
						DesignObjVariation_CommandParameterDataRow drCommandParameter = mds.tDesignObjVariation_CommandParameter[this.ParentUIDesignObjVariation.ID, this.CCommandParameterID];
						drCommandParameter.dr.Delete();
						mds.tDesignObjVariation_CommandParameter.Update(1,mds.m_conn,mds.m_trans);
						*/
					}

					blnChanged = true;
					m_blnDeleted = true;
				}
				else
				{
					if(this.NeedsInsert)
					{
/* sean
						//insert this object
						DesignObjVariation_CommandParameterDataRow drCommandParameter = new DesignObjVariation_CommandParameterDataRow(mds.tDesignObjVariation_CommandParameter.NewRow());

						drCommandParameter.fFK_CommandParameter = this.CCommandParameterID;
						drCommandParameter.fFK_DesignObjVariation = this.ParentUIDesignObjVariation.ID;


						if(this.VariableID == -1)
							drCommandParameter.fFK_VariableSetNull();
						else
							drCommandParameter.fFK_Variable = this.VariableID;

						if(this.Value == "")
							drCommandParameter.fValueSetNull();
						else
							drCommandParameter.fValue = this.Value;
					

						mds.tDesignObjVariation_CommandParameter.Rows.Add(drCommandParameter.dr);
*/

						blnChanged = true;

					}
					else
					{
					
						

						if(this.OriginalsChanged)
						{
/*   sean
							//update this object
							DesignObjVariation_CommandParameterDataRow drCommandParameter = mds.tDesignObjVariation_CommandParameter[this.ParentUIDesignObjVariation.ID, this.CCommandParameterID];
							
							if(this.VariableID == -1)
								drCommandParameter.fFK_VariableSetNull();
							else
								drCommandParameter.fFK_Variable = this.VariableID;

							if(this.Value == "")
								drCommandParameter.fValueSetNull();
							else
								drCommandParameter.fValue = this.Value;
*/
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
			m_intVariableIDOriginal = m_intVariableID;
			m_strValueOriginal = m_strValue;
		}



	}
}
