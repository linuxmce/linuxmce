namespace HAData.DataAccess {
	using System;
	using System.Data;
	using Microsoft.Data.Odbc;
	using System.Collections;

	using HAData.Common;

	public class CommandParameterData : MyDataSet {
		//
		// CommandParameter table constants
		//
		public const String COMMANDPARAMETER_TABLE = "CommandParameter";
		public const String PK_COMMANDPARAMETER_FIELD = "PK_CommandParameter";
		public const String DESCRIPTION_FIELD = "Description";
		public const String DEFINE_FIELD = "Define";
		public const String FK_PARAMETERTYPE_FIELD = "FK_ParameterType";
		// table+field constants
		public const String PK_COMMANDPARAMETER_TABLE_FIELD = "CommandParameter.PK_CommandParameter";
		public const String DESCRIPTION_TABLE_FIELD = "CommandParameter.Description";
		public const String DEFINE_TABLE_FIELD = "CommandParameter.Define";
		public const String FK_PARAMETERTYPE_TABLE_FIELD = "CommandParameter.FK_ParameterType";
		public const int FILENAME_CONST = 13;
		// DataSetCommand object
		protected OdbcDataAdapter m_DSCommand;

		// Stored procedure parameters
		protected const String PK_COMMANDPARAMETER_PARM = "@PK_CommandParameter";
		protected const String DESCRIPTION_PARM = "@Description";
		protected const String DEFINE_PARM = "@Define";
		protected const String FK_PARAMETERTYPE_PARM = "@FK_ParameterType";
		protected const String USERID_PARM = "@UserID";

		protected OdbcCommand m_LoadCommand;
		protected OdbcCommand m_InsertCommand;
		protected OdbcCommand m_UpdateCommand;
		protected OdbcCommand m_DeleteCommand;
		protected OdbcConnection m_Connection;
		protected OdbcTransaction m_Transaction;
		public DataTable Table { get { return Tables[0]; } }


		public CommandParameterData() {  // marker:1
			//
			// Create the tables in the dataset
			//
			Tables.Add(BuildDataTables());
			m_Connection = HADataConfiguration.GetOdbcConnection();
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

			m_DSCommand.TableMappings.Add("Table", CommandParameterData.COMMANDPARAMETER_TABLE);
		}

		public CommandParameterData(OdbcConnection conn,OdbcTransaction trans) {

			m_Connection = conn;
			m_Transaction = trans;
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

			m_DSCommand.TableMappings.Add("Table", CommandParameterData.COMMANDPARAMETER_TABLE);
		}

		private CommandParameterData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context) {
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
			return (DataTable) BuildCommandParameterTable();
		}
		public static CommandParameterTable BuildCommandParameterTable() {
			//
			// Create the CommandParameter table
			//
			CommandParameterTable Table = new CommandParameterTable();
			DataColumnCollection Columns = Table.Columns;
			DataColumn[] PKColumns = new DataColumn[1];

			DataColumn Column = Columns.Add(PK_COMMANDPARAMETER_FIELD, typeof(System.Int32));
			Column.AutoIncrement = true;
			Column.AutoIncrementSeed = -1;
			Column.AutoIncrementStep = -1;
			PKColumns[0] = Column;

			Column = Columns.Add(DESCRIPTION_FIELD, typeof(System.String));
			Column.AllowDBNull = false;

			Column = Columns.Add(DEFINE_FIELD, typeof(System.String));
			Column.AllowDBNull = false;

			Column = Columns.Add(FK_PARAMETERTYPE_FIELD, typeof(System.Int32));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Table.PrimaryKey = PKColumns;

			return Table;
		}
		protected static void CreateParameters(OdbcParameterCollection Params, bool IsInsert) {
			Params.Add(new OdbcParameter(PK_COMMANDPARAMETER_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(DESCRIPTION_PARM, OdbcType.VarChar, 30));
			Params.Add(new OdbcParameter(DEFINE_PARM, OdbcType.VarChar, 30));
			Params.Add(new OdbcParameter(FK_PARAMETERTYPE_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(USERID_PARM, OdbcType.Int));

			// map the parameters to the data table

			Params[PK_COMMANDPARAMETER_PARM].SourceColumn = CommandParameterData.PK_COMMANDPARAMETER_FIELD;
			if(IsInsert) {
				Params[PK_COMMANDPARAMETER_PARM].Direction = ParameterDirection.Output;
			}

			Params[DESCRIPTION_PARM].SourceColumn = CommandParameterData.DESCRIPTION_FIELD;
			Params[DEFINE_PARM].SourceColumn = CommandParameterData.DEFINE_FIELD;
			Params[FK_PARAMETERTYPE_PARM].SourceColumn = CommandParameterData.FK_PARAMETERTYPE_FIELD;
		}

		protected static void CreateCommands(OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
			if(LoadCommand == null) {
				// Create the command since it's null
				LoadCommand = new OdbcCommand("sp_Select_CommandParameter", Conn);
				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(PK_COMMANDPARAMETER_PARM, OdbcType.Int,4));
			}

			if(InsertCommand == null) {
				// Create the command since it's null
				InsertCommand = new OdbcCommand("sp_Insert_CommandParameter", Conn);
				InsertCommand.CommandType = CommandType.StoredProcedure;
				InsertCommand.Transaction = Trans;

				OdbcParameterCollection Params = InsertCommand.Parameters;

				CreateParameters(Params, true);

			}

			if(UpdateCommand == null) {
				// Create the command since it's null
				UpdateCommand = new OdbcCommand("sp_Update_CommandParameter", Conn);
				UpdateCommand.CommandType = CommandType.StoredProcedure;
				UpdateCommand.Transaction = Trans;

				OdbcParameterCollection Params = UpdateCommand.Parameters;

				CreateParameters(Params, false);

			}
			if (DeleteCommand == null)
			{
				DeleteCommand = new OdbcCommand("sp_Delete_CommandParameter", Conn);
				DeleteCommand.CommandType = CommandType.StoredProcedure;
				DeleteCommand.Transaction = Trans;

				DeleteCommand.Parameters.Add(PK_COMMANDPARAMETER_PARM, OdbcType.Int,4, PK_COMMANDPARAMETER_FIELD);
				DeleteCommand.Parameters.Add(USERID_PARM, OdbcType.Int);
			}
		}

		protected static void CreateCommands(OdbcDataAdapter odbcda,OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
				LoadCommand = new OdbcCommand("SELECT PK_CommandParameter,Description,Define,FK_ParameterType FROM CommandParameter", Conn);
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(PK_COMMANDPARAMETER_PARM, OdbcType.Int,4));

			odbcda.SelectCommand = LoadCommand;
			OdbcCommandBuilder odbcCB = new OdbcCommandBuilder(odbcda);
			odbcCB.RefreshSchema();
			DeleteCommand = odbcCB.GetDeleteCommand();
			InsertCommand = odbcCB.GetInsertCommand();
			UpdateCommand = odbcCB.GetUpdateCommand();
		}

		public CommandParameterData LoadCommandParameter(System.Int32 PK_CommandParameter)
		{
			m_DSCommand.SelectCommand = m_LoadCommand;
			m_DSCommand.SelectCommand.Parameters[PK_COMMANDPARAMETER_PARM].Value = PK_CommandParameter;

			m_DSCommand.Fill(this);
			return this;
		}

		public static DataRowCollection LoadCommandParameterWithWhere(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, string WhereClause) // marker:2
		{
			DataRowCollection dr;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["CommandParameter"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}
			
			DataSet dsTemp = new MyDataSet();
			dsTemp.Tables.Add(BuildDataTables());
			
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			string sSQL = "SELECT PK_CommandParameter, Description, Define, FK_ParameterType FROM CommandParameter WHERE " + WhereClause;
			
			OdbcCommand LoadCommand = new OdbcCommand(sSQL,conn);
			
			if( trans!=null )
				LoadCommand.Transaction = trans;
			
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(dsTemp,"CommandParameter");
			
			dr=dsTemp.Tables["CommandParameter"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}

		public static DataRow LoadNoCacheCommandParameter(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 PK_CommandParameter)
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["CommandParameter"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}

			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand;
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();

			LoadCommand = new OdbcCommand("sp_Select_CommandParameter", conn);

			LoadCommand.CommandType = CommandType.StoredProcedure;
			LoadCommand.Parameters.Add(new OdbcParameter(PK_COMMANDPARAMETER_PARM, OdbcType.Int,4));
			LoadCommand.Parameters[PK_COMMANDPARAMETER_PARM].Value = PK_CommandParameter;
			if( trans!=null )
				LoadCommand.Transaction = trans;
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(ds,"CommandParameter");
				dr = ds.Tables["CommandParameter"].Rows.Find(PK_CommandParameter);
			return dr;
		}

		public static DataRow LoadCommandParameter(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 PK_CommandParameter)  // marker:3
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["CommandParameter"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
				else
				dr = dt.Rows.Find(PK_CommandParameter);
			}

			if( dr==null )
			{
				OdbcDataAdapter sqlda = new OdbcDataAdapter();
				OdbcCommand LoadCommand;
				if( conn==null )
					conn = HADataConfiguration.GetOdbcConnection();

				LoadCommand = new OdbcCommand("sp_Select_CommandParameter", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new OdbcParameter(PK_COMMANDPARAMETER_PARM, OdbcType.Int,4));
				LoadCommand.Parameters[PK_COMMANDPARAMETER_PARM].Value = PK_CommandParameter;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(ds,"CommandParameter");
				dr = ds.Tables["CommandParameter"].Rows.Find(PK_CommandParameter);
			}
			return dr;
		}

		public CommandParameterData LoadAll() {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new OdbcCommand("SELECT * FROM CommandParameter", m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this);
			return this;

		}

		public static DataRowCollection LoadAll(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {

			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT * FROM CommandParameter", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(ds,"CommandParameter")==0 )
				return null;
			else
				return ds.Tables["CommandParameter"].Rows;

		}

		public CommandParameterData ExecuteQuery(String sSQL) {
			return ExecuteQuery(sSQL,COMMANDPARAMETER_TABLE);
		}
		public CommandParameterData ExecuteQuery(String sSQL,String sTableName) {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new OdbcCommand(sSQL, m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this,sTableName);

			return this;
		}

		public static DataRowCollection ExecuteQuery(String sSQL,ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {
			return ExecuteQuery(sSQL,ref ds,conn,trans,"CommandParameter");
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

		public bool UpdateCommandParameter(int CurUserID) {
			m_DSCommand.UpdateCommand = m_UpdateCommand;
			m_DSCommand.UpdateCommand.Parameters[USERID_PARM].Value = CurUserID;

			m_DSCommand.InsertCommand = m_InsertCommand;
			m_DSCommand.InsertCommand.Parameters[USERID_PARM].Value = CurUserID;

			if( m_DeleteCommand != null )
			{
				m_DSCommand.DeleteCommand = m_DeleteCommand;
				m_DSCommand.DeleteCommand.Parameters[USERID_PARM].Value = CurUserID;
			}

			m_DSCommand.Update(this, CommandParameterData.COMMANDPARAMETER_TABLE);
			return true;
		}

		public static bool UpdateCommandParameter(ref MyDataSet ds, int CurUserID)
		{
			OdbcConnection OdbcConn = HADataConfiguration.GetOdbcConnection();
			return UpdateCommandParameter(ref ds,CurUserID,OdbcConn,null);
		}

		public static bool UpdateCommandParameter(ref MyDataSet ds, int CurUserID,OdbcConnection OdbcConn,OdbcTransaction Trans)
		{
			DataTable dt = ds.Tables[COMMANDPARAMETER_TABLE];
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

	} // public class CommandParameterData
	public class CommandParameterDataRow
	{
		public DataRow dr = null;
		public CommandParameterDataRow(DataRow d)
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
		public System.Int32 fPK_CommandParameter
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
		public System.Int32 fFK_ParameterType
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
		public ParameterTypeDataRow fFK_ParameterType_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tParameterType[Convert.ToInt32(dr[3])];
			}
		}
	} // public class CommandParameterDataRow
	public class CommandParameterDataReader
	{
		public OdbcDataReader dr;
		bool bCache=false;
		int iRecord=-1,iNumRecords=-1;
		ArrayList al = null;

		public CommandParameterDataReader(OdbcDataReader d)
		{
			dr=d;
		}
		public CommandParameterDataReader(OdbcCommand cmd)
		{
			dr = cmd.ExecuteReader();
		}

		public CommandParameterDataReader(OdbcCommand cmd,bool Cache)
		{
			dr = cmd.ExecuteReader();
			bCache=Cache;
			if( bCache )
				CacheAndClose();
		}

		public CommandParameterDataReader(string sSQL)
		{
			OdbcConnection conn = HADataConfiguration.GetOdbcConnection();

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM CommandParameter WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public CommandParameterDataReader(string sSQL,OdbcConnection conn)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM CommandParameter WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public CommandParameterDataReader(string sSQL,OdbcConnection conn,OdbcTransaction trans,bool Cache)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM CommandParameter WHERE " + sSQL;
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
				object[] objs = new object[4];
				for(int i=0;i<4;i++)
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
		public System.Int32 fPK_CommandParameter
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
		public System.Int32 fFK_ParameterType
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[3]);
				else
					return Convert.ToInt32(dr[3]);
			}
		}
	} // public class CommandParameterDataReader
	public class CommandParameterTable : DataTable
	{
		public CommandParameterTable() : base("CommandParameter") {}

		public CommandParameterDataRow this [System.Int32 PK_CommandParameter]
		{
			get
			{
				CommandParameterDataRow dr = new CommandParameterDataRow(Rows.Find(PK_CommandParameter));
				if( !dr.bIsValid  && false /* can't do this with ODBC */  )
				{
					MyDataSet mds = (MyDataSet) DataSet;
					if( mds.m_conn==null )
						return dr;
					OdbcDataAdapter sqlda = new OdbcDataAdapter();
					OdbcCommand LoadCommand = new OdbcCommand("sp_Select_CommandParameter", mds.m_conn,mds.m_trans);
					LoadCommand.CommandType = CommandType.StoredProcedure;
					LoadCommand.Parameters.Add(new OdbcParameter("@PK_CommandParameter", OdbcType.Int,4));
					LoadCommand.Parameters["@PK_CommandParameter"].Value = PK_CommandParameter;
					sqlda.SelectCommand = LoadCommand;
					sqlda.Fill(mds,"CommandParameter");
					dr = new CommandParameterDataRow(Rows.Find(PK_CommandParameter));
				}
				return dr;
			}
		}
		public DataRowCollection LoadAll(OdbcConnection conn, OdbcTransaction trans)
		{
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT PK_CommandParameter,Description,Define,FK_ParameterType FROM CommandParameter", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(this.DataSet,"CommandParameter")==0 )
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
			CommandParameterData.UpdateCommandParameter(ref ds,PK_Users,conn,trans);
		}
		public DataColumn cPK_CommandParameter
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
		public DataColumn cFK_ParameterType
		{
			get
			{
				return Columns[3];
			}
		}
	}
} // namespace HAData.Common.Data
