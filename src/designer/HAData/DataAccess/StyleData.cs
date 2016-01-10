namespace HAData.DataAccess {
	using System;
	using System.Data;
	using Microsoft.Data.Odbc;
	using System.Collections;

	using HAData.Common;

	public class StyleData : MyDataSet {
		//
		// Style table constants
		//
		public const String STYLE_TABLE = "Style";
		public const String PK_STYLE_FIELD = "PK_Style";
		public const String DESCRIPTION_FIELD = "Description";
		public const String FK_STYLE_SELECTED_FIELD = "FK_Style_Selected";
		public const String FK_STYLE_HIGHLIGHTED_FIELD = "FK_Style_Highlighted";
		public const String FK_STYLE_ALT_FIELD = "FK_Style_Alt";
		public const String ALWAYSINCLUDEONORBITER_FIELD = "AlwaysIncludeOnOrbiter";
		// table+field constants
		public const String PK_STYLE_TABLE_FIELD = "Style.PK_Style";
		public const String DESCRIPTION_TABLE_FIELD = "Style.Description";
		public const String FK_STYLE_SELECTED_TABLE_FIELD = "Style.FK_Style_Selected";
		public const String FK_STYLE_HIGHLIGHTED_TABLE_FIELD = "Style.FK_Style_Highlighted";
		public const String FK_STYLE_ALT_TABLE_FIELD = "Style.FK_Style_Alt";
		public const String ALWAYSINCLUDEONORBITER_TABLE_FIELD = "Style.AlwaysIncludeOnOrbiter";
		// DataSetCommand object
		protected OdbcDataAdapter m_DSCommand;

		// Stored procedure parameters
		protected const String PK_STYLE_PARM = "@PK_Style";
		protected const String DESCRIPTION_PARM = "@Description";
		protected const String FK_STYLE_SELECTED_PARM = "@FK_Style_Selected";
		protected const String FK_STYLE_HIGHLIGHTED_PARM = "@FK_Style_Highlighted";
		protected const String FK_STYLE_ALT_PARM = "@FK_Style_Alt";
		protected const String ALWAYSINCLUDEONORBITER_PARM = "@AlwaysIncludeOnOrbiter";
		protected const String USERID_PARM = "@UserID";

		protected OdbcCommand m_LoadCommand;
		protected OdbcCommand m_InsertCommand;
		protected OdbcCommand m_UpdateCommand;
		protected OdbcCommand m_DeleteCommand;
		protected OdbcConnection m_Connection;
		protected OdbcTransaction m_Transaction;
		public DataTable Table { get { return Tables[0]; } }


		public StyleData() {  // marker:1
			//
			// Create the tables in the dataset
			//
			Tables.Add(BuildDataTables());
			m_Connection = HADataConfiguration.GetOdbcConnection();
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

			m_DSCommand.TableMappings.Add("Table", StyleData.STYLE_TABLE);
		}

		public StyleData(OdbcConnection conn,OdbcTransaction trans) {

			m_Connection = conn;
			m_Transaction = trans;
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

			m_DSCommand.TableMappings.Add("Table", StyleData.STYLE_TABLE);
		}

		private StyleData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context) {
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
			return (DataTable) BuildStyleTable();
		}
		public static StyleTable BuildStyleTable() {
			//
			// Create the Style table
			//
			StyleTable Table = new StyleTable();
			DataColumnCollection Columns = Table.Columns;
			DataColumn[] PKColumns = new DataColumn[1];

			DataColumn Column = Columns.Add(PK_STYLE_FIELD, typeof(System.Int32));
			Column.AutoIncrement = true;
			Column.AutoIncrementSeed = -1;
			Column.AutoIncrementStep = -1;
			PKColumns[0] = Column;

			Columns.Add(DESCRIPTION_FIELD, typeof(System.String));
			Columns.Add(FK_STYLE_SELECTED_FIELD, typeof(System.Int32));
			Columns.Add(FK_STYLE_HIGHLIGHTED_FIELD, typeof(System.Int32));
			Columns.Add(FK_STYLE_ALT_FIELD, typeof(System.Int32));
			Columns.Add(ALWAYSINCLUDEONORBITER_FIELD, typeof(System.Boolean));
			Table.PrimaryKey = PKColumns;

			return Table;
		}
		protected static void CreateParameters(OdbcParameterCollection Params, bool IsInsert) {
			Params.Add(new OdbcParameter(PK_STYLE_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(DESCRIPTION_PARM, OdbcType.VarChar, 20));
			Params.Add(new OdbcParameter(FK_STYLE_SELECTED_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_STYLE_HIGHLIGHTED_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_STYLE_ALT_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(ALWAYSINCLUDEONORBITER_PARM, OdbcType.Bit,1));
			Params.Add(new OdbcParameter(USERID_PARM, OdbcType.Int));

			// map the parameters to the data table

			Params[PK_STYLE_PARM].SourceColumn = StyleData.PK_STYLE_FIELD;
			if(IsInsert) {
				Params[PK_STYLE_PARM].Direction = ParameterDirection.Output;
			}

			Params[DESCRIPTION_PARM].SourceColumn = StyleData.DESCRIPTION_FIELD;
			Params[FK_STYLE_SELECTED_PARM].SourceColumn = StyleData.FK_STYLE_SELECTED_FIELD;
			Params[FK_STYLE_HIGHLIGHTED_PARM].SourceColumn = StyleData.FK_STYLE_HIGHLIGHTED_FIELD;
			Params[FK_STYLE_ALT_PARM].SourceColumn = StyleData.FK_STYLE_ALT_FIELD;
			Params[ALWAYSINCLUDEONORBITER_PARM].SourceColumn = StyleData.ALWAYSINCLUDEONORBITER_FIELD;
		}

		protected static void CreateCommands(OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
			if(LoadCommand == null) {
				// Create the command since it's null
				LoadCommand = new OdbcCommand("sp_Select_Style", Conn);
				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(PK_STYLE_PARM, OdbcType.Int,4));
			}

			if(InsertCommand == null) {
				// Create the command since it's null
				InsertCommand = new OdbcCommand("sp_Insert_Style", Conn);
				InsertCommand.CommandType = CommandType.StoredProcedure;
				InsertCommand.Transaction = Trans;

				OdbcParameterCollection Params = InsertCommand.Parameters;

				CreateParameters(Params, true);

			}

			if(UpdateCommand == null) {
				// Create the command since it's null
				UpdateCommand = new OdbcCommand("sp_Update_Style", Conn);
				UpdateCommand.CommandType = CommandType.StoredProcedure;
				UpdateCommand.Transaction = Trans;

				OdbcParameterCollection Params = UpdateCommand.Parameters;

				CreateParameters(Params, false);

			}
			if (DeleteCommand == null)
			{
				DeleteCommand = new OdbcCommand("sp_Delete_Style", Conn);
				DeleteCommand.CommandType = CommandType.StoredProcedure;
				DeleteCommand.Transaction = Trans;

				DeleteCommand.Parameters.Add(PK_STYLE_PARM, OdbcType.Int,4, PK_STYLE_FIELD);
				DeleteCommand.Parameters.Add(USERID_PARM, OdbcType.Int);
			}
		}

		protected static void CreateCommands(OdbcDataAdapter odbcda,OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
				LoadCommand = new OdbcCommand("SELECT PK_Style,Description,FK_Style_Selected,FK_Style_Highlighted,FK_Style_Alt,AlwaysIncludeOnOrbiter FROM Style", Conn);
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(PK_STYLE_PARM, OdbcType.Int,4));

			odbcda.SelectCommand = LoadCommand;
			OdbcCommandBuilder odbcCB = new OdbcCommandBuilder(odbcda);
			odbcCB.RefreshSchema();
			DeleteCommand = odbcCB.GetDeleteCommand();
			InsertCommand = odbcCB.GetInsertCommand();
			UpdateCommand = odbcCB.GetUpdateCommand();
		}

		public StyleData LoadStyle(System.Int32 PK_Style)
		{
			m_DSCommand.SelectCommand = m_LoadCommand;
			m_DSCommand.SelectCommand.Parameters[PK_STYLE_PARM].Value = PK_Style;

			m_DSCommand.Fill(this);
			return this;
		}

		public static DataRowCollection LoadStyleWithWhere(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, string WhereClause) // marker:2
		{
			DataRowCollection dr;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["Style"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}
			
			DataSet dsTemp = new MyDataSet();
			dsTemp.Tables.Add(BuildDataTables());
			
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			string sSQL = "SELECT PK_Style, Description, FK_Style_Selected, FK_Style_Highlighted, FK_Style_Alt, AlwaysIncludeOnOrbiter FROM Style WHERE " + WhereClause;
			
			OdbcCommand LoadCommand = new OdbcCommand(sSQL,conn);
			
			if( trans!=null )
				LoadCommand.Transaction = trans;
			
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(dsTemp,"Style");
			
			dr=dsTemp.Tables["Style"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}

		public static DataRow LoadNoCacheStyle(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 PK_Style)
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["Style"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}

			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand;
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();

			LoadCommand = new OdbcCommand("sp_Select_Style", conn);

			LoadCommand.CommandType = CommandType.StoredProcedure;
			LoadCommand.Parameters.Add(new OdbcParameter(PK_STYLE_PARM, OdbcType.Int,4));
			LoadCommand.Parameters[PK_STYLE_PARM].Value = PK_Style;
			if( trans!=null )
				LoadCommand.Transaction = trans;
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(ds,"Style");
				dr = ds.Tables["Style"].Rows.Find(PK_Style);
			return dr;
		}

		public static DataRow LoadStyle(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 PK_Style)  // marker:3
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["Style"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
				else
				dr = dt.Rows.Find(PK_Style);
			}

			if( dr==null )
			{
				OdbcDataAdapter sqlda = new OdbcDataAdapter();
				OdbcCommand LoadCommand;
				if( conn==null )
					conn = HADataConfiguration.GetOdbcConnection();

				LoadCommand = new OdbcCommand("sp_Select_Style", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new OdbcParameter(PK_STYLE_PARM, OdbcType.Int,4));
				LoadCommand.Parameters[PK_STYLE_PARM].Value = PK_Style;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(ds,"Style");
				dr = ds.Tables["Style"].Rows.Find(PK_Style);
			}
			return dr;
		}

		public StyleData LoadAll() {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new OdbcCommand("SELECT * FROM Style", m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this);
			return this;

		}

		public static DataRowCollection LoadAll(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {

			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT * FROM Style", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(ds,"Style")==0 )
				return null;
			else
				return ds.Tables["Style"].Rows;

		}

		public StyleData ExecuteQuery(String sSQL) {
			return ExecuteQuery(sSQL,STYLE_TABLE);
		}
		public StyleData ExecuteQuery(String sSQL,String sTableName) {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new OdbcCommand(sSQL, m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this,sTableName);

			return this;
		}

		public static DataRowCollection ExecuteQuery(String sSQL,ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {
			return ExecuteQuery(sSQL,ref ds,conn,trans,"Style");
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

		public bool UpdateStyle(int CurUserID) {
			m_DSCommand.UpdateCommand = m_UpdateCommand;
			m_DSCommand.UpdateCommand.Parameters[USERID_PARM].Value = CurUserID;

			m_DSCommand.InsertCommand = m_InsertCommand;
			m_DSCommand.InsertCommand.Parameters[USERID_PARM].Value = CurUserID;

			if( m_DeleteCommand != null )
			{
				m_DSCommand.DeleteCommand = m_DeleteCommand;
				m_DSCommand.DeleteCommand.Parameters[USERID_PARM].Value = CurUserID;
			}

			m_DSCommand.Update(this, StyleData.STYLE_TABLE);
			return true;
		}

		public static bool UpdateStyle(ref MyDataSet ds, int CurUserID)
		{
			OdbcConnection OdbcConn = HADataConfiguration.GetOdbcConnection();
			return UpdateStyle(ref ds,CurUserID,OdbcConn,null);
		}

		public static bool UpdateStyle(ref MyDataSet ds, int CurUserID,OdbcConnection OdbcConn,OdbcTransaction Trans)
		{
			DataTable dt = ds.Tables[STYLE_TABLE];
			if( dt == null )
				return false;

			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = null;
			OdbcCommand InsertCommand = null;
			OdbcCommand UpdateCommand = null;
			OdbcCommand DeleteCommand = null;
			CreateCommands(sqlda,OdbcConn, Trans, ref LoadCommand, ref InsertCommand, ref UpdateCommand, ref DeleteCommand);
			sqlda.RowUpdated += new OdbcRowUpdatedEventHandler(MyRowUpdated);

			sqlda.Update(dt);
			return true;
		}

		static void MyRowUpdated(Object sender, OdbcRowUpdatedEventArgs e)
		{
			if( e.StatementType==StatementType.Insert )
			{
				OdbcCommand ocmd = new OdbcCommand("SELECT @@IDENTITY", e.Command.Connection);
				int value = Int32.Parse(ocmd.ExecuteScalar().ToString());
				e.Row[0]=value;
				e.Row.AcceptChanges();
			}
		}

	} // public class StyleData
	public class StyleDataRow
	{
		public DataRow dr = null;
		public StyleDataRow(DataRow d)
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
		public System.Int32 fPK_Style
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
		public bool fDescriptionIsNull
		{
			get
			{
				return dr[1]==DBNull.Value;
			}
		}
		public void fDescriptionSetNull()
		{
			dr[1]=DBNull.Value;
		}
		public System.Int32 fFK_Style_Selected
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
		public bool fFK_Style_SelectedIsNull
		{
			get
			{
				return dr[2]==DBNull.Value;
			}
		}
		public void fFK_Style_SelectedSetNull()
		{
			dr[2]=DBNull.Value;
		}
		public StyleDataRow fFK_Style_Selected_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tStyle[Convert.ToInt32(dr[2])];
			}
		}
		public System.Int32 fFK_Style_Highlighted
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
		public bool fFK_Style_HighlightedIsNull
		{
			get
			{
				return dr[3]==DBNull.Value;
			}
		}
		public void fFK_Style_HighlightedSetNull()
		{
			dr[3]=DBNull.Value;
		}
		public StyleDataRow fFK_Style_Highlighted_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tStyle[Convert.ToInt32(dr[3])];
			}
		}
		public System.Int32 fFK_Style_Alt
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
		public bool fFK_Style_AltIsNull
		{
			get
			{
				return dr[4]==DBNull.Value;
			}
		}
		public void fFK_Style_AltSetNull()
		{
			dr[4]=DBNull.Value;
		}
		public StyleDataRow fFK_Style_Alt_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tStyle[Convert.ToInt32(dr[4])];
			}
		}
		public System.Boolean fAlwaysIncludeOnOrbiter
		{
			get
			{
				return Convert.ToBoolean(dr[5]);
			}
			set
			{
				dr[5]=value;
			}
		}
		public bool fAlwaysIncludeOnOrbiterIsNull
		{
			get
			{
				return dr[5]==DBNull.Value;
			}
		}
		public void fAlwaysIncludeOnOrbiterSetNull()
		{
			dr[5]=DBNull.Value;
		}
	} // public class StyleDataRow
	public class StyleDataReader
	{
		public OdbcDataReader dr;
		bool bCache=false;
		int iRecord=-1,iNumRecords=-1;
		ArrayList al = null;

		public StyleDataReader(OdbcDataReader d)
		{
			dr=d;
		}
		public StyleDataReader(OdbcCommand cmd)
		{
			dr = cmd.ExecuteReader();
		}

		public StyleDataReader(OdbcCommand cmd,bool Cache)
		{
			dr = cmd.ExecuteReader();
			bCache=Cache;
			if( bCache )
				CacheAndClose();
		}

		public StyleDataReader(string sSQL)
		{
			OdbcConnection conn = HADataConfiguration.GetOdbcConnection();

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM Style WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public StyleDataReader(string sSQL,OdbcConnection conn)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM Style WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public StyleDataReader(string sSQL,OdbcConnection conn,OdbcTransaction trans,bool Cache)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM Style WHERE " + sSQL;
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
				object[] objs = new object[6];
				for(int i=0;i<6;i++)
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
		public System.Int32 fPK_Style
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
		public bool fDescriptionIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[1]==DBNull.Value;
				else
					return dr[1]==DBNull.Value;
			}
		}
		public System.Int32 fFK_Style_Selected
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[2]);
				else
					return Convert.ToInt32(dr[2]);
			}
		}
		public bool fFK_Style_SelectedIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[2]==DBNull.Value;
				else
					return dr[2]==DBNull.Value;
			}
		}
		public System.Int32 fFK_Style_Highlighted
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[3]);
				else
					return Convert.ToInt32(dr[3]);
			}
		}
		public bool fFK_Style_HighlightedIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[3]==DBNull.Value;
				else
					return dr[3]==DBNull.Value;
			}
		}
		public System.Int32 fFK_Style_Alt
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[4]);
				else
					return Convert.ToInt32(dr[4]);
			}
		}
		public bool fFK_Style_AltIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[4]==DBNull.Value;
				else
					return dr[4]==DBNull.Value;
			}
		}
		public System.Boolean fAlwaysIncludeOnOrbiter
		{
			get
			{
				if( bCache )
					return Convert.ToBoolean(((object[]) al[iRecord])[5]);
				else
					return Convert.ToBoolean(dr[5]);
			}
		}
		public bool fAlwaysIncludeOnOrbiterIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[5]==DBNull.Value;
				else
					return dr[5]==DBNull.Value;
			}
		}
	} // public class StyleDataReader
	public class StyleTable : DataTable
	{
		public StyleTable() : base("Style") {}

		public StyleDataRow this [System.Int32 PK_Style]
		{
			get
			{
				StyleDataRow dr = new StyleDataRow(Rows.Find(PK_Style));
				if( !dr.bIsValid  && false /* can't do this with ODBC */  )
				{
					MyDataSet mds = (MyDataSet) DataSet;
					if( mds.m_conn==null )
						return dr;
					OdbcDataAdapter sqlda = new OdbcDataAdapter();
					OdbcCommand LoadCommand = new OdbcCommand("sp_Select_Style", mds.m_conn,mds.m_trans);
					LoadCommand.CommandType = CommandType.StoredProcedure;
					LoadCommand.Parameters.Add(new OdbcParameter("@PK_Style", OdbcType.Int,4));
					LoadCommand.Parameters["@PK_Style"].Value = PK_Style;
					sqlda.SelectCommand = LoadCommand;
					sqlda.Fill(mds,"Style");
					dr = new StyleDataRow(Rows.Find(PK_Style));
				}
				return dr;
			}
		}
		public DataRowCollection LoadAll(OdbcConnection conn, OdbcTransaction trans)
		{
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT PK_Style,Description,FK_Style_Selected,FK_Style_Highlighted,FK_Style_Alt,AlwaysIncludeOnOrbiter FROM Style", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(this.DataSet,"Style")==0 )
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
			StyleData.UpdateStyle(ref ds,PK_Users,conn,trans);
		}
		public DataColumn cPK_Style
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
		public DataColumn cFK_Style_Selected
		{
			get
			{
				return Columns[2];
			}
		}
		public DataColumn cFK_Style_Highlighted
		{
			get
			{
				return Columns[3];
			}
		}
		public DataColumn cFK_Style_Alt
		{
			get
			{
				return Columns[4];
			}
		}
		public DataColumn cAlwaysIncludeOnOrbiter
		{
			get
			{
				return Columns[5];
			}
		}
	}
} // namespace HAData.Common.Data
