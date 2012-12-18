namespace HAData.DataAccess {
	using System;
	using System.Data;
	using MySql;
	using MySql.Data;
	using MySql.Data.MySqlClient;
	using System.Collections;

	using HAData.Common;

	public class DesignObjVariation_DesignObjData : MyDataSet {
		//
		// DesignObjVariation_DesignObj table constants
		//
		public const String DESIGNOBJVARIATION_DESIGNOBJ_TABLE = "DesignObjVariation_DesignObj";
		public const String PK_DESIGNOBJVARIATION_DESIGNOBJ_FIELD = "PK_DesignObjVariation_DesignObj";
		public const String FK_DESIGNOBJVARIATION_PARENT_FIELD = "FK_DesignObjVariation_Parent";
		public const String FK_DESIGNOBJ_CHILD_FIELD = "FK_DesignObj_Child";

		// table+field constants
		public const String PK_DESIGNOBJVARIATION_DESIGNOBJ_TABLE_FIELD = "DesignObjVariation_DesignObj.PK_DesignObjVariation_DesignObj";
		public const String FK_DESIGNOBJVARIATION_PARENT_TABLE_FIELD = "DesignObjVariation_DesignObj.FK_DesignObjVariation_Parent";
		public const String FK_DESIGNOBJ_CHILD_TABLE_FIELD = "DesignObjVariation_DesignObj.FK_DesignObj_Child";

		// DataSetCommand object
		protected MySqlDataAdapter m_DSCommand;

		// Stored procedure parameters
		protected const String PK_DESIGNOBJVARIATION_DESIGNOBJ_PARM = "@PK_DesignObjVariation_DesignObj";
		protected const String FK_DESIGNOBJVARIATION_PARENT_PARM = "@FK_DesignObjVariation_Parent";
		protected const String FK_DESIGNOBJ_CHILD_PARM = "@FK_DesignObj_Child";

		protected const String USERID_PARM = "@UserID";

		protected MySqlCommand m_LoadCommand;
		protected MySqlCommand m_InsertCommand;
		protected MySqlCommand m_UpdateCommand;
		protected MySqlCommand m_DeleteCommand;
		protected MySqlConnection m_Connection;
		protected MySqlTransaction m_Transaction;
		public DataTable Table { get { return Tables[0]; } }


		public DesignObjVariation_DesignObjData() {  // marker:1
			//
			// Create the tables in the dataset
			//
			Tables.Add(BuildDataTables());
			m_Connection = HADataConfiguration.GetMySqlConnection();
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new MySqlDataAdapter();

			m_DSCommand.TableMappings.Add("Table", DesignObjVariation_DesignObjData.DESIGNOBJVARIATION_DESIGNOBJ_TABLE);
		}

		public DesignObjVariation_DesignObjData(MySqlConnection conn,MySqlTransaction trans) {

			m_Connection = conn;
			m_Transaction = trans;
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new MySqlDataAdapter();

			m_DSCommand.TableMappings.Add("Table", DesignObjVariation_DesignObjData.DESIGNOBJVARIATION_DESIGNOBJ_TABLE);
		}

		private DesignObjVariation_DesignObjData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context) {
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
			return (DataTable) BuildDesignObjVariation_DesignObjTable();
		}
		public static DesignObjVariation_DesignObjTable BuildDesignObjVariation_DesignObjTable() {
			//
			// Create the DesignObjVariation_DesignObj table
			//
			DesignObjVariation_DesignObjTable Table = new DesignObjVariation_DesignObjTable();
			DataColumnCollection Columns = Table.Columns;
			DataColumn[] PKColumns = new DataColumn[1];

			DataColumn Column = Columns.Add(PK_DESIGNOBJVARIATION_DESIGNOBJ_FIELD, typeof(System.Int32));
			Column.AutoIncrement = true;
			Column.AutoIncrementSeed = -1;
			Column.AutoIncrementStep = -1;
			PKColumns[0] = Column;

			Columns.Add(FK_DESIGNOBJVARIATION_PARENT_FIELD, typeof(System.Int32));
			Columns.Add(FK_DESIGNOBJ_CHILD_FIELD, typeof(System.Int32));

			Table.PrimaryKey = PKColumns;

			return Table;
		}
		protected static void CreateParameters(MySqlParameterCollection Params, bool IsInsert) {
			Params.Add(new MySqlParameter(PK_DESIGNOBJVARIATION_DESIGNOBJ_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(FK_DESIGNOBJVARIATION_PARENT_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(FK_DESIGNOBJ_CHILD_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(USERID_PARM, MySqlDbType.Int32));

			// map the parameters to the data table

			Params[PK_DESIGNOBJVARIATION_DESIGNOBJ_PARM].SourceColumn = DesignObjVariation_DesignObjData.PK_DESIGNOBJVARIATION_DESIGNOBJ_FIELD;
			if(IsInsert) {
				Params[PK_DESIGNOBJVARIATION_DESIGNOBJ_PARM].Direction = ParameterDirection.Output;
			}

			Params[FK_DESIGNOBJVARIATION_PARENT_PARM].SourceColumn = DesignObjVariation_DesignObjData.FK_DESIGNOBJVARIATION_PARENT_FIELD;
			Params[FK_DESIGNOBJ_CHILD_PARM].SourceColumn = DesignObjVariation_DesignObjData.FK_DESIGNOBJ_CHILD_FIELD;
		}

		protected static void CreateCommands(MySqlConnection Conn, MySqlTransaction Trans, ref MySqlCommand LoadCommand, ref MySqlCommand InsertCommand, ref MySqlCommand UpdateCommand, ref MySqlCommand DeleteCommand) {
			if(LoadCommand == null) {
				// Create the command since it's null
				LoadCommand = new MySqlCommand("sp_Select_DesignObjVariation_DesignObj", Conn);
				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new MySqlParameter(PK_DESIGNOBJVARIATION_DESIGNOBJ_PARM, MySqlDbType.Int32,4));
			}

			if(InsertCommand == null) {
				// Create the command since it's null
				InsertCommand = new MySqlCommand("sp_Insert_DesignObjVariation_DesignObj", Conn);
				InsertCommand.CommandType = CommandType.StoredProcedure;
				InsertCommand.Transaction = Trans;

				MySqlParameterCollection Params = InsertCommand.Parameters;

				CreateParameters(Params, true);

			}

			if(UpdateCommand == null) {
				// Create the command since it's null
				UpdateCommand = new MySqlCommand("sp_Update_DesignObjVariation_DesignObj", Conn);
				UpdateCommand.CommandType = CommandType.StoredProcedure;
				UpdateCommand.Transaction = Trans;

				MySqlParameterCollection Params = UpdateCommand.Parameters;

				CreateParameters(Params, false);

			}
			if (DeleteCommand == null)
			{
				DeleteCommand = new MySqlCommand("sp_Delete_DesignObjVariation_DesignObj", Conn);
				DeleteCommand.CommandType = CommandType.StoredProcedure;
				DeleteCommand.Transaction = Trans;

				DeleteCommand.Parameters.Add(PK_DESIGNOBJVARIATION_DESIGNOBJ_PARM, MySqlDbType.Int32,4, PK_DESIGNOBJVARIATION_DESIGNOBJ_FIELD);
				DeleteCommand.Parameters.Add(USERID_PARM, MySqlDbType.Int32);
			}
		}

		protected static void CreateCommands(MySqlDataAdapter odbcda,MySqlConnection Conn, MySqlTransaction Trans, ref MySqlCommand LoadCommand, ref MySqlCommand InsertCommand, ref MySqlCommand UpdateCommand, ref MySqlCommand DeleteCommand) {
				LoadCommand = new MySqlCommand("SELECT PK_DesignObjVariation_DesignObj,FK_DesignObjVariation_Parent,FK_DesignObj_Child FROM DesignObjVariation_DesignObj", Conn);
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new MySqlParameter(PK_DESIGNOBJVARIATION_DESIGNOBJ_PARM, MySqlDbType.Int32,4));

			odbcda.SelectCommand = LoadCommand;
			MySqlCommandBuilder odbcCB = new MySqlCommandBuilder(odbcda);
			odbcCB.RefreshSchema();
			DeleteCommand = odbcCB.GetDeleteCommand();
			InsertCommand = odbcCB.GetInsertCommand();
			UpdateCommand = odbcCB.GetUpdateCommand();
		}

		public DesignObjVariation_DesignObjData LoadDesignObjVariation_DesignObj(System.Int32 PK_DesignObjVariation_DesignObj)
		{
			m_DSCommand.SelectCommand = m_LoadCommand;
			m_DSCommand.SelectCommand.Parameters[PK_DESIGNOBJVARIATION_DESIGNOBJ_PARM].Value = PK_DesignObjVariation_DesignObj;

			m_DSCommand.Fill(this);
			return this;
		}

		public static DataRowCollection LoadDesignObjVariation_DesignObjWithWhere(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans, string WhereClause) // marker:2
		{
			DataRowCollection dr;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["DesignObjVariation_DesignObj"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}
			
			DataSet dsTemp = new MyDataSet();
			dsTemp.Tables.Add(BuildDataTables());
			
			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();
			
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			string sSQL = "SELECT PK_DesignObjVariation_DesignObj, FK_DesignObjVariation_Parent, FK_DesignObj_Child FROM DesignObjVariation_DesignObj WHERE " + WhereClause;
			
			MySqlCommand LoadCommand = new MySqlCommand(sSQL,conn);
			
			if( trans!=null )
				LoadCommand.Transaction = trans;
			
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(dsTemp,"DesignObjVariation_DesignObj");
			
			dr=dsTemp.Tables["DesignObjVariation_DesignObj"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}

		public static DataRow LoadNoCacheDesignObjVariation_DesignObj(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans, System.Int32 PK_DesignObjVariation_DesignObj)
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["DesignObjVariation_DesignObj"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}

			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand;
			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();

			LoadCommand = new MySqlCommand("sp_Select_DesignObjVariation_DesignObj", conn);

			LoadCommand.CommandType = CommandType.StoredProcedure;
			LoadCommand.Parameters.Add(new MySqlParameter(PK_DESIGNOBJVARIATION_DESIGNOBJ_PARM, MySqlDbType.Int32,4));
			LoadCommand.Parameters[PK_DESIGNOBJVARIATION_DESIGNOBJ_PARM].Value = PK_DesignObjVariation_DesignObj;
			if( trans!=null )
				LoadCommand.Transaction = trans;
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(ds,"DesignObjVariation_DesignObj");
				dr = ds.Tables["DesignObjVariation_DesignObj"].Rows.Find(PK_DesignObjVariation_DesignObj);
			return dr;
		}

		public static DataRow LoadDesignObjVariation_DesignObj(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans, System.Int32 PK_DesignObjVariation_DesignObj)  // marker:3
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["DesignObjVariation_DesignObj"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
				else
				dr = dt.Rows.Find(PK_DesignObjVariation_DesignObj);
			}

			if( dr==null )
			{
				MySqlDataAdapter sqlda = new MySqlDataAdapter();
				MySqlCommand LoadCommand;
				if( conn==null )
					conn = HADataConfiguration.GetMySqlConnection();

				LoadCommand = new MySqlCommand("sp_Select_DesignObjVariation_DesignObj", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new MySqlParameter(PK_DESIGNOBJVARIATION_DESIGNOBJ_PARM, MySqlDbType.Int32,4));
				LoadCommand.Parameters[PK_DESIGNOBJVARIATION_DESIGNOBJ_PARM].Value = PK_DesignObjVariation_DesignObj;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(ds,"DesignObjVariation_DesignObj");
				dr = ds.Tables["DesignObjVariation_DesignObj"].Rows.Find(PK_DesignObjVariation_DesignObj);
			}
			return dr;
		}

		public DesignObjVariation_DesignObjData LoadAll() {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new MySqlCommand("SELECT * FROM DesignObjVariation_DesignObj", m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this);
			return this;

		}

		public static DataRowCollection LoadAll(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans) {

			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand = new MySqlCommand("SELECT * FROM DesignObjVariation_DesignObj", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(ds,"DesignObjVariation_DesignObj")==0 )
				return null;
			else
				return ds.Tables["DesignObjVariation_DesignObj"].Rows;

		}

		public DesignObjVariation_DesignObjData ExecuteQuery(String sSQL) {
			return ExecuteQuery(sSQL,DESIGNOBJVARIATION_DESIGNOBJ_TABLE);
		}
		public DesignObjVariation_DesignObjData ExecuteQuery(String sSQL,String sTableName) {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new MySqlCommand(sSQL, m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this,sTableName);

			return this;
		}

		public static DataRowCollection ExecuteQuery(String sSQL,ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans) {
			return ExecuteQuery(sSQL,ref ds,conn,trans,"DesignObjVariation_DesignObj");
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

		public bool UpdateDesignObjVariation_DesignObj(int CurUserID) {
			m_DSCommand.UpdateCommand = m_UpdateCommand;
			m_DSCommand.UpdateCommand.Parameters[USERID_PARM].Value = CurUserID;

			m_DSCommand.InsertCommand = m_InsertCommand;
			m_DSCommand.InsertCommand.Parameters[USERID_PARM].Value = CurUserID;

			if( m_DeleteCommand != null )
			{
				m_DSCommand.DeleteCommand = m_DeleteCommand;
				m_DSCommand.DeleteCommand.Parameters[USERID_PARM].Value = CurUserID;
			}

			m_DSCommand.Update(this, DesignObjVariation_DesignObjData.DESIGNOBJVARIATION_DESIGNOBJ_TABLE);
			return true;
		}

		public static bool UpdateDesignObjVariation_DesignObj(ref MyDataSet ds, int CurUserID)
		{
			MySqlConnection OdbcConn = HADataConfiguration.GetMySqlConnection();
			return UpdateDesignObjVariation_DesignObj(ref ds,CurUserID,OdbcConn,null);
		}

		public static bool UpdateDesignObjVariation_DesignObj(ref MyDataSet ds, int CurUserID,MySqlConnection OdbcConn,MySqlTransaction Trans)
		{
			DataTable dt = ds.Tables[DESIGNOBJVARIATION_DESIGNOBJ_TABLE];
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

	} // public class DesignObjVariation_DesignObjData
	public class DesignObjVariation_DesignObjDataRow
	{
		public DataRow dr = null;
		public DesignObjVariation_DesignObjDataRow(DataRow d)
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
		public System.Int32 fPK_DesignObjVariation_DesignObj
		{
			get
			{
				return Convert.ToInt32(dr[0]);
			}
		}
		public System.Int32 fFK_DesignObjVariation_Parent
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
		public bool fFK_DesignObjVariation_ParentIsNull
		{
			get
			{
				return dr[1]==DBNull.Value;
			}
		}
		public void fFK_DesignObjVariation_ParentSetNull()
		{
			dr[1]=DBNull.Value;
		}
		public DesignObjVariationDataRow fFK_DesignObjVariation_Parent_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tDesignObjVariation[Convert.ToInt32(dr[1])];
			}
		}
		public System.Int32 fFK_DesignObj_Child
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
		public bool fFK_DesignObj_ChildIsNull
		{
			get
			{
				return dr[2]==DBNull.Value;
			}
		}
		public void fFK_DesignObj_ChildSetNull()
		{
			dr[2]=DBNull.Value;
		}
		public DesignObjDataRow fFK_DesignObj_Child_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tDesignObj[Convert.ToInt32(dr[2])];
			}
		}
	} // public class DesignObjVariation_DesignObjDataRow
	public class DesignObjVariation_DesignObjDataReader
	{
		public MySqlDataReader dr;
		bool bCache=false;
		int iRecord=-1,iNumRecords=-1;
		ArrayList al = null;

		public DesignObjVariation_DesignObjDataReader(MySqlDataReader d)
		{
			dr=d;
		}
		public DesignObjVariation_DesignObjDataReader(MySqlCommand cmd)
		{
			dr = cmd.ExecuteReader();
		}

		public DesignObjVariation_DesignObjDataReader(MySqlCommand cmd,bool Cache)
		{
			dr = cmd.ExecuteReader();
			bCache=Cache;
			if( bCache )
				CacheAndClose();
		}

		public DesignObjVariation_DesignObjDataReader(string sSQL)
		{
			MySqlConnection conn = HADataConfiguration.GetMySqlConnection();

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM DesignObjVariation_DesignObj WHERE " + sSQL;
			}

			MySqlCommand cmd = new MySqlCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public DesignObjVariation_DesignObjDataReader(string sSQL,MySqlConnection conn)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetMySqlConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM DesignObjVariation_DesignObj WHERE " + sSQL;
			}

			MySqlCommand cmd = new MySqlCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public DesignObjVariation_DesignObjDataReader(string sSQL,MySqlConnection conn,MySqlTransaction trans,bool Cache)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetMySqlConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM DesignObjVariation_DesignObj WHERE " + sSQL;
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
				object[] objs = new object[22];
				for(int i=0;i<22;i++)
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
		public System.Int32 fPK_DesignObjVariation_DesignObj
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[0]);
				else
					return Convert.ToInt32(dr[0]);
			}
		}
		public System.Int32 fFK_DesignObjVariation_Parent
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[1]);
				else
					return Convert.ToInt32(dr[1]);
			}
		}
		public bool fFK_DesignObjVariation_ParentIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[1]==DBNull.Value;
				else
					return dr[1]==DBNull.Value;
			}
		}
		public System.Int32 fFK_DesignObj_Child
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[2]);
				else
					return Convert.ToInt32(dr[2]);
			}
		}
		public bool fFK_DesignObj_ChildIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[2]==DBNull.Value;
				else
					return dr[2]==DBNull.Value;
			}
		}
	} // public class DesignObjVariation_DesignObjDataReader
	public class DesignObjVariation_DesignObjTable : DataTable
	{
		public DesignObjVariation_DesignObjTable() : base("DesignObjVariation_DesignObj") {}

		public DesignObjVariation_DesignObjDataRow this [System.Int32 PK_DesignObjVariation_DesignObj]
		{
			get
			{
				DesignObjVariation_DesignObjDataRow dr = new DesignObjVariation_DesignObjDataRow(Rows.Find(PK_DesignObjVariation_DesignObj));
				return dr;
			}
		}
		public DataRowCollection LoadAll(MySqlConnection conn, MySqlTransaction trans)
		{
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand = new MySqlCommand("SELECT PK_DesignObjVariation_DesignObj,FK_DesignObjVariation_Parent,FK_DesignObj_Child FROM DesignObjVariation_DesignObj", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(this.DataSet,"DesignObjVariation_DesignObj")==0 )
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
			DesignObjVariation_DesignObjData.UpdateDesignObjVariation_DesignObj(ref ds,PK_Users,conn,trans);
		}
		public DataColumn cPK_DesignObjVariation_DesignObj
		{
			get
			{
				return Columns[0];
			}
		}
		public DataColumn cFK_DesignObjVariation_Parent
		{
			get
			{
				return Columns[1];
			}
		}
		public DataColumn cFK_DesignObj_Child
		{
			get
			{
				return Columns[2];
			}
		}
	}
} // namespace HAData.Common.Data
