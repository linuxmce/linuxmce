namespace HAData.DataAccess {
	using System;
	using System.Data;
	using Microsoft.Data.Odbc;
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
		protected OdbcDataAdapter m_DSCommand;

		// Stored procedure parameters
		protected const String FK_DESIGNOBJVARIATION_PARM = "@FK_DesignObjVariation";
		protected const String FK_DESIGNOBJPARAMETER_PARM = "@FK_DesignObjParameter";
		protected const String VALUE_PARM = "@Value";
		protected const String FK_SKIN_PARM = "@FK_Skin";
		protected const String FK_LANGUAGE_PARM = "@FK_Language";
		protected const String USERID_PARM = "@UserID";

		protected OdbcCommand m_LoadCommand;
		protected OdbcCommand m_InsertCommand;
		protected OdbcCommand m_UpdateCommand;
		protected OdbcCommand m_DeleteCommand;
		protected OdbcConnection m_Connection;
		protected OdbcTransaction m_Transaction;
		public DataTable Table { get { return Tables[0]; } }


		public DesignObjVariation_DesignObjParameterData() {  // marker:1
			//
			// Create the tables in the dataset
			//
			Tables.Add(BuildDataTables());
			m_Connection = HADataConfiguration.GetOdbcConnection();
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

			m_DSCommand.TableMappings.Add("Table", DesignObjVariation_DesignObjParameterData.DESIGNOBJVARIATION_DESIGNOBJPARAMETER_TABLE);
		}

		public DesignObjVariation_DesignObjParameterData(OdbcConnection conn,OdbcTransaction trans) {

			m_Connection = conn;
			m_Transaction = trans;
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

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
		protected static void CreateParameters(OdbcParameterCollection Params, bool IsInsert) {
			Params.Add(new OdbcParameter(FK_DESIGNOBJVARIATION_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_DESIGNOBJPARAMETER_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(VALUE_PARM, OdbcType.Text));
			Params.Add(new OdbcParameter(FK_SKIN_PARM, OdbcType.Text));
			Params.Add(new OdbcParameter(FK_LANGUAGE_PARM, OdbcType.Text));
			Params.Add(new OdbcParameter(USERID_PARM, OdbcType.Int));

			// map the parameters to the data table

			Params[FK_DESIGNOBJVARIATION_PARM].SourceColumn = DesignObjVariation_DesignObjParameterData.FK_DESIGNOBJVARIATION_FIELD;
			Params[FK_DESIGNOBJPARAMETER_PARM].SourceColumn = DesignObjVariation_DesignObjParameterData.FK_DESIGNOBJPARAMETER_FIELD;
			Params[VALUE_PARM].SourceColumn = DesignObjVariation_DesignObjParameterData.VALUE_FIELD;
			Params[FK_SKIN_PARM].SourceColumn = DesignObjVariation_DesignObjParameterData.FK_SKIN_FIELD;
			Params[FK_LANGUAGE_PARM].SourceColumn = DesignObjVariation_DesignObjParameterData.FK_LANGUAGE_FIELD;
		}

		protected static void CreateCommands(OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
			if(LoadCommand == null) {
				// Create the command since it's null
				LoadCommand = new OdbcCommand("sp_Select_DesignObjVariation_DesignObjParameter", Conn);
				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(FK_DESIGNOBJVARIATION_PARM, OdbcType.Int,4));
				LoadCommand.Parameters.Add(new OdbcParameter(FK_DESIGNOBJPARAMETER_PARM, OdbcType.Int,4));
			}

			if(InsertCommand == null) {
				// Create the command since it's null
				InsertCommand = new OdbcCommand("sp_Insert_DesignObjVariation_DesignObjParameter", Conn);
				InsertCommand.CommandType = CommandType.StoredProcedure;
				InsertCommand.Transaction = Trans;

				OdbcParameterCollection Params = InsertCommand.Parameters;

				CreateParameters(Params, true);

			}

			if(UpdateCommand == null) {
				// Create the command since it's null
				UpdateCommand = new OdbcCommand("sp_Update_DesignObjVariation_DesignObjParameter", Conn);
				UpdateCommand.CommandType = CommandType.StoredProcedure;
				UpdateCommand.Transaction = Trans;

				OdbcParameterCollection Params = UpdateCommand.Parameters;

				CreateParameters(Params, false);

			}
			if (DeleteCommand == null)
			{
				DeleteCommand = new OdbcCommand("sp_Delete_DesignObjVariation_DesignObjParameter", Conn);
				DeleteCommand.CommandType = CommandType.StoredProcedure;
				DeleteCommand.Transaction = Trans;

				DeleteCommand.Parameters.Add(FK_DESIGNOBJVARIATION_PARM, OdbcType.Int,4, FK_DESIGNOBJVARIATION_FIELD);
				DeleteCommand.Parameters.Add(FK_DESIGNOBJPARAMETER_PARM, OdbcType.Int,4, FK_DESIGNOBJPARAMETER_FIELD);
				DeleteCommand.Parameters.Add(USERID_PARM, OdbcType.Int);
			}
		}

		protected static void CreateCommands(OdbcDataAdapter odbcda,OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
				LoadCommand = new OdbcCommand("SELECT FK_DesignObjVariation,FK_DesignObjParameter,Value,FK_Skin,FK_Language FROM DesignObjVariation_DesignObjParameter", Conn);
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(FK_DESIGNOBJVARIATION_PARM, OdbcType.Int,4));
				LoadCommand.Parameters.Add(new OdbcParameter(FK_DESIGNOBJPARAMETER_PARM, OdbcType.Int,4));

			odbcda.SelectCommand = LoadCommand;
			OdbcCommandBuilder odbcCB = new OdbcCommandBuilder(odbcda);
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

		public static DataRowCollection LoadDesignObjVariation_DesignObjParameterWithWhere(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, string WhereClause) // marker:2
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
				conn = HADataConfiguration.GetOdbcConnection();
			
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			string sSQL = "SELECT FK_DesignObjVariation, FK_DesignObjParameter, Value, FK_Skin, FK_Language FROM DesignObjVariation_DesignObjParameter WHERE " + WhereClause;
			
			OdbcCommand LoadCommand = new OdbcCommand(sSQL,conn);
			
			if( trans!=null )
				LoadCommand.Transaction = trans;
			
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(dsTemp,"DesignObjVariation_DesignObjParameter");
			
			dr=dsTemp.Tables["DesignObjVariation_DesignObjParameter"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}

		public static DataRow LoadNoCacheDesignObjVariation_DesignObjParameter(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 FK_DesignObjVariation, System.Int32 FK_DesignObjParameter)
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

			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand;
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();

			LoadCommand = new OdbcCommand("sp_Select_DesignObjVariation_DesignObjParameter", conn);

			LoadCommand.CommandType = CommandType.StoredProcedure;
			LoadCommand.Parameters.Add(new OdbcParameter(FK_DESIGNOBJVARIATION_PARM, OdbcType.Int,4));
			LoadCommand.Parameters.Add(new OdbcParameter(FK_DESIGNOBJPARAMETER_PARM, OdbcType.Int,4));
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

		public static DataRow LoadDesignObjVariation_DesignObjParameter(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 FK_DesignObjVariation, System.Int32 FK_DesignObjParameter)  // marker:3
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
				OdbcDataAdapter sqlda = new OdbcDataAdapter();
				OdbcCommand LoadCommand;
				if( conn==null )
					conn = HADataConfiguration.GetOdbcConnection();

				LoadCommand = new OdbcCommand("sp_Select_DesignObjVariation_DesignObjParameter", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new OdbcParameter(FK_DESIGNOBJVARIATION_PARM, OdbcType.Int,4));
				LoadCommand.Parameters.Add(new OdbcParameter(FK_DESIGNOBJPARAMETER_PARM, OdbcType.Int,4));
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

		public static DataRowCollection LoadDesignObjVariation_DesignObjParameter_FirstPK(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans,System.Int32 FK_DesignObjVariation)
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
				conn = HADataConfiguration.GetOdbcConnection();
			
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
				OdbcCommand LoadCommand;

				LoadCommand = new OdbcCommand("sp_Select_DesignObjVariation_DesignObjParameter_FirstPK", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new OdbcParameter(FK_DESIGNOBJVARIATION_PARM, OdbcType.Int,4));
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
			m_DSCommand.SelectCommand = new OdbcCommand("SELECT * FROM DesignObjVariation_DesignObjParameter", m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this);
			return this;

		}

		public static DataRowCollection LoadAll(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {

			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT * FROM DesignObjVariation_DesignObjParameter", conn);
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
			m_DSCommand.SelectCommand = new OdbcCommand(sSQL, m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this,sTableName);

			return this;
		}

		public static DataRowCollection ExecuteQuery(String sSQL,ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {
			return ExecuteQuery(sSQL,ref ds,conn,trans,"DesignObjVariation_DesignObjParameter");
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
			OdbcConnection OdbcConn = HADataConfiguration.GetOdbcConnection();
			return UpdateDesignObjVariation_DesignObjParameter(ref ds,CurUserID,OdbcConn,null);
		}

		public static bool UpdateDesignObjVariation_DesignObjParameter(ref MyDataSet ds, int CurUserID,OdbcConnection OdbcConn,OdbcTransaction Trans)
		{
			DataTable dt = ds.Tables[DESIGNOBJVARIATION_DESIGNOBJPARAMETER_TABLE];
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
		public OdbcDataReader dr;
		bool bCache=false;
		int iRecord=-1,iNumRecords=-1;
		ArrayList al = null;

		public DesignObjVariation_DesignObjParameterDataReader(OdbcDataReader d)
		{
			dr=d;
		}
		public DesignObjVariation_DesignObjParameterDataReader(OdbcCommand cmd)
		{
			dr = cmd.ExecuteReader();
		}

		public DesignObjVariation_DesignObjParameterDataReader(OdbcCommand cmd,bool Cache)
		{
			dr = cmd.ExecuteReader();
			bCache=Cache;
			if( bCache )
				CacheAndClose();
		}

		public DesignObjVariation_DesignObjParameterDataReader(string sSQL)
		{
			OdbcConnection conn = HADataConfiguration.GetOdbcConnection();

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM DesignObjVariation_DesignObjParameter WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public DesignObjVariation_DesignObjParameterDataReader(string sSQL,OdbcConnection conn)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM DesignObjVariation_DesignObjParameter WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public DesignObjVariation_DesignObjParameterDataReader(string sSQL,OdbcConnection conn,OdbcTransaction trans,bool Cache)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM DesignObjVariation_DesignObjParameter WHERE " + sSQL;
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
				if( !dr.bIsValid  && false /* can't do this with ODBC */  )
				{
					MyDataSet mds = (MyDataSet) DataSet;
					if( mds.m_conn==null )
						return dr;
					OdbcDataAdapter sqlda = new OdbcDataAdapter();
					OdbcCommand LoadCommand = new OdbcCommand("sp_Select_DesignObjVariation_DesignObjParameter", mds.m_conn,mds.m_trans);
					LoadCommand.CommandType = CommandType.StoredProcedure;
					LoadCommand.Parameters.Add(new OdbcParameter("@FK_DesignObjVariation", OdbcType.Int,4));
					LoadCommand.Parameters.Add(new OdbcParameter("@FK_DesignObjParameter", OdbcType.Int,4));
					LoadCommand.Parameters["@FK_DesignObjVariation"].Value = FK_DesignObjVariation;
					LoadCommand.Parameters["@FK_DesignObjParameter"].Value = FK_DesignObjParameter;
					sqlda.SelectCommand = LoadCommand;
					sqlda.Fill(mds,"DesignObjVariation_DesignObjParameter");
					dr = new DesignObjVariation_DesignObjParameterDataRow(Rows.Find(findTheseVals));
				}
				return dr;
			}
		}
		public DataRowCollection LoadAll(OdbcConnection conn, OdbcTransaction trans)
		{
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT FK_DesignObjVariation,FK_DesignObjParameter,Value FROM DesignObjVariation_DesignObjParameter", conn);
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
		public void Update(int PK_Users,OdbcConnection conn, OdbcTransaction trans)
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
