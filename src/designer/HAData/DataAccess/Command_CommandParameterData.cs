namespace HAData.DataAccess {
	using System;
	using System.Data;
	using Microsoft.Data.Odbc;
	using System.Collections;

	using HAData.Common;

	public class Command_CommandParameterData : MyDataSet {
		//
		// Command_CommandParameter table constants
		//
		public const String COMMAND_COMMANDPARAMETER_TABLE = "Command_CommandParameter";
		public const String FK_COMMAND_FIELD = "FK_Command";
		public const String FK_COMMANDPARAMETER_FIELD = "FK_CommandParameter";
		public const String DESCRIPTION_FIELD = "Description";
		// table+field constants
		public const String FK_COMMAND_TABLE_FIELD = "Command_CommandParameter.FK_Command";
		public const String FK_COMMANDPARAMETER_TABLE_FIELD = "Command_CommandParameter.FK_CommandParameter";
		public const String DESCRIPTION_TABLE_FIELD = "Command_CommandParameter.Description";
		// DataSetCommand object
		protected OdbcDataAdapter m_DSCommand;

		// Stored procedure parameters
		protected const String FK_COMMAND_PARM = "@FK_Command";
		protected const String FK_COMMANDPARAMETER_PARM = "@FK_CommandParameter";
		protected const String DESCRIPTION_PARM = "@Description";
		protected const String USERID_PARM = "@UserID";

		protected OdbcCommand m_LoadCommand;
		protected OdbcCommand m_InsertCommand;
		protected OdbcCommand m_UpdateCommand;
		protected OdbcCommand m_DeleteCommand;
		protected OdbcConnection m_Connection;
		protected OdbcTransaction m_Transaction;
		public DataTable Table { get { return Tables[0]; } }


		public Command_CommandParameterData() {  // marker:1
			//
			// Create the tables in the dataset
			//
			Tables.Add(BuildDataTables());
			m_Connection = HADataConfiguration.GetOdbcConnection();
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

			m_DSCommand.TableMappings.Add("Table", Command_CommandParameterData.COMMAND_COMMANDPARAMETER_TABLE);
		}

		public Command_CommandParameterData(OdbcConnection conn,OdbcTransaction trans) {

			m_Connection = conn;
			m_Transaction = trans;
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

			m_DSCommand.TableMappings.Add("Table", Command_CommandParameterData.COMMAND_COMMANDPARAMETER_TABLE);
		}

		private Command_CommandParameterData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context) {
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
			return (DataTable) BuildCommand_CommandParameterTable();
		}
		public static Command_CommandParameterTable BuildCommand_CommandParameterTable() {
			//
			// Create the Command_CommandParameter table
			//
			Command_CommandParameterTable Table = new Command_CommandParameterTable();
			DataColumnCollection Columns = Table.Columns;
			DataColumn[] PKColumns = new DataColumn[2];

			DataColumn Column = Columns.Add(FK_COMMAND_FIELD, typeof(System.Int32));
			Column.DefaultValue = 0;
			PKColumns[0] = Column;
			Column.DefaultValue = -1;

			Column = Columns.Add(FK_COMMANDPARAMETER_FIELD, typeof(System.Int32));
			Column.DefaultValue = 0;
			PKColumns[1] = Column;
			Column.DefaultValue = -1;

			Columns.Add(DESCRIPTION_FIELD, typeof(System.String));
			Table.PrimaryKey = PKColumns;

			return Table;
		}
		protected static void CreateParameters(OdbcParameterCollection Params, bool IsInsert) {
			Params.Add(new OdbcParameter(FK_COMMAND_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_COMMANDPARAMETER_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(DESCRIPTION_PARM, OdbcType.VarChar, 100));
			Params.Add(new OdbcParameter(USERID_PARM, OdbcType.Int));

			// map the parameters to the data table

			Params[FK_COMMAND_PARM].SourceColumn = Command_CommandParameterData.FK_COMMAND_FIELD;
			Params[FK_COMMANDPARAMETER_PARM].SourceColumn = Command_CommandParameterData.FK_COMMANDPARAMETER_FIELD;
			Params[DESCRIPTION_PARM].SourceColumn = Command_CommandParameterData.DESCRIPTION_FIELD;
		}

		protected static void CreateCommands(OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
			if(LoadCommand == null) {
				// Create the command since it's null
				LoadCommand = new OdbcCommand("sp_Select_Command_CommandParameter", Conn);
				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(FK_COMMAND_PARM, OdbcType.Int,4));
				LoadCommand.Parameters.Add(new OdbcParameter(FK_COMMANDPARAMETER_PARM, OdbcType.Int,4));
			}

			if(InsertCommand == null) {
				// Create the command since it's null
				InsertCommand = new OdbcCommand("sp_Insert_Command_CommandParameter", Conn);
				InsertCommand.CommandType = CommandType.StoredProcedure;
				InsertCommand.Transaction = Trans;

				OdbcParameterCollection Params = InsertCommand.Parameters;

				CreateParameters(Params, true);

			}

			if(UpdateCommand == null) {
				// Create the command since it's null
				UpdateCommand = new OdbcCommand("sp_Update_Command_CommandParameter", Conn);
				UpdateCommand.CommandType = CommandType.StoredProcedure;
				UpdateCommand.Transaction = Trans;

				OdbcParameterCollection Params = UpdateCommand.Parameters;

				CreateParameters(Params, false);

			}
			if (DeleteCommand == null)
			{
				DeleteCommand = new OdbcCommand("sp_Delete_Command_CommandParameter", Conn);
				DeleteCommand.CommandType = CommandType.StoredProcedure;
				DeleteCommand.Transaction = Trans;

				DeleteCommand.Parameters.Add(FK_COMMAND_PARM, OdbcType.Int,4, FK_COMMAND_FIELD);
				DeleteCommand.Parameters.Add(FK_COMMANDPARAMETER_PARM, OdbcType.Int,4, FK_COMMANDPARAMETER_FIELD);
				DeleteCommand.Parameters.Add(USERID_PARM, OdbcType.Int);
			}
		}

		protected static void CreateCommands(OdbcDataAdapter odbcda,OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
				LoadCommand = new OdbcCommand("SELECT FK_Command,FK_CommandParameter,Description FROM Command_CommandParameter", Conn);
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(FK_COMMAND_PARM, OdbcType.Int,4));
				LoadCommand.Parameters.Add(new OdbcParameter(FK_COMMANDPARAMETER_PARM, OdbcType.Int,4));

			odbcda.SelectCommand = LoadCommand;
			OdbcCommandBuilder odbcCB = new OdbcCommandBuilder(odbcda);
			odbcCB.RefreshSchema();
			DeleteCommand = odbcCB.GetDeleteCommand();
			InsertCommand = odbcCB.GetInsertCommand();
			UpdateCommand = odbcCB.GetUpdateCommand();
		}

		public Command_CommandParameterData LoadCommand_CommandParameter(System.Int32 FK_Command, System.Int32 FK_CommandParameter)
		{
			m_DSCommand.SelectCommand = m_LoadCommand;
			m_DSCommand.SelectCommand.Parameters[FK_COMMAND_PARM].Value = FK_Command;
			m_DSCommand.SelectCommand.Parameters[FK_COMMANDPARAMETER_PARM].Value = FK_CommandParameter;

			m_DSCommand.Fill(this);
			return this;
		}

		public static DataRowCollection LoadCommand_CommandParameterWithWhere(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, string WhereClause) // marker:2
		{
			DataRowCollection dr;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["Command_CommandParameter"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}
			
			DataSet dsTemp = new MyDataSet();
			dsTemp.Tables.Add(BuildDataTables());
			
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			string sSQL = "SELECT FK_Command, FK_CommandParameter, Description FROM Command_CommandParameter WHERE " + WhereClause;
			
			OdbcCommand LoadCommand = new OdbcCommand(sSQL,conn);
			
			if( trans!=null )
				LoadCommand.Transaction = trans;
			
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(dsTemp,"Command_CommandParameter");
			
			dr=dsTemp.Tables["Command_CommandParameter"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}

		public static DataRow LoadNoCacheCommand_CommandParameter(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 FK_Command, System.Int32 FK_CommandParameter)
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["Command_CommandParameter"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}

			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand;
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();

			LoadCommand = new OdbcCommand("sp_Select_Command_CommandParameter", conn);

			LoadCommand.CommandType = CommandType.StoredProcedure;
			LoadCommand.Parameters.Add(new OdbcParameter(FK_COMMAND_PARM, OdbcType.Int,4));
			LoadCommand.Parameters.Add(new OdbcParameter(FK_COMMANDPARAMETER_PARM, OdbcType.Int,4));
			LoadCommand.Parameters[FK_COMMAND_PARM].Value = FK_Command;
			LoadCommand.Parameters[FK_COMMANDPARAMETER_PARM].Value = FK_CommandParameter;
			if( trans!=null )
				LoadCommand.Transaction = trans;
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(ds,"Command_CommandParameter");
			object[]findTheseVals = new object[2];
			findTheseVals[0] = FK_Command;
			findTheseVals[1] = FK_CommandParameter;
			dr = ds.Tables["Command_CommandParameter"].Rows.Find(findTheseVals);
			return dr;
		}

		public static DataRow LoadCommand_CommandParameter(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 FK_Command, System.Int32 FK_CommandParameter)  // marker:3
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
				findTheseVals[0] = FK_Command;
				findTheseVals[1] = FK_CommandParameter;
				DataTable dt = ds.Tables["Command_CommandParameter"];
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
				OdbcDataAdapter sqlda = new OdbcDataAdapter();
				OdbcCommand LoadCommand;
				if( conn==null )
					conn = HADataConfiguration.GetOdbcConnection();

				LoadCommand = new OdbcCommand("sp_Select_Command_CommandParameter", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new OdbcParameter(FK_COMMAND_PARM, OdbcType.Int,4));
				LoadCommand.Parameters.Add(new OdbcParameter(FK_COMMANDPARAMETER_PARM, OdbcType.Int,4));
				LoadCommand.Parameters[FK_COMMAND_PARM].Value = FK_Command;
				LoadCommand.Parameters[FK_COMMANDPARAMETER_PARM].Value = FK_CommandParameter;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(ds,"Command_CommandParameter");
				object[]findTheseVals = new object[2];
				findTheseVals[0] = FK_Command;
				findTheseVals[1] = FK_CommandParameter;
				dr = ds.Tables["Command_CommandParameter"].Rows.Find(findTheseVals);
			}
			return dr;
		}

		public static DataRowCollection LoadCommand_CommandParameter_FirstPK(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans,System.Int32 FK_Command)
		{
			DataRowCollection dr;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["Command_CommandParameter"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}
			
			DataSet dsTemp = new MyDataSet();
			dsTemp.Tables.Add(BuildDataTables());
			
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
				OdbcCommand LoadCommand;

				LoadCommand = new OdbcCommand("sp_Select_Command_CommandParameter_FirstPK", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new OdbcParameter(FK_COMMAND_PARM, OdbcType.Int,4));
				LoadCommand.Parameters[FK_COMMAND_PARM].Value = FK_Command;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(dsTemp,"Command_CommandParameter");
			
			dr=dsTemp.Tables["Command_CommandParameter"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}
		public Command_CommandParameterData LoadAll() {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new OdbcCommand("SELECT * FROM Command_CommandParameter", m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this);
			return this;

		}

		public static DataRowCollection LoadAll(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {

			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT * FROM Command_CommandParameter", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(ds,"Command_CommandParameter")==0 )
				return null;
			else
				return ds.Tables["Command_CommandParameter"].Rows;

		}

		public Command_CommandParameterData ExecuteQuery(String sSQL) {
			return ExecuteQuery(sSQL,COMMAND_COMMANDPARAMETER_TABLE);
		}
		public Command_CommandParameterData ExecuteQuery(String sSQL,String sTableName) {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new OdbcCommand(sSQL, m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this,sTableName);

			return this;
		}

		public static DataRowCollection ExecuteQuery(String sSQL,ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {
			return ExecuteQuery(sSQL,ref ds,conn,trans,"Command_CommandParameter");
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

		public bool UpdateCommand_CommandParameter(int CurUserID) {
			m_DSCommand.UpdateCommand = m_UpdateCommand;
			m_DSCommand.UpdateCommand.Parameters[USERID_PARM].Value = CurUserID;

			m_DSCommand.InsertCommand = m_InsertCommand;
			m_DSCommand.InsertCommand.Parameters[USERID_PARM].Value = CurUserID;

			if( m_DeleteCommand != null )
			{
				m_DSCommand.DeleteCommand = m_DeleteCommand;
				m_DSCommand.DeleteCommand.Parameters[USERID_PARM].Value = CurUserID;
			}

			m_DSCommand.Update(this, Command_CommandParameterData.COMMAND_COMMANDPARAMETER_TABLE);
			return true;
		}

		public static bool UpdateCommand_CommandParameter(ref MyDataSet ds, int CurUserID)
		{
			OdbcConnection OdbcConn = HADataConfiguration.GetOdbcConnection();
			return UpdateCommand_CommandParameter(ref ds,CurUserID,OdbcConn,null);
		}

		public static bool UpdateCommand_CommandParameter(ref MyDataSet ds, int CurUserID,OdbcConnection OdbcConn,OdbcTransaction Trans)
		{
			DataTable dt = ds.Tables[COMMAND_COMMANDPARAMETER_TABLE];
			if( dt == null )
				return false;

			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = null;
			OdbcCommand InsertCommand = null;
			OdbcCommand UpdateCommand = null;
			OdbcCommand DeleteCommand = null;
			CreateCommands(sqlda,OdbcConn, Trans, ref LoadCommand, ref InsertCommand, ref UpdateCommand, ref DeleteCommand);

			sqlda.Update(dt);
			return true;
		}

	} // public class Command_CommandParameterData
	public class Command_CommandParameterDataRow
	{
		public DataRow dr = null;
		public Command_CommandParameterDataRow(DataRow d)
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
		public System.Int32 fFK_Command
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
		public CommandDataRow fFK_Command_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tCommand[Convert.ToInt32(dr[0])];
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
		public System.String fDescription
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
		public bool fDescriptionIsNull
		{
			get
			{
				return dr[2]==DBNull.Value;
			}
		}
		public void fDescriptionSetNull()
		{
			dr[2]=DBNull.Value;
		}
	} // public class Command_CommandParameterDataRow
	public class Command_CommandParameterDataReader
	{
		public OdbcDataReader dr;
		bool bCache=false;
		int iRecord=-1,iNumRecords=-1;
		ArrayList al = null;

		public Command_CommandParameterDataReader(OdbcDataReader d)
		{
			dr=d;
		}
		public Command_CommandParameterDataReader(OdbcCommand cmd)
		{
			dr = cmd.ExecuteReader();
		}

		public Command_CommandParameterDataReader(OdbcCommand cmd,bool Cache)
		{
			dr = cmd.ExecuteReader();
			bCache=Cache;
			if( bCache )
				CacheAndClose();
		}

		public Command_CommandParameterDataReader(string sSQL)
		{
			OdbcConnection conn = HADataConfiguration.GetOdbcConnection();

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM Command_CommandParameter WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public Command_CommandParameterDataReader(string sSQL,OdbcConnection conn)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM Command_CommandParameter WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public Command_CommandParameterDataReader(string sSQL,OdbcConnection conn,OdbcTransaction trans,bool Cache)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM Command_CommandParameter WHERE " + sSQL;
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
		public System.Int32 fFK_Command
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
		public System.String fDescription
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[2]);
				else
					return Convert.ToString(dr[2]);
			}
		}
		public bool fDescriptionIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[2]==DBNull.Value;
				else
					return dr[2]==DBNull.Value;
			}
		}
	} // public class Command_CommandParameterDataReader
	public class Command_CommandParameterTable : DataTable
	{
		public Command_CommandParameterTable() : base("Command_CommandParameter") {}

		public Command_CommandParameterDataRow this [System.Int32 FK_Command, System.Int32 FK_CommandParameter]
		{
			get
			{
				object[]findTheseVals = new object[2];
				findTheseVals[0] = FK_Command;
				findTheseVals[1] = FK_CommandParameter;
				Command_CommandParameterDataRow dr = new Command_CommandParameterDataRow(Rows.Find(findTheseVals));
				if( !dr.bIsValid  && false /* can't do this with ODBC */  )
				{
					MyDataSet mds = (MyDataSet) DataSet;
					if( mds.m_conn==null )
						return dr;
					OdbcDataAdapter sqlda = new OdbcDataAdapter();
					OdbcCommand LoadCommand = new OdbcCommand("sp_Select_Command_CommandParameter", mds.m_conn,mds.m_trans);
					LoadCommand.CommandType = CommandType.StoredProcedure;
					LoadCommand.Parameters.Add(new OdbcParameter("@FK_Command", OdbcType.Int,4));
					LoadCommand.Parameters.Add(new OdbcParameter("@FK_CommandParameter", OdbcType.Int,4));
					LoadCommand.Parameters["@FK_Command"].Value = FK_Command;
					LoadCommand.Parameters["@FK_CommandParameter"].Value = FK_CommandParameter;
					sqlda.SelectCommand = LoadCommand;
					sqlda.Fill(mds,"Command_CommandParameter");
					dr = new Command_CommandParameterDataRow(Rows.Find(findTheseVals));
				}
				return dr;
			}
		}
		public DataRowCollection LoadAll(OdbcConnection conn, OdbcTransaction trans)
		{
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT FK_Command,FK_CommandParameter,Description FROM Command_CommandParameter", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(this.DataSet,"Command_CommandParameter")==0 )
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
			Command_CommandParameterData.UpdateCommand_CommandParameter(ref ds,PK_Users,conn,trans);
		}
		public DataColumn cFK_Command
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
		public DataColumn cDescription
		{
			get
			{
				return Columns[2];
			}
		}
	}
} // namespace HAData.Common.Data
