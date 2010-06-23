using System;
using System.Collections;
using HAData.Common;
using HAData.DataAccess;
using System.Data;

namespace HADesigner
{
	/// <summary>
	/// Summary description for UIDesignObjVariationCommand.
	/// </summary>
	public class UIDesignObjVariationCommand
	{

		//MEMBER VARIABLES
		//standard
		private bool m_blnNeedsDBInsert = false;
		private bool m_blnNeedsDBDelete = false;
		private bool m_blnDeleted = false;			//this becomes true AFTER the delete
		private string m_strDescription = "";

		private const int cnPK_Screen = 741;
		private const String csPK_CommandParam_Goto_Screen = "159";
		
		//specific
		private int m_intLinkID = -1;
		private int m_intCommandID = -1;
		private ArrayList m_alParameters = new ArrayList();
		private UIDesignObjVariationCommandGroup m_objParentUIDesignObjVariationCommandGroup;

		public int m_intOrderNum = -1;
		public int m_intMasterDeviceList = -1;
		public int m_intDeviceCategory = -1;
		public bool m_boolRelToSender = false;
		public int m_intBroadcast = -1;

		//things that can change
		private int m_intOrderNumOriginal = -1;
		private int m_intMasterDeviceListOriginal = -1;
		private int m_intDeviceCategoryOriginal = -1;
		private bool m_boolRelToSenderOriginal = false;
		private int m_intBroadcastOriginal = -1;



		//PROPERTIES
		//standard
		public bool WarnToSave
		{
			get
			{
				if(NeedsInsert||NeedsDelete||OriginalsChanged) return true;
				foreach(UIDesignObjVariationCommandParameter obj in this.Parameters)
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
				return m_intOrderNumOriginal != m_intOrderNum || 
					m_intMasterDeviceList != m_intMasterDeviceListOriginal || 
					m_intDeviceCategory != m_intDeviceCategoryOriginal ||
					m_boolRelToSender != m_boolRelToSenderOriginal ||
					m_intBroadcast != m_intBroadcastOriginal;
			}
		}

		public MyDataSet mds
		{
			get{return HADataConfiguration.m_mdsCache;}
		}

		
		//specific
		public int LinkID
		{
			get{ return m_intLinkID; }
			set{ m_intLinkID = value; }
		}
		public int CommandID
		{
			get{ return m_intCommandID;	}
			set{ m_intCommandID = value; }
		}
		
		public string Description
		{
			get{ return m_strDescription; }
			set{ m_strDescription = value; }
		}
		public ArrayList Parameters
		{
			get{ return m_alParameters; }
			set{ m_alParameters = value; }
		}

		public UIDesignObjVariationCommandGroup ParentUIDesignObjVariationCommandGroup
		{
			get{ return m_objParentUIDesignObjVariationCommandGroup; }
			set{ m_objParentUIDesignObjVariationCommandGroup = value; }
		}

		public int OrderNum
		{
			get{ return m_intOrderNum; }
			set{ m_intOrderNum = value; }
		}


		public UIDesignObjVariationCommand(UIDesignObjVariationCommandGroup objParentCommandGroup, int intLinkID, int intCommandID)
		{
			this.ParentUIDesignObjVariationCommandGroup = objParentCommandGroup;
			
			this.LinkID = intLinkID;
			
			if(this.LinkID == -1)
			{
				this.CommandID = intCommandID;
				this.NeedsInsert = true;

				//set the order num = to the max order num + 1
				this.OrderNum = this.ParentUIDesignObjVariationCommandGroup.GetMaxOrderNum() + 1;

			}
			else
			{
				LoadLinkFromDatabase();
			}

			LoadCommandFromDatabase();

			LoadParameters();


			//set the originals
			this.ResetOriginals();

		}





		//METHODS
		public void LoadCommandFromDatabase()
		{
			CommandDataRow drCommand = this.mds.tCommand[this.CommandID];
			this.Description = drCommand.fDescription;
		}


		public void LoadLinkFromDatabase()
		{
			CommandGroup_D_CommandDataRow drLink = this.mds.tCommandGroup_D_Command[this.LinkID];
			m_intMasterDeviceList = drLink.fFK_DeviceTemplateIsNull ?  -1 : Convert.ToInt32(drLink.fFK_DeviceTemplate);
			m_intDeviceCategory = drLink.fFK_DeviceCategoryIsNull ?  -1 : Convert.ToInt32(drLink.fFK_DeviceCategory);
			m_boolRelToSender = drLink.fRelativeToSenderIsNull ? false : drLink.fRelativeToSender;
			m_intBroadcast = drLink.fBroadcastLevelIsNull ? -1 : drLink.fBroadcastLevel;
			this.CommandID = drLink.fFK_Command;
			this.OrderNum = (drLink.fOrderNumIsNull) ? 1 : drLink.fOrderNum;
//			this.DeviceID = (drLink.fFK_DeviceIsNull) ? -1 : drLink.fFK_Device;
//			this.DeviceGroupID = (drLink.fFK_DeviceGroupIsNull) ? -1 : drLink.fFK_DeviceGroup;
		}


		public void LoadParameters()
		{
			//load all the parameters
			Command_CommandParameterDataRow drParameter;
			DataRow[] drParameters = this.mds.tCommand_CommandParameter.Select(Command_CommandParameterData.FK_COMMAND_FIELD + " = " + this.CommandID, null);
			foreach(DataRow dr in drParameters)
			{
				drParameter = new Command_CommandParameterDataRow(dr);
				this.Parameters.Add(new UIDesignObjVariationCommandParameter(this, drParameter.fFK_CommandParameter));
			}

			//also add extra parameters
			if(this.CommandID == cnPK_Screen)
			{
				String sPK_Screen = "0";
				CommandGroup_D_Command_CommandParameterDataRow drCommandGroup_D_Command_CommandParameter;
				String sCommandParamsSql = CommandGroup_D_Command_CommandParameterData.FK_COMMANDGROUP_D_COMMAND_FIELD + " = " + 
					this.LinkID + " AND " + 
					CommandGroup_D_Command_CommandParameterData.FK_COMMANDPARAMETER_FIELD + " = " + 
					csPK_CommandParam_Goto_Screen;

				DataRow[] drCommandGroup_D_Command_CommandParameters = 
					this.mds.tCommandGroup_D_Command_CommandParameter.Select(sCommandParamsSql);
				foreach(DataRow dr in drCommandGroup_D_Command_CommandParameters) //should be only one
				{
					drCommandGroup_D_Command_CommandParameter = new CommandGroup_D_Command_CommandParameterDataRow(dr);
					sPK_Screen = drCommandGroup_D_Command_CommandParameter.fIK_CommandParameter;
					break;
				}

				if(sPK_Screen != "")
				{
					try  // Screen may be non-numeric
					{
						Screen_CommandParameterDataRow drScreenParameter; 
						String sql = Screen_CommandParameterData.FK_SCREEN_FIELD + " = '" + sPK_Screen + "'";
						DataRow[] drScreenParameters = this.mds.tScreen_CommandParameter.Select(sql, null);
						foreach(DataRow dr in drScreenParameters)
						{
							drScreenParameter = new Screen_CommandParameterDataRow(dr);
							this.Parameters.Add(new UIDesignObjVariationCommandParameter(this, drScreenParameter.fFK_CommandParameter));
						}
					}
					catch(Exception)
					{
					}
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
						//first delete the action parameters
						foreach(UIDesignObjVariationCommandParameter obj in this.Parameters)
						{
							obj.NeedsDelete = true;
							obj.SaveToDatabase();
						}

						//delete this action
						//actually, we are removing a row from tCommandGroup_D_Command
						CommandGroup_D_CommandDataRow drCommand = this.mds.tCommandGroup_D_Command[this.LinkID];
						drCommand.dr.Delete();
						this.mds.tCommandGroup_D_Command.Update(1,mds.m_conn,mds.m_trans);

						blnChanged = true;
					}

					
					m_blnDeleted = true;
				}
				else
				{
					if(this.NeedsInsert)
					{
						//insert this action
						CommandGroup_D_CommandDataRow drCommand = new CommandGroup_D_CommandDataRow(this.mds.tCommandGroup_D_Command.NewRow());

						drCommand.fFK_Command = this.CommandID;
						drCommand.fFK_CommandGroup_D = this.ParentUIDesignObjVariationCommandGroup.CommandGroupID;
						drCommand.fOrderNum = this.OrderNum;

						if( m_intMasterDeviceList == -1 )
							drCommand.fFK_DeviceTemplateSetNull();
						else
							drCommand.fFK_DeviceTemplate = m_intMasterDeviceList;

						if( m_intDeviceCategory == -1 )
							drCommand.fFK_DeviceCategorySetNull();
						else
							drCommand.fFK_DeviceCategory = m_intDeviceCategory;

						drCommand.fRelativeToSender = m_boolRelToSender;

						if( m_intBroadcast == -1 )
							drCommand.fBroadcastLevelSetNull();
						else
							drCommand.fBroadcastLevel = m_intBroadcast;

/*
						if(this.DeviceID == -1)
						{
							drCommand.fFK_DeviceSetNull();
						}
						else
						{
							drCommand.fFK_Device = this.DeviceID;
						}

						if(this.DeviceGroupID == -1)
						{
							drCommand.fFK_DeviceGroupSetNull();
						}
						else
						{
							drCommand.fFK_DeviceGroup = this.DeviceGroupID;
						}
*/
						this.mds.tCommandGroup_D_Command.Rows.Add(drCommand.dr);

						this.mds.tCommandGroup_D_Command.Update(1,mds.m_conn,mds.m_trans);

						this.LinkID = drCommand.fPK_CommandGroup_D_Command;


						blnChanged = true;

					}
					else
					{
						if(this.OriginalsChanged)
						{
							//update this action
							CommandGroup_D_CommandDataRow drCommand = this.mds.tCommandGroup_D_Command[this.LinkID];
							
							drCommand.fOrderNum = this.OrderNum;
							if( m_intMasterDeviceList == -1 )
								drCommand.fFK_DeviceTemplateSetNull();
							else
								drCommand.fFK_DeviceTemplate = m_intMasterDeviceList;

							if( m_intDeviceCategory == -1 )
								drCommand.fFK_DeviceCategorySetNull();
							else
								drCommand.fFK_DeviceCategory = m_intDeviceCategory;

							drCommand.fRelativeToSender = m_boolRelToSender;

							if( m_intBroadcast == -1 )
								drCommand.fBroadcastLevelSetNull();
							else
								drCommand.fBroadcastLevel = m_intBroadcast;

							/*
							if(this.DeviceID == -1)
							{
								drCommand.fFK_DeviceSetNull();
							}
							else
							{
								drCommand.fFK_Device = this.DeviceID;
							}

							if(this.DeviceGroupID == -1)
							{
								drCommand.fFK_DeviceGroupSetNull();
							}
							else
							{
								drCommand.fFK_DeviceGroup = this.DeviceGroupID;
							}
							*/

							blnChanged = true;

						}
					}


					foreach(UIDesignObjVariationCommandParameter obj in this.Parameters)
					{
						blnChanged = obj.SaveToDatabase() || blnChanged;
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
			m_intOrderNumOriginal = m_intOrderNum;
			m_intMasterDeviceListOriginal = m_intMasterDeviceList;
			m_intDeviceCategoryOriginal = m_intDeviceCategory;
			m_boolRelToSenderOriginal = m_boolRelToSender;
			m_intBroadcastOriginal = m_intBroadcast;
		}



	}
}
