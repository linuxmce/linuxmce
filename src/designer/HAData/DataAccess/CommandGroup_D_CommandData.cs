namespace HAData.DataAccess {
	using System;
	using System.Data;
	using Microsoft.Data.Odbc;
	using System.Collections;

	using HAData.Common;

	public class CommandGroup_D_CommandData : MyDataSet {
		//
		// CommandGroup_D_Command table constants
		//
		public const String COMMANDGROUP_D_COMMAND_TABLE = "CommandGroup_D_Command";
		public const String PK_COMMANDGROUP_D_COMMAND_FIELD = "PK_CommandGroup_D_Command";
		public const String FK_COMMANDGROUP_D_FIELD = "FK_CommandGroup_D";
		public const String FK_COMMAND_FIELD = "FK_Command";
		public const String FK_DEVICETEMPLATE_FIELD = "FK_DeviceTemplate";
		public const String FK_DEVICECATEGORY_FIELD = "FK_DeviceCategory";
		public const String BROADCASTLEVEL_FIELD = "BroadcastLevel";
		public const String RELATIVETOSENDER_FIELD = "RelativeToSender";
		public const String ORDERNUM_FIELD = "OrderNum";
		// table+field constants
		public const String PK_COMMANDGROUP_D_COMMAND_TABLE_FIELD = "CommandGroup_D_Command.PK_CommandGroup_D_Command";
		public const String FK_COMMANDGROUP_D_TABLE_FIELD = "CommandGroup_D_Command.FK_CommandGroup_D";
		public const String FK_COMMAND_TABLE_FIELD = "CommandGroup_D_Command.FK_Command";
		public const String FK_DEVICETEMPLATE_TABLE_FIELD = "CommandGroup_D_Command.FK_DeviceTemplate";
		public const String FK_DEVICECATEGORY_TABLE_FIELD = "CommandGroup_D_Command.FK_DeviceCategory";
		public const String BROADCASTLEVEL_TABLE_FIELD = "CommandGroup_D_Command.BroadcastLevel";
		public const String RELATIVETOSENDER_TABLE_FIELD = "CommandGroup_D_Command.RelativeToSender";
		public const String ORDERNUM_TABLE_FIELD = "CommandGroup_D_Command.OrderNum";
		// DataSetCommand object
		protected OdbcDataAdapter m_DSCommand;

		// Stored procedure parameters
		protected const String PK_COMMANDGROUP_D_COMMAND_PARM = "@PK_CommandGroup_D_Command";
		protected const String FK_COMMANDGROUP_D_PARM = "@FK_CommandGroup_D";
		protected const String FK_COMMAND_PARM = "@FK_Command";
		protected const String FK_DEVICETEMPLATE_PARM = "@FK_DeviceTemplate";
		protected const String FK_DEVICECATEGORY_PARM = "@FK_DeviceCategory";
		protected const String BROADCASTLEVEL_PARM = "@BroadcastLevel";
		protected const String RELATIVETOSENDER_PARM = "@RelativeToSender";
		protected const String ORDERNUM_PARM = "@OrderNum";
		protected const String USERID_PARM = "@UserID";

		protected OdbcCommand m_LoadCommand;
		protected OdbcCommand m_InsertCommand;
		protected OdbcCommand m_UpdateCommand;
		protected OdbcCommand m_DeleteCommand;
		protected OdbcConnection m_Connection;
		protected OdbcTransaction m_Transaction;
		public DataTable Table { get { return Tables[0]; } }


		public CommandGroup_D_CommandData() {  // marker:1
			//
			// Create the tables in the dataset
			//
			Tables.Add(BuildDataTables());
			m_Connection = HADataConfiguration.GetOdbcConnection();
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

			m_DSCommand.TableMappings.Add("Table", CommandGroup_D_CommandData.COMMANDGROUP_D_COMMAND_TABLE);
		}

		public CommandGroup_D_CommandData(OdbcConnection conn,OdbcTransaction trans) {

			m_Connection = conn;
			m_Transaction = trans;
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

			m_DSCommand.TableMappings.Add("Table", CommandGroup_D_CommandData.COMMANDGROUP_D_COMMAND_TABLE);
		}

		private CommandGroup_D_CommandData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context) {
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
			return (DataTable) BuildCommandGroup_D_CommandTable();
		}
		public static CommandGroup_D_CommandTable BuildCommandGroup_D_CommandTable() {
			//
			// Create the CommandGroup_D_Command table
			//
			CommandGroup_D_CommandTable Table = new CommandGroup_D_CommandTable();
			DataColumnCollection Columns = Table.Columns;
			DataColumn[] PKColumns = new DataColumn[1];

			DataColumn Column = Columns.Add(PK_COMMANDGROUP_D_COMMAND_FIELD, typeof(System.Int32));
			Column.AutoIncrement = true;
			Column.AutoIncrementSeed = -1;
			Column.AutoIncrementStep = -1;
			PKColumns[0] = Column;

			Column = Columns.Add(FK_COMMANDGROUP_D_FIELD, typeof(System.Int32));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Column = Columns.Add(FK_COMMAND_FIELD, typeof(System.Int32));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Columns.Add(FK_DEVICETEMPLATE_FIELD, typeof(System.Int32));
			Columns.Add(FK_DEVICECATEGORY_FIELD, typeof(System.Int32));
			Columns.Add(BROADCASTLEVEL_FIELD, typeof(System.Int32));
			Columns.Add(RELATIVETOSENDER_FIELD, typeof(System.Boolean));
			Columns.Add(ORDERNUM_FIELD, typeof(System.Int32));
			Table.PrimaryKey = PKColumns;

			return Table;
		}
		protected static void CreateParameters(OdbcParameterCollection Params, bool IsInsert) {
			Params.Add(new OdbcParameter(PK_COMMANDGROUP_D_COMMAND_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_COMMANDGROUP_D_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_COMMAND_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_DEVICETEMPLATE_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_DEVICECATEGORY_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(BROADCASTLEVEL_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(RELATIVETOSENDER_PARM, OdbcType.Bit,1));
			Params.Add(new OdbcParameter(ORDERNUM_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(USERID_PARM, OdbcType.Int));

			// map the parameters to the data table

			Params[PK_COMMANDGROUP_D_COMMAND_PARM].SourceColumn = CommandGroup_D_CommandData.PK_COMMANDGROUP_D_COMMAND_FIELD;
			if(IsInsert) {
				Params[PK_COMMANDGROUP_D_COMMAND_PARM].Direction = ParameterDirection.Output;
			}

			Params[FK_COMMANDGROUP_D_PARM].SourceColumn = CommandGroup_D_CommandData.FK_COMMANDGROUP_D_FIELD;
			Params[FK_COMMAND_PARM].SourceColumn = CommandGroup_D_CommandData.FK_COMMAND_FIELD;
			Params[FK_DEVICETEMPLATE_PARM].SourceColumn = CommandGroup_D_CommandData.FK_DEVICETEMPLATE_FIELD;
			Params[FK_DEVICECATEGORY_PARM].SourceColumn = CommandGroup_D_CommandData.FK_DEVICECATEGORY_FIELD;
			Params[BROADCASTLEVEL_PARM].SourceColumn = CommandGroup_D_CommandData.BROADCASTLEVEL_FIELD;
			Params[RELATIVETOSENDER_PARM].SourceColumn = CommandGroup_D_CommandData.RELATIVETOSENDER_FIELD;
			Params[ORDERNUM_PARM].SourceColumn = CommandGroup_D_CommandData.ORDERNUM_FIELD;
		}

		protected static void CreateCommands(OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
			if(LoadCommand == null) {
				// Create the command since it's null
				LoadCommand = new OdbcCommand("sp_Select_CommandGroup_D_Command", Conn);
				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(PK_COMMANDGROUP_D_COMMAND_PARM, OdbcType.Int,4));
			}

			if(InsertCommand == null) {
				// Create the command since it's null
				InsertCommand = new OdbcCommand("sp_Insert_CommandGroup_D_Command", Conn);
				InsertCommand.CommandType = CommandType.StoredProcedure;
				InsertCommand.Transaction = Trans;

				OdbcParameterCollection Params = InsertCommand.Parameters;

				CreateParameters(Params, true);

			}

			if(UpdateCommand == null) {
				// Create the command since it's null
				UpdateCommand = new OdbcCommand("sp_Update_CommandGroup_D_Command", Conn);
				UpdateCommand.CommandType = CommandType.StoredProcedure;
				UpdateCommand.Transaction = Trans;

				OdbcParameterCollection Params = UpdateCommand.Parameters;

				CreateParameters(Params, false);

			}
			if (DeleteCommand == null)
			{
				DeleteCommand = new OdbcCommand("sp_Delete_CommandGroup_D_Command", Conn);
				DeleteCommand.CommandType = CommandType.StoredProcedure;
				DeleteCommand.Transaction = Trans;

				DeleteCommand.Parameters.Add(PK_COMMANDGROUP_D_COMMAND_PARM, OdbcType.Int,4, PK_COMMANDGROUP_D_COMMAND_FIELD);
				DeleteCommand.Parameters.Add(USERID_PARM, OdbcType.Int);
			}
		}

		protected static void CreateCommands(OdbcDataAdapter odbcda,OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
				LoadCommand = new OdbcCommand("SELECT PK_CommandGroup_D_Command,FK_CommandGroup_D,FK_Command,FK_DeviceTemplate,FK_DeviceCategory,BroadcastLevel,RelativeToSender,OrderNum FROM CommandGroup_D_Command", Conn);
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(PK_COMMANDGROUP_D_COMMAND_PARM, OdbcType.Int,4));

			odbcda.SelectCommand = LoadCommand;
			OdbcCommandBuilder odbcCB = new OdbcCommandBuilder(odbcda);
			odbcCB.RefreshSchema();
			DeleteCommand = odbcCB.GetDeleteCommand();
			InsertCommand = odbcCB.GetInsertCommand();
			UpdateCommand = odbcCB.GetUpdateCommand();
		}

		public CommandGroup_D_CommandData LoadCommandGroup_D_Command(System.Int32 PK_CommandGroup_D_Command)
		{
			m_DSCommand.SelectCommand = m_LoadCommand;
			m_DSCommand.SelectCommand.Parameters[PK_COMMANDGROUP_D_COMMAND_PARM].Value = PK_CommandGroup_D_Command;

			m_DSCommand.Fill(this);
			return this;
		}

		public static DataRowCollection LoadCommandGroup_D_CommandWithWhere(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, string WhereClause) // marker:2
		{
			DataRowCollection dr;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["CommandGroup_D_Command"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}
			
			DataSet dsTemp = new MyDataSet();
			dsTemp.Tables.Add(BuildDataTables());
			
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			string sSQL = "SELECT PK_CommandGroup_D_Command, FK_CommandGroup_D, FK_Command, FK_DeviceTemplate, FK_DeviceCategory, BroadcastLevel, RelativeToSender, OrderNum FROM CommandGroup_D_Command WHERE " + WhereClause;
			
			OdbcCommand LoadCommand = new OdbcCommand(sSQL,conn);
			
			if( trans!=null )
				LoadCommand.Transaction = trans;
			
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(dsTemp,"CommandGroup_D_Command");
			
			dr=dsTemp.Tables["CommandGroup_D_Command"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}

		public static DataRow LoadNoCacheCommandGroup_D_Command(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 PK_CommandGroup_D_Command)
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["CommandGroup_D_Command"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}

			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand;
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();

			LoadCommand = new OdbcCommand("sp_Select_CommandGroup_D_Command", conn);

			LoadCommand.CommandType = CommandType.StoredProcedure;
			LoadCommand.Parameters.Add(new OdbcParameter(PK_COMMANDGROUP_D_COMMAND_PARM, OdbcType.Int,4));
			LoadCommand.Parameters[PK_COMMANDGROUP_D_COMMAND_PARM].Value = PK_CommandGroup_D_Command;
			if( trans!=null )
				LoadCommand.Transaction = trans;
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(ds,"CommandGroup_D_Command");
				dr = ds.Tables["CommandGroup_D_Command"].Rows.Find(PK_CommandGroup_D_Command);
			return dr;
		}

		public static DataRow LoadCommandGroup_D_Command(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 PK_CommandGroup_D_Command)  // marker:3
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["CommandGroup_D_Command"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
				else
				dr = dt.Rows.Find(PK_CommandGroup_D_Command);
			}

			if( dr==null )
			{
				OdbcDataAdapter sqlda = new OdbcDataAdapter();
				OdbcCommand LoadCommand;
				if( conn==null )
					conn = HADataConfiguration.GetOdbcConnection();

				LoadCommand = new OdbcCommand("sp_Select_CommandGroup_D_Command", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new OdbcParameter(PK_COMMANDGROUP_D_COMMAND_PARM, OdbcType.Int,4));
				LoadCommand.Parameters[PK_COMMANDGROUP_D_COMMAND_PARM].Value = PK_CommandGroup_D_Command;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(ds,"CommandGroup_D_Command");
				dr = ds.Tables["CommandGroup_D_Command"].Rows.Find(PK_CommandGroup_D_Command);
			}
			return dr;
		}

		public CommandGroup_D_CommandData LoadAll() {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new OdbcCommand("SELECT * FROM CommandGroup_D_Command", m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this);
			return this;

		}

		public static DataRowCollection LoadAll(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {

			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT * FROM CommandGroup_D_Command", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(ds,"CommandGroup_D_Command")==0 )
				return null;
			else
				return ds.Tables["CommandGroup_D_Command"].Rows;

		}

		public CommandGroup_D_CommandData ExecuteQuery(String sSQL) {
			return ExecuteQuery(sSQL,COMMANDGROUP_D_COMMAND_TABLE);
		}
		public CommandGroup_D_CommandData ExecuteQuery(String sSQL,String sTableName) {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new OdbcCommand(sSQL, m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this,sTableName);

			return this;
		}

		public static DataRowCollection ExecuteQuery(String sSQL,ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {
			return ExecuteQuery(sSQL,ref ds,conn,trans,"CommandGroup_D_Command");
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

		public bool UpdateCommandGroup_D_Command(int CurUserID) {
			m_DSCommand.UpdateCommand = m_UpdateCommand;
			m_DSCommand.UpdateCommand.Parameters[USERID_PARM].Value = CurUserID;

			m_DSCommand.InsertCommand = m_InsertCommand;
			m_DSCommand.InsertCommand.Parameters[USERID_PARM].Value = CurUserID;

			if( m_DeleteCommand != null )
			{
				m_DSCommand.DeleteCommand = m_DeleteCommand;
				m_DSCommand.DeleteCommand.Parameters[USERID_PARM].Value = CurUserID;
			}

			m_DSCommand.Update(this, CommandGroup_D_CommandData.COMMANDGROUP_D_COMMAND_TABLE);
			return true;
		}

		public static bool UpdateCommandGroup_D_Command(ref MyDataSet ds, int CurUserID)
		{
			OdbcConnection OdbcConn = HADataConfiguration.GetOdbcConnection();
			return UpdateCommandGroup_D_Command(ref ds,CurUserID,OdbcConn,null);
		}

		public static bool UpdateCommandGroup_D_Command(ref MyDataSet ds, int CurUserID,OdbcConnection OdbcConn,OdbcTransaction Trans)
		{
			DataTable dt = ds.Tables[COMMANDGROUP_D_COMMAND_TABLE];
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

	} // public class CommandGroup_D_CommandData
	public class CommandGroup_D_CommandDataRow
	{
		public DataRow dr = null;
		public CommandGroup_D_CommandDataRow(DataRow d)
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
		public System.Int32 fPK_CommandGroup_D_Command
		{
			get
			{
				return Convert.ToInt32(dr[0]);
			}
		}
		public System.Int32 fFK_CommandGroup_D
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
		public CommandGroup_DDataRow fFK_CommandGroup_D_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tCommandGroup_D[Convert.ToInt32(dr[1])];
			}
		}
		public System.Int32 fFK_Command
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
		public CommandDataRow fFK_Command_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tCommand[Convert.ToInt32(dr[2])];
			}
		}
		public System.Int32 fFK_DeviceTemplate
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
		public bool fFK_DeviceTemplateIsNull
		{
			get
			{
				return dr[3]==DBNull.Value;
			}
		}
		public void fFK_DeviceTemplateSetNull()
		{
			dr[3]=DBNull.Value;
		}
		public DeviceTemplateDataRow fFK_DeviceTemplate_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tDeviceTemplate[Convert.ToInt32(dr[3])];
			}
		}
		public System.Int32 fFK_DeviceCategory
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
		public bool fFK_DeviceCategoryIsNull
		{
			get
			{
				return dr[4]==DBNull.Value;
			}
		}
		public void fFK_DeviceCategorySetNull()
		{
			dr[4]=DBNull.Value;
		}
		public System.Int32 fBroadcastLevel
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
		public bool fBroadcastLevelIsNull
		{
			get
			{
				return dr[5]==DBNull.Value;
			}
		}
		public void fBroadcastLevelSetNull()
		{
			dr[5]=DBNull.Value;
		}
		public System.Boolean fRelativeToSender
		{
			get
			{
				return Convert.ToBoolean(dr[6]);
			}
			set
			{
				dr[6]=value;
			}
		}
		public bool fRelativeToSenderIsNull
		{
			get
			{
				return dr[6]==DBNull.Value;
			}
		}
		public void fRelativeToSenderSetNull()
		{
			dr[6]=DBNull.Value;
		}
		public System.Int32 fOrderNum
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
		public bool fOrderNumIsNull
		{
			get
			{
				return dr[7]==DBNull.Value;
			}
		}
		public void fOrderNumSetNull()
		{
			dr[7]=DBNull.Value;
		}
	} // public class CommandGroup_D_CommandDataRow
	public class CommandGroup_D_CommandDataReader
	{
		public OdbcDataReader dr;
		bool bCache=false;
		int iRecord=-1,iNumRecords=-1;
		ArrayList al = null;

		public CommandGroup_D_CommandDataReader(OdbcDataReader d)
		{
			dr=d;
		}
		public CommandGroup_D_CommandDataReader(OdbcCommand cmd)
		{
			dr = cmd.ExecuteReader();
		}

		public CommandGroup_D_CommandDataReader(OdbcCommand cmd,bool Cache)
		{
			dr = cmd.ExecuteReader();
			bCache=Cache;
			if( bCache )
				CacheAndClose();
		}

		public CommandGroup_D_CommandDataReader(string sSQL)
		{
			OdbcConnection conn = HADataConfiguration.GetOdbcConnection();

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM CommandGroup_D_Command WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public CommandGroup_D_CommandDataReader(string sSQL,OdbcConnection conn)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM CommandGroup_D_Command WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public CommandGroup_D_CommandDataReader(string sSQL,OdbcConnection conn,OdbcTransaction trans,bool Cache)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM CommandGroup_D_Command WHERE " + sSQL;
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
				object[] objs = new object[8];
				for(int i=0;i<8;i++)
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
		public System.Int32 fPK_CommandGroup_D_Command
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[0]);
				else
					return Convert.ToInt32(dr[0]);
			}
		}
		public System.Int32 fFK_CommandGroup_D
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[1]);
				else
					return Convert.ToInt32(dr[1]);
			}
		}
		public System.Int32 fFK_Command
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[2]);
				else
					return Convert.ToInt32(dr[2]);
			}
		}
		public System.Int32 fFK_DeviceTemplate
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[3]);
				else
					return Convert.ToInt32(dr[3]);
			}
		}
		public bool fFK_DeviceTemplateIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[3]==DBNull.Value;
				else
					return dr[3]==DBNull.Value;
			}
		}
		public System.Int32 fFK_DeviceCategory
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[4]);
				else
					return Convert.ToInt32(dr[4]);
			}
		}
		public bool fFK_DeviceCategoryIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[4]==DBNull.Value;
				else
					return dr[4]==DBNull.Value;
			}
		}
		public System.Int32 fBroadcastLevel
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[5]);
				else
					return Convert.ToInt32(dr[5]);
			}
		}
		public bool fBroadcastLevelIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[5]==DBNull.Value;
				else
					return dr[5]==DBNull.Value;
			}
		}
		public System.Boolean fRelativeToSender
		{
			get
			{
				if( bCache )
					return Convert.ToBoolean(((object[]) al[iRecord])[6]);
				else
					return Convert.ToBoolean(dr[6]);
			}
		}
		public bool fRelativeToSenderIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[6]==DBNull.Value;
				else
					return dr[6]==DBNull.Value;
			}
		}
		public System.Int32 fOrderNum
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[7]);
				else
					return Convert.ToInt32(dr[7]);
			}
		}
		public bool fOrderNumIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[7]==DBNull.Value;
				else
					return dr[7]==DBNull.Value;
			}
		}
	} // public class CommandGroup_D_CommandDataReader
	public class CommandGroup_D_CommandTable : DataTable
	{
		public CommandGroup_D_CommandTable() : base("CommandGroup_D_Command") {}

		public CommandGroup_D_CommandDataRow this [System.Int32 PK_CommandGroup_D_Command]
		{
			get
			{
				CommandGroup_D_CommandDataRow dr = new CommandGroup_D_CommandDataRow(Rows.Find(PK_CommandGroup_D_Command));
				if( !dr.bIsValid  && false /* can't do this with ODBC */  )
				{
					MyDataSet mds = (MyDataSet) DataSet;
					if( mds.m_conn==null )
						return dr;
					OdbcDataAdapter sqlda = new OdbcDataAdapter();
					OdbcCommand LoadCommand = new OdbcCommand("sp_Select_CommandGroup_D_Command", mds.m_conn,mds.m_trans);
					LoadCommand.CommandType = CommandType.StoredProcedure;
					LoadCommand.Parameters.Add(new OdbcParameter("@PK_CommandGroup_D_Command", OdbcType.Int,4));
					LoadCommand.Parameters["@PK_CommandGroup_D_Command"].Value = PK_CommandGroup_D_Command;
					sqlda.SelectCommand = LoadCommand;
					sqlda.Fill(mds,"CommandGroup_D_Command");
					dr = new CommandGroup_D_CommandDataRow(Rows.Find(PK_CommandGroup_D_Command));
				}
				return dr;
			}
		}
		public DataRowCollection LoadAll(OdbcConnection conn, OdbcTransaction trans)
		{
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT PK_CommandGroup_D_Command,FK_CommandGroup_D,FK_Command,FK_DeviceTemplate,FK_DeviceCategory,BroadcastLevel,RelativeToSender,OrderNum FROM CommandGroup_D_Command", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(this.DataSet,"CommandGroup_D_Command")==0 )
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
			CommandGroup_D_CommandData.UpdateCommandGroup_D_Command(ref ds,PK_Users,conn,trans);
		}
		public DataColumn cPK_CommandGroup_D_Command
		{
			get
			{
				return Columns[0];
			}
		}
		public DataColumn cFK_CommandGroup_D
		{
			get
			{
				return Columns[1];
			}
		}
		public DataColumn cFK_Command
		{
			get
			{
				return Columns[2];
			}
		}
		public DataColumn cFK_DeviceTemplate
		{
			get
			{
				return Columns[3];
			}
		}
		public DataColumn cFK_DeviceCategory
		{
			get
			{
				return Columns[4];
			}
		}
		public DataColumn cBroadcastLevel
		{
			get
			{
				return Columns[5];
			}
		}
		public DataColumn cRelativeToSender
		{
			get
			{
				return Columns[6];
			}
		}
		public DataColumn cOrderNum
		{
			get
			{
				return Columns[7];
			}
		}
	}
} // namespace HAData.Common.Data
