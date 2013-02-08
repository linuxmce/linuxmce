namespace HAData.DataAccess {
	using System;
	using System.Data;
	using MySql;
	using MySql.Data;
	using MySql.Data.MySqlClient;
	using System.Collections;

	using HAData.Common;

	public class Text_LS_AltVersionsData : MyDataSet {
		//
		// Text_LS_AltVersions table constants
		//
		public const String TEXT_LS_ALTVERSIONS_TABLE = "Text_LS_AltVersions";
		public const String FK_TEXT_FIELD = "FK_Text";
		public const String FK_LANGUAGE_FIELD = "FK_Language";
		public const String VERSION_FIELD = "Version";
		public const String DESCRIPTION_FIELD = "Description";
		// table+field constants
		public const String FK_TEXT_TABLE_FIELD = "Text_LS_AltVersions.FK_Text";
		public const String FK_LANGUAGE_TABLE_FIELD = "Text_LS_AltVersions.FK_Language";
		public const String VERSION_TABLE_FIELD = "Text_LS_AltVersions.Version";
		public const String DESCRIPTION_TABLE_FIELD = "Text_LS_AltVersions.Description";
		// DataSetCommand object
		protected MySqlDataAdapter m_DSCommand;

		// Stored procedure parameters
		protected const String FK_TEXT_PARM = "@FK_Text";
		protected const String FK_LANGUAGE_PARM = "@FK_Language";
		protected const String VERSION_PARM = "@Version";
		protected const String DESCRIPTION_PARM = "@Description";
		protected const String USERID_PARM = "@UserID";

		protected MySqlCommand m_LoadCommand;
		protected MySqlCommand m_InsertCommand;
		protected MySqlCommand m_UpdateCommand;
		protected MySqlCommand m_DeleteCommand;
		protected MySqlConnection m_Connection;
		protected MySqlTransaction m_Transaction;
		public DataTable Table { get { return Tables[0]; } }


		public Text_LS_AltVersionsData() {  // marker:1
			//
			// Create the tables in the dataset
			//
			Tables.Add(BuildDataTables());
			m_Connection = HADataConfiguration.GetMySqlConnection();
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new MySqlDataAdapter();

			m_DSCommand.TableMappings.Add("Table", Text_LS_AltVersionsData.TEXT_LS_ALTVERSIONS_TABLE);
		}

		public Text_LS_AltVersionsData(MySqlConnection conn,MySqlTransaction trans) {

			m_Connection = conn;
			m_Transaction = trans;
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new MySqlDataAdapter();

			m_DSCommand.TableMappings.Add("Table", Text_LS_AltVersionsData.TEXT_LS_ALTVERSIONS_TABLE);
		}

		private Text_LS_AltVersionsData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context) {
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
			return (DataTable) BuildText_LS_AltVersionsTable();
		}
		public static Text_LS_AltVersionsTable BuildText_LS_AltVersionsTable() {
			//
			// Create the Text_LS_AltVersions table
			//
			Text_LS_AltVersionsTable Table = new Text_LS_AltVersionsTable();
			DataColumnCollection Columns = Table.Columns;
			DataColumn[] PKColumns = new DataColumn[3];

			DataColumn Column = Columns.Add(FK_TEXT_FIELD, typeof(System.Int32));
			Column.DefaultValue = 0;
			PKColumns[0] = Column;
			Column.DefaultValue = -1;

			Column = Columns.Add(FK_LANGUAGE_FIELD, typeof(System.Int32));
			Column.DefaultValue = 0;
			PKColumns[1] = Column;
			Column.DefaultValue = -1;

			Column = Columns.Add(VERSION_FIELD, typeof(System.Int32));
			Column.DefaultValue = 0;
			PKColumns[2] = Column;
			Column.DefaultValue = -1;

			Column = Columns.Add(DESCRIPTION_FIELD, typeof(System.String));
			Column.AllowDBNull = false;

			Table.PrimaryKey = PKColumns;

			return Table;
		}
		protected static void CreateParameters(MySqlParameterCollection Params, bool IsInsert) {
			Params.Add(new MySqlParameter(FK_TEXT_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(FK_LANGUAGE_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(VERSION_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(DESCRIPTION_PARM, MySqlDbType.Text));
			Params.Add(new MySqlParameter(USERID_PARM, MySqlDbType.Int32));

			// map the parameters to the data table

			Params[FK_TEXT_PARM].SourceColumn = Text_LS_AltVersionsData.FK_TEXT_FIELD;
			Params[FK_LANGUAGE_PARM].SourceColumn = Text_LS_AltVersionsData.FK_LANGUAGE_FIELD;
			Params[VERSION_PARM].SourceColumn = Text_LS_AltVersionsData.VERSION_FIELD;
			Params[DESCRIPTION_PARM].SourceColumn = Text_LS_AltVersionsData.DESCRIPTION_FIELD;
		}

		protected static void CreateCommands(MySqlConnection Conn, MySqlTransaction Trans, ref MySqlCommand LoadCommand, ref MySqlCommand InsertCommand, ref MySqlCommand UpdateCommand, ref MySqlCommand DeleteCommand) {
			if(LoadCommand == null) {
				// Create the command since it's null
				LoadCommand = new MySqlCommand("sp_Select_Text_LS_AltVersions", Conn);
				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new MySqlParameter(FK_TEXT_PARM, MySqlDbType.Int32,4));
				LoadCommand.Parameters.Add(new MySqlParameter(FK_LANGUAGE_PARM, MySqlDbType.Int32,4));
				LoadCommand.Parameters.Add(new MySqlParameter(VERSION_PARM, MySqlDbType.Int32,4));
			}

			if(InsertCommand == null) {
				// Create the command since it's null
				InsertCommand = new MySqlCommand("sp_Insert_Text_LS_AltVersions", Conn);
				InsertCommand.CommandType = CommandType.StoredProcedure;
				InsertCommand.Transaction = Trans;

				MySqlParameterCollection Params = InsertCommand.Parameters;

				CreateParameters(Params, true);

			}

			if(UpdateCommand == null) {
				// Create the command since it's null
				UpdateCommand = new MySqlCommand("sp_Update_Text_LS_AltVersions", Conn);
				UpdateCommand.CommandType = CommandType.StoredProcedure;
				UpdateCommand.Transaction = Trans;

				MySqlParameterCollection Params = UpdateCommand.Parameters;

				CreateParameters(Params, false);

			}
			if (DeleteCommand == null)
			{
				DeleteCommand = new MySqlCommand("sp_Delete_Text_LS_AltVersions", Conn);
				DeleteCommand.CommandType = CommandType.StoredProcedure;
				DeleteCommand.Transaction = Trans;

				DeleteCommand.Parameters.Add(FK_TEXT_PARM, MySqlDbType.Int32,4, FK_TEXT_FIELD);
				DeleteCommand.Parameters.Add(FK_LANGUAGE_PARM, MySqlDbType.Int32,4, FK_LANGUAGE_FIELD);
				DeleteCommand.Parameters.Add(VERSION_PARM, MySqlDbType.Int32,4, VERSION_FIELD);
				DeleteCommand.Parameters.Add(USERID_PARM, MySqlDbType.Int32);
			}
		}

		protected static void CreateCommands(MySqlDataAdapter odbcda,MySqlConnection Conn, MySqlTransaction Trans, ref MySqlCommand LoadCommand, ref MySqlCommand InsertCommand, ref MySqlCommand UpdateCommand, ref MySqlCommand DeleteCommand) {
				LoadCommand = new MySqlCommand("SELECT FK_Text,FK_Language,Version,Description FROM Text_LS_AltVersions", Conn);
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new MySqlParameter(FK_TEXT_PARM, MySqlDbType.Int32,4));
				LoadCommand.Parameters.Add(new MySqlParameter(FK_LANGUAGE_PARM, MySqlDbType.Int32,4));
				LoadCommand.Parameters.Add(new MySqlParameter(VERSION_PARM, MySqlDbType.Int32,4));

			odbcda.SelectCommand = LoadCommand;
			MySqlCommandBuilder odbcCB = new MySqlCommandBuilder(odbcda);
			odbcCB.RefreshSchema();
			DeleteCommand = odbcCB.GetDeleteCommand();
			InsertCommand = odbcCB.GetInsertCommand();
			UpdateCommand = odbcCB.GetUpdateCommand();
		}

		public Text_LS_AltVersionsData LoadText_LS_AltVersions(System.Int32 FK_Text, System.Int32 FK_Language, System.Int32 Version)
		{
			m_DSCommand.SelectCommand = m_LoadCommand;
			m_DSCommand.SelectCommand.Parameters[FK_TEXT_PARM].Value = FK_Text;
			m_DSCommand.SelectCommand.Parameters[FK_LANGUAGE_PARM].Value = FK_Language;
			m_DSCommand.SelectCommand.Parameters[VERSION_PARM].Value = Version;

			m_DSCommand.Fill(this);
			return this;
		}

		public static DataRowCollection LoadText_LS_AltVersionsWithWhere(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans, string WhereClause) // marker:2
		{
			DataRowCollection dr;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["Text_LS_AltVersions"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}
			
			DataSet dsTemp = new MyDataSet();
			dsTemp.Tables.Add(BuildDataTables());
			
			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();
			
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			string sSQL = "SELECT FK_Text, FK_Language, Version, Description FROM Text_LS_AltVersions WHERE " + WhereClause;
			
			MySqlCommand LoadCommand = new MySqlCommand(sSQL,conn);
			
			if( trans!=null )
				LoadCommand.Transaction = trans;
			
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(dsTemp,"Text_LS_AltVersions");
			
			dr=dsTemp.Tables["Text_LS_AltVersions"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}

		public static DataRow LoadNoCacheText_LS_AltVersions(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans, System.Int32 FK_Text, System.Int32 FK_Language, System.Int32 Version)
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["Text_LS_AltVersions"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}

			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand;
			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();

			LoadCommand = new MySqlCommand("sp_Select_Text_LS_AltVersions", conn);

			LoadCommand.CommandType = CommandType.StoredProcedure;
			LoadCommand.Parameters.Add(new MySqlParameter(FK_TEXT_PARM, MySqlDbType.Int32,4));
			LoadCommand.Parameters.Add(new MySqlParameter(FK_LANGUAGE_PARM, MySqlDbType.Int32,4));
			LoadCommand.Parameters.Add(new MySqlParameter(VERSION_PARM, MySqlDbType.Int32,4));
			LoadCommand.Parameters[FK_TEXT_PARM].Value = FK_Text;
			LoadCommand.Parameters[FK_LANGUAGE_PARM].Value = FK_Language;
			LoadCommand.Parameters[VERSION_PARM].Value = Version;
			if( trans!=null )
				LoadCommand.Transaction = trans;
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(ds,"Text_LS_AltVersions");
			object[]findTheseVals = new object[3];
			findTheseVals[0] = FK_Text;
			findTheseVals[1] = FK_Language;
			findTheseVals[2] = Version;
			dr = ds.Tables["Text_LS_AltVersions"].Rows.Find(findTheseVals);
			return dr;
		}

		public static DataRow LoadText_LS_AltVersions(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans, System.Int32 FK_Text, System.Int32 FK_Language, System.Int32 Version)  // marker:3
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				object[]findTheseVals = new object[3];
				findTheseVals[0] = FK_Text;
				findTheseVals[1] = FK_Language;
				findTheseVals[2] = Version;
				DataTable dt = ds.Tables["Text_LS_AltVersions"];
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

				LoadCommand = new MySqlCommand("sp_Select_Text_LS_AltVersions", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new MySqlParameter(FK_TEXT_PARM, MySqlDbType.Int32,4));
				LoadCommand.Parameters.Add(new MySqlParameter(FK_LANGUAGE_PARM, MySqlDbType.Int32,4));
				LoadCommand.Parameters.Add(new MySqlParameter(VERSION_PARM, MySqlDbType.Int32,4));
				LoadCommand.Parameters[FK_TEXT_PARM].Value = FK_Text;
				LoadCommand.Parameters[FK_LANGUAGE_PARM].Value = FK_Language;
				LoadCommand.Parameters[VERSION_PARM].Value = Version;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(ds,"Text_LS_AltVersions");
				object[]findTheseVals = new object[3];
				findTheseVals[0] = FK_Text;
				findTheseVals[1] = FK_Language;
				findTheseVals[2] = Version;
				dr = ds.Tables["Text_LS_AltVersions"].Rows.Find(findTheseVals);
			}
			return dr;
		}

		public static DataRowCollection LoadText_LS_AltVersions_FirstPK(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans,System.Int32 FK_Text)
		{
			DataRowCollection dr;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["Text_LS_AltVersions"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}
			
			DataSet dsTemp = new MyDataSet();
			dsTemp.Tables.Add(BuildDataTables());
			
			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();
			
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
				MySqlCommand LoadCommand;

				LoadCommand = new MySqlCommand("sp_Select_Text_LS_AltVersions_FirstPK", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new MySqlParameter(FK_TEXT_PARM, MySqlDbType.Int32,4));
				LoadCommand.Parameters[FK_TEXT_PARM].Value = FK_Text;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(dsTemp,"Text_LS_AltVersions");
			
			dr=dsTemp.Tables["Text_LS_AltVersions"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}
		public Text_LS_AltVersionsData LoadAll() {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new MySqlCommand("SELECT * FROM Text_LS_AltVersions", m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this);
			return this;

		}

		public static DataRowCollection LoadAll(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans) {

			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand = new MySqlCommand("SELECT * FROM Text_LS_AltVersions", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(ds,"Text_LS_AltVersions")==0 )
				return null;
			else
				return ds.Tables["Text_LS_AltVersions"].Rows;

		}

		public Text_LS_AltVersionsData ExecuteQuery(String sSQL) {
			return ExecuteQuery(sSQL,TEXT_LS_ALTVERSIONS_TABLE);
		}
		public Text_LS_AltVersionsData ExecuteQuery(String sSQL,String sTableName) {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new MySqlCommand(sSQL, m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this,sTableName);

			return this;
		}

		public static DataRowCollection ExecuteQuery(String sSQL,ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans) {
			return ExecuteQuery(sSQL,ref ds,conn,trans,"Text_LS_AltVersions");
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

		public bool UpdateText_LS_AltVersions(int CurUserID) {
			m_DSCommand.UpdateCommand = m_UpdateCommand;
			m_DSCommand.UpdateCommand.Parameters[USERID_PARM].Value = CurUserID;

			m_DSCommand.InsertCommand = m_InsertCommand;
			m_DSCommand.InsertCommand.Parameters[USERID_PARM].Value = CurUserID;

			if( m_DeleteCommand != null )
			{
				m_DSCommand.DeleteCommand = m_DeleteCommand;
				m_DSCommand.DeleteCommand.Parameters[USERID_PARM].Value = CurUserID;
			}

			m_DSCommand.Update(this, Text_LS_AltVersionsData.TEXT_LS_ALTVERSIONS_TABLE);
			return true;
		}

		public static bool UpdateText_LS_AltVersions(ref MyDataSet ds, int CurUserID)
		{
			MySqlConnection OdbcConn = HADataConfiguration.GetMySqlConnection();
			return UpdateText_LS_AltVersions(ref ds,CurUserID,OdbcConn,null);
		}

		public static bool UpdateText_LS_AltVersions(ref MyDataSet ds, int CurUserID,MySqlConnection OdbcConn,MySqlTransaction Trans)
		{
			DataTable dt = ds.Tables[TEXT_LS_ALTVERSIONS_TABLE];
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

	} // public class Text_LS_AltVersionsData
	public class Text_LS_AltVersionsDataRow
	{
		public DataRow dr = null;
		public Text_LS_AltVersionsDataRow(DataRow d)
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
		public System.Int32 fFK_Text
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
		public TextDataRow fFK_Text_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tText[Convert.ToInt32(dr[0])];
			}
		}
		public System.Int32 fFK_Language
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
		public LanguageDataRow fFK_Language_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tLanguage[Convert.ToInt32(dr[1])];
			}
		}
		public System.Int32 fVersion
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
		public System.String fDescription
		{
			get
			{
				return Convert.ToString(dr[3]);
			}
			set
			{
				dr[3]=value;
			}
		}
	} // public class Text_LS_AltVersionsDataRow
	public class Text_LS_AltVersionsDataReader
	{
		public MySqlDataReader dr;
		bool bCache=false;
		int iRecord=-1,iNumRecords=-1;
		ArrayList al = null;

		public Text_LS_AltVersionsDataReader(MySqlDataReader d)
		{
			dr=d;
		}
		public Text_LS_AltVersionsDataReader(MySqlCommand cmd)
		{
			dr = cmd.ExecuteReader();
		}

		public Text_LS_AltVersionsDataReader(MySqlCommand cmd,bool Cache)
		{
			dr = cmd.ExecuteReader();
			bCache=Cache;
			if( bCache )
				CacheAndClose();
		}

		public Text_LS_AltVersionsDataReader(string sSQL)
		{
			MySqlConnection conn = HADataConfiguration.GetMySqlConnection();

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM Text_LS_AltVersions WHERE " + sSQL;
			}

			MySqlCommand cmd = new MySqlCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public Text_LS_AltVersionsDataReader(string sSQL,MySqlConnection conn)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetMySqlConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM Text_LS_AltVersions WHERE " + sSQL;
			}

			MySqlCommand cmd = new MySqlCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public Text_LS_AltVersionsDataReader(string sSQL,MySqlConnection conn,MySqlTransaction trans,bool Cache)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetMySqlConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM Text_LS_AltVersions WHERE " + sSQL;
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
		public System.Int32 fFK_Text
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[0]);
				else
					return Convert.ToInt32(dr[0]);
			}
		}
		public System.Int32 fFK_Language
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[1]);
				else
					return Convert.ToInt32(dr[1]);
			}
		}
		public System.Int32 fVersion
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[2]);
				else
					return Convert.ToInt32(dr[2]);
			}
		}
		public System.String fDescription
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[3]);
				else
					return Convert.ToString(dr[3]);
			}
		}
	} // public class Text_LS_AltVersionsDataReader
	public class Text_LS_AltVersionsTable : DataTable
	{
		public Text_LS_AltVersionsTable() : base("Text_LS_AltVersions") {}

		public Text_LS_AltVersionsDataRow this [System.Int32 FK_Text, System.Int32 FK_Language, System.Int32 Version]
		{
			get
			{
				object[]findTheseVals = new object[3];
				findTheseVals[0] = FK_Text;
				findTheseVals[1] = FK_Language;
				findTheseVals[2] = Version;
				Text_LS_AltVersionsDataRow dr = new Text_LS_AltVersionsDataRow(Rows.Find(findTheseVals));
				return dr;
			}
		}
		public DataRowCollection LoadAll(MySqlConnection conn, MySqlTransaction trans)
		{
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand = new MySqlCommand("SELECT FK_Text,FK_Language,Version,Description FROM Text_LS_AltVersions", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(this.DataSet,"Text_LS_AltVersions")==0 )
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
			Text_LS_AltVersionsData.UpdateText_LS_AltVersions(ref ds,PK_Users,conn,trans);
		}
		public DataColumn cFK_Text
		{
			get
			{
				return Columns[0];
			}
		}
		public DataColumn cFK_Language
		{
			get
			{
				return Columns[1];
			}
		}
		public DataColumn cVersion
		{
			get
			{
				return Columns[2];
			}
		}
		public DataColumn cDescription
		{
			get
			{
				return Columns[3];
			}
		}
	}
} // namespace HAData.Common.Data
