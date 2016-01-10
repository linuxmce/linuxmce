namespace HAData.DataAccess {
	using System;
	using System.Data;
	using Microsoft.Data.Odbc;
	using System.Collections;

	using HAData.Common;

	public class DesignObjVariation_ZoneData : MyDataSet {
		//
		// DesignObjVariation_Zone table constants
		//
		public const String DESIGNOBJVARIATION_ZONE_TABLE = "DesignObjVariation_Zone";
		public const String PK_DESIGNOBJVARIATION_ZONE_FIELD = "PK_DesignObjVariation_Zone";
		public const String FK_DESIGNOBJVARIATION_FIELD = "FK_DesignObjVariation";
		public const String FK_BUTTON_FIELD = "FK_Button";
		public const String FK_COMMANDGROUP_D_FIELD = "FK_CommandGroup_D";
		public const String FK_DESIGNOBJ_GOTO_FIELD = "FK_DesignObj_Goto";
		public const String DESCRIPTION_FIELD = "Description";
		public const String X_FIELD = "X";
		public const String Y_FIELD = "Y";
		public const String WIDTH_FIELD = "Width";
		public const String HEIGHT_FIELD = "Height";
		// table+field constants
		public const String PK_DESIGNOBJVARIATION_ZONE_TABLE_FIELD = "DesignObjVariation_Zone.PK_DesignObjVariation_Zone";
		public const String FK_DESIGNOBJVARIATION_TABLE_FIELD = "DesignObjVariation_Zone.FK_DesignObjVariation";
		public const String FK_BUTTON_TABLE_FIELD = "DesignObjVariation_Zone.FK_Button";
		public const String FK_COMMANDGROUP_D_TABLE_FIELD = "DesignObjVariation_Zone.FK_CommandGroup_D";
		public const String FK_DESIGNOBJ_GOTO_TABLE_FIELD = "DesignObjVariation_Zone.FK_DesignObj_Goto";
		public const String DESCRIPTION_TABLE_FIELD = "DesignObjVariation_Zone.Description";
		public const String X_TABLE_FIELD = "DesignObjVariation_Zone.X";
		public const String Y_TABLE_FIELD = "DesignObjVariation_Zone.Y";
		public const String WIDTH_TABLE_FIELD = "DesignObjVariation_Zone.Width";
		public const String HEIGHT_TABLE_FIELD = "DesignObjVariation_Zone.Height";
		// DataSetCommand object
		protected OdbcDataAdapter m_DSCommand;

		// Stored procedure parameters
		protected const String PK_DESIGNOBJVARIATION_ZONE_PARM = "@PK_DesignObjVariation_Zone";
		protected const String FK_DESIGNOBJVARIATION_PARM = "@FK_DesignObjVariation";
		protected const String FK_BUTTON_PARM = "@FK_Button";
		protected const String FK_COMMANDGROUP_D_PARM = "@FK_CommandGroup_D";
		protected const String FK_DESIGNOBJ_GOTO_PARM = "@FK_DesignObj_Goto";
		protected const String DESCRIPTION_PARM = "@Description";
		protected const String X_PARM = "@X";
		protected const String Y_PARM = "@Y";
		protected const String WIDTH_PARM = "@Width";
		protected const String HEIGHT_PARM = "@Height";
		protected const String USERID_PARM = "@UserID";

		protected OdbcCommand m_LoadCommand;
		protected OdbcCommand m_InsertCommand;
		protected OdbcCommand m_UpdateCommand;
		protected OdbcCommand m_DeleteCommand;
		protected OdbcConnection m_Connection;
		protected OdbcTransaction m_Transaction;
		public DataTable Table { get { return Tables[0]; } }


		public DesignObjVariation_ZoneData() {  // marker:1
			//
			// Create the tables in the dataset
			//
			Tables.Add(BuildDataTables());
			m_Connection = HADataConfiguration.GetOdbcConnection();
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

			m_DSCommand.TableMappings.Add("Table", DesignObjVariation_ZoneData.DESIGNOBJVARIATION_ZONE_TABLE);
		}

		public DesignObjVariation_ZoneData(OdbcConnection conn,OdbcTransaction trans) {

			m_Connection = conn;
			m_Transaction = trans;
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

			m_DSCommand.TableMappings.Add("Table", DesignObjVariation_ZoneData.DESIGNOBJVARIATION_ZONE_TABLE);
		}

		private DesignObjVariation_ZoneData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context) {
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
			return (DataTable) BuildDesignObjVariation_ZoneTable();
		}
		public static DesignObjVariation_ZoneTable BuildDesignObjVariation_ZoneTable() {
			//
			// Create the DesignObjVariation_Zone table
			//
			DesignObjVariation_ZoneTable Table = new DesignObjVariation_ZoneTable();
			DataColumnCollection Columns = Table.Columns;
			DataColumn[] PKColumns = new DataColumn[1];

			DataColumn Column = Columns.Add(PK_DESIGNOBJVARIATION_ZONE_FIELD, typeof(System.Int32));
			Column.AutoIncrement = true;
			Column.AutoIncrementSeed = -1;
			Column.AutoIncrementStep = -1;
			PKColumns[0] = Column;

			Columns.Add(FK_DESIGNOBJVARIATION_FIELD, typeof(System.Int32));
			Columns.Add(FK_BUTTON_FIELD, typeof(System.Int32));
			Column = Columns.Add(FK_COMMANDGROUP_D_FIELD, typeof(System.Int32));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Columns.Add(FK_DESIGNOBJ_GOTO_FIELD, typeof(System.Int32));
			Column = Columns.Add(DESCRIPTION_FIELD, typeof(System.String));
			Column.AllowDBNull = false;

			Column = Columns.Add(X_FIELD, typeof(System.Int32));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Column = Columns.Add(Y_FIELD, typeof(System.Int32));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Column = Columns.Add(WIDTH_FIELD, typeof(System.Int32));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Column = Columns.Add(HEIGHT_FIELD, typeof(System.Int32));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Table.PrimaryKey = PKColumns;

			return Table;
		}
		protected static void CreateParameters(OdbcParameterCollection Params, bool IsInsert) {
			Params.Add(new OdbcParameter(PK_DESIGNOBJVARIATION_ZONE_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_DESIGNOBJVARIATION_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_BUTTON_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_COMMANDGROUP_D_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_DESIGNOBJ_GOTO_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(DESCRIPTION_PARM, OdbcType.VarChar, 20));
			Params.Add(new OdbcParameter(X_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(Y_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(WIDTH_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(HEIGHT_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(USERID_PARM, OdbcType.Int));

			// map the parameters to the data table

			Params[PK_DESIGNOBJVARIATION_ZONE_PARM].SourceColumn = DesignObjVariation_ZoneData.PK_DESIGNOBJVARIATION_ZONE_FIELD;
			if(IsInsert) {
				Params[PK_DESIGNOBJVARIATION_ZONE_PARM].Direction = ParameterDirection.Output;
			}

			Params[FK_DESIGNOBJVARIATION_PARM].SourceColumn = DesignObjVariation_ZoneData.FK_DESIGNOBJVARIATION_FIELD;
			Params[FK_BUTTON_PARM].SourceColumn = DesignObjVariation_ZoneData.FK_BUTTON_FIELD;
			Params[FK_COMMANDGROUP_D_PARM].SourceColumn = DesignObjVariation_ZoneData.FK_COMMANDGROUP_D_FIELD;
			Params[FK_DESIGNOBJ_GOTO_PARM].SourceColumn = DesignObjVariation_ZoneData.FK_DESIGNOBJ_GOTO_FIELD;
			Params[DESCRIPTION_PARM].SourceColumn = DesignObjVariation_ZoneData.DESCRIPTION_FIELD;
			Params[X_PARM].SourceColumn = DesignObjVariation_ZoneData.X_FIELD;
			Params[Y_PARM].SourceColumn = DesignObjVariation_ZoneData.Y_FIELD;
			Params[WIDTH_PARM].SourceColumn = DesignObjVariation_ZoneData.WIDTH_FIELD;
			Params[HEIGHT_PARM].SourceColumn = DesignObjVariation_ZoneData.HEIGHT_FIELD;
		}

		protected static void CreateCommands(OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
			if(LoadCommand == null) {
				// Create the command since it's null
				LoadCommand = new OdbcCommand("sp_Select_DesignObjVariation_Zone", Conn);
				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(PK_DESIGNOBJVARIATION_ZONE_PARM, OdbcType.Int,4));
			}

			if(InsertCommand == null) {
				// Create the command since it's null
				InsertCommand = new OdbcCommand("sp_Insert_DesignObjVariation_Zone", Conn);
				InsertCommand.CommandType = CommandType.StoredProcedure;
				InsertCommand.Transaction = Trans;

				OdbcParameterCollection Params = InsertCommand.Parameters;

				CreateParameters(Params, true);

			}

			if(UpdateCommand == null) {
				// Create the command since it's null
				UpdateCommand = new OdbcCommand("sp_Update_DesignObjVariation_Zone", Conn);
				UpdateCommand.CommandType = CommandType.StoredProcedure;
				UpdateCommand.Transaction = Trans;

				OdbcParameterCollection Params = UpdateCommand.Parameters;

				CreateParameters(Params, false);

			}
			if (DeleteCommand == null)
			{
				DeleteCommand = new OdbcCommand("sp_Delete_DesignObjVariation_Zone", Conn);
				DeleteCommand.CommandType = CommandType.StoredProcedure;
				DeleteCommand.Transaction = Trans;

				DeleteCommand.Parameters.Add(PK_DESIGNOBJVARIATION_ZONE_PARM, OdbcType.Int,4, PK_DESIGNOBJVARIATION_ZONE_FIELD);
				DeleteCommand.Parameters.Add(USERID_PARM, OdbcType.Int);
			}
		}

		protected static void CreateCommands(OdbcDataAdapter odbcda,OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
				LoadCommand = new OdbcCommand("SELECT PK_DesignObjVariation_Zone,FK_DesignObjVariation,FK_Button,FK_CommandGroup_D,FK_DesignObj_Goto,Description,X,Y,Width,Height FROM DesignObjVariation_Zone", Conn);
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(PK_DESIGNOBJVARIATION_ZONE_PARM, OdbcType.Int,4));

			odbcda.SelectCommand = LoadCommand;
			OdbcCommandBuilder odbcCB = new OdbcCommandBuilder(odbcda);
			odbcCB.RefreshSchema();
			DeleteCommand = odbcCB.GetDeleteCommand();
			InsertCommand = odbcCB.GetInsertCommand();
			UpdateCommand = odbcCB.GetUpdateCommand();
		}

		public DesignObjVariation_ZoneData LoadDesignObjVariation_Zone(System.Int32 PK_DesignObjVariation_Zone)
		{
			m_DSCommand.SelectCommand = m_LoadCommand;
			m_DSCommand.SelectCommand.Parameters[PK_DESIGNOBJVARIATION_ZONE_PARM].Value = PK_DesignObjVariation_Zone;

			m_DSCommand.Fill(this);
			return this;
		}

		public static DataRowCollection LoadDesignObjVariation_ZoneWithWhere(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, string WhereClause) // marker:2
		{
			DataRowCollection dr;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["DesignObjVariation_Zone"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}
			
			DataSet dsTemp = new MyDataSet();
			dsTemp.Tables.Add(BuildDataTables());
			
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			string sSQL = "SELECT PK_DesignObjVariation_Zone, FK_DesignObjVariation, FK_Button, FK_CommandGroup_D, FK_DesignObj_Goto, Description, X, Y, Width, Height FROM DesignObjVariation_Zone WHERE " + WhereClause;
			
			OdbcCommand LoadCommand = new OdbcCommand(sSQL,conn);
			
			if( trans!=null )
				LoadCommand.Transaction = trans;
			
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(dsTemp,"DesignObjVariation_Zone");
			
			dr=dsTemp.Tables["DesignObjVariation_Zone"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}

		public static DataRow LoadNoCacheDesignObjVariation_Zone(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 PK_DesignObjVariation_Zone)
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["DesignObjVariation_Zone"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}

			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand;
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();

			LoadCommand = new OdbcCommand("sp_Select_DesignObjVariation_Zone", conn);

			LoadCommand.CommandType = CommandType.StoredProcedure;
			LoadCommand.Parameters.Add(new OdbcParameter(PK_DESIGNOBJVARIATION_ZONE_PARM, OdbcType.Int,4));
			LoadCommand.Parameters[PK_DESIGNOBJVARIATION_ZONE_PARM].Value = PK_DesignObjVariation_Zone;
			if( trans!=null )
				LoadCommand.Transaction = trans;
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(ds,"DesignObjVariation_Zone");
				dr = ds.Tables["DesignObjVariation_Zone"].Rows.Find(PK_DesignObjVariation_Zone);
			return dr;
		}

		public static DataRow LoadDesignObjVariation_Zone(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 PK_DesignObjVariation_Zone)  // marker:3
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["DesignObjVariation_Zone"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
				else
				dr = dt.Rows.Find(PK_DesignObjVariation_Zone);
			}

			if( dr==null )
			{
				OdbcDataAdapter sqlda = new OdbcDataAdapter();
				OdbcCommand LoadCommand;
				if( conn==null )
					conn = HADataConfiguration.GetOdbcConnection();

				LoadCommand = new OdbcCommand("sp_Select_DesignObjVariation_Zone", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new OdbcParameter(PK_DESIGNOBJVARIATION_ZONE_PARM, OdbcType.Int,4));
				LoadCommand.Parameters[PK_DESIGNOBJVARIATION_ZONE_PARM].Value = PK_DesignObjVariation_Zone;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(ds,"DesignObjVariation_Zone");
				dr = ds.Tables["DesignObjVariation_Zone"].Rows.Find(PK_DesignObjVariation_Zone);
			}
			return dr;
		}

		public DesignObjVariation_ZoneData LoadAll() {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new OdbcCommand("SELECT * FROM DesignObjVariation_Zone", m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this);
			return this;

		}

		public static DataRowCollection LoadAll(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {

			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT * FROM DesignObjVariation_Zone", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(ds,"DesignObjVariation_Zone")==0 )
				return null;
			else
				return ds.Tables["DesignObjVariation_Zone"].Rows;

		}

		public DesignObjVariation_ZoneData ExecuteQuery(String sSQL) {
			return ExecuteQuery(sSQL,DESIGNOBJVARIATION_ZONE_TABLE);
		}
		public DesignObjVariation_ZoneData ExecuteQuery(String sSQL,String sTableName) {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new OdbcCommand(sSQL, m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this,sTableName);

			return this;
		}

		public static DataRowCollection ExecuteQuery(String sSQL,ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {
			return ExecuteQuery(sSQL,ref ds,conn,trans,"DesignObjVariation_Zone");
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

		public bool UpdateDesignObjVariation_Zone(int CurUserID) {
			m_DSCommand.UpdateCommand = m_UpdateCommand;
			m_DSCommand.UpdateCommand.Parameters[USERID_PARM].Value = CurUserID;

			m_DSCommand.InsertCommand = m_InsertCommand;
			m_DSCommand.InsertCommand.Parameters[USERID_PARM].Value = CurUserID;

			if( m_DeleteCommand != null )
			{
				m_DSCommand.DeleteCommand = m_DeleteCommand;
				m_DSCommand.DeleteCommand.Parameters[USERID_PARM].Value = CurUserID;
			}

			m_DSCommand.Update(this, DesignObjVariation_ZoneData.DESIGNOBJVARIATION_ZONE_TABLE);
			return true;
		}

		public static bool UpdateDesignObjVariation_Zone(ref MyDataSet ds, int CurUserID)
		{
			OdbcConnection OdbcConn = HADataConfiguration.GetOdbcConnection();
			return UpdateDesignObjVariation_Zone(ref ds,CurUserID,OdbcConn,null);
		}

		public static bool UpdateDesignObjVariation_Zone(ref MyDataSet ds, int CurUserID,OdbcConnection OdbcConn,OdbcTransaction Trans)
		{
			DataTable dt = ds.Tables[DESIGNOBJVARIATION_ZONE_TABLE];
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

	} // public class DesignObjVariation_ZoneData
	public class DesignObjVariation_ZoneDataRow
	{
		public DataRow dr = null;
		public DesignObjVariation_ZoneDataRow(DataRow d)
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
		public System.Int32 fPK_DesignObjVariation_Zone
		{
			get
			{
				return Convert.ToInt32(dr[0]);
			}
		}
		public System.Int32 fFK_DesignObjVariation
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
		public bool fFK_DesignObjVariationIsNull
		{
			get
			{
				return dr[1]==DBNull.Value;
			}
		}
		public void fFK_DesignObjVariationSetNull()
		{
			dr[1]=DBNull.Value;
		}
		public DesignObjVariationDataRow fFK_DesignObjVariation_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tDesignObjVariation[Convert.ToInt32(dr[1])];
			}
		}
		public System.Int32 fFK_Button
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
		public bool fFK_ButtonIsNull
		{
			get
			{
				return dr[2]==DBNull.Value;
			}
		}
		public void fFK_ButtonSetNull()
		{
			dr[2]=DBNull.Value;
		}
		public ButtonDataRow fFK_Button_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tButton[Convert.ToInt32(dr[2])];
			}
		}
		public System.Int32 fFK_CommandGroup_D
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
		public CommandGroup_DDataRow fFK_CommandGroup_D_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tCommandGroup_D[Convert.ToInt32(dr[3])];
			}
		}
		public System.Int32 fFK_DesignObj_Goto
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
		public bool fFK_DesignObj_GotoIsNull
		{
			get
			{
				return dr[4]==DBNull.Value;
			}
		}
		public void fFK_DesignObj_GotoSetNull()
		{
			dr[4]=DBNull.Value;
		}
		public DesignObjDataRow fFK_DesignObj_Goto_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tDesignObj[Convert.ToInt32(dr[4])];
			}
		}
		public System.String fDescription
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
		public System.Int32 fX
		{
			get
			{
				return Convert.ToInt32(dr[6]);
			}
			set
			{
				dr[6]=value;
			}
		}
		public System.Int32 fY
		{
			get
			{
				return Convert.ToInt32(dr[7]);
			}
			set
			{
				dr[7]=value;
			}
		}
		public System.Int32 fWidth
		{
			get
			{
				return Convert.ToInt32(dr[8]);
			}
			set
			{
				dr[8]=value;
			}
		}
		public System.Int32 fHeight
		{
			get
			{
				return Convert.ToInt32(dr[9]);
			}
			set
			{
				dr[9]=value;
			}
		}
	} // public class DesignObjVariation_ZoneDataRow
	public class DesignObjVariation_ZoneDataReader
	{
		public OdbcDataReader dr;
		bool bCache=false;
		int iRecord=-1,iNumRecords=-1;
		ArrayList al = null;

		public DesignObjVariation_ZoneDataReader(OdbcDataReader d)
		{
			dr=d;
		}
		public DesignObjVariation_ZoneDataReader(OdbcCommand cmd)
		{
			dr = cmd.ExecuteReader();
		}

		public DesignObjVariation_ZoneDataReader(OdbcCommand cmd,bool Cache)
		{
			dr = cmd.ExecuteReader();
			bCache=Cache;
			if( bCache )
				CacheAndClose();
		}

		public DesignObjVariation_ZoneDataReader(string sSQL)
		{
			OdbcConnection conn = HADataConfiguration.GetOdbcConnection();

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM DesignObjVariation_Zone WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public DesignObjVariation_ZoneDataReader(string sSQL,OdbcConnection conn)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM DesignObjVariation_Zone WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public DesignObjVariation_ZoneDataReader(string sSQL,OdbcConnection conn,OdbcTransaction trans,bool Cache)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM DesignObjVariation_Zone WHERE " + sSQL;
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
				object[] objs = new object[10];
				for(int i=0;i<10;i++)
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
		public System.Int32 fPK_DesignObjVariation_Zone
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[0]);
				else
					return Convert.ToInt32(dr[0]);
			}
		}
		public System.Int32 fFK_DesignObjVariation
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[1]);
				else
					return Convert.ToInt32(dr[1]);
			}
		}
		public bool fFK_DesignObjVariationIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[1]==DBNull.Value;
				else
					return dr[1]==DBNull.Value;
			}
		}
		public System.Int32 fFK_Button
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[2]);
				else
					return Convert.ToInt32(dr[2]);
			}
		}
		public bool fFK_ButtonIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[2]==DBNull.Value;
				else
					return dr[2]==DBNull.Value;
			}
		}
		public System.Int32 fFK_CommandGroup_D
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[3]);
				else
					return Convert.ToInt32(dr[3]);
			}
		}
		public System.Int32 fFK_DesignObj_Goto
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[4]);
				else
					return Convert.ToInt32(dr[4]);
			}
		}
		public bool fFK_DesignObj_GotoIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[4]==DBNull.Value;
				else
					return dr[4]==DBNull.Value;
			}
		}
		public System.String fDescription
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[5]);
				else
					return Convert.ToString(dr[5]);
			}
		}
		public System.Int32 fX
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[6]);
				else
					return Convert.ToInt32(dr[6]);
			}
		}
		public System.Int32 fY
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[7]);
				else
					return Convert.ToInt32(dr[7]);
			}
		}
		public System.Int32 fWidth
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[8]);
				else
					return Convert.ToInt32(dr[8]);
			}
		}
		public System.Int32 fHeight
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[9]);
				else
					return Convert.ToInt32(dr[9]);
			}
		}
	} // public class DesignObjVariation_ZoneDataReader
	public class DesignObjVariation_ZoneTable : DataTable
	{
		public DesignObjVariation_ZoneTable() : base("DesignObjVariation_Zone") {}

		public DesignObjVariation_ZoneDataRow this [System.Int32 PK_DesignObjVariation_Zone]
		{
			get
			{
				DesignObjVariation_ZoneDataRow dr = new DesignObjVariation_ZoneDataRow(Rows.Find(PK_DesignObjVariation_Zone));
				if( !dr.bIsValid  && false /* can't do this with ODBC */  )
				{
					MyDataSet mds = (MyDataSet) DataSet;
					if( mds.m_conn==null )
						return dr;
					OdbcDataAdapter sqlda = new OdbcDataAdapter();
					OdbcCommand LoadCommand = new OdbcCommand("sp_Select_DesignObjVariation_Zone", mds.m_conn,mds.m_trans);
					LoadCommand.CommandType = CommandType.StoredProcedure;
					LoadCommand.Parameters.Add(new OdbcParameter("@PK_DesignObjVariation_Zone", OdbcType.Int,4));
					LoadCommand.Parameters["@PK_DesignObjVariation_Zone"].Value = PK_DesignObjVariation_Zone;
					sqlda.SelectCommand = LoadCommand;
					sqlda.Fill(mds,"DesignObjVariation_Zone");
					dr = new DesignObjVariation_ZoneDataRow(Rows.Find(PK_DesignObjVariation_Zone));
				}
				return dr;
			}
		}
		public DataRowCollection LoadAll(OdbcConnection conn, OdbcTransaction trans)
		{
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT PK_DesignObjVariation_Zone,FK_DesignObjVariation,FK_Button,FK_CommandGroup_D,FK_DesignObj_Goto,Description,X,Y,Width,Height FROM DesignObjVariation_Zone", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(this.DataSet,"DesignObjVariation_Zone")==0 )
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
			DesignObjVariation_ZoneData.UpdateDesignObjVariation_Zone(ref ds,PK_Users,conn,trans);
		}
		public DataColumn cPK_DesignObjVariation_Zone
		{
			get
			{
				return Columns[0];
			}
		}
		public DataColumn cFK_DesignObjVariation
		{
			get
			{
				return Columns[1];
			}
		}
		public DataColumn cFK_Button
		{
			get
			{
				return Columns[2];
			}
		}
		public DataColumn cFK_CommandGroup_D
		{
			get
			{
				return Columns[3];
			}
		}
		public DataColumn cFK_DesignObj_Goto
		{
			get
			{
				return Columns[4];
			}
		}
		public DataColumn cDescription
		{
			get
			{
				return Columns[5];
			}
		}
		public DataColumn cX
		{
			get
			{
				return Columns[6];
			}
		}
		public DataColumn cY
		{
			get
			{
				return Columns[7];
			}
		}
		public DataColumn cWidth
		{
			get
			{
				return Columns[8];
			}
		}
		public DataColumn cHeight
		{
			get
			{
				return Columns[9];
			}
		}
	}
} // namespace HAData.Common.Data
