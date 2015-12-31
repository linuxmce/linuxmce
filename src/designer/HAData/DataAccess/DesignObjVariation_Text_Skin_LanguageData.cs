namespace HAData.DataAccess {
	using System;
	using System.Data;
	using Microsoft.Data.Odbc;
	using System.Collections;

	using HAData.Common;

	public class DesignObjVariation_Text_Skin_LanguageData : MyDataSet {
		//
		// DesignObjVariation_Text_Skin_Language table constants
		//
		public const String DESIGNOBJVARIATION_TEXT_SKIN_LANGUAGE_TABLE = "DesignObjVariation_Text_Skin_Language";
		public const String PK_DESIGNOBJVARIATION_TEXT_SKIN_LANGUAGE_FIELD = "PK_DesignObjVariation_Text_Skin_Language";
		public const String FK_DESIGNOBJVARIATION_TEXT_FIELD = "FK_DesignObjVariation_Text";
		public const String FK_SKIN_FIELD = "FK_Skin";
		public const String FK_LANGUAGE_FIELD = "FK_Language";
		public const String X_FIELD = "X";
		public const String Y_FIELD = "Y";
		public const String WIDTH_FIELD = "Width";
		public const String HEIGHT_FIELD = "Height";
		public const String ROTATE_FIELD = "Rotate";
		public const String OPACITY_FIELD = "Opacity";
		public const String FK_HORIZALIGNMENT_FIELD = "FK_HorizAlignment";
		public const String FK_VERTALIGNMENT_FIELD = "FK_VertAlignment";
		public const String FK_STYLE_FIELD = "FK_Style";
		public const String PLAINBACKGROUNDCOLOR_FIELD = "PlainBackgroundColor";
		// table+field constants
		public const String PK_DESIGNOBJVARIATION_TEXT_SKIN_LANGUAGE_TABLE_FIELD = "DesignObjVariation_Text_Skin_Language.PK_DesignObjVariation_Text_Skin_Language";
		public const String FK_DESIGNOBJVARIATION_TEXT_TABLE_FIELD = "DesignObjVariation_Text_Skin_Language.FK_DesignObjVariation_Text";
		public const String FK_SKIN_TABLE_FIELD = "DesignObjVariation_Text_Skin_Language.FK_Skin";
		public const String FK_LANGUAGE_TABLE_FIELD = "DesignObjVariation_Text_Skin_Language.FK_Language";
		public const String X_TABLE_FIELD = "DesignObjVariation_Text_Skin_Language.X";
		public const String Y_TABLE_FIELD = "DesignObjVariation_Text_Skin_Language.Y";
		public const String WIDTH_TABLE_FIELD = "DesignObjVariation_Text_Skin_Language.Width";
		public const String HEIGHT_TABLE_FIELD = "DesignObjVariation_Text_Skin_Language.Height";
		public const String ROTATE_TABLE_FIELD = "DesignObjVariation_Text_Skin_Language.Rotate";
		public const String OPACITY_TABLE_FIELD = "DesignObjVariation_Text_Skin_Language.Opacity";
		public const String FK_HORIZALIGNMENT_TABLE_FIELD = "DesignObjVariation_Text_Skin_Language.FK_HorizAlignment";
		public const String FK_VERTALIGNMENT_TABLE_FIELD = "DesignObjVariation_Text_Skin_Language.FK_VertAlignment";
		public const String FK_STYLE_TABLE_FIELD = "DesignObjVariation_Text_Skin_Language.FK_Style";
		public const String PLAINBACKGROUNDCOLOR_TABLE_FIELD = "DesignObjVariation_Text_Skin_Language.PlainBackgroundColor";
		// DataSetCommand object
		protected OdbcDataAdapter m_DSCommand;

		// Stored procedure parameters
		protected const String PK_DESIGNOBJVARIATION_TEXT_SKIN_LANGUAGE_PARM = "@PK_DesignObjVariation_Text_Skin_Language";
		protected const String FK_DESIGNOBJVARIATION_TEXT_PARM = "@FK_DesignObjVariation_Text";
		protected const String FK_SKIN_PARM = "@FK_Skin";
		protected const String FK_LANGUAGE_PARM = "@FK_Language";
		protected const String X_PARM = "@X";
		protected const String Y_PARM = "@Y";
		protected const String WIDTH_PARM = "@Width";
		protected const String HEIGHT_PARM = "@Height";
		protected const String ROTATE_PARM = "@Rotate";
		protected const String OPACITY_PARM = "@Opacity";
		protected const String FK_HORIZALIGNMENT_PARM = "@FK_HorizAlignment";
		protected const String FK_VERTALIGNMENT_PARM = "@FK_VertAlignment";
		protected const String FK_STYLE_PARM = "@FK_Style";
		protected const String PLAINBACKGROUNDCOLOR_PARM = "@PlainBackgroundColor";
		protected const String USERID_PARM = "@UserID";

		protected OdbcCommand m_LoadCommand;
		protected OdbcCommand m_InsertCommand;
		protected OdbcCommand m_UpdateCommand;
		protected OdbcCommand m_DeleteCommand;
		protected OdbcConnection m_Connection;
		protected OdbcTransaction m_Transaction;
		public DataTable Table { get { return Tables[0]; } }


		public DesignObjVariation_Text_Skin_LanguageData() {  // marker:1
			//
			// Create the tables in the dataset
			//
			Tables.Add(BuildDataTables());
			m_Connection = HADataConfiguration.GetOdbcConnection();
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

			m_DSCommand.TableMappings.Add("Table", DesignObjVariation_Text_Skin_LanguageData.DESIGNOBJVARIATION_TEXT_SKIN_LANGUAGE_TABLE);
		}

		public DesignObjVariation_Text_Skin_LanguageData(OdbcConnection conn,OdbcTransaction trans) {

			m_Connection = conn;
			m_Transaction = trans;
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

			m_DSCommand.TableMappings.Add("Table", DesignObjVariation_Text_Skin_LanguageData.DESIGNOBJVARIATION_TEXT_SKIN_LANGUAGE_TABLE);
		}

		private DesignObjVariation_Text_Skin_LanguageData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context) {
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
			return (DataTable) BuildDesignObjVariation_Text_Skin_LanguageTable();
		}
		public static DesignObjVariation_Text_Skin_LanguageTable BuildDesignObjVariation_Text_Skin_LanguageTable() {
			//
			// Create the DesignObjVariation_Text_Skin_Language table
			//
			DesignObjVariation_Text_Skin_LanguageTable Table = new DesignObjVariation_Text_Skin_LanguageTable();
			DataColumnCollection Columns = Table.Columns;
			DataColumn[] PKColumns = new DataColumn[1];

			DataColumn Column = Columns.Add(PK_DESIGNOBJVARIATION_TEXT_SKIN_LANGUAGE_FIELD, typeof(System.Int32));
			Column.AutoIncrement = true;
			Column.AutoIncrementSeed = -1;
			Column.AutoIncrementStep = -1;
			PKColumns[0] = Column;

			Column = Columns.Add(FK_DESIGNOBJVARIATION_TEXT_FIELD, typeof(System.Int32));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Columns.Add(FK_SKIN_FIELD, typeof(System.Int32));
			Columns.Add(FK_LANGUAGE_FIELD, typeof(System.Int32));
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

			Columns.Add(ROTATE_FIELD, typeof(System.Int32));
			Columns.Add(OPACITY_FIELD, typeof(System.Int32));
			Columns.Add(FK_HORIZALIGNMENT_FIELD, typeof(System.Int32));
			Columns.Add(FK_VERTALIGNMENT_FIELD, typeof(System.Int32));
			Columns.Add(FK_STYLE_FIELD, typeof(System.Int32));
			Columns.Add(PLAINBACKGROUNDCOLOR_FIELD, typeof(System.Int32));
			Table.PrimaryKey = PKColumns;

			return Table;
		}
		protected static void CreateParameters(OdbcParameterCollection Params, bool IsInsert) {
			Params.Add(new OdbcParameter(PK_DESIGNOBJVARIATION_TEXT_SKIN_LANGUAGE_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_DESIGNOBJVARIATION_TEXT_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_SKIN_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_LANGUAGE_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(X_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(Y_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(WIDTH_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(HEIGHT_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(ROTATE_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(OPACITY_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_HORIZALIGNMENT_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_VERTALIGNMENT_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_STYLE_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(PLAINBACKGROUNDCOLOR_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(USERID_PARM, OdbcType.Int));

			// map the parameters to the data table

			Params[PK_DESIGNOBJVARIATION_TEXT_SKIN_LANGUAGE_PARM].SourceColumn = DesignObjVariation_Text_Skin_LanguageData.PK_DESIGNOBJVARIATION_TEXT_SKIN_LANGUAGE_FIELD;
			if(IsInsert) {
				Params[PK_DESIGNOBJVARIATION_TEXT_SKIN_LANGUAGE_PARM].Direction = ParameterDirection.Output;
			}

			Params[FK_DESIGNOBJVARIATION_TEXT_PARM].SourceColumn = DesignObjVariation_Text_Skin_LanguageData.FK_DESIGNOBJVARIATION_TEXT_FIELD;
			Params[FK_SKIN_PARM].SourceColumn = DesignObjVariation_Text_Skin_LanguageData.FK_SKIN_FIELD;
			Params[FK_LANGUAGE_PARM].SourceColumn = DesignObjVariation_Text_Skin_LanguageData.FK_LANGUAGE_FIELD;
			Params[X_PARM].SourceColumn = DesignObjVariation_Text_Skin_LanguageData.X_FIELD;
			Params[Y_PARM].SourceColumn = DesignObjVariation_Text_Skin_LanguageData.Y_FIELD;
			Params[WIDTH_PARM].SourceColumn = DesignObjVariation_Text_Skin_LanguageData.WIDTH_FIELD;
			Params[HEIGHT_PARM].SourceColumn = DesignObjVariation_Text_Skin_LanguageData.HEIGHT_FIELD;
			Params[ROTATE_PARM].SourceColumn = DesignObjVariation_Text_Skin_LanguageData.ROTATE_FIELD;
			Params[OPACITY_PARM].SourceColumn = DesignObjVariation_Text_Skin_LanguageData.OPACITY_FIELD;
			Params[FK_HORIZALIGNMENT_PARM].SourceColumn = DesignObjVariation_Text_Skin_LanguageData.FK_HORIZALIGNMENT_FIELD;
			Params[FK_VERTALIGNMENT_PARM].SourceColumn = DesignObjVariation_Text_Skin_LanguageData.FK_VERTALIGNMENT_FIELD;
			Params[FK_STYLE_PARM].SourceColumn = DesignObjVariation_Text_Skin_LanguageData.FK_STYLE_FIELD;
			Params[PLAINBACKGROUNDCOLOR_PARM].SourceColumn = DesignObjVariation_Text_Skin_LanguageData.PLAINBACKGROUNDCOLOR_FIELD;
		}

		protected static void CreateCommands(OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
			if(LoadCommand == null) {
				// Create the command since it's null
				LoadCommand = new OdbcCommand("sp_Select_DesignObjVariation_Text_Skin_Language", Conn);
				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(PK_DESIGNOBJVARIATION_TEXT_SKIN_LANGUAGE_PARM, OdbcType.Int,4));
			}

			if(InsertCommand == null) {
				// Create the command since it's null
				InsertCommand = new OdbcCommand("sp_Insert_DesignObjVariation_Text_Skin_Language", Conn);
				InsertCommand.CommandType = CommandType.StoredProcedure;
				InsertCommand.Transaction = Trans;

				OdbcParameterCollection Params = InsertCommand.Parameters;

				CreateParameters(Params, true);

			}

			if(UpdateCommand == null) {
				// Create the command since it's null
				UpdateCommand = new OdbcCommand("sp_Update_DesignObjVariation_Text_Skin_Language", Conn);
				UpdateCommand.CommandType = CommandType.StoredProcedure;
				UpdateCommand.Transaction = Trans;

				OdbcParameterCollection Params = UpdateCommand.Parameters;

				CreateParameters(Params, false);

			}
			if (DeleteCommand == null)
			{
				DeleteCommand = new OdbcCommand("sp_Delete_DesignObjVariation_Text_Skin_Language", Conn);
				DeleteCommand.CommandType = CommandType.StoredProcedure;
				DeleteCommand.Transaction = Trans;

				DeleteCommand.Parameters.Add(PK_DESIGNOBJVARIATION_TEXT_SKIN_LANGUAGE_PARM, OdbcType.Int,4, PK_DESIGNOBJVARIATION_TEXT_SKIN_LANGUAGE_FIELD);
				DeleteCommand.Parameters.Add(USERID_PARM, OdbcType.Int);
			}
		}

		protected static void CreateCommands(OdbcDataAdapter odbcda,OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
				LoadCommand = new OdbcCommand("SELECT PK_DesignObjVariation_Text_Skin_Language,FK_DesignObjVariation_Text,FK_Skin,FK_Language,X,Y,Width,Height,Rotate,Opacity,FK_HorizAlignment,FK_VertAlignment,FK_Style,PlainBackgroundColor FROM DesignObjVariation_Text_Skin_Language", Conn);
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(PK_DESIGNOBJVARIATION_TEXT_SKIN_LANGUAGE_PARM, OdbcType.Int,4));

			odbcda.SelectCommand = LoadCommand;
			OdbcCommandBuilder odbcCB = new OdbcCommandBuilder(odbcda);
			odbcCB.RefreshSchema();
			DeleteCommand = odbcCB.GetDeleteCommand();
			InsertCommand = odbcCB.GetInsertCommand();
			UpdateCommand = odbcCB.GetUpdateCommand();
		}

		public DesignObjVariation_Text_Skin_LanguageData LoadDesignObjVariation_Text_Skin_Language(System.Int32 PK_DesignObjVariation_Text_Skin_Language)
		{
			m_DSCommand.SelectCommand = m_LoadCommand;
			m_DSCommand.SelectCommand.Parameters[PK_DESIGNOBJVARIATION_TEXT_SKIN_LANGUAGE_PARM].Value = PK_DesignObjVariation_Text_Skin_Language;

			m_DSCommand.Fill(this);
			return this;
		}

		public static DataRowCollection LoadDesignObjVariation_Text_Skin_LanguageWithWhere(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, string WhereClause) // marker:2
		{
			DataRowCollection dr;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["DesignObjVariation_Text_Skin_Language"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}
			
			DataSet dsTemp = new MyDataSet();
			dsTemp.Tables.Add(BuildDataTables());
			
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			string sSQL = "SELECT PK_DesignObjVariation_Text_Skin_Language, FK_DesignObjVariation_Text, FK_Skin, FK_Language, X, Y, Width, Height, Rotate, Opacity, FK_HorizAlignment, FK_VertAlignment, FK_Style, PlainBackgroundColor FROM DesignObjVariation_Text_Skin_Language WHERE " + WhereClause;
			
			OdbcCommand LoadCommand = new OdbcCommand(sSQL,conn);
			
			if( trans!=null )
				LoadCommand.Transaction = trans;
			
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(dsTemp,"DesignObjVariation_Text_Skin_Language");
			
			dr=dsTemp.Tables["DesignObjVariation_Text_Skin_Language"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}

		public static DataRow LoadNoCacheDesignObjVariation_Text_Skin_Language(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 PK_DesignObjVariation_Text_Skin_Language)
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["DesignObjVariation_Text_Skin_Language"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}

			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand;
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();

			LoadCommand = new OdbcCommand("sp_Select_DesignObjVariation_Text_Skin_Language", conn);

			LoadCommand.CommandType = CommandType.StoredProcedure;
			LoadCommand.Parameters.Add(new OdbcParameter(PK_DESIGNOBJVARIATION_TEXT_SKIN_LANGUAGE_PARM, OdbcType.Int,4));
			LoadCommand.Parameters[PK_DESIGNOBJVARIATION_TEXT_SKIN_LANGUAGE_PARM].Value = PK_DesignObjVariation_Text_Skin_Language;
			if( trans!=null )
				LoadCommand.Transaction = trans;
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(ds,"DesignObjVariation_Text_Skin_Language");
				dr = ds.Tables["DesignObjVariation_Text_Skin_Language"].Rows.Find(PK_DesignObjVariation_Text_Skin_Language);
			return dr;
		}

		public static DataRow LoadDesignObjVariation_Text_Skin_Language(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 PK_DesignObjVariation_Text_Skin_Language)  // marker:3
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["DesignObjVariation_Text_Skin_Language"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
				else
				dr = dt.Rows.Find(PK_DesignObjVariation_Text_Skin_Language);
			}

			if( dr==null )
			{
				OdbcDataAdapter sqlda = new OdbcDataAdapter();
				OdbcCommand LoadCommand;
				if( conn==null )
					conn = HADataConfiguration.GetOdbcConnection();

				LoadCommand = new OdbcCommand("sp_Select_DesignObjVariation_Text_Skin_Language", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new OdbcParameter(PK_DESIGNOBJVARIATION_TEXT_SKIN_LANGUAGE_PARM, OdbcType.Int,4));
				LoadCommand.Parameters[PK_DESIGNOBJVARIATION_TEXT_SKIN_LANGUAGE_PARM].Value = PK_DesignObjVariation_Text_Skin_Language;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(ds,"DesignObjVariation_Text_Skin_Language");
				dr = ds.Tables["DesignObjVariation_Text_Skin_Language"].Rows.Find(PK_DesignObjVariation_Text_Skin_Language);
			}
			return dr;
		}

		public DesignObjVariation_Text_Skin_LanguageData LoadAll() {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new OdbcCommand("SELECT * FROM DesignObjVariation_Text_Skin_Language", m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this);
			return this;

		}

		public static DataRowCollection LoadAll(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {

			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT * FROM DesignObjVariation_Text_Skin_Language", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(ds,"DesignObjVariation_Text_Skin_Language")==0 )
				return null;
			else
				return ds.Tables["DesignObjVariation_Text_Skin_Language"].Rows;

		}

		public DesignObjVariation_Text_Skin_LanguageData ExecuteQuery(String sSQL) {
			return ExecuteQuery(sSQL,DESIGNOBJVARIATION_TEXT_SKIN_LANGUAGE_TABLE);
		}
		public DesignObjVariation_Text_Skin_LanguageData ExecuteQuery(String sSQL,String sTableName) {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new OdbcCommand(sSQL, m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this,sTableName);

			return this;
		}

		public static DataRowCollection ExecuteQuery(String sSQL,ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {
			return ExecuteQuery(sSQL,ref ds,conn,trans,"DesignObjVariation_Text_Skin_Language");
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

		public bool UpdateDesignObjVariation_Text_Skin_Language(int CurUserID) {
			m_DSCommand.UpdateCommand = m_UpdateCommand;
			m_DSCommand.UpdateCommand.Parameters[USERID_PARM].Value = CurUserID;

			m_DSCommand.InsertCommand = m_InsertCommand;
			m_DSCommand.InsertCommand.Parameters[USERID_PARM].Value = CurUserID;

			if( m_DeleteCommand != null )
			{
				m_DSCommand.DeleteCommand = m_DeleteCommand;
				m_DSCommand.DeleteCommand.Parameters[USERID_PARM].Value = CurUserID;
			}

			m_DSCommand.Update(this, DesignObjVariation_Text_Skin_LanguageData.DESIGNOBJVARIATION_TEXT_SKIN_LANGUAGE_TABLE);
			return true;
		}

		public static bool UpdateDesignObjVariation_Text_Skin_Language(ref MyDataSet ds, int CurUserID)
		{
			OdbcConnection OdbcConn = HADataConfiguration.GetOdbcConnection();
			return UpdateDesignObjVariation_Text_Skin_Language(ref ds,CurUserID,OdbcConn,null);
		}

		public static bool UpdateDesignObjVariation_Text_Skin_Language(ref MyDataSet ds, int CurUserID,OdbcConnection OdbcConn,OdbcTransaction Trans)
		{
			DataTable dt = ds.Tables[DESIGNOBJVARIATION_TEXT_SKIN_LANGUAGE_TABLE];
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

	} // public class DesignObjVariation_Text_Skin_LanguageData
	public class DesignObjVariation_Text_Skin_LanguageDataRow
	{
		public DataRow dr = null;
		public DesignObjVariation_Text_Skin_LanguageDataRow(DataRow d)
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
		public System.Int32 fPK_DesignObjVariation_Text_Skin_Language
		{
			get
			{
				return Convert.ToInt32(dr[0]);
			}
		}
		public System.Int32 fFK_DesignObjVariation_Text
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
		public DesignObjVariation_TextDataRow fFK_DesignObjVariation_Text_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tDesignObjVariation_Text[Convert.ToInt32(dr[1])];
			}
		}
		public System.Int32 fFK_Skin
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
		public bool fFK_SkinIsNull
		{
			get
			{
				return dr[2]==DBNull.Value;
			}
		}
		public void fFK_SkinSetNull()
		{
			dr[2]=DBNull.Value;
		}
		public SkinDataRow fFK_Skin_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tSkin[Convert.ToInt32(dr[2])];
			}
		}
		public System.Int32 fFK_Language
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
		public bool fFK_LanguageIsNull
		{
			get
			{
				return dr[3]==DBNull.Value;
			}
		}
		public void fFK_LanguageSetNull()
		{
			dr[3]=DBNull.Value;
		}
		public LanguageDataRow fFK_Language_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tLanguage[Convert.ToInt32(dr[3])];
			}
		}
		public System.Int32 fX
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
		public System.Int32 fY
		{
			get
			{
				return Convert.ToInt32(dr[5]);
			}
			set
			{
				dr[5]=value;
			}
		}
		public System.Int32 fWidth
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
		public System.Int32 fHeight
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
		public System.Int32 fRotate
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
		public bool fRotateIsNull
		{
			get
			{
				return dr[8]==DBNull.Value;
			}
		}
		public void fRotateSetNull()
		{
			dr[8]=DBNull.Value;
		}
		public System.Int32 fOpacity
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
		public bool fOpacityIsNull
		{
			get
			{
				return dr[9]==DBNull.Value;
			}
		}
		public void fOpacitySetNull()
		{
			dr[9]=DBNull.Value;
		}
		public System.Int32 fFK_HorizAlignment
		{
			get
			{
				return Convert.ToInt32(dr[10]);
			}
			set
			{
				dr[10]=value;
			}
		}
		public bool fFK_HorizAlignmentIsNull
		{
			get
			{
				return dr[10]==DBNull.Value;
			}
		}
		public void fFK_HorizAlignmentSetNull()
		{
			dr[10]=DBNull.Value;
		}
		public HorizAlignmentDataRow fFK_HorizAlignment_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tHorizAlignment[Convert.ToInt32(dr[10])];
			}
		}
		public System.Int32 fFK_VertAlignment
		{
			get
			{
				return Convert.ToInt32(dr[11]);
			}
			set
			{
				dr[11]=value;
			}
		}
		public bool fFK_VertAlignmentIsNull
		{
			get
			{
				return dr[11]==DBNull.Value;
			}
		}
		public void fFK_VertAlignmentSetNull()
		{
			dr[11]=DBNull.Value;
		}
		public VertAlignmentDataRow fFK_VertAlignment_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tVertAlignment[Convert.ToInt32(dr[11])];
			}
		}
		public System.Int32 fFK_Style
		{
			get
			{
				return Convert.ToInt32(dr[12]);
			}
			set
			{
				dr[12]=value;
			}
		}
		public bool fFK_StyleIsNull
		{
			get
			{
				return dr[12]==DBNull.Value;
			}
		}
		public void fFK_StyleSetNull()
		{
			dr[12]=DBNull.Value;
		}
		public StyleDataRow fFK_Style_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tStyle[Convert.ToInt32(dr[12])];
			}
		}
		public System.Int32 fPlainBackgroundColor
		{
			get
			{
				return Convert.ToInt32(dr[13]);
			}
			set
			{
				dr[13]=value;
			}
		}
		public bool fPlainBackgroundColorIsNull
		{
			get
			{
				return dr[13]==DBNull.Value;
			}
		}
		public void fPlainBackgroundColorSetNull()
		{
			dr[13]=DBNull.Value;
		}
	} // public class DesignObjVariation_Text_Skin_LanguageDataRow
	public class DesignObjVariation_Text_Skin_LanguageDataReader
	{
		public OdbcDataReader dr;
		bool bCache=false;
		int iRecord=-1,iNumRecords=-1;
		ArrayList al = null;

		public DesignObjVariation_Text_Skin_LanguageDataReader(OdbcDataReader d)
		{
			dr=d;
		}
		public DesignObjVariation_Text_Skin_LanguageDataReader(OdbcCommand cmd)
		{
			dr = cmd.ExecuteReader();
		}

		public DesignObjVariation_Text_Skin_LanguageDataReader(OdbcCommand cmd,bool Cache)
		{
			dr = cmd.ExecuteReader();
			bCache=Cache;
			if( bCache )
				CacheAndClose();
		}

		public DesignObjVariation_Text_Skin_LanguageDataReader(string sSQL)
		{
			OdbcConnection conn = HADataConfiguration.GetOdbcConnection();

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM DesignObjVariation_Text_Skin_Language WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public DesignObjVariation_Text_Skin_LanguageDataReader(string sSQL,OdbcConnection conn)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM DesignObjVariation_Text_Skin_Language WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public DesignObjVariation_Text_Skin_LanguageDataReader(string sSQL,OdbcConnection conn,OdbcTransaction trans,bool Cache)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM DesignObjVariation_Text_Skin_Language WHERE " + sSQL;
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
				object[] objs = new object[14];
				for(int i=0;i<14;i++)
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
		public System.Int32 fPK_DesignObjVariation_Text_Skin_Language
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[0]);
				else
					return Convert.ToInt32(dr[0]);
			}
		}
		public System.Int32 fFK_DesignObjVariation_Text
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[1]);
				else
					return Convert.ToInt32(dr[1]);
			}
		}
		public System.Int32 fFK_Skin
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[2]);
				else
					return Convert.ToInt32(dr[2]);
			}
		}
		public bool fFK_SkinIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[2]==DBNull.Value;
				else
					return dr[2]==DBNull.Value;
			}
		}
		public System.Int32 fFK_Language
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[3]);
				else
					return Convert.ToInt32(dr[3]);
			}
		}
		public bool fFK_LanguageIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[3]==DBNull.Value;
				else
					return dr[3]==DBNull.Value;
			}
		}
		public System.Int32 fX
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[4]);
				else
					return Convert.ToInt32(dr[4]);
			}
		}
		public System.Int32 fY
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[5]);
				else
					return Convert.ToInt32(dr[5]);
			}
		}
		public System.Int32 fWidth
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[6]);
				else
					return Convert.ToInt32(dr[6]);
			}
		}
		public System.Int32 fHeight
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[7]);
				else
					return Convert.ToInt32(dr[7]);
			}
		}
		public System.Int32 fRotate
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[8]);
				else
					return Convert.ToInt32(dr[8]);
			}
		}
		public bool fRotateIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[8]==DBNull.Value;
				else
					return dr[8]==DBNull.Value;
			}
		}
		public System.Int32 fOpacity
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[9]);
				else
					return Convert.ToInt32(dr[9]);
			}
		}
		public bool fOpacityIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[9]==DBNull.Value;
				else
					return dr[9]==DBNull.Value;
			}
		}
		public System.Int32 fFK_HorizAlignment
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[10]);
				else
					return Convert.ToInt32(dr[10]);
			}
		}
		public bool fFK_HorizAlignmentIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[10]==DBNull.Value;
				else
					return dr[10]==DBNull.Value;
			}
		}
		public System.Int32 fFK_VertAlignment
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[11]);
				else
					return Convert.ToInt32(dr[11]);
			}
		}
		public bool fFK_VertAlignmentIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[11]==DBNull.Value;
				else
					return dr[11]==DBNull.Value;
			}
		}
		public System.Int32 fFK_Style
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[12]);
				else
					return Convert.ToInt32(dr[12]);
			}
		}
		public bool fFK_StyleIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[12]==DBNull.Value;
				else
					return dr[12]==DBNull.Value;
			}
		}
		public System.Int32 fPlainBackgroundColor
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[13]);
				else
					return Convert.ToInt32(dr[13]);
			}
		}
		public bool fPlainBackgroundColorIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[13]==DBNull.Value;
				else
					return dr[13]==DBNull.Value;
			}
		}
	} // public class DesignObjVariation_Text_Skin_LanguageDataReader
	public class DesignObjVariation_Text_Skin_LanguageTable : DataTable
	{
		public DesignObjVariation_Text_Skin_LanguageTable() : base("DesignObjVariation_Text_Skin_Language") {}

		public DesignObjVariation_Text_Skin_LanguageDataRow this [System.Int32 PK_DesignObjVariation_Text_Skin_Language]
		{
			get
			{
				DesignObjVariation_Text_Skin_LanguageDataRow dr = new DesignObjVariation_Text_Skin_LanguageDataRow(Rows.Find(PK_DesignObjVariation_Text_Skin_Language));
				if( !dr.bIsValid  && false /* can't do this with ODBC */  )
				{
					MyDataSet mds = (MyDataSet) DataSet;
					if( mds.m_conn==null )
						return dr;
					OdbcDataAdapter sqlda = new OdbcDataAdapter();
					OdbcCommand LoadCommand = new OdbcCommand("sp_Select_DesignObjVariation_Text_Skin_Language", mds.m_conn,mds.m_trans);
					LoadCommand.CommandType = CommandType.StoredProcedure;
					LoadCommand.Parameters.Add(new OdbcParameter("@PK_DesignObjVariation_Text_Skin_Language", OdbcType.Int,4));
					LoadCommand.Parameters["@PK_DesignObjVariation_Text_Skin_Language"].Value = PK_DesignObjVariation_Text_Skin_Language;
					sqlda.SelectCommand = LoadCommand;
					sqlda.Fill(mds,"DesignObjVariation_Text_Skin_Language");
					dr = new DesignObjVariation_Text_Skin_LanguageDataRow(Rows.Find(PK_DesignObjVariation_Text_Skin_Language));
				}
				return dr;
			}
		}
		public DataRowCollection LoadAll(OdbcConnection conn, OdbcTransaction trans)
		{
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT PK_DesignObjVariation_Text_Skin_Language,FK_DesignObjVariation_Text,FK_Skin,FK_Language,X,Y,Width,Height,Rotate,Opacity,FK_HorizAlignment,FK_VertAlignment,FK_Style,PlainBackgroundColor FROM DesignObjVariation_Text_Skin_Language", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(this.DataSet,"DesignObjVariation_Text_Skin_Language")==0 )
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
			DesignObjVariation_Text_Skin_LanguageData.UpdateDesignObjVariation_Text_Skin_Language(ref ds,PK_Users,conn,trans);
		}
		public DataColumn cPK_DesignObjVariation_Text_Skin_Language
		{
			get
			{
				return Columns[0];
			}
		}
		public DataColumn cFK_DesignObjVariation_Text
		{
			get
			{
				return Columns[1];
			}
		}
		public DataColumn cFK_Skin
		{
			get
			{
				return Columns[2];
			}
		}
		public DataColumn cFK_Language
		{
			get
			{
				return Columns[3];
			}
		}
		public DataColumn cX
		{
			get
			{
				return Columns[4];
			}
		}
		public DataColumn cY
		{
			get
			{
				return Columns[5];
			}
		}
		public DataColumn cWidth
		{
			get
			{
				return Columns[6];
			}
		}
		public DataColumn cHeight
		{
			get
			{
				return Columns[7];
			}
		}
		public DataColumn cRotate
		{
			get
			{
				return Columns[8];
			}
		}
		public DataColumn cOpacity
		{
			get
			{
				return Columns[9];
			}
		}
		public DataColumn cFK_HorizAlignment
		{
			get
			{
				return Columns[10];
			}
		}
		public DataColumn cFK_VertAlignment
		{
			get
			{
				return Columns[11];
			}
		}
		public DataColumn cFK_Style
		{
			get
			{
				return Columns[12];
			}
		}
		public DataColumn cPlainBackgroundColor
		{
			get
			{
				return Columns[13];
			}
		}
	}
} // namespace HAData.Common.Data
