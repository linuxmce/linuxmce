namespace HAData.DataAccess {
	using System;
	using System.Data;
	using Microsoft.Data.Odbc;
	using System.Collections;

	using HAData.Common;

	public class DesignObjVariationData : MyDataSet {
		//
		// DesignObjVariation table constants
		//
		public const String DESIGNOBJVARIATION_TABLE = "DesignObjVariation";
		public const String PK_DESIGNOBJVARIATION_FIELD = "PK_DesignObjVariation";
		public const String FK_DESIGNOBJ_FIELD = "FK_DesignObj";
		public const String FK_UI_FIELD = "FK_UI";
		public const String FK_DESIGNOBJ_GOTO_FIELD = "FK_DesignObj_Goto";
		public const String FK_COMMANDGROUP_D_ONACTIVATE_FIELD = "FK_CommandGroup_D_OnActivate";
		public const String FK_COMMANDGROUP_D_ONLOAD_FIELD = "FK_CommandGroup_D_OnLoad";
		public const String FK_COMMANDGROUP_D_ONUNLOAD_FIELD = "FK_CommandGroup_D_OnUnload";
		public const String FK_COMMANDGROUP_D_ONTIMEOUT_FIELD = "FK_CommandGroup_D_OnTimeout";
		public const String FK_COMMANDGROUP_D_ONSTARTUP_FIELD = "FK_CommandGroup_D_OnStartup";
		public const String FK_COMMANDGROUP_D_ONHIGHLIGHT_FIELD = "FK_CommandGroup_D_OnHighlight";
		public const String FK_COMMANDGROUP_D_ONUNHIGHLIGHT_FIELD = "FK_CommandGroup_D_OnUnhighlight";
		public const String FK_EFFECTTYPE_SELECTED_WITHCHANGE_FIELD = "FK_EffectType_Selected_WithChange";
		public const String FK_EFFECTTYPE_SELECTED_NOCHANGE_FIELD = "FK_EffectType_Selected_NoChange";
		public const String FK_EFFECTTYPE_HIGHLIGHTED_FIELD = "FK_EffectType_Highlighted";
		public const String FK_BUTTON_FIELD = "FK_Button";
		public const String FK_CRITERIA_ORBITER_FIELD = "FK_Criteria_Orbiter";
		public const String DONTRESETSELECTEDSTATE_FIELD = "DontResetSelectedState";
		public const String FK_STABILITYSTATUS_FIELD = "FK_StabilityStatus";
		// table+field constants
		public const String PK_DESIGNOBJVARIATION_TABLE_FIELD = "DesignObjVariation.PK_DesignObjVariation";
		public const String FK_DESIGNOBJ_TABLE_FIELD = "DesignObjVariation.FK_DesignObj";
		public const String FK_UI_TABLE_FIELD = "DesignObjVariation.FK_UI";
		public const String FK_DESIGNOBJ_GOTO_TABLE_FIELD = "DesignObjVariation.FK_DesignObj_Goto";
		public const String FK_COMMANDGROUP_D_ONACTIVATE_TABLE_FIELD = "DesignObjVariation.FK_CommandGroup_D_OnActivate";
		public const String FK_COMMANDGROUP_D_ONLOAD_TABLE_FIELD = "DesignObjVariation.FK_CommandGroup_D_OnLoad";
		public const String FK_COMMANDGROUP_D_ONUNLOAD_TABLE_FIELD = "DesignObjVariation.FK_CommandGroup_D_OnUnload";
		public const String FK_COMMANDGROUP_D_ONTIMEOUT_TABLE_FIELD = "DesignObjVariation.FK_CommandGroup_D_OnTimeout";
		public const String FK_COMMANDGROUP_D_ONSTARTUP_TABLE_FIELD = "DesignObjVariation.FK_CommandGroup_D_OnStartup";
		public const String FK_COMMANDGROUP_D_ONHIGHLIGHT_TABLE_FIELD = "DesignObjVariation.FK_CommandGroup_D_OnHighlight";
		public const String FK_COMMANDGROUP_D_ONUNHIGHLIGHTP_TABLE_FIELD = "DesignObjVariation.FK_CommandGroup_D_OnUnhighlight";
		public const String FK_EFFECTTYPE_SELECTED_TABLE_WITHCHANGE = "DesignObjVariation.FK_EffectType_Selected_WithChange";
		public const String FK_EFFECTTYPE_SELECTED_TABLE_NOCHANGE = "DesignObjVariation.FK_EffectType_Selected_NoChange";
		public const String FK_EFFECTTYPE_TABLE_HIGHLIGHTED = "DesignObjVariation.FK_EffectType_Highlighted";
		public const String FK_BUTTON_TABLE_FIELD = "DesignObjVariation.FK_Button";
		public const String FK_CRITERIA_ORBITER_TABLE_FIELD = "DesignObjVariation.FK_Criteria_Orbiter";
		public const String DONTRESETSELECTEDSTATE_TABLE_FIELD = "DesignObjVariation.DontResetSelectedState";
		public const String FK_STABILITYSTATUS_TABLE_FIELD = "DesignObjVariation.FK_StabilityStatus";
		// DataSetCommand object
		protected OdbcDataAdapter m_DSCommand;

		// Stored procedure parameters
		protected const String PK_DESIGNOBJVARIATION_PARM = "@PK_DesignObjVariation";
		protected const String FK_DESIGNOBJ_PARM = "@FK_DesignObj";
		protected const String FK_UI_PARM = "@FK_UI";
		protected const String FK_DESIGNOBJ_GOTO_PARM = "@FK_DesignObj_Goto";
		protected const String FK_COMMANDGROUP_D_ONACTIVATE_PARM = "@FK_CommandGroup_D_OnActivate";
		protected const String FK_COMMANDGROUP_D_ONLOAD_PARM = "@FK_CommandGroup_D_OnLoad";
		protected const String FK_COMMANDGROUP_D_ONUNLOAD_PARM = "@FK_CommandGroup_D_OnUnload";
		protected const String FK_COMMANDGROUP_D_ONTIMEOUT_PARM = "@FK_CommandGroup_D_OnTimeout";
		protected const String FK_COMMANDGROUP_D_ONSTARTUP_PARM = "@FK_CommandGroup_D_OnStartup";
		protected const String FK_COMMANDGROUP_D_ONHIGHLIGHT_PARM = "@FK_CommandGroup_D_OnHighlight";
		protected const String FK_COMMANDGROUP_D_ONUNHIGHLIGHT_PARM = "@FK_CommandGroup_D_OnUnhighlight";
		protected const String FK_EFFECTTYPE_SELECTED_WITHCHANGE_PARM = "@FK_EffectType_Selected_WithChange";
		protected const String FK_EFFECTTYPE_SELECTED_NOCHANGE_PARM = "@FK_EffectType_Selected_NoChange";
		protected const String FK_EFFECTTYPE_HIGHLIGHTED_PARM = "@FK_EffectType_Highlighted";
		protected const String FK_BUTTON_PARM = "@FK_Button";
		protected const String FK_CRITERIA_ORBITER_PARM = "@FK_Criteria_Orbiter";
		protected const String DONTRESETSELECTEDSTATE_PARM = "@DontResetSelectedState";
		protected const String FK_STABILITYSTATUS_PARM = "@FK_StabilityStatus";
		protected const String USERID_PARM = "@UserID";

		protected OdbcCommand m_LoadCommand;
		protected OdbcCommand m_InsertCommand;
		protected OdbcCommand m_UpdateCommand;
		protected OdbcCommand m_DeleteCommand;
		protected OdbcConnection m_Connection;
		protected OdbcTransaction m_Transaction;
		public DataTable Table { get { return Tables[0]; } }


		public DesignObjVariationData() {  // marker:1
			//
			// Create the tables in the dataset
			//
			Tables.Add(BuildDataTables());
			m_Connection = HADataConfiguration.GetOdbcConnection();
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

			m_DSCommand.TableMappings.Add("Table", DesignObjVariationData.DESIGNOBJVARIATION_TABLE);
		}

		public DesignObjVariationData(OdbcConnection conn,OdbcTransaction trans) {

			m_Connection = conn;
			m_Transaction = trans;
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

			m_DSCommand.TableMappings.Add("Table", DesignObjVariationData.DESIGNOBJVARIATION_TABLE);
		}

		private DesignObjVariationData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context) {
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
			return (DataTable) BuildDesignObjVariationTable();
		}
		public static DesignObjVariationTable BuildDesignObjVariationTable() {
			//
			// Create the DesignObjVariation table
			//
			DesignObjVariationTable Table = new DesignObjVariationTable();
			DataColumnCollection Columns = Table.Columns;
			DataColumn[] PKColumns = new DataColumn[1];

			DataColumn Column = Columns.Add(PK_DESIGNOBJVARIATION_FIELD, typeof(System.Int32));
			Column.AutoIncrement = true;
			Column.AutoIncrementSeed = -1;
			Column.AutoIncrementStep = -1;
			PKColumns[0] = Column;

			Column = Columns.Add(FK_DESIGNOBJ_FIELD, typeof(System.Int32));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Columns.Add(FK_UI_FIELD, typeof(System.Int32));
			Columns.Add(FK_DESIGNOBJ_GOTO_FIELD, typeof(System.Int32));
			Columns.Add(FK_COMMANDGROUP_D_ONACTIVATE_FIELD, typeof(System.Int32));
			Columns.Add(FK_COMMANDGROUP_D_ONLOAD_FIELD, typeof(System.Int32));
			Columns.Add(FK_COMMANDGROUP_D_ONUNLOAD_FIELD, typeof(System.Int32));
			Columns.Add(FK_COMMANDGROUP_D_ONTIMEOUT_FIELD, typeof(System.Int32));
			Columns.Add(FK_COMMANDGROUP_D_ONSTARTUP_FIELD, typeof(System.Int32));
			Columns.Add(FK_COMMANDGROUP_D_ONHIGHLIGHT_FIELD, typeof(System.Int32));
			Columns.Add(FK_COMMANDGROUP_D_ONUNHIGHLIGHT_FIELD, typeof(System.Int32));
			Columns.Add(FK_EFFECTTYPE_SELECTED_WITHCHANGE_FIELD, typeof(System.Int32));
			Columns.Add(FK_EFFECTTYPE_SELECTED_NOCHANGE_FIELD, typeof(System.Int32));
			Columns.Add(FK_EFFECTTYPE_HIGHLIGHTED_FIELD, typeof(System.Int32));
			Columns.Add(FK_BUTTON_FIELD, typeof(System.Int32));
			Columns.Add(FK_CRITERIA_ORBITER_FIELD, typeof(System.Int32));
			Column = Columns.Add(DONTRESETSELECTEDSTATE_FIELD, typeof(System.Int16));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Column = Columns.Add(FK_STABILITYSTATUS_FIELD, typeof(System.Int32));
			Column.AllowDBNull = false;
			Column.DefaultValue = 1;

			Table.PrimaryKey = PKColumns;

			return Table;
		}
		protected static void CreateParameters(OdbcParameterCollection Params, bool IsInsert) {
			Params.Add(new OdbcParameter(PK_DESIGNOBJVARIATION_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_DESIGNOBJ_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_UI_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_DESIGNOBJ_GOTO_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_COMMANDGROUP_D_ONACTIVATE_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_COMMANDGROUP_D_ONLOAD_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_COMMANDGROUP_D_ONUNLOAD_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_COMMANDGROUP_D_ONTIMEOUT_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_COMMANDGROUP_D_ONSTARTUP_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_COMMANDGROUP_D_ONHIGHLIGHT_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_COMMANDGROUP_D_ONUNHIGHLIGHT_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_EFFECTTYPE_SELECTED_WITHCHANGE_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_EFFECTTYPE_SELECTED_NOCHANGE_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_EFFECTTYPE_HIGHLIGHTED_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_BUTTON_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_CRITERIA_ORBITER_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(DONTRESETSELECTEDSTATE_PARM, OdbcType.SmallInt,2));
			Params.Add(new OdbcParameter(FK_STABILITYSTATUS_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(USERID_PARM, OdbcType.Int));

			// map the parameters to the data table

			Params[PK_DESIGNOBJVARIATION_PARM].SourceColumn = DesignObjVariationData.PK_DESIGNOBJVARIATION_FIELD;
			if(IsInsert) {
				Params[PK_DESIGNOBJVARIATION_PARM].Direction = ParameterDirection.Output;
			}

			Params[FK_DESIGNOBJ_PARM].SourceColumn = DesignObjVariationData.FK_DESIGNOBJ_FIELD;
			Params[FK_UI_PARM].SourceColumn = DesignObjVariationData.FK_UI_FIELD;
			Params[FK_DESIGNOBJ_GOTO_PARM].SourceColumn = DesignObjVariationData.FK_DESIGNOBJ_GOTO_FIELD;
			Params[FK_COMMANDGROUP_D_ONACTIVATE_PARM].SourceColumn = DesignObjVariationData.FK_COMMANDGROUP_D_ONACTIVATE_FIELD;
			Params[FK_COMMANDGROUP_D_ONLOAD_PARM].SourceColumn = DesignObjVariationData.FK_COMMANDGROUP_D_ONLOAD_FIELD;
			Params[FK_COMMANDGROUP_D_ONUNLOAD_PARM].SourceColumn = DesignObjVariationData.FK_COMMANDGROUP_D_ONUNLOAD_FIELD;
			Params[FK_COMMANDGROUP_D_ONTIMEOUT_PARM].SourceColumn = DesignObjVariationData.FK_COMMANDGROUP_D_ONTIMEOUT_FIELD;
			Params[FK_COMMANDGROUP_D_ONSTARTUP_PARM].SourceColumn = DesignObjVariationData.FK_COMMANDGROUP_D_ONSTARTUP_FIELD;
			Params[FK_COMMANDGROUP_D_ONHIGHLIGHT_PARM].SourceColumn = DesignObjVariationData.FK_COMMANDGROUP_D_ONHIGHLIGHT_FIELD;
			Params[FK_COMMANDGROUP_D_ONUNHIGHLIGHT_PARM].SourceColumn = DesignObjVariationData.FK_COMMANDGROUP_D_ONUNHIGHLIGHT_FIELD;
			Params[FK_EFFECTTYPE_SELECTED_WITHCHANGE_PARM].SourceColumn = DesignObjVariationData.FK_EFFECTTYPE_SELECTED_WITHCHANGE_FIELD;
			Params[FK_EFFECTTYPE_SELECTED_NOCHANGE_PARM].SourceColumn = DesignObjVariationData.FK_EFFECTTYPE_SELECTED_NOCHANGE_FIELD;
			Params[FK_EFFECTTYPE_HIGHLIGHTED_PARM].SourceColumn = DesignObjVariationData.FK_EFFECTTYPE_HIGHLIGHTED_FIELD;
			Params[FK_BUTTON_PARM].SourceColumn = DesignObjVariationData.FK_BUTTON_FIELD;
			Params[FK_CRITERIA_ORBITER_PARM].SourceColumn = DesignObjVariationData.FK_CRITERIA_ORBITER_FIELD;
			Params[DONTRESETSELECTEDSTATE_PARM].SourceColumn = DesignObjVariationData.DONTRESETSELECTEDSTATE_FIELD;
			Params[FK_STABILITYSTATUS_PARM].SourceColumn = DesignObjVariationData.FK_STABILITYSTATUS_FIELD;
		}

		protected static void CreateCommands(OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
			if(LoadCommand == null) {
				// Create the command since it's null
				LoadCommand = new OdbcCommand("sp_Select_DesignObjVariation", Conn);
				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(PK_DESIGNOBJVARIATION_PARM, OdbcType.Int,4));
			}

			if(InsertCommand == null) {
				// Create the command since it's null
				InsertCommand = new OdbcCommand("sp_Insert_DesignObjVariation", Conn);
				InsertCommand.CommandType = CommandType.StoredProcedure;
				InsertCommand.Transaction = Trans;

				OdbcParameterCollection Params = InsertCommand.Parameters;

				CreateParameters(Params, true);

			}

			if(UpdateCommand == null) {
				// Create the command since it's null
				UpdateCommand = new OdbcCommand("sp_Update_DesignObjVariation", Conn);
				UpdateCommand.CommandType = CommandType.StoredProcedure;
				UpdateCommand.Transaction = Trans;

				OdbcParameterCollection Params = UpdateCommand.Parameters;

				CreateParameters(Params, false);

			}
			if (DeleteCommand == null)
			{
				DeleteCommand = new OdbcCommand("sp_Delete_DesignObjVariation", Conn);
				DeleteCommand.CommandType = CommandType.StoredProcedure;
				DeleteCommand.Transaction = Trans;

				DeleteCommand.Parameters.Add(PK_DESIGNOBJVARIATION_PARM, OdbcType.Int,4, PK_DESIGNOBJVARIATION_FIELD);
				DeleteCommand.Parameters.Add(USERID_PARM, OdbcType.Int);
			}
		}

		protected static void CreateCommands(OdbcDataAdapter odbcda,OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
				LoadCommand = new OdbcCommand(
					"SELECT PK_DesignObjVariation,FK_DesignObj,FK_UI,FK_DesignObj_Goto,FK_CommandGroup_D_OnActivate," + 
					"FK_CommandGroup_D_OnLoad,FK_CommandGroup_D_OnUnload,FK_CommandGroup_D_OnTimeout," +
					"FK_CommandGroup_D_OnStartup,FK_CommandGroup_D_OnHighlight,FK_CommandGroup_D_OnUnhighlight," +
					"FK_EffectType_Selected_WithChange, FK_EffectType_Selected_NoChange," + 
					"FK_EffectType_Highlighted, FK_Button,FK_Criteria_Orbiter,DontResetSelectedState," + 
					"FK_StabilityStatus FROM DesignObjVariation", Conn);
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(PK_DESIGNOBJVARIATION_PARM, OdbcType.Int,4));

			odbcda.SelectCommand = LoadCommand;
			OdbcCommandBuilder odbcCB = new OdbcCommandBuilder(odbcda);
			odbcCB.RefreshSchema();
			DeleteCommand = odbcCB.GetDeleteCommand();
			InsertCommand = odbcCB.GetInsertCommand();
			UpdateCommand = odbcCB.GetUpdateCommand();
		}

		public DesignObjVariationData LoadDesignObjVariation(System.Int32 PK_DesignObjVariation)
		{
			m_DSCommand.SelectCommand = m_LoadCommand;
			m_DSCommand.SelectCommand.Parameters[PK_DESIGNOBJVARIATION_PARM].Value = PK_DesignObjVariation;

			m_DSCommand.Fill(this);
			return this;
		}

		public static DataRowCollection LoadDesignObjVariationWithWhere(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, string WhereClause) // marker:2
		{
			DataRowCollection dr;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["DesignObjVariation"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}
			
			DataSet dsTemp = new MyDataSet();
			dsTemp.Tables.Add(BuildDataTables());
			
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			string sSQL = "SELECT PK_DesignObjVariation, FK_DesignObj, FK_UI, FK_DesignObj_Goto, FK_CommandGroup_D_OnActivate, " +
				"FK_CommandGroup_D_OnLoad, FK_CommandGroup_D_OnUnload, FK_CommandGroup_D_OnTimeout, FK_CommandGroup_D_OnStartup, " +
				"FK_CommandGroup_D_OnHighlight, FK_CommandGroup_D_OnUnhighlight," + 
				"FK_EffectType_Selected_WithChange, FK_EffectType_Selected_NoChange, FK_EffectType_Highlighted, " + 
				"FK_Button, FK_Criteria_Orbiter, DontResetSelectedState, FK_StabilityStatus FROM DesignObjVariation WHERE " + WhereClause;
			
			OdbcCommand LoadCommand = new OdbcCommand(sSQL,conn);
			
			if( trans!=null )
				LoadCommand.Transaction = trans;
			
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(dsTemp,"DesignObjVariation");
			
			dr=dsTemp.Tables["DesignObjVariation"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}

		public static DataRow LoadNoCacheDesignObjVariation(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 PK_DesignObjVariation)
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["DesignObjVariation"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}

			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand;
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();

			LoadCommand = new OdbcCommand("sp_Select_DesignObjVariation", conn);

			LoadCommand.CommandType = CommandType.StoredProcedure;
			LoadCommand.Parameters.Add(new OdbcParameter(PK_DESIGNOBJVARIATION_PARM, OdbcType.Int,4));
			LoadCommand.Parameters[PK_DESIGNOBJVARIATION_PARM].Value = PK_DesignObjVariation;
			if( trans!=null )
				LoadCommand.Transaction = trans;
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(ds,"DesignObjVariation");
				dr = ds.Tables["DesignObjVariation"].Rows.Find(PK_DesignObjVariation);
			return dr;
		}

		public static DataRow LoadDesignObjVariation(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 PK_DesignObjVariation)  // marker:3
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["DesignObjVariation"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
				else
				dr = dt.Rows.Find(PK_DesignObjVariation);
			}

			if( dr==null )
			{
				OdbcDataAdapter sqlda = new OdbcDataAdapter();
				OdbcCommand LoadCommand;
				if( conn==null )
					conn = HADataConfiguration.GetOdbcConnection();

				LoadCommand = new OdbcCommand("sp_Select_DesignObjVariation", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new OdbcParameter(PK_DESIGNOBJVARIATION_PARM, OdbcType.Int,4));
				LoadCommand.Parameters[PK_DESIGNOBJVARIATION_PARM].Value = PK_DesignObjVariation;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(ds,"DesignObjVariation");
				dr = ds.Tables["DesignObjVariation"].Rows.Find(PK_DesignObjVariation);
			}
			return dr;
		}

		public DesignObjVariationData LoadAll() {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new OdbcCommand("SELECT * FROM DesignObjVariation", m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this);
			return this;

		}

		public static DataRowCollection LoadAll(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {

			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT * FROM DesignObjVariation", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(ds,"DesignObjVariation")==0 )
				return null;
			else
				return ds.Tables["DesignObjVariation"].Rows;

		}

		public DesignObjVariationData ExecuteQuery(String sSQL) {
			return ExecuteQuery(sSQL,DESIGNOBJVARIATION_TABLE);
		}
		public DesignObjVariationData ExecuteQuery(String sSQL,String sTableName) {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new OdbcCommand(sSQL, m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this,sTableName);

			return this;
		}

		public static DataRowCollection ExecuteQuery(String sSQL,ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {
			return ExecuteQuery(sSQL,ref ds,conn,trans,"DesignObjVariation");
		}

		public static DataRowCollection ExecuteQuery(String sSQL,ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans,string sTableName) {
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand(sSQL, conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if(sqlda.Fill(ds,sTableName)==0 )
				return null;
			else
				return ds.Tables[sTableName].Rows;

		}

		public bool UpdateDesignObjVariation(int CurUserID) {
			m_DSCommand.UpdateCommand = m_UpdateCommand;
			m_DSCommand.UpdateCommand.Parameters[USERID_PARM].Value = CurUserID;

			m_DSCommand.InsertCommand = m_InsertCommand;
			m_DSCommand.InsertCommand.Parameters[USERID_PARM].Value = CurUserID;

			if( m_DeleteCommand != null )
			{
				m_DSCommand.DeleteCommand = m_DeleteCommand;
				m_DSCommand.DeleteCommand.Parameters[USERID_PARM].Value = CurUserID;
			}

			m_DSCommand.Update(this, DesignObjVariationData.DESIGNOBJVARIATION_TABLE);
			return true;
		}

		public static bool UpdateDesignObjVariation(ref MyDataSet ds, int CurUserID)
		{
			OdbcConnection OdbcConn = HADataConfiguration.GetOdbcConnection();
			return UpdateDesignObjVariation(ref ds,CurUserID,OdbcConn,null);
		}

		public static bool UpdateDesignObjVariation(ref MyDataSet ds, int CurUserID,OdbcConnection OdbcConn,OdbcTransaction Trans)
		{
			DataTable dt = ds.Tables[DESIGNOBJVARIATION_TABLE];
			if( dt == null )
				return false;

			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = null;
			OdbcCommand InsertCommand = null;
			OdbcCommand UpdateCommand = null;
			OdbcCommand DeleteCommand = null;
			CreateCommands(sqlda,OdbcConn, Trans, ref LoadCommand, ref InsertCommand, ref UpdateCommand, ref DeleteCommand);
			sqlda.RowUpdated += new OdbcRowUpdatedEventHandler(MyRowUpdated);

			sqlda.Update(dt);
			return true;
		}

		static void MyRowUpdated(Object sender, OdbcRowUpdatedEventArgs e)
		{
			if( e.StatementType==StatementType.Insert )
			{
				OdbcCommand ocmd = new OdbcCommand("SELECT @@IDENTITY", e.Command.Connection);
				int value = Int32.Parse(ocmd.ExecuteScalar().ToString());
				e.Row[0]=value;
				e.Row.AcceptChanges();
			}
		}

	} // public class DesignObjVariationData
	public class DesignObjVariationDataRow
	{
		public DataRow dr = null;
		public DesignObjVariationDataRow(DataRow d)
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
		public System.Int32 fPK_DesignObjVariation
		{
			get
			{
				return Convert.ToInt32(dr[0]);
			}
		}
		public System.Int32 fFK_DesignObj
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
		public DesignObjDataRow fFK_DesignObj_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tDesignObj[Convert.ToInt32(dr[1])];
			}
		}
		public System.Int32 fFK_UI
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
		public bool fFK_UIIsNull
		{
			get
			{
				return dr[2]==DBNull.Value;
			}
		}
		public void fFK_UISetNull()
		{
			dr[2]=DBNull.Value;
		}
		public UIDataRow fFK_UI_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tUI[Convert.ToInt32(dr[2])];
			}
		}
		public System.Int32 fFK_DesignObj_Goto
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
		public bool fFK_DesignObj_GotoIsNull
		{
			get
			{
				return dr[3]==DBNull.Value;
			}
		}
		public void fFK_DesignObj_GotoSetNull()
		{
			dr[3]=DBNull.Value;
		}
		public DesignObjDataRow fFK_DesignObj_Goto_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tDesignObj[Convert.ToInt32(dr[3])];
			}
		}
		public System.Int32 fFK_CommandGroup_D_OnActivate
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
		public bool fFK_CommandGroup_D_OnActivateIsNull
		{
			get
			{
				return dr[4]==DBNull.Value;
			}
		}
		public void fFK_CommandGroup_D_OnActivateSetNull()
		{
			dr[4]=DBNull.Value;
		}
		public CommandGroup_DDataRow fFK_CommandGroup_D_OnActivate_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tCommandGroup_D[Convert.ToInt32(dr[4])];
			}
		}
		public System.Int32 fFK_CommandGroup_D_OnLoad
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
		public bool fFK_CommandGroup_D_OnLoadIsNull
		{
			get
			{
				return dr[5]==DBNull.Value;
			}
		}
		public void fFK_CommandGroup_D_OnLoadSetNull()
		{
			dr[5]=DBNull.Value;
		}
		public CommandGroup_DDataRow fFK_CommandGroup_D_OnLoad_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tCommandGroup_D[Convert.ToInt32(dr[5])];
			}
		}
		public System.Int32 fFK_CommandGroup_D_OnUnload
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
		public bool fFK_CommandGroup_D_OnUnloadIsNull
		{
			get
			{
				return dr[6]==DBNull.Value;
			}
		}
		public void fFK_CommandGroup_D_OnUnloadSetNull()
		{
			dr[6]=DBNull.Value;
		}
		public CommandGroup_DDataRow fFK_CommandGroup_D_OnUnload_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tCommandGroup_D[Convert.ToInt32(dr[6])];
			}
		}
		public System.Int32 fFK_CommandGroup_D_OnTimeout
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
		public bool fFK_CommandGroup_D_OnTimeoutIsNull
		{
			get
			{
				return dr[7]==DBNull.Value;
			}
		}
		public void fFK_CommandGroup_D_OnTimeoutSetNull()
		{
			dr[7]=DBNull.Value;
		}
		public CommandGroup_DDataRow fFK_CommandGroup_D_OnTimeout_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tCommandGroup_D[Convert.ToInt32(dr[7])];
			}
		}



		public System.Int32 fFK_CommandGroup_D_OnStartup
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
		public bool fFK_CommandGroup_D_OnStartupIsNull
		{
			get
			{
				return dr[8]==DBNull.Value;
			}
		}
		public void fFK_CommandGroup_D_OnStartupSetNull()
		{
			dr[8]=DBNull.Value;
		}
		public CommandGroup_DDataRow fFK_CommandGroup_D_OnStartup_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tCommandGroup_D[Convert.ToInt32(dr[8])];
			}
		}


		//FK_CommandGroup_D_OnHighlight
		public System.Int32 fFK_CommandGroup_D_OnHighlight
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
		public bool fFK_CommandGroup_D_OnHighlightIsNull
		{
			get
			{
				return dr[9]==DBNull.Value;
			}
		}
		public void fFK_CommandGroup_D_OnHighlightSetNull()
		{
			dr[9]=DBNull.Value;
		}
		public CommandGroup_DDataRow fFK_CommandGroup_D_OnHighlight_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tCommandGroup_D[Convert.ToInt32(dr[9])];
			}
		}

		//FK_CommandGroup_D_OnUnhighlight
		public System.Int32 fFK_CommandGroup_D_OnUnhighlight
		{
			get
			{
				return Convert.ToInt32(dr[10]);
			}
			set
			{
				dr[10]=value;
			}
		}
		public bool fFK_CommandGroup_D_OnUnhighlightIsNull
		{
			get
			{
				return dr[10]==DBNull.Value;
			}
		}
		public void fFK_CommandGroup_D_OnUnhighlightSetNull()
		{
			dr[10]=DBNull.Value;
		}
		public CommandGroup_DDataRow fFK_CommandGroup_D_OnUnhighlight_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tCommandGroup_D[Convert.ToInt32(dr[10])];
			}
		}

		//FK_EffectType_Selected_WithChange
		public System.Int32 fFK_EffectType_Selected_WithChange
		{
			get
			{
				return Convert.ToInt32(dr[11]);
			}
			set
			{
				dr[11]=value;
			}
		}
		public bool fFK_EffectType_Selected_WithChangeIsNull
		{
			get
			{
				return dr[11]==DBNull.Value;
			}
		}
		public void fFK_EffectType_Selected_WithChangeSetNull()
		{
			dr[11]=DBNull.Value;
		}
		public CommandGroup_DDataRow fFK_EffectType_Selected_WithChange_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tCommandGroup_D[Convert.ToInt32(dr[11])];
			}
		}

		//FK_EffectType_Selected_NoChange
		public System.Int32 fFK_EffectType_Selected_NoChange
		{
			get
			{
				return Convert.ToInt32(dr[12]);
			}
			set
			{
				dr[12]=value;
			}
		}
		public bool fFK_EffectType_Selected_NoChangeIsNull
		{
			get
			{
				return dr[12]==DBNull.Value;
			}
		}
		public void fFK_EffectType_Selected_NoChangeSetNull()
		{
			dr[12]=DBNull.Value;
		}
		public CommandGroup_DDataRow fFK_EffectType_Selected_NoChange_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tCommandGroup_D[Convert.ToInt32(dr[12])];
			}
		}

		//FK_EffectType_Highlighted
		public System.Int32 fFK_EffectType_Highlighted
		{
			get
			{
				return Convert.ToInt32(dr[13]);
			}
			set
			{
				dr[13]=value;
			}
		}
		public bool fFK_EffectType_HighlightedIsNull
		{
			get
			{
				return dr[13]==DBNull.Value;
			}
		}
		public void fFK_EffectType_HighlightedSetNull()
		{
			dr[13]=DBNull.Value;
		}
		public CommandGroup_DDataRow fFK_EffectType_Highlighted_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tCommandGroup_D[Convert.ToInt32(dr[13])];
			}
		}


		public System.Int32 fFK_Button
		{
			get
			{
				return Convert.ToInt32(dr[14]);
			}
			set
			{
				dr[14]=value;
			}
		}
		public bool fFK_ButtonIsNull
		{
			get
			{
				return dr[14]==DBNull.Value;
			}
		}
		public void fFK_ButtonSetNull()
		{
			dr[14]=DBNull.Value;
		}
		public ButtonDataRow fFK_Button_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tButton[Convert.ToInt32(dr[14])];
			}
		}



		public System.Int32 fFK_Criteria_Orbiter
		{
			get
			{
				return Convert.ToInt32(dr[15]);
			}
			set
			{
				dr[15]=value;
			}
		}
		public bool fFK_Criteria_OrbiterIsNull
		{
			get
			{
				return dr[15]==DBNull.Value;
			}
		}
		public void fFK_Criteria_OrbiterSetNull()
		{
			dr[15]=DBNull.Value;
		}



		public System.Int16 fDontResetSelectedState
		{
			get
			{
				return Convert.ToInt16(dr[16]);
			}
			set
			{
				dr[16]=value;
			}
		}
		public System.Int32 fFK_StabilityStatus
		{
			get
			{
				return Convert.ToInt32(dr[16]);
			}
			set
			{
				dr[16]=value;
			}
		}
	} // public class DesignObjVariationDataRow
	public class DesignObjVariationDataReader
	{
		public OdbcDataReader dr;
		bool bCache=false;
		int iRecord=-1,iNumRecords=-1;
		ArrayList al = null;

		public DesignObjVariationDataReader(OdbcDataReader d)
		{
			dr=d;
		}
		public DesignObjVariationDataReader(OdbcCommand cmd)
		{
			dr = cmd.ExecuteReader();
		}

		public DesignObjVariationDataReader(OdbcCommand cmd,bool Cache)
		{
			dr = cmd.ExecuteReader();
			bCache=Cache;
			if( bCache )
				CacheAndClose();
		}

		public DesignObjVariationDataReader(string sSQL)
		{
			OdbcConnection conn = HADataConfiguration.GetOdbcConnection();

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM DesignObjVariation WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public DesignObjVariationDataReader(string sSQL,OdbcConnection conn)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM DesignObjVariation WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public DesignObjVariationDataReader(string sSQL,OdbcConnection conn,OdbcTransaction trans,bool Cache)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM DesignObjVariation WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,trans);
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
				object[] objs = new object[13];
				for(int i=0;i<13;i++)
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
		public System.Int32 fPK_DesignObjVariation
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[0]);
				else
					return Convert.ToInt32(dr[0]);
			}
		}
		public System.Int32 fFK_DesignObj
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[1]);
				else
					return Convert.ToInt32(dr[1]);
			}
		}
		public System.Int32 fFK_UI
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[2]);
				else
					return Convert.ToInt32(dr[2]);
			}
		}
		public bool fFK_UIIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[2]==DBNull.Value;
				else
					return dr[2]==DBNull.Value;
			}
		}
		public System.Int32 fFK_DesignObj_Goto
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[3]);
				else
					return Convert.ToInt32(dr[3]);
			}
		}
		public bool fFK_DesignObj_GotoIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[3]==DBNull.Value;
				else
					return dr[3]==DBNull.Value;
			}
		}
		public System.Int32 fFK_CommandGroup_D_OnActivate
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[4]);
				else
					return Convert.ToInt32(dr[4]);
			}
		}
		public bool fFK_CommandGroup_D_OnActivateIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[4]==DBNull.Value;
				else
					return dr[4]==DBNull.Value;
			}
		}
		public System.Int32 fFK_CommandGroup_D_OnLoad
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[5]);
				else
					return Convert.ToInt32(dr[5]);
			}
		}
		public bool fFK_CommandGroup_D_OnLoadIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[5]==DBNull.Value;
				else
					return dr[5]==DBNull.Value;
			}
		}
		public System.Int32 fFK_CommandGroup_D_OnUnload
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[6]);
				else
					return Convert.ToInt32(dr[6]);
			}
		}
		public bool fFK_CommandGroup_D_OnUnloadIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[6]==DBNull.Value;
				else
					return dr[6]==DBNull.Value;
			}
		}
		public System.Int32 fFK_CommandGroup_D_OnTimeout
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[7]);
				else
					return Convert.ToInt32(dr[7]);
			}
		}
		public bool fFK_CommandGroup_D_OnTimeoutIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[7]==DBNull.Value;
				else
					return dr[7]==DBNull.Value;
			}
		}
		public System.Int32 fFK_CommandGroup_D_OnStartup
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[8]);
				else
					return Convert.ToInt32(dr[8]);
			}
		}
		public bool fFK_CommandGroup_D_OnStartupIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[8]==DBNull.Value;
				else
					return dr[8]==DBNull.Value;
			}
		}

		//FK_EffectType_Selected_WithChange
		public System.Int32 fFK_EffectType_Selected_WithChange
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[9]);
				else
					return Convert.ToInt32(dr[9]);
			}
		}
		public bool fFK_EffectType_Selected_WithChangeIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[9]==DBNull.Value;
				else
					return dr[9]==DBNull.Value;
			}
		}

		//FK_EffectType_Selected_NoChange
		public System.Int32 fFK_EffectType_Selected_NoChange
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[10]);
				else
					return Convert.ToInt32(dr[10]);
			}
		}
		public bool fFK_EffectType_Selected_NoChangeIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[10]==DBNull.Value;
				else
					return dr[10]==DBNull.Value;
			}
		}

		//FK_EffectType_Highlighted
		public System.Int32 fFK_EffectType_Highlighted
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[11]);
				else
					return Convert.ToInt32(dr[11]);
			}
		}
		public bool fFK_EffectType_HighlightedIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[11]==DBNull.Value;
				else
					return dr[11]==DBNull.Value;
			}
		}

		public System.Int32 fFK_Button
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[12]);
				else
					return Convert.ToInt32(dr[12]);
			}
		}
		public bool fFK_ButtonIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[12]==DBNull.Value;
				else
					return dr[12]==DBNull.Value;
			}
		}
		public System.Int32 fFK_Criteria_Orbiter
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[13]);
				else
					return Convert.ToInt32(dr[13]);
			}
		}
		public bool fFK_Criteria_OrbiterIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[13]==DBNull.Value;
				else
					return dr[13]==DBNull.Value;
			}
		}
		public System.Int16 fDontResetSelectedState
		{
			get
			{
				if( bCache )
					return Convert.ToInt16(((object[]) al[iRecord])[14]);
				else
					return Convert.ToInt16(dr[14]);
			}
		}
		public System.Int32 fFK_StabilityStatus
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[14]);
				else
					return Convert.ToInt32(dr[14]);
			}
		}
	} // public class DesignObjVariationDataReader
	public class DesignObjVariationTable : DataTable
	{
		public DesignObjVariationTable() : base("DesignObjVariation") {}

		public DesignObjVariationDataRow this [System.Int32 PK_DesignObjVariation]
		{
			get
			{
				DesignObjVariationDataRow dr = new DesignObjVariationDataRow(Rows.Find(PK_DesignObjVariation));
				if( !dr.bIsValid  && false /* can't do this with ODBC */  )
				{
					MyDataSet mds = (MyDataSet) DataSet;
					if( mds.m_conn==null )
						return dr;
					OdbcDataAdapter sqlda = new OdbcDataAdapter();
					OdbcCommand LoadCommand = new OdbcCommand("sp_Select_DesignObjVariation", mds.m_conn,mds.m_trans);
					LoadCommand.CommandType = CommandType.StoredProcedure;
					LoadCommand.Parameters.Add(new OdbcParameter("@PK_DesignObjVariation", OdbcType.Int,4));
					LoadCommand.Parameters["@PK_DesignObjVariation"].Value = PK_DesignObjVariation;
					sqlda.SelectCommand = LoadCommand;
					sqlda.Fill(mds,"DesignObjVariation");
					dr = new DesignObjVariationDataRow(Rows.Find(PK_DesignObjVariation));
				}
				return dr;
			}
		}
		public DataRowCollection LoadAll(OdbcConnection conn, OdbcTransaction trans)
		{
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand(
				"SELECT PK_DesignObjVariation,FK_DesignObj,FK_UI,FK_DesignObj_Goto,FK_CommandGroup_D_OnActivate,FK_CommandGroup_D_OnLoad,"+
				"FK_CommandGroup_D_OnUnload,FK_CommandGroup_D_OnTimeout,FK_CommandGroup_D_OnStartup," +
				"FK_CommandGroup_D_OnHighlight, FK_CommandGroup_D_OnUnhighlight, " + 
				"FK_EffectType_Selected_WithChange, FK_EffectType_Selected_NoChange, FK_EffectType_Highlighted, " + 
				"FK_Button,FK_Criteria_Orbiter,DontResetSelectedState,FK_StabilityStatus FROM DesignObjVariation", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(this.DataSet,"DesignObjVariation")==0 )
				return null;
			else
				return Rows;
		}
		public void Update(int PK_Users)
		{
			Update(PK_Users,((MyDataSet) DataSet).m_conn,((MyDataSet) DataSet).m_trans);
		}
		public void Update(int PK_Users,OdbcConnection conn, OdbcTransaction trans)
		{
			if( conn==null )
				return;
			MyDataSet ds = (MyDataSet) this.DataSet;
			DesignObjVariationData.UpdateDesignObjVariation(ref ds,PK_Users,conn,trans);
		}
		public DataColumn cPK_DesignObjVariation
		{
			get
			{
				return Columns[0];
			}
		}
		public DataColumn cFK_DesignObj
		{
			get
			{
				return Columns[1];
			}
		}
		public DataColumn cFK_UI
		{
			get
			{
				return Columns[2];
			}
		}
		public DataColumn cFK_DesignObj_Goto
		{
			get
			{
				return Columns[3];
			}
		}
		public DataColumn cFK_CommandGroup_D_OnActivate
		{
			get
			{
				return Columns[4];
			}
		}
		public DataColumn cFK_CommandGroup_D_OnLoad
		{
			get
			{
				return Columns[5];
			}
		}
		public DataColumn cFK_CommandGroup_D_OnUnload
		{
			get
			{
				return Columns[6];
			}
		}
		public DataColumn cFK_CommandGroup_D_OnTimeout
		{
			get
			{
				return Columns[7];
			}
		}
		public DataColumn cFK_CommandGroup_D_OnStartup
		{
			get
			{
				return Columns[8];
			}
		}

		public DataColumn cFK_CommandGroup_D_OnHighlight
		{
			get
			{
				return Columns[9];
			}
		}

		public DataColumn cFK_CommandGroup_D_OnUnhighlight
		{
			get
			{
				return Columns[10];
			}
		}

		public DataColumn cFK_EffectType_Selected_WithChange
		{
			get
			{
				return Columns[11];
			}
		}

		public DataColumn cFK_EffectType_Selected_NoChange
		{
			get
			{
				return Columns[12];
			}
		}

		public DataColumn cFK_EffectType_Highlighted
		{
			get
			{
				return Columns[13];
			}
		}


		public DataColumn cFK_Button
		{
			get
			{
				return Columns[14];
			}
		}
		public DataColumn cFK_Criteria_Orbiter
		{
			get
			{
				return Columns[15];
			}
		}
		public DataColumn cDontResetSelectedState
		{
			get
			{
				return Columns[16];
			}
		}
		public DataColumn cFK_StabilityStatus
		{
			get
			{
				return Columns[17];
			}
		}
	}
} // namespace HAData.Common.Data
