namespace HAData.DataAccess {
	using System;
	using System.Data;
	using MySql;
	using MySql.Data;
	using MySql.Data.MySqlClient;
	using System.Collections;

	using HAData.Common;

	public class InstallationData : MyDataSet {
		//
		// Installation table constants
		//
		public const String INSTALLATION_TABLE = "Installation";
		public const String PK_INSTALLATION_FIELD = "PK_Installation";
		public const String DESCRIPTION_FIELD = "Description";
		public const String NAME_FIELD = "Name";
		public const String ADDRESS_FIELD = "Address";
		public const String CITY_FIELD = "City";
		public const String STATE_FIELD = "State";
		public const String ZIP_FIELD = "Zip";
		public const String ACTIVATIONCODE_FIELD = "ActivationCode";
		public const String LASTSTATUS_FIELD = "LastStatus";
		public const String LASTALIVE_FIELD = "LastAlive";
		public const String ISACTIVE_FIELD = "isActive";
		public const String FK_VERSION_FIELD = "FK_Version";
		public const String ISMONITORED_FIELD = "isMonitored";
		// table+field constants
		public const String PK_INSTALLATION_TABLE_FIELD = "Installation.PK_Installation";
		public const String DESCRIPTION_TABLE_FIELD = "Installation.Description";
		public const String NAME_TABLE_FIELD = "Installation.Name";
		public const String ADDRESS_TABLE_FIELD = "Installation.Address";
		public const String CITY_TABLE_FIELD = "Installation.City";
		public const String STATE_TABLE_FIELD = "Installation.State";
		public const String ZIP_TABLE_FIELD = "Installation.Zip";
		public const String ACTIVATIONCODE_TABLE_FIELD = "Installation.ActivationCode";
		public const String LASTSTATUS_TABLE_FIELD = "Installation.LastStatus";
		public const String LASTALIVE_TABLE_FIELD = "Installation.LastAlive";
		public const String ISACTIVE_TABLE_FIELD = "Installation.isActive";
		public const String FK_VERSION_TABLE_FIELD = "Installation.FK_Version";
		public const String ISMONITORED_TABLE_FIELD = "Installation.isMonitored";
		// DataSetCommand object
		protected MySqlDataAdapter m_DSCommand;

		// Stored procedure parameters
		protected const String PK_INSTALLATION_PARM = "@PK_Installation";
		protected const String DESCRIPTION_PARM = "@Description";
		protected const String NAME_PARM = "@Name";
		protected const String ADDRESS_PARM = "@Address";
		protected const String CITY_PARM = "@City";
		protected const String STATE_PARM = "@State";
		protected const String ZIP_PARM = "@Zip";
		protected const String ACTIVATIONCODE_PARM = "@ActivationCode";
		protected const String LASTSTATUS_PARM = "@LastStatus";
		protected const String LASTALIVE_PARM = "@LastAlive";
		protected const String ISACTIVE_PARM = "@isActive";
		protected const String FK_VERSION_PARM = "@FK_Version";
		protected const String ISMONITORED_PARM = "@isMonitored";
		protected const String USERID_PARM = "@UserID";

		protected MySqlCommand m_LoadCommand;
		protected MySqlCommand m_InsertCommand;
		protected MySqlCommand m_UpdateCommand;
		protected MySqlCommand m_DeleteCommand;
		protected MySqlConnection m_Connection;
		protected MySqlTransaction m_Transaction;
		public DataTable Table { get { return Tables[0]; } }


		public InstallationData() {  // marker:1
			//
			// Create the tables in the dataset
			//
			Tables.Add(BuildDataTables());
			m_Connection = HADataConfiguration.GetMySqlConnection();
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new MySqlDataAdapter();

			m_DSCommand.TableMappings.Add("Table", InstallationData.INSTALLATION_TABLE);
		}

		public InstallationData(MySqlConnection conn,MySqlTransaction trans) {

			m_Connection = conn;
			m_Transaction = trans;
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new MySqlDataAdapter();

			m_DSCommand.TableMappings.Add("Table", InstallationData.INSTALLATION_TABLE);
		}

		private InstallationData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context) {
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
			return (DataTable) BuildInstallationTable();
		}
		public static InstallationTable BuildInstallationTable() {
			//
			// Create the Installation table
			//
			InstallationTable Table = new InstallationTable();
			DataColumnCollection Columns = Table.Columns;
			DataColumn[] PKColumns = new DataColumn[1];

			DataColumn Column = Columns.Add(PK_INSTALLATION_FIELD, typeof(System.Int32));
			Column.AutoIncrement = true;
			Column.AutoIncrementSeed = -1;
			Column.AutoIncrementStep = -1;
			PKColumns[0] = Column;

			Column = Columns.Add(DESCRIPTION_FIELD, typeof(System.String));
			Column.AllowDBNull = false;

			Columns.Add(NAME_FIELD, typeof(System.String));
			Columns.Add(ADDRESS_FIELD, typeof(System.String));
			Columns.Add(CITY_FIELD, typeof(System.String));
			Columns.Add(STATE_FIELD, typeof(System.String));
			Columns.Add(ZIP_FIELD, typeof(System.String));
			Columns.Add(ACTIVATIONCODE_FIELD, typeof(System.String));
			Columns.Add(LASTSTATUS_FIELD, typeof(System.String));
			Columns.Add(LASTALIVE_FIELD, typeof(System.DateTime));
			Column = Columns.Add(ISACTIVE_FIELD, typeof(System.Int16));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Columns.Add(FK_VERSION_FIELD, typeof(System.Int32));
			Columns.Add(ISMONITORED_FIELD, typeof(System.Int16));
			Table.PrimaryKey = PKColumns;

			return Table;
		}
		protected static void CreateParameters(MySqlParameterCollection Params, bool IsInsert) {
			Params.Add(new MySqlParameter(PK_INSTALLATION_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(DESCRIPTION_PARM, MySqlDbType.VarChar, 20));
			Params.Add(new MySqlParameter(NAME_PARM, MySqlDbType.VarChar, 30));
			Params.Add(new MySqlParameter(ADDRESS_PARM, MySqlDbType.VarChar, 50));
			Params.Add(new MySqlParameter(CITY_PARM, MySqlDbType.VarChar, 20));
			Params.Add(new MySqlParameter(STATE_PARM, MySqlDbType.VarChar, 10));
			Params.Add(new MySqlParameter(ZIP_PARM, MySqlDbType.VarChar, 10));
			Params.Add(new MySqlParameter(ACTIVATIONCODE_PARM, MySqlDbType.VarChar, 50));
			Params.Add(new MySqlParameter(LASTSTATUS_PARM, MySqlDbType.Text));
			Params.Add(new MySqlParameter(LASTALIVE_PARM, MySqlDbType.DateTime,4));
			Params.Add(new MySqlParameter(ISACTIVE_PARM, MySqlDbType.Int16,2));
			Params.Add(new MySqlParameter(FK_VERSION_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(ISMONITORED_PARM, MySqlDbType.Int16,2));
			Params.Add(new MySqlParameter(USERID_PARM, MySqlDbType.Int32));

			// map the parameters to the data table

			Params[PK_INSTALLATION_PARM].SourceColumn = InstallationData.PK_INSTALLATION_FIELD;
			if(IsInsert) {
				Params[PK_INSTALLATION_PARM].Direction = ParameterDirection.Output;
			}

			Params[DESCRIPTION_PARM].SourceColumn = InstallationData.DESCRIPTION_FIELD;
			Params[NAME_PARM].SourceColumn = InstallationData.NAME_FIELD;
			Params[ADDRESS_PARM].SourceColumn = InstallationData.ADDRESS_FIELD;
			Params[CITY_PARM].SourceColumn = InstallationData.CITY_FIELD;
			Params[STATE_PARM].SourceColumn = InstallationData.STATE_FIELD;
			Params[ZIP_PARM].SourceColumn = InstallationData.ZIP_FIELD;
			Params[ACTIVATIONCODE_PARM].SourceColumn = InstallationData.ACTIVATIONCODE_FIELD;
			Params[LASTSTATUS_PARM].SourceColumn = InstallationData.LASTSTATUS_FIELD;
			Params[LASTALIVE_PARM].SourceColumn = InstallationData.LASTALIVE_FIELD;
			Params[ISACTIVE_PARM].SourceColumn = InstallationData.ISACTIVE_FIELD;
			Params[FK_VERSION_PARM].SourceColumn = InstallationData.FK_VERSION_FIELD;
			Params[ISMONITORED_PARM].SourceColumn = InstallationData.ISMONITORED_FIELD;
		}

		protected static void CreateCommands(MySqlConnection Conn, MySqlTransaction Trans, ref MySqlCommand LoadCommand, ref MySqlCommand InsertCommand, ref MySqlCommand UpdateCommand, ref MySqlCommand DeleteCommand) {
			if(LoadCommand == null) {
				// Create the command since it's null
				LoadCommand = new MySqlCommand("sp_Select_Installation", Conn);
				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new MySqlParameter(PK_INSTALLATION_PARM, MySqlDbType.Int32,4));
			}

			if(InsertCommand == null) {
				// Create the command since it's null
				InsertCommand = new MySqlCommand("sp_Insert_Installation", Conn);
				InsertCommand.CommandType = CommandType.StoredProcedure;
				InsertCommand.Transaction = Trans;

				MySqlParameterCollection Params = InsertCommand.Parameters;

				CreateParameters(Params, true);

			}

			if(UpdateCommand == null) {
				// Create the command since it's null
				UpdateCommand = new MySqlCommand("sp_Update_Installation", Conn);
				UpdateCommand.CommandType = CommandType.StoredProcedure;
				UpdateCommand.Transaction = Trans;

				MySqlParameterCollection Params = UpdateCommand.Parameters;

				CreateParameters(Params, false);

			}
			if (DeleteCommand == null)
			{
				DeleteCommand = new MySqlCommand("sp_Delete_Installation", Conn);
				DeleteCommand.CommandType = CommandType.StoredProcedure;
				DeleteCommand.Transaction = Trans;

				DeleteCommand.Parameters.Add(PK_INSTALLATION_PARM, MySqlDbType.Int32,4, PK_INSTALLATION_FIELD);
				DeleteCommand.Parameters.Add(USERID_PARM, MySqlDbType.Int32);
			}
		}

		protected static void CreateCommands(MySqlDataAdapter odbcda,MySqlConnection Conn, MySqlTransaction Trans, ref MySqlCommand LoadCommand, ref MySqlCommand InsertCommand, ref MySqlCommand UpdateCommand, ref MySqlCommand DeleteCommand) {
				LoadCommand = new MySqlCommand("SELECT PK_Installation,Description,Name,Address,City,State,Zip,ActivationCode,LastStatus,LastAlive,isActive,FK_Version,isMonitored FROM Installation", Conn);
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new MySqlParameter(PK_INSTALLATION_PARM, MySqlDbType.Int32,4));

			odbcda.SelectCommand = LoadCommand;
			MySqlCommandBuilder odbcCB = new MySqlCommandBuilder(odbcda);
			odbcCB.RefreshSchema();
			DeleteCommand = odbcCB.GetDeleteCommand();
			InsertCommand = odbcCB.GetInsertCommand();
			UpdateCommand = odbcCB.GetUpdateCommand();
		}

		public InstallationData LoadInstallation(System.Int32 PK_Installation)
		{
			m_DSCommand.SelectCommand = m_LoadCommand;
			m_DSCommand.SelectCommand.Parameters[PK_INSTALLATION_PARM].Value = PK_Installation;

			m_DSCommand.Fill(this);
			return this;
		}

		public static DataRowCollection LoadInstallationWithWhere(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans, string WhereClause) // marker:2
		{
			DataRowCollection dr;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["Installation"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}
			
			DataSet dsTemp = new MyDataSet();
			dsTemp.Tables.Add(BuildDataTables());
			
			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();
			
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			string sSQL = "SELECT PK_Installation, Description, Name, Address, City, State, Zip, ActivationCode, LastStatus, LastAlive, isActive, FK_Version, isMonitored FROM Installation WHERE " + WhereClause;
			
			MySqlCommand LoadCommand = new MySqlCommand(sSQL,conn);
			
			if( trans!=null )
				LoadCommand.Transaction = trans;
			
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(dsTemp,"Installation");
			
			dr=dsTemp.Tables["Installation"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}

		public static DataRow LoadNoCacheInstallation(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans, System.Int32 PK_Installation)
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["Installation"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}

			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand;
			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();

			LoadCommand = new MySqlCommand("sp_Select_Installation", conn);

			LoadCommand.CommandType = CommandType.StoredProcedure;
			LoadCommand.Parameters.Add(new MySqlParameter(PK_INSTALLATION_PARM, MySqlDbType.Int32,4));
			LoadCommand.Parameters[PK_INSTALLATION_PARM].Value = PK_Installation;
			if( trans!=null )
				LoadCommand.Transaction = trans;
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(ds,"Installation");
				dr = ds.Tables["Installation"].Rows.Find(PK_Installation);
			return dr;
		}

		public static DataRow LoadInstallation(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans, System.Int32 PK_Installation)  // marker:3
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["Installation"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
				else
				dr = dt.Rows.Find(PK_Installation);
			}

			if( dr==null )
			{
				MySqlDataAdapter sqlda = new MySqlDataAdapter();
				MySqlCommand LoadCommand;
				if( conn==null )
					conn = HADataConfiguration.GetMySqlConnection();

				LoadCommand = new MySqlCommand("sp_Select_Installation", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new MySqlParameter(PK_INSTALLATION_PARM, MySqlDbType.Int32,4));
				LoadCommand.Parameters[PK_INSTALLATION_PARM].Value = PK_Installation;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(ds,"Installation");
				dr = ds.Tables["Installation"].Rows.Find(PK_Installation);
			}
			return dr;
		}

		public InstallationData LoadAll() {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new MySqlCommand("SELECT * FROM Installation", m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this);
			return this;

		}

		public static DataRowCollection LoadAll(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans) {

			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand = new MySqlCommand("SELECT * FROM Installation", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(ds,"Installation")==0 )
				return null;
			else
				return ds.Tables["Installation"].Rows;

		}

		public InstallationData ExecuteQuery(String sSQL) {
			return ExecuteQuery(sSQL,INSTALLATION_TABLE);
		}
		public InstallationData ExecuteQuery(String sSQL,String sTableName) {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new MySqlCommand(sSQL, m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this,sTableName);

			return this;
		}

		public static DataRowCollection ExecuteQuery(String sSQL,ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans) {
			return ExecuteQuery(sSQL,ref ds,conn,trans,"Installation");
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

		public bool UpdateInstallation(int CurUserID) {
			m_DSCommand.UpdateCommand = m_UpdateCommand;
			m_DSCommand.UpdateCommand.Parameters[USERID_PARM].Value = CurUserID;

			m_DSCommand.InsertCommand = m_InsertCommand;
			m_DSCommand.InsertCommand.Parameters[USERID_PARM].Value = CurUserID;

			if( m_DeleteCommand != null )
			{
				m_DSCommand.DeleteCommand = m_DeleteCommand;
				m_DSCommand.DeleteCommand.Parameters[USERID_PARM].Value = CurUserID;
			}

			m_DSCommand.Update(this, InstallationData.INSTALLATION_TABLE);
			return true;
		}

		public static bool UpdateInstallation(ref MyDataSet ds, int CurUserID)
		{
			MySqlConnection OdbcConn = HADataConfiguration.GetMySqlConnection();
			return UpdateInstallation(ref ds,CurUserID,OdbcConn,null);
		}

		public static bool UpdateInstallation(ref MyDataSet ds, int CurUserID,MySqlConnection OdbcConn,MySqlTransaction Trans)
		{
			DataTable dt = ds.Tables[INSTALLATION_TABLE];
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

	} // public class InstallationData
	public class InstallationDataRow
	{
		public DataRow dr = null;
		public InstallationDataRow(DataRow d)
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
		public System.Int32 fPK_Installation
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
		public System.String fName
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
		public bool fNameIsNull
		{
			get
			{
				return dr[2]==DBNull.Value;
			}
		}
		public void fNameSetNull()
		{
			dr[2]=DBNull.Value;
		}
		public System.String fAddress
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
		public bool fAddressIsNull
		{
			get
			{
				return dr[3]==DBNull.Value;
			}
		}
		public void fAddressSetNull()
		{
			dr[3]=DBNull.Value;
		}
		public System.String fCity
		{
			get
			{
				return Convert.ToString(dr[4]);
			}
			set
			{
				dr[4]=value;
			}
		}
		public bool fCityIsNull
		{
			get
			{
				return dr[4]==DBNull.Value;
			}
		}
		public void fCitySetNull()
		{
			dr[4]=DBNull.Value;
		}
		public System.String fState
		{
			get
			{
				return Convert.ToString(dr[5]);
			}
			set
			{
				dr[5]=value;
			}
		}
		public bool fStateIsNull
		{
			get
			{
				return dr[5]==DBNull.Value;
			}
		}
		public void fStateSetNull()
		{
			dr[5]=DBNull.Value;
		}
		public System.String fZip
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
		public bool fZipIsNull
		{
			get
			{
				return dr[6]==DBNull.Value;
			}
		}
		public void fZipSetNull()
		{
			dr[6]=DBNull.Value;
		}
		public System.String fActivationCode
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
		public bool fActivationCodeIsNull
		{
			get
			{
				return dr[7]==DBNull.Value;
			}
		}
		public void fActivationCodeSetNull()
		{
			dr[7]=DBNull.Value;
		}
		public System.String fLastStatus
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
		public bool fLastStatusIsNull
		{
			get
			{
				return dr[8]==DBNull.Value;
			}
		}
		public void fLastStatusSetNull()
		{
			dr[8]=DBNull.Value;
		}
		public System.DateTime fLastAlive
		{
			get
			{
				return Convert.ToDateTime(dr[9]);
			}
			set
			{
				dr[9]=value;
			}
		}
		public bool fLastAliveIsNull
		{
			get
			{
				return dr[9]==DBNull.Value;
			}
		}
		public void fLastAliveSetNull()
		{
			dr[9]=DBNull.Value;
		}
		public System.Int16 fisActive
		{
			get
			{
				return Convert.ToInt16(dr[10]);
			}
			set
			{
				dr[10]=value;
			}
		}
		public System.Int32 fFK_Version
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
		public bool fFK_VersionIsNull
		{
			get
			{
				return dr[11]==DBNull.Value;
			}
		}
		public void fFK_VersionSetNull()
		{
			dr[11]=DBNull.Value;
		}
		public VersionDataRow fFK_Version_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tVersion[Convert.ToInt32(dr[11])];
			}
		}
		public System.Int16 fisMonitored
		{
			get
			{
				return Convert.ToInt16(dr[12]);
			}
			set
			{
				dr[12]=value;
			}
		}
		public bool fisMonitoredIsNull
		{
			get
			{
				return dr[12]==DBNull.Value;
			}
		}
		public void fisMonitoredSetNull()
		{
			dr[12]=DBNull.Value;
		}
	} // public class InstallationDataRow
	public class InstallationDataReader
	{
		public MySqlDataReader dr;
		bool bCache=false;
		int iRecord=-1,iNumRecords=-1;
		ArrayList al = null;

		public InstallationDataReader(MySqlDataReader d)
		{
			dr=d;
		}
		public InstallationDataReader(MySqlCommand cmd)
		{
			dr = cmd.ExecuteReader();
		}

		public InstallationDataReader(MySqlCommand cmd,bool Cache)
		{
			dr = cmd.ExecuteReader();
			bCache=Cache;
			if( bCache )
				CacheAndClose();
		}

		public InstallationDataReader(string sSQL)
		{
			MySqlConnection conn = HADataConfiguration.GetMySqlConnection();

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM Installation WHERE " + sSQL;
			}

			MySqlCommand cmd = new MySqlCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public InstallationDataReader(string sSQL,MySqlConnection conn)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetMySqlConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM Installation WHERE " + sSQL;
			}

			MySqlCommand cmd = new MySqlCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public InstallationDataReader(string sSQL,MySqlConnection conn,MySqlTransaction trans,bool Cache)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetMySqlConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM Installation WHERE " + sSQL;
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
		public System.Int32 fPK_Installation
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
		public System.String fName
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[2]);
				else
					return Convert.ToString(dr[2]);
			}
		}
		public bool fNameIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[2]==DBNull.Value;
				else
					return dr[2]==DBNull.Value;
			}
		}
		public System.String fAddress
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[3]);
				else
					return Convert.ToString(dr[3]);
			}
		}
		public bool fAddressIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[3]==DBNull.Value;
				else
					return dr[3]==DBNull.Value;
			}
		}
		public System.String fCity
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[4]);
				else
					return Convert.ToString(dr[4]);
			}
		}
		public bool fCityIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[4]==DBNull.Value;
				else
					return dr[4]==DBNull.Value;
			}
		}
		public System.String fState
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[5]);
				else
					return Convert.ToString(dr[5]);
			}
		}
		public bool fStateIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[5]==DBNull.Value;
				else
					return dr[5]==DBNull.Value;
			}
		}
		public System.String fZip
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[6]);
				else
					return Convert.ToString(dr[6]);
			}
		}
		public bool fZipIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[6]==DBNull.Value;
				else
					return dr[6]==DBNull.Value;
			}
		}
		public System.String fActivationCode
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[7]);
				else
					return Convert.ToString(dr[7]);
			}
		}
		public bool fActivationCodeIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[7]==DBNull.Value;
				else
					return dr[7]==DBNull.Value;
			}
		}
		public System.String fLastStatus
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[8]);
				else
					return Convert.ToString(dr[8]);
			}
		}
		public bool fLastStatusIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[8]==DBNull.Value;
				else
					return dr[8]==DBNull.Value;
			}
		}
		public System.DateTime fLastAlive
		{
			get
			{
				if( bCache )
					return Convert.ToDateTime(((object[]) al[iRecord])[9]);
				else
					return Convert.ToDateTime(dr[9]);
			}
		}
		public bool fLastAliveIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[9]==DBNull.Value;
				else
					return dr[9]==DBNull.Value;
			}
		}
		public System.Int16 fisActive
		{
			get
			{
				if( bCache )
					return Convert.ToInt16(((object[]) al[iRecord])[10]);
				else
					return Convert.ToInt16(dr[10]);
			}
		}
		public System.Int32 fFK_Version
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[11]);
				else
					return Convert.ToInt32(dr[11]);
			}
		}
		public bool fFK_VersionIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[11]==DBNull.Value;
				else
					return dr[11]==DBNull.Value;
			}
		}
		public System.Int16 fisMonitored
		{
			get
			{
				if( bCache )
					return Convert.ToInt16(((object[]) al[iRecord])[12]);
				else
					return Convert.ToInt16(dr[12]);
			}
		}
		public bool fisMonitoredIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[12]==DBNull.Value;
				else
					return dr[12]==DBNull.Value;
			}
		}
	} // public class InstallationDataReader
	public class InstallationTable : DataTable
	{
		public InstallationTable() : base("Installation") {}

		public InstallationDataRow this [System.Int32 PK_Installation]
		{
			get
			{
				InstallationDataRow dr = new InstallationDataRow(Rows.Find(PK_Installation));
				return dr;
			}
		}
		public DataRowCollection LoadAll(MySqlConnection conn, MySqlTransaction trans)
		{
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand = new MySqlCommand("SELECT PK_Installation,Description,Name,Address,City,State,Zip,ActivationCode,LastStatus,LastAlive,isActive,FK_Version,isMonitored FROM Installation", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(this.DataSet,"Installation")==0 )
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
			InstallationData.UpdateInstallation(ref ds,PK_Users,conn,trans);
		}
		public DataColumn cPK_Installation
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
		public DataColumn cName
		{
			get
			{
				return Columns[2];
			}
		}
		public DataColumn cAddress
		{
			get
			{
				return Columns[3];
			}
		}
		public DataColumn cCity
		{
			get
			{
				return Columns[4];
			}
		}
		public DataColumn cState
		{
			get
			{
				return Columns[5];
			}
		}
		public DataColumn cZip
		{
			get
			{
				return Columns[6];
			}
		}
		public DataColumn cActivationCode
		{
			get
			{
				return Columns[7];
			}
		}
		public DataColumn cLastStatus
		{
			get
			{
				return Columns[8];
			}
		}
		public DataColumn cLastAlive
		{
			get
			{
				return Columns[9];
			}
		}
		public DataColumn cisActive
		{
			get
			{
				return Columns[10];
			}
		}
		public DataColumn cFK_Version
		{
			get
			{
				return Columns[11];
			}
		}
		public DataColumn cisMonitored
		{
			get
			{
				return Columns[12];
			}
		}
	}
} // namespace HAData.Common.Data
