namespace HAData.DataAccess {
	using System;
	using System.Data;
	using Microsoft.Data.Odbc;
	using System.Collections;

	using HAData.Common;

	public class Screen_CommandParameterData : MyDataSet {
		//
		// Screen_CommandParameter table constants
		//
		public const String Screen_CommandParameter_TABLE = "Screen_CommandParameter";
		public const String FK_SCREEN_FIELD = "FK_Screen";
		public const String FK_COMMANDPARAMETER_FIELD = "FK_CommandParameter";
		public const String DESCRIPTION_FIELD = "Description";
		// table+field constants
		public const String FK_SCREEN_TABLE_FIELD = "Screen_CommandParameter.FK_Screen";
		public const String FK_COMMANDPARAMETER_TABLE_FIELD = "Screen_CommandParameter.FK_CommandParameter";
		public const String DESCRIPTION_TABLE_FIELD = "Screen_CommandParameter.Description";
		// DataSetCommand object
		protected OdbcDataAdapter m_DSCommand;

		// Stored procedure parameters
		protected const String FK_SCREEN_PARM = "@FK_Screen";
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


		public Screen_CommandParameterData() {  // marker:1
			//
			// Create the tables in the dataset
			//
			Tables.Add(BuildDataTables());
			m_Connection = HADataConfiguration.GetOdbcConnection();
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

			m_DSCommand.TableMappings.Add("Table", Screen_CommandParameterData.Screen_CommandParameter_TABLE);
		}

		public Screen_CommandParameterData(OdbcConnection conn,OdbcTransaction trans) {

			m_Connection = conn;
			m_Transaction = trans;
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

			m_DSCommand.TableMappings.Add("Table", Screen_CommandParameterData.Screen_CommandParameter_TABLE);
		}

		private Screen_CommandParameterData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context) {
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
			return (DataTable) BuildScreen_CommandParameterTable();
		}
		public static Screen_CommandParameterTable BuildScreen_CommandParameterTable() {
			//
			// Create the Screen_CommandParameter table
			//
			Screen_CommandParameterTable Table = new Screen_CommandParameterTable();
			DataColumnCollection Columns = Table.Columns;
			DataColumn[] PKColumns = new DataColumn[2];

			DataColumn Column = Columns.Add(FK_SCREEN_FIELD, typeof(System.Int32));
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
			Params.Add(new OdbcParameter(FK_SCREEN_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_COMMANDPARAMETER_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(DESCRIPTION_PARM, OdbcType.VarChar, 100));
			Params.Add(new OdbcParameter(USERID_PARM, OdbcType.Int));

			// map the parameters to the data table

			Params[FK_SCREEN_PARM].SourceColumn = Screen_CommandParameterData.FK_SCREEN_FIELD;
			Params[FK_COMMANDPARAMETER_PARM].SourceColumn = Screen_CommandParameterData.FK_COMMANDPARAMETER_FIELD;
			Params[DESCRIPTION_PARM].SourceColumn = Screen_CommandParameterData.DESCRIPTION_FIELD;
		}

		protected static void CreateCommands(OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
			if(LoadCommand == null) {
				// Create the command since it's null
				LoadCommand = new OdbcCommand("sp_Select_Screen_CommandParameter", Conn);
				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(FK_SCREEN_PARM, OdbcType.Int,4));
				LoadCommand.Parameters.Add(new OdbcParameter(FK_COMMANDPARAMETER_PARM, OdbcType.Int,4));
			}

			if(InsertCommand == null) {
				// Create the command since it's null
				InsertCommand = new OdbcCommand("sp_Insert_Screen_CommandParameter", Conn);
				InsertCommand.CommandType = CommandType.StoredProcedure;
				InsertCommand.Transaction = Trans;

				OdbcParameterCollection Params = InsertCommand.Parameters;

				CreateParameters(Params, true);

			}

			if(UpdateCommand == null) {
				// Create the command since it's null
				UpdateCommand = new OdbcCommand("sp_Update_Screen_CommandParameter", Conn);
				UpdateCommand.CommandType = CommandType.StoredProcedure;
				UpdateCommand.Transaction = Trans;

				OdbcParameterCollection Params = UpdateCommand.Parameters;

				CreateParameters(Params, false);

			}
			if (DeleteCommand == null)
			{
				DeleteCommand = new OdbcCommand("sp_Delete_Screen_CommandParameter", Conn);
				DeleteCommand.CommandType = CommandType.StoredProcedure;
				DeleteCommand.Transaction = Trans;

				DeleteCommand.Parameters.Add(FK_SCREEN_PARM, OdbcType.Int,4, FK_SCREEN_FIELD);
				DeleteCommand.Parameters.Add(FK_COMMANDPARAMETER_PARM, OdbcType.Int,4, FK_COMMANDPARAMETER_FIELD);
				DeleteCommand.Parameters.Add(USERID_PARM, OdbcType.Int);
			}
		}

		protected static void CreateCommands(OdbcDataAdapter odbcda,OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
				LoadCommand = new OdbcCommand("SELECT FK_Screen,FK_CommandParameter,Description FROM Screen_CommandParameter", Conn);
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(FK_SCREEN_PARM, OdbcType.Int,4));
				LoadCommand.Parameters.Add(new OdbcParameter(FK_COMMANDPARAMETER_PARM, OdbcType.Int,4));

			odbcda.SelectCommand = LoadCommand;
			OdbcCommandBuilder odbcCB = new OdbcCommandBuilder(odbcda);
			odbcCB.RefreshSchema();
			DeleteCommand = odbcCB.GetDeleteCommand();
			InsertCommand = odbcCB.GetInsertCommand();
			UpdateCommand = odbcCB.GetUpdateCommand();
		}

		public Screen_CommandParameterData LoadScreen_CommandParameter(System.Int32 FK_Screen, System.Int32 FK_CommandParameter)
		{
			m_DSCommand.SelectCommand = m_LoadCommand;
			m_DSCommand.SelectCommand.Parameters[FK_SCREEN_PARM].Value = FK_Screen;
			m_DSCommand.SelectCommand.Parameters[FK_COMMANDPARAMETER_PARM].Value = FK_CommandParameter;

			m_DSCommand.Fill(this);
			return this;
		}

		public static DataRowCollection LoadScreen_CommandParameterWithWhere(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, string WhereClause) // marker:2
		{
			DataRowCollection dr;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["Screen_CommandParameter"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}
			
			DataSet dsTemp = new MyDataSet();
			dsTemp.Tables.Add(BuildDataTables());
			
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			string sSQL = "SELECT FK_Screen, FK_CommandParameter, Description FROM Screen_CommandParameter WHERE " + WhereClause;
			
			OdbcCommand LoadCommand = new OdbcCommand(sSQL,conn);
			
			if( trans!=null )
				LoadCommand.Transaction = trans;
			
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(dsTemp,"Screen_CommandParameter");
			
			dr=dsTemp.Tables["Screen_CommandParameter"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}

		public static DataRow LoadNoCacheScreen_CommandParameter(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 FK_Screen, System.Int32 FK_CommandParameter)
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["Screen_CommandParameter"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}

			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand;
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();

			LoadCommand = new OdbcCommand("sp_Select_Screen_CommandParameter", conn);

			LoadCommand.CommandType = CommandType.StoredProcedure;
			LoadCommand.Parameters.Add(new OdbcParameter(FK_SCREEN_PARM, OdbcType.Int,4));
			LoadCommand.Parameters.Add(new OdbcParameter(FK_COMMANDPARAMETER_PARM, OdbcType.Int,4));
			LoadCommand.Parameters[FK_SCREEN_PARM].Value = FK_Screen;
			LoadCommand.Parameters[FK_COMMANDPARAMETER_PARM].Value = FK_CommandParameter;
			if( trans!=null )
				LoadCommand.Transaction = trans;
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(ds,"Screen_CommandParameter");
			object[]findTheseVals = new object[2];
			findTheseVals[0] = FK_Screen;
			findTheseVals[1] = FK_CommandParameter;
			dr = ds.Tables["Screen_CommandParameter"].Rows.Find(findTheseVals);
			return dr;
		}

		public static DataRow LoadScreen_CommandParameter(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 FK_Screen, System.Int32 FK_CommandParameter)  // marker:3
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
				findTheseVals[0] = FK_Screen;
				findTheseVals[1] = FK_CommandParameter;
				DataTable dt = ds.Tables["Screen_CommandParameter"];
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

				LoadCommand = new OdbcCommand("sp_Select_Screen_CommandParameter", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new OdbcParameter(FK_SCREEN_PARM, OdbcType.Int,4));
				LoadCommand.Parameters.Add(new OdbcParameter(FK_COMMANDPARAMETER_PARM, OdbcType.Int,4));
				LoadCommand.Parameters[FK_SCREEN_PARM].Value = FK_Screen;
				LoadCommand.Parameters[FK_COMMANDPARAMETER_PARM].Value = FK_CommandParameter;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(ds,"Screen_CommandParameter");
				object[]findTheseVals = new object[2];
				findTheseVals[0] = FK_Screen;
				findTheseVals[1] = FK_CommandParameter;
				dr = ds.Tables["Screen_CommandParameter"].Rows.Find(findTheseVals);
			}
			return dr;
		}

		public static DataRowCollection LoadScreen_CommandParameter_FirstPK(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans,System.Int32 FK_Screen)
		{
			DataRowCollection dr;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["Screen_CommandParameter"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}
			
			DataSet dsTemp = new MyDataSet();
			dsTemp.Tables.Add(BuildDataTables());
			
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
				OdbcCommand LoadCommand;

				LoadCommand = new OdbcCommand("sp_Select_Screen_CommandParameter_FirstPK", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new OdbcParameter(FK_SCREEN_PARM, OdbcType.Int,4));
				LoadCommand.Parameters[FK_SCREEN_PARM].Value = FK_Screen;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(dsTemp,"Screen_CommandParameter");
			
			dr=dsTemp.Tables["Screen_CommandParameter"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}
		public Screen_CommandParameterData LoadAll() {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new OdbcCommand("SELECT * FROM Screen_CommandParameter", m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this);
			return this;

		}

		public static DataRowCollection LoadAll(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {

			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT * FROM Screen_CommandParameter", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(ds,"Screen_CommandParameter")==0 )
				return null;
			else
				return ds.Tables["Screen_CommandParameter"].Rows;

		}

		public Screen_CommandParameterData ExecuteQuery(String sSQL) {
			return ExecuteQuery(sSQL,Screen_CommandParameter_TABLE);
		}
		public Screen_CommandParameterData ExecuteQuery(String sSQL,String sTableName) {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new OdbcCommand(sSQL, m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this,sTableName);

			return this;
		}

		public static DataRowCollection ExecuteQuery(String sSQL,ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {
			return ExecuteQuery(sSQL,ref ds,conn,trans,"Screen_CommandParameter");
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

		public bool UpdateScreen_CommandParameter(int CurUserID) {
			m_DSCommand.UpdateCommand = m_UpdateCommand;
			m_DSCommand.UpdateCommand.Parameters[USERID_PARM].Value = CurUserID;

			m_DSCommand.InsertCommand = m_InsertCommand;
			m_DSCommand.InsertCommand.Parameters[USERID_PARM].Value = CurUserID;

			if( m_DeleteCommand != null )
			{
				m_DSCommand.DeleteCommand = m_DeleteCommand;
				m_DSCommand.DeleteCommand.Parameters[USERID_PARM].Value = CurUserID;
			}

			m_DSCommand.Update(this, Screen_CommandParameterData.Screen_CommandParameter_TABLE);
			return true;
		}

		public static bool UpdateScreen_CommandParameter(ref MyDataSet ds, int CurUserID)
		{
			OdbcConnection OdbcConn = HADataConfiguration.GetOdbcConnection();
			return UpdateScreen_CommandParameter(ref ds,CurUserID,OdbcConn,null);
		}

		public static bool UpdateScreen_CommandParameter(ref MyDataSet ds, int CurUserID,OdbcConnection OdbcConn,OdbcTransaction Trans)
		{
			DataTable dt = ds.Tables[Screen_CommandParameter_TABLE];
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

	} // public class Screen_CommandParameterData
	public class Screen_CommandParameterDataRow
	{
		public DataRow dr = null;
		public Screen_CommandParameterDataRow(DataRow d)
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
		public System.Int32 fFK_Screen
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
	} // public class Screen_CommandParameterDataRow
	public class Screen_CommandParameterDataReader
	{
		public OdbcDataReader dr;
		bool bCache=false;
		int iRecord=-1,iNumRecords=-1;
		ArrayList al = null;

		public Screen_CommandParameterDataReader(OdbcDataReader d)
		{
			dr=d;
		}
		public Screen_CommandParameterDataReader(OdbcCommand cmd)
		{
			dr = cmd.ExecuteReader();
		}

		public Screen_CommandParameterDataReader(OdbcCommand cmd,bool Cache)
		{
			dr = cmd.ExecuteReader();
			bCache=Cache;
			if( bCache )
				CacheAndClose();
		}

		public Screen_CommandParameterDataReader(string sSQL)
		{
			OdbcConnection conn = HADataConfiguration.GetOdbcConnection();

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM Screen_CommandParameter WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public Screen_CommandParameterDataReader(string sSQL,OdbcConnection conn)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM Screen_CommandParameter WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public Screen_CommandParameterDataReader(string sSQL,OdbcConnection conn,OdbcTransaction trans,bool Cache)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM Screen_CommandParameter WHERE " + sSQL;
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
		public System.Int32 fFK_Screen
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
	} // public class Screen_CommandParameterDataReader
	public class Screen_CommandParameterTable : DataTable
	{
		public Screen_CommandParameterTable() : base("Screen_CommandParameter") {}

		public Screen_CommandParameterDataRow this [System.Int32 FK_Screen, System.Int32 FK_CommandParameter]
		{
			get
			{
				object[]findTheseVals = new object[2];
				findTheseVals[0] = FK_Screen;
				findTheseVals[1] = FK_CommandParameter;
				Screen_CommandParameterDataRow dr = new Screen_CommandParameterDataRow(Rows.Find(findTheseVals));
				if( !dr.bIsValid  && false /* can't do this with ODBC */  )
				{
					MyDataSet mds = (MyDataSet) DataSet;
					if( mds.m_conn==null )
						return dr;
					OdbcDataAdapter sqlda = new OdbcDataAdapter();
					OdbcCommand LoadCommand = new OdbcCommand("sp_Select_Screen_CommandParameter", mds.m_conn,mds.m_trans);
					LoadCommand.CommandType = CommandType.StoredProcedure;
					LoadCommand.Parameters.Add(new OdbcParameter("@FK_Screen", OdbcType.Int,4));
					LoadCommand.Parameters.Add(new OdbcParameter("@FK_CommandParameter", OdbcType.Int,4));
					LoadCommand.Parameters["@FK_Screen"].Value = FK_Screen;
					LoadCommand.Parameters["@FK_CommandParameter"].Value = FK_CommandParameter;
					sqlda.SelectCommand = LoadCommand;
					sqlda.Fill(mds,"Screen_CommandParameter");
					dr = new Screen_CommandParameterDataRow(Rows.Find(findTheseVals));
				}
				return dr;
			}
		}
		public DataRowCollection LoadAll(OdbcConnection conn, OdbcTransaction trans)
		{
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT FK_Screen,FK_CommandParameter,Description FROM Screen_CommandParameter", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(this.DataSet,"Screen_CommandParameter")==0 )
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
			Screen_CommandParameterData.UpdateScreen_CommandParameter(ref ds,PK_Users,conn,trans);
		}
		public DataColumn cFK_Screen
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
