using System;
using System.Collections;
using HAData.Common;
using HAData.DataAccess;
using System.Data;

namespace HADesigner
{
	/// <summary>
	/// Summary description for UIDesignObjVariationCommandGroup.
	/// </summary>
	public class UIDesignObjVariationCommandGroup
	{


		//MEMBER VARIABLES
		//standard
		private bool m_blnNeedsDBInsert = false;
		private bool m_blnNeedsDBDelete = false;
		private bool m_blnDeleted = false;			//this becomes true AFTER the delete

		
		//specific
		private int m_intCommandGroupID = -1;
		private ArrayList m_alCommands = new ArrayList();
		private UIDesignObjVariation m_objParentUIDesignObjVariation;



		//things that can change






		//PROPERTIES
		//standard
		public bool WarnToSave
		{
			get
			{
				if(NeedsInsert||NeedsDelete||OriginalsChanged) return true;
				foreach(UIDesignObjVariationCommand obj in this.Commands)
				{
					if(obj.WarnToSave) return true;
				}
				return false;
			}
		}
		public bool NeedsInsert
		{	
			get
			{
				bool blnReturn = false;
				//only insert if there are actions
				if(m_blnNeedsDBInsert)
				{
					foreach(UIDesignObjVariationCommand objCommand in this.Commands)
					{
						if(objCommand.Include)
						{
							blnReturn  = true;
							break;
						}
					}
				}
				return blnReturn;
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
				//return m_intVariableIDOriginal != m_intVariableID || m_strValueOriginal != m_strValue;
				return false;
			}
		}

		public MyDataSet mds
		{
			get{return HADataConfiguration.m_mdsCache;}
		}

		
		//specific
		public int CommandGroupID
		{
			get{ return m_intCommandGroupID;	}
			set{ m_intCommandGroupID = value; }
		}
		
		public ArrayList Commands
		{
			get{ return m_alCommands; }
			set{ m_alCommands = value; }
		}

		public UIDesignObjVariation ParentUIDesignObjVariation
		{
			get{ return m_objParentUIDesignObjVariation; }
			set{ m_objParentUIDesignObjVariation = value; }
		}



		//CONSTRUCTORS
		public UIDesignObjVariationCommandGroup(UIDesignObjVariation objParent, int intCommandGroupID)
		{
			this.ParentUIDesignObjVariation = objParent;

			if(intCommandGroupID == -1)
			{
				this.NeedsInsert = true;
			}
			else
			{
				this.CommandGroupID = intCommandGroupID;
				LoadFromDatabase();
			}

		}
		public UIDesignObjVariationCommandGroup(int intCommandGroupID)
		{

			if(intCommandGroupID == -1)
			{
				this.NeedsInsert = true;
			}
			else
			{
				this.CommandGroupID = intCommandGroupID;
				LoadFromDatabase();
			}

		}



		//METHODS
		public void LoadFromDatabase()
		{


			//load all the actions
			CommandGroup_D_CommandDataRow drCommand;
			DataRow[] drCommands = this.mds.tCommandGroup_D_Command.Select(CommandGroup_D_CommandData.FK_COMMANDGROUP_D_FIELD + " = " + this.CommandGroupID, CommandGroup_D_CommandData.ORDERNUM_FIELD);
			foreach(DataRow dr in drCommands)
			{
				drCommand = new CommandGroup_D_CommandDataRow(dr);
				this.Commands.Add(new UIDesignObjVariationCommand(this, drCommand.fPK_CommandGroup_D_Command, -1));
			}


			//set the originals
			this.ResetOriginals();


			//clean up the order nums if needed, ie make sure there are no duplicates
			bool blnReOrdered = false;
			UIDesignObjVariationCommand objPreviousCommand = null;
			foreach(UIDesignObjVariationCommand obj in this.Commands)
			{
				if(objPreviousCommand != null)
				{
					if(objPreviousCommand.OrderNum >= obj.OrderNum)
					{
						obj.OrderNum = objPreviousCommand.OrderNum + 1;
						blnReOrdered = true;
					}
				}
				objPreviousCommand = obj;
			}
			if(blnReOrdered){ this.SaveToDatabase(); }



		}



		public bool SaveToDatabase()
		{
			//returns whether anything has been saved to the database
			bool blnChanged = false;


			
			//if there are no actions then delete the action group
			bool blnAtleastOneIncluded = false;
			foreach(UIDesignObjVariationCommand obj in this.Commands)
			{
				if(obj.Include){blnAtleastOneIncluded = true;}
			}
			if(!blnAtleastOneIncluded){ this.NeedsDelete = true; }



			if(!this.Deleted)
			{
				if(this.NeedsDelete)
				{
					//if(!this.NeedsInsert)
					//here we need to use the variable and not the property
					//because the property takes in to consideration whether there are actions
					if(!m_blnNeedsDBInsert)
					{


						//remove the actions first
						foreach(UIDesignObjVariationCommand obj in this.Commands)
						{
							obj.NeedsDelete = true;
							obj.SaveToDatabase();
						}


						//delete this action group
			
						

						blnChanged = true;

					}

					
					m_blnDeleted = true;
				}
				else
				{
					if(this.NeedsInsert)
					{
						//insert this object
						CommandGroup_DDataRow drCommandGroup = new CommandGroup_DDataRow(this.mds.tCommandGroup_D.NewRow());
						this.mds.tCommandGroup_D.Rows.Add(drCommandGroup.dr);
						this.mds.tCommandGroup_D.Update(1,mds.m_conn,mds.m_trans);
						this.CommandGroupID = drCommandGroup.fPK_CommandGroup_D;

						blnChanged = true;

					}
					else
					{
						if(this.OriginalsChanged)
						{
							//update this object
							//nothing can change at this point


							blnChanged = true;

						}
					}

				}

				//set the originals so we know how and when to save
				this.ResetOriginals();


				this.NeedsInsert = false;
				this.NeedsDelete = false;




				//update the actions
				UIDesignObjVariationCommand objCommand;
				foreach(Object obj in this.Commands)
				{
					objCommand = (UIDesignObjVariationCommand) obj;
					blnChanged = objCommand.SaveToDatabase() || blnChanged;
				}


			}



			return blnChanged;

		}


		public int GetMaxOrderNum()
		{
			int intMaxOrderNum = 0;
			foreach(UIDesignObjVariationCommand obj in this.Commands)
			{
				if(obj.OrderNum > intMaxOrderNum){ intMaxOrderNum = obj.OrderNum; }
			}
			return intMaxOrderNum;
		}


		private void ResetOriginals()
		{

		}




	}
}
