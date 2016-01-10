namespace HAData.DataAccess {
	using System;
	using System.Data;
	using MySql;
	using MySql.Data;
	using MySql.Data.MySqlClient;
	using System.Collections;

	using HAData.Common;

	public class LanguageData : MyDataSet {
		//
		// Language table constants
		//
		public const String LANGUAGE_TABLE = "Language";
		public const String PK_LANGUAGE_FIELD = "PK_Language";
		public const String DESCRIPTION_FIELD = "Description";
		public const String FK_LANGUAGE_TEXTPLACEMENT_FIELD = "FK_Language_TextPlacement";
		// table+field constants
		public const String PK_LANGUAGE_TABLE_FIELD = "Language.PK_Language";
		public const String DESCRIPTION_TABLE_FIELD = "Language.Description";
		public const String FK_LANGUAGE_TEXTPLACEMENT_TABLE_FIELD = "Language.FK_Language_TextPlacement";
		// DataSetCommand object
		protected MySqlDataAdapter m_DSCommand;

		// Stored procedure parameters
		protected const String PK_LANGUAGE_PARM = "@PK_Language";
		protected const String DESCRIPTION_PARM = "@Description";
		protected const String FK_LANGUAGE_TEXTPLACEMENT_PARM = "@FK_Language_TextPlacement";
		protected const String USERID_PARM = "@UserID";

		protected MySqlCommand m_LoadCommand;
		protected MySqlCommand m_InsertCommand;
		protected MySqlCommand m_UpdateCommand;
		protected MySqlCommand m_DeleteCommand;
		protected MySqlConnection m_Connection;
		protected MySqlTransaction m_Transaction;
		public DataTable Table { get { return Tables[0]; } }


		public LanguageData() {  // marker:1
			//
			// Create the tables in the dataset
			//
			Tables.Add(BuildDataTables());
			m_Connection = HADataConfiguration.GetMySqlConnection();
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new MySqlDataAdapter();

			m_DSCommand.TableMappings.Add("Table", LanguageData.LANGUAGE_TABLE);
		}

		public LanguageData(MySqlConnection conn,MySqlTransaction trans) {

			m_Connection = conn;
			m_Transaction = trans;
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new MySqlDataAdapter();

			m_DSCommand.TableMappings.Add("Table", LanguageData.LANGUAGE_TABLE);
		}

		private LanguageData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context) {
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
			return (DataTable) BuildLanguageTable();
		}
		public static LanguageTable BuildLanguageTable() {
			//
			// Create the Language table
			//
			LanguageTable Table = new LanguageTable();
			DataColumnCollection Columns = Table.Columns;
			DataColumn[] PKColumns = new DataColumn[1];

			DataColumn Column = Columns.Add(PK_LANGUAGE_FIELD, typeof(System.Int32));
			Column.AutoIncrement = true;
			Column.AutoIncrementSeed = -1;
			Column.AutoIncrementStep = -1;
			PKColumns[0] = Column;

			Column = Columns.Add(DESCRIPTION_FIELD, typeof(System.String));
			Column.AllowDBNull = false;

			Columns.Add(FK_LANGUAGE_TEXTPLACEMENT_FIELD, typeof(System.Int32));
			Table.PrimaryKey = PKColumns;

			return Table;
		}
		protected static void CreateParameters(MySqlParameterCollection Params, bool IsInsert) {
			Params.Add(new MySqlParameter(PK_LANGUAGE_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(DESCRIPTION_PARM, MySqlDbType.VarChar, 30));
			Params.Add(new MySqlParameter(FK_LANGUAGE_TEXTPLACEMENT_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(USERID_PARM, MySqlDbType.Int32));

			// map the parameters to the data table

			Params[PK_LANGUAGE_PARM].SourceColumn = LanguageData.PK_LANGUAGE_FIELD;
			if(IsInsert) {
				Params[PK_LANGUAGE_PARM].Direction = ParameterDirection.Output;
			}

			Params[DESCRIPTION_PARM].SourceColumn = LanguageData.DESCRIPTION_FIELD;
			Params[FK_LANGUAGE_TEXTPLACEMENT_PARM].SourceColumn = LanguageData.FK_LANGUAGE_TEXTPLACEMENT_FIELD;
		}

		protected static void CreateCommands(MySqlConnection Conn, MySqlTransaction Trans, ref MySqlCommand LoadCommand, ref MySqlCommand InsertCommand, ref MySqlCommand UpdateCommand, ref MySqlCommand DeleteCommand) {
			if(LoadCommand == null) {
				// Create the command since it's null
				LoadCommand = new MySqlCommand("sp_Select_Language", Conn);
				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new MySqlParameter(PK_LANGUAGE_PARM, MySqlDbType.Int32,4));
			}

			if(InsertCommand == null) {
				// Create the command since it's null
				InsertCommand = new MySqlCommand("sp_Insert_Language", Conn);
				InsertCommand.CommandType = CommandType.StoredProcedure;
				InsertCommand.Transaction = Trans;

				MySqlParameterCollection Params = InsertCommand.Parameters;

				CreateParameters(Params, true);

			}

			if(UpdateCommand == null) {
				// Create the command since it's null
				UpdateCommand = new MySqlCommand("sp_Update_Language", Conn);
				UpdateCommand.CommandType = CommandType.StoredProcedure;
				UpdateCommand.Transaction = Trans;

				MySqlParameterCollection Params = UpdateCommand.Parameters;

				CreateParameters(Params, false);

			}
			if (DeleteCommand == null)
			{
				DeleteCommand = new MySqlCommand("sp_Delete_Language", Conn);
				DeleteCommand.CommandType = CommandType.StoredProcedure;
				DeleteCommand.Transaction = Trans;

				DeleteCommand.Parameters.Add(PK_LANGUAGE_PARM, MySqlDbType.Int32,4, PK_LANGUAGE_FIELD);
				DeleteCommand.Parameters.Add(USERID_PARM, MySqlDbType.Int32);
			}
		}

		protected static void CreateCommands(MySqlDataAdapter odbcda,MySqlConnection Conn, MySqlTransaction Trans, ref MySqlCommand LoadCommand, ref MySqlCommand InsertCommand, ref MySqlCommand UpdateCommand, ref MySqlCommand DeleteCommand) {
				LoadCommand = new MySqlCommand("SELECT PK_Language,Description,FK_Language_TextPlacement FROM Language", Conn);
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new MySqlParameter(PK_LANGUAGE_PARM, MySqlDbType.Int32,4));

			odbcda.SelectCommand = LoadCommand;
			MySqlCommandBuilder odbcCB = new MySqlCommandBuilder(odbcda);
			odbcCB.RefreshSchema();
			DeleteCommand = odbcCB.GetDeleteCommand();
			InsertCommand = odbcCB.GetInsertCommand();
			UpdateCommand = odbcCB.GetUpdateCommand();
		}

		public LanguageData LoadLanguage(System.Int32 PK_Language)
		{
			m_DSCommand.SelectCommand = m_LoadCommand;
			m_DSCommand.SelectCommand.Parameters[PK_LANGUAGE_PARM].Value = PK_Language;

			m_DSCommand.Fill(this);
			return this;
		}

		public static DataRowCollection LoadLanguageWithWhere(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans, string WhereClause) // marker:2
		{
			DataRowCollection dr;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["Language"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}
			
			DataSet dsTemp = new MyDataSet();
			dsTemp.Tables.Add(BuildDataTables());
			
			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();
			
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			string sSQL = "SELECT PK_Language, Description, FK_Language_TextPlacement FROM Language WHERE " + WhereClause;
			
			MySqlCommand LoadCommand = new MySqlCommand(sSQL,conn);
			
			if( trans!=null )
				LoadCommand.Transaction = trans;
			
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(dsTemp,"Language");
			
			dr=dsTemp.Tables["Language"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}

		public static DataRow LoadNoCacheLanguage(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans, System.Int32 PK_Language)
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["Language"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}

			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand;
			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();

			LoadCommand = new MySqlCommand("sp_Select_Language", conn);

			LoadCommand.CommandType = CommandType.StoredProcedure;
			LoadCommand.Parameters.Add(new MySqlParameter(PK_LANGUAGE_PARM, MySqlDbType.Int32,4));
			LoadCommand.Parameters[PK_LANGUAGE_PARM].Value = PK_Language;
			if( trans!=null )
				LoadCommand.Transaction = trans;
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(ds,"Language");
				dr = ds.Tables["Language"].Rows.Find(PK_Language);
			return dr;
		}

		public static DataRow LoadLanguage(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans, System.Int32 PK_Language)  // marker:3
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["Language"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
				else
				dr = dt.Rows.Find(PK_Language);
			}

			if( dr==null )
			{
				MySqlDataAdapter sqlda = new MySqlDataAdapter();
				MySqlCommand LoadCommand;
				if( conn==null )
					conn = HADataConfiguration.GetMySqlConnection();

				LoadCommand = new MySqlCommand("sp_Select_Language", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new MySqlParameter(PK_LANGUAGE_PARM, MySqlDbType.Int32,4));
				LoadCommand.Parameters[PK_LANGUAGE_PARM].Value = PK_Language;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(ds,"Language");
				dr = ds.Tables["Language"].Rows.Find(PK_Language);
			}
			return dr;
		}

		public LanguageData LoadAll() {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new MySqlCommand("SELECT * FROM Language", m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this);
			return this;

		}

		public static DataRowCollection LoadAll(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans) {

			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand = new MySqlCommand("SELECT * FROM Language", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(ds,"Language")==0 )
				return null;
			else
				return ds.Tables["Language"].Rows;

		}

		public LanguageData ExecuteQuery(String sSQL) {
			return ExecuteQuery(sSQL,LANGUAGE_TABLE);
		}
		public LanguageData ExecuteQuery(String sSQL,String sTableName) {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new MySqlCommand(sSQL, m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this,sTableName);

			return this;
		}

		public static DataRowCollection ExecuteQuery(String sSQL,ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans) {
			return ExecuteQuery(sSQL,ref ds,conn,trans,"Language");
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

		public bool UpdateLanguage(int CurUserID) {
			m_DSCommand.UpdateCommand = m_UpdateCommand;
			m_DSCommand.UpdateCommand.Parameters[USERID_PARM].Value = CurUserID;

			m_DSCommand.InsertCommand = m_InsertCommand;
			m_DSCommand.InsertCommand.Parameters[USERID_PARM].Value = CurUserID;

			if( m_DeleteCommand != null )
			{
				m_DSCommand.DeleteCommand = m_DeleteCommand;
				m_DSCommand.DeleteCommand.Parameters[USERID_PARM].Value = CurUserID;
			}

			m_DSCommand.Update(this, LanguageData.LANGUAGE_TABLE);
			return true;
		}

		public static bool UpdateLanguage(ref MyDataSet ds, int CurUserID)
		{
			MySqlConnection OdbcConn = HADataConfiguration.GetMySqlConnection();
			return UpdateLanguage(ref ds,CurUserID,OdbcConn,null);
		}

		public static bool UpdateLanguage(ref MyDataSet ds, int CurUserID,MySqlConnection OdbcConn,MySqlTransaction Trans)
		{
			DataTable dt = ds.Tables[LANGUAGE_TABLE];
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

	} // public class LanguageData
	public class LanguageDataRow
	{
		public DataRow dr = null;
		public LanguageDataRow(DataRow d)
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
		public System.Int32 fPK_Language
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
		public System.Int32 fFK_Language_TextPlacement
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
		public bool fFK_Language_TextPlacementIsNull
		{
			get
			{
				return dr[2]==DBNull.Value;
			}
		}
		public void fFK_Language_TextPlacementSetNull()
		{
			dr[2]=DBNull.Value;
		}
		public LanguageDataRow fFK_Language_TextPlacement_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tLanguage[Convert.ToInt32(dr[2])];
			}
		}
	} // public class LanguageDataRow
	public class LanguageDataReader
	{
		public MySqlDataReader dr;
		bool bCache=false;
		int iRecord=-1,iNumRecords=-1;
		ArrayList al = null;

		public LanguageDataReader(MySqlDataReader d)
		{
			dr=d;
		}
		public LanguageDataReader(MySqlCommand cmd)
		{
			dr = cmd.ExecuteReader();
		}

		public LanguageDataReader(MySqlCommand cmd,bool Cache)
		{
			dr = cmd.ExecuteReader();
			bCache=Cache;
			if( bCache )
				CacheAndClose();
		}

		public LanguageDataReader(string sSQL)
		{
			MySqlConnection conn = HADataConfiguration.GetMySqlConnection();

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM Language WHERE " + sSQL;
			}

			MySqlCommand cmd = new MySqlCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public LanguageDataReader(string sSQL,MySqlConnection conn)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetMySqlConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM Language WHERE " + sSQL;
			}

			MySqlCommand cmd = new MySqlCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public LanguageDataReader(string sSQL,MySqlConnection conn,MySqlTransaction trans,bool Cache)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetMySqlConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM Language WHERE " + sSQL;
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
		public System.Int32 fPK_Language
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
		public System.Int32 fFK_Language_TextPlacement
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[2]);
				else
					return Convert.ToInt32(dr[2]);
			}
		}
		public bool fFK_Language_TextPlacementIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[2]==DBNull.Value;
				else
					return dr[2]==DBNull.Value;
			}
		}
	} // public class LanguageDataReader
	public class LanguageTable : DataTable
	{
		public LanguageTable() : base("Language") {}

		public LanguageDataRow this [System.Int32 PK_Language]
		{
			get
			{
				LanguageDataRow dr = new LanguageDataRow(Rows.Find(PK_Language));
				return dr;
			}
		}
		public DataRowCollection LoadAll(MySqlConnection conn, MySqlTransaction trans)
		{
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand = new MySqlCommand("SELECT PK_Language,Description,FK_Language_TextPlacement FROM Language", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(this.DataSet,"Language")==0 )
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
			LanguageData.UpdateLanguage(ref ds,PK_Users,conn,trans);
		}
		public DataColumn cPK_Language
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
		public DataColumn cFK_Language_TextPlacement
		{
			get
			{
				return Columns[2];
			}
		}
	}
} // namespace HAData.Common.Data
