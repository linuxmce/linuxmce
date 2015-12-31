namespace HAData.DataAccess {
	using System;
	using System.Data;
	using Microsoft.Data.Odbc;
	using System.Collections;

	using HAData.Common;

	public class StyleVariationData : MyDataSet {
		//
		// StyleVariation table constants
		//
		public const String STYLEVARIATION_TABLE = "StyleVariation";
		public const String PK_STYLEVARIATION_FIELD = "PK_StyleVariation";
		public const String FK_STYLE_FIELD = "FK_Style";
		public const String FK_SKIN_FIELD = "FK_Skin";
		public const String FK_UI_FIELD = "FK_UI";
		public const String FONT_FIELD = "Font";
		public const String FORECOLOR_FIELD = "ForeColor";
		public const String PIXELHEIGHT_FIELD = "PixelHeight";
		public const String BOLD_FIELD = "Bold";
		public const String ITALIC_FIELD = "Italic";
		public const String UNDERLINE_FIELD = "Underline";
		public const String SHADOWX_FIELD = "ShadowX";
		public const String SHADOWY_FIELD = "ShadowY";
		public const String SHADOWCOLOR_FIELD = "ShadowColor";
		public const String FK_HORIZALIGNMENT_FIELD = "FK_HorizAlignment";
		public const String FK_VERTALIGNMENT_FIELD = "FK_VertAlignment";
		public const String BACKCOLOR_FIELD = "BackColor";
		// table+field constants
		public const String PK_STYLEVARIATION_TABLE_FIELD = "StyleVariation.PK_StyleVariation";
		public const String FK_STYLE_TABLE_FIELD = "StyleVariation.FK_Style";
		public const String FK_SKIN_TABLE_FIELD = "StyleVariation.FK_Skin";
		public const String FK_UI_TABLE_FIELD = "StyleVariation.FK_UI";
		public const String FONT_TABLE_FIELD = "StyleVariation.Font";
		public const String FORECOLOR_TABLE_FIELD = "StyleVariation.ForeColor";
		public const String PIXELHEIGHT_TABLE_FIELD = "StyleVariation.PixelHeight";
		public const String BOLD_TABLE_FIELD = "StyleVariation.Bold";
		public const String ITALIC_TABLE_FIELD = "StyleVariation.Italic";
		public const String UNDERLINE_TABLE_FIELD = "StyleVariation.Underline";
		public const String SHADOWX_TABLE_FIELD = "StyleVariation.ShadowX";
		public const String SHADOWY_TABLE_FIELD = "StyleVariation.ShadowY";
		public const String SHADOWCOLOR_TABLE_FIELD = "StyleVariation.ShadowColor";
		public const String FK_HORIZALIGNMENT_TABLE_FIELD = "StyleVariation.FK_HorizAlignment";
		public const String FK_VERTALIGNMENT_TABLE_FIELD = "StyleVariation.FK_VertAlignment";
		public const String BACKCOLOR_TABLE_FIELD = "StyleVariation.BackColor";
		// DataSetCommand object
		protected OdbcDataAdapter m_DSCommand;

		// Stored procedure parameters
		protected const String PK_STYLEVARIATION_PARM = "@PK_StyleVariation";
		protected const String FK_STYLE_PARM = "@FK_Style";
		protected const String FK_SKIN_PARM = "@FK_Skin";
		protected const String FK_UI_PARM = "@FK_UI";
		protected const String FONT_PARM = "@Font";
		protected const String FORECOLOR_PARM = "@ForeColor";
		protected const String PIXELHEIGHT_PARM = "@PixelHeight";
		protected const String BOLD_PARM = "@Bold";
		protected const String ITALIC_PARM = "@Italic";
		protected const String UNDERLINE_PARM = "@Underline";
		protected const String SHADOWX_PARM = "@ShadowX";
		protected const String SHADOWY_PARM = "@ShadowY";
		protected const String SHADOWCOLOR_PARM = "@ShadowColor";
		protected const String FK_HORIZALIGNMENT_PARM = "@FK_HorizAlignment";
		protected const String FK_VERTALIGNMENT_PARM = "@FK_VertAlignment";
		protected const String BACKCOLOR_PARM = "@BackColor";
		protected const String USERID_PARM = "@UserID";

		protected OdbcCommand m_LoadCommand;
		protected OdbcCommand m_InsertCommand;
		protected OdbcCommand m_UpdateCommand;
		protected OdbcCommand m_DeleteCommand;
		protected OdbcConnection m_Connection;
		protected OdbcTransaction m_Transaction;
		public DataTable Table { get { return Tables[0]; } }


		public StyleVariationData() {  // marker:1
			//
			// Create the tables in the dataset
			//
			Tables.Add(BuildDataTables());
			m_Connection = HADataConfiguration.GetOdbcConnection();
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

			m_DSCommand.TableMappings.Add("Table", StyleVariationData.STYLEVARIATION_TABLE);
		}

		public StyleVariationData(OdbcConnection conn,OdbcTransaction trans) {

			m_Connection = conn;
			m_Transaction = trans;
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

			m_DSCommand.TableMappings.Add("Table", StyleVariationData.STYLEVARIATION_TABLE);
		}

		private StyleVariationData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context) {
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
			return (DataTable) BuildStyleVariationTable();
		}
		public static StyleVariationTable BuildStyleVariationTable() {
			//
			// Create the StyleVariation table
			//
			StyleVariationTable Table = new StyleVariationTable();
			DataColumnCollection Columns = Table.Columns;
			DataColumn[] PKColumns = new DataColumn[1];

			DataColumn Column = Columns.Add(PK_STYLEVARIATION_FIELD, typeof(System.Int32));
			Column.AutoIncrement = true;
			Column.AutoIncrementSeed = -1;
			Column.AutoIncrementStep = -1;
			PKColumns[0] = Column;

			Column = Columns.Add(FK_STYLE_FIELD, typeof(System.Int32));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Columns.Add(FK_SKIN_FIELD, typeof(System.Int32));
			Columns.Add(FK_UI_FIELD, typeof(System.Int32));
			Column = Columns.Add(FONT_FIELD, typeof(System.String));
			Column.AllowDBNull = false;

			Column = Columns.Add(FORECOLOR_FIELD, typeof(System.Int32));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Column = Columns.Add(PIXELHEIGHT_FIELD, typeof(System.Int32));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Column = Columns.Add(BOLD_FIELD, typeof(System.Boolean));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Column = Columns.Add(ITALIC_FIELD, typeof(System.Boolean));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Column = Columns.Add(UNDERLINE_FIELD, typeof(System.Boolean));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Columns.Add(SHADOWX_FIELD, typeof(System.Int32));
			Columns.Add(SHADOWY_FIELD, typeof(System.Int32));
			Columns.Add(SHADOWCOLOR_FIELD, typeof(System.Int32));
			Column = Columns.Add(FK_HORIZALIGNMENT_FIELD, typeof(System.Int32));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Column = Columns.Add(FK_VERTALIGNMENT_FIELD, typeof(System.Int32));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Columns.Add(BACKCOLOR_FIELD, typeof(System.Int32));
			Table.PrimaryKey = PKColumns;

			return Table;
		}
		protected static void CreateParameters(OdbcParameterCollection Params, bool IsInsert) {
			Params.Add(new OdbcParameter(PK_STYLEVARIATION_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_STYLE_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_SKIN_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_UI_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FONT_PARM, OdbcType.VarChar, 30));
			Params.Add(new OdbcParameter(FORECOLOR_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(PIXELHEIGHT_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(BOLD_PARM, OdbcType.Bit,1));
			Params.Add(new OdbcParameter(ITALIC_PARM, OdbcType.Bit,1));
			Params.Add(new OdbcParameter(UNDERLINE_PARM, OdbcType.Bit,1));
			Params.Add(new OdbcParameter(SHADOWX_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(SHADOWY_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(SHADOWCOLOR_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_HORIZALIGNMENT_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_VERTALIGNMENT_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(BACKCOLOR_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(USERID_PARM, OdbcType.Int));

			// map the parameters to the data table

			Params[PK_STYLEVARIATION_PARM].SourceColumn = StyleVariationData.PK_STYLEVARIATION_FIELD;
			if(IsInsert) {
				Params[PK_STYLEVARIATION_PARM].Direction = ParameterDirection.Output;
			}

			Params[FK_STYLE_PARM].SourceColumn = StyleVariationData.FK_STYLE_FIELD;
			Params[FK_SKIN_PARM].SourceColumn = StyleVariationData.FK_SKIN_FIELD;
			Params[FK_UI_PARM].SourceColumn = StyleVariationData.FK_UI_FIELD;
			Params[FONT_PARM].SourceColumn = StyleVariationData.FONT_FIELD;
			Params[FORECOLOR_PARM].SourceColumn = StyleVariationData.FORECOLOR_FIELD;
			Params[PIXELHEIGHT_PARM].SourceColumn = StyleVariationData.PIXELHEIGHT_FIELD;
			Params[BOLD_PARM].SourceColumn = StyleVariationData.BOLD_FIELD;
			Params[ITALIC_PARM].SourceColumn = StyleVariationData.ITALIC_FIELD;
			Params[UNDERLINE_PARM].SourceColumn = StyleVariationData.UNDERLINE_FIELD;
			Params[SHADOWX_PARM].SourceColumn = StyleVariationData.SHADOWX_FIELD;
			Params[SHADOWY_PARM].SourceColumn = StyleVariationData.SHADOWY_FIELD;
			Params[SHADOWCOLOR_PARM].SourceColumn = StyleVariationData.SHADOWCOLOR_FIELD;
			Params[FK_HORIZALIGNMENT_PARM].SourceColumn = StyleVariationData.FK_HORIZALIGNMENT_FIELD;
			Params[FK_VERTALIGNMENT_PARM].SourceColumn = StyleVariationData.FK_VERTALIGNMENT_FIELD;
			Params[BACKCOLOR_PARM].SourceColumn = StyleVariationData.BACKCOLOR_FIELD;
		}

		protected static void CreateCommands(OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
			if(LoadCommand == null) {
				// Create the command since it's null
				LoadCommand = new OdbcCommand("sp_Select_StyleVariation", Conn);
				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(PK_STYLEVARIATION_PARM, OdbcType.Int,4));
			}

			if(InsertCommand == null) {
				// Create the command since it's null
				InsertCommand = new OdbcCommand("sp_Insert_StyleVariation", Conn);
				InsertCommand.CommandType = CommandType.StoredProcedure;
				InsertCommand.Transaction = Trans;

				OdbcParameterCollection Params = InsertCommand.Parameters;

				CreateParameters(Params, true);

			}

			if(UpdateCommand == null) {
				// Create the command since it's null
				UpdateCommand = new OdbcCommand("sp_Update_StyleVariation", Conn);
				UpdateCommand.CommandType = CommandType.StoredProcedure;
				UpdateCommand.Transaction = Trans;

				OdbcParameterCollection Params = UpdateCommand.Parameters;

				CreateParameters(Params, false);

			}
			if (DeleteCommand == null)
			{
				DeleteCommand = new OdbcCommand("sp_Delete_StyleVariation", Conn);
				DeleteCommand.CommandType = CommandType.StoredProcedure;
				DeleteCommand.Transaction = Trans;

				DeleteCommand.Parameters.Add(PK_STYLEVARIATION_PARM, OdbcType.Int,4, PK_STYLEVARIATION_FIELD);
				DeleteCommand.Parameters.Add(USERID_PARM, OdbcType.Int);
			}
		}

		protected static void CreateCommands(OdbcDataAdapter odbcda,OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
				LoadCommand = new OdbcCommand("SELECT PK_StyleVariation,FK_Style,FK_Skin,FK_UI,Font,ForeColor,PixelHeight,Bold,Italic,Underline,ShadowX,ShadowY,ShadowColor,FK_HorizAlignment,FK_VertAlignment,BackColor FROM StyleVariation", Conn);
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(PK_STYLEVARIATION_PARM, OdbcType.Int,4));

			odbcda.SelectCommand = LoadCommand;
			OdbcCommandBuilder odbcCB = new OdbcCommandBuilder(odbcda);
			odbcCB.RefreshSchema();
			DeleteCommand = odbcCB.GetDeleteCommand();
			InsertCommand = odbcCB.GetInsertCommand();
			UpdateCommand = odbcCB.GetUpdateCommand();
		}

		public StyleVariationData LoadStyleVariation(System.Int32 PK_StyleVariation)
		{
			m_DSCommand.SelectCommand = m_LoadCommand;
			m_DSCommand.SelectCommand.Parameters[PK_STYLEVARIATION_PARM].Value = PK_StyleVariation;

			m_DSCommand.Fill(this);
			return this;
		}

		public static DataRowCollection LoadStyleVariationWithWhere(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, string WhereClause) // marker:2
		{
			DataRowCollection dr;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["StyleVariation"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}
			
			DataSet dsTemp = new MyDataSet();
			dsTemp.Tables.Add(BuildDataTables());
			
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			string sSQL = "SELECT PK_StyleVariation, FK_Style, FK_Skin, FK_UI, Font, ForeColor, PixelHeight, Bold, Italic, Underline, ShadowX, ShadowY, ShadowColor, FK_HorizAlignment, FK_VertAlignment, BackColor FROM StyleVariation WHERE " + WhereClause;
			
			OdbcCommand LoadCommand = new OdbcCommand(sSQL,conn);
			
			if( trans!=null )
				LoadCommand.Transaction = trans;
			
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(dsTemp,"StyleVariation");
			
			dr=dsTemp.Tables["StyleVariation"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}

		public static DataRow LoadNoCacheStyleVariation(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 PK_StyleVariation)
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["StyleVariation"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}

			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand;
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();

			LoadCommand = new OdbcCommand("sp_Select_StyleVariation", conn);

			LoadCommand.CommandType = CommandType.StoredProcedure;
			LoadCommand.Parameters.Add(new OdbcParameter(PK_STYLEVARIATION_PARM, OdbcType.Int,4));
			LoadCommand.Parameters[PK_STYLEVARIATION_PARM].Value = PK_StyleVariation;
			if( trans!=null )
				LoadCommand.Transaction = trans;
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(ds,"StyleVariation");
				dr = ds.Tables["StyleVariation"].Rows.Find(PK_StyleVariation);
			return dr;
		}

		public static DataRow LoadStyleVariation(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 PK_StyleVariation)  // marker:3
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["StyleVariation"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
				else
				dr = dt.Rows.Find(PK_StyleVariation);
			}

			if( dr==null )
			{
				OdbcDataAdapter sqlda = new OdbcDataAdapter();
				OdbcCommand LoadCommand;
				if( conn==null )
					conn = HADataConfiguration.GetOdbcConnection();

				LoadCommand = new OdbcCommand("sp_Select_StyleVariation", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new OdbcParameter(PK_STYLEVARIATION_PARM, OdbcType.Int,4));
				LoadCommand.Parameters[PK_STYLEVARIATION_PARM].Value = PK_StyleVariation;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(ds,"StyleVariation");
				dr = ds.Tables["StyleVariation"].Rows.Find(PK_StyleVariation);
			}
			return dr;
		}

		public StyleVariationData LoadAll() {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new OdbcCommand("SELECT * FROM StyleVariation", m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this);
			return this;

		}

		public static DataRowCollection LoadAll(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {

			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT * FROM StyleVariation", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(ds,"StyleVariation")==0 )
				return null;
			else
				return ds.Tables["StyleVariation"].Rows;

		}

		public StyleVariationData ExecuteQuery(String sSQL) {
			return ExecuteQuery(sSQL,STYLEVARIATION_TABLE);
		}
		public StyleVariationData ExecuteQuery(String sSQL,String sTableName) {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new OdbcCommand(sSQL, m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this,sTableName);

			return this;
		}

		public static DataRowCollection ExecuteQuery(String sSQL,ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {
			return ExecuteQuery(sSQL,ref ds,conn,trans,"StyleVariation");
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

		public bool UpdateStyleVariation(int CurUserID) {
			m_DSCommand.UpdateCommand = m_UpdateCommand;
			m_DSCommand.UpdateCommand.Parameters[USERID_PARM].Value = CurUserID;

			m_DSCommand.InsertCommand = m_InsertCommand;
			m_DSCommand.InsertCommand.Parameters[USERID_PARM].Value = CurUserID;

			if( m_DeleteCommand != null )
			{
				m_DSCommand.DeleteCommand = m_DeleteCommand;
				m_DSCommand.DeleteCommand.Parameters[USERID_PARM].Value = CurUserID;
			}

			m_DSCommand.Update(this, StyleVariationData.STYLEVARIATION_TABLE);
			return true;
		}

		public static bool UpdateStyleVariation(ref MyDataSet ds, int CurUserID)
		{
			OdbcConnection OdbcConn = HADataConfiguration.GetOdbcConnection();
			return UpdateStyleVariation(ref ds,CurUserID,OdbcConn,null);
		}

		public static bool UpdateStyleVariation(ref MyDataSet ds, int CurUserID,OdbcConnection OdbcConn,OdbcTransaction Trans)
		{
			DataTable dt = ds.Tables[STYLEVARIATION_TABLE];
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

	} // public class StyleVariationData
	public class StyleVariationDataRow
	{
		public DataRow dr = null;
		public StyleVariationDataRow(DataRow d)
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
		public System.Int32 fPK_StyleVariation
		{
			get
			{
				return Convert.ToInt32(dr[0]);
			}
		}
		public System.Int32 fFK_Style
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
		public StyleDataRow fFK_Style_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tStyle[Convert.ToInt32(dr[1])];
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
		public System.Int32 fFK_UI
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
		public bool fFK_UIIsNull
		{
			get
			{
				return dr[3]==DBNull.Value;
			}
		}
		public void fFK_UISetNull()
		{
			dr[3]=DBNull.Value;
		}
		public UIDataRow fFK_UI_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tUI[Convert.ToInt32(dr[3])];
			}
		}
		public System.String fFont
		{
			get
			{
				return Convert.ToString(dr[4]);
			}
			set
			{
				dr[4]=value;
			}
		}
		public System.Int32 fForeColor
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
		public System.Int32 fPixelHeight
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
		public System.Boolean fBold
		{
			get
			{
				return Convert.ToBoolean(dr[7]);
			}
			set
			{
				dr[7]=value;
			}
		}
		public System.Boolean fItalic
		{
			get
			{
				return Convert.ToBoolean(dr[8]);
			}
			set
			{
				dr[8]=value;
			}
		}
		public System.Boolean fUnderline
		{
			get
			{
				return Convert.ToBoolean(dr[9]);
			}
			set
			{
				dr[9]=value;
			}
		}
		public System.Int32 fShadowX
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
		public bool fShadowXIsNull
		{
			get
			{
				return dr[10]==DBNull.Value;
			}
		}
		public void fShadowXSetNull()
		{
			dr[10]=DBNull.Value;
		}
		public System.Int32 fShadowY
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
		public bool fShadowYIsNull
		{
			get
			{
				return dr[11]==DBNull.Value;
			}
		}
		public void fShadowYSetNull()
		{
			dr[11]=DBNull.Value;
		}
		public System.Int32 fShadowColor
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
		public bool fShadowColorIsNull
		{
			get
			{
				return dr[12]==DBNull.Value;
			}
		}
		public void fShadowColorSetNull()
		{
			dr[12]=DBNull.Value;
		}
		public System.Int32 fFK_HorizAlignment
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
		public HorizAlignmentDataRow fFK_HorizAlignment_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tHorizAlignment[Convert.ToInt32(dr[13])];
			}
		}
		public System.Int32 fFK_VertAlignment
		{
			get
			{
				return Convert.ToInt32(dr[14]);
			}
			set
			{
				dr[14]=value;
			}
		}
		public VertAlignmentDataRow fFK_VertAlignment_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tVertAlignment[Convert.ToInt32(dr[14])];
			}
		}
		public System.Int32 fBackColor
		{
			get
			{
				return Convert.ToInt32(dr[15]);
			}
			set
			{
				dr[15]=value;
			}
		}
		public bool fBackColorIsNull
		{
			get
			{
				return dr[15]==DBNull.Value;
			}
		}
		public void fBackColorSetNull()
		{
			dr[15]=DBNull.Value;
		}
	} // public class StyleVariationDataRow
	public class StyleVariationDataReader
	{
		public OdbcDataReader dr;
		bool bCache=false;
		int iRecord=-1,iNumRecords=-1;
		ArrayList al = null;

		public StyleVariationDataReader(OdbcDataReader d)
		{
			dr=d;
		}
		public StyleVariationDataReader(OdbcCommand cmd)
		{
			dr = cmd.ExecuteReader();
		}

		public StyleVariationDataReader(OdbcCommand cmd,bool Cache)
		{
			dr = cmd.ExecuteReader();
			bCache=Cache;
			if( bCache )
				CacheAndClose();
		}

		public StyleVariationDataReader(string sSQL)
		{
			OdbcConnection conn = HADataConfiguration.GetOdbcConnection();

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM StyleVariation WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public StyleVariationDataReader(string sSQL,OdbcConnection conn)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM StyleVariation WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public StyleVariationDataReader(string sSQL,OdbcConnection conn,OdbcTransaction trans,bool Cache)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM StyleVariation WHERE " + sSQL;
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
				object[] objs = new object[16];
				for(int i=0;i<16;i++)
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
		public System.Int32 fPK_StyleVariation
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[0]);
				else
					return Convert.ToInt32(dr[0]);
			}
		}
		public System.Int32 fFK_Style
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
		public System.Int32 fFK_UI
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[3]);
				else
					return Convert.ToInt32(dr[3]);
			}
		}
		public bool fFK_UIIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[3]==DBNull.Value;
				else
					return dr[3]==DBNull.Value;
			}
		}
		public System.String fFont
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[4]);
				else
					return Convert.ToString(dr[4]);
			}
		}
		public System.Int32 fForeColor
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[5]);
				else
					return Convert.ToInt32(dr[5]);
			}
		}
		public System.Int32 fPixelHeight
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[6]);
				else
					return Convert.ToInt32(dr[6]);
			}
		}
		public System.Boolean fBold
		{
			get
			{
				if( bCache )
					return Convert.ToBoolean(((object[]) al[iRecord])[7]);
				else
					return Convert.ToBoolean(dr[7]);
			}
		}
		public System.Boolean fItalic
		{
			get
			{
				if( bCache )
					return Convert.ToBoolean(((object[]) al[iRecord])[8]);
				else
					return Convert.ToBoolean(dr[8]);
			}
		}
		public System.Boolean fUnderline
		{
			get
			{
				if( bCache )
					return Convert.ToBoolean(((object[]) al[iRecord])[9]);
				else
					return Convert.ToBoolean(dr[9]);
			}
		}
		public System.Int32 fShadowX
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[10]);
				else
					return Convert.ToInt32(dr[10]);
			}
		}
		public bool fShadowXIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[10]==DBNull.Value;
				else
					return dr[10]==DBNull.Value;
			}
		}
		public System.Int32 fShadowY
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[11]);
				else
					return Convert.ToInt32(dr[11]);
			}
		}
		public bool fShadowYIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[11]==DBNull.Value;
				else
					return dr[11]==DBNull.Value;
			}
		}
		public System.Int32 fShadowColor
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[12]);
				else
					return Convert.ToInt32(dr[12]);
			}
		}
		public bool fShadowColorIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[12]==DBNull.Value;
				else
					return dr[12]==DBNull.Value;
			}
		}
		public System.Int32 fFK_HorizAlignment
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[13]);
				else
					return Convert.ToInt32(dr[13]);
			}
		}
		public System.Int32 fFK_VertAlignment
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[14]);
				else
					return Convert.ToInt32(dr[14]);
			}
		}
		public System.Int32 fBackColor
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[15]);
				else
					return Convert.ToInt32(dr[15]);
			}
		}
		public bool fBackColorIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[15]==DBNull.Value;
				else
					return dr[15]==DBNull.Value;
			}
		}
	} // public class StyleVariationDataReader
	public class StyleVariationTable : DataTable
	{
		public StyleVariationTable() : base("StyleVariation") {}

		public StyleVariationDataRow this [System.Int32 PK_StyleVariation]
		{
			get
			{
				StyleVariationDataRow dr = new StyleVariationDataRow(Rows.Find(PK_StyleVariation));
				if( !dr.bIsValid  && false /* can't do this with ODBC */  )
				{
					MyDataSet mds = (MyDataSet) DataSet;
					if( mds.m_conn==null )
						return dr;
					OdbcDataAdapter sqlda = new OdbcDataAdapter();
					OdbcCommand LoadCommand = new OdbcCommand("sp_Select_StyleVariation", mds.m_conn,mds.m_trans);
					LoadCommand.CommandType = CommandType.StoredProcedure;
					LoadCommand.Parameters.Add(new OdbcParameter("@PK_StyleVariation", OdbcType.Int,4));
					LoadCommand.Parameters["@PK_StyleVariation"].Value = PK_StyleVariation;
					sqlda.SelectCommand = LoadCommand;
					sqlda.Fill(mds,"StyleVariation");
					dr = new StyleVariationDataRow(Rows.Find(PK_StyleVariation));
				}
				return dr;
			}
		}
		public DataRowCollection LoadAll(OdbcConnection conn, OdbcTransaction trans)
		{
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT PK_StyleVariation,FK_Style,FK_Skin,FK_UI,Font,ForeColor,PixelHeight,Bold,Italic,Underline,ShadowX,ShadowY,ShadowColor,FK_HorizAlignment,FK_VertAlignment,BackColor FROM StyleVariation", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(this.DataSet,"StyleVariation")==0 )
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
			StyleVariationData.UpdateStyleVariation(ref ds,PK_Users,conn,trans);
		}
		public DataColumn cPK_StyleVariation
		{
			get
			{
				return Columns[0];
			}
		}
		public DataColumn cFK_Style
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
		public DataColumn cFK_UI
		{
			get
			{
				return Columns[3];
			}
		}
		public DataColumn cFont
		{
			get
			{
				return Columns[4];
			}
		}
		public DataColumn cForeColor
		{
			get
			{
				return Columns[5];
			}
		}
		public DataColumn cPixelHeight
		{
			get
			{
				return Columns[6];
			}
		}
		public DataColumn cBold
		{
			get
			{
				return Columns[7];
			}
		}
		public DataColumn cItalic
		{
			get
			{
				return Columns[8];
			}
		}
		public DataColumn cUnderline
		{
			get
			{
				return Columns[9];
			}
		}
		public DataColumn cShadowX
		{
			get
			{
				return Columns[10];
			}
		}
		public DataColumn cShadowY
		{
			get
			{
				return Columns[11];
			}
		}
		public DataColumn cShadowColor
		{
			get
			{
				return Columns[12];
			}
		}
		public DataColumn cFK_HorizAlignment
		{
			get
			{
				return Columns[13];
			}
		}
		public DataColumn cFK_VertAlignment
		{
			get
			{
				return Columns[14];
			}
		}
		public DataColumn cBackColor
		{
			get
			{
				return Columns[15];
			}
		}
	}
} // namespace HAData.Common.Data
