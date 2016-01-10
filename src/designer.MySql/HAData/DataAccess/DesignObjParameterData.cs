namespace HAData.DataAccess {
	using System;
	using System.Data;
	using MySql;
	using MySql.Data;
	using MySql.Data.MySqlClient;
	using System.Collections;

	using HAData.Common;

	public class DesignObjParameterData : MyDataSet {
		//
		// DesignObjParameter table constants
		//
		public const String DESIGNOBJPARAMETER_TABLE = "DesignObjParameter";
		public const String PK_DESIGNOBJPARAMETER_FIELD = "PK_DesignObjParameter";
		public const String DEFINE_FIELD = "Define";
		public const String COMMENTS_FIELD = "Comments";
		public const String DESCRIPTION_FIELD = "Description";
		public const String FK_PARAMETERTYPE_FIELD = "FK_ParameterType";
		// table+field constants
		public const String PK_DESIGNOBJPARAMETER_TABLE_FIELD = "DesignObjParameter.PK_DesignObjParameter";
		public const String DEFINE_TABLE_FIELD = "DesignObjParameter.Define";
		public const String COMMENTS_TABLE_FIELD = "DesignObjParameter.Comments";
		public const String DESCRIPTION_TABLE_FIELD = "DesignObjParameter.Description";
		public const String FK_PARAMETERTYPE_TABLE_FIELD = "DesignObjParameter.FK_ParameterType";
		public const int GRAPHIC_FILENAME_CONST = 1;
		public const int XPOS_CONST = 2;
		public const int YPOS_CONST = 3;
		public const int NUMCOLUMNS_CONST = 7;
		public const int NUMROWS_CONST = 8;
		public const int FIXEDROWHEIGHT_CONST = 9;
		public const int FIXEDCOLUMNWIDTH_CONST = 10;
		public const int PKID_ARRAY_CONST = 11;
		public const int DOWN_ACROSS_CONST = 12;
		public const int ROW_SPACING_CONST = 13;
		public const int COLUMN_SPACING_CONST = 14;
		public const int PKID_OBJHDR_MORE_FWD_CONST = 15;
		public const int PKID_OBJHDR_MORE_BAK_CONST = 16;
		public const int PKID_DEVICE_CONST = 17;
		public const int WIDTH_CONST = 18;
		public const int HEIGHT_CONST = 19;
		public const int FIRST_ROW_COLOR_CONST = 20;
		public const int CELL_COLOR_CONST = 21;
		public const int FIRST_COLUMN_COLOR_CONST = 22;
		public const int FONT_CONST = 23;
		public const int FIRST_ROW_FONT_CONST = 24;
		public const int FIRST_COLUMN_FONT_CONST = 25;
		public const int FIRST_ROW_HEIGHT_CONST = 26;
		public const int FIRST_COLUMN_WIDTH_CONST = 27;
		public const int KEEP_ROW_HEADER_CONST = 28;
		public const int KEEP_COLUMN_HEADER_CONST = 29;
		public const int BORDER_STYLE_CONST = 30;
		public const int INIT_ROW_NUM_CONST = 31;
		public const int INIT_COL_NUM_CONST = 32;
		public const int SELECTED_CELL_COLOR_CONST = 40;
		public const int IS_MULTI_SELECT_CONST = 41;
		public const int PKID_VARIABLE_CONST = 42;
		public const int DATAGRID_ID_CONST = 43;
		public const int CYCLE_GRID_CONST = 44;
		public const int ALTERNATE_CELL_COLOR_CONST = 45;
		public const int SOURCE_CONST = 47;
		public const int OPTIONS_CONST = 49;
		public const int SELECTED_GRAPHIC_FIL_CONST = 50;
		public const int ALT_GRAPHIC_FILE_CONST = 51;
		public const int TRANSPARENCY_CONST = 52;
		public const int PERSIST_XY_POSITION_CONST = 53;
		public const int EXTRA_INFO_CONST = 55;
		public const int PAGE_CONST = 56;
		public const int TYPE_CONST = 57;
		public const int PRONTO_FILE_CONST = 58;
		public const int HIGHLIGHTED_GRAPHIC_CONST = 59;
		// DataSetCommand object
		protected MySqlDataAdapter m_DSCommand;

		// Stored procedure parameters
		protected const String PK_DESIGNOBJPARAMETER_PARM = "@PK_DesignObjParameter";
		protected const String DEFINE_PARM = "@Define";
		protected const String COMMENTS_PARM = "@Comments";
		protected const String DESCRIPTION_PARM = "@Description";
		protected const String FK_PARAMETERTYPE_PARM = "@FK_ParameterType";
		protected const String USERID_PARM = "@UserID";

		protected MySqlCommand m_LoadCommand;
		protected MySqlCommand m_InsertCommand;
		protected MySqlCommand m_UpdateCommand;
		protected MySqlCommand m_DeleteCommand;
		protected MySqlConnection m_Connection;
		protected MySqlTransaction m_Transaction;
		public DataTable Table { get { return Tables[0]; } }


		public DesignObjParameterData() {  // marker:1
			//
			// Create the tables in the dataset
			//
			Tables.Add(BuildDataTables());
			m_Connection = HADataConfiguration.GetMySqlConnection();
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new MySqlDataAdapter();

			m_DSCommand.TableMappings.Add("Table", DesignObjParameterData.DESIGNOBJPARAMETER_TABLE);
		}

		public DesignObjParameterData(MySqlConnection conn,MySqlTransaction trans) {

			m_Connection = conn;
			m_Transaction = trans;
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new MySqlDataAdapter();

			m_DSCommand.TableMappings.Add("Table", DesignObjParameterData.DESIGNOBJPARAMETER_TABLE);
		}

		private DesignObjParameterData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context) {
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
			return (DataTable) BuildDesignObjParameterTable();
		}
		public static DesignObjParameterTable BuildDesignObjParameterTable() {
			//
			// Create the DesignObjParameter table
			//
			DesignObjParameterTable Table = new DesignObjParameterTable();
			DataColumnCollection Columns = Table.Columns;
			DataColumn[] PKColumns = new DataColumn[1];

			DataColumn Column = Columns.Add(PK_DESIGNOBJPARAMETER_FIELD, typeof(System.Int32));
			Column.AutoIncrement = true;
			Column.AutoIncrementSeed = -1;
			Column.AutoIncrementStep = -1;
			PKColumns[0] = Column;

			Column = Columns.Add(DEFINE_FIELD, typeof(System.String));
			Column.AllowDBNull = false;

			Columns.Add(COMMENTS_FIELD, typeof(System.String));
			Column = Columns.Add(DESCRIPTION_FIELD, typeof(System.String));
			Column.AllowDBNull = false;

			Column = Columns.Add(FK_PARAMETERTYPE_FIELD, typeof(System.Int32));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Table.PrimaryKey = PKColumns;

			return Table;
		}
		protected static void CreateParameters(MySqlParameterCollection Params, bool IsInsert) {
			Params.Add(new MySqlParameter(PK_DESIGNOBJPARAMETER_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(DEFINE_PARM, MySqlDbType.VarChar, 20));
			Params.Add(new MySqlParameter(COMMENTS_PARM, MySqlDbType.VarChar, 255));
			Params.Add(new MySqlParameter(DESCRIPTION_PARM, MySqlDbType.VarChar, 20));
			Params.Add(new MySqlParameter(FK_PARAMETERTYPE_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(USERID_PARM, MySqlDbType.Int32));

			// map the parameters to the data table

			Params[PK_DESIGNOBJPARAMETER_PARM].SourceColumn = DesignObjParameterData.PK_DESIGNOBJPARAMETER_FIELD;
			if(IsInsert) {
				Params[PK_DESIGNOBJPARAMETER_PARM].Direction = ParameterDirection.Output;
			}

			Params[DEFINE_PARM].SourceColumn = DesignObjParameterData.DEFINE_FIELD;
			Params[COMMENTS_PARM].SourceColumn = DesignObjParameterData.COMMENTS_FIELD;
			Params[DESCRIPTION_PARM].SourceColumn = DesignObjParameterData.DESCRIPTION_FIELD;
			Params[FK_PARAMETERTYPE_PARM].SourceColumn = DesignObjParameterData.FK_PARAMETERTYPE_FIELD;
		}

		protected static void CreateCommands(MySqlConnection Conn, MySqlTransaction Trans, ref MySqlCommand LoadCommand, ref MySqlCommand InsertCommand, ref MySqlCommand UpdateCommand, ref MySqlCommand DeleteCommand) {
			if(LoadCommand == null) {
				// Create the command since it's null
				LoadCommand = new MySqlCommand("sp_Select_DesignObjParameter", Conn);
				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new MySqlParameter(PK_DESIGNOBJPARAMETER_PARM, MySqlDbType.Int32,4));
			}

			if(InsertCommand == null) {
				// Create the command since it's null
				InsertCommand = new MySqlCommand("sp_Insert_DesignObjParameter", Conn);
				InsertCommand.CommandType = CommandType.StoredProcedure;
				InsertCommand.Transaction = Trans;

				MySqlParameterCollection Params = InsertCommand.Parameters;

				CreateParameters(Params, true);

			}

			if(UpdateCommand == null) {
				// Create the command since it's null
				UpdateCommand = new MySqlCommand("sp_Update_DesignObjParameter", Conn);
				UpdateCommand.CommandType = CommandType.StoredProcedure;
				UpdateCommand.Transaction = Trans;

				MySqlParameterCollection Params = UpdateCommand.Parameters;

				CreateParameters(Params, false);

			}
			if (DeleteCommand == null)
			{
				DeleteCommand = new MySqlCommand("sp_Delete_DesignObjParameter", Conn);
				DeleteCommand.CommandType = CommandType.StoredProcedure;
				DeleteCommand.Transaction = Trans;

				DeleteCommand.Parameters.Add(PK_DESIGNOBJPARAMETER_PARM, MySqlDbType.Int32,4, PK_DESIGNOBJPARAMETER_FIELD);
				DeleteCommand.Parameters.Add(USERID_PARM, MySqlDbType.Int32);
			}
		}

		protected static void CreateCommands(MySqlDataAdapter odbcda,MySqlConnection Conn, MySqlTransaction Trans, ref MySqlCommand LoadCommand, ref MySqlCommand InsertCommand, ref MySqlCommand UpdateCommand, ref MySqlCommand DeleteCommand) {
				LoadCommand = new MySqlCommand("SELECT PK_DesignObjParameter,Define,Comments,Description,FK_ParameterType FROM DesignObjParameter", Conn);
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new MySqlParameter(PK_DESIGNOBJPARAMETER_PARM, MySqlDbType.Int32,4));

			odbcda.SelectCommand = LoadCommand;
			MySqlCommandBuilder odbcCB = new MySqlCommandBuilder(odbcda);
			odbcCB.RefreshSchema();
			DeleteCommand = odbcCB.GetDeleteCommand();
			InsertCommand = odbcCB.GetInsertCommand();
			UpdateCommand = odbcCB.GetUpdateCommand();
		}

		public DesignObjParameterData LoadDesignObjParameter(System.Int32 PK_DesignObjParameter)
		{
			m_DSCommand.SelectCommand = m_LoadCommand;
			m_DSCommand.SelectCommand.Parameters[PK_DESIGNOBJPARAMETER_PARM].Value = PK_DesignObjParameter;

			m_DSCommand.Fill(this);
			return this;
		}

		public static DataRowCollection LoadDesignObjParameterWithWhere(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans, string WhereClause) // marker:2
		{
			DataRowCollection dr;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["DesignObjParameter"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}
			
			DataSet dsTemp = new MyDataSet();
			dsTemp.Tables.Add(BuildDataTables());
			
			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();
			
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			string sSQL = "SELECT PK_DesignObjParameter, Define, Comments, Description, FK_ParameterType FROM DesignObjParameter WHERE " + WhereClause;
			
			MySqlCommand LoadCommand = new MySqlCommand(sSQL,conn);
			
			if( trans!=null )
				LoadCommand.Transaction = trans;
			
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(dsTemp,"DesignObjParameter");
			
			dr=dsTemp.Tables["DesignObjParameter"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}

		public static DataRow LoadNoCacheDesignObjParameter(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans, System.Int32 PK_DesignObjParameter)
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["DesignObjParameter"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}

			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand;
			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();

			LoadCommand = new MySqlCommand("sp_Select_DesignObjParameter", conn);

			LoadCommand.CommandType = CommandType.StoredProcedure;
			LoadCommand.Parameters.Add(new MySqlParameter(PK_DESIGNOBJPARAMETER_PARM, MySqlDbType.Int32,4));
			LoadCommand.Parameters[PK_DESIGNOBJPARAMETER_PARM].Value = PK_DesignObjParameter;
			if( trans!=null )
				LoadCommand.Transaction = trans;
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(ds,"DesignObjParameter");
				dr = ds.Tables["DesignObjParameter"].Rows.Find(PK_DesignObjParameter);
			return dr;
		}

		public static DataRow LoadDesignObjParameter(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans, System.Int32 PK_DesignObjParameter)  // marker:3
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["DesignObjParameter"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
				else
				dr = dt.Rows.Find(PK_DesignObjParameter);
			}

			if( dr==null )
			{
				MySqlDataAdapter sqlda = new MySqlDataAdapter();
				MySqlCommand LoadCommand;
				if( conn==null )
					conn = HADataConfiguration.GetMySqlConnection();

				LoadCommand = new MySqlCommand("sp_Select_DesignObjParameter", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new MySqlParameter(PK_DESIGNOBJPARAMETER_PARM, MySqlDbType.Int32,4));
				LoadCommand.Parameters[PK_DESIGNOBJPARAMETER_PARM].Value = PK_DesignObjParameter;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(ds,"DesignObjParameter");
				dr = ds.Tables["DesignObjParameter"].Rows.Find(PK_DesignObjParameter);
			}
			return dr;
		}

		public DesignObjParameterData LoadAll() {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new MySqlCommand("SELECT * FROM DesignObjParameter", m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this);
			return this;

		}

		public static DataRowCollection LoadAll(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans) {

			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand = new MySqlCommand("SELECT * FROM DesignObjParameter", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(ds,"DesignObjParameter")==0 )
				return null;
			else
				return ds.Tables["DesignObjParameter"].Rows;

		}

		public DesignObjParameterData ExecuteQuery(String sSQL) {
			return ExecuteQuery(sSQL,DESIGNOBJPARAMETER_TABLE);
		}
		public DesignObjParameterData ExecuteQuery(String sSQL,String sTableName) {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new MySqlCommand(sSQL, m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this,sTableName);

			return this;
		}

		public static DataRowCollection ExecuteQuery(String sSQL,ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans) {
			return ExecuteQuery(sSQL,ref ds,conn,trans,"DesignObjParameter");
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

		public bool UpdateDesignObjParameter(int CurUserID) {
			m_DSCommand.UpdateCommand = m_UpdateCommand;
			m_DSCommand.UpdateCommand.Parameters[USERID_PARM].Value = CurUserID;

			m_DSCommand.InsertCommand = m_InsertCommand;
			m_DSCommand.InsertCommand.Parameters[USERID_PARM].Value = CurUserID;

			if( m_DeleteCommand != null )
			{
				m_DSCommand.DeleteCommand = m_DeleteCommand;
				m_DSCommand.DeleteCommand.Parameters[USERID_PARM].Value = CurUserID;
			}

			m_DSCommand.Update(this, DesignObjParameterData.DESIGNOBJPARAMETER_TABLE);
			return true;
		}

		public static bool UpdateDesignObjParameter(ref MyDataSet ds, int CurUserID)
		{
			MySqlConnection OdbcConn = HADataConfiguration.GetMySqlConnection();
			return UpdateDesignObjParameter(ref ds,CurUserID,OdbcConn,null);
		}

		public static bool UpdateDesignObjParameter(ref MyDataSet ds, int CurUserID,MySqlConnection OdbcConn,MySqlTransaction Trans)
		{
			DataTable dt = ds.Tables[DESIGNOBJPARAMETER_TABLE];
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

	} // public class DesignObjParameterData
	public class DesignObjParameterDataRow
	{
		public DataRow dr = null;
		public DesignObjParameterDataRow(DataRow d)
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
		public System.Int32 fPK_DesignObjParameter
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
		public System.String fComments
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
		public bool fCommentsIsNull
		{
			get
			{
				return dr[2]==DBNull.Value;
			}
		}
		public void fCommentsSetNull()
		{
			dr[2]=DBNull.Value;
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
		public System.Int32 fFK_ParameterType
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
		public ParameterTypeDataRow fFK_ParameterType_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tParameterType[Convert.ToInt32(dr[4])];
			}
		}
	} // public class DesignObjParameterDataRow
	public class DesignObjParameterDataReader
	{
		public MySqlDataReader dr;
		bool bCache=false;
		int iRecord=-1,iNumRecords=-1;
		ArrayList al = null;

		public DesignObjParameterDataReader(MySqlDataReader d)
		{
			dr=d;
		}
		public DesignObjParameterDataReader(MySqlCommand cmd)
		{
			dr = cmd.ExecuteReader();
		}

		public DesignObjParameterDataReader(MySqlCommand cmd,bool Cache)
		{
			dr = cmd.ExecuteReader();
			bCache=Cache;
			if( bCache )
				CacheAndClose();
		}

		public DesignObjParameterDataReader(string sSQL)
		{
			MySqlConnection conn = HADataConfiguration.GetMySqlConnection();

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM DesignObjParameter WHERE " + sSQL;
			}

			MySqlCommand cmd = new MySqlCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public DesignObjParameterDataReader(string sSQL,MySqlConnection conn)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetMySqlConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM DesignObjParameter WHERE " + sSQL;
			}

			MySqlCommand cmd = new MySqlCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public DesignObjParameterDataReader(string sSQL,MySqlConnection conn,MySqlTransaction trans,bool Cache)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetMySqlConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM DesignObjParameter WHERE " + sSQL;
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
				object[] objs = new object[5];
				for(int i=0;i<5;i++)
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
		public System.Int32 fPK_DesignObjParameter
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
		public System.String fComments
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[2]);
				else
					return Convert.ToString(dr[2]);
			}
		}
		public bool fCommentsIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[2]==DBNull.Value;
				else
					return dr[2]==DBNull.Value;
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
		public System.Int32 fFK_ParameterType
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[4]);
				else
					return Convert.ToInt32(dr[4]);
			}
		}
	} // public class DesignObjParameterDataReader
	public class DesignObjParameterTable : DataTable
	{
		public DesignObjParameterTable() : base("DesignObjParameter") {}

		public DesignObjParameterDataRow this [System.Int32 PK_DesignObjParameter]
		{
			get
			{
				DesignObjParameterDataRow dr = new DesignObjParameterDataRow(Rows.Find(PK_DesignObjParameter));
				return dr;
			}
		}
		public DataRowCollection LoadAll(MySqlConnection conn, MySqlTransaction trans)
		{
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand = new MySqlCommand("SELECT PK_DesignObjParameter,Define,Comments,Description,FK_ParameterType FROM DesignObjParameter", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(this.DataSet,"DesignObjParameter")==0 )
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
			DesignObjParameterData.UpdateDesignObjParameter(ref ds,PK_Users,conn,trans);
		}
		public DataColumn cPK_DesignObjParameter
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
		public DataColumn cComments
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
		public DataColumn cFK_ParameterType
		{
			get
			{
				return Columns[4];
			}
		}
	}
} // namespace HAData.Common.Data
