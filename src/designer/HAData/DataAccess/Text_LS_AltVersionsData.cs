namespace HAData.DataAccess {
	using System;
	using System.Data;
	using Microsoft.Data.Odbc;
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
		protected OdbcDataAdapter m_DSCommand;

		// Stored procedure parameters
		protected const String FK_TEXT_PARM = "@FK_Text";
		protected const String FK_LANGUAGE_PARM = "@FK_Language";
		protected const String VERSION_PARM = "@Version";
		protected const String DESCRIPTION_PARM = "@Description";
		protected const String USERID_PARM = "@UserID";

		protected OdbcCommand m_LoadCommand;
		protected OdbcCommand m_InsertCommand;
		protected OdbcCommand m_UpdateCommand;
		protected OdbcCommand m_DeleteCommand;
		protected OdbcConnection m_Connection;
		protected OdbcTransaction m_Transaction;
		public DataTable Table { get { return Tables[0]; } }


		public Text_LS_AltVersionsData() {  // marker:1
			//
			// Create the tables in the dataset
			//
			Tables.Add(BuildDataTables());
			m_Connection = HADataConfiguration.GetOdbcConnection();
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

			m_DSCommand.TableMappings.Add("Table", Text_LS_AltVersionsData.TEXT_LS_ALTVERSIONS_TABLE);
		}

		public Text_LS_AltVersionsData(OdbcConnection conn,OdbcTransaction trans) {

			m_Connection = conn;
			m_Transaction = trans;
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

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
		protected static void CreateParameters(OdbcParameterCollection Params, bool IsInsert) {
			Params.Add(new OdbcParameter(FK_TEXT_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_LANGUAGE_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(VERSION_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(DESCRIPTION_PARM, OdbcType.Text));
			Params.Add(new OdbcParameter(USERID_PARM, OdbcType.Int));

			// map the parameters to the data table

			Params[FK_TEXT_PARM].SourceColumn = Text_LS_AltVersionsData.FK_TEXT_FIELD;
			Params[FK_LANGUAGE_PARM].SourceColumn = Text_LS_AltVersionsData.FK_LANGUAGE_FIELD;
			Params[VERSION_PARM].SourceColumn = Text_LS_AltVersionsData.VERSION_FIELD;
			Params[DESCRIPTION_PARM].SourceColumn = Text_LS_AltVersionsData.DESCRIPTION_FIELD;
		}

		protected static void CreateCommands(OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
			if(LoadCommand == null) {
				// Create the command since it's null
				LoadCommand = new OdbcCommand("sp_Select_Text_LS_AltVersions", Conn);
				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(FK_TEXT_PARM, OdbcType.Int,4));
				LoadCommand.Parameters.Add(new OdbcParameter(FK_LANGUAGE_PARM, OdbcType.Int,4));
				LoadCommand.Parameters.Add(new OdbcParameter(VERSION_PARM, OdbcType.Int,4));
			}

			if(InsertCommand == null) {
				// Create the command since it's null
				InsertCommand = new OdbcCommand("sp_Insert_Text_LS_AltVersions", Conn);
				InsertCommand.CommandType = CommandType.StoredProcedure;
				InsertCommand.Transaction = Trans;

				OdbcParameterCollection Params = InsertCommand.Parameters;

				CreateParameters(Params, true);

			}

			if(UpdateCommand == null) {
				// Create the command since it's null
				UpdateCommand = new OdbcCommand("sp_Update_Text_LS_AltVersions", Conn);
				UpdateCommand.CommandType = CommandType.StoredProcedure;
				UpdateCommand.Transaction = Trans;

				OdbcParameterCollection Params = UpdateCommand.Parameters;

				CreateParameters(Params, false);

			}
			if (DeleteCommand == null)
			{
				DeleteCommand = new OdbcCommand("sp_Delete_Text_LS_AltVersions", Conn);
				DeleteCommand.CommandType = CommandType.StoredProcedure;
				DeleteCommand.Transaction = Trans;

				DeleteCommand.Parameters.Add(FK_TEXT_PARM, OdbcType.Int,4, FK_TEXT_FIELD);
				DeleteCommand.Parameters.Add(FK_LANGUAGE_PARM, OdbcType.Int,4, FK_LANGUAGE_FIELD);
				DeleteCommand.Parameters.Add(VERSION_PARM, OdbcType.Int,4, VERSION_FIELD);
				DeleteCommand.Parameters.Add(USERID_PARM, OdbcType.Int);
			}
		}

		protected static void CreateCommands(OdbcDataAdapter odbcda,OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
				LoadCommand = new OdbcCommand("SELECT FK_Text,FK_Language,Version,Description FROM Text_LS_AltVersions", Conn);
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(FK_TEXT_PARM, OdbcType.Int,4));
				LoadCommand.Parameters.Add(new OdbcParameter(FK_LANGUAGE_PARM, OdbcType.Int,4));
				LoadCommand.Parameters.Add(new OdbcParameter(VERSION_PARM, OdbcType.Int,4));

			odbcda.SelectCommand = LoadCommand;
			OdbcCommandBuilder odbcCB = new OdbcCommandBuilder(odbcda);
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

		public static DataRowCollection LoadText_LS_AltVersionsWithWhere(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, string WhereClause) // marker:2
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
				conn = HADataConfiguration.GetOdbcConnection();
			
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			string sSQL = "SELECT FK_Text, FK_Language, Version, Description FROM Text_LS_AltVersions WHERE " + WhereClause;
			
			OdbcCommand LoadCommand = new OdbcCommand(sSQL,conn);
			
			if( trans!=null )
				LoadCommand.Transaction = trans;
			
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(dsTemp,"Text_LS_AltVersions");
			
			dr=dsTemp.Tables["Text_LS_AltVersions"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}

		public static DataRow LoadNoCacheText_LS_AltVersions(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 FK_Text, System.Int32 FK_Language, System.Int32 Version)
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

			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand;
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();

			LoadCommand = new OdbcCommand("sp_Select_Text_LS_AltVersions", conn);

			LoadCommand.CommandType = CommandType.StoredProcedure;
			LoadCommand.Parameters.Add(new OdbcParameter(FK_TEXT_PARM, OdbcType.Int,4));
			LoadCommand.Parameters.Add(new OdbcParameter(FK_LANGUAGE_PARM, OdbcType.Int,4));
			LoadCommand.Parameters.Add(new OdbcParameter(VERSION_PARM, OdbcType.Int,4));
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

		public static DataRow LoadText_LS_AltVersions(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 FK_Text, System.Int32 FK_Language, System.Int32 Version)  // marker:3
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
				OdbcDataAdapter sqlda = new OdbcDataAdapter();
				OdbcCommand LoadCommand;
				if( conn==null )
					conn = HADataConfiguration.GetOdbcConnection();

				LoadCommand = new OdbcCommand("sp_Select_Text_LS_AltVersions", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new OdbcParameter(FK_TEXT_PARM, OdbcType.Int,4));
				LoadCommand.Parameters.Add(new OdbcParameter(FK_LANGUAGE_PARM, OdbcType.Int,4));
				LoadCommand.Parameters.Add(new OdbcParameter(VERSION_PARM, OdbcType.Int,4));
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

		public static DataRowCollection LoadText_LS_AltVersions_FirstPK(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans,System.Int32 FK_Text)
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
				conn = HADataConfiguration.GetOdbcConnection();
			
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
				OdbcCommand LoadCommand;

				LoadCommand = new OdbcCommand("sp_Select_Text_LS_AltVersions_FirstPK", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new OdbcParameter(FK_TEXT_PARM, OdbcType.Int,4));
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
			m_DSCommand.SelectCommand = new OdbcCommand("SELECT * FROM Text_LS_AltVersions", m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this);
			return this;

		}

		public static DataRowCollection LoadAll(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {

			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT * FROM Text_LS_AltVersions", conn);
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
			m_DSCommand.SelectCommand = new OdbcCommand(sSQL, m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this,sTableName);

			return this;
		}

		public static DataRowCollection ExecuteQuery(String sSQL,ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {
			return ExecuteQuery(sSQL,ref ds,conn,trans,"Text_LS_AltVersions");
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
			OdbcConnection OdbcConn = HADataConfiguration.GetOdbcConnection();
			return UpdateText_LS_AltVersions(ref ds,CurUserID,OdbcConn,null);
		}

		public static bool UpdateText_LS_AltVersions(ref MyDataSet ds, int CurUserID,OdbcConnection OdbcConn,OdbcTransaction Trans)
		{
			DataTable dt = ds.Tables[TEXT_LS_ALTVERSIONS_TABLE];
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
		public OdbcDataReader dr;
		bool bCache=false;
		int iRecord=-1,iNumRecords=-1;
		ArrayList al = null;

		public Text_LS_AltVersionsDataReader(OdbcDataReader d)
		{
			dr=d;
		}
		public Text_LS_AltVersionsDataReader(OdbcCommand cmd)
		{
			dr = cmd.ExecuteReader();
		}

		public Text_LS_AltVersionsDataReader(OdbcCommand cmd,bool Cache)
		{
			dr = cmd.ExecuteReader();
			bCache=Cache;
			if( bCache )
				CacheAndClose();
		}

		public Text_LS_AltVersionsDataReader(string sSQL)
		{
			OdbcConnection conn = HADataConfiguration.GetOdbcConnection();

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM Text_LS_AltVersions WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public Text_LS_AltVersionsDataReader(string sSQL,OdbcConnection conn)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM Text_LS_AltVersions WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public Text_LS_AltVersionsDataReader(string sSQL,OdbcConnection conn,OdbcTransaction trans,bool Cache)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM Text_LS_AltVersions WHERE " + sSQL;
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
				if( !dr.bIsValid  && false /* can't do this with ODBC */  )
				{
					MyDataSet mds = (MyDataSet) DataSet;
					if( mds.m_conn==null )
						return dr;
					OdbcDataAdapter sqlda = new OdbcDataAdapter();
					OdbcCommand LoadCommand = new OdbcCommand("sp_Select_Text_LS_AltVersions", mds.m_conn,mds.m_trans);
					LoadCommand.CommandType = CommandType.StoredProcedure;
					LoadCommand.Parameters.Add(new OdbcParameter("@FK_Text", OdbcType.Int,4));
					LoadCommand.Parameters.Add(new OdbcParameter("@FK_Language", OdbcType.Int,4));
					LoadCommand.Parameters.Add(new OdbcParameter("@Version", OdbcType.Int,4));
					LoadCommand.Parameters["@FK_Text"].Value = FK_Text;
					LoadCommand.Parameters["@FK_Language"].Value = FK_Language;
					LoadCommand.Parameters["@Version"].Value = Version;
					sqlda.SelectCommand = LoadCommand;
					sqlda.Fill(mds,"Text_LS_AltVersions");
					dr = new Text_LS_AltVersionsDataRow(Rows.Find(findTheseVals));
				}
				return dr;
			}
		}
		public DataRowCollection LoadAll(OdbcConnection conn, OdbcTransaction trans)
		{
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT FK_Text,FK_Language,Version,Description FROM Text_LS_AltVersions", conn);
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
		public void Update(int PK_Users,OdbcConnection conn, OdbcTransaction trans)
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
