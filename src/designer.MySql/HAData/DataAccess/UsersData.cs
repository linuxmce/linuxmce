namespace HAData.DataAccess {
	using System;
	using System.Data;
	using MySql;
	using MySql.Data;
	using MySql.Data.MySqlClient;
	using System.Collections;

	using HAData.Common;

	public class UsersData : MyDataSet {
		//
		// Users table constants
		//
		public const String USERS_TABLE = "Users";
		public const String PK_USERS_FIELD = "PK_Users";
		public const String USERNAME_FIELD = "UserName";
		public const String PASSWORD_FIELD = "Password";
		public const String HASMAILBOX_FIELD = "HasMailbox";
		public const String ACCESSGENERALMAILBOX_FIELD = "AccessGeneralMailbox";
		public const String EXTENSION_FIELD = "Extension";
		public const String FIRSTNAME_FIELD = "FirstName";
		public const String LASTNAME_FIELD = "LastName";
		public const String NICKNAME_FIELD = "Nickname";
		public const String EXTENSIONRINGTIMEOUT_FIELD = "ExtensionRingTimeout";
		public const String FORWARDEMAIL_FIELD = "ForwardEmail";
		public const String FK_LANGUAGE_FIELD = "FK_Language";
		public const String FK_INSTALLATION_MAIN_FIELD = "FK_Installation_Main";
		// table+field constants
		public const String PK_USERS_TABLE_FIELD = "Users.PK_Users";
		public const String USERNAME_TABLE_FIELD = "Users.UserName";
		public const String PASSWORD_TABLE_FIELD = "Users.Password";
		public const String HASMAILBOX_TABLE_FIELD = "Users.HasMailbox";
		public const String ACCESSGENERALMAILBOX_TABLE_FIELD = "Users.AccessGeneralMailbox";
		public const String EXTENSION_TABLE_FIELD = "Users.Extension";
		public const String FIRSTNAME_TABLE_FIELD = "Users.FirstName";
		public const String LASTNAME_TABLE_FIELD = "Users.LastName";
		public const String NICKNAME_TABLE_FIELD = "Users.Nickname";
		public const String EXTENSIONRINGTIMEOUT_TABLE_FIELD = "Users.ExtensionRingTimeout";
		public const String FORWARDEMAIL_TABLE_FIELD = "Users.ForwardEmail";
		public const String FK_LANGUAGE_TABLE_FIELD = "Users.FK_Language";
		public const String FK_INSTALLATION_MAIN_TABLE_FIELD = "Users.FK_Installation_Main";
		// DataSetCommand object
		protected MySqlDataAdapter m_DSCommand;

		// Stored procedure parameters
		protected const String PK_USERS_PARM = "@PK_Users";
		protected const String USERNAME_PARM = "@UserName";
		protected const String PASSWORD_PARM = "@Password";
		protected const String HASMAILBOX_PARM = "@HasMailbox";
		protected const String ACCESSGENERALMAILBOX_PARM = "@AccessGeneralMailbox";
		protected const String EXTENSION_PARM = "@Extension";
		protected const String FIRSTNAME_PARM = "@FirstName";
		protected const String LASTNAME_PARM = "@LastName";
		protected const String NICKNAME_PARM = "@Nickname";
		protected const String EXTENSIONRINGTIMEOUT_PARM = "@ExtensionRingTimeout";
		protected const String FORWARDEMAIL_PARM = "@ForwardEmail";
		protected const String FK_LANGUAGE_PARM = "@FK_Language";
		protected const String FK_INSTALLATION_MAIN_PARM = "@FK_Installation_Main";
		protected const String USERID_PARM = "@UserID";

		protected MySqlCommand m_LoadCommand;
		protected MySqlCommand m_InsertCommand;
		protected MySqlCommand m_UpdateCommand;
		protected MySqlCommand m_DeleteCommand;
		protected MySqlConnection m_Connection;
		protected MySqlTransaction m_Transaction;
		public DataTable Table { get { return Tables[0]; } }


		public UsersData() {  // marker:1
			//
			// Create the tables in the dataset
			//
			Tables.Add(BuildDataTables());
			m_Connection = HADataConfiguration.GetMySqlConnection();
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new MySqlDataAdapter();

			m_DSCommand.TableMappings.Add("Table", UsersData.USERS_TABLE);
		}

		public UsersData(MySqlConnection conn,MySqlTransaction trans) {

			m_Connection = conn;
			m_Transaction = trans;
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new MySqlDataAdapter();

			m_DSCommand.TableMappings.Add("Table", UsersData.USERS_TABLE);
		}

		private UsersData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context) {
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
			return (DataTable) BuildUsersTable();
		}
		public static UsersTable BuildUsersTable() {
			//
			// Create the Users table
			//
			UsersTable Table = new UsersTable();
			DataColumnCollection Columns = Table.Columns;
			DataColumn[] PKColumns = new DataColumn[1];

			DataColumn Column = Columns.Add(PK_USERS_FIELD, typeof(System.Int32));
			Column.AutoIncrement = true;
			Column.AutoIncrementSeed = -1;
			Column.AutoIncrementStep = -1;
			PKColumns[0] = Column;

			Column = Columns.Add(USERNAME_FIELD, typeof(System.String));
			Column.AllowDBNull = false;

			Column = Columns.Add(PASSWORD_FIELD, typeof(System.String));
			Column.AllowDBNull = false;

			Column = Columns.Add(HASMAILBOX_FIELD, typeof(System.Int16));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Column = Columns.Add(ACCESSGENERALMAILBOX_FIELD, typeof(System.Int16));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Columns.Add(EXTENSION_FIELD, typeof(System.Int32));
			Columns.Add(FIRSTNAME_FIELD, typeof(System.String));
			Columns.Add(LASTNAME_FIELD, typeof(System.String));
			Columns.Add(NICKNAME_FIELD, typeof(System.String));
			Column = Columns.Add(EXTENSIONRINGTIMEOUT_FIELD, typeof(System.Int32));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Columns.Add(FORWARDEMAIL_FIELD, typeof(System.String));
			Columns.Add(FK_LANGUAGE_FIELD, typeof(System.Int32));
			Columns.Add(FK_INSTALLATION_MAIN_FIELD, typeof(System.Int32));
			Table.PrimaryKey = PKColumns;

			return Table;
		}
		protected static void CreateParameters(MySqlParameterCollection Params, bool IsInsert) {
			Params.Add(new MySqlParameter(PK_USERS_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(USERNAME_PARM, MySqlDbType.VarChar, 25));
			Params.Add(new MySqlParameter(PASSWORD_PARM, MySqlDbType.VarChar, 32));
			Params.Add(new MySqlParameter(HASMAILBOX_PARM, MySqlDbType.Int16,2));
			Params.Add(new MySqlParameter(ACCESSGENERALMAILBOX_PARM, MySqlDbType.Int16,2));
			Params.Add(new MySqlParameter(EXTENSION_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(FIRSTNAME_PARM, MySqlDbType.VarChar, 20));
			Params.Add(new MySqlParameter(LASTNAME_PARM, MySqlDbType.VarChar, 30));
			Params.Add(new MySqlParameter(NICKNAME_PARM, MySqlDbType.VarChar, 15));
			Params.Add(new MySqlParameter(EXTENSIONRINGTIMEOUT_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(FORWARDEMAIL_PARM, MySqlDbType.VarChar, 50));
			Params.Add(new MySqlParameter(FK_LANGUAGE_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(FK_INSTALLATION_MAIN_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(USERID_PARM, MySqlDbType.Int32));

			// map the parameters to the data table

			Params[PK_USERS_PARM].SourceColumn = UsersData.PK_USERS_FIELD;
			if(IsInsert) {
				Params[PK_USERS_PARM].Direction = ParameterDirection.Output;
			}

			Params[USERNAME_PARM].SourceColumn = UsersData.USERNAME_FIELD;
			Params[PASSWORD_PARM].SourceColumn = UsersData.PASSWORD_FIELD;
			Params[HASMAILBOX_PARM].SourceColumn = UsersData.HASMAILBOX_FIELD;
			Params[ACCESSGENERALMAILBOX_PARM].SourceColumn = UsersData.ACCESSGENERALMAILBOX_FIELD;
			Params[EXTENSION_PARM].SourceColumn = UsersData.EXTENSION_FIELD;
			Params[FIRSTNAME_PARM].SourceColumn = UsersData.FIRSTNAME_FIELD;
			Params[LASTNAME_PARM].SourceColumn = UsersData.LASTNAME_FIELD;
			Params[NICKNAME_PARM].SourceColumn = UsersData.NICKNAME_FIELD;
			Params[EXTENSIONRINGTIMEOUT_PARM].SourceColumn = UsersData.EXTENSIONRINGTIMEOUT_FIELD;
			Params[FORWARDEMAIL_PARM].SourceColumn = UsersData.FORWARDEMAIL_FIELD;
			Params[FK_LANGUAGE_PARM].SourceColumn = UsersData.FK_LANGUAGE_FIELD;
			Params[FK_INSTALLATION_MAIN_PARM].SourceColumn = UsersData.FK_INSTALLATION_MAIN_FIELD;
		}

		protected static void CreateCommands(MySqlConnection Conn, MySqlTransaction Trans, ref MySqlCommand LoadCommand, ref MySqlCommand InsertCommand, ref MySqlCommand UpdateCommand, ref MySqlCommand DeleteCommand) {
			if(LoadCommand == null) {
				// Create the command since it's null
				LoadCommand = new MySqlCommand("sp_Select_Users", Conn);
				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new MySqlParameter(PK_USERS_PARM, MySqlDbType.Int32,4));
			}

			if(InsertCommand == null) {
				// Create the command since it's null
				InsertCommand = new MySqlCommand("sp_Insert_Users", Conn);
				InsertCommand.CommandType = CommandType.StoredProcedure;
				InsertCommand.Transaction = Trans;

				MySqlParameterCollection Params = InsertCommand.Parameters;

				CreateParameters(Params, true);

			}

			if(UpdateCommand == null) {
				// Create the command since it's null
				UpdateCommand = new MySqlCommand("sp_Update_Users", Conn);
				UpdateCommand.CommandType = CommandType.StoredProcedure;
				UpdateCommand.Transaction = Trans;

				MySqlParameterCollection Params = UpdateCommand.Parameters;

				CreateParameters(Params, false);

			}
			if (DeleteCommand == null)
			{
				DeleteCommand = new MySqlCommand("sp_Delete_Users", Conn);
				DeleteCommand.CommandType = CommandType.StoredProcedure;
				DeleteCommand.Transaction = Trans;

				DeleteCommand.Parameters.Add(PK_USERS_PARM, MySqlDbType.Int32,4, PK_USERS_FIELD);
				DeleteCommand.Parameters.Add(USERID_PARM, MySqlDbType.Int32);
			}
		}

		protected static void CreateCommands(MySqlDataAdapter odbcda,MySqlConnection Conn, MySqlTransaction Trans, ref MySqlCommand LoadCommand, ref MySqlCommand InsertCommand, ref MySqlCommand UpdateCommand, ref MySqlCommand DeleteCommand) {
				LoadCommand = new MySqlCommand("SELECT PK_Users,UserName,Password,HasMailbox,AccessGeneralMailbox,Extension,FirstName,LastName,Nickname,ExtensionRingTimeout,ForwardEmail,FK_Language,FK_Installation_Main FROM Users", Conn);
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new MySqlParameter(PK_USERS_PARM, MySqlDbType.Int32,4));

			odbcda.SelectCommand = LoadCommand;
			MySqlCommandBuilder odbcCB = new MySqlCommandBuilder(odbcda);
			odbcCB.RefreshSchema();
			DeleteCommand = odbcCB.GetDeleteCommand();
			InsertCommand = odbcCB.GetInsertCommand();
			UpdateCommand = odbcCB.GetUpdateCommand();
		}

		public UsersData LoadUsers(System.Int32 PK_Users)
		{
			m_DSCommand.SelectCommand = m_LoadCommand;
			m_DSCommand.SelectCommand.Parameters[PK_USERS_PARM].Value = PK_Users;

			m_DSCommand.Fill(this);
			return this;
		}

		public static DataRowCollection LoadUsersWithWhere(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans, string WhereClause) // marker:2
		{
			DataRowCollection dr;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["Users"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}
			
			DataSet dsTemp = new MyDataSet();
			dsTemp.Tables.Add(BuildDataTables());
			
			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();
			
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			string sSQL = "SELECT PK_Users, UserName, Password, HasMailbox, AccessGeneralMailbox, Extension, FirstName, LastName, Nickname, ExtensionRingTimeout, ForwardEmail, FK_Language, FK_Installation_Main FROM Users WHERE " + WhereClause;
			
			MySqlCommand LoadCommand = new MySqlCommand(sSQL,conn);
			
			if( trans!=null )
				LoadCommand.Transaction = trans;
			
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(dsTemp,"Users");
			
			dr=dsTemp.Tables["Users"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}

		public static DataRow LoadNoCacheUsers(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans, System.Int32 PK_Users)
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["Users"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}

			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand;
			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();

			LoadCommand = new MySqlCommand("sp_Select_Users", conn);

			LoadCommand.CommandType = CommandType.StoredProcedure;
			LoadCommand.Parameters.Add(new MySqlParameter(PK_USERS_PARM, MySqlDbType.Int32,4));
			LoadCommand.Parameters[PK_USERS_PARM].Value = PK_Users;
			if( trans!=null )
				LoadCommand.Transaction = trans;
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(ds,"Users");
				dr = ds.Tables["Users"].Rows.Find(PK_Users);
			return dr;
		}

		public static DataRow LoadUsers(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans, System.Int32 PK_Users)  // marker:3
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["Users"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
				else
				dr = dt.Rows.Find(PK_Users);
			}

			if( dr==null )
			{
				MySqlDataAdapter sqlda = new MySqlDataAdapter();
				MySqlCommand LoadCommand;
				if( conn==null )
					conn = HADataConfiguration.GetMySqlConnection();

				LoadCommand = new MySqlCommand("sp_Select_Users", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new MySqlParameter(PK_USERS_PARM, MySqlDbType.Int32,4));
				LoadCommand.Parameters[PK_USERS_PARM].Value = PK_Users;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(ds,"Users");
				dr = ds.Tables["Users"].Rows.Find(PK_Users);
			}
			return dr;
		}

		public UsersData LoadAll() {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new MySqlCommand("SELECT * FROM Users", m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this);
			return this;

		}

		public static DataRowCollection LoadAll(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans) {

			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand = new MySqlCommand("SELECT * FROM Users", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(ds,"Users")==0 )
				return null;
			else
				return ds.Tables["Users"].Rows;

		}

		public UsersData ExecuteQuery(String sSQL) {
			return ExecuteQuery(sSQL,USERS_TABLE);
		}
		public UsersData ExecuteQuery(String sSQL,String sTableName) {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new MySqlCommand(sSQL, m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this,sTableName);

			return this;
		}

		public static DataRowCollection ExecuteQuery(String sSQL,ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans) {
			return ExecuteQuery(sSQL,ref ds,conn,trans,"Users");
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

		public bool UpdateUsers(int CurUserID) {
			m_DSCommand.UpdateCommand = m_UpdateCommand;
			m_DSCommand.UpdateCommand.Parameters[USERID_PARM].Value = CurUserID;

			m_DSCommand.InsertCommand = m_InsertCommand;
			m_DSCommand.InsertCommand.Parameters[USERID_PARM].Value = CurUserID;

			if( m_DeleteCommand != null )
			{
				m_DSCommand.DeleteCommand = m_DeleteCommand;
				m_DSCommand.DeleteCommand.Parameters[USERID_PARM].Value = CurUserID;
			}

			m_DSCommand.Update(this, UsersData.USERS_TABLE);
			return true;
		}

		public static bool UpdateUsers(ref MyDataSet ds, int CurUserID)
		{
			MySqlConnection OdbcConn = HADataConfiguration.GetMySqlConnection();
			return UpdateUsers(ref ds,CurUserID,OdbcConn,null);
		}

		public static bool UpdateUsers(ref MyDataSet ds, int CurUserID,MySqlConnection OdbcConn,MySqlTransaction Trans)
		{
			DataTable dt = ds.Tables[USERS_TABLE];
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

	} // public class UsersData
	public class UsersDataRow
	{
		public DataRow dr = null;
		public UsersDataRow(DataRow d)
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
		public System.Int32 fPK_Users
		{
			get
			{
				return Convert.ToInt32(dr[0]);
			}
		}
		public System.String fUserName
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
		public System.String fPassword
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
		public System.Int16 fHasMailbox
		{
			get
			{
				return Convert.ToInt16(dr[3]);
			}
			set
			{
				dr[3]=value;
			}
		}
		public System.Int16 fAccessGeneralMailbox
		{
			get
			{
				return Convert.ToInt16(dr[4]);
			}
			set
			{
				dr[4]=value;
			}
		}
		public System.Int32 fExtension
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
		public bool fExtensionIsNull
		{
			get
			{
				return dr[5]==DBNull.Value;
			}
		}
		public void fExtensionSetNull()
		{
			dr[5]=DBNull.Value;
		}
		public System.String fFirstName
		{
			get
			{
				return Convert.ToString(dr[6]);
			}
			set
			{
				dr[6]=value;
			}
		}
		public bool fFirstNameIsNull
		{
			get
			{
				return dr[6]==DBNull.Value;
			}
		}
		public void fFirstNameSetNull()
		{
			dr[6]=DBNull.Value;
		}
		public System.String fLastName
		{
			get
			{
				return Convert.ToString(dr[7]);
			}
			set
			{
				dr[7]=value;
			}
		}
		public bool fLastNameIsNull
		{
			get
			{
				return dr[7]==DBNull.Value;
			}
		}
		public void fLastNameSetNull()
		{
			dr[7]=DBNull.Value;
		}
		public System.String fNickname
		{
			get
			{
				return Convert.ToString(dr[8]);
			}
			set
			{
				dr[8]=value;
			}
		}
		public bool fNicknameIsNull
		{
			get
			{
				return dr[8]==DBNull.Value;
			}
		}
		public void fNicknameSetNull()
		{
			dr[8]=DBNull.Value;
		}
		public System.Int32 fExtensionRingTimeout
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
		public System.String fForwardEmail
		{
			get
			{
				return Convert.ToString(dr[10]);
			}
			set
			{
				dr[10]=value;
			}
		}
		public bool fForwardEmailIsNull
		{
			get
			{
				return dr[10]==DBNull.Value;
			}
		}
		public void fForwardEmailSetNull()
		{
			dr[10]=DBNull.Value;
		}
		public System.Int32 fFK_Language
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
		public bool fFK_LanguageIsNull
		{
			get
			{
				return dr[11]==DBNull.Value;
			}
		}
		public void fFK_LanguageSetNull()
		{
			dr[11]=DBNull.Value;
		}
		public LanguageDataRow fFK_Language_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tLanguage[Convert.ToInt32(dr[11])];
			}
		}
		public System.Int32 fFK_Installation_Main
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
		public bool fFK_Installation_MainIsNull
		{
			get
			{
				return dr[12]==DBNull.Value;
			}
		}
		public void fFK_Installation_MainSetNull()
		{
			dr[12]=DBNull.Value;
		}
		public InstallationDataRow fFK_Installation_Main_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tInstallation[Convert.ToInt32(dr[12])];
			}
		}
	} // public class UsersDataRow
	public class UsersDataReader
	{
		public MySqlDataReader dr;
		bool bCache=false;
		int iRecord=-1,iNumRecords=-1;
		ArrayList al = null;

		public UsersDataReader(MySqlDataReader d)
		{
			dr=d;
		}
		public UsersDataReader(MySqlCommand cmd)
		{
			dr = cmd.ExecuteReader();
		}

		public UsersDataReader(MySqlCommand cmd,bool Cache)
		{
			dr = cmd.ExecuteReader();
			bCache=Cache;
			if( bCache )
				CacheAndClose();
		}

		public UsersDataReader(string sSQL)
		{
			MySqlConnection conn = HADataConfiguration.GetMySqlConnection();

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM Users WHERE " + sSQL;
			}

			MySqlCommand cmd = new MySqlCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public UsersDataReader(string sSQL,MySqlConnection conn)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetMySqlConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM Users WHERE " + sSQL;
			}

			MySqlCommand cmd = new MySqlCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public UsersDataReader(string sSQL,MySqlConnection conn,MySqlTransaction trans,bool Cache)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetMySqlConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM Users WHERE " + sSQL;
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
		public System.Int32 fPK_Users
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[0]);
				else
					return Convert.ToInt32(dr[0]);
			}
		}
		public System.String fUserName
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[1]);
				else
					return Convert.ToString(dr[1]);
			}
		}
		public System.String fPassword
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[2]);
				else
					return Convert.ToString(dr[2]);
			}
		}
		public System.Int16 fHasMailbox
		{
			get
			{
				if( bCache )
					return Convert.ToInt16(((object[]) al[iRecord])[3]);
				else
					return Convert.ToInt16(dr[3]);
			}
		}
		public System.Int16 fAccessGeneralMailbox
		{
			get
			{
				if( bCache )
					return Convert.ToInt16(((object[]) al[iRecord])[4]);
				else
					return Convert.ToInt16(dr[4]);
			}
		}
		public System.Int32 fExtension
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[5]);
				else
					return Convert.ToInt32(dr[5]);
			}
		}
		public bool fExtensionIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[5]==DBNull.Value;
				else
					return dr[5]==DBNull.Value;
			}
		}
		public System.String fFirstName
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[6]);
				else
					return Convert.ToString(dr[6]);
			}
		}
		public bool fFirstNameIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[6]==DBNull.Value;
				else
					return dr[6]==DBNull.Value;
			}
		}
		public System.String fLastName
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[7]);
				else
					return Convert.ToString(dr[7]);
			}
		}
		public bool fLastNameIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[7]==DBNull.Value;
				else
					return dr[7]==DBNull.Value;
			}
		}
		public System.String fNickname
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[8]);
				else
					return Convert.ToString(dr[8]);
			}
		}
		public bool fNicknameIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[8]==DBNull.Value;
				else
					return dr[8]==DBNull.Value;
			}
		}
		public System.Int32 fExtensionRingTimeout
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[9]);
				else
					return Convert.ToInt32(dr[9]);
			}
		}
		public System.String fForwardEmail
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[10]);
				else
					return Convert.ToString(dr[10]);
			}
		}
		public bool fForwardEmailIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[10]==DBNull.Value;
				else
					return dr[10]==DBNull.Value;
			}
		}
		public System.Int32 fFK_Language
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[11]);
				else
					return Convert.ToInt32(dr[11]);
			}
		}
		public bool fFK_LanguageIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[11]==DBNull.Value;
				else
					return dr[11]==DBNull.Value;
			}
		}
		public System.Int32 fFK_Installation_Main
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[12]);
				else
					return Convert.ToInt32(dr[12]);
			}
		}
		public bool fFK_Installation_MainIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[12]==DBNull.Value;
				else
					return dr[12]==DBNull.Value;
			}
		}
	} // public class UsersDataReader
	public class UsersTable : DataTable
	{
		public UsersTable() : base("Users") {}

		public UsersDataRow this [System.Int32 PK_Users]
		{
			get
			{
				UsersDataRow dr = new UsersDataRow(Rows.Find(PK_Users));
				return dr;
			}
		}
		public DataRowCollection LoadAll(MySqlConnection conn, MySqlTransaction trans)
		{
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand = new MySqlCommand("SELECT PK_Users,UserName,Password,HasMailbox,AccessGeneralMailbox,Extension,FirstName,LastName,Nickname,ExtensionRingTimeout,ForwardEmail,FK_Language,FK_Installation_Main FROM Users", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(this.DataSet,"Users")==0 )
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
			UsersData.UpdateUsers(ref ds,PK_Users,conn,trans);
		}
		public DataColumn cPK_Users
		{
			get
			{
				return Columns[0];
			}
		}
		public DataColumn cUserName
		{
			get
			{
				return Columns[1];
			}
		}
		public DataColumn cPassword
		{
			get
			{
				return Columns[2];
			}
		}
		public DataColumn cHasMailbox
		{
			get
			{
				return Columns[3];
			}
		}
		public DataColumn cAccessGeneralMailbox
		{
			get
			{
				return Columns[4];
			}
		}
		public DataColumn cExtension
		{
			get
			{
				return Columns[5];
			}
		}
		public DataColumn cFirstName
		{
			get
			{
				return Columns[6];
			}
		}
		public DataColumn cLastName
		{
			get
			{
				return Columns[7];
			}
		}
		public DataColumn cNickname
		{
			get
			{
				return Columns[8];
			}
		}
		public DataColumn cExtensionRingTimeout
		{
			get
			{
				return Columns[9];
			}
		}
		public DataColumn cForwardEmail
		{
			get
			{
				return Columns[10];
			}
		}
		public DataColumn cFK_Language
		{
			get
			{
				return Columns[11];
			}
		}
		public DataColumn cFK_Installation_Main
		{
			get
			{
				return Columns[12];
			}
		}
	}
} // namespace HAData.Common.Data
