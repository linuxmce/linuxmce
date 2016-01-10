namespace HAData.DataAccess {
	using System;
	using System.Data;
	using Microsoft.Data.Odbc;
	using System.Collections;

	using HAData.Common;

	public class DesignObjData : MyDataSet {
		//
		// DesignObj table constants
		//
		public const String DESIGNOBJ_TABLE = "DesignObj";
		public const String PK_DESIGNOBJ_FIELD = "PK_DesignObj";
		public const String DESCRIPTION_FIELD = "Description";
		public const String DEFINE_FIELD = "Define";
		public const String FK_DESIGNOBJTYPE_FIELD = "FK_DesignObjType";
		public const String FK_DESIGNOBJCATEGORY_FIELD = "FK_DesignObjCategory";
		public const String FK_DESIGNOBJ_INCLUDEIFOTHERINCLUDED_FIELD = "FK_DesignObj_IncludeIfOtherIncluded";
		public const String PRIORITY_FIELD = "Priority";
		public const String CANTGOBACK_FIELD = "CantGoBack";
		public const String COMMANDSPROCESSEDATSERVER_FIELD = "CommandsProcessedAtServer";
		public const String TIMEOUTSECONDS_FIELD = "TimeoutSeconds";
		public const String ANIMATE_FIELD = "Animate";
		public const String FK_STABILITYSTATUS_FIELD = "FK_StabilityStatus";
		// table+field constants
		public const String PK_DESIGNOBJ_TABLE_FIELD = "DesignObj.PK_DesignObj";
		public const String DESCRIPTION_TABLE_FIELD = "DesignObj.Description";
		public const String DEFINE_TABLE_FIELD = "DesignObj.Define";
		public const String FK_DESIGNOBJTYPE_TABLE_FIELD = "DesignObj.FK_DesignObjType";
		public const String FK_DESIGNOBJCATEGORY_TABLE_FIELD = "DesignObj.FK_DesignObjCategory";
		public const String FK_DESIGNOBJ_INCLUDEIFOTHERINCLUDED_TABLE_FIELD = "DesignObj.FK_DesignObj_IncludeIfOtherIncluded";
		public const String PRIORITY_TABLE_FIELD = "DesignObj.Priority";
		public const String CANTGOBACK_TABLE_FIELD = "DesignObj.CantGoBack";
		public const String COMMANDSPROCESSEDATSERVER_TABLE_FIELD = "DesignObj.CommandsProcessedAtServer";
		public const String TIMEOUTSECONDS_TABLE_FIELD = "DesignObj.TimeoutSeconds";
		public const String ANIMATE_TABLE_FIELD = "DesignObj.Animate";
		public const String FK_STABILITYSTATUS_TABLE_FIELD = "DesignObj.FK_StabilityStatus";
		// DataSetCommand object
		protected OdbcDataAdapter m_DSCommand;

		// Stored procedure parameters
		protected const String PK_DESIGNOBJ_PARM = "@PK_DesignObj";
		protected const String DESCRIPTION_PARM = "@Description";
		protected const String DEFINE_PARM = "@Define";
		protected const String FK_DESIGNOBJTYPE_PARM = "@FK_DesignObjType";
		protected const String FK_DESIGNOBJCATEGORY_PARM = "@FK_DesignObjCategory";
		protected const String FK_DESIGNOBJ_INCLUDEIFOTHERINCLUDED_PARM = "@FK_DesignObj_IncludeIfOtherIncluded";
		protected const String PRIORITY_PARM = "@Priority";
		protected const String CANTGOBACK_PARM = "@CantGoBack";
		protected const String COMMANDSPROCESSEDATSERVER_PARM = "@CommandsProcessedAtServer";
		protected const String TIMEOUTSECONDS_PARM = "@TimeoutSeconds";
		protected const String ANIMATE_PARM = "@Animate";
		protected const String FK_STABILITYSTATUS_PARM = "@FK_StabilityStatus";
		protected const String USERID_PARM = "@UserID";

		protected OdbcCommand m_LoadCommand;
		protected OdbcCommand m_InsertCommand;
		protected OdbcCommand m_UpdateCommand;
		protected OdbcCommand m_DeleteCommand;
		protected OdbcConnection m_Connection;
		protected OdbcTransaction m_Transaction;
		public DataTable Table { get { return Tables[0]; } }


		public DesignObjData() {  // marker:1
			//
			// Create the tables in the dataset
			//
			Tables.Add(BuildDataTables());
			m_Connection = HADataConfiguration.GetOdbcConnection();
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

			m_DSCommand.TableMappings.Add("Table", DesignObjData.DESIGNOBJ_TABLE);
		}

		public DesignObjData(OdbcConnection conn,OdbcTransaction trans) {

			m_Connection = conn;
			m_Transaction = trans;
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

			m_DSCommand.TableMappings.Add("Table", DesignObjData.DESIGNOBJ_TABLE);
		}

		private DesignObjData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context) {
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
			return (DataTable) BuildDesignObjTable();
		}
		public static DesignObjTable BuildDesignObjTable() {
			//
			// Create the DesignObj table
			//
			DesignObjTable Table = new DesignObjTable();
			DataColumnCollection Columns = Table.Columns;
			DataColumn[] PKColumns = new DataColumn[1];

			DataColumn Column = Columns.Add(PK_DESIGNOBJ_FIELD, typeof(System.Int32));
			Column.AutoIncrement = true;
			Column.AutoIncrementSeed = -1;
			Column.AutoIncrementStep = -1;
			PKColumns[0] = Column;

			Column = Columns.Add(DESCRIPTION_FIELD, typeof(System.String));
			Column.AllowDBNull = false;

			Columns.Add(DEFINE_FIELD, typeof(System.String));
			Column = Columns.Add(FK_DESIGNOBJTYPE_FIELD, typeof(System.Int32));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Column = Columns.Add(FK_DESIGNOBJCATEGORY_FIELD, typeof(System.Int32));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Columns.Add(FK_DESIGNOBJ_INCLUDEIFOTHERINCLUDED_FIELD, typeof(System.Int32));
			Column = Columns.Add(PRIORITY_FIELD, typeof(System.SByte));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Column = Columns.Add(CANTGOBACK_FIELD, typeof(System.Boolean));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Column = Columns.Add(COMMANDSPROCESSEDATSERVER_FIELD, typeof(System.Boolean));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Columns.Add(TIMEOUTSECONDS_FIELD, typeof(System.Int16));
			Column = Columns.Add(ANIMATE_FIELD, typeof(System.Boolean));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Columns.Add(FK_STABILITYSTATUS_FIELD, typeof(System.Int32));
			Table.PrimaryKey = PKColumns;

			return Table;
		}
		protected static void CreateParameters(OdbcParameterCollection Params, bool IsInsert) {
			Params.Add(new OdbcParameter(PK_DESIGNOBJ_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(DESCRIPTION_PARM, OdbcType.VarChar, 30));
			Params.Add(new OdbcParameter(DEFINE_PARM, OdbcType.VarChar, 25));
			Params.Add(new OdbcParameter(FK_DESIGNOBJTYPE_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_DESIGNOBJCATEGORY_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_DESIGNOBJ_INCLUDEIFOTHERINCLUDED_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(PRIORITY_PARM, OdbcType.TinyInt,1));
			Params.Add(new OdbcParameter(CANTGOBACK_PARM, OdbcType.Bit,1));
			Params.Add(new OdbcParameter(COMMANDSPROCESSEDATSERVER_PARM, OdbcType.Bit,1));
			Params.Add(new OdbcParameter(TIMEOUTSECONDS_PARM, OdbcType.SmallInt,2));
			Params.Add(new OdbcParameter(ANIMATE_PARM, OdbcType.Bit,1));
			Params.Add(new OdbcParameter(FK_STABILITYSTATUS_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(USERID_PARM, OdbcType.Int));

			// map the parameters to the data table

			Params[PK_DESIGNOBJ_PARM].SourceColumn = DesignObjData.PK_DESIGNOBJ_FIELD;
			if(IsInsert) {
				Params[PK_DESIGNOBJ_PARM].Direction = ParameterDirection.Output;
			}

			Params[DESCRIPTION_PARM].SourceColumn = DesignObjData.DESCRIPTION_FIELD;
			Params[DEFINE_PARM].SourceColumn = DesignObjData.DEFINE_FIELD;
			Params[FK_DESIGNOBJTYPE_PARM].SourceColumn = DesignObjData.FK_DESIGNOBJTYPE_FIELD;
			Params[FK_DESIGNOBJCATEGORY_PARM].SourceColumn = DesignObjData.FK_DESIGNOBJCATEGORY_FIELD;
			Params[FK_DESIGNOBJ_INCLUDEIFOTHERINCLUDED_PARM].SourceColumn = DesignObjData.FK_DESIGNOBJ_INCLUDEIFOTHERINCLUDED_FIELD;
			Params[PRIORITY_PARM].SourceColumn = DesignObjData.PRIORITY_FIELD;
			Params[CANTGOBACK_PARM].SourceColumn = DesignObjData.CANTGOBACK_FIELD;
			Params[COMMANDSPROCESSEDATSERVER_PARM].SourceColumn = DesignObjData.COMMANDSPROCESSEDATSERVER_FIELD;
			Params[TIMEOUTSECONDS_PARM].SourceColumn = DesignObjData.TIMEOUTSECONDS_FIELD;
			Params[ANIMATE_PARM].SourceColumn = DesignObjData.ANIMATE_FIELD;
			Params[FK_STABILITYSTATUS_PARM].SourceColumn = DesignObjData.FK_STABILITYSTATUS_FIELD;
		}

		protected static void CreateCommands(OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
			if(LoadCommand == null) {
				// Create the command since it's null
				LoadCommand = new OdbcCommand("sp_Select_DesignObj", Conn);
				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(PK_DESIGNOBJ_PARM, OdbcType.Int,4));
			}

			if(InsertCommand == null) {
				// Create the command since it's null
				InsertCommand = new OdbcCommand("sp_Insert_DesignObj", Conn);
				InsertCommand.CommandType = CommandType.StoredProcedure;
				InsertCommand.Transaction = Trans;

				OdbcParameterCollection Params = InsertCommand.Parameters;

				CreateParameters(Params, true);

			}

			if(UpdateCommand == null) {
				// Create the command since it's null
				UpdateCommand = new OdbcCommand("sp_Update_DesignObj", Conn);
				UpdateCommand.CommandType = CommandType.StoredProcedure;
				UpdateCommand.Transaction = Trans;

				OdbcParameterCollection Params = UpdateCommand.Parameters;

				CreateParameters(Params, false);

			}
			if (DeleteCommand == null)
			{
				DeleteCommand = new OdbcCommand("sp_Delete_DesignObj", Conn);
				DeleteCommand.CommandType = CommandType.StoredProcedure;
				DeleteCommand.Transaction = Trans;

				DeleteCommand.Parameters.Add(PK_DESIGNOBJ_PARM, OdbcType.Int,4, PK_DESIGNOBJ_FIELD);
				DeleteCommand.Parameters.Add(USERID_PARM, OdbcType.Int);
			}
		}

		protected static void CreateCommands(OdbcDataAdapter odbcda,OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
				LoadCommand = new OdbcCommand("SELECT PK_DesignObj,Description,Define,FK_DesignObjType,FK_DesignObjCategory,FK_DesignObj_IncludeIfOtherIncluded,Priority,CantGoBack,CommandsProcessedAtServer,TimeoutSeconds,Animate,FK_StabilityStatus FROM DesignObj", Conn);
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(PK_DESIGNOBJ_PARM, OdbcType.Int,4));

			odbcda.SelectCommand = LoadCommand;
			OdbcCommandBuilder odbcCB = new OdbcCommandBuilder(odbcda);
			odbcCB.RefreshSchema();
			DeleteCommand = odbcCB.GetDeleteCommand();
			InsertCommand = odbcCB.GetInsertCommand();
			UpdateCommand = odbcCB.GetUpdateCommand();
		}

		public DesignObjData LoadDesignObj(System.Int32 PK_DesignObj)
		{
			m_DSCommand.SelectCommand = m_LoadCommand;
			m_DSCommand.SelectCommand.Parameters[PK_DESIGNOBJ_PARM].Value = PK_DesignObj;

			m_DSCommand.Fill(this);
			return this;
		}

		public static DataRowCollection LoadDesignObjWithWhere(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, string WhereClause) // marker:2
		{
			DataRowCollection dr;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["DesignObj"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}
			
			DataSet dsTemp = new MyDataSet();
			dsTemp.Tables.Add(BuildDataTables());
			
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			string sSQL = "SELECT PK_DesignObj, Description, Define, FK_DesignObjType, FK_DesignObjCategory, FK_DesignObj_IncludeIfOtherIncluded, Priority, CantGoBack, CommandsProcessedAtServer, TimeoutSeconds, Animate, FK_StabilityStatus FROM DesignObj WHERE " + WhereClause;
			
			OdbcCommand LoadCommand = new OdbcCommand(sSQL,conn);
			
			if( trans!=null )
				LoadCommand.Transaction = trans;
			
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(dsTemp,"DesignObj");
			
			dr=dsTemp.Tables["DesignObj"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}

		public static DataRow LoadNoCacheDesignObj(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 PK_DesignObj)
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["DesignObj"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}

			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand;
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();

			LoadCommand = new OdbcCommand("sp_Select_DesignObj", conn);

			LoadCommand.CommandType = CommandType.StoredProcedure;
			LoadCommand.Parameters.Add(new OdbcParameter(PK_DESIGNOBJ_PARM, OdbcType.Int,4));
			LoadCommand.Parameters[PK_DESIGNOBJ_PARM].Value = PK_DesignObj;
			if( trans!=null )
				LoadCommand.Transaction = trans;
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(ds,"DesignObj");
				dr = ds.Tables["DesignObj"].Rows.Find(PK_DesignObj);
			return dr;
		}

		public static DataRow LoadDesignObj(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 PK_DesignObj)  // marker:3
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["DesignObj"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
				else
				dr = dt.Rows.Find(PK_DesignObj);
			}

			if( dr==null )
			{
				OdbcDataAdapter sqlda = new OdbcDataAdapter();
				OdbcCommand LoadCommand;
				if( conn==null )
					conn = HADataConfiguration.GetOdbcConnection();

				LoadCommand = new OdbcCommand("sp_Select_DesignObj", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new OdbcParameter(PK_DESIGNOBJ_PARM, OdbcType.Int,4));
				LoadCommand.Parameters[PK_DESIGNOBJ_PARM].Value = PK_DesignObj;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(ds,"DesignObj");
				dr = ds.Tables["DesignObj"].Rows.Find(PK_DesignObj);
			}
			return dr;
		}

		public DesignObjData LoadAll() {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new OdbcCommand("SELECT * FROM DesignObj", m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this);
			return this;

		}

		public static DataRowCollection LoadAll(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {

			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT * FROM DesignObj", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(ds,"DesignObj")==0 )
				return null;
			else
				return ds.Tables["DesignObj"].Rows;

		}

		public DesignObjData ExecuteQuery(String sSQL) {
			return ExecuteQuery(sSQL,DESIGNOBJ_TABLE);
		}
		public DesignObjData ExecuteQuery(String sSQL,String sTableName) {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new OdbcCommand(sSQL, m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this,sTableName);

			return this;
		}

		public static DataRowCollection ExecuteQuery(String sSQL,ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {
			return ExecuteQuery(sSQL,ref ds,conn,trans,"DesignObj");
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

		public bool UpdateDesignObj(int CurUserID) {
			m_DSCommand.UpdateCommand = m_UpdateCommand;
			m_DSCommand.UpdateCommand.Parameters[USERID_PARM].Value = CurUserID;

			m_DSCommand.InsertCommand = m_InsertCommand;
			m_DSCommand.InsertCommand.Parameters[USERID_PARM].Value = CurUserID;

			if( m_DeleteCommand != null )
			{
				m_DSCommand.DeleteCommand = m_DeleteCommand;
				m_DSCommand.DeleteCommand.Parameters[USERID_PARM].Value = CurUserID;
			}

			m_DSCommand.Update(this, DesignObjData.DESIGNOBJ_TABLE);
			return true;
		}

		public static bool UpdateDesignObj(ref MyDataSet ds, int CurUserID)
		{
			OdbcConnection OdbcConn = HADataConfiguration.GetOdbcConnection();
			return UpdateDesignObj(ref ds,CurUserID,OdbcConn,null);
		}

		public static bool UpdateDesignObj(ref MyDataSet ds, int CurUserID,OdbcConnection OdbcConn,OdbcTransaction Trans)
		{
			DataTable dt = ds.Tables[DESIGNOBJ_TABLE];
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

	} // public class DesignObjData
	public class DesignObjDataRow
	{
		public DataRow dr = null;
		public DesignObjDataRow(DataRow d)
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
		public System.Int32 fPK_DesignObj
		{
			get
			{
				return Convert.ToInt32(dr[0]);
			}
		}
		public System.String fDescription
		{
			get
			{
				return Convert.ToString(dr[1]);
			}
			set
			{
				dr[1]=value;
			}
		}
		public System.String fDefine
		{
			get
			{
				return Convert.ToString(dr[2]);
			}
			set
			{
				dr[2]=value;
			}
		}
		public bool fDefineIsNull
		{
			get
			{
				return dr[2]==DBNull.Value;
			}
		}
		public void fDefineSetNull()
		{
			dr[2]=DBNull.Value;
		}
		public System.Int32 fFK_DesignObjType
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
		public DesignObjTypeDataRow fFK_DesignObjType_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tDesignObjType[Convert.ToInt32(dr[3])];
			}
		}
		public System.Int32 fFK_DesignObjCategory
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
		public DesignObjCategoryDataRow fFK_DesignObjCategory_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tDesignObjCategory[Convert.ToInt32(dr[4])];
			}
		}
		public System.Int32 fFK_DesignObj_IncludeIfOtherIncluded
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
		public bool fFK_DesignObj_IncludeIfOtherIncludedIsNull
		{
			get
			{
				return dr[5]==DBNull.Value;
			}
		}
		public void fFK_DesignObj_IncludeIfOtherIncludedSetNull()
		{
			dr[5]=DBNull.Value;
		}
		public DesignObjDataRow fFK_DesignObj_IncludeIfOtherIncluded_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tDesignObj[Convert.ToInt32(dr[5])];
			}
		}
		public System.SByte fPriority
		{
			get
			{
				return Convert.ToSByte(dr[6]);
			}
			set
			{
				dr[6]=value;
			}
		}
		public System.Boolean fCantGoBack
		{
			get
			{
				return Convert.ToBoolean(dr[7]);
			}
			set
			{
				dr[7]=value;
			}
		}
		public System.Boolean fCommandsProcessedAtServer
		{
			get
			{
				return Convert.ToBoolean(dr[8]);
			}
			set
			{
				dr[8]=value;
			}
		}
		public System.Int16 fTimeoutSeconds
		{
			get
			{
				return Convert.ToInt16(dr[9]);
			}
			set
			{
				dr[9]=value;
			}
		}
		public bool fTimeoutSecondsIsNull
		{
			get
			{
				return dr[9]==DBNull.Value;
			}
		}
		public void fTimeoutSecondsSetNull()
		{
			dr[9]=DBNull.Value;
		}
		public System.Boolean fAnimate
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
		public System.Int32 fFK_StabilityStatus
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
		public bool fFK_StabilityStatusIsNull
		{
			get
			{
				return dr[11]==DBNull.Value;
			}
		}
		public void fFK_StabilityStatusSetNull()
		{
			dr[11]=DBNull.Value;
		}
	} // public class DesignObjDataRow
	public class DesignObjDataReader
	{
		public OdbcDataReader dr;
		bool bCache=false;
		int iRecord=-1,iNumRecords=-1;
		ArrayList al = null;

		public DesignObjDataReader(OdbcDataReader d)
		{
			dr=d;
		}
		public DesignObjDataReader(OdbcCommand cmd)
		{
			dr = cmd.ExecuteReader();
		}

		public DesignObjDataReader(OdbcCommand cmd,bool Cache)
		{
			dr = cmd.ExecuteReader();
			bCache=Cache;
			if( bCache )
				CacheAndClose();
		}

		public DesignObjDataReader(string sSQL)
		{
			OdbcConnection conn = HADataConfiguration.GetOdbcConnection();

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM DesignObj WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public DesignObjDataReader(string sSQL,OdbcConnection conn)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM DesignObj WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public DesignObjDataReader(string sSQL,OdbcConnection conn,OdbcTransaction trans,bool Cache)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM DesignObj WHERE " + sSQL;
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
				object[] objs = new object[12];
				for(int i=0;i<12;i++)
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
		public System.Int32 fPK_DesignObj
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[0]);
				else
					return Convert.ToInt32(dr[0]);
			}
		}
		public System.String fDescription
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[1]);
				else
					return Convert.ToString(dr[1]);
			}
		}
		public System.String fDefine
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[2]);
				else
					return Convert.ToString(dr[2]);
			}
		}
		public bool fDefineIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[2]==DBNull.Value;
				else
					return dr[2]==DBNull.Value;
			}
		}
		public System.Int32 fFK_DesignObjType
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[3]);
				else
					return Convert.ToInt32(dr[3]);
			}
		}
		public System.Int32 fFK_DesignObjCategory
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[4]);
				else
					return Convert.ToInt32(dr[4]);
			}
		}
		public System.Int32 fFK_DesignObj_IncludeIfOtherIncluded
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[5]);
				else
					return Convert.ToInt32(dr[5]);
			}
		}
		public bool fFK_DesignObj_IncludeIfOtherIncludedIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[5]==DBNull.Value;
				else
					return dr[5]==DBNull.Value;
			}
		}
		public System.SByte fPriority
		{
			get
			{
				if( bCache )
					return Convert.ToSByte(((object[]) al[iRecord])[6]);
				else
					return Convert.ToSByte(dr[6]);
			}
		}
		public System.Boolean fCantGoBack
		{
			get
			{
				if( bCache )
					return Convert.ToBoolean(((object[]) al[iRecord])[7]);
				else
					return Convert.ToBoolean(dr[7]);
			}
		}
		public System.Boolean fCommandsProcessedAtServer
		{
			get
			{
				if( bCache )
					return Convert.ToBoolean(((object[]) al[iRecord])[8]);
				else
					return Convert.ToBoolean(dr[8]);
			}
		}
		public System.Int16 fTimeoutSeconds
		{
			get
			{
				if( bCache )
					return Convert.ToInt16(((object[]) al[iRecord])[9]);
				else
					return Convert.ToInt16(dr[9]);
			}
		}
		public bool fTimeoutSecondsIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[9]==DBNull.Value;
				else
					return dr[9]==DBNull.Value;
			}
		}
		public System.Boolean fAnimate
		{
			get
			{
				if( bCache )
					return Convert.ToBoolean(((object[]) al[iRecord])[10]);
				else
					return Convert.ToBoolean(dr[10]);
			}
		}
		public System.Int32 fFK_StabilityStatus
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[11]);
				else
					return Convert.ToInt32(dr[11]);
			}
		}
		public bool fFK_StabilityStatusIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[11]==DBNull.Value;
				else
					return dr[11]==DBNull.Value;
			}
		}
	} // public class DesignObjDataReader
	public class DesignObjTable : DataTable
	{
		public DesignObjTable() : base("DesignObj") {}

		public DesignObjDataRow this [System.Int32 PK_DesignObj]
		{
			get
			{
				DesignObjDataRow dr = new DesignObjDataRow(Rows.Find(PK_DesignObj));
				if( !dr.bIsValid  && false /* can't do this with ODBC */  )
				{
					MyDataSet mds = (MyDataSet) DataSet;
					if( mds.m_conn==null )
						return dr;
					OdbcDataAdapter sqlda = new OdbcDataAdapter();
					OdbcCommand LoadCommand = new OdbcCommand("sp_Select_DesignObj", mds.m_conn,mds.m_trans);
					LoadCommand.CommandType = CommandType.StoredProcedure;
					LoadCommand.Parameters.Add(new OdbcParameter("@PK_DesignObj", OdbcType.Int,4));
					LoadCommand.Parameters["@PK_DesignObj"].Value = PK_DesignObj;
					sqlda.SelectCommand = LoadCommand;
					sqlda.Fill(mds,"DesignObj");
					dr = new DesignObjDataRow(Rows.Find(PK_DesignObj));
				}
				return dr;
			}
		}
		public DataRowCollection LoadAll(OdbcConnection conn, OdbcTransaction trans)
		{
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT PK_DesignObj,Description,Define,FK_DesignObjType,FK_DesignObjCategory,FK_DesignObj_IncludeIfOtherIncluded,Priority,CantGoBack,CommandsProcessedAtServer,TimeoutSeconds,Animate,FK_StabilityStatus FROM DesignObj", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(this.DataSet,"DesignObj")==0 )
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
			DesignObjData.UpdateDesignObj(ref ds,PK_Users,conn,trans);
		}
		public DataColumn cPK_DesignObj
		{
			get
			{
				return Columns[0];
			}
		}
		public DataColumn cDescription
		{
			get
			{
				return Columns[1];
			}
		}
		public DataColumn cDefine
		{
			get
			{
				return Columns[2];
			}
		}
		public DataColumn cFK_DesignObjType
		{
			get
			{
				return Columns[3];
			}
		}
		public DataColumn cFK_DesignObjCategory
		{
			get
			{
				return Columns[4];
			}
		}
		public DataColumn cFK_DesignObj_IncludeIfOtherIncluded
		{
			get
			{
				return Columns[5];
			}
		}
		public DataColumn cPriority
		{
			get
			{
				return Columns[6];
			}
		}
		public DataColumn cCantGoBack
		{
			get
			{
				return Columns[7];
			}
		}
		public DataColumn cCommandsProcessedAtServer
		{
			get
			{
				return Columns[8];
			}
		}
		public DataColumn cTimeoutSeconds
		{
			get
			{
				return Columns[9];
			}
		}
		public DataColumn cAnimate
		{
			get
			{
				return Columns[10];
			}
		}
		public DataColumn cFK_StabilityStatus
		{
			get
			{
				return Columns[11];
			}
		}
	}
} // namespace HAData.Common.Data
