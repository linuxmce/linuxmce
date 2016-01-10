namespace HAData.DataAccess {
	using System;
	using System.Data;
	using Microsoft.Data.Odbc;
	using System.Collections;

	using HAData.Common;

	public class CriteriaList_CriteriaParmListData : MyDataSet {
		//
		// CriteriaList_CriteriaParmList table constants
		//
		public const String CRITERIALIST_CRITERIAPARMLIST_TABLE = "CriteriaList_CriteriaParmList";
		public const String FK_CRITERIALIST_FIELD = "FK_CriteriaList";
		public const String FK_CRITERIAPARMLIST_FIELD = "FK_CriteriaParmList";
		// table+field constants
		public const String FK_CRITERIALIST_TABLE_FIELD = "CriteriaList_CriteriaParmList.FK_CriteriaList";
		public const String FK_CRITERIAPARMLIST_TABLE_FIELD = "CriteriaList_CriteriaParmList.FK_CriteriaParmList";
		// DataSetCommand object
		protected OdbcDataAdapter m_DSCommand;

		// Stored procedure parameters
		protected const String FK_CRITERIALIST_PARM = "@FK_CriteriaList";
		protected const String FK_CRITERIAPARMLIST_PARM = "@FK_CriteriaParmList";
		protected const String USERID_PARM = "@UserID";

		protected OdbcCommand m_LoadCommand;
		protected OdbcCommand m_InsertCommand;
		protected OdbcCommand m_UpdateCommand;
		protected OdbcCommand m_DeleteCommand;
		protected OdbcConnection m_Connection;
		protected OdbcTransaction m_Transaction;
		public DataTable Table { get { return Tables[0]; } }


		public CriteriaList_CriteriaParmListData() {  // marker:1
			//
			// Create the tables in the dataset
			//
			Tables.Add(BuildDataTables());
			m_Connection = HADataConfiguration.GetOdbcConnection();
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

			m_DSCommand.TableMappings.Add("Table", CriteriaList_CriteriaParmListData.CRITERIALIST_CRITERIAPARMLIST_TABLE);
		}

		public CriteriaList_CriteriaParmListData(OdbcConnection conn,OdbcTransaction trans) {

			m_Connection = conn;
			m_Transaction = trans;
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

			m_DSCommand.TableMappings.Add("Table", CriteriaList_CriteriaParmListData.CRITERIALIST_CRITERIAPARMLIST_TABLE);
		}

		private CriteriaList_CriteriaParmListData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context) {
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
			return (DataTable) BuildCriteriaList_CriteriaParmListTable();
		}
		public static CriteriaList_CriteriaParmListTable BuildCriteriaList_CriteriaParmListTable() {
			//
			// Create the CriteriaList_CriteriaParmList table
			//
			CriteriaList_CriteriaParmListTable Table = new CriteriaList_CriteriaParmListTable();
			DataColumnCollection Columns = Table.Columns;
			DataColumn[] PKColumns = new DataColumn[2];

			DataColumn Column = Columns.Add(FK_CRITERIALIST_FIELD, typeof(System.Int32));
			Column.DefaultValue = 0;
			PKColumns[0] = Column;
			Column.DefaultValue = -1;

			Column = Columns.Add(FK_CRITERIAPARMLIST_FIELD, typeof(System.Int32));
			Column.DefaultValue = 0;
			PKColumns[1] = Column;
			Column.DefaultValue = -1;

			Table.PrimaryKey = PKColumns;

			return Table;
		}
		protected static void CreateParameters(OdbcParameterCollection Params, bool IsInsert) {
			Params.Add(new OdbcParameter(FK_CRITERIALIST_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_CRITERIAPARMLIST_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(USERID_PARM, OdbcType.Int));

			// map the parameters to the data table

			Params[FK_CRITERIALIST_PARM].SourceColumn = CriteriaList_CriteriaParmListData.FK_CRITERIALIST_FIELD;
			Params[FK_CRITERIAPARMLIST_PARM].SourceColumn = CriteriaList_CriteriaParmListData.FK_CRITERIAPARMLIST_FIELD;
		}

		protected static void CreateCommands(OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
			if(LoadCommand == null) {
				// Create the command since it's null
				LoadCommand = new OdbcCommand("sp_Select_CriteriaList_CriteriaParmList", Conn);
				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(FK_CRITERIALIST_PARM, OdbcType.Int,4));
				LoadCommand.Parameters.Add(new OdbcParameter(FK_CRITERIAPARMLIST_PARM, OdbcType.Int,4));
			}

			if(InsertCommand == null) {
				// Create the command since it's null
				InsertCommand = new OdbcCommand("sp_Insert_CriteriaList_CriteriaParmList", Conn);
				InsertCommand.CommandType = CommandType.StoredProcedure;
				InsertCommand.Transaction = Trans;

				OdbcParameterCollection Params = InsertCommand.Parameters;

				CreateParameters(Params, true);

			}

			if(UpdateCommand == null) {
				// Create the command since it's null
				UpdateCommand = new OdbcCommand("sp_Update_CriteriaList_CriteriaParmList", Conn);
				UpdateCommand.CommandType = CommandType.StoredProcedure;
				UpdateCommand.Transaction = Trans;

				OdbcParameterCollection Params = UpdateCommand.Parameters;

				CreateParameters(Params, false);

			}
			if (DeleteCommand == null)
			{
				DeleteCommand = new OdbcCommand("sp_Delete_CriteriaList_CriteriaParmList", Conn);
				DeleteCommand.CommandType = CommandType.StoredProcedure;
				DeleteCommand.Transaction = Trans;

				DeleteCommand.Parameters.Add(FK_CRITERIALIST_PARM, OdbcType.Int,4, FK_CRITERIALIST_FIELD);
				DeleteCommand.Parameters.Add(FK_CRITERIAPARMLIST_PARM, OdbcType.Int,4, FK_CRITERIAPARMLIST_FIELD);
				DeleteCommand.Parameters.Add(USERID_PARM, OdbcType.Int);
			}
		}

		protected static void CreateCommands(OdbcDataAdapter odbcda,OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
				LoadCommand = new OdbcCommand("SELECT FK_CriteriaList,FK_CriteriaParmList FROM CriteriaList_CriteriaParmList", Conn);
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(FK_CRITERIALIST_PARM, OdbcType.Int,4));
				LoadCommand.Parameters.Add(new OdbcParameter(FK_CRITERIAPARMLIST_PARM, OdbcType.Int,4));

			odbcda.SelectCommand = LoadCommand;
			OdbcCommandBuilder odbcCB = new OdbcCommandBuilder(odbcda);
			odbcCB.RefreshSchema();
			DeleteCommand = odbcCB.GetDeleteCommand();
			InsertCommand = odbcCB.GetInsertCommand();
			UpdateCommand = odbcCB.GetUpdateCommand();
		}

		public CriteriaList_CriteriaParmListData LoadCriteriaList_CriteriaParmList(System.Int32 FK_CriteriaList, System.Int32 FK_CriteriaParmList)
		{
			m_DSCommand.SelectCommand = m_LoadCommand;
			m_DSCommand.SelectCommand.Parameters[FK_CRITERIALIST_PARM].Value = FK_CriteriaList;
			m_DSCommand.SelectCommand.Parameters[FK_CRITERIAPARMLIST_PARM].Value = FK_CriteriaParmList;

			m_DSCommand.Fill(this);
			return this;
		}

		public static DataRowCollection LoadCriteriaList_CriteriaParmListWithWhere(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, string WhereClause) // marker:2
		{
			DataRowCollection dr;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["CriteriaList_CriteriaParmList"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}
			
			DataSet dsTemp = new MyDataSet();
			dsTemp.Tables.Add(BuildDataTables());
			
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			string sSQL = "SELECT FK_CriteriaList, FK_CriteriaParmList FROM CriteriaList_CriteriaParmList WHERE " + WhereClause;
			
			OdbcCommand LoadCommand = new OdbcCommand(sSQL,conn);
			
			if( trans!=null )
				LoadCommand.Transaction = trans;
			
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(dsTemp,"CriteriaList_CriteriaParmList");
			
			dr=dsTemp.Tables["CriteriaList_CriteriaParmList"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}

		public static DataRow LoadNoCacheCriteriaList_CriteriaParmList(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 FK_CriteriaList, System.Int32 FK_CriteriaParmList)
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["CriteriaList_CriteriaParmList"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}

			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand;
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();

			LoadCommand = new OdbcCommand("sp_Select_CriteriaList_CriteriaParmList", conn);

			LoadCommand.CommandType = CommandType.StoredProcedure;
			LoadCommand.Parameters.Add(new OdbcParameter(FK_CRITERIALIST_PARM, OdbcType.Int,4));
			LoadCommand.Parameters.Add(new OdbcParameter(FK_CRITERIAPARMLIST_PARM, OdbcType.Int,4));
			LoadCommand.Parameters[FK_CRITERIALIST_PARM].Value = FK_CriteriaList;
			LoadCommand.Parameters[FK_CRITERIAPARMLIST_PARM].Value = FK_CriteriaParmList;
			if( trans!=null )
				LoadCommand.Transaction = trans;
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(ds,"CriteriaList_CriteriaParmList");
			object[]findTheseVals = new object[2];
			findTheseVals[0] = FK_CriteriaList;
			findTheseVals[1] = FK_CriteriaParmList;
			dr = ds.Tables["CriteriaList_CriteriaParmList"].Rows.Find(findTheseVals);
			return dr;
		}

		public static DataRow LoadCriteriaList_CriteriaParmList(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 FK_CriteriaList, System.Int32 FK_CriteriaParmList)  // marker:3
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
				findTheseVals[0] = FK_CriteriaList;
				findTheseVals[1] = FK_CriteriaParmList;
				DataTable dt = ds.Tables["CriteriaList_CriteriaParmList"];
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

				LoadCommand = new OdbcCommand("sp_Select_CriteriaList_CriteriaParmList", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new OdbcParameter(FK_CRITERIALIST_PARM, OdbcType.Int,4));
				LoadCommand.Parameters.Add(new OdbcParameter(FK_CRITERIAPARMLIST_PARM, OdbcType.Int,4));
				LoadCommand.Parameters[FK_CRITERIALIST_PARM].Value = FK_CriteriaList;
				LoadCommand.Parameters[FK_CRITERIAPARMLIST_PARM].Value = FK_CriteriaParmList;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(ds,"CriteriaList_CriteriaParmList");
				object[]findTheseVals = new object[2];
				findTheseVals[0] = FK_CriteriaList;
				findTheseVals[1] = FK_CriteriaParmList;
				dr = ds.Tables["CriteriaList_CriteriaParmList"].Rows.Find(findTheseVals);
			}
			return dr;
		}

		public static DataRowCollection LoadCriteriaList_CriteriaParmList_FirstPK(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans,System.Int32 FK_CriteriaList)
		{
			DataRowCollection dr;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["CriteriaList_CriteriaParmList"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}
			
			DataSet dsTemp = new MyDataSet();
			dsTemp.Tables.Add(BuildDataTables());
			
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
				OdbcCommand LoadCommand;

				LoadCommand = new OdbcCommand("sp_Select_CriteriaList_CriteriaParmList_FirstPK", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new OdbcParameter(FK_CRITERIALIST_PARM, OdbcType.Int,4));
				LoadCommand.Parameters[FK_CRITERIALIST_PARM].Value = FK_CriteriaList;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(dsTemp,"CriteriaList_CriteriaParmList");
			
			dr=dsTemp.Tables["CriteriaList_CriteriaParmList"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}
		public CriteriaList_CriteriaParmListData LoadAll() {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new OdbcCommand("SELECT * FROM CriteriaList_CriteriaParmList", m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this);
			return this;

		}

		public static DataRowCollection LoadAll(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {

			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT * FROM CriteriaList_CriteriaParmList", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(ds,"CriteriaList_CriteriaParmList")==0 )
				return null;
			else
				return ds.Tables["CriteriaList_CriteriaParmList"].Rows;

		}

		public CriteriaList_CriteriaParmListData ExecuteQuery(String sSQL) {
			return ExecuteQuery(sSQL,CRITERIALIST_CRITERIAPARMLIST_TABLE);
		}
		public CriteriaList_CriteriaParmListData ExecuteQuery(String sSQL,String sTableName) {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new OdbcCommand(sSQL, m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this,sTableName);

			return this;
		}

		public static DataRowCollection ExecuteQuery(String sSQL,ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {
			return ExecuteQuery(sSQL,ref ds,conn,trans,"CriteriaList_CriteriaParmList");
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

		public bool UpdateCriteriaList_CriteriaParmList(int CurUserID) {
			m_DSCommand.UpdateCommand = m_UpdateCommand;
			m_DSCommand.UpdateCommand.Parameters[USERID_PARM].Value = CurUserID;

			m_DSCommand.InsertCommand = m_InsertCommand;
			m_DSCommand.InsertCommand.Parameters[USERID_PARM].Value = CurUserID;

			if( m_DeleteCommand != null )
			{
				m_DSCommand.DeleteCommand = m_DeleteCommand;
				m_DSCommand.DeleteCommand.Parameters[USERID_PARM].Value = CurUserID;
			}

			m_DSCommand.Update(this, CriteriaList_CriteriaParmListData.CRITERIALIST_CRITERIAPARMLIST_TABLE);
			return true;
		}

		public static bool UpdateCriteriaList_CriteriaParmList(ref MyDataSet ds, int CurUserID)
		{
			OdbcConnection OdbcConn = HADataConfiguration.GetOdbcConnection();
			return UpdateCriteriaList_CriteriaParmList(ref ds,CurUserID,OdbcConn,null);
		}

		public static bool UpdateCriteriaList_CriteriaParmList(ref MyDataSet ds, int CurUserID,OdbcConnection OdbcConn,OdbcTransaction Trans)
		{
			DataTable dt = ds.Tables[CRITERIALIST_CRITERIAPARMLIST_TABLE];
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

	} // public class CriteriaList_CriteriaParmListData
	public class CriteriaList_CriteriaParmListDataRow
	{
		public DataRow dr = null;
		public CriteriaList_CriteriaParmListDataRow(DataRow d)
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
		public System.Int32 fFK_CriteriaList
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
		public CriteriaListDataRow fFK_CriteriaList_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tCriteriaList[Convert.ToInt32(dr[0])];
			}
		}
		public System.Int32 fFK_CriteriaParmList
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
		public CriteriaParmListDataRow fFK_CriteriaParmList_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tCriteriaParmList[Convert.ToInt32(dr[1])];
			}
		}
	} // public class CriteriaList_CriteriaParmListDataRow
	public class CriteriaList_CriteriaParmListDataReader
	{
		public OdbcDataReader dr;
		bool bCache=false;
		int iRecord=-1,iNumRecords=-1;
		ArrayList al = null;

		public CriteriaList_CriteriaParmListDataReader(OdbcDataReader d)
		{
			dr=d;
		}
		public CriteriaList_CriteriaParmListDataReader(OdbcCommand cmd)
		{
			dr = cmd.ExecuteReader();
		}

		public CriteriaList_CriteriaParmListDataReader(OdbcCommand cmd,bool Cache)
		{
			dr = cmd.ExecuteReader();
			bCache=Cache;
			if( bCache )
				CacheAndClose();
		}

		public CriteriaList_CriteriaParmListDataReader(string sSQL)
		{
			OdbcConnection conn = HADataConfiguration.GetOdbcConnection();

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM CriteriaList_CriteriaParmList WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public CriteriaList_CriteriaParmListDataReader(string sSQL,OdbcConnection conn)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM CriteriaList_CriteriaParmList WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public CriteriaList_CriteriaParmListDataReader(string sSQL,OdbcConnection conn,OdbcTransaction trans,bool Cache)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM CriteriaList_CriteriaParmList WHERE " + sSQL;
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
		public System.Int32 fFK_CriteriaList
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[0]);
				else
					return Convert.ToInt32(dr[0]);
			}
		}
		public System.Int32 fFK_CriteriaParmList
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[1]);
				else
					return Convert.ToInt32(dr[1]);
			}
		}
	} // public class CriteriaList_CriteriaParmListDataReader
	public class CriteriaList_CriteriaParmListTable : DataTable
	{
		public CriteriaList_CriteriaParmListTable() : base("CriteriaList_CriteriaParmList") {}

		public CriteriaList_CriteriaParmListDataRow this [System.Int32 FK_CriteriaList, System.Int32 FK_CriteriaParmList]
		{
			get
			{
				object[]findTheseVals = new object[2];
				findTheseVals[0] = FK_CriteriaList;
				findTheseVals[1] = FK_CriteriaParmList;
				CriteriaList_CriteriaParmListDataRow dr = new CriteriaList_CriteriaParmListDataRow(Rows.Find(findTheseVals));
				if( !dr.bIsValid  && false /* can't do this with ODBC */  )
				{
					MyDataSet mds = (MyDataSet) DataSet;
					if( mds.m_conn==null )
						return dr;
					OdbcDataAdapter sqlda = new OdbcDataAdapter();
					OdbcCommand LoadCommand = new OdbcCommand("sp_Select_CriteriaList_CriteriaParmList", mds.m_conn,mds.m_trans);
					LoadCommand.CommandType = CommandType.StoredProcedure;
					LoadCommand.Parameters.Add(new OdbcParameter("@FK_CriteriaList", OdbcType.Int,4));
					LoadCommand.Parameters.Add(new OdbcParameter("@FK_CriteriaParmList", OdbcType.Int,4));
					LoadCommand.Parameters["@FK_CriteriaList"].Value = FK_CriteriaList;
					LoadCommand.Parameters["@FK_CriteriaParmList"].Value = FK_CriteriaParmList;
					sqlda.SelectCommand = LoadCommand;
					sqlda.Fill(mds,"CriteriaList_CriteriaParmList");
					dr = new CriteriaList_CriteriaParmListDataRow(Rows.Find(findTheseVals));
				}
				return dr;
			}
		}
		public DataRowCollection LoadAll(OdbcConnection conn, OdbcTransaction trans)
		{
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT FK_CriteriaList,FK_CriteriaParmList FROM CriteriaList_CriteriaParmList", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(this.DataSet,"CriteriaList_CriteriaParmList")==0 )
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
			CriteriaList_CriteriaParmListData.UpdateCriteriaList_CriteriaParmList(ref ds,PK_Users,conn,trans);
		}
		public DataColumn cFK_CriteriaList
		{
			get
			{
				return Columns[0];
			}
		}
		public DataColumn cFK_CriteriaParmList
		{
			get
			{
				return Columns[1];
			}
		}
	}
} // namespace HAData.Common.Data
