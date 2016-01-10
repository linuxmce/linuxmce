namespace HAData.DataAccess {
	using System;
	using System.Data;
	using System.IO;
	using MySql;
	using MySql.Data;
	using MySql.Data.MySqlClient;
	using System.Diagnostics;
	using HAData.Common;

	public class MyDataSet : SerializableDataSet
	{
		bool m_bIsCache=false;
		MyDataSet m_mdsCache=null;
		public MySqlConnection m_conn=null;
		public MySqlTransaction m_trans=null;
		public MyDataSet()
		{
				m_conn = HADataConfiguration.GetMySqlConnection();
		}
		public MyDataSet(MySqlConnection conn, MySqlTransaction trans,bool bKeepConnection,bool bCache)
		{
			if( conn==null && (bKeepConnection || bCache) )
			{
				conn = HADataConfiguration.GetMySqlConnection();
			}

			if( bKeepConnection )
			{
				m_conn=conn;
				m_trans=trans;
			}
			if( bCache )
			{
				tArray.LoadAll(conn,trans);
				tButton.LoadAll(conn,trans);
				tCommand.LoadAll(conn,trans);
				tCommand_CommandParameter.LoadAll(conn,trans);
				tCommandCategory.LoadAll(conn,trans);
				tCommandGroup_D.LoadAll(conn,trans);
				tCommandGroup_D_Command.LoadAll(conn,trans);
				tCommandGroup_D_Command_CommandParameter.LoadAll(conn,trans);
				tCommandParameter.LoadAll(conn,trans);
				tUI.LoadAll(conn,trans);
				tCriteriaList.LoadAll(conn,trans);
				tCriteriaList_CriteriaParmList.LoadAll(conn,trans);
				tCriteriaParmList.LoadAll(conn,trans);
				tDesignObj.LoadAll(conn,trans);
				tDesignObjCategory.LoadAll(conn,trans);
				tDesignObjParameter.LoadAll(conn,trans);
				tDesignObjType.LoadAll(conn,trans);
				tDesignObjType_DesignObjParameter.LoadAll(conn,trans);
				tDesignObjVariation.LoadAll(conn,trans);
				tDesignObjVariation_DesignObj.LoadAll(conn,trans);
				tDesignObjVariation_DesignObj_Skin_Language.LoadAll(conn,trans);
				tDesignObjVariation_DesignObjParameter.LoadAll(conn,trans);
				tDesignObjVariation_Text.LoadAll(conn,trans);
				tDesignObjVariation_Text_Skin_Language.LoadAll(conn,trans);
				tDesignObjVariation_Zone.LoadAll(conn,trans);
				tDeviceTemplate.LoadAll(conn,trans);
				tEffectType.LoadAll(conn, trans);
				tHorizAlignment.LoadAll(conn,trans);
				tIcon.LoadAll(conn,trans);
				tInstallation.LoadAll(conn,trans);
				tLanguage.LoadAll(conn,trans);
				tParameterType.LoadAll(conn,trans);
				tSize.LoadAll(conn,trans);
				tSkin.LoadAll(conn,trans);
				tStyle.LoadAll(conn,trans);
				tStyleVariation.LoadAll(conn,trans);
				tSystem.LoadAll(conn,trans);
				tScreen_CommandParameter.LoadAll(conn,trans);
				tText.LoadAll(conn,trans);
				tText_LS.LoadAll(conn,trans);
				tText_LS_AltVersions.LoadAll(conn,trans);
				tTextCategory.LoadAll(conn,trans);
				tUserMode.LoadAll(conn,trans);
				tUsers.LoadAll(conn,trans);
				tVariable.LoadAll(conn,trans);
				tVersion.LoadAll(conn,trans);
				tVertAlignment.LoadAll(conn,trans);
				m_bIsCache=true;
			}
		}
		public MyDataSet(MySqlConnection conn, MySqlTransaction trans,bool bKeepConnection,ref MyDataSet cache)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetMySqlConnection();
			}

			if( bKeepConnection )
			{
				m_conn=conn;
				m_trans=trans;
			}
			m_mdsCache = cache;
		}
		public void BuildTables()
		{
			Tables.Add(ArrayData.BuildDataTables());
			Tables.Add(ButtonData.BuildDataTables());
			Tables.Add(CommandData.BuildDataTables());
			Tables.Add(Command_CommandParameterData.BuildDataTables());
			Tables.Add(CommandCategoryData.BuildDataTables());
			Tables.Add(CommandGroup_DData.BuildDataTables());
			Tables.Add(CommandGroup_D_CommandData.BuildDataTables());
			Tables.Add(CommandGroup_D_Command_CommandParameterData.BuildDataTables());
			Tables.Add(CommandParameterData.BuildDataTables());
			Tables.Add(UIData.BuildDataTables());
			Tables.Add(CriteriaListData.BuildDataTables());
			Tables.Add(CriteriaList_CriteriaParmListData.BuildDataTables());
			Tables.Add(CriteriaParmListData.BuildDataTables());
			Tables.Add(DesignObjData.BuildDataTables());
			Tables.Add(DesignObjCategoryData.BuildDataTables());
			Tables.Add(DesignObjParameterData.BuildDataTables());
			Tables.Add(DesignObjTypeData.BuildDataTables());
			Tables.Add(DesignObjType_DesignObjParameterData.BuildDataTables());
			Tables.Add(DesignObjVariationData.BuildDataTables());
			Tables.Add(DesignObjVariation_DesignObjData.BuildDataTables());
			Tables.Add(DesignObjVariation_DesignObj_Skin_LanguageData.BuildDataTables());
			Tables.Add(DesignObjVariation_DesignObjParameterData.BuildDataTables());
			Tables.Add(DesignObjVariation_TextData.BuildDataTables());
			Tables.Add(DesignObjVariation_Text_Skin_LanguageData.BuildDataTables());
			Tables.Add(DesignObjVariation_ZoneData.BuildDataTables());
			Tables.Add(DeviceTemplateData.BuildDataTables());
			Tables.Add(HorizAlignmentData.BuildDataTables());
			Tables.Add(IconData.BuildDataTables());
			Tables.Add(InstallationData.BuildDataTables());
			Tables.Add(LanguageData.BuildDataTables());
			Tables.Add(ParameterTypeData.BuildDataTables());
			Tables.Add(SizeData.BuildDataTables());
			Tables.Add(SkinData.BuildDataTables());
			Tables.Add(StyleData.BuildDataTables());
			Tables.Add(StyleVariationData.BuildDataTables());
			Tables.Add(SystemData.BuildDataTables());
			Tables.Add(TextData.BuildDataTables());
			Tables.Add(Text_LSData.BuildDataTables());
			Tables.Add(Text_LS_AltVersionsData.BuildDataTables());
			Tables.Add(TextCategoryData.BuildDataTables());
			Tables.Add(UserModeData.BuildDataTables());
			Tables.Add(UsersData.BuildDataTables());
			Tables.Add(VariableData.BuildDataTables());
			Tables.Add(VersionData.BuildDataTables());
			Tables.Add(VertAlignmentData.BuildDataTables());
		}
		public void Serialize(bool bSave,FileStream fs)
		{
			if( bSave )
			{
				StreamWriter s = new StreamWriter(fs);
				s.WriteLine(Tables.Count); // num tables
				foreach(DataTable dt in Tables)
				{
					int NumColumns = dt.Columns.Count;
					s.WriteLine(dt.TableName);
					s.WriteLine(NumColumns); // num columns
					foreach(DataColumn dc in dt.Columns)
						s.WriteLine(dc.ColumnName);
			
					int NumRows = dt.Rows.Count;
					s.WriteLine(NumRows); // num rows
					for(int ir=0;ir<NumRows;ir++)
					{
						for(int c=0;c<NumColumns;c++)  // Num Columns
						{
							string str = dt.Rows[ir][c].ToString();  // Table num
							s.WriteLine(str.Length);
							if( str.Length>0 )
								s.Write(str);
						}
					}
				}
				s.Close();
			}
			else
			{
				StreamReader s = new StreamReader(fs);
				int NumTables = Convert.ToInt32(s.ReadLine());
				for(int t = 0;t<NumTables;t++ )
				{
					string TableName = s.ReadLine();
					int TableNum = Tables.IndexOf(TableName);
					DataTable dt = Tables[TableNum];
					DataColumn[] pk = dt.PrimaryKey;

					int NumColumns = Convert.ToInt32(s.ReadLine());
					int[] ColumnMapping = new int[NumColumns];

					for(int c=0;c<NumColumns;c++)
					{
						string ColName = s.ReadLine();
						ColumnMapping[c] = Tables[TableNum].Columns.IndexOf(ColName);
					}

					// At least the primary keys have to be the same
					for(int c=0;c<pk.Length;c++)
						Debug.Assert(ColumnMapping[c]==c);

					int NumRows = Convert.ToInt32(s.ReadLine());

					for(int ir=0;ir<NumRows;ir++)
					{
						object[] oVals = new object[NumColumns];
						for(int ic=0;ic<NumColumns;ic++)
						{
							int Length = Convert.ToInt32(s.ReadLine());
							if( Length==0 )
								oVals[ic]=null;
							else
							{
								char[] buffer = new char[Length];
								s.Read(buffer,0,Length);
								oVals[ic]=new string(buffer);
							}
						}

						object[]findTheseVals = new object[pk.Length];
						for(int c=0;c<pk.Length;c++)
						{
							findTheseVals[c] = oVals[c];
						}

						bool bNew=false;
						DataRow dr=dt.Rows.Find(findTheseVals);
						if( dr==null )
						{
							dr = dt.NewRow();
							bNew=true;
						}
						for(int c=0;c<NumColumns;c++)
						{
							if( oVals[c]!=null )
							{
								dr[ColumnMapping[c]] = oVals[c];
							}
						}
						if( bNew )
							dt.Rows.Add(dr);
					}
				}
				s.Close();
			}
		}

		public void Update(int PK_Users)
		{
			if( m_conn==null )
				return;
			Update(PK_Users,m_conn,m_trans);
		}
		public void Update(int PK_Users,MySqlConnection conn,MySqlTransaction trans)
		{
			if( conn==null )
				return;
			DataTable dt;
			MyDataSet ds = this;
			dt=Tables["Array"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				ArrayData.UpdateArray(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["Button"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				ButtonData.UpdateButton(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["Command"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				CommandData.UpdateCommand(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["Command_CommandParameter"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				Command_CommandParameterData.UpdateCommand_CommandParameter(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["CommandCategory"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				CommandCategoryData.UpdateCommandCategory(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["CommandGroup_D"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				CommandGroup_DData.UpdateCommandGroup_D(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["CommandGroup_D_Command"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				CommandGroup_D_CommandData.UpdateCommandGroup_D_Command(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["CommandGroup_D_Command_CommandParameter"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				CommandGroup_D_Command_CommandParameterData.UpdateCommandGroup_D_Command_CommandParameter(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["CommandParameter"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				CommandParameterData.UpdateCommandParameter(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["UI"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				UIData.UpdateUI(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["CriteriaList"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				CriteriaListData.UpdateCriteriaList(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["CriteriaList_CriteriaParmList"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				CriteriaList_CriteriaParmListData.UpdateCriteriaList_CriteriaParmList(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["CriteriaParmList"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				CriteriaParmListData.UpdateCriteriaParmList(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["DesignObj"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				DesignObjData.UpdateDesignObj(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["DesignObjCategory"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				DesignObjCategoryData.UpdateDesignObjCategory(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["DesignObjParameter"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				DesignObjParameterData.UpdateDesignObjParameter(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["DesignObjType"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				DesignObjTypeData.UpdateDesignObjType(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["DesignObjType_DesignObjParameter"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				DesignObjType_DesignObjParameterData.UpdateDesignObjType_DesignObjParameter(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["DesignObjVariation"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				DesignObjVariationData.UpdateDesignObjVariation(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["DesignObjVariation_DesignObj"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				DesignObjVariation_DesignObjData.UpdateDesignObjVariation_DesignObj(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["DesignObjVariation_DesignObj_Skin_Language"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
					DesignObjVariation_DesignObj_Skin_LanguageData.UpdateDesignObjVariation_DesignObj_Skin_Language(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["DesignObjVariation_DesignObjParameter"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				DesignObjVariation_DesignObjParameterData.UpdateDesignObjVariation_DesignObjParameter(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["DesignObjVariation_Text"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				DesignObjVariation_TextData.UpdateDesignObjVariation_Text(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["DesignObjVariation_Text_Skin_Language"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				DesignObjVariation_Text_Skin_LanguageData.UpdateDesignObjVariation_Text_Skin_Language(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["DesignObjVariation_Zone"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				DesignObjVariation_ZoneData.UpdateDesignObjVariation_Zone(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["DeviceTemplate"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				DeviceTemplateData.UpdateDeviceTemplate(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["HorizAlignment"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				HorizAlignmentData.UpdateHorizAlignment(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["Icon"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				IconData.UpdateIcon(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["Installation"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				InstallationData.UpdateInstallation(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["Language"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				LanguageData.UpdateLanguage(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["ParameterType"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				ParameterTypeData.UpdateParameterType(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["Size"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				SizeData.UpdateSize(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["Skin"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				SkinData.UpdateSkin(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["Style"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				StyleData.UpdateStyle(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["StyleVariation"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				StyleVariationData.UpdateStyleVariation(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["System"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				SystemData.UpdateSystem(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["Text"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				TextData.UpdateText(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["Text_LS_AltVersions"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				Text_LS_AltVersionsData.UpdateText_LS_AltVersions(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["TextCategory"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				TextCategoryData.UpdateTextCategory(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["UserMode"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				UserModeData.UpdateUserMode(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["Users"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				UsersData.UpdateUsers(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["Variable"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				VariableData.UpdateVariable(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["Version"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				VersionData.UpdateVersion(ref ds,PK_Users,conn,trans);
				}
			}
			dt=Tables["VertAlignment"];
			if( dt!=null )
			{
				if( !dt.Rows.IsSynchronized )
				{
				VertAlignmentData.UpdateVertAlignment(ref ds,PK_Users,conn,trans);
				}
			}
		}
		public ArrayTable tArray
		{
			get
			{
				if( m_bIsCache )
					return (ArrayTable) Tables["Array"];
				if( m_mdsCache!=null )
					return (ArrayTable) m_mdsCache.Tables["Array"];
				ArrayTable dt = (ArrayTable)Tables["Array"];
				if( dt==null )
				{
					dt = ArrayData.BuildArrayTable();
					Tables.Add(dt);
				}
				return (ArrayTable) dt;
			}
		}
		public ButtonTable tButton
		{
			get
			{
				if( m_bIsCache )
					return (ButtonTable) Tables["Button"];
				if( m_mdsCache!=null )
					return (ButtonTable) m_mdsCache.Tables["Button"];
				ButtonTable dt = (ButtonTable)Tables["Button"];
				if( dt==null )
				{
					dt = ButtonData.BuildButtonTable();
					Tables.Add(dt);
				}
				return (ButtonTable) dt;
			}
		}
		public CommandTable tCommand
		{
			get
			{
				if( m_bIsCache )
					return (CommandTable) Tables["Command"];
				if( m_mdsCache!=null )
					return (CommandTable) m_mdsCache.Tables["Command"];
				CommandTable dt = (CommandTable)Tables["Command"];
				if( dt==null )
				{
					dt = CommandData.BuildCommandTable();
					Tables.Add(dt);
				}
				return (CommandTable) dt;
			}
		}
		public Command_CommandParameterTable tCommand_CommandParameter
		{
			get
			{
				if( m_bIsCache )
					return (Command_CommandParameterTable) Tables["Command_CommandParameter"];
				if( m_mdsCache!=null )
					return (Command_CommandParameterTable) m_mdsCache.Tables["Command_CommandParameter"];
				Command_CommandParameterTable dt = (Command_CommandParameterTable)Tables["Command_CommandParameter"];
				if( dt==null )
				{
					dt = Command_CommandParameterData.BuildCommand_CommandParameterTable();
					Tables.Add(dt);
				}
				return (Command_CommandParameterTable) dt;
			}
		}
		public Screen_CommandParameterTable tScreen_CommandParameter
		{
			get
			{
				if( m_bIsCache )
					return (Screen_CommandParameterTable) Tables["Screen_CommandParameter"];
				if( m_mdsCache!=null )
					return (Screen_CommandParameterTable) m_mdsCache.Tables["Screen_CommandParameter"];
				Screen_CommandParameterTable dt = (Screen_CommandParameterTable)Tables["Screen_CommandParameter"];
				if( dt==null )
				{
					dt = Screen_CommandParameterData.BuildScreen_CommandParameterTable();
					Tables.Add(dt);
				}
				return (Screen_CommandParameterTable) dt;
			}
		}
        public EffectTypeTable tEffectType
		{
			get
			{
				if( m_bIsCache )
					return (EffectTypeTable) Tables["EffectType"];
				if( m_mdsCache!=null )
					return (EffectTypeTable) m_mdsCache.Tables["EffectType"];
				EffectTypeTable dt = (EffectTypeTable)Tables["EffectType"];
				if( dt==null )
				{
					dt = EffectTypeData.BuildEffectTypeTable();
					Tables.Add(dt);
				}
				return (EffectTypeTable) dt;
			}
		}
		public CommandCategoryTable tCommandCategory
		{
			get
			{
				if( m_bIsCache )
					return (CommandCategoryTable) Tables["CommandCategory"];
				if( m_mdsCache!=null )
					return (CommandCategoryTable) m_mdsCache.Tables["CommandCategory"];
				CommandCategoryTable dt = (CommandCategoryTable)Tables["CommandCategory"];
				if( dt==null )
				{
					dt = CommandCategoryData.BuildCommandCategoryTable();
					Tables.Add(dt);
				}
				return (CommandCategoryTable) dt;
			}
		}
		public CommandGroup_DTable tCommandGroup_D
		{
			get
			{
				if( m_bIsCache )
					return (CommandGroup_DTable) Tables["CommandGroup_D"];
				if( m_mdsCache!=null )
					return (CommandGroup_DTable) m_mdsCache.Tables["CommandGroup_D"];
				CommandGroup_DTable dt = (CommandGroup_DTable)Tables["CommandGroup_D"];
				if( dt==null )
				{
					dt = CommandGroup_DData.BuildCommandGroup_DTable();
					Tables.Add(dt);
				}
				return (CommandGroup_DTable) dt;
			}
		}
		public CommandGroup_D_CommandTable tCommandGroup_D_Command
		{
			get
			{
				if( m_bIsCache )
					return (CommandGroup_D_CommandTable) Tables["CommandGroup_D_Command"];
				if( m_mdsCache!=null )
					return (CommandGroup_D_CommandTable) m_mdsCache.Tables["CommandGroup_D_Command"];
				CommandGroup_D_CommandTable dt = (CommandGroup_D_CommandTable)Tables["CommandGroup_D_Command"];
				if( dt==null )
				{
					dt = CommandGroup_D_CommandData.BuildCommandGroup_D_CommandTable();
					Tables.Add(dt);
				}
				return (CommandGroup_D_CommandTable) dt;
			}
		}
		public CommandGroup_D_Command_CommandParameterTable tCommandGroup_D_Command_CommandParameter
		{
			get
			{
				if( m_bIsCache )
					return (CommandGroup_D_Command_CommandParameterTable) Tables["CommandGroup_D_Command_CommandParameter"];
				if( m_mdsCache!=null )
					return (CommandGroup_D_Command_CommandParameterTable) m_mdsCache.Tables["CommandGroup_D_Command_CommandParameter"];
				CommandGroup_D_Command_CommandParameterTable dt = (CommandGroup_D_Command_CommandParameterTable)Tables["CommandGroup_D_Command_CommandParameter"];
				if( dt==null )
				{
					dt = CommandGroup_D_Command_CommandParameterData.BuildCommandGroup_D_Command_CommandParameterTable();
					Tables.Add(dt);
				}
				return (CommandGroup_D_Command_CommandParameterTable) dt;
			}
		}
		public CommandParameterTable tCommandParameter
		{
			get
			{
				if( m_bIsCache )
					return (CommandParameterTable) Tables["CommandParameter"];
				if( m_mdsCache!=null )
					return (CommandParameterTable) m_mdsCache.Tables["CommandParameter"];
				CommandParameterTable dt = (CommandParameterTable)Tables["CommandParameter"];
				if( dt==null )
				{
					dt = CommandParameterData.BuildCommandParameterTable();
					Tables.Add(dt);
				}
				return (CommandParameterTable) dt;
			}
		}
		public UITable tUI
		{
			get
			{
				if( m_bIsCache )
					return (UITable) Tables["UI"];
				if( m_mdsCache!=null )
					return (UITable) m_mdsCache.Tables["UI"];
				UITable dt = (UITable)Tables["UI"];
				if( dt==null )
				{
					dt = UIData.BuildUITable();
					Tables.Add(dt);
				}
				return (UITable) dt;
			}
		}
		public CriteriaListTable tCriteriaList
		{
			get
			{
				if( m_bIsCache )
					return (CriteriaListTable) Tables["CriteriaList"];
				if( m_mdsCache!=null )
					return (CriteriaListTable) m_mdsCache.Tables["CriteriaList"];
				CriteriaListTable dt = (CriteriaListTable)Tables["CriteriaList"];
				if( dt==null )
				{
					dt = CriteriaListData.BuildCriteriaListTable();
					Tables.Add(dt);
				}
				return (CriteriaListTable) dt;
			}
		}
		public CriteriaList_CriteriaParmListTable tCriteriaList_CriteriaParmList
		{
			get
			{
				if( m_bIsCache )
					return (CriteriaList_CriteriaParmListTable) Tables["CriteriaList_CriteriaParmList"];
				if( m_mdsCache!=null )
					return (CriteriaList_CriteriaParmListTable) m_mdsCache.Tables["CriteriaList_CriteriaParmList"];
				CriteriaList_CriteriaParmListTable dt = (CriteriaList_CriteriaParmListTable)Tables["CriteriaList_CriteriaParmList"];
				if( dt==null )
				{
					dt = CriteriaList_CriteriaParmListData.BuildCriteriaList_CriteriaParmListTable();
					Tables.Add(dt);
				}
				return (CriteriaList_CriteriaParmListTable) dt;
			}
		}
		public CriteriaParmListTable tCriteriaParmList
		{
			get
			{
				if( m_bIsCache )
					return (CriteriaParmListTable) Tables["CriteriaParmList"];
				if( m_mdsCache!=null )
					return (CriteriaParmListTable) m_mdsCache.Tables["CriteriaParmList"];
				CriteriaParmListTable dt = (CriteriaParmListTable)Tables["CriteriaParmList"];
				if( dt==null )
				{
					dt = CriteriaParmListData.BuildCriteriaParmListTable();
					Tables.Add(dt);
				}
				return (CriteriaParmListTable) dt;
			}
		}
		public DesignObjTable tDesignObj
		{
			get
			{
				if( m_bIsCache )
					return (DesignObjTable) Tables["DesignObj"];
				if( m_mdsCache!=null )
					return (DesignObjTable) m_mdsCache.Tables["DesignObj"];
				DesignObjTable dt = (DesignObjTable)Tables["DesignObj"];
				if( dt==null )
				{
					dt = DesignObjData.BuildDesignObjTable();
					Tables.Add(dt);
				}
				return (DesignObjTable) dt;
			}
		}
		public DesignObjCategoryTable tDesignObjCategory
		{
			get
			{
				if( m_bIsCache )
					return (DesignObjCategoryTable) Tables["DesignObjCategory"];
				if( m_mdsCache!=null )
					return (DesignObjCategoryTable) m_mdsCache.Tables["DesignObjCategory"];
				DesignObjCategoryTable dt = (DesignObjCategoryTable)Tables["DesignObjCategory"];
				if( dt==null )
				{
					dt = DesignObjCategoryData.BuildDesignObjCategoryTable();
					Tables.Add(dt);
				}
				return (DesignObjCategoryTable) dt;
			}
		}
		public DesignObjParameterTable tDesignObjParameter
		{
			get
			{
				if( m_bIsCache )
					return (DesignObjParameterTable) Tables["DesignObjParameter"];
				if( m_mdsCache!=null )
					return (DesignObjParameterTable) m_mdsCache.Tables["DesignObjParameter"];
				DesignObjParameterTable dt = (DesignObjParameterTable)Tables["DesignObjParameter"];
				if( dt==null )
				{
					dt = DesignObjParameterData.BuildDesignObjParameterTable();
					Tables.Add(dt);
				}
				return (DesignObjParameterTable) dt;
			}
		}
		public DesignObjTypeTable tDesignObjType
		{
			get
			{
				if( m_bIsCache )
					return (DesignObjTypeTable) Tables["DesignObjType"];
				if( m_mdsCache!=null )
					return (DesignObjTypeTable) m_mdsCache.Tables["DesignObjType"];
				DesignObjTypeTable dt = (DesignObjTypeTable)Tables["DesignObjType"];
				if( dt==null )
				{
					dt = DesignObjTypeData.BuildDesignObjTypeTable();
					Tables.Add(dt);
				}
				return (DesignObjTypeTable) dt;
			}
		}
		public DesignObjType_DesignObjParameterTable tDesignObjType_DesignObjParameter
		{
			get
			{
				if( m_bIsCache )
					return (DesignObjType_DesignObjParameterTable) Tables["DesignObjType_DesignObjParameter"];
				if( m_mdsCache!=null )
					return (DesignObjType_DesignObjParameterTable) m_mdsCache.Tables["DesignObjType_DesignObjParameter"];
				DesignObjType_DesignObjParameterTable dt = (DesignObjType_DesignObjParameterTable)Tables["DesignObjType_DesignObjParameter"];
				if( dt==null )
				{
					dt = DesignObjType_DesignObjParameterData.BuildDesignObjType_DesignObjParameterTable();
					Tables.Add(dt);
				}
				return (DesignObjType_DesignObjParameterTable) dt;
			}
		}
		public DesignObjVariationTable tDesignObjVariation
		{
			get
			{
				if( m_bIsCache )
					return (DesignObjVariationTable) Tables["DesignObjVariation"];
				if( m_mdsCache!=null )
					return (DesignObjVariationTable) m_mdsCache.Tables["DesignObjVariation"];
				DesignObjVariationTable dt = (DesignObjVariationTable)Tables["DesignObjVariation"];
				if( dt==null )
				{
					dt = DesignObjVariationData.BuildDesignObjVariationTable();
					Tables.Add(dt);
				}
				return (DesignObjVariationTable) dt;
			}
		}
		public DesignObjVariation_DesignObjTable tDesignObjVariation_DesignObj
		{
			get
			{
				if( m_bIsCache )
					return (DesignObjVariation_DesignObjTable) Tables["DesignObjVariation_DesignObj"];
				if( m_mdsCache!=null )
					return (DesignObjVariation_DesignObjTable) m_mdsCache.Tables["DesignObjVariation_DesignObj"];
				DesignObjVariation_DesignObjTable dt = (DesignObjVariation_DesignObjTable)Tables["DesignObjVariation_DesignObj"];
				if( dt==null )
				{
					dt = DesignObjVariation_DesignObjData.BuildDesignObjVariation_DesignObjTable();
					Tables.Add(dt);
				}
				return (DesignObjVariation_DesignObjTable) dt;
			}
		}
		public DesignObjVariation_DesignObj_Skin_LanguageTable tDesignObjVariation_DesignObj_Skin_Language
		{
			get
			{
				if( m_bIsCache )
					return (DesignObjVariation_DesignObj_Skin_LanguageTable) Tables["DesignObjVariation_DesignObj_Skin_Language"];
				if( m_mdsCache!=null )
					return (DesignObjVariation_DesignObj_Skin_LanguageTable) m_mdsCache.Tables["DesignObjVariation_DesignObj_Skin_Language"];
				DesignObjVariation_DesignObj_Skin_LanguageTable dt = (DesignObjVariation_DesignObj_Skin_LanguageTable)Tables["DesignObjVariation_DesignObj_Skin_Language"];
				if( dt==null )
				{
					dt = DesignObjVariation_DesignObj_Skin_LanguageData.BuildDesignObjVariation_DesignObj_Skin_LanguageTable();
					Tables.Add(dt);
				}
				return (DesignObjVariation_DesignObj_Skin_LanguageTable) dt;
			}
		}
		public DesignObjVariation_DesignObjParameterTable tDesignObjVariation_DesignObjParameter
		{
			get
			{
				if( m_bIsCache )
					return (DesignObjVariation_DesignObjParameterTable) Tables["DesignObjVariation_DesignObjParameter"];
				if( m_mdsCache!=null )
					return (DesignObjVariation_DesignObjParameterTable) m_mdsCache.Tables["DesignObjVariation_DesignObjParameter"];
				DesignObjVariation_DesignObjParameterTable dt = (DesignObjVariation_DesignObjParameterTable)Tables["DesignObjVariation_DesignObjParameter"];
				if( dt==null )
				{
					dt = DesignObjVariation_DesignObjParameterData.BuildDesignObjVariation_DesignObjParameterTable();
					Tables.Add(dt);
				}
				return (DesignObjVariation_DesignObjParameterTable) dt;
			}
		}
		public DesignObjVariation_TextTable tDesignObjVariation_Text
		{
			get
			{
				if( m_bIsCache )
					return (DesignObjVariation_TextTable) Tables["DesignObjVariation_Text"];
				if( m_mdsCache!=null )
					return (DesignObjVariation_TextTable) m_mdsCache.Tables["DesignObjVariation_Text"];
				DesignObjVariation_TextTable dt = (DesignObjVariation_TextTable)Tables["DesignObjVariation_Text"];
				if( dt==null )
				{
					dt = DesignObjVariation_TextData.BuildDesignObjVariation_TextTable();
					Tables.Add(dt);
				}
				return (DesignObjVariation_TextTable) dt;
			}
		}
		public DesignObjVariation_Text_Skin_LanguageTable tDesignObjVariation_Text_Skin_Language
		{
			get
			{
				if( m_bIsCache )
					return (DesignObjVariation_Text_Skin_LanguageTable) Tables["DesignObjVariation_Text_Skin_Language"];
				if( m_mdsCache!=null )
					return (DesignObjVariation_Text_Skin_LanguageTable) m_mdsCache.Tables["DesignObjVariation_Text_Skin_Language"];
				DesignObjVariation_Text_Skin_LanguageTable dt = (DesignObjVariation_Text_Skin_LanguageTable)Tables["DesignObjVariation_Text_Skin_Language"];
				if( dt==null )
				{
					dt = DesignObjVariation_Text_Skin_LanguageData.BuildDesignObjVariation_Text_Skin_LanguageTable();
					Tables.Add(dt);
				}
				return (DesignObjVariation_Text_Skin_LanguageTable) dt;
			}
		}
		public DesignObjVariation_ZoneTable tDesignObjVariation_Zone
		{
			get
			{
				if( m_bIsCache )
					return (DesignObjVariation_ZoneTable) Tables["DesignObjVariation_Zone"];
				if( m_mdsCache!=null )
					return (DesignObjVariation_ZoneTable) m_mdsCache.Tables["DesignObjVariation_Zone"];
				DesignObjVariation_ZoneTable dt = (DesignObjVariation_ZoneTable)Tables["DesignObjVariation_Zone"];
				if( dt==null )
				{
					dt = DesignObjVariation_ZoneData.BuildDesignObjVariation_ZoneTable();
					Tables.Add(dt);
				}
				return (DesignObjVariation_ZoneTable) dt;
			}
		}
		public DeviceTemplateTable tDeviceTemplate
		{
			get
			{
				if( m_bIsCache )
					return (DeviceTemplateTable) Tables["DeviceTemplate"];
				if( m_mdsCache!=null )
					return (DeviceTemplateTable) m_mdsCache.Tables["DeviceTemplate"];
				DeviceTemplateTable dt = (DeviceTemplateTable)Tables["DeviceTemplate"];
				if( dt==null )
				{
					dt = DeviceTemplateData.BuildDeviceTemplateTable();
					Tables.Add(dt);
				}
				return (DeviceTemplateTable) dt;
			}
		}
		public HorizAlignmentTable tHorizAlignment
		{
			get
			{
				if( m_bIsCache )
					return (HorizAlignmentTable) Tables["HorizAlignment"];
				if( m_mdsCache!=null )
					return (HorizAlignmentTable) m_mdsCache.Tables["HorizAlignment"];
				HorizAlignmentTable dt = (HorizAlignmentTable)Tables["HorizAlignment"];
				if( dt==null )
				{
					dt = HorizAlignmentData.BuildHorizAlignmentTable();
					Tables.Add(dt);
				}
				return (HorizAlignmentTable) dt;
			}
		}
		public IconTable tIcon
		{
			get
			{
				if( m_bIsCache )
					return (IconTable) Tables["Icon"];
				if( m_mdsCache!=null )
					return (IconTable) m_mdsCache.Tables["Icon"];
				IconTable dt = (IconTable)Tables["Icon"];
				if( dt==null )
				{
					dt = IconData.BuildIconTable();
					Tables.Add(dt);
				}
				return (IconTable) dt;
			}
		}
		public InstallationTable tInstallation
		{
			get
			{
				if( m_bIsCache )
					return (InstallationTable) Tables["Installation"];
				if( m_mdsCache!=null )
					return (InstallationTable) m_mdsCache.Tables["Installation"];
				InstallationTable dt = (InstallationTable)Tables["Installation"];
				if( dt==null )
				{
					dt = InstallationData.BuildInstallationTable();
					Tables.Add(dt);
				}
				return (InstallationTable) dt;
			}
		}
		public LanguageTable tLanguage
		{
			get
			{
				if( m_bIsCache )
					return (LanguageTable) Tables["Language"];
				if( m_mdsCache!=null )
					return (LanguageTable) m_mdsCache.Tables["Language"];
				LanguageTable dt = (LanguageTable)Tables["Language"];
				if( dt==null )
				{
					dt = LanguageData.BuildLanguageTable();
					Tables.Add(dt);
				}
				return (LanguageTable) dt;
			}
		}
		public ParameterTypeTable tParameterType
		{
			get
			{
				if( m_bIsCache )
					return (ParameterTypeTable) Tables["ParameterType"];
				if( m_mdsCache!=null )
					return (ParameterTypeTable) m_mdsCache.Tables["ParameterType"];
				ParameterTypeTable dt = (ParameterTypeTable)Tables["ParameterType"];
				if( dt==null )
				{
					dt = ParameterTypeData.BuildParameterTypeTable();
					Tables.Add(dt);
				}
				return (ParameterTypeTable) dt;
			}
		}
		public SizeTable tSize
		{
			get
			{
				if( m_bIsCache )
					return (SizeTable) Tables["Size"];
				if( m_mdsCache!=null )
					return (SizeTable) m_mdsCache.Tables["Size"];
				SizeTable dt = (SizeTable)Tables["Size"];
				if( dt==null )
				{
					dt = SizeData.BuildSizeTable();
					Tables.Add(dt);
				}
				return (SizeTable) dt;
			}
		}
		public SkinTable tSkin
		{
			get
			{
				if( m_bIsCache )
					return (SkinTable) Tables["Skin"];
				if( m_mdsCache!=null )
					return (SkinTable) m_mdsCache.Tables["Skin"];
				SkinTable dt = (SkinTable)Tables["Skin"];
				if( dt==null )
				{
					dt = SkinData.BuildSkinTable();
					Tables.Add(dt);
				}
				return (SkinTable) dt;
			}
		}
		public StyleTable tStyle
		{
			get
			{
				if( m_bIsCache )
					return (StyleTable) Tables["Style"];
				if( m_mdsCache!=null )
					return (StyleTable) m_mdsCache.Tables["Style"];
				StyleTable dt = (StyleTable)Tables["Style"];
				if( dt==null )
				{
					dt = StyleData.BuildStyleTable();
					Tables.Add(dt);
				}
				return (StyleTable) dt;
			}
		}
		public StyleVariationTable tStyleVariation
		{
			get
			{
				if( m_bIsCache )
					return (StyleVariationTable) Tables["StyleVariation"];
				if( m_mdsCache!=null )
					return (StyleVariationTable) m_mdsCache.Tables["StyleVariation"];
				StyleVariationTable dt = (StyleVariationTable)Tables["StyleVariation"];
				if( dt==null )
				{
					dt = StyleVariationData.BuildStyleVariationTable();
					Tables.Add(dt);
				}
				return (StyleVariationTable) dt;
			}
		}
		public SystemTable tSystem
		{
			get
			{
				if( m_bIsCache )
					return (SystemTable) Tables["System"];
				if( m_mdsCache!=null )
					return (SystemTable) m_mdsCache.Tables["System"];
				SystemTable dt = (SystemTable)Tables["System"];
				if( dt==null )
				{
					dt = SystemData.BuildSystemTable();
					Tables.Add(dt);
				}
				return (SystemTable) dt;
			}
		}
		public TextTable tText
		{
			get
			{
				if( m_bIsCache )
					return (TextTable) Tables["Text"];
				if( m_mdsCache!=null )
					return (TextTable) m_mdsCache.Tables["Text"];
				TextTable dt = (TextTable)Tables["Text"];
				if( dt==null )
				{
					dt = TextData.BuildTextTable();
					Tables.Add(dt);
				}
				return (TextTable) dt;
			}
		}
		public Text_LSTable tText_LS
		{
			get
			{
				if( m_bIsCache )
					return (Text_LSTable) Tables["Text_LS"];
				if( m_mdsCache!=null )
					return (Text_LSTable) m_mdsCache.Tables["Text_LS"];
				DataTable dt = Tables["Text_LS"];
				if( dt==null )
				{
					dt = Text_LSData.BuildText_LSTable();
					Tables.Add(dt);
				}
				return (Text_LSTable) dt;
			}
		}
		public Text_LS_AltVersionsTable tText_LS_AltVersions
		{
			get
			{
				if( m_bIsCache )
					return (Text_LS_AltVersionsTable) Tables["Text_LS_AltVersions"];
				if( m_mdsCache!=null )
					return (Text_LS_AltVersionsTable) m_mdsCache.Tables["Text_LS_AltVersions"];
				Text_LS_AltVersionsTable dt = (Text_LS_AltVersionsTable)Tables["Text_LS_AltVersions"];
				if( dt==null )
				{
					dt = Text_LS_AltVersionsData.BuildText_LS_AltVersionsTable();
					Tables.Add(dt);
				}
				return (Text_LS_AltVersionsTable) dt;
			}
		}
		public TextCategoryTable tTextCategory
		{
			get
			{
				if( m_bIsCache )
					return (TextCategoryTable) Tables["TextCategory"];
				if( m_mdsCache!=null )
					return (TextCategoryTable) m_mdsCache.Tables["TextCategory"];
				TextCategoryTable dt = (TextCategoryTable)Tables["TextCategory"];
				if( dt==null )
				{
					dt = TextCategoryData.BuildTextCategoryTable();
					Tables.Add(dt);
				}
				return (TextCategoryTable) dt;
			}
		}
		public UserModeTable tUserMode
		{
			get
			{
				if( m_bIsCache )
					return (UserModeTable) Tables["UserMode"];
				if( m_mdsCache!=null )
					return (UserModeTable) m_mdsCache.Tables["UserMode"];
				UserModeTable dt = (UserModeTable)Tables["UserMode"];
				if( dt==null )
				{
					dt = UserModeData.BuildUserModeTable();
					Tables.Add(dt);
				}
				return (UserModeTable) dt;
			}
		}
		public UsersTable tUsers
		{
			get
			{
				if( m_bIsCache )
					return (UsersTable) Tables["Users"];
				if( m_mdsCache!=null )
					return (UsersTable) m_mdsCache.Tables["Users"];
				UsersTable dt = (UsersTable)Tables["Users"];
				if( dt==null )
				{
					dt = UsersData.BuildUsersTable();
					Tables.Add(dt);
				}
				return (UsersTable) dt;
			}
		}
		public VariableTable tVariable
		{
			get
			{
				if( m_bIsCache )
					return (VariableTable) Tables["Variable"];
				if( m_mdsCache!=null )
					return (VariableTable) m_mdsCache.Tables["Variable"];
				VariableTable dt = (VariableTable)Tables["Variable"];
				if( dt==null )
				{
					dt = VariableData.BuildVariableTable();
					Tables.Add(dt);
				}
				return (VariableTable) dt;
			}
		}
		public VersionTable tVersion
		{
			get
			{
				if( m_bIsCache )
					return (VersionTable) Tables["Version"];
				if( m_mdsCache!=null )
					return (VersionTable) m_mdsCache.Tables["Version"];
				VersionTable dt = (VersionTable)Tables["Version"];
				if( dt==null )
				{
					dt = VersionData.BuildVersionTable();
					Tables.Add(dt);
				}
				return (VersionTable) dt;
			}
		}
		public VertAlignmentTable tVertAlignment
		{
			get
			{
				if( m_bIsCache )
					return (VertAlignmentTable) Tables["VertAlignment"];
				if( m_mdsCache!=null )
					return (VertAlignmentTable) m_mdsCache.Tables["VertAlignment"];
				VertAlignmentTable dt = (VertAlignmentTable)Tables["VertAlignment"];
				if( dt==null )
				{
					dt = VertAlignmentData.BuildVertAlignmentTable();
					Tables.Add(dt);
				}
				return (VertAlignmentTable) dt;
			}
		}
		public void SetRelations()
		{
			Relations.Add(MyRelations.Command_FK_CommandCategory,tCommandCategory.cPK_CommandCategory,tCommand.cFK_CommandCategory);
			Relations.Add(MyRelations.Command_CommandParameter_FK_Command,tCommand.cPK_Command,tCommand_CommandParameter.cFK_Command);
			Relations.Add(MyRelations.Command_CommandParameter_FK_CommandParameter,tCommandParameter.cPK_CommandParameter,tCommand_CommandParameter.cFK_CommandParameter);
			Relations.Add(MyRelations.CommandCategory_FK_CommandCategory_Parent,tCommandCategory.cPK_CommandCategory,tCommandCategory.cFK_CommandCategory_Parent);
			Relations.Add(MyRelations.CommandGroup_D_Command_FK_CommandGroup_D,tCommandGroup_D.cPK_CommandGroup_D,tCommandGroup_D_Command.cFK_CommandGroup_D);
			Relations.Add(MyRelations.CommandGroup_D_Command_FK_Command,tCommand.cPK_Command,tCommandGroup_D_Command.cFK_Command);
			Relations.Add(MyRelations.CommandGroup_D_Command_FK_DeviceTemplate,tDeviceTemplate.cPK_DeviceTemplate,tCommandGroup_D_Command.cFK_DeviceTemplate);
			Relations.Add(MyRelations.CommandGroup_D_Command_CommandParameter_FK_CommandGroup_D_Command,tCommandGroup_D_Command.cPK_CommandGroup_D_Command,tCommandGroup_D_Command_CommandParameter.cFK_CommandGroup_D_Command);
			Relations.Add(MyRelations.CommandGroup_D_Command_CommandParameter_FK_CommandParameter,tCommandParameter.cPK_CommandParameter,tCommandGroup_D_Command_CommandParameter.cFK_CommandParameter);
			Relations.Add(MyRelations.CommandParameter_FK_ParameterType,tParameterType.cPK_ParameterType,tCommandParameter.cFK_ParameterType);
			Relations.Add(MyRelations.CriteriaList_CriteriaParmList_FK_CriteriaList,tCriteriaList.cPK_CriteriaList,tCriteriaList_CriteriaParmList.cFK_CriteriaList);
			Relations.Add(MyRelations.CriteriaList_CriteriaParmList_FK_CriteriaParmList,tCriteriaParmList.cPK_CriteriaParmList,tCriteriaList_CriteriaParmList.cFK_CriteriaParmList);
			Relations.Add(MyRelations.CriteriaParmList_FK_ParameterType,tParameterType.cPK_ParameterType,tCriteriaParmList.cFK_ParameterType);
			Relations.Add(MyRelations.DesignObj_FK_DesignObjType,tDesignObjType.cPK_DesignObjType,tDesignObj.cFK_DesignObjType);
			Relations.Add(MyRelations.DesignObj_FK_DesignObjCategory,tDesignObjCategory.cPK_DesignObjCategory,tDesignObj.cFK_DesignObjCategory);
			Relations.Add(MyRelations.DesignObj_FK_DesignObj_IncludeIfOtherIncluded,tDesignObj.cPK_DesignObj,tDesignObj.cFK_DesignObj_IncludeIfOtherIncluded);
			Relations.Add(MyRelations.DesignObjCategory_FK_DesignObjCategory_Parent,tDesignObjCategory.cPK_DesignObjCategory,tDesignObjCategory.cFK_DesignObjCategory_Parent);
			Relations.Add(MyRelations.DesignObjParameter_FK_ParameterType,tParameterType.cPK_ParameterType,tDesignObjParameter.cFK_ParameterType);
			Relations.Add(MyRelations.DesignObjType_DesignObjParameter_FK_DesignObjType,tDesignObjType.cPK_DesignObjType,tDesignObjType_DesignObjParameter.cFK_DesignObjType);
			Relations.Add(MyRelations.DesignObjType_DesignObjParameter_FK_DesignObjParameter,tDesignObjParameter.cPK_DesignObjParameter,tDesignObjType_DesignObjParameter.cFK_DesignObjParameter);
			Relations.Add(MyRelations.DesignObjVariation_FK_DesignObj,tDesignObj.cPK_DesignObj,tDesignObjVariation.cFK_DesignObj);
			Relations.Add(MyRelations.DesignObjVariation_FK_UI,tUI.cPK_UI,tDesignObjVariation.cFK_UI);
			Relations.Add(MyRelations.DesignObjVariation_FK_DesignObj_Goto,tDesignObj.cPK_DesignObj,tDesignObjVariation.cFK_DesignObj_Goto);
			Relations.Add(MyRelations.DesignObjVariation_FK_CommandGroup_D_OnActivate,tCommandGroup_D.cPK_CommandGroup_D,tDesignObjVariation.cFK_CommandGroup_D_OnActivate);
			Relations.Add(MyRelations.DesignObjVariation_FK_CommandGroup_D_OnLoad,tCommandGroup_D.cPK_CommandGroup_D,tDesignObjVariation.cFK_CommandGroup_D_OnLoad);
			Relations.Add(MyRelations.DesignObjVariation_FK_CommandGroup_D_OnUnload,tCommandGroup_D.cPK_CommandGroup_D,tDesignObjVariation.cFK_CommandGroup_D_OnUnload);
			Relations.Add(MyRelations.DesignObjVariation_FK_CommandGroup_D_OnTimeout,tCommandGroup_D.cPK_CommandGroup_D,tDesignObjVariation.cFK_CommandGroup_D_OnTimeout);
			Relations.Add(MyRelations.DesignObjVariation_FK_CommandGroup_D_OnStartup,tCommandGroup_D.cPK_CommandGroup_D,tDesignObjVariation.cFK_CommandGroup_D_OnStartup);
			Relations.Add(MyRelations.DesignObjVariation_FK_CommandGroup_D_OnHighlight,tCommandGroup_D.cPK_CommandGroup_D,tDesignObjVariation.cFK_CommandGroup_D_OnHighlight);
			Relations.Add(MyRelations.DesignObjVariation_FK_CommandGroup_D_OnUnhighlight,tCommandGroup_D.cPK_CommandGroup_D,tDesignObjVariation.cFK_CommandGroup_D_OnUnhighlight);
			Relations.Add(MyRelations.DesignObjVariation_FK_Button,tButton.cPK_Button,tDesignObjVariation.cFK_Button);
			Relations.Add(MyRelations.DesignObjVariation_DesignObj_FK_DesignObjVariation_Parent,tDesignObjVariation.cPK_DesignObjVariation,tDesignObjVariation_DesignObj.cFK_DesignObjVariation_Parent);
			Relations.Add(MyRelations.DesignObjVariation_DesignObj_FK_DesignObj_Child,tDesignObj.cPK_DesignObj,tDesignObjVariation_DesignObj.cFK_DesignObj_Child);
			Relations.Add(MyRelations.DesignObjVariation_DesignObj_Skin_Language_FK_DesignObj_InsteadOf,tDesignObj.cPK_DesignObj,tDesignObjVariation_DesignObj_Skin_Language.cFK_DesignObj_InsteadOf);
			Relations.Add(MyRelations.DesignObjVariation_DesignObj_Skin_Language_FK_DesignObj_Up,tDesignObj.cPK_DesignObj,tDesignObjVariation_DesignObj_Skin_Language.cFK_DesignObj_Up);
			Relations.Add(MyRelations.DesignObjVariation_DesignObj_Skin_Language_FK_DesignObj_Down,tDesignObj.cPK_DesignObj,tDesignObjVariation_DesignObj_Skin_Language.cFK_DesignObj_Down);
			Relations.Add(MyRelations.DesignObjVariation_DesignObj_Skin_Language_FK_DesignObj_Left,tDesignObj.cPK_DesignObj,tDesignObjVariation_DesignObj_Skin_Language.cFK_DesignObj_Left);
			Relations.Add(MyRelations.DesignObjVariation_DesignObj_Skin_Language_FK_DesignObj_Right,tDesignObj.cPK_DesignObj,tDesignObjVariation_DesignObj_Skin_Language.cFK_DesignObj_Right);
			Relations.Add(MyRelations.DesignObjVariation_DesignObjParameter_FK_DesignObjVariation,tDesignObjVariation.cPK_DesignObjVariation,tDesignObjVariation_DesignObjParameter.cFK_DesignObjVariation);
			Relations.Add(MyRelations.DesignObjVariation_DesignObjParameter_FK_DesignObjParameter,tDesignObjParameter.cPK_DesignObjParameter,tDesignObjVariation_DesignObjParameter.cFK_DesignObjParameter);
			Relations.Add(MyRelations.DesignObjVariation_Text_FK_DesignObjVariation,tDesignObjVariation.cPK_DesignObjVariation,tDesignObjVariation_Text.cFK_DesignObjVariation);
			Relations.Add(MyRelations.DesignObjVariation_Text_FK_Text,tText.cPK_Text,tDesignObjVariation_Text.cFK_Text);
			Relations.Add(MyRelations.DesignObjVariation_Text_FK_Text_OverrideFromHeader,tText.cPK_Text,tDesignObjVariation_Text.cFK_Text_OverrideFromHeader);
			Relations.Add(MyRelations.DesignObjVariation_Text_Skin_Language_FK_DesignObjVariation_Text,tDesignObjVariation_Text.cPK_DesignObjVariation_Text,tDesignObjVariation_Text_Skin_Language.cFK_DesignObjVariation_Text);
			Relations.Add(MyRelations.DesignObjVariation_Text_Skin_Language_FK_Skin,tSkin.cPK_Skin,tDesignObjVariation_Text_Skin_Language.cFK_Skin);
			Relations.Add(MyRelations.DesignObjVariation_Text_Skin_Language_FK_Language,tLanguage.cPK_Language,tDesignObjVariation_Text_Skin_Language.cFK_Language);
			Relations.Add(MyRelations.DesignObjVariation_Text_Skin_Language_FK_HorizAlignment,tHorizAlignment.cPK_HorizAlignment,tDesignObjVariation_Text_Skin_Language.cFK_HorizAlignment);
			Relations.Add(MyRelations.DesignObjVariation_Text_Skin_Language_FK_VertAlignment,tVertAlignment.cPK_VertAlignment,tDesignObjVariation_Text_Skin_Language.cFK_VertAlignment);
			Relations.Add(MyRelations.DesignObjVariation_Text_Skin_Language_FK_Style,tStyle.cPK_Style,tDesignObjVariation_Text_Skin_Language.cFK_Style);
			Relations.Add(MyRelations.DesignObjVariation_Zone_FK_DesignObjVariation,tDesignObjVariation.cPK_DesignObjVariation,tDesignObjVariation_Zone.cFK_DesignObjVariation);
			Relations.Add(MyRelations.DesignObjVariation_Zone_FK_Button,tButton.cPK_Button,tDesignObjVariation_Zone.cFK_Button);
			Relations.Add(MyRelations.DesignObjVariation_Zone_FK_CommandGroup_D,tCommandGroup_D.cPK_CommandGroup_D,tDesignObjVariation_Zone.cFK_CommandGroup_D);
			Relations.Add(MyRelations.DesignObjVariation_Zone_FK_DesignObj_Goto,tDesignObj.cPK_DesignObj,tDesignObjVariation_Zone.cFK_DesignObj_Goto);
			Relations.Add(MyRelations.Installation_FK_Version,tVersion.cPK_Version,tInstallation.cFK_Version);
			Relations.Add(MyRelations.Language_FK_Language_TextPlacement,tLanguage.cPK_Language,tLanguage.cFK_Language_TextPlacement);
			Relations.Add(MyRelations.Skin_FK_Style,tStyle.cPK_Style,tSkin.cFK_Style);
			Relations.Add(MyRelations.Skin_FK_Skin_TextPlacement,tSkin.cPK_Skin,tSkin.cFK_Skin_TextPlacement);
			Relations.Add(MyRelations.Style_FK_Style_Selected,tStyle.cPK_Style,tStyle.cFK_Style_Selected);
			Relations.Add(MyRelations.Style_FK_Style_Highlighted,tStyle.cPK_Style,tStyle.cFK_Style_Highlighted);
			Relations.Add(MyRelations.Style_FK_Style_Alt,tStyle.cPK_Style,tStyle.cFK_Style_Alt);
			Relations.Add(MyRelations.StyleVariation_FK_Style,tStyle.cPK_Style,tStyleVariation.cFK_Style);
			Relations.Add(MyRelations.StyleVariation_FK_Skin,tSkin.cPK_Skin,tStyleVariation.cFK_Skin);
			Relations.Add(MyRelations.StyleVariation_FK_UI,tUI.cPK_UI,tStyleVariation.cFK_UI);
			Relations.Add(MyRelations.StyleVariation_FK_HorizAlignment,tHorizAlignment.cPK_HorizAlignment,tStyleVariation.cFK_HorizAlignment);
			Relations.Add(MyRelations.StyleVariation_FK_VertAlignment,tVertAlignment.cPK_VertAlignment,tStyleVariation.cFK_VertAlignment);
			Relations.Add(MyRelations.Text_FK_TextCategory,tTextCategory.cPK_TextCategory,tText.cFK_TextCategory);
			Relations.Add(MyRelations.Text_LS_AltVersions_FK_Text,tText.cPK_Text,tText_LS_AltVersions.cFK_Text);
			Relations.Add(MyRelations.Text_LS_AltVersions_FK_Language,tLanguage.cPK_Language,tText_LS_AltVersions.cFK_Language);
			Relations.Add(MyRelations.TextCategory_FK_TextCategory_Parent,tTextCategory.cPK_TextCategory,tTextCategory.cFK_TextCategory_Parent);
			Relations.Add(MyRelations.Users_FK_Language,tLanguage.cPK_Language,tUsers.cFK_Language);
			Relations.Add(MyRelations.Users_FK_Installation_Main,tInstallation.cPK_Installation,tUsers.cFK_Installation_Main);
			Relations.Add(MyRelations.Screen_CommandParameter_FK_CommandParameter,tCommandParameter.cPK_CommandParameter,tScreen_CommandParameter.cFK_CommandParameter);

			//DesignObjVariation - EffectType relations
			Relations.Add(MyRelations.DesignObjVariation_FK_EffectType_Selected_WithChange, 
				tEffectType.cPK_EffectType, 
				tDesignObjVariation.cFK_EffectType_Selected_WithChange);

			Relations.Add(MyRelations.DesignObjVariation_FK_EffectType_Selected_NoChange, 
				tEffectType.cPK_EffectType, 
				tDesignObjVariation.cFK_EffectType_Selected_NoChange);			

			Relations.Add(MyRelations.DesignObjVariation_FK_EffectType_Highlighted, 
				tEffectType.cPK_EffectType, 
				tDesignObjVariation.cFK_EffectType_Highlighted);			

			Relations.Add(MyRelations.DesignObjvariantion_DesignObj_PK_DesignObjVariation_DesignObj_DesignObjVariation_DesignObj_Skin_Language_FK_DesignObjVariation_DesignObj,
				tDesignObjVariation_DesignObj.cPK_DesignObjVariation_DesignObj,
				tDesignObjVariation_DesignObj_Skin_Language.cFK_DesignObjVariation_DesignObj);
		}

		public class MyRelations
		{
			public const string Command_FK_CommandCategory="1";
			public const string Command_CommandParameter_FK_Command="2";
			public const string Command_CommandParameter_FK_CommandParameter="3";
			public const string CommandCategory_FK_CommandCategory_Parent="4";
			public const string CommandGroup_D_Command_FK_CommandGroup_D="5";
			public const string CommandGroup_D_Command_FK_Command="6";
			public const string CommandGroup_D_Command_FK_DeviceTemplate="7";
			public const string CommandGroup_D_Command_CommandParameter_FK_CommandGroup_D_Command="8";
			public const string CommandGroup_D_Command_CommandParameter_FK_CommandParameter="9";
			public const string CommandParameter_FK_ParameterType="10";
			public const string CriteriaList_CriteriaParmList_FK_CriteriaList="13";
			public const string CriteriaList_CriteriaParmList_FK_CriteriaParmList="14";
			public const string CriteriaParmList_FK_ParameterType="17";
			public const string DesignObj_FK_DesignObjType="19";
			public const string DesignObj_FK_DesignObjCategory="20";
			public const string DesignObj_FK_DesignObj_IncludeIfOtherIncluded="21";
			public const string DesignObjCategory_FK_DesignObjCategory_Parent="22";
			public const string DesignObjParameter_FK_ParameterType="23";
			public const string DesignObjType_DesignObjParameter_FK_DesignObjType="24";
			public const string DesignObjType_DesignObjParameter_FK_DesignObjParameter="25";
			public const string DesignObjVariation_FK_DesignObj="26";
			public const string DesignObjVariation_FK_UI="27";
			public const string DesignObjVariation_FK_DesignObj_Goto="28";
			public const string DesignObjVariation_FK_CommandGroup_D_OnActivate="29";
			public const string DesignObjVariation_FK_CommandGroup_D_OnLoad="30";
			public const string DesignObjVariation_FK_CommandGroup_D_OnUnload="31";
			public const string DesignObjVariation_FK_CommandGroup_D_OnTimeout="32";
			public const string DesignObjVariation_FK_CommandGroup_D_OnStartup="33";
			public const string DesignObjVariation_FK_Button="34";
			public const string DesignObjVariation_DesignObj_FK_DesignObjVariation_Parent="35";
			public const string DesignObjVariation_DesignObj_FK_DesignObj_Child="36";
			public const string DesignObjVariation_DesignObj_Skin_Language_FK_DesignObj_InsteadOf="37";
			public const string DesignObjVariation_DesignObj_Skin_Language_FK_DesignObj_Up="38";
			public const string DesignObjVariation_DesignObj_Skin_Language_FK_DesignObj_Down="39";
			public const string DesignObjVariation_DesignObj_Skin_Language_FK_DesignObj_Left="40";
			public const string DesignObjVariation_DesignObj_Skin_Language_FK_DesignObj_Right="41";
			public const string DesignObjVariation_DesignObjParameter_FK_DesignObjVariation="42";
			public const string DesignObjVariation_DesignObjParameter_FK_DesignObjParameter="43";
			public const string DesignObjVariation_Text_FK_DesignObjVariation="44";
			public const string DesignObjVariation_Text_FK_Text="45";
			public const string DesignObjVariation_Text_FK_Text_OverrideFromHeader="46";
			public const string DesignObjVariation_Text_Skin_Language_FK_DesignObjVariation_Text="47";
			public const string DesignObjVariation_Text_Skin_Language_FK_Skin="48";
			public const string DesignObjVariation_Text_Skin_Language_FK_Language="49";
			public const string DesignObjVariation_Text_Skin_Language_FK_HorizAlignment="50";
			public const string DesignObjVariation_Text_Skin_Language_FK_VertAlignment="51";
			public const string DesignObjVariation_Text_Skin_Language_FK_Style="52";
			public const string DesignObjVariation_Zone_FK_DesignObjVariation="53";
			public const string DesignObjVariation_Zone_FK_Button="54";
			public const string DesignObjVariation_Zone_FK_CommandGroup_D="55";
			public const string DesignObjVariation_Zone_FK_DesignObj_Goto="56";
			public const string Installation_FK_Version="57";
			public const string Language_FK_Language_TextPlacement="58";
			public const string Skin_FK_Style="59";
			public const string Skin_FK_Skin_TextPlacement="60";
			public const string Style_FK_Style_Selected="61";
			public const string Style_FK_Style_Highlighted="62";
			public const string Style_FK_Style_Alt="63";
			public const string StyleVariation_FK_Style="64";
			public const string StyleVariation_FK_Skin="65";
			public const string StyleVariation_FK_UI="66";
			public const string StyleVariation_FK_HorizAlignment="67";
			public const string StyleVariation_FK_VertAlignment="68";
			public const string Text_FK_TextCategory="69";
			public const string Text_LS_AltVersions_FK_Text="70";
			public const string Text_LS_AltVersions_FK_Language="71";
			public const string TextCategory_FK_TextCategory_Parent="72";
			public const string Users_FK_Language="73";
			public const string Users_FK_Installation_Main="74";
			public const string Screen_CommandParameter_FK_CommandParameter="75";
			public const string DesignObjVariation_FK_EffectType_Selected_WithChange="76";
			public const string DesignObjVariation_FK_EffectType_Selected_NoChange="77";
			public const string DesignObjVariation_FK_EffectType_Highlighted="78";
			public const string DesignObjVariation_FK_CommandGroup_D_OnHighlight="79";
			public const string DesignObjVariation_FK_CommandGroup_D_OnUnhighlight="80";
			public const string DesignObjvariantion_DesignObj_PK_DesignObjVariation_DesignObj_DesignObjVariation_DesignObj_Skin_Language_FK_DesignObjVariation_DesignObj="81";
		}
	}
} // namespace HAData.Common.Data
