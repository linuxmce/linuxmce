namespace HAData.DataAccess {
	using System;
	using System.Data;
	using MySql;
	using MySql.Data;
	using MySql.Data.MySqlClient;
	using System.Collections;

	using HAData.Common;

	public class CommandGroup_D_Command_CommandParameterData : MyDataSet {
		//
		// CommandGroup_D_Command_CommandParameter table constants
		//
		public const String COMMANDGROUP_D_COMMAND_COMMANDPARAMETER_TABLE = "CommandGroup_D_Command_CommandParameter";
		public const String FK_COMMANDGROUP_D_COMMAND_FIELD = "FK_CommandGroup_D_Command";
		public const String FK_COMMANDPARAMETER_FIELD = "FK_CommandParameter";
		public const String IK_COMMANDPARAMETER_FIELD = "IK_CommandParameter";
		// table+field constants
		public const String FK_COMMANDGROUP_D_COMMAND_TABLE_FIELD = "CommandGroup_D_Command_CommandParameter.FK_CommandGroup_D_Command";
		public const String FK_COMMANDPARAMETER_TABLE_FIELD = "CommandGroup_D_Command_CommandParameter.FK_CommandParameter";
		public const String IK_COMMANDPARAMETER_TABLE_FIELD = "CommandGroup_D_Command_CommandParameter.IK_CommandParameter";
		// DataSetCommand object
		protected MySqlDataAdapter m_DSCommand;

		// Stored procedure parameters
		protected const String FK_COMMANDGROUP_D_COMMAND_PARM = "@FK_CommandGroup_D_Command";
		protected const String FK_COMMANDPARAMETER_PARM = "@FK_CommandParameter";
		protected const String IK_COMMANDPARAMETER_PARM = "@IK_CommandParameter";
		protected const String USERID_PARM = "@UserID";

		protected MySqlCommand m_LoadCommand;
		protected MySqlCommand m_InsertCommand;
		protected MySqlCommand m_UpdateCommand;
		protected MySqlCommand m_DeleteCommand;
		protected MySqlConnection m_Connection;
		protected MySqlTransaction m_Transaction;
		public DataTable Table { get { return Tables[0]; } }


		public CommandGroup_D_Command_CommandParameterData() {  // marker:1
			//
			// Create the tables in the dataset
			//
			Tables.Add(BuildDataTables());
			m_Connection = HADataConfiguration.GetMySqlConnection();
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new MySqlDataAdapter();

			m_DSCommand.TableMappings.Add("Table", CommandGroup_D_Command_CommandParameterData.COMMANDGROUP_D_COMMAND_COMMANDPARAMETER_TABLE);
		}

		public CommandGroup_D_Command_CommandParameterData(MySqlConnection conn,MySqlTransaction trans) {

			m_Connection = conn;
			m_Transaction = trans;
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new MySqlDataAdapter();

			m_DSCommand.TableMappings.Add("Table", CommandGroup_D_Command_CommandParameterData.COMMANDGROUP_D_COMMAND_COMMANDPARAMETER_TABLE);
		}

		private CommandGroup_D_Command_CommandParameterData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context) {
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
			return (DataTable) BuildCommandGroup_D_Command_CommandParameterTable();
		}
		public static CommandGroup_D_Command_CommandParameterTable BuildCommandGroup_D_Command_CommandParameterTable() {
			//
			// Create the CommandGroup_D_Command_CommandParameter table
			//
			CommandGroup_D_Command_CommandParameterTable Table = new CommandGroup_D_Command_CommandParameterTable();
			DataColumnCollection Columns = Table.Columns;
			DataColumn[] PKColumns = new DataColumn[2];

			DataColumn Column = Columns.Add(FK_COMMANDGROUP_D_COMMAND_FIELD, typeof(System.Int32));
			Column.DefaultValue = 0;
			PKColumns[0] = Column;
			Column.DefaultValue = -1;

			Column = Columns.Add(FK_COMMANDPARAMETER_FIELD, typeof(System.Int32));
			Column.DefaultValue = 0;
			PKColumns[1] = Column;
			Column.DefaultValue = -1;

			Columns.Add(IK_COMMANDPARAMETER_FIELD, typeof(System.String));
			Table.PrimaryKey = PKColumns;

			return Table;
		}
		protected static void CreateParameters(MySqlParameterCollection Params, bool IsInsert) {
			Params.Add(new MySqlParameter(FK_COMMANDGROUP_D_COMMAND_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(FK_COMMANDPARAMETER_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(IK_COMMANDPARAMETER_PARM, MySqlDbType.VarChar, 50));
			Params.Add(new MySqlParameter(USERID_PARM, MySqlDbType.Int32));

			// map the parameters to the data table

			Params[FK_COMMANDGROUP_D_COMMAND_PARM].SourceColumn = CommandGroup_D_Command_CommandParameterData.FK_COMMANDGROUP_D_COMMAND_FIELD;
			Params[FK_COMMANDPARAMETER_PARM].SourceColumn = CommandGroup_D_Command_CommandParameterData.FK_COMMANDPARAMETER_FIELD;
			Params[IK_COMMANDPARAMETER_PARM].SourceColumn = CommandGroup_D_Command_CommandParameterData.IK_COMMANDPARAMETER_FIELD;
		}

		protected static void CreateCommands(MySqlConnection Conn, MySqlTransaction Trans, ref MySqlCommand LoadCommand, ref MySqlCommand InsertCommand, ref MySqlCommand UpdateCommand, ref MySqlCommand DeleteCommand) {
			if(LoadCommand == null) {
				// Create the command since it's null
				LoadCommand = new MySqlCommand("sp_Select_CommandGroup_D_Command_CommandParameter", Conn);
				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new MySqlParameter(FK_COMMANDGROUP_D_COMMAND_PARM, MySqlDbType.Int32,4));
				LoadCommand.Parameters.Add(new MySqlParameter(FK_COMMANDPARAMETER_PARM, MySqlDbType.Int32,4));
			}

			if(InsertCommand == null) {
				// Create the command since it's null
				InsertCommand = new MySqlCommand("sp_Insert_CommandGroup_D_Command_CommandParameter", Conn);
				InsertCommand.CommandType = CommandType.StoredProcedure;
				InsertCommand.Transaction = Trans;

				MySqlParameterCollection Params = InsertCommand.Parameters;

				CreateParameters(Params, true);

			}

			if(UpdateCommand == null) {
				// Create the command since it's null
				UpdateCommand = new MySqlCommand("sp_Update_CommandGroup_D_Command_CommandParameter", Conn);
				UpdateCommand.CommandType = CommandType.StoredProcedure;
				UpdateCommand.Transaction = Trans;

				MySqlParameterCollection Params = UpdateCommand.Parameters;

				CreateParameters(Params, false);

			}
			if (DeleteCommand == null)
			{
				DeleteCommand = new MySqlCommand("sp_Delete_CommandGroup_D_Command_CommandParameter", Conn);
				DeleteCommand.CommandType = CommandType.StoredProcedure;
				DeleteCommand.Transaction = Trans;

				DeleteCommand.Parameters.Add(FK_COMMANDGROUP_D_COMMAND_PARM, MySqlDbType.Int32,4, FK_COMMANDGROUP_D_COMMAND_FIELD);
				DeleteCommand.Parameters.Add(FK_COMMANDPARAMETER_PARM, MySqlDbType.Int32,4, FK_COMMANDPARAMETER_FIELD);
				DeleteCommand.Parameters.Add(USERID_PARM, MySqlDbType.Int32);
			}
		}

		protected static void CreateCommands(MySqlDataAdapter odbcda,MySqlConnection Conn, MySqlTransaction Trans, ref MySqlCommand LoadCommand, ref MySqlCommand InsertCommand, ref MySqlCommand UpdateCommand, ref MySqlCommand DeleteCommand) {
				LoadCommand = new MySqlCommand("SELECT FK_CommandGroup_D_Command,FK_CommandParameter,IK_CommandParameter FROM CommandGroup_D_Command_CommandParameter", Conn);
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new MySqlParameter(FK_COMMANDGROUP_D_COMMAND_PARM, MySqlDbType.Int32,4));
				LoadCommand.Parameters.Add(new MySqlParameter(FK_COMMANDPARAMETER_PARM, MySqlDbType.Int32,4));

			odbcda.SelectCommand = LoadCommand;
			MySqlCommandBuilder odbcCB = new MySqlCommandBuilder(odbcda);
			odbcCB.RefreshSchema();
			DeleteCommand = odbcCB.GetDeleteCommand();
			InsertCommand = odbcCB.GetInsertCommand();
			UpdateCommand = odbcCB.GetUpdateCommand();
		}

		public CommandGroup_D_Command_CommandParameterData LoadCommandGroup_D_Command_CommandParameter(System.Int32 FK_CommandGroup_D_Command, System.Int32 FK_CommandParameter)
		{
			m_DSCommand.SelectCommand = m_LoadCommand;
			m_DSCommand.SelectCommand.Parameters[FK_COMMANDGROUP_D_COMMAND_PARM].Value = FK_CommandGroup_D_Command;
			m_DSCommand.SelectCommand.Parameters[FK_COMMANDPARAMETER_PARM].Value = FK_CommandParameter;

			m_DSCommand.Fill(this);
			return this;
		}

		public static DataRowCollection LoadCommandGroup_D_Command_CommandParameterWithWhere(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans, string WhereClause) // marker:2
		{
			DataRowCollection dr;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["CommandGroup_D_Command_CommandParameter"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}
			
			DataSet dsTemp = new MyDataSet();
			dsTemp.Tables.Add(BuildDataTables());
			
			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();
			
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			string sSQL = "SELECT FK_CommandGroup_D_Command, FK_CommandParameter, IK_CommandParameter FROM CommandGroup_D_Command_CommandParameter WHERE " + WhereClause;
			
			MySqlCommand LoadCommand = new MySqlCommand(sSQL,conn);
			
			if( trans!=null )
				LoadCommand.Transaction = trans;
			
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(dsTemp,"CommandGroup_D_Command_CommandParameter");
			
			dr=dsTemp.Tables["CommandGroup_D_Command_CommandParameter"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}

		public static DataRow LoadNoCacheCommandGroup_D_Command_CommandParameter(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans, System.Int32 FK_CommandGroup_D_Command, System.Int32 FK_CommandParameter)
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["CommandGroup_D_Command_CommandParameter"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}

			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand;
			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();

			LoadCommand = new MySqlCommand("sp_Select_CommandGroup_D_Command_CommandParameter", conn);

			LoadCommand.CommandType = CommandType.StoredProcedure;
			LoadCommand.Parameters.Add(new MySqlParameter(FK_COMMANDGROUP_D_COMMAND_PARM, MySqlDbType.Int32,4));
			LoadCommand.Parameters.Add(new MySqlParameter(FK_COMMANDPARAMETER_PARM, MySqlDbType.Int32,4));
			LoadCommand.Parameters[FK_COMMANDGROUP_D_COMMAND_PARM].Value = FK_CommandGroup_D_Command;
			LoadCommand.Parameters[FK_COMMANDPARAMETER_PARM].Value = FK_CommandParameter;
			if( trans!=null )
				LoadCommand.Transaction = trans;
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(ds,"CommandGroup_D_Command_CommandParameter");
			object[]findTheseVals = new object[2];
			findTheseVals[0] = FK_CommandGroup_D_Command;
			findTheseVals[1] = FK_CommandParameter;
			dr = ds.Tables["CommandGroup_D_Command_CommandParameter"].Rows.Find(findTheseVals);
			return dr;
		}

		public static DataRow LoadCommandGroup_D_Command_CommandParameter(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans, System.Int32 FK_CommandGroup_D_Command, System.Int32 FK_CommandParameter)  // marker:3
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				object[]findTheseVals = new object[2];
				findTheseVals[0] = FK_CommandGroup_D_Command;
				findTheseVals[1] = FK_CommandParameter;
				DataTable dt = ds.Tables["CommandGroup_D_Command_CommandParameter"];
				if( dt==null )
				{
						dt=BuildDataTables();
						ds.Tables.Add(dt);
				}
				else
					dr = dt.Rows.Find(findTheseVals);
			}

			if( dr==null )
			{
				MySqlDataAdapter sqlda = new MySqlDataAdapter();
				MySqlCommand LoadCommand;
				if( conn==null )
					conn = HADataConfiguration.GetMySqlConnection();

				LoadCommand = new MySqlCommand("sp_Select_CommandGroup_D_Command_CommandParameter", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new MySqlParameter(FK_COMMANDGROUP_D_COMMAND_PARM, MySqlDbType.Int32,4));
				LoadCommand.Parameters.Add(new MySqlParameter(FK_COMMANDPARAMETER_PARM, MySqlDbType.Int32,4));
				LoadCommand.Parameters[FK_COMMANDGROUP_D_COMMAND_PARM].Value = FK_CommandGroup_D_Command;
				LoadCommand.Parameters[FK_COMMANDPARAMETER_PARM].Value = FK_CommandParameter;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(ds,"CommandGroup_D_Command_CommandParameter");
				object[]findTheseVals = new object[2];
				findTheseVals[0] = FK_CommandGroup_D_Command;
				findTheseVals[1] = FK_CommandParameter;
				dr = ds.Tables["CommandGroup_D_Command_CommandParameter"].Rows.Find(findTheseVals);
			}
			return dr;
		}

		public static DataRowCollection LoadCommandGroup_D_Command_CommandParameter_FirstPK(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans,System.Int32 FK_CommandGroup_D_Command)
		{
			DataRowCollection dr;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["CommandGroup_D_Command_CommandParameter"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}
			
			DataSet dsTemp = new MyDataSet();
			dsTemp.Tables.Add(BuildDataTables());
			
			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();
			
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
				MySqlCommand LoadCommand;

				LoadCommand = new MySqlCommand("sp_Select_CommandGroup_D_Command_CommandParameter_FirstPK", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new MySqlParameter(FK_COMMANDGROUP_D_COMMAND_PARM, MySqlDbType.Int32,4));
				LoadCommand.Parameters[FK_COMMANDGROUP_D_COMMAND_PARM].Value = FK_CommandGroup_D_Command;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(dsTemp,"CommandGroup_D_Command_CommandParameter");
			
			dr=dsTemp.Tables["CommandGroup_D_Command_CommandParameter"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}
		public CommandGroup_D_Command_CommandParameterData LoadAll() {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new MySqlCommand("SELECT * FROM CommandGroup_D_Command_CommandParameter", m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this);
			return this;

		}

		public static DataRowCollection LoadAll(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans) {

			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand = new MySqlCommand("SELECT * FROM CommandGroup_D_Command_CommandParameter", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(ds,"CommandGroup_D_Command_CommandParameter")==0 )
				return null;
			else
				return ds.Tables["CommandGroup_D_Command_CommandParameter"].Rows;

		}

		public CommandGroup_D_Command_CommandParameterData ExecuteQuery(String sSQL) {
			return ExecuteQuery(sSQL,COMMANDGROUP_D_COMMAND_COMMANDPARAMETER_TABLE);
		}
		public CommandGroup_D_Command_CommandParameterData ExecuteQuery(String sSQL,String sTableName) {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new MySqlCommand(sSQL, m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this,sTableName);

			return this;
		}

		public static DataRowCollection ExecuteQuery(String sSQL,ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans) {
			return ExecuteQuery(sSQL,ref ds,conn,trans,"CommandGroup_D_Command_CommandParameter");
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

		public bool UpdateCommandGroup_D_Command_CommandParameter(int CurUserID) {
			m_DSCommand.UpdateCommand = m_UpdateCommand;
			m_DSCommand.UpdateCommand.Parameters[USERID_PARM].Value = CurUserID;

			m_DSCommand.InsertCommand = m_InsertCommand;
			m_DSCommand.InsertCommand.Parameters[USERID_PARM].Value = CurUserID;

			if( m_DeleteCommand != null )
			{
				m_DSCommand.DeleteCommand = m_DeleteCommand;
				m_DSCommand.DeleteCommand.Parameters[USERID_PARM].Value = CurUserID;
			}

			m_DSCommand.Update(this, CommandGroup_D_Command_CommandParameterData.COMMANDGROUP_D_COMMAND_COMMANDPARAMETER_TABLE);
			return true;
		}

		public static bool UpdateCommandGroup_D_Command_CommandParameter(ref MyDataSet ds, int CurUserID)
		{
			MySqlConnection OdbcConn = HADataConfiguration.GetMySqlConnection();
			return UpdateCommandGroup_D_Command_CommandParameter(ref ds,CurUserID,OdbcConn,null);
		}

		public static bool UpdateCommandGroup_D_Command_CommandParameter(ref MyDataSet ds, int CurUserID,MySqlConnection OdbcConn,MySqlTransaction Trans)
		{
			DataTable dt = ds.Tables[COMMANDGROUP_D_COMMAND_COMMANDPARAMETER_TABLE];
			if( dt == null )
				return false;

			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand = null;
			MySqlCommand InsertCommand = null;
			MySqlCommand UpdateCommand = null;
			MySqlCommand DeleteCommand = null;
			CreateCommands(sqlda,OdbcConn, Trans, ref LoadCommand, ref InsertCommand, ref UpdateCommand, ref DeleteCommand);

			sqlda.Update(dt);
			return true;
		}

	} // public class CommandGroup_D_Command_CommandParameterData
	public class CommandGroup_D_Command_CommandParameterDataRow
	{
		public DataRow dr = null;
		public CommandGroup_D_Command_CommandParameterDataRow(DataRow d)
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
		public System.Int32 fFK_CommandGroup_D_Command
		{
			get
			{
				return Convert.ToInt32(dr[0]);
			}
			set
			{
				dr[0]=value;
			}
		}
		public CommandGroup_D_CommandDataRow fFK_CommandGroup_D_Command_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tCommandGroup_D_Command[Convert.ToInt32(dr[0])];
			}
		}
		public System.Int32 fFK_CommandParameter
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
		public CommandParameterDataRow fFK_CommandParameter_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tCommandParameter[Convert.ToInt32(dr[1])];
			}
		}
		public System.String fIK_CommandParameter
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
		public bool fIK_CommandParameterIsNull
		{
			get
			{
				return dr[2]==DBNull.Value;
			}
		}
		public void fIK_CommandParameterSetNull()
		{
			dr[2]=DBNull.Value;
		}
	} // public class CommandGroup_D_Command_CommandParameterDataRow
	public class CommandGroup_D_Command_CommandParameterDataReader
	{
		public MySqlDataReader dr;
		bool bCache=false;
		int iRecord=-1,iNumRecords=-1;
		ArrayList al = null;

		public CommandGroup_D_Command_CommandParameterDataReader(MySqlDataReader d)
		{
			dr=d;
		}
		public CommandGroup_D_Command_CommandParameterDataReader(MySqlCommand cmd)
		{
			dr = cmd.ExecuteReader();
		}

		public CommandGroup_D_Command_CommandParameterDataReader(MySqlCommand cmd,bool Cache)
		{
			dr = cmd.ExecuteReader();
			bCache=Cache;
			if( bCache )
				CacheAndClose();
		}

		public CommandGroup_D_Command_CommandParameterDataReader(string sSQL)
		{
			MySqlConnection conn = HADataConfiguration.GetMySqlConnection();

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM CommandGroup_D_Command_CommandParameter WHERE " + sSQL;
			}

			MySqlCommand cmd = new MySqlCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public CommandGroup_D_Command_CommandParameterDataReader(string sSQL,MySqlConnection conn)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetMySqlConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM CommandGroup_D_Command_CommandParameter WHERE " + sSQL;
			}

			MySqlCommand cmd = new MySqlCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public CommandGroup_D_Command_CommandParameterDataReader(string sSQL,MySqlConnection conn,MySqlTransaction trans,bool Cache)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetMySqlConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM CommandGroup_D_Command_CommandParameter WHERE " + sSQL;
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
				object[] objs = new object[3];
				for(int i=0;i<3;i++)
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
		public System.Int32 fFK_CommandGroup_D_Command
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[0]);
				else
					return Convert.ToInt32(dr[0]);
			}
		}
		public System.Int32 fFK_CommandParameter
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[1]);
				else
					return Convert.ToInt32(dr[1]);
			}
		}
		public System.String fIK_CommandParameter
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[2]);
				else
					return Convert.ToString(dr[2]);
			}
		}
		public bool fIK_CommandParameterIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[2]==DBNull.Value;
				else
					return dr[2]==DBNull.Value;
			}
		}
	} // public class CommandGroup_D_Command_CommandParameterDataReader
	public class CommandGroup_D_Command_CommandParameterTable : DataTable
	{
		public CommandGroup_D_Command_CommandParameterTable() : base("CommandGroup_D_Command_CommandParameter") {}

		public CommandGroup_D_Command_CommandParameterDataRow this [System.Int32 FK_CommandGroup_D_Command, System.Int32 FK_CommandParameter]
		{
			get
			{
				object[]findTheseVals = new object[2];
				findTheseVals[0] = FK_CommandGroup_D_Command;
				findTheseVals[1] = FK_CommandParameter;
				CommandGroup_D_Command_CommandParameterDataRow dr = new CommandGroup_D_Command_CommandParameterDataRow(Rows.Find(findTheseVals));
				return dr;
			}
		}
		public DataRowCollection LoadAll(MySqlConnection conn, MySqlTransaction trans)
		{
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand = new MySqlCommand("SELECT FK_CommandGroup_D_Command,FK_CommandParameter,IK_CommandParameter FROM CommandGroup_D_Command_CommandParameter", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(this.DataSet,"CommandGroup_D_Command_CommandParameter")==0 )
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
			CommandGroup_D_Command_CommandParameterData.UpdateCommandGroup_D_Command_CommandParameter(ref ds,PK_Users,conn,trans);
		}
		public DataColumn cFK_CommandGroup_D_Command
		{
			get
			{
				return Columns[0];
			}
		}
		public DataColumn cFK_CommandParameter
		{
			get
			{
				return Columns[1];
			}
		}
		public DataColumn cIK_CommandParameter
		{
			get
			{
				return Columns[2];
			}
		}
	}
} // namespace HAData.Common.Data
