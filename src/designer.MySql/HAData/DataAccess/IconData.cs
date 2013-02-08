namespace HAData.DataAccess {
	using System;
	using System.Data;
	using MySql;
	using MySql.Data;
	using MySql.Data.MySqlClient;
	using System.Collections;

	using HAData.Common;

	public class IconData : MyDataSet {
		//
		// Icon table constants
		//
		public const String ICON_TABLE = "Icon";
		public const String PK_ICON_FIELD = "PK_Icon";
		public const String DEFINE_FIELD = "Define";
		public const String DESCRIPTION_FIELD = "Description";
		public const String TRANSPARENTCOLOR_FIELD = "TransparentColor";
		public const String MAINFILENAME_FIELD = "MainFileName";
		public const String SELECTEDFILENAME_FIELD = "SelectedFileName";
		public const String ALTFILENAMES_FIELD = "AltFileNames";
		public const String BACKGROUNDFILENAME_FIELD = "BackgroundFileName";
		// table+field constants
		public const String PK_ICON_TABLE_FIELD = "Icon.PK_Icon";
		public const String DEFINE_TABLE_FIELD = "Icon.Define";
		public const String DESCRIPTION_TABLE_FIELD = "Icon.Description";
		public const String TRANSPARENTCOLOR_TABLE_FIELD = "Icon.TransparentColor";
		public const String MAINFILENAME_TABLE_FIELD = "Icon.MainFileName";
		public const String SELECTEDFILENAME_TABLE_FIELD = "Icon.SelectedFileName";
		public const String ALTFILENAMES_TABLE_FIELD = "Icon.AltFileNames";
		public const String BACKGROUNDFILENAME_TABLE_FIELD = "Icon.BackgroundFileName";
		// DataSetCommand object
		protected MySqlDataAdapter m_DSCommand;

		// Stored procedure parameters
		protected const String PK_ICON_PARM = "@PK_Icon";
		protected const String DEFINE_PARM = "@Define";
		protected const String DESCRIPTION_PARM = "@Description";
		protected const String TRANSPARENTCOLOR_PARM = "@TransparentColor";
		protected const String MAINFILENAME_PARM = "@MainFileName";
		protected const String SELECTEDFILENAME_PARM = "@SelectedFileName";
		protected const String ALTFILENAMES_PARM = "@AltFileNames";
		protected const String BACKGROUNDFILENAME_PARM = "@BackgroundFileName";
		protected const String USERID_PARM = "@UserID";

		protected MySqlCommand m_LoadCommand;
		protected MySqlCommand m_InsertCommand;
		protected MySqlCommand m_UpdateCommand;
		protected MySqlCommand m_DeleteCommand;
		protected MySqlConnection m_Connection;
		protected MySqlTransaction m_Transaction;
		public DataTable Table { get { return Tables[0]; } }


		public IconData() {  // marker:1
			//
			// Create the tables in the dataset
			//
			Tables.Add(BuildDataTables());
			m_Connection = HADataConfiguration.GetMySqlConnection();
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new MySqlDataAdapter();

			m_DSCommand.TableMappings.Add("Table", IconData.ICON_TABLE);
		}

		public IconData(MySqlConnection conn,MySqlTransaction trans) {

			m_Connection = conn;
			m_Transaction = trans;
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new MySqlDataAdapter();

			m_DSCommand.TableMappings.Add("Table", IconData.ICON_TABLE);
		}

		private IconData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context) {
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
			return (DataTable) BuildIconTable();
		}
		public static IconTable BuildIconTable() {
			//
			// Create the Icon table
			//
			IconTable Table = new IconTable();
			DataColumnCollection Columns = Table.Columns;
			DataColumn[] PKColumns = new DataColumn[1];

			DataColumn Column = Columns.Add(PK_ICON_FIELD, typeof(System.Int32));
			Column.AutoIncrement = true;
			Column.AutoIncrementSeed = -1;
			Column.AutoIncrementStep = -1;
			PKColumns[0] = Column;

			Column = Columns.Add(DEFINE_FIELD, typeof(System.String));
			Column.AllowDBNull = false;

			Column = Columns.Add(DESCRIPTION_FIELD, typeof(System.String));
			Column.AllowDBNull = false;

			Columns.Add(TRANSPARENTCOLOR_FIELD, typeof(System.Int32));
			Column = Columns.Add(MAINFILENAME_FIELD, typeof(System.String));
			Column.AllowDBNull = false;

			Columns.Add(SELECTEDFILENAME_FIELD, typeof(System.String));
			Columns.Add(ALTFILENAMES_FIELD, typeof(System.String));
			Columns.Add(BACKGROUNDFILENAME_FIELD, typeof(System.String));
			Table.PrimaryKey = PKColumns;

			return Table;
		}
		protected static void CreateParameters(MySqlParameterCollection Params, bool IsInsert) {
			Params.Add(new MySqlParameter(PK_ICON_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(DEFINE_PARM, MySqlDbType.VarChar, 25));
			Params.Add(new MySqlParameter(DESCRIPTION_PARM, MySqlDbType.VarChar, 25));
			Params.Add(new MySqlParameter(TRANSPARENTCOLOR_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(MAINFILENAME_PARM, MySqlDbType.VarChar, 50));
			Params.Add(new MySqlParameter(SELECTEDFILENAME_PARM, MySqlDbType.VarChar, 50));
			Params.Add(new MySqlParameter(ALTFILENAMES_PARM, MySqlDbType.VarChar, 200));
			Params.Add(new MySqlParameter(BACKGROUNDFILENAME_PARM, MySqlDbType.VarChar, 50));
			Params.Add(new MySqlParameter(USERID_PARM, MySqlDbType.Int32));

			// map the parameters to the data table

			Params[PK_ICON_PARM].SourceColumn = IconData.PK_ICON_FIELD;
			if(IsInsert) {
				Params[PK_ICON_PARM].Direction = ParameterDirection.Output;
			}

			Params[DEFINE_PARM].SourceColumn = IconData.DEFINE_FIELD;
			Params[DESCRIPTION_PARM].SourceColumn = IconData.DESCRIPTION_FIELD;
			Params[TRANSPARENTCOLOR_PARM].SourceColumn = IconData.TRANSPARENTCOLOR_FIELD;
			Params[MAINFILENAME_PARM].SourceColumn = IconData.MAINFILENAME_FIELD;
			Params[SELECTEDFILENAME_PARM].SourceColumn = IconData.SELECTEDFILENAME_FIELD;
			Params[ALTFILENAMES_PARM].SourceColumn = IconData.ALTFILENAMES_FIELD;
			Params[BACKGROUNDFILENAME_PARM].SourceColumn = IconData.BACKGROUNDFILENAME_FIELD;
		}

		protected static void CreateCommands(MySqlConnection Conn, MySqlTransaction Trans, ref MySqlCommand LoadCommand, ref MySqlCommand InsertCommand, ref MySqlCommand UpdateCommand, ref MySqlCommand DeleteCommand) {
			if(LoadCommand == null) {
				// Create the command since it's null
				LoadCommand = new MySqlCommand("sp_Select_Icon", Conn);
				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new MySqlParameter(PK_ICON_PARM, MySqlDbType.Int32,4));
			}

			if(InsertCommand == null) {
				// Create the command since it's null
				InsertCommand = new MySqlCommand("sp_Insert_Icon", Conn);
				InsertCommand.CommandType = CommandType.StoredProcedure;
				InsertCommand.Transaction = Trans;

				MySqlParameterCollection Params = InsertCommand.Parameters;

				CreateParameters(Params, true);

			}

			if(UpdateCommand == null) {
				// Create the command since it's null
				UpdateCommand = new MySqlCommand("sp_Update_Icon", Conn);
				UpdateCommand.CommandType = CommandType.StoredProcedure;
				UpdateCommand.Transaction = Trans;

				MySqlParameterCollection Params = UpdateCommand.Parameters;

				CreateParameters(Params, false);

			}
			if (DeleteCommand == null)
			{
				DeleteCommand = new MySqlCommand("sp_Delete_Icon", Conn);
				DeleteCommand.CommandType = CommandType.StoredProcedure;
				DeleteCommand.Transaction = Trans;

				DeleteCommand.Parameters.Add(PK_ICON_PARM, MySqlDbType.Int32,4, PK_ICON_FIELD);
				DeleteCommand.Parameters.Add(USERID_PARM, MySqlDbType.Int32);
			}
		}

		protected static void CreateCommands(MySqlDataAdapter odbcda,MySqlConnection Conn, MySqlTransaction Trans, ref MySqlCommand LoadCommand, ref MySqlCommand InsertCommand, ref MySqlCommand UpdateCommand, ref MySqlCommand DeleteCommand) {
				LoadCommand = new MySqlCommand("SELECT PK_Icon,Define,Description,TransparentColor,MainFileName,SelectedFileName,AltFileNames,BackgroundFileName FROM Icon", Conn);
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new MySqlParameter(PK_ICON_PARM, MySqlDbType.Int32,4));

			odbcda.SelectCommand = LoadCommand;
			MySqlCommandBuilder odbcCB = new MySqlCommandBuilder(odbcda);
			odbcCB.RefreshSchema();
			DeleteCommand = odbcCB.GetDeleteCommand();
			InsertCommand = odbcCB.GetInsertCommand();
			UpdateCommand = odbcCB.GetUpdateCommand();
		}

		public IconData LoadIcon(System.Int32 PK_Icon)
		{
			m_DSCommand.SelectCommand = m_LoadCommand;
			m_DSCommand.SelectCommand.Parameters[PK_ICON_PARM].Value = PK_Icon;

			m_DSCommand.Fill(this);
			return this;
		}

		public static DataRowCollection LoadIconWithWhere(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans, string WhereClause) // marker:2
		{
			DataRowCollection dr;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["Icon"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}
			
			DataSet dsTemp = new MyDataSet();
			dsTemp.Tables.Add(BuildDataTables());
			
			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();
			
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			string sSQL = "SELECT PK_Icon, Define, Description, TransparentColor, MainFileName, SelectedFileName, AltFileNames, BackgroundFileName FROM Icon WHERE " + WhereClause;
			
			MySqlCommand LoadCommand = new MySqlCommand(sSQL,conn);
			
			if( trans!=null )
				LoadCommand.Transaction = trans;
			
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(dsTemp,"Icon");
			
			dr=dsTemp.Tables["Icon"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}

		public static DataRow LoadNoCacheIcon(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans, System.Int32 PK_Icon)
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["Icon"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}

			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand;
			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();

			LoadCommand = new MySqlCommand("sp_Select_Icon", conn);

			LoadCommand.CommandType = CommandType.StoredProcedure;
			LoadCommand.Parameters.Add(new MySqlParameter(PK_ICON_PARM, MySqlDbType.Int32,4));
			LoadCommand.Parameters[PK_ICON_PARM].Value = PK_Icon;
			if( trans!=null )
				LoadCommand.Transaction = trans;
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(ds,"Icon");
				dr = ds.Tables["Icon"].Rows.Find(PK_Icon);
			return dr;
		}

		public static DataRow LoadIcon(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans, System.Int32 PK_Icon)  // marker:3
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["Icon"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
				else
				dr = dt.Rows.Find(PK_Icon);
			}

			if( dr==null )
			{
				MySqlDataAdapter sqlda = new MySqlDataAdapter();
				MySqlCommand LoadCommand;
				if( conn==null )
					conn = HADataConfiguration.GetMySqlConnection();

				LoadCommand = new MySqlCommand("sp_Select_Icon", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new MySqlParameter(PK_ICON_PARM, MySqlDbType.Int32,4));
				LoadCommand.Parameters[PK_ICON_PARM].Value = PK_Icon;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(ds,"Icon");
				dr = ds.Tables["Icon"].Rows.Find(PK_Icon);
			}
			return dr;
		}

		public IconData LoadAll() {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new MySqlCommand("SELECT * FROM Icon", m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this);
			return this;

		}

		public static DataRowCollection LoadAll(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans) {

			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand = new MySqlCommand("SELECT * FROM Icon", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(ds,"Icon")==0 )
				return null;
			else
				return ds.Tables["Icon"].Rows;

		}

		public IconData ExecuteQuery(String sSQL) {
			return ExecuteQuery(sSQL,ICON_TABLE);
		}
		public IconData ExecuteQuery(String sSQL,String sTableName) {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new MySqlCommand(sSQL, m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this,sTableName);

			return this;
		}

		public static DataRowCollection ExecuteQuery(String sSQL,ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans) {
			return ExecuteQuery(sSQL,ref ds,conn,trans,"Icon");
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

		public bool UpdateIcon(int CurUserID) {
			m_DSCommand.UpdateCommand = m_UpdateCommand;
			m_DSCommand.UpdateCommand.Parameters[USERID_PARM].Value = CurUserID;

			m_DSCommand.InsertCommand = m_InsertCommand;
			m_DSCommand.InsertCommand.Parameters[USERID_PARM].Value = CurUserID;

			if( m_DeleteCommand != null )
			{
				m_DSCommand.DeleteCommand = m_DeleteCommand;
				m_DSCommand.DeleteCommand.Parameters[USERID_PARM].Value = CurUserID;
			}

			m_DSCommand.Update(this, IconData.ICON_TABLE);
			return true;
		}

		public static bool UpdateIcon(ref MyDataSet ds, int CurUserID)
		{
			MySqlConnection OdbcConn = HADataConfiguration.GetMySqlConnection();
			return UpdateIcon(ref ds,CurUserID,OdbcConn,null);
		}

		public static bool UpdateIcon(ref MyDataSet ds, int CurUserID,MySqlConnection OdbcConn,MySqlTransaction Trans)
		{
			DataTable dt = ds.Tables[ICON_TABLE];
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

	} // public class IconData
	public class IconDataRow
	{
		public DataRow dr = null;
		public IconDataRow(DataRow d)
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
		public System.Int32 fPK_Icon
		{
			get
			{
				return Convert.ToInt32(dr[0]);
			}
		}
		public System.String fDefine
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
		public System.Int32 fTransparentColor
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
		public bool fTransparentColorIsNull
		{
			get
			{
				return dr[3]==DBNull.Value;
			}
		}
		public void fTransparentColorSetNull()
		{
			dr[3]=DBNull.Value;
		}
		public System.String fMainFileName
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
		public System.String fSelectedFileName
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
		public bool fSelectedFileNameIsNull
		{
			get
			{
				return dr[5]==DBNull.Value;
			}
		}
		public void fSelectedFileNameSetNull()
		{
			dr[5]=DBNull.Value;
		}
		public System.String fAltFileNames
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
		public bool fAltFileNamesIsNull
		{
			get
			{
				return dr[6]==DBNull.Value;
			}
		}
		public void fAltFileNamesSetNull()
		{
			dr[6]=DBNull.Value;
		}
		public System.String fBackgroundFileName
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
		public bool fBackgroundFileNameIsNull
		{
			get
			{
				return dr[7]==DBNull.Value;
			}
		}
		public void fBackgroundFileNameSetNull()
		{
			dr[7]=DBNull.Value;
		}
	} // public class IconDataRow
	public class IconDataReader
	{
		public MySqlDataReader dr;
		bool bCache=false;
		int iRecord=-1,iNumRecords=-1;
		ArrayList al = null;

		public IconDataReader(MySqlDataReader d)
		{
			dr=d;
		}
		public IconDataReader(MySqlCommand cmd)
		{
			dr = cmd.ExecuteReader();
		}

		public IconDataReader(MySqlCommand cmd,bool Cache)
		{
			dr = cmd.ExecuteReader();
			bCache=Cache;
			if( bCache )
				CacheAndClose();
		}

		public IconDataReader(string sSQL)
		{
			MySqlConnection conn = HADataConfiguration.GetMySqlConnection();

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM Icon WHERE " + sSQL;
			}

			MySqlCommand cmd = new MySqlCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public IconDataReader(string sSQL,MySqlConnection conn)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetMySqlConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM Icon WHERE " + sSQL;
			}

			MySqlCommand cmd = new MySqlCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public IconDataReader(string sSQL,MySqlConnection conn,MySqlTransaction trans,bool Cache)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetMySqlConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM Icon WHERE " + sSQL;
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
		public System.Int32 fPK_Icon
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[0]);
				else
					return Convert.ToInt32(dr[0]);
			}
		}
		public System.String fDefine
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[1]);
				else
					return Convert.ToString(dr[1]);
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
		public System.Int32 fTransparentColor
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[3]);
				else
					return Convert.ToInt32(dr[3]);
			}
		}
		public bool fTransparentColorIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[3]==DBNull.Value;
				else
					return dr[3]==DBNull.Value;
			}
		}
		public System.String fMainFileName
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[4]);
				else
					return Convert.ToString(dr[4]);
			}
		}
		public System.String fSelectedFileName
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[5]);
				else
					return Convert.ToString(dr[5]);
			}
		}
		public bool fSelectedFileNameIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[5]==DBNull.Value;
				else
					return dr[5]==DBNull.Value;
			}
		}
		public System.String fAltFileNames
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[6]);
				else
					return Convert.ToString(dr[6]);
			}
		}
		public bool fAltFileNamesIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[6]==DBNull.Value;
				else
					return dr[6]==DBNull.Value;
			}
		}
		public System.String fBackgroundFileName
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[7]);
				else
					return Convert.ToString(dr[7]);
			}
		}
		public bool fBackgroundFileNameIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[7]==DBNull.Value;
				else
					return dr[7]==DBNull.Value;
			}
		}
	} // public class IconDataReader
	public class IconTable : DataTable
	{
		public IconTable() : base("Icon") {}

		public IconDataRow this [System.Int32 PK_Icon]
		{
			get
			{
				IconDataRow dr = new IconDataRow(Rows.Find(PK_Icon));
				return dr;
			}
		}
		public DataRowCollection LoadAll(MySqlConnection conn, MySqlTransaction trans)
		{
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand = new MySqlCommand("SELECT PK_Icon,Define,Description,TransparentColor,MainFileName,SelectedFileName,AltFileNames,BackgroundFileName FROM Icon", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(this.DataSet,"Icon")==0 )
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
			IconData.UpdateIcon(ref ds,PK_Users,conn,trans);
		}
		public DataColumn cPK_Icon
		{
			get
			{
				return Columns[0];
			}
		}
		public DataColumn cDefine
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
		public DataColumn cTransparentColor
		{
			get
			{
				return Columns[3];
			}
		}
		public DataColumn cMainFileName
		{
			get
			{
				return Columns[4];
			}
		}
		public DataColumn cSelectedFileName
		{
			get
			{
				return Columns[5];
			}
		}
		public DataColumn cAltFileNames
		{
			get
			{
				return Columns[6];
			}
		}
		public DataColumn cBackgroundFileName
		{
			get
			{
				return Columns[7];
			}
		}
	}
} // namespace HAData.Common.Data
