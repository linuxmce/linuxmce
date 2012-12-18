namespace HAData.DataAccess {
	using System;
	using System.Data;
	using MySql;
	using MySql.Data;
	using MySql.Data.MySqlClient;
	using System.Collections;

	using HAData.Common;

	public class DesignObjVariation_DesignObj_Skin_LanguageData : MyDataSet {
		//
		// DesignObjVariation_DesignObj_Skin_Language table constants
		//
		public const String DESIGNOBJVARIATION_DESIGNOBJ_SKIN_LANGUAGE_TABLE = "DesignObjVariation_DesignObj_Skin_Language";
		public const String PK_DESIGNOBJVARIATION_DESIGNOBJ_SKIN_LANGUAGE_FIELD = "PK_DesignObjVariation_DesignObj_Skin_Language";
		public const String FK_DESIGNOBJVARIATION_DESIGNOBJ_FIELD = "FK_DesignObjVariation_DesignObj";
		public const String FK_SKIN_FIELD = "FK_Skin";
		public const String FK_LANGUAGE_FIELD = "FK_Language";
		public const String DISPLAYORDER_FIELD = "DisplayOrder";
		public const String X_FIELD = "X";
		public const String Y_FIELD = "Y";
		public const String WIDTH_FIELD = "Width";
		public const String HEIGHT_FIELD = "Height";
		public const String FK_DESIGNOBJ_INSTEADOF_FIELD = "FK_DesignObj_InsteadOf";
		public const String CANBEHIDDEN_FIELD = "CanBeHidden";
		public const String HIDEBYDEFAULT_FIELD = "HideByDefault";
		public const String REGENERATEFOREACHSCREEN_FIELD = "RegenerateForEachScreen";
		public const String DISPLAYCHILDRENBEFORETEXT_FIELD = "DisplayChildrenBeforeText";
		public const String DISPLAYCHILDRENBEHINDBACKGROUND_FIELD = "DisplayChildrenBehindBackground";
		public const String DONTMERGEBACKGROUND_FIELD = "DontMergeBackground";
		public const String ISTABSTOP_FIELD = "IsTabStop";
		public const String FK_DESIGNOBJ_UP_FIELD = "FK_DesignObj_Up";
		public const String FK_DESIGNOBJ_DOWN_FIELD = "FK_DesignObj_Down";
		public const String FK_DESIGNOBJ_LEFT_FIELD = "FK_DesignObj_Left";
		public const String FK_DESIGNOBJ_RIGHT_FIELD = "FK_DesignObj_Right";
		public const String SFK_DESIGNOBJ_TIEDTO_FIELD = "sFK_DesignObj_TiedTo";
		public const String VISIBLESTATES_FIELD = "VisibleStates";
		// table+field constants
		public const String PK_DESIGNOBJVARIATION_DESIGNOBJ_SKIN_LANGUAGE_TABLE_FIELD = "DesignObjVariation_DesignObj_Skin_Language.PK_DesignObjVariation_DesignObj_Skin_Language";
		public const String FK_DESIGNOBJVARIATION_DESIGNOBJ_TABLE_FIELD = "DesignObjVariation_DesignObj_Skin_Language.FK_DesignObjVariation_DesignObj";
		public const String FK_SKIN_TABLE_FIELD = "DesignObjVariation_DesignObj_Skin_Language.FK_Skin";
		public const String FK_LANGUAGE_TABLE_FIELD = "DesignObjVariation_DesignObj_Skin_Language.FK_Language";
		public const String DISPLAYORDER_TABLE_FIELD = "DesignObjVariation_DesignObj_Skin_Language.DisplayOrder";
		public const String X_TABLE_FIELD = "DesignObjVariation_DesignObj_Skin_Language.X";
		public const String Y_TABLE_FIELD = "DesignObjVariation_DesignObj_Skin_Language.Y";
		public const String WIDTH_TABLE_FIELD = "DesignObjVariation_DesignObj_Skin_Language.Width";
		public const String HEIGHT_TABLE_FIELD = "DesignObjVariation_DesignObj_Skin_Language.Height";
		public const String FK_DESIGNOBJ_INSTEADOF_TABLE_FIELD = "DesignObjVariation_DesignObj_Skin_Language.FK_DesignObj_InsteadOf";
		public const String CANBEHIDDEN_TABLE_FIELD = "DesignObjVariation_DesignObj_Skin_Language.CanBeHidden";
		public const String HIDEBYDEFAULT_TABLE_FIELD = "DesignObjVariation_DesignObj_Skin_Language.HideByDefault";
		public const String REGENERATEFOREACHSCREEN_TABLE_FIELD = "DesignObjVariation_DesignObj_Skin_Language.RegenerateForEachScreen";
		public const String DISPLAYCHILDRENBEFORETEXT_TABLE_FIELD = "DesignObjVariation_DesignObj_Skin_Language.DisplayChildrenBeforeText";
		public const String DISPLAYCHILDRENBEHINDBACKGROUND_TABLE_FIELD = "DesignObjVariation_DesignObj_Skin_Language.DisplayChildrenBehindBackground";
		public const String DONTMERGEBACKGROUND_TABLE_FIELD = "DesignObjVariation_DesignObj_Skin_Language.DontMergeBackground";
		public const String ISTABSTOP_TABLE_FIELD = "DesignObjVariation_DesignObj_Skin_Language.IsTabStop";
		public const String FK_DESIGNOBJ_UP_TABLE_FIELD = "DesignObjVariation_DesignObj_Skin_Language.FK_DesignObj_Up";
		public const String FK_DESIGNOBJ_DOWN_TABLE_FIELD = "DesignObjVariation_DesignObj_Skin_Language.FK_DesignObj_Down";
		public const String FK_DESIGNOBJ_LEFT_TABLE_FIELD = "DesignObjVariation_DesignObj_Skin_Language.FK_DesignObj_Left";
		public const String FK_DESIGNOBJ_RIGHT_TABLE_FIELD = "DesignObjVariation_DesignObj_Skin_Language.FK_DesignObj_Right";
		public const String SFK_DESIGNOBJ_TIEDTO_TABLE_FIELD = "DesignObjVariation_DesignObj_Skin_Language.sFK_DesignObj_TiedTo";
		public const String VISIBLESTATES_TABLE_FIELD = "DesignObjVariation_DesignObj_Skin_Language.VisibleStates";
		// DataSetCommand object
		protected MySqlDataAdapter m_DSCommand;

		// Stored procedure parameters
		protected const String PK_DESIGNOBJVARIATION_DESIGNOBJ_SKIN_LANGUAGE_PARM = "@PK_DesignObjVariation_DesignObj_Skin_Language";
		protected const String FK_DESIGNOBJVARIATION_DESIGNOBJ_PARM = "@FK_DesignObjVariation_DesignObj";
		protected const String FK_SKIN_PARM = "@FK_Skin";
		protected const String FK_LANGUAGE_PARM = "@FK_Language";
		protected const String DISPLAYORDER_PARM = "@DisplayOrder";
		protected const String X_PARM = "@X";
		protected const String Y_PARM = "@Y";
		protected const String WIDTH_PARM = "@Width";
		protected const String HEIGHT_PARM = "@Height";
		protected const String FK_DESIGNOBJ_INSTEADOF_PARM = "@FK_DesignObj_InsteadOf";
		protected const String CANBEHIDDEN_PARM = "@CanBeHidden";
		protected const String HIDEBYDEFAULT_PARM = "@HideByDefault";
		protected const String REGENERATEFOREACHSCREEN_PARM = "@RegenerateForEachScreen";
		protected const String DISPLAYCHILDRENBEFORETEXT_PARM = "@DisplayChildrenBeforeText";
		protected const String DISPLAYCHILDRENBEHINDBACKGROUND_PARM = "@DisplayChildrenBehindBackground";
		protected const String DONTMERGEBACKGROUND_PARM = "@DontMergeBackground";
		protected const String ISTABSTOP_PARM = "@IsTabStop";
		protected const String FK_DESIGNOBJ_UP_PARM = "@FK_DesignObj_Up";
		protected const String FK_DESIGNOBJ_DOWN_PARM = "@FK_DesignObj_Down";
		protected const String FK_DESIGNOBJ_LEFT_PARM = "@FK_DesignObj_Left";
		protected const String FK_DESIGNOBJ_RIGHT_PARM = "@FK_DesignObj_Right";
		protected const String SFK_DESIGNOBJ_TIEDTO_PARM = "@sFK_DesignObj_TiedTo";
		protected const String VISIBLESTATES_PARM = "@VisibleStates";
		protected const String USERID_PARM = "@UserID";

		protected MySqlCommand m_LoadCommand;
		protected MySqlCommand m_InsertCommand;
		protected MySqlCommand m_UpdateCommand;
		protected MySqlCommand m_DeleteCommand;
		protected MySqlConnection m_Connection;
		protected MySqlTransaction m_Transaction;
		public DataTable Table { get { return Tables[0]; } }


		public DesignObjVariation_DesignObj_Skin_LanguageData() {  // marker:1
			//
			// Create the tables in the dataset
			//
			Tables.Add(BuildDataTables());
			m_Connection = HADataConfiguration.GetMySqlConnection();
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new MySqlDataAdapter();

			m_DSCommand.TableMappings.Add("Table", DesignObjVariation_DesignObj_Skin_LanguageData.DESIGNOBJVARIATION_DESIGNOBJ_SKIN_LANGUAGE_TABLE);
		}

		public DesignObjVariation_DesignObj_Skin_LanguageData(MySqlConnection conn,MySqlTransaction trans) {

			m_Connection = conn;
			m_Transaction = trans;
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new MySqlDataAdapter();

			m_DSCommand.TableMappings.Add("Table", DesignObjVariation_DesignObj_Skin_LanguageData.DESIGNOBJVARIATION_DESIGNOBJ_SKIN_LANGUAGE_TABLE);
		}

		private DesignObjVariation_DesignObj_Skin_LanguageData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context) {
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);

			//
			// Build the schema
			//
			Tables.Add(BuildDataTables());
			//
			// Populate the dataset
			//
			base.SetObjectData(info, context);
		}

		public static DataTable BuildDataTables() {
			return (DataTable) BuildDesignObjVariation_DesignObj_Skin_LanguageTable();
		}
		public static DesignObjVariation_DesignObj_Skin_LanguageTable BuildDesignObjVariation_DesignObj_Skin_LanguageTable() {
			//
			// Create the DesignObjVariation_DesignObj_Skin_Language table
			//
			DesignObjVariation_DesignObj_Skin_LanguageTable Table = new DesignObjVariation_DesignObj_Skin_LanguageTable();
			DataColumnCollection Columns = Table.Columns;
			DataColumn[] PKColumns = new DataColumn[1];

			DataColumn Column = Columns.Add(PK_DESIGNOBJVARIATION_DESIGNOBJ_SKIN_LANGUAGE_FIELD, typeof(System.Int32));
			Column.AutoIncrement = true;
			Column.AutoIncrementSeed = -1;
			Column.AutoIncrementStep = -1;
			PKColumns[0] = Column;

			Columns.Add(FK_DESIGNOBJVARIATION_DESIGNOBJ_FIELD, typeof(System.Int32));
			Columns.Add(FK_SKIN_FIELD, typeof(System.Int32));
			Columns.Add(FK_LANGUAGE_FIELD, typeof(System.Int32));
			Column = Columns.Add(DISPLAYORDER_FIELD, typeof(System.Int32));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Column = Columns.Add(X_FIELD, typeof(System.Int32));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Column = Columns.Add(Y_FIELD, typeof(System.Int32));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Columns.Add(WIDTH_FIELD, typeof(System.Int32));
			Columns.Add(HEIGHT_FIELD, typeof(System.Int32));
			Columns.Add(FK_DESIGNOBJ_INSTEADOF_FIELD, typeof(System.Int32));
			Column = Columns.Add(CANBEHIDDEN_FIELD, typeof(System.Boolean));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Column = Columns.Add(HIDEBYDEFAULT_FIELD, typeof(System.Boolean));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Column = Columns.Add(REGENERATEFOREACHSCREEN_FIELD, typeof(System.Boolean));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Column = Columns.Add(DISPLAYCHILDRENBEFORETEXT_FIELD, typeof(System.Boolean));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Column = Columns.Add(DISPLAYCHILDRENBEHINDBACKGROUND_FIELD, typeof(System.Boolean));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Column = Columns.Add(DONTMERGEBACKGROUND_FIELD, typeof(System.Boolean));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Column = Columns.Add(ISTABSTOP_FIELD, typeof(System.Boolean));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Columns.Add(FK_DESIGNOBJ_UP_FIELD, typeof(System.Int32));
			Columns.Add(FK_DESIGNOBJ_DOWN_FIELD, typeof(System.Int32));
			Columns.Add(FK_DESIGNOBJ_LEFT_FIELD, typeof(System.Int32));
			Columns.Add(FK_DESIGNOBJ_RIGHT_FIELD, typeof(System.Int32));
			Columns.Add(SFK_DESIGNOBJ_TIEDTO_FIELD, typeof(System.String));
			Columns.Add(VISIBLESTATES_FIELD, typeof(System.String));
			Table.PrimaryKey = PKColumns;

			return Table;
		}
		protected static void CreateParameters(MySqlParameterCollection Params, bool IsInsert) {
			Params.Add(new MySqlParameter(PK_DESIGNOBJVARIATION_DESIGNOBJ_SKIN_LANGUAGE_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(FK_DESIGNOBJVARIATION_DESIGNOBJ_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(FK_SKIN_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(FK_LANGUAGE_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(DISPLAYORDER_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(X_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(Y_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(WIDTH_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(HEIGHT_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(FK_DESIGNOBJ_INSTEADOF_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(CANBEHIDDEN_PARM, MySqlDbType.Bit,1));
			Params.Add(new MySqlParameter(HIDEBYDEFAULT_PARM, MySqlDbType.Bit,1));
			Params.Add(new MySqlParameter(REGENERATEFOREACHSCREEN_PARM, MySqlDbType.Bit,1));
			Params.Add(new MySqlParameter(DISPLAYCHILDRENBEFORETEXT_PARM, MySqlDbType.Bit,1));
			Params.Add(new MySqlParameter(DISPLAYCHILDRENBEHINDBACKGROUND_PARM, MySqlDbType.Bit,1));
			Params.Add(new MySqlParameter(DONTMERGEBACKGROUND_PARM, MySqlDbType.Bit,1));
			Params.Add(new MySqlParameter(ISTABSTOP_PARM, MySqlDbType.Bit,1));
			Params.Add(new MySqlParameter(FK_DESIGNOBJ_UP_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(FK_DESIGNOBJ_DOWN_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(FK_DESIGNOBJ_LEFT_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(FK_DESIGNOBJ_RIGHT_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(SFK_DESIGNOBJ_TIEDTO_PARM, MySqlDbType.VarChar, 14));
			Params.Add(new MySqlParameter(VISIBLESTATES_PARM, MySqlDbType.VarChar, 10));
			Params.Add(new MySqlParameter(USERID_PARM, MySqlDbType.Int32));

			// map the parameters to the data table

			Params[PK_DESIGNOBJVARIATION_DESIGNOBJ_SKIN_LANGUAGE_PARM].SourceColumn = DesignObjVariation_DesignObj_Skin_LanguageData.PK_DESIGNOBJVARIATION_DESIGNOBJ_SKIN_LANGUAGE_FIELD;
			if(IsInsert) {
				Params[PK_DESIGNOBJVARIATION_DESIGNOBJ_SKIN_LANGUAGE_PARM].Direction = ParameterDirection.Output;
			}

			Params[FK_DESIGNOBJVARIATION_DESIGNOBJ_PARM].SourceColumn = DesignObjVariation_DesignObj_Skin_LanguageData.FK_DESIGNOBJVARIATION_DESIGNOBJ_FIELD;
			Params[FK_SKIN_PARM].SourceColumn = DesignObjVariation_DesignObj_Skin_LanguageData.FK_SKIN_FIELD;
			Params[FK_LANGUAGE_PARM].SourceColumn = DesignObjVariation_DesignObj_Skin_LanguageData.FK_LANGUAGE_FIELD;
			Params[DISPLAYORDER_PARM].SourceColumn = DesignObjVariation_DesignObj_Skin_LanguageData.DISPLAYORDER_FIELD;
			Params[X_PARM].SourceColumn = DesignObjVariation_DesignObj_Skin_LanguageData.X_FIELD;
			Params[Y_PARM].SourceColumn = DesignObjVariation_DesignObj_Skin_LanguageData.Y_FIELD;
			Params[WIDTH_PARM].SourceColumn = DesignObjVariation_DesignObj_Skin_LanguageData.WIDTH_FIELD;
			Params[HEIGHT_PARM].SourceColumn = DesignObjVariation_DesignObj_Skin_LanguageData.HEIGHT_FIELD;
			Params[FK_DESIGNOBJ_INSTEADOF_PARM].SourceColumn = DesignObjVariation_DesignObj_Skin_LanguageData.FK_DESIGNOBJ_INSTEADOF_FIELD;
			Params[CANBEHIDDEN_PARM].SourceColumn = DesignObjVariation_DesignObj_Skin_LanguageData.CANBEHIDDEN_FIELD;
			Params[HIDEBYDEFAULT_PARM].SourceColumn = DesignObjVariation_DesignObj_Skin_LanguageData.HIDEBYDEFAULT_FIELD;
			Params[REGENERATEFOREACHSCREEN_PARM].SourceColumn = DesignObjVariation_DesignObj_Skin_LanguageData.REGENERATEFOREACHSCREEN_FIELD;
			Params[DISPLAYCHILDRENBEFORETEXT_PARM].SourceColumn = DesignObjVariation_DesignObj_Skin_LanguageData.DISPLAYCHILDRENBEFORETEXT_FIELD;
			Params[DISPLAYCHILDRENBEHINDBACKGROUND_PARM].SourceColumn = DesignObjVariation_DesignObj_Skin_LanguageData.DISPLAYCHILDRENBEHINDBACKGROUND_FIELD;
			Params[DONTMERGEBACKGROUND_PARM].SourceColumn = DesignObjVariation_DesignObj_Skin_LanguageData.DONTMERGEBACKGROUND_FIELD;
			Params[ISTABSTOP_PARM].SourceColumn = DesignObjVariation_DesignObj_Skin_LanguageData.ISTABSTOP_FIELD;
			Params[FK_DESIGNOBJ_UP_PARM].SourceColumn = DesignObjVariation_DesignObj_Skin_LanguageData.FK_DESIGNOBJ_UP_FIELD;
			Params[FK_DESIGNOBJ_DOWN_PARM].SourceColumn = DesignObjVariation_DesignObj_Skin_LanguageData.FK_DESIGNOBJ_DOWN_FIELD;
			Params[FK_DESIGNOBJ_LEFT_PARM].SourceColumn = DesignObjVariation_DesignObj_Skin_LanguageData.FK_DESIGNOBJ_LEFT_FIELD;
			Params[FK_DESIGNOBJ_RIGHT_PARM].SourceColumn = DesignObjVariation_DesignObj_Skin_LanguageData.FK_DESIGNOBJ_RIGHT_FIELD;
			Params[SFK_DESIGNOBJ_TIEDTO_PARM].SourceColumn = DesignObjVariation_DesignObj_Skin_LanguageData.SFK_DESIGNOBJ_TIEDTO_FIELD;
			Params[VISIBLESTATES_PARM].SourceColumn = DesignObjVariation_DesignObj_Skin_LanguageData.VISIBLESTATES_FIELD;
		}

		protected static void CreateCommands(MySqlConnection Conn, MySqlTransaction Trans, ref MySqlCommand LoadCommand, ref MySqlCommand InsertCommand, ref MySqlCommand UpdateCommand, ref MySqlCommand DeleteCommand) {
			if(LoadCommand == null) {
				// Create the command since it's null
				LoadCommand = new MySqlCommand("sp_Select_DesignObjVariation_DesignObj", Conn);
				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new MySqlParameter(PK_DESIGNOBJVARIATION_DESIGNOBJ_SKIN_LANGUAGE_PARM, MySqlDbType.Int32,4));
			}

			if(InsertCommand == null) {
				// Create the command since it's null
				InsertCommand = new MySqlCommand("sp_Insert_DesignObjVariation_DesignObj", Conn);
				InsertCommand.CommandType = CommandType.StoredProcedure;
				InsertCommand.Transaction = Trans;

				MySqlParameterCollection Params = InsertCommand.Parameters;

				CreateParameters(Params, true);

			}

			if(UpdateCommand == null) {
				// Create the command since it's null
				UpdateCommand = new MySqlCommand("sp_Update_DesignObjVariation_DesignObj", Conn);
				UpdateCommand.CommandType = CommandType.StoredProcedure;
				UpdateCommand.Transaction = Trans;

				MySqlParameterCollection Params = UpdateCommand.Parameters;

				CreateParameters(Params, false);

			}
			if (DeleteCommand == null)
			{
				DeleteCommand = new MySqlCommand("sp_Delete_DesignObjVariation_DesignObj", Conn);
				DeleteCommand.CommandType = CommandType.StoredProcedure;
				DeleteCommand.Transaction = Trans;

				DeleteCommand.Parameters.Add(PK_DESIGNOBJVARIATION_DESIGNOBJ_SKIN_LANGUAGE_PARM, MySqlDbType.Int32,4, PK_DESIGNOBJVARIATION_DESIGNOBJ_SKIN_LANGUAGE_FIELD);
				DeleteCommand.Parameters.Add(USERID_PARM, MySqlDbType.Int32);
			}
		}

		protected static void CreateCommands(MySqlDataAdapter odbcda,MySqlConnection Conn, MySqlTransaction Trans, ref MySqlCommand LoadCommand, ref MySqlCommand InsertCommand, ref MySqlCommand UpdateCommand, ref MySqlCommand DeleteCommand) {
				LoadCommand = new MySqlCommand("SELECT PK_DesignObjVariation_DesignObj_Skin_Language,FK_DesignObjVariation_DesignObj,FK_Skin,FK_Language,DisplayOrder,X,Y,Width,Height,FK_DesignObj_InsteadOf,CanBeHidden,HideByDefault,RegenerateForEachScreen,DisplayChildrenBeforeText,DisplayChildrenBehindBackground,DontMergeBackground,IsTabStop,FK_DesignObj_Up,FK_DesignObj_Down,FK_DesignObj_Left,FK_DesignObj_Right,sFK_DesignObj_TiedTo,VisibleStates FROM DesignObjVariation_DesignObj_Skin_Language", Conn);
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new MySqlParameter(PK_DESIGNOBJVARIATION_DESIGNOBJ_SKIN_LANGUAGE_PARM, MySqlDbType.Int32,4));

			odbcda.SelectCommand = LoadCommand;
			MySqlCommandBuilder odbcCB = new MySqlCommandBuilder(odbcda);
			odbcCB.RefreshSchema();
			DeleteCommand = odbcCB.GetDeleteCommand();
			InsertCommand = odbcCB.GetInsertCommand();
			UpdateCommand = odbcCB.GetUpdateCommand();
		}

		public DesignObjVariation_DesignObj_Skin_LanguageData LoadDesignObjVariation_DesignObj(System.Int32 PK_DesignObjVariation_DesignObj_Skin_Language)
		{
			m_DSCommand.SelectCommand = m_LoadCommand;
			m_DSCommand.SelectCommand.Parameters[PK_DESIGNOBJVARIATION_DESIGNOBJ_SKIN_LANGUAGE_PARM].Value = PK_DesignObjVariation_DesignObj_Skin_Language;

			m_DSCommand.Fill(this);
			return this;
		}

		public static DataRowCollection LoadDesignObjVariation_DesignObjWithWhere(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans, string WhereClause) // marker:2
		{
			DataRowCollection dr;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["DesignObjVariation_DesignObj_Skin_Language"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}
			
			DataSet dsTemp = new MyDataSet();
			dsTemp.Tables.Add(BuildDataTables());
			
			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();
			
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			string sSQL = "SELECT PK_DesignObjVariation_DesignObj_Skin_Language, FK_DesignObjVariation_DesignObj, FK_Skin, FK_Language, DisplayOrder, X, Y, Width, Height, FK_DesignObj_InsteadOf, CanBeHidden, HideByDefault, RegenerateForEachScreen, DisplayChildrenBeforeText, DisplayChildrenBehindBackground, DontMergeBackground, IsTabStop, FK_DesignObj_Up, FK_DesignObj_Down, FK_DesignObj_Left, FK_DesignObj_Right, sFK_DesignObj_TiedTo, VisibleStates FROM DesignObjVariation_DesignObj_Skin_Language WHERE " + WhereClause;
			
			MySqlCommand LoadCommand = new MySqlCommand(sSQL,conn);
			
			if( trans!=null )
				LoadCommand.Transaction = trans;
			
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(dsTemp,"DesignObjVariation_DesignObj_Skin_Language");
			
			dr=dsTemp.Tables["DesignObjVariation_DesignObj_Skin_Language"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}

		public static DataRow LoadNoCacheDesignObjVariation_DesignObj(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans, System.Int32 PK_DesignObjVariation_DesignObj_Skin_Language)
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["DesignObjVariation_DesignObj_Skin_Language"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}

			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand;
			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();

			LoadCommand = new MySqlCommand("sp_Select_DesignObjVariation_DesignObj", conn);

			LoadCommand.CommandType = CommandType.StoredProcedure;
			LoadCommand.Parameters.Add(new MySqlParameter(PK_DESIGNOBJVARIATION_DESIGNOBJ_SKIN_LANGUAGE_PARM, MySqlDbType.Int32,4));
			LoadCommand.Parameters[PK_DESIGNOBJVARIATION_DESIGNOBJ_SKIN_LANGUAGE_PARM].Value = PK_DesignObjVariation_DesignObj_Skin_Language;
			if( trans!=null )
				LoadCommand.Transaction = trans;
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(ds,"DesignObjVariation_DesignObj_Skin_Language");
				dr = ds.Tables["DesignObjVariation_DesignObj_Skin_Language"].Rows.Find(PK_DesignObjVariation_DesignObj_Skin_Language);
			return dr;
		}

		public static DataRow LoadDesignObjVariation_DesignObj(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans, System.Int32 PK_DesignObjVariation_DesignObj_Skin_Language)  // marker:3
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["DesignObjVariation_DesignObj_Skin_Language"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
				else
				dr = dt.Rows.Find(PK_DesignObjVariation_DesignObj_Skin_Language);
			}

			if( dr==null )
			{
				MySqlDataAdapter sqlda = new MySqlDataAdapter();
				MySqlCommand LoadCommand;
				if( conn==null )
					conn = HADataConfiguration.GetMySqlConnection();

				LoadCommand = new MySqlCommand("sp_Select_DesignObjVariation_DesignObj", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new MySqlParameter(PK_DESIGNOBJVARIATION_DESIGNOBJ_SKIN_LANGUAGE_PARM, MySqlDbType.Int32,4));
				LoadCommand.Parameters[PK_DESIGNOBJVARIATION_DESIGNOBJ_SKIN_LANGUAGE_PARM].Value = PK_DesignObjVariation_DesignObj_Skin_Language;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(ds,"DesignObjVariation_DesignObj_Skin_Language");
				dr = ds.Tables["DesignObjVariation_DesignObj_Skin_Language"].Rows.Find(PK_DesignObjVariation_DesignObj_Skin_Language);
			}
			return dr;
		}

		public DesignObjVariation_DesignObj_Skin_LanguageData LoadAll() {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new MySqlCommand("SELECT * FROM DesignObjVariation_DesignObj_Skin_Language", m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this);
			return this;

		}

		public static DataRowCollection LoadAll(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans) {

			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand = new MySqlCommand("SELECT * FROM DesignObjVariation_DesignObj_Skin_Language", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(ds,"DesignObjVariation_DesignObj_Skin_Language")==0 )
				return null;
			else
				return ds.Tables["DesignObjVariation_DesignObj_Skin_Language"].Rows;

		}

		public DesignObjVariation_DesignObj_Skin_LanguageData ExecuteQuery(String sSQL) {
			return ExecuteQuery(sSQL,DESIGNOBJVARIATION_DESIGNOBJ_SKIN_LANGUAGE_TABLE);
		}
		public DesignObjVariation_DesignObj_Skin_LanguageData ExecuteQuery(String sSQL,String sTableName) {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new MySqlCommand(sSQL, m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this,sTableName);

			return this;
		}

		public static DataRowCollection ExecuteQuery(String sSQL,ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans) {
			return ExecuteQuery(sSQL,ref ds,conn,trans,"DesignObjVariation_DesignObj_Skin_Language");
		}

		public static DataRowCollection ExecuteQuery(String sSQL,ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans,string sTableName) {
			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand = new MySqlCommand(sSQL, conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if(sqlda.Fill(ds,sTableName)==0 )
				return null;
			else
				return ds.Tables[sTableName].Rows;

		}

		public bool UpdateDesignObjVariation_DesignObj_Skin_LanguageData(int CurUserID) {
			m_DSCommand.UpdateCommand = m_UpdateCommand;
			m_DSCommand.UpdateCommand.Parameters[USERID_PARM].Value = CurUserID;

			m_DSCommand.InsertCommand = m_InsertCommand;
			m_DSCommand.InsertCommand.Parameters[USERID_PARM].Value = CurUserID;

			if( m_DeleteCommand != null )
			{
				m_DSCommand.DeleteCommand = m_DeleteCommand;
				m_DSCommand.DeleteCommand.Parameters[USERID_PARM].Value = CurUserID;
			}

			m_DSCommand.Update(this, DesignObjVariation_DesignObj_Skin_LanguageData.DESIGNOBJVARIATION_DESIGNOBJ_SKIN_LANGUAGE_TABLE);
			return true;
		}

		public static bool UpdateDesignObjVariation_DesignObj_Skin_Language(ref MyDataSet ds, int CurUserID)
		{
			MySqlConnection OdbcConn = HADataConfiguration.GetMySqlConnection();
			return UpdateDesignObjVariation_DesignObj_Skin_Language(ref ds,CurUserID,OdbcConn,null);
		}

		public static bool UpdateDesignObjVariation_DesignObj_Skin_Language(ref MyDataSet ds, int CurUserID,MySqlConnection OdbcConn,MySqlTransaction Trans)
		{
			DataTable dt = ds.Tables[DESIGNOBJVARIATION_DESIGNOBJ_SKIN_LANGUAGE_TABLE];
			if( dt == null )
				return false;

			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand = null;
			MySqlCommand InsertCommand = null;
			MySqlCommand UpdateCommand = null;
			MySqlCommand DeleteCommand = null;
			CreateCommands(sqlda,OdbcConn, Trans, ref LoadCommand, ref InsertCommand, ref UpdateCommand, ref DeleteCommand);
			sqlda.RowUpdated += new MySqlRowUpdatedEventHandler(MyRowUpdated);

			sqlda.Update(dt);
			return true;
		}

		static void MyRowUpdated(Object sender, MySqlRowUpdatedEventArgs e)
		{
			if( e.StatementType==StatementType.Insert )
			{
				MySqlCommand ocmd = new MySqlCommand("SELECT @@IDENTITY", e.Command.Connection);
				int value = Int32.Parse(ocmd.ExecuteScalar().ToString());
				e.Row[0]=value;
				e.Row.AcceptChanges();
			}
		}

	} // public class DesignObjVariation_DesignObj_Skin_Language
	public class DesignObjVariation_DesignObj_Skin_LanguageDataRow
	{
		public DataRow dr = null;
		public DesignObjVariation_DesignObj_Skin_LanguageDataRow(DataRow d)
		{
			dr=d;
		}

		public bool bIsValid
		{
			get
			{
				return dr!=null;
			}
		}
		public System.Int32 fPK_DesignObjVariation_DesignObj_Skin_Language
		{
			get
			{
				return Convert.ToInt32(dr[0]);
			}
		}
		public System.Int32 fFK_DesignObjVariation_DesignObj
		{
			get
			{
				return Convert.ToInt32(dr[1]);
			}
			set
			{
				dr[1]=value;
			}
		}
		public bool fFK_DesignObjVariation_DesignObjIsNull
		{
			get
			{
				return dr[1]==DBNull.Value;
			}
		}
		public void fFK_DesignObjVariation_DesignObjSetNull()
		{
			dr[1]=DBNull.Value;
		}
		public DesignObjVariationDataRow fFK_DesignObjVariation_DesignObj_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tDesignObjVariation[Convert.ToInt32(dr[1])];
			}
		}
		public System.Int32 fFK_Skin
		{
			get
			{
				return Convert.ToInt32(dr[2]);
			}
			set
			{
				dr[2]=value;
			}
		}
		public bool fFK_SkinIsNull
		{
			get
			{
				return dr[2]==DBNull.Value;
			}
		}
		public void fFK_SkinSetNull()
		{
			dr[2]=DBNull.Value;
		}
		public DesignObjDataRow fFK_Skin_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tDesignObj[Convert.ToInt32(dr[2])];
			}
		}

		public System.Int32 fFK_Language
		{
			get
			{
				return Convert.ToInt32(dr[3]);
			}
			set
			{
				dr[3]=value;
			}
		}
		public bool fFK_LanguageIsNull
		{
			get
			{
				return dr[3]==DBNull.Value;
			}
		}
		public void fFK_LanguageSetNull()
		{
			dr[3]=DBNull.Value;
		}
		public DesignObjDataRow fFK_Language_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tDesignObj[Convert.ToInt32(dr[3])];
			}
		}
		
		public System.Int32 fDisplayOrder
		{
			get
			{
				return Convert.ToInt32(dr[4]);
			}
			set
			{
				dr[4]=value;
			}
		}
		public System.Int32 fX
		{
			get
			{
				return Convert.ToInt32(dr[5]);
			}
			set
			{
				dr[5]=value;
			}
		}
		public System.Int32 fY
		{
			get
			{
				return Convert.ToInt32(dr[6]);
			}
			set
			{
				dr[6]=value;
			}
		}
		public System.Int32 fWidth
		{
			get
			{
				return Convert.ToInt32(dr[7]);
			}
			set
			{
				dr[7]=value;
			}
		}
		public bool fWidthIsNull
		{
			get
			{
				return dr[7]==DBNull.Value;
			}
		}
		public void fWidthSetNull()
		{
			dr[7]=DBNull.Value;
		}
		public System.Int32 fHeight
		{
			get
			{
				return Convert.ToInt32(dr[8]);
			}
			set
			{
				dr[8]=value;
			}
		}
		public bool fHeightIsNull
		{
			get
			{
				return dr[8]==DBNull.Value;
			}
		}
		public void fHeightSetNull()
		{
			dr[8]=DBNull.Value;
		}
		public System.Int32 fFK_DesignObj_InsteadOf
		{
			get
			{
				return Convert.ToInt32(dr[9]);
			}
			set
			{
				dr[9]=value;
			}
		}
		public bool fFK_DesignObj_InsteadOfIsNull
		{
			get
			{
				return dr[9]==DBNull.Value;
			}
		}
		public void fFK_DesignObj_InsteadOfSetNull()
		{
			dr[9]=DBNull.Value;
		}
		public DesignObjDataRow fFK_DesignObj_InsteadOf_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tDesignObj[Convert.ToInt32(dr[9])];
			}
		}
		public System.Boolean fCanBeHidden
		{
			get
			{
				return Convert.ToBoolean(dr[10]);
			}
			set
			{
				dr[10]=value;
			}
		}
		public System.Boolean fHideByDefault
		{
			get
			{
				return Convert.ToBoolean(dr[11]);
			}
			set
			{
				dr[11]=value;
			}
		}
		public System.Boolean fRegenerateForEachScreen
		{
			get
			{
				return Convert.ToBoolean(dr[12]);
			}
			set
			{
				dr[12]=value;
			}
		}
		public System.Boolean fDisplayChildrenBeforeText
		{
			get
			{
				return Convert.ToBoolean(dr[13]);
			}
			set
			{
				dr[13]=value;
			}
		}
		public System.Boolean fDisplayChildrenBehindBackground
		{
			get
			{
				return Convert.ToBoolean(dr[14]);
			}
			set
			{
				dr[14]=value;
			}
		}
		public System.Boolean fDontMergeBackground
		{
			get
			{
				return Convert.ToBoolean(dr[15]);
			}
			set
			{
				dr[15]=value;
			}
		}
		public System.Boolean fIsTabStop
		{
			get
			{
				return Convert.ToBoolean(dr[16]);
			}
			set
			{
				dr[16]=value;
			}
		}
		public System.Int32 fFK_DesignObj_Up
		{
			get
			{
				return Convert.ToInt32(dr[17]);
			}
			set
			{
				dr[17]=value;
			}
		}
		public bool fFK_DesignObj_UpIsNull
		{
			get
			{
				return dr[17]==DBNull.Value;
			}
		}
		public void fFK_DesignObj_UpSetNull()
		{
			dr[17]=DBNull.Value;
		}
		public DesignObjDataRow fFK_DesignObj_Up_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tDesignObj[Convert.ToInt32(dr[17])];
			}
		}
		public System.Int32 fFK_DesignObj_Down
		{
			get
			{
				return Convert.ToInt32(dr[18]);
			}
			set
			{
				dr[18]=value;
			}
		}
		public bool fFK_DesignObj_DownIsNull
		{
			get
			{
				return dr[18]==DBNull.Value;
			}
		}
		public void fFK_DesignObj_DownSetNull()
		{
			dr[18]=DBNull.Value;
		}
		public DesignObjDataRow fFK_DesignObj_Down_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tDesignObj[Convert.ToInt32(dr[18])];
			}
		}
		public System.Int32 fFK_DesignObj_Left
		{
			get
			{
				return Convert.ToInt32(dr[19]);
			}
			set
			{
				dr[19]=value;
			}
		}
		public bool fFK_DesignObj_LeftIsNull
		{
			get
			{
				return dr[19]==DBNull.Value;
			}
		}
		public void fFK_DesignObj_LeftSetNull()
		{
			dr[19]=DBNull.Value;
		}
		public DesignObjDataRow fFK_DesignObj_Left_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tDesignObj[Convert.ToInt32(dr[19])];
			}
		}
		public System.Int32 fFK_DesignObj_Right
		{
			get
			{
				return Convert.ToInt32(dr[20]);
			}
			set
			{
				dr[20]=value;
			}
		}
		public bool fFK_DesignObj_RightIsNull
		{
			get
			{
				return dr[20]==DBNull.Value;
			}
		}
		public void fFK_DesignObj_RightSetNull()
		{
			dr[20]=DBNull.Value;
		}
		public DesignObjDataRow fFK_DesignObj_Right_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tDesignObj[Convert.ToInt32(dr[20])];
			}
		}
		public System.String fsFK_DesignObj_TiedTo
		{
			get
			{
				return Convert.ToString(dr[21]);
			}
			set
			{
				dr[21]=value;
			}
		}
		public bool fsFK_DesignObj_TiedToIsNull
		{
			get
			{
				return dr[21]==DBNull.Value;
			}
		}
		public void fsFK_DesignObj_TiedToSetNull()
		{
			dr[21]=DBNull.Value;
		}
		public System.String fVisibleStates
		{
			get
			{
				return Convert.ToString(dr[22]);
			}
			set
			{
				dr[22]=value;
			}
		}
		public bool fVisibleStatesIsNull
		{
			get
			{
				return dr[22]==DBNull.Value;
			}
		}
		public void fVisibleStatesSetNull()
		{
			dr[22]=DBNull.Value;
		}
	} // public class DesignObjVariation_DesignObj_Skin_LanguageDataRow
	public class DesignObjVariation_DesignObj_Skin_LanguageDataReader
	{
		public MySqlDataReader dr;
		bool bCache=false;
		int iRecord=-1,iNumRecords=-1;
		ArrayList al = null;

		public DesignObjVariation_DesignObj_Skin_LanguageDataReader(MySqlDataReader d)
		{
			dr=d;
		}
		public DesignObjVariation_DesignObj_Skin_LanguageDataReader(MySqlCommand cmd)
		{
			dr = cmd.ExecuteReader();
		}

		public DesignObjVariation_DesignObj_Skin_LanguageDataReader(MySqlCommand cmd,bool Cache)
		{
			dr = cmd.ExecuteReader();
			bCache=Cache;
			if( bCache )
				CacheAndClose();
		}

		public DesignObjVariation_DesignObj_Skin_LanguageDataReader(string sSQL)
		{
			MySqlConnection conn = HADataConfiguration.GetMySqlConnection();

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM DesignObjVariation_DesignObj_Skin_Language WHERE " + sSQL;
			}

			MySqlCommand cmd = new MySqlCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public DesignObjVariation_DesignObj_Skin_LanguageDataReader(string sSQL,MySqlConnection conn)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetMySqlConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM DesignObjVariation_DesignObj_Skin_Language WHERE " + sSQL;
			}

			MySqlCommand cmd = new MySqlCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public DesignObjVariation_DesignObj_Skin_LanguageDataReader(string sSQL,MySqlConnection conn,MySqlTransaction trans,bool Cache)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetMySqlConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM DesignObjVariation_DesignObj_Skin_Language WHERE " + sSQL;
			}

			MySqlCommand cmd = new MySqlCommand(sSQL,conn,trans);
			dr = cmd.ExecuteReader();
			bCache=Cache;
			if( bCache )
				CacheAndClose();
		}

		public void GotoTop() { iRecord=-1; }
		public int NumRecords { get { return iNumRecords; } }
		void CacheAndClose()
		{
			al = new ArrayList();
			iNumRecords=0;
			while( dr.Read() )
			{
				iNumRecords++;
				object[] objs = new object[22];
				for(int i=0;i<22;i++)
					objs[i]=dr[i];
				al.Add(objs);
			}
			dr.Close();
		}
		public bool Read()
		{
			if( !bCache )
				return dr.Read();

			return ++iRecord<iNumRecords;;
		}

		public void Close()
		{
			if( !bCache )
				dr.Close();
			else
			{
				bCache=false;
				al = null;
			}
		}
		public System.Int32 fPK_DesignObjVariation_DesignObj_Skin_Language
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[0]);
				else
					return Convert.ToInt32(dr[0]);
			}
		}
		public System.Int32 fFK_DesignObjVariation_DesignObj
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[1]);
				else
					return Convert.ToInt32(dr[1]);
			}
		}
		public bool fFK_DesignObjVariation_DesignObjIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[1]==DBNull.Value;
				else
					return dr[1]==DBNull.Value;
			}
		}
		public System.Int32 fFK_Skin
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[2]);
				else
					return Convert.ToInt32(dr[2]);
			}
		}
		public bool fFK_SkinIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[2]==DBNull.Value;
				else
					return dr[2]==DBNull.Value;
			}
		}

		public System.Int32 fFK_Language
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[3]);
				else
					return Convert.ToInt32(dr[3]);
			}
		}
		public bool fFK_LanguageIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[3]==DBNull.Value;
				else
					return dr[3]==DBNull.Value;
			}
		}		
		public System.Int32 fDisplayOrder
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[4]);
				else
					return Convert.ToInt32(dr[4]);
			}
		}
		public System.Int32 fX
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[5]);
				else
					return Convert.ToInt32(dr[5]);
			}
		}
		public System.Int32 fY
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[6]);
				else
					return Convert.ToInt32(dr[6]);
			}
		}
		public System.Int32 fWidth
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[7]);
				else
					return Convert.ToInt32(dr[7]);
			}
		}
		public bool fWidthIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[7]==DBNull.Value;
				else
					return dr[7]==DBNull.Value;
			}
		}
		public System.Int32 fHeight
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[8]);
				else
					return Convert.ToInt32(dr[8]);
			}
		}
		public bool fHeightIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[8]==DBNull.Value;
				else
					return dr[8]==DBNull.Value;
			}
		}
		public System.Int32 fFK_DesignObj_InsteadOf
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[9]);
				else
					return Convert.ToInt32(dr[9]);
			}
		}
		public bool fFK_DesignObj_InsteadOfIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[9]==DBNull.Value;
				else
					return dr[9]==DBNull.Value;
			}
		}
		public System.Boolean fCanBeHidden
		{
			get
			{
				if( bCache )
					return Convert.ToBoolean(((object[]) al[iRecord])[10]);
				else
					return Convert.ToBoolean(dr[10]);
			}
		}
		public System.Boolean fHideByDefault
		{
			get
			{
				if( bCache )
					return Convert.ToBoolean(((object[]) al[iRecord])[11]);
				else
					return Convert.ToBoolean(dr[11]);
			}
		}
		public System.Boolean fRegenerateForEachScreen
		{
			get
			{
				if( bCache )
					return Convert.ToBoolean(((object[]) al[iRecord])[12]);
				else
					return Convert.ToBoolean(dr[12]);
			}
		}
		public System.Boolean fDisplayChildrenBeforeText
		{
			get
			{
				if( bCache )
					return Convert.ToBoolean(((object[]) al[iRecord])[13]);
				else
					return Convert.ToBoolean(dr[13]);
			}
		}
		public System.Boolean fDisplayChildrenBehindBackground
		{
			get
			{
				if( bCache )
					return Convert.ToBoolean(((object[]) al[iRecord])[14]);
				else
					return Convert.ToBoolean(dr[14]);
			}
		}
		public System.Boolean fDontMergeBackground
		{
			get
			{
				if( bCache )
					return Convert.ToBoolean(((object[]) al[iRecord])[15]);
				else
					return Convert.ToBoolean(dr[15]);
			}
		}
		public System.Boolean fIsTabStop
		{
			get
			{
				if( bCache )
					return Convert.ToBoolean(((object[]) al[iRecord])[16]);
				else
					return Convert.ToBoolean(dr[16]);
			}
		}
		public System.Int32 fFK_DesignObj_Up
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[17]);
				else
					return Convert.ToInt32(dr[17]);
			}
		}
		public bool fFK_DesignObj_UpIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[17]==DBNull.Value;
				else
					return dr[17]==DBNull.Value;
			}
		}
		public System.Int32 fFK_DesignObj_Down
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[18]);
				else
					return Convert.ToInt32(dr[18]);
			}
		}
		public bool fFK_DesignObj_DownIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[18]==DBNull.Value;
				else
					return dr[18]==DBNull.Value;
			}
		}
		public System.Int32 fFK_DesignObj_Left
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[19]);
				else
					return Convert.ToInt32(dr[19]);
			}
		}
		public bool fFK_DesignObj_LeftIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[19]==DBNull.Value;
				else
					return dr[19]==DBNull.Value;
			}
		}
		public System.Int32 fFK_DesignObj_Right
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[20]);
				else
					return Convert.ToInt32(dr[20]);
			}
		}
		public bool fFK_DesignObj_RightIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[20]==DBNull.Value;
				else
					return dr[20]==DBNull.Value;
			}
		}
		public System.String fsFK_DesignObj_TiedTo
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[21]);
				else
					return Convert.ToString(dr[21]);
			}
		}
		public bool fsFK_DesignObj_TiedToIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[21]==DBNull.Value;
				else
					return dr[21]==DBNull.Value;
			}
		}
		public System.String fVisibleStates
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[22]);
				else
					return Convert.ToString(dr[22]);
			}
		}
		public bool fVisibleStatesIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[22]==DBNull.Value;
				else
					return dr[22]==DBNull.Value;
			}
		}
	} // public class DesignObjVariation_DesignObj_Skin_LanguageDataReader
	public class DesignObjVariation_DesignObj_Skin_LanguageTable : DataTable
	{
		public DesignObjVariation_DesignObj_Skin_LanguageTable() : base("DesignObjVariation_DesignObj_Skin_Language") {}

		public DesignObjVariation_DesignObj_Skin_LanguageDataRow this [System.Int32 PK_DesignObjVariation_DesignObj_Skin_Language]
		{
			get
			{
				DesignObjVariation_DesignObj_Skin_LanguageDataRow dr = new DesignObjVariation_DesignObj_Skin_LanguageDataRow(Rows.Find(PK_DesignObjVariation_DesignObj_Skin_Language));
				return dr;
			}
		}
		public DataRowCollection LoadAll(MySqlConnection conn, MySqlTransaction trans)
		{
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand = new MySqlCommand("SELECT PK_DesignObjVariation_DesignObj_Skin_Language,FK_DesignObjVariation_DesignObj,FK_Skin,FK_Language,DisplayOrder,X,Y,Width,Height,FK_DesignObj_InsteadOf,CanBeHidden,HideByDefault,RegenerateForEachScreen,DisplayChildrenBeforeText,DisplayChildrenBehindBackground,DontMergeBackground,IsTabStop,FK_DesignObj_Up,FK_DesignObj_Down,FK_DesignObj_Left,FK_DesignObj_Right,sFK_DesignObj_TiedTo,VisibleStates FROM DesignObjVariation_DesignObj_Skin_Language", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(this.DataSet,"DesignObjVariation_DesignObj_Skin_Language")==0 )
				return null;
			else
				return Rows;
		}
		public void Update(int PK_Users)
		{
			Update(PK_Users,((MyDataSet) DataSet).m_conn,((MyDataSet) DataSet).m_trans);
		}
		public void Update(int PK_Users,MySqlConnection conn, MySqlTransaction trans)
		{
			if( conn==null )
				return;
			MyDataSet ds = (MyDataSet) this.DataSet;
			DesignObjVariation_DesignObj_Skin_LanguageData.UpdateDesignObjVariation_DesignObj_Skin_Language(ref ds,PK_Users,conn,trans);
		}
		public DataColumn cPK_DesignObjVariation_DesignObj_Skin_Language
		{
			get
			{
				return Columns[0];
			}
		}
		public DataColumn cFK_DesignObjVariation_DesignObj
		{
			get
			{
				return Columns[1];
			}
		}
		public DataColumn cFK_Skin
		{
			get
			{
				return Columns[2];
			}
		}
		public DataColumn cFK_Language
		{
			get
			{
				return Columns[3];
			}
		}
		public DataColumn cDisplayOrder
		{
			get
			{
				return Columns[4];
			}
		}
		public DataColumn cX
		{
			get
			{
				return Columns[5];
			}
		}
		public DataColumn cY
		{
			get
			{
				return Columns[6];
			}
		}
		public DataColumn cWidth
		{
			get
			{
				return Columns[7];
			}
		}
		public DataColumn cHeight
		{
			get
			{
				return Columns[8];
			}
		}
		public DataColumn cFK_DesignObj_InsteadOf
		{
			get
			{
				return Columns[9];
			}
		}
		public DataColumn cCanBeHidden
		{
			get
			{
				return Columns[10];
			}
		}
		public DataColumn cHideByDefault
		{
			get
			{
				return Columns[11];
			}
		}
		public DataColumn cRegenerateForEachScreen
		{
			get
			{
				return Columns[12];
			}
		}
		public DataColumn cDisplayChildrenBeforeText
		{
			get
			{
				return Columns[13];
			}
		}
		public DataColumn cDisplayChildrenBehindBackground
		{
			get
			{
				return Columns[14];
			}
		}
		public DataColumn cDontMergeBackground
		{
			get
			{
				return Columns[15];
			}
		}
		public DataColumn cIsTabStop
		{
			get
			{
				return Columns[16];
			}
		}
		public DataColumn cFK_DesignObj_Up
		{
			get
			{
				return Columns[17];
			}
		}
		public DataColumn cFK_DesignObj_Down
		{
			get
			{
				return Columns[18];
			}
		}
		public DataColumn cFK_DesignObj_Left
		{
			get
			{
				return Columns[19];
			}
		}
		public DataColumn cFK_DesignObj_Right
		{
			get
			{
				return Columns[20];
			}
		}
		public DataColumn csFK_DesignObj_TiedTo
		{
			get
			{
				return Columns[21];
			}
		}
		public DataColumn cVisibleStates
		{
			get
			{
				return Columns[22];
			}
		}
	}
} // namespace HAData.Common.Data
