using System;
using HAData.Common;
using HAData.DataAccess;
using System.Data;


namespace HADesigner
{
	/// <summary>
	/// Summary description for UIDesignObjVariationParameter.
	/// </summary>
	public class UIDesignObjVariationParameter
	{
		

		//MEMBER VARIABLES
		private int m_intCParameterID;
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

		public int CParameterID
		{	
			get
			{
				return m_intCParameterID;
			}
			set
			{
				m_intCParameterID = value;
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
			set{
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
		public UIDesignObjVariationParameter(UIDesignObjVariation objUIDesignObjVariation, int intCParameterID)
		{
			m_objParentUIDesignObjVariation = objUIDesignObjVariation;
			m_intCParameterID = intCParameterID;

			MyDataSet mds = HADataConfiguration.m_mdsCache;
			this.Description = mds.tDesignObjParameter[this.m_intCParameterID].fDescription;


			//if the id is -1, it's not in the database
			if(objUIDesignObjVariation.ID != -1)
			{
				LoadFromDatabase();
			}
		}


		

		//METHODS
		public void LoadFromDatabase()
		{
			MyDataSet mds = HADataConfiguration.m_mdsCache;
			
			DesignObjVariation_DesignObjParameterDataRow drParameter = mds.tDesignObjVariation_DesignObjParameter[this.ParentUIDesignObjVariation.ID, m_intCParameterID];

			m_intVariableID = (drParameter.fFK_VariableIsNull) ? -1 : drParameter.fFK_Variable;
			m_strValue = (drParameter.fValueIsNull) ? "" : drParameter.fValue;

			//this.Description = mds.tDesignObjParameter[this.m_intCParameterID].fDescription;
		

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
					//delete this parameter
					DesignObjVariation_DesignObjParameterDataRow drParameter = mds.tDesignObjVariation_DesignObjParameter[this.ParentUIDesignObjVariation.ID, this.CParameterID];
					drParameter.dr.Delete();
					mds.tDesignObjVariation_DesignObjParameter.Update(1,mds.m_conn,mds.m_trans);
					}

					blnChanged = true;
					m_blnDeleted = true;
				}
				else
				{
					if(this.NeedsInsert)
					{
						//insert this object
						DesignObjVariation_DesignObjParameterDataRow drParameter = new DesignObjVariation_DesignObjParameterDataRow(mds.tDesignObjVariation_DesignObjParameter.NewRow());

						drParameter.fFK_DesignObjParameter = this.CParameterID;
						drParameter.fFK_DesignObjVariation = this.ParentUIDesignObjVariation.ID;


						if(this.VariableID == -1)
							drParameter.fFK_VariableSetNull();
						else
							drParameter.fFK_Variable = this.VariableID;

						if(this.Value == "")
							drParameter.fValueSetNull();
						else
							drParameter.fValue = this.Value;
					

						mds.tDesignObjVariation_DesignObjParameter.Rows.Add(drParameter.dr);


						blnChanged = true;

					}
					else
					{
					
						

						if(this.OriginalsChanged)
						{
							//update this object
							DesignObjVariation_DesignObjParameterDataRow drParameter = mds.tDesignObjVariation_DesignObjParameter[this.ParentUIDesignObjVariation.ID, this.CParameterID];
							
							if(this.VariableID == -1)
								drParameter.fFK_VariableSetNull();
							else
								drParameter.fFK_Variable = this.VariableID;

							if(this.Value == "")
								drParameter.fValueSetNull();
							else
								drParameter.fValue = this.Value;

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
