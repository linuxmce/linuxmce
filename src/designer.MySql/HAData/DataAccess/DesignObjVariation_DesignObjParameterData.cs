namespace HAData.DataAccess {
	using System;
	using System.Data;
	using MySql;
	using MySql.Data;
	using MySql.Data.MySqlClient;
	using System.Collections;

	using HAData.Common;

	public class DesignObjVariation_DesignObjParameterData : MyDataSet {
		//
		// DesignObjVariation_DesignObjParameter table constants
		//
		public const String DESIGNOBJVARIATION_DESIGNOBJPARAMETER_TABLE = "DesignObjVariation_DesignObjParameter";
		public const String FK_DESIGNOBJVARIATION_FIELD = "FK_DesignObjVariation";
		public const String FK_DESIGNOBJPARAMETER_FIELD = "FK_DesignObjParameter";
		public const String VALUE_FIELD = "Value";
		public const String FK_SKIN_FIELD = "FK_Skin";
		public const String FK_LANGUAGE_FIELD = "FK_Language";
		// table+field constants
		public const String FK_DESIGNOBJVARIATION_TABLE_FIELD = "DesignObjVariation_DesignObjParameter.FK_DesignObjVariation";
		public const String FK_DESIGNOBJPARAMETER_TABLE_FIELD = "DesignObjVariation_DesignObjParameter.FK_DesignObjParameter";
		public const String VALUE_TABLE_FIELD = "DesignObjVariation_DesignObjParameter.Value";
		public const String FK_SKIN_TABLE_FIELD = "DesignObjVariation_DesignObjParameter.FK_Skin";
		public const String FK_LANGUAGE_TABLE_FIELD = "DesignObjVariation_DesignObjParameter.FK_Language";
		// DataSetCommand object
		protected MySqlDataAdapter m_DSCommand;

		// Stored procedure parameters
		protected const String FK_DESIGNOBJVARIATION_PARM = "@FK_DesignObjVariation";
		protected const String FK_DESIGNOBJPARAMETER_PARM = "@FK_DesignObjParameter";
		protected const String VALUE_PARM = "@Value";
		protected const String FK_SKIN_PARM = "@FK_Skin";
		protected const String FK_LANGUAGE_PARM = "@FK_Language";
		protected const String USERID_PARM = "@UserID";

		protected MySqlCommand m_LoadCommand;
		protected MySqlCommand m_InsertCommand;
		protected MySqlCommand m_UpdateCommand;
		protected MySqlCommand m_DeleteCommand;
		protected MySqlConnection m_Connection;
		protected MySqlTransaction m_Transaction;
		public DataTable Table { get { return Tables[0]; } }


		public DesignObjVariation_DesignObjParameterData() {  // marker:1
			//
			// Create the tables in the dataset
			//
			Tables.Add(BuildDataTables());
			m_Connection = HADataConfiguration.GetMySqlConnection();
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new MySqlDataAdapter();

			m_DSCommand.TableMappings.Add("Table", DesignObjVariation_DesignObjParameterData.DESIGNOBJVARIATION_DESIGNOBJPARAMETER_TABLE);
		}

		public DesignObjVariation_DesignObjParameterData(MySqlConnection conn,MySqlTransaction trans) {

			m_Connection = conn;
			m_Transaction = trans;
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new MySqlDataAdapter();

			m_DSCommand.TableMappings.Add("Table", DesignObjVariation_DesignObjParameterData.DESIGNOBJVARIATION_DESIGNOBJPARAMETER_TABLE);
		}

		private DesignObjVariation_DesignObjParameterData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context) {
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
			return (DataTable) BuildDesignObjVariation_DesignObjParameterTable();
		}
		public static DesignObjVariation_DesignObjParameterTable BuildDesignObjVariation_DesignObjParameterTable() {
			//
			// Create the DesignObjVariation_DesignObjParameter table
			//
			DesignObjVariation_DesignObjParameterTable Table = new DesignObjVariation_DesignObjParameterTable();
			DataColumnCollection Columns = Table.Columns;
			DataColumn[] PKColumns = new DataColumn[2];

			DataColumn Column = Columns.Add(FK_DESIGNOBJVARIATION_FIELD, typeof(System.Int32));
			Column.DefaultValue = 0;
			PKColumns[0] = Column;
			Column.DefaultValue = -1;

			Column = Columns.Add(FK_DESIGNOBJPARAMETER_FIELD, typeof(System.Int32));
			Column.DefaultValue = 0;
			PKColumns[1] = Column;
			Column.DefaultValue = -1;

			Columns.Add(VALUE_FIELD, typeof(System.String));
			Columns.Add(FK_SKIN_FIELD, typeof(System.Int32));
			Columns.Add(FK_LANGUAGE_FIELD, typeof(System.Int32));
			Table.PrimaryKey = PKColumns;

			return Table;
		}
		protected static void CreateParameters(MySqlParameterCollection Params, bool IsInsert) {
			Params.Add(new MySqlParameter(FK_DESIGNOBJVARIATION_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(FK_DESIGNOBJPARAMETER_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(VALUE_PARM, MySqlDbType.Text));
			Params.Add(new MySqlParameter(FK_SKIN_PARM, MySqlDbType.Text));
			Params.Add(new MySqlParameter(FK_LANGUAGE_PARM, MySqlDbType.Text));
			Params.Add(new MySqlParameter(USERID_PARM, MySqlDbType.Int32));

			// map the parameters to the data table

			Params[FK_DESIGNOBJVARIATION_PARM].SourceColumn = DesignObjVariation_DesignObjParameterData.FK_DESIGNOBJVARIATION_FIELD;
			Params[FK_DESIGNOBJPARAMETER_PARM].SourceColumn = DesignObjVariation_DesignObjParameterData.FK_DESIGNOBJPARAMETER_FIELD;
			Params[VALUE_PARM].SourceColumn = DesignObjVariation_DesignObjParameterData.VALUE_FIELD;
			Params[FK_SKIN_PARM].SourceColumn = DesignObjVariation_DesignObjParameterData.FK_SKIN_FIELD;
			Params[FK_LANGUAGE_PARM].SourceColumn = DesignObjVariation_DesignObjParameterData.FK_LANGUAGE_FIELD;
		}

		protected static void CreateCommands(MySqlConnection Conn, MySqlTransaction Trans, ref MySqlCommand LoadCommand, ref MySqlCommand InsertCommand, ref MySqlCommand UpdateCommand, ref MySqlCommand DeleteCommand) {
			if(LoadCommand == null) {
				// Create the command since it's null
				LoadCommand = new MySqlCommand("sp_Select_DesignObjVariation_DesignObjParameter", Conn);
				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new MySqlParameter(FK_DESIGNOBJVARIATION_PARM, MySqlDbType.Int32,4));
				LoadCommand.Parameters.Add(new MySqlParameter(FK_DESIGNOBJPARAMETER_PARM, MySqlDbType.Int32,4));
			}

			if(InsertCommand == null) {
				// Create the command since it's null
				InsertCommand = new MySqlCommand("sp_Insert_DesignObjVariation_DesignObjParameter", Conn);
				InsertCommand.CommandType = CommandType.StoredProcedure;
				InsertCommand.Transaction = Trans;

				MySqlParameterCollection Params = InsertCommand.Parameters;

				CreateParameters(Params, true);

			}

			if(UpdateCommand == null) {
				// Create the command since it's null
				UpdateCommand = new MySqlCommand("sp_Update_DesignObjVariation_DesignObjParameter", Conn);
				UpdateCommand.CommandType = CommandType.StoredProcedure;
				UpdateCommand.Transaction = Trans;

				MySqlParameterCollection Params = UpdateCommand.Parameters;

				CreateParameters(Params, false);

			}
			if (DeleteCommand == null)
			{
				DeleteCommand = new MySqlCommand("sp_Delete_DesignObjVariation_DesignObjParameter", Conn);
				DeleteCommand.CommandType = CommandType.StoredProcedure;
				DeleteCommand.Transaction = Trans;

				DeleteCommand.Parameters.Add(FK_DESIGNOBJVARIATION_PARM, MySqlDbType.Int32,4, FK_DESIGNOBJVARIATION_FIELD);
				DeleteCommand.Parameters.Add(FK_DESIGNOBJPARAMETER_PARM, MySqlDbType.Int32,4, FK_DESIGNOBJPARAMETER_FIELD);
				DeleteCommand.Parameters.Add(USERID_PARM, MySqlDbType.Int32);
			}
		}

		protected static void CreateCommands(MySqlDataAdapter odbcda,MySqlConnection Conn, MySqlTransaction Trans, ref MySqlCommand LoadCommand, ref MySqlCommand InsertCommand, ref MySqlCommand UpdateCommand, ref MySqlCommand DeleteCommand) {
				LoadCommand = new MySqlCommand("SELECT FK_DesignObjVariation,FK_DesignObjParameter,Value,FK_Skin,FK_Language FROM DesignObjVariation_DesignObjParameter", Conn);
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new MySqlParameter(FK_DESIGNOBJVARIATION_PARM, MySqlDbType.Int32,4));
				LoadCommand.Parameters.Add(new MySqlParameter(FK_DESIGNOBJPARAMETER_PARM, MySqlDbType.Int32,4));

			odbcda.SelectCommand = LoadCommand;
			MySqlCommandBuilder odbcCB = new MySqlCommandBuilder(odbcda);
			odbcCB.RefreshSchema();
			DeleteCommand = odbcCB.GetDeleteCommand();
			InsertCommand = odbcCB.GetInsertCommand();
			UpdateCommand = odbcCB.GetUpdateCommand();
		}

		public DesignObjVariation_DesignObjParameterData LoadDesignObjVariation_DesignObjParameter(System.Int32 FK_DesignObjVariation, System.Int32 FK_DesignObjParameter)
		{
			m_DSCommand.SelectCommand = m_LoadCommand;
			m_DSCommand.SelectCommand.Parameters[FK_DESIGNOBJVARIATION_PARM].Value = FK_DesignObjVariation;
			m_DSCommand.SelectCommand.Parameters[FK_DESIGNOBJPARAMETER_PARM].Value = FK_DesignObjParameter;

			m_DSCommand.Fill(this);
			return this;
		}

		public static DataRowCollection LoadDesignObjVariation_DesignObjParameterWithWhere(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans, string WhereClause) // marker:2
		{
			DataRowCollection dr;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["DesignObjVariation_DesignObjParameter"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}
			
			DataSet dsTemp = new MyDataSet();
			dsTemp.Tables.Add(BuildDataTables());
			
			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();
			
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			string sSQL = "SELECT FK_DesignObjVariation, FK_DesignObjParameter, Value, FK_Skin, FK_Language FROM DesignObjVariation_DesignObjParameter WHERE " + WhereClause;
			
			MySqlCommand LoadCommand = new MySqlCommand(sSQL,conn);
			
			if( trans!=null )
				LoadCommand.Transaction = trans;
			
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(dsTemp,"DesignObjVariation_DesignObjParameter");
			
			dr=dsTemp.Tables["DesignObjVariation_DesignObjParameter"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}

		public static DataRow LoadNoCacheDesignObjVariation_DesignObjParameter(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans, System.Int32 FK_DesignObjVariation, System.Int32 FK_DesignObjParameter)
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["DesignObjVariation_DesignObjParameter"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}

			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand;
			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();

			LoadCommand = new MySqlCommand("sp_Select_DesignObjVariation_DesignObjParameter", conn);

			LoadCommand.CommandType = CommandType.StoredProcedure;
			LoadCommand.Parameters.Add(new MySqlParameter(FK_DESIGNOBJVARIATION_PARM, MySqlDbType.Int32,4));
			LoadCommand.Parameters.Add(new MySqlParameter(FK_DESIGNOBJPARAMETER_PARM, MySqlDbType.Int32,4));
			LoadCommand.Parameters[FK_DESIGNOBJVARIATION_PARM].Value = FK_DesignObjVariation;
			LoadCommand.Parameters[FK_DESIGNOBJPARAMETER_PARM].Value = FK_DesignObjParameter;
			if( trans!=null )
				LoadCommand.Transaction = trans;
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(ds,"DesignObjVariation_DesignObjParameter");
			object[]findTheseVals = new object[2];
			findTheseVals[0] = FK_DesignObjVariation;
			findTheseVals[1] = FK_DesignObjParameter;
			dr = ds.Tables["DesignObjVariation_DesignObjParameter"].Rows.Find(findTheseVals);
			return dr;
		}

		public static DataRow LoadDesignObjVariation_DesignObjParameter(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans, System.Int32 FK_DesignObjVariation, System.Int32 FK_DesignObjParameter)  // marker:3
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
				findTheseVals[0] = FK_DesignObjVariation;
				findTheseVals[1] = FK_DesignObjParameter;
				DataTable dt = ds.Tables["DesignObjVariation_DesignObjParameter"];
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

				LoadCommand = new MySqlCommand("sp_Select_DesignObjVariation_DesignObjParameter", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new MySqlParameter(FK_DESIGNOBJVARIATION_PARM, MySqlDbType.Int32,4));
				LoadCommand.Parameters.Add(new MySqlParameter(FK_DESIGNOBJPARAMETER_PARM, MySqlDbType.Int32,4));
				LoadCommand.Parameters[FK_DESIGNOBJVARIATION_PARM].Value = FK_DesignObjVariation;
				LoadCommand.Parameters[FK_DESIGNOBJPARAMETER_PARM].Value = FK_DesignObjParameter;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(ds,"DesignObjVariation_DesignObjParameter");
				object[]findTheseVals = new object[2];
				findTheseVals[0] = FK_DesignObjVariation;
				findTheseVals[1] = FK_DesignObjParameter;
				dr = ds.Tables["DesignObjVariation_DesignObjParameter"].Rows.Find(findTheseVals);
			}
			return dr;
		}

		public static DataRowCollection LoadDesignObjVariation_DesignObjParameter_FirstPK(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans,System.Int32 FK_DesignObjVariation)
		{
			DataRowCollection dr;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["DesignObjVariation_DesignObjParameter"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}
			
			DataSet dsTemp = new MyDataSet();
			dsTemp.Tables.Add(BuildDataTables());
			
			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();
			
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
				MySqlCommand LoadCommand;

				LoadCommand = new MySqlCommand("sp_Select_DesignObjVariation_DesignObjParameter_FirstPK", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new MySqlParameter(FK_DESIGNOBJVARIATION_PARM, MySqlDbType.Int32,4));
				LoadCommand.Parameters[FK_DESIGNOBJVARIATION_PARM].Value = FK_DesignObjVariation;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(dsTemp,"DesignObjVariation_DesignObjParameter");
			
			dr=dsTemp.Tables["DesignObjVariation_DesignObjParameter"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}
		public DesignObjVariation_DesignObjParameterData LoadAll() {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new MySqlCommand("SELECT * FROM DesignObjVariation_DesignObjParameter", m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this);
			return this;

		}

		public static DataRowCollection LoadAll(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans) {

			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand = new MySqlCommand("SELECT * FROM DesignObjVariation_DesignObjParameter", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(ds,"DesignObjVariation_DesignObjParameter")==0 )
				return null;
			else
				return ds.Tables["DesignObjVariation_DesignObjParameter"].Rows;

		}

		public DesignObjVariation_DesignObjParameterData ExecuteQuery(String sSQL) {
			return ExecuteQuery(sSQL,DESIGNOBJVARIATION_DESIGNOBJPARAMETER_TABLE);
		}
		public DesignObjVariation_DesignObjParameterData ExecuteQuery(String sSQL,String sTableName) {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new MySqlCommand(sSQL, m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this,sTableName);

			return this;
		}

		public static DataRowCollection ExecuteQuery(String sSQL,ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans) {
			return ExecuteQuery(sSQL,ref ds,conn,trans,"DesignObjVariation_DesignObjParameter");
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

		public bool UpdateDesignObjVariation_DesignObjParameter(int CurUserID) {
			m_DSCommand.UpdateCommand = m_UpdateCommand;
			m_DSCommand.UpdateCommand.Parameters[USERID_PARM].Value = CurUserID;

			m_DSCommand.InsertCommand = m_InsertCommand;
			m_DSCommand.InsertCommand.Parameters[USERID_PARM].Value = CurUserID;

			if( m_DeleteCommand != null )
			{
				m_DSCommand.DeleteCommand = m_DeleteCommand;
				m_DSCommand.DeleteCommand.Parameters[USERID_PARM].Value = CurUserID;
			}

			m_DSCommand.Update(this, DesignObjVariation_DesignObjParameterData.DESIGNOBJVARIATION_DESIGNOBJPARAMETER_TABLE);
			return true;
		}

		public static bool UpdateDesignObjVariation_DesignObjParameter(ref MyDataSet ds, int CurUserID)
		{
			MySqlConnection OdbcConn = HADataConfiguration.GetMySqlConnection();
			return UpdateDesignObjVariation_DesignObjParameter(ref ds,CurUserID,OdbcConn,null);
		}

		public static bool UpdateDesignObjVariation_DesignObjParameter(ref MyDataSet ds, int CurUserID,MySqlConnection OdbcConn,MySqlTransaction Trans)
		{
			DataTable dt = ds.Tables[DESIGNOBJVARIATION_DESIGNOBJPARAMETER_TABLE];
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

	} // public class DesignObjVariation_DesignObjParameterData
	public class DesignObjVariation_DesignObjParameterDataRow
	{
		public DataRow dr = null;
		public DesignObjVariation_DesignObjParameterDataRow(DataRow d)
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
		public System.Int32 fFK_DesignObjVariation
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
		public DesignObjVariationDataRow fFK_DesignObjVariation_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tDesignObjVariation[Convert.ToInt32(dr[0])];
			}
		}
		public System.Int32 fFK_DesignObjParameter
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
		public DesignObjParameterDataRow fFK_DesignObjParameter_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tDesignObjParameter[Convert.ToInt32(dr[1])];
			}
		}
		public System.String fValue
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
		public bool fValueIsNull
		{
			get
			{
				return dr[2]==DBNull.Value;
			}
		}
		public void fValueSetNull()
		{
			dr[2]=DBNull.Value;
		}
		public System.Int32 fFK_Skin
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
		public bool fFK_SkinIsNull
		{
			get
			{
				return dr[3]==DBNull.Value;
			}
		}
		public void fFK_SkinSetNull()
		{
			dr[3]=DBNull.Value;
		}
		public System.Int32 fFK_Language
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
		public bool fFK_LanguageIsNull
		{
			get
			{
				return dr[4]==DBNull.Value;
			}
		}
		public void fFK_LanguageSetNull()
		{
			dr[4]=DBNull.Value;
		}
	} // public class DesignObjVariation_DesignObjParameterDataRow
	public class DesignObjVariation_DesignObjParameterDataReader
	{
		public MySqlDataReader dr;
		bool bCache=false;
		int iRecord=-1,iNumRecords=-1;
		ArrayList al = null;

		public DesignObjVariation_DesignObjParameterDataReader(MySqlDataReader d)
		{
			dr=d;
		}
		public DesignObjVariation_DesignObjParameterDataReader(MySqlCommand cmd)
		{
			dr = cmd.ExecuteReader();
		}

		public DesignObjVariation_DesignObjParameterDataReader(MySqlCommand cmd,bool Cache)
		{
			dr = cmd.ExecuteReader();
			bCache=Cache;
			if( bCache )
				CacheAndClose();
		}

		public DesignObjVariation_DesignObjParameterDataReader(string sSQL)
		{
			MySqlConnection conn = HADataConfiguration.GetMySqlConnection();

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM DesignObjVariation_DesignObjParameter WHERE " + sSQL;
			}

			MySqlCommand cmd = new MySqlCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public DesignObjVariation_DesignObjParameterDataReader(string sSQL,MySqlConnection conn)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetMySqlConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM DesignObjVariation_DesignObjParameter WHERE " + sSQL;
			}

			MySqlCommand cmd = new MySqlCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public DesignObjVariation_DesignObjParameterDataReader(string sSQL,MySqlConnection conn,MySqlTransaction trans,bool Cache)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetMySqlConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM DesignObjVariation_DesignObjParameter WHERE " + sSQL;
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
		public System.Int32 fFK_DesignObjVariation
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[0]);
				else
					return Convert.ToInt32(dr[0]);
			}
		}
		public System.Int32 fFK_DesignObjParameter
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[1]);
				else
					return Convert.ToInt32(dr[1]);
			}
		}
		public System.String fValue
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[2]);
				else
					return Convert.ToString(dr[2]);
			}
		}
		public bool fValueIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[2]==DBNull.Value;
				else
					return dr[2]==DBNull.Value;
			}
		}
	} // public class DesignObjVariation_DesignObjParameterDataReader
	public class DesignObjVariation_DesignObjParameterTable : DataTable
	{
		public DesignObjVariation_DesignObjParameterTable() : base("DesignObjVariation_DesignObjParameter") {}

		public DesignObjVariation_DesignObjParameterDataRow this [System.Int32 FK_DesignObjVariation, System.Int32 FK_DesignObjParameter]
		{
			get
			{
				object[]findTheseVals = new object[2];
				findTheseVals[0] = FK_DesignObjVariation;
				findTheseVals[1] = FK_DesignObjParameter;
				DesignObjVariation_DesignObjParameterDataRow dr = new DesignObjVariation_DesignObjParameterDataRow(Rows.Find(findTheseVals));
				return dr;
			}
		}
		public DataRowCollection LoadAll(MySqlConnection conn, MySqlTransaction trans)
		{
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand = new MySqlCommand("SELECT FK_DesignObjVariation,FK_DesignObjParameter,Value FROM DesignObjVariation_DesignObjParameter", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(this.DataSet,"DesignObjVariation_DesignObjParameter")==0 )
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
			DesignObjVariation_DesignObjParameterData.UpdateDesignObjVariation_DesignObjParameter(ref ds,PK_Users,conn,trans);
		}
		public DataColumn cFK_DesignObjVariation
		{
			get
			{
				return Columns[0];
			}
		}
		public DataColumn cFK_DesignObjParameter
		{
			get
			{
				return Columns[1];
			}
		}
		public DataColumn cValue
		{
			get
			{
				return Columns[2];
			}
		}
	}
} // namespace HAData.Common.Data
