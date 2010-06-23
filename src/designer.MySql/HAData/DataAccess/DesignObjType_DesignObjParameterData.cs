namespace HAData.DataAccess {
	using System;
	using System.Data;
	using Microsoft.Data.Odbc;
	using System.Collections;

	using HAData.Common;

	public class DesignObjType_DesignObjParameterData : MyDataSet {
		//
		// DesignObjType_DesignObjParameter table constants
		//
		public const String DESIGNOBJTYPE_DESIGNOBJPARAMETER_TABLE = "DesignObjType_DesignObjParameter";
		public const String FK_DESIGNOBJTYPE_FIELD = "FK_DesignObjType";
		public const String FK_DESIGNOBJPARAMETER_FIELD = "FK_DesignObjParameter";
		// table+field constants
		public const String FK_DESIGNOBJTYPE_TABLE_FIELD = "DesignObjType_DesignObjParameter.FK_DesignObjType";
		public const String FK_DESIGNOBJPARAMETER_TABLE_FIELD = "DesignObjType_DesignObjParameter.FK_DesignObjParameter";
		// DataSetCommand object
		protected OdbcDataAdapter m_DSCommand;

		// Stored procedure parameters
		protected const String FK_DESIGNOBJTYPE_PARM = "@FK_DesignObjType";
		protected const String FK_DESIGNOBJPARAMETER_PARM = "@FK_DesignObjParameter";
		protected const String USERID_PARM = "@UserID";

		protected OdbcCommand m_LoadCommand;
		protected OdbcCommand m_InsertCommand;
		protected OdbcCommand m_UpdateCommand;
		protected OdbcCommand m_DeleteCommand;
		protected OdbcConnection m_Connection;
		protected OdbcTransaction m_Transaction;
		public DataTable Table { get { return Tables[0]; } }


		public DesignObjType_DesignObjParameterData() {  // marker:1
			//
			// Create the tables in the dataset
			//
			Tables.Add(BuildDataTables());
			m_Connection = HADataConfiguration.GetOdbcConnection();
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

			m_DSCommand.TableMappings.Add("Table", DesignObjType_DesignObjParameterData.DESIGNOBJTYPE_DESIGNOBJPARAMETER_TABLE);
		}

		public DesignObjType_DesignObjParameterData(OdbcConnection conn,OdbcTransaction trans) {

			m_Connection = conn;
			m_Transaction = trans;
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

			m_DSCommand.TableMappings.Add("Table", DesignObjType_DesignObjParameterData.DESIGNOBJTYPE_DESIGNOBJPARAMETER_TABLE);
		}

		private DesignObjType_DesignObjParameterData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context) {
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
			return (DataTable) BuildDesignObjType_DesignObjParameterTable();
		}
		public static DesignObjType_DesignObjParameterTable BuildDesignObjType_DesignObjParameterTable() {
			//
			// Create the DesignObjType_DesignObjParameter table
			//
			DesignObjType_DesignObjParameterTable Table = new DesignObjType_DesignObjParameterTable();
			DataColumnCollection Columns = Table.Columns;
			DataColumn[] PKColumns = new DataColumn[2];

			DataColumn Column = Columns.Add(FK_DESIGNOBJTYPE_FIELD, typeof(System.Int32));
			Column.DefaultValue = 0;
			PKColumns[0] = Column;
			Column.DefaultValue = -1;

			Column = Columns.Add(FK_DESIGNOBJPARAMETER_FIELD, typeof(System.Int32));
			Column.DefaultValue = 0;
			PKColumns[1] = Column;
			Column.DefaultValue = -1;

			Table.PrimaryKey = PKColumns;

			return Table;
		}
		protected static void CreateParameters(OdbcParameterCollection Params, bool IsInsert) {
			Params.Add(new OdbcParameter(FK_DESIGNOBJTYPE_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_DESIGNOBJPARAMETER_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(USERID_PARM, OdbcType.Int));

			// map the parameters to the data table

			Params[FK_DESIGNOBJTYPE_PARM].SourceColumn = DesignObjType_DesignObjParameterData.FK_DESIGNOBJTYPE_FIELD;
			Params[FK_DESIGNOBJPARAMETER_PARM].SourceColumn = DesignObjType_DesignObjParameterData.FK_DESIGNOBJPARAMETER_FIELD;
		}

		protected static void CreateCommands(OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
			if(LoadCommand == null) {
				// Create the command since it's null
				LoadCommand = new OdbcCommand("sp_Select_DesignObjType_DesignObjParameter", Conn);
				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(FK_DESIGNOBJTYPE_PARM, OdbcType.Int,4));
				LoadCommand.Parameters.Add(new OdbcParameter(FK_DESIGNOBJPARAMETER_PARM, OdbcType.Int,4));
			}

			if(InsertCommand == null) {
				// Create the command since it's null
				InsertCommand = new OdbcCommand("sp_Insert_DesignObjType_DesignObjParameter", Conn);
				InsertCommand.CommandType = CommandType.StoredProcedure;
				InsertCommand.Transaction = Trans;

				OdbcParameterCollection Params = InsertCommand.Parameters;

				CreateParameters(Params, true);

			}

			if(UpdateCommand == null) {
				// Create the command since it's null
				UpdateCommand = new OdbcCommand("sp_Update_DesignObjType_DesignObjParameter", Conn);
				UpdateCommand.CommandType = CommandType.StoredProcedure;
				UpdateCommand.Transaction = Trans;

				OdbcParameterCollection Params = UpdateCommand.Parameters;

				CreateParameters(Params, false);

			}
			if (DeleteCommand == null)
			{
				DeleteCommand = new OdbcCommand("sp_Delete_DesignObjType_DesignObjParameter", Conn);
				DeleteCommand.CommandType = CommandType.StoredProcedure;
				DeleteCommand.Transaction = Trans;

				DeleteCommand.Parameters.Add(FK_DESIGNOBJTYPE_PARM, OdbcType.Int,4, FK_DESIGNOBJTYPE_FIELD);
				DeleteCommand.Parameters.Add(FK_DESIGNOBJPARAMETER_PARM, OdbcType.Int,4, FK_DESIGNOBJPARAMETER_FIELD);
				DeleteCommand.Parameters.Add(USERID_PARM, OdbcType.Int);
			}
		}

		protected static void CreateCommands(OdbcDataAdapter odbcda,OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
				LoadCommand = new OdbcCommand("SELECT FK_DesignObjType,FK_DesignObjParameter FROM DesignObjType_DesignObjParameter", Conn);
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(FK_DESIGNOBJTYPE_PARM, OdbcType.Int,4));
				LoadCommand.Parameters.Add(new OdbcParameter(FK_DESIGNOBJPARAMETER_PARM, OdbcType.Int,4));

			odbcda.SelectCommand = LoadCommand;
			OdbcCommandBuilder odbcCB = new OdbcCommandBuilder(odbcda);
			odbcCB.RefreshSchema();
			DeleteCommand = odbcCB.GetDeleteCommand();
			InsertCommand = odbcCB.GetInsertCommand();
			UpdateCommand = odbcCB.GetUpdateCommand();
		}

		public DesignObjType_DesignObjParameterData LoadDesignObjType_DesignObjParameter(System.Int32 FK_DesignObjType, System.Int32 FK_DesignObjParameter)
		{
			m_DSCommand.SelectCommand = m_LoadCommand;
			m_DSCommand.SelectCommand.Parameters[FK_DESIGNOBJTYPE_PARM].Value = FK_DesignObjType;
			m_DSCommand.SelectCommand.Parameters[FK_DESIGNOBJPARAMETER_PARM].Value = FK_DesignObjParameter;

			m_DSCommand.Fill(this);
			return this;
		}

		public static DataRowCollection LoadDesignObjType_DesignObjParameterWithWhere(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, string WhereClause) // marker:2
		{
			DataRowCollection dr;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["DesignObjType_DesignObjParameter"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}
			
			DataSet dsTemp = new MyDataSet();
			dsTemp.Tables.Add(BuildDataTables());
			
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			string sSQL = "SELECT FK_DesignObjType, FK_DesignObjParameter FROM DesignObjType_DesignObjParameter WHERE " + WhereClause;
			
			OdbcCommand LoadCommand = new OdbcCommand(sSQL,conn);
			
			if( trans!=null )
				LoadCommand.Transaction = trans;
			
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(dsTemp,"DesignObjType_DesignObjParameter");
			
			dr=dsTemp.Tables["DesignObjType_DesignObjParameter"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}

		public static DataRow LoadNoCacheDesignObjType_DesignObjParameter(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 FK_DesignObjType, System.Int32 FK_DesignObjParameter)
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["DesignObjType_DesignObjParameter"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}

			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand;
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();

			LoadCommand = new OdbcCommand("sp_Select_DesignObjType_DesignObjParameter", conn);

			LoadCommand.CommandType = CommandType.StoredProcedure;
			LoadCommand.Parameters.Add(new OdbcParameter(FK_DESIGNOBJTYPE_PARM, OdbcType.Int,4));
			LoadCommand.Parameters.Add(new OdbcParameter(FK_DESIGNOBJPARAMETER_PARM, OdbcType.Int,4));
			LoadCommand.Parameters[FK_DESIGNOBJTYPE_PARM].Value = FK_DesignObjType;
			LoadCommand.Parameters[FK_DESIGNOBJPARAMETER_PARM].Value = FK_DesignObjParameter;
			if( trans!=null )
				LoadCommand.Transaction = trans;
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(ds,"DesignObjType_DesignObjParameter");
			object[]findTheseVals = new object[2];
			findTheseVals[0] = FK_DesignObjType;
			findTheseVals[1] = FK_DesignObjParameter;
			dr = ds.Tables["DesignObjType_DesignObjParameter"].Rows.Find(findTheseVals);
			return dr;
		}

		public static DataRow LoadDesignObjType_DesignObjParameter(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 FK_DesignObjType, System.Int32 FK_DesignObjParameter)  // marker:3
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
				findTheseVals[0] = FK_DesignObjType;
				findTheseVals[1] = FK_DesignObjParameter;
				DataTable dt = ds.Tables["DesignObjType_DesignObjParameter"];
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

				LoadCommand = new OdbcCommand("sp_Select_DesignObjType_DesignObjParameter", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new OdbcParameter(FK_DESIGNOBJTYPE_PARM, OdbcType.Int,4));
				LoadCommand.Parameters.Add(new OdbcParameter(FK_DESIGNOBJPARAMETER_PARM, OdbcType.Int,4));
				LoadCommand.Parameters[FK_DESIGNOBJTYPE_PARM].Value = FK_DesignObjType;
				LoadCommand.Parameters[FK_DESIGNOBJPARAMETER_PARM].Value = FK_DesignObjParameter;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(ds,"DesignObjType_DesignObjParameter");
				object[]findTheseVals = new object[2];
				findTheseVals[0] = FK_DesignObjType;
				findTheseVals[1] = FK_DesignObjParameter;
				dr = ds.Tables["DesignObjType_DesignObjParameter"].Rows.Find(findTheseVals);
			}
			return dr;
		}

		public static DataRowCollection LoadDesignObjType_DesignObjParameter_FirstPK(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans,System.Int32 FK_DesignObjType)
		{
			DataRowCollection dr;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["DesignObjType_DesignObjParameter"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}
			
			DataSet dsTemp = new MyDataSet();
			dsTemp.Tables.Add(BuildDataTables());
			
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
				OdbcCommand LoadCommand;

				LoadCommand = new OdbcCommand("sp_Select_DesignObjType_DesignObjParameter_FirstPK", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new OdbcParameter(FK_DESIGNOBJTYPE_PARM, OdbcType.Int,4));
				LoadCommand.Parameters[FK_DESIGNOBJTYPE_PARM].Value = FK_DesignObjType;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(dsTemp,"DesignObjType_DesignObjParameter");
			
			dr=dsTemp.Tables["DesignObjType_DesignObjParameter"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}
		public DesignObjType_DesignObjParameterData LoadAll() {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new OdbcCommand("SELECT * FROM DesignObjType_DesignObjParameter", m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this);
			return this;

		}

		public static DataRowCollection LoadAll(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {

			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT * FROM DesignObjType_DesignObjParameter", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(ds,"DesignObjType_DesignObjParameter")==0 )
				return null;
			else
				return ds.Tables["DesignObjType_DesignObjParameter"].Rows;

		}

		public DesignObjType_DesignObjParameterData ExecuteQuery(String sSQL) {
			return ExecuteQuery(sSQL,DESIGNOBJTYPE_DESIGNOBJPARAMETER_TABLE);
		}
		public DesignObjType_DesignObjParameterData ExecuteQuery(String sSQL,String sTableName) {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new OdbcCommand(sSQL, m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this,sTableName);

			return this;
		}

		public static DataRowCollection ExecuteQuery(String sSQL,ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {
			return ExecuteQuery(sSQL,ref ds,conn,trans,"DesignObjType_DesignObjParameter");
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

		public bool UpdateDesignObjType_DesignObjParameter(int CurUserID) {
			m_DSCommand.UpdateCommand = m_UpdateCommand;
			m_DSCommand.UpdateCommand.Parameters[USERID_PARM].Value = CurUserID;

			m_DSCommand.InsertCommand = m_InsertCommand;
			m_DSCommand.InsertCommand.Parameters[USERID_PARM].Value = CurUserID;

			if( m_DeleteCommand != null )
			{
				m_DSCommand.DeleteCommand = m_DeleteCommand;
				m_DSCommand.DeleteCommand.Parameters[USERID_PARM].Value = CurUserID;
			}

			m_DSCommand.Update(this, DesignObjType_DesignObjParameterData.DESIGNOBJTYPE_DESIGNOBJPARAMETER_TABLE);
			return true;
		}

		public static bool UpdateDesignObjType_DesignObjParameter(ref MyDataSet ds, int CurUserID)
		{
			OdbcConnection OdbcConn = HADataConfiguration.GetOdbcConnection();
			return UpdateDesignObjType_DesignObjParameter(ref ds,CurUserID,OdbcConn,null);
		}

		public static bool UpdateDesignObjType_DesignObjParameter(ref MyDataSet ds, int CurUserID,OdbcConnection OdbcConn,OdbcTransaction Trans)
		{
			DataTable dt = ds.Tables[DESIGNOBJTYPE_DESIGNOBJPARAMETER_TABLE];
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

	} // public class DesignObjType_DesignObjParameterData
	public class DesignObjType_DesignObjParameterDataRow
	{
		public DataRow dr = null;
		public DesignObjType_DesignObjParameterDataRow(DataRow d)
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
		public System.Int32 fFK_DesignObjType
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
		public DesignObjTypeDataRow fFK_DesignObjType_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tDesignObjType[Convert.ToInt32(dr[0])];
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
	} // public class DesignObjType_DesignObjParameterDataRow
	public class DesignObjType_DesignObjParameterDataReader
	{
		public OdbcDataReader dr;
		bool bCache=false;
		int iRecord=-1,iNumRecords=-1;
		ArrayList al = null;

		public DesignObjType_DesignObjParameterDataReader(OdbcDataReader d)
		{
			dr=d;
		}
		public DesignObjType_DesignObjParameterDataReader(OdbcCommand cmd)
		{
			dr = cmd.ExecuteReader();
		}

		public DesignObjType_DesignObjParameterDataReader(OdbcCommand cmd,bool Cache)
		{
			dr = cmd.ExecuteReader();
			bCache=Cache;
			if( bCache )
				CacheAndClose();
		}

		public DesignObjType_DesignObjParameterDataReader(string sSQL)
		{
			OdbcConnection conn = HADataConfiguration.GetOdbcConnection();

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM DesignObjType_DesignObjParameter WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public DesignObjType_DesignObjParameterDataReader(string sSQL,OdbcConnection conn)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM DesignObjType_DesignObjParameter WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public DesignObjType_DesignObjParameterDataReader(string sSQL,OdbcConnection conn,OdbcTransaction trans,bool Cache)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM DesignObjType_DesignObjParameter WHERE " + sSQL;
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
				object[] objs = new object[2];
				for(int i=0;i<2;i++)
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
		public System.Int32 fFK_DesignObjType
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
	} // public class DesignObjType_DesignObjParameterDataReader
	public class DesignObjType_DesignObjParameterTable : DataTable
	{
		public DesignObjType_DesignObjParameterTable() : base("DesignObjType_DesignObjParameter") {}

		public DesignObjType_DesignObjParameterDataRow this [System.Int32 FK_DesignObjType, System.Int32 FK_DesignObjParameter]
		{
			get
			{
				object[]findTheseVals = new object[2];
				findTheseVals[0] = FK_DesignObjType;
				findTheseVals[1] = FK_DesignObjParameter;
				DesignObjType_DesignObjParameterDataRow dr = new DesignObjType_DesignObjParameterDataRow(Rows.Find(findTheseVals));
				if( !dr.bIsValid  && false /* can't do this with ODBC */  )
				{
					MyDataSet mds = (MyDataSet) DataSet;
					if( mds.m_conn==null )
						return dr;
					OdbcDataAdapter sqlda = new OdbcDataAdapter();
					OdbcCommand LoadCommand = new OdbcCommand("sp_Select_DesignObjType_DesignObjParameter", mds.m_conn,mds.m_trans);
					LoadCommand.CommandType = CommandType.StoredProcedure;
					LoadCommand.Parameters.Add(new OdbcParameter("@FK_DesignObjType", OdbcType.Int,4));
					LoadCommand.Parameters.Add(new OdbcParameter("@FK_DesignObjParameter", OdbcType.Int,4));
					LoadCommand.Parameters["@FK_DesignObjType"].Value = FK_DesignObjType;
					LoadCommand.Parameters["@FK_DesignObjParameter"].Value = FK_DesignObjParameter;
					sqlda.SelectCommand = LoadCommand;
					sqlda.Fill(mds,"DesignObjType_DesignObjParameter");
					dr = new DesignObjType_DesignObjParameterDataRow(Rows.Find(findTheseVals));
				}
				return dr;
			}
		}
		public DataRowCollection LoadAll(OdbcConnection conn, OdbcTransaction trans)
		{
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT FK_DesignObjType,FK_DesignObjParameter FROM DesignObjType_DesignObjParameter", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(this.DataSet,"DesignObjType_DesignObjParameter")==0 )
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
			DesignObjType_DesignObjParameterData.UpdateDesignObjType_DesignObjParameter(ref ds,PK_Users,conn,trans);
		}
		public DataColumn cFK_DesignObjType
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
	}
} // namespace HAData.Common.Data
