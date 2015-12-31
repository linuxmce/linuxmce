namespace HAData.DataAccess {
	using System;
	using System.Data;
	using Microsoft.Data.Odbc;
	using System.Collections;

	using HAData.Common;

	public class DeviceTemplateData : MyDataSet {
		//
		// DeviceTemplate table constants
		//
		public const String DEVICETEMPLATE_TABLE = "DeviceTemplate";
		public const String PK_DEVICETEMPLATE_FIELD = "PK_DeviceTemplate";
		public const String DESCRIPTION_FIELD = "Description";
		public const String COMMENTS_FIELD = "Comments";
		public const String FK_DEVICECATEGORY_FIELD = "FK_DeviceCategory";
		public const String FK_MANUFACTURER_FIELD = "FK_Manufacturer";
		public const String DEFINE_FIELD = "Define";
		public const String IMPLEMENTSDCE_FIELD = "ImplementsDCE";
		public const String ISEMBEDDED_FIELD = "IsEmbedded";
		public const String COMMANDLINE_FIELD = "CommandLine";
		public const String ISPLUGANDPLAY_FIELD = "IsPlugAndPlay";
		public const String ISIPBASED_FIELD = "IsIPBased";
		public const String ISPLUGIN_FIELD = "IsPlugIn";
		public const String IRFREQUENCY_FIELD = "IRFrequency";
		public const String FK_STABILITYSTATUS_FIELD = "FK_StabilityStatus";
		// table+field constants
		public const String PK_DEVICETEMPLATE_TABLE_FIELD = "DeviceTemplate.PK_DeviceTemplate";
		public const String DESCRIPTION_TABLE_FIELD = "DeviceTemplate.Description";
		public const String COMMENTS_TABLE_FIELD = "DeviceTemplate.Comments";
		public const String FK_DEVICECATEGORY_TABLE_FIELD = "DeviceTemplate.FK_DeviceCategory";
		public const String FK_MANUFACTURER_TABLE_FIELD = "DeviceTemplate.FK_Manufacturer";
		public const String DEFINE_TABLE_FIELD = "DeviceTemplate.Define";
		public const String IMPLEMENTSDCE_TABLE_FIELD = "DeviceTemplate.ImplementsDCE";
		public const String ISEMBEDDED_TABLE_FIELD = "DeviceTemplate.IsEmbedded";
		public const String COMMANDLINE_TABLE_FIELD = "DeviceTemplate.CommandLine";
		public const String ISPLUGANDPLAY_TABLE_FIELD = "DeviceTemplate.IsPlugAndPlay";
		public const String ISIPBASED_TABLE_FIELD = "DeviceTemplate.IsIPBased";
		public const String ISPLUGIN_TABLE_FIELD = "DeviceTemplate.IsPlugIn";
		public const String IRFREQUENCY_TABLE_FIELD = "DeviceTemplate.IRFrequency";
		public const String FK_STABILITYSTATUS_TABLE_FIELD = "DeviceTemplate.FK_StabilityStatus";
		// DataSetCommand object
		protected OdbcDataAdapter m_DSCommand;

		// Stored procedure parameters
		protected const String PK_DEVICETEMPLATE_PARM = "@PK_DeviceTemplate";
		protected const String DESCRIPTION_PARM = "@Description";
		protected const String COMMENTS_PARM = "@Comments";
		protected const String FK_DEVICECATEGORY_PARM = "@FK_DeviceCategory";
		protected const String FK_MANUFACTURER_PARM = "@FK_Manufacturer";
		protected const String DEFINE_PARM = "@Define";
		protected const String IMPLEMENTSDCE_PARM = "@ImplementsDCE";
		protected const String ISEMBEDDED_PARM = "@IsEmbedded";
		protected const String COMMANDLINE_PARM = "@CommandLine";
		protected const String ISPLUGANDPLAY_PARM = "@IsPlugAndPlay";
		protected const String ISIPBASED_PARM = "@IsIPBased";
		protected const String ISPLUGIN_PARM = "@IsPlugIn";
		protected const String IRFREQUENCY_PARM = "@IRFrequency";
		protected const String FK_STABILITYSTATUS_PARM = "@FK_StabilityStatus";
		protected const String USERID_PARM = "@UserID";

		protected OdbcCommand m_LoadCommand;
		protected OdbcCommand m_InsertCommand;
		protected OdbcCommand m_UpdateCommand;
		protected OdbcCommand m_DeleteCommand;
		protected OdbcConnection m_Connection;
		protected OdbcTransaction m_Transaction;
		public DataTable Table { get { return Tables[0]; } }


		public DeviceTemplateData() {  // marker:1
			//
			// Create the tables in the dataset
			//
			Tables.Add(BuildDataTables());
			m_Connection = HADataConfiguration.GetOdbcConnection();
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

			m_DSCommand.TableMappings.Add("Table", DeviceTemplateData.DEVICETEMPLATE_TABLE);
		}

		public DeviceTemplateData(OdbcConnection conn,OdbcTransaction trans) {

			m_Connection = conn;
			m_Transaction = trans;
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

			m_DSCommand.TableMappings.Add("Table", DeviceTemplateData.DEVICETEMPLATE_TABLE);
		}

		private DeviceTemplateData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context) {
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
			return (DataTable) BuildDeviceTemplateTable();
		}
		public static DeviceTemplateTable BuildDeviceTemplateTable() {
			//
			// Create the DeviceTemplate table
			//
			DeviceTemplateTable Table = new DeviceTemplateTable();
			DataColumnCollection Columns = Table.Columns;
			DataColumn[] PKColumns = new DataColumn[1];

			DataColumn Column = Columns.Add(PK_DEVICETEMPLATE_FIELD, typeof(System.Int32));
			Column.AutoIncrement = true;
			Column.AutoIncrementSeed = -1;
			Column.AutoIncrementStep = -1;
			PKColumns[0] = Column;

			Columns.Add(DESCRIPTION_FIELD, typeof(System.String));
			Columns.Add(COMMENTS_FIELD, typeof(System.String));
			Columns.Add(FK_DEVICECATEGORY_FIELD, typeof(System.Int32));
			Columns.Add(FK_MANUFACTURER_FIELD, typeof(System.Int32));
			Columns.Add(DEFINE_FIELD, typeof(System.String));
			Columns.Add(IMPLEMENTSDCE_FIELD, typeof(System.Int16));
			Columns.Add(ISEMBEDDED_FIELD, typeof(System.Boolean));
			Columns.Add(COMMANDLINE_FIELD, typeof(System.String));
			Columns.Add(ISPLUGANDPLAY_FIELD, typeof(System.Boolean));
			Columns.Add(ISIPBASED_FIELD, typeof(System.Boolean));
			Columns.Add(ISPLUGIN_FIELD, typeof(System.Boolean));
			Columns.Add(IRFREQUENCY_FIELD, typeof(System.Int32));
			Columns.Add(FK_STABILITYSTATUS_FIELD, typeof(System.Int32));
			Table.PrimaryKey = PKColumns;

			return Table;
		}
		protected static void CreateParameters(OdbcParameterCollection Params, bool IsInsert) {
			Params.Add(new OdbcParameter(PK_DEVICETEMPLATE_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(DESCRIPTION_PARM, OdbcType.VarChar, 30));
			Params.Add(new OdbcParameter(COMMENTS_PARM, OdbcType.VarChar, 255));
			Params.Add(new OdbcParameter(FK_DEVICECATEGORY_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_MANUFACTURER_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(DEFINE_PARM, OdbcType.VarChar, 30));
			Params.Add(new OdbcParameter(IMPLEMENTSDCE_PARM, OdbcType.SmallInt,2));
			Params.Add(new OdbcParameter(ISEMBEDDED_PARM, OdbcType.Bit,1));
			Params.Add(new OdbcParameter(COMMANDLINE_PARM, OdbcType.VarChar, 100));
			Params.Add(new OdbcParameter(ISPLUGANDPLAY_PARM, OdbcType.Bit,1));
			Params.Add(new OdbcParameter(ISIPBASED_PARM, OdbcType.Bit,1));
			Params.Add(new OdbcParameter(ISPLUGIN_PARM, OdbcType.Bit,1));
			Params.Add(new OdbcParameter(IRFREQUENCY_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_STABILITYSTATUS_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(USERID_PARM, OdbcType.Int));

			// map the parameters to the data table

			Params[PK_DEVICETEMPLATE_PARM].SourceColumn = DeviceTemplateData.PK_DEVICETEMPLATE_FIELD;
			if(IsInsert) {
				Params[PK_DEVICETEMPLATE_PARM].Direction = ParameterDirection.Output;
			}

			Params[DESCRIPTION_PARM].SourceColumn = DeviceTemplateData.DESCRIPTION_FIELD;
			Params[COMMENTS_PARM].SourceColumn = DeviceTemplateData.COMMENTS_FIELD;
			Params[FK_DEVICECATEGORY_PARM].SourceColumn = DeviceTemplateData.FK_DEVICECATEGORY_FIELD;
			Params[FK_MANUFACTURER_PARM].SourceColumn = DeviceTemplateData.FK_MANUFACTURER_FIELD;
			Params[DEFINE_PARM].SourceColumn = DeviceTemplateData.DEFINE_FIELD;
			Params[IMPLEMENTSDCE_PARM].SourceColumn = DeviceTemplateData.IMPLEMENTSDCE_FIELD;
			Params[ISEMBEDDED_PARM].SourceColumn = DeviceTemplateData.ISEMBEDDED_FIELD;
			Params[COMMANDLINE_PARM].SourceColumn = DeviceTemplateData.COMMANDLINE_FIELD;
			Params[ISPLUGANDPLAY_PARM].SourceColumn = DeviceTemplateData.ISPLUGANDPLAY_FIELD;
			Params[ISIPBASED_PARM].SourceColumn = DeviceTemplateData.ISIPBASED_FIELD;
			Params[ISPLUGIN_PARM].SourceColumn = DeviceTemplateData.ISPLUGIN_FIELD;
			Params[IRFREQUENCY_PARM].SourceColumn = DeviceTemplateData.IRFREQUENCY_FIELD;
			Params[FK_STABILITYSTATUS_PARM].SourceColumn = DeviceTemplateData.FK_STABILITYSTATUS_FIELD;
		}

		protected static void CreateCommands(OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
			if(LoadCommand == null) {
				// Create the command since it's null
				LoadCommand = new OdbcCommand("sp_Select_DeviceTemplate", Conn);
				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(PK_DEVICETEMPLATE_PARM, OdbcType.Int,4));
			}

			if(InsertCommand == null) {
				// Create the command since it's null
				InsertCommand = new OdbcCommand("sp_Insert_DeviceTemplate", Conn);
				InsertCommand.CommandType = CommandType.StoredProcedure;
				InsertCommand.Transaction = Trans;

				OdbcParameterCollection Params = InsertCommand.Parameters;

				CreateParameters(Params, true);

			}

			if(UpdateCommand == null) {
				// Create the command since it's null
				UpdateCommand = new OdbcCommand("sp_Update_DeviceTemplate", Conn);
				UpdateCommand.CommandType = CommandType.StoredProcedure;
				UpdateCommand.Transaction = Trans;

				OdbcParameterCollection Params = UpdateCommand.Parameters;

				CreateParameters(Params, false);

			}
			if (DeleteCommand == null)
			{
				DeleteCommand = new OdbcCommand("sp_Delete_DeviceTemplate", Conn);
				DeleteCommand.CommandType = CommandType.StoredProcedure;
				DeleteCommand.Transaction = Trans;

				DeleteCommand.Parameters.Add(PK_DEVICETEMPLATE_PARM, OdbcType.Int,4, PK_DEVICETEMPLATE_FIELD);
				DeleteCommand.Parameters.Add(USERID_PARM, OdbcType.Int);
			}
		}

		protected static void CreateCommands(OdbcDataAdapter odbcda,OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
				LoadCommand = new OdbcCommand("SELECT PK_DeviceTemplate,Description,Comments,FK_DeviceCategory,FK_Manufacturer,Define,ImplementsDCE,IsEmbedded,CommandLine,IsPlugAndPlay,IsIPBased,IsPlugIn,IRFrequency,FK_StabilityStatus FROM DeviceTemplate", Conn);
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(PK_DEVICETEMPLATE_PARM, OdbcType.Int,4));

			odbcda.SelectCommand = LoadCommand;
			OdbcCommandBuilder odbcCB = new OdbcCommandBuilder(odbcda);
			odbcCB.RefreshSchema();
			DeleteCommand = odbcCB.GetDeleteCommand();
			InsertCommand = odbcCB.GetInsertCommand();
			UpdateCommand = odbcCB.GetUpdateCommand();
		}

		public DeviceTemplateData LoadDeviceTemplate(System.Int32 PK_DeviceTemplate)
		{
			m_DSCommand.SelectCommand = m_LoadCommand;
			m_DSCommand.SelectCommand.Parameters[PK_DEVICETEMPLATE_PARM].Value = PK_DeviceTemplate;

			m_DSCommand.Fill(this);
			return this;
		}

		public static DataRowCollection LoadDeviceTemplateWithWhere(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, string WhereClause) // marker:2
		{
			DataRowCollection dr;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["DeviceTemplate"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}
			
			DataSet dsTemp = new MyDataSet();
			dsTemp.Tables.Add(BuildDataTables());
			
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			string sSQL = "SELECT PK_DeviceTemplate, Description, Comments, FK_DeviceCategory, FK_Manufacturer, Define, ImplementsDCE, IsEmbedded, CommandLine, IsPlugAndPlay, IsIPBased, IsPlugIn, IRFrequency, FK_StabilityStatus FROM DeviceTemplate WHERE " + WhereClause;
			
			OdbcCommand LoadCommand = new OdbcCommand(sSQL,conn);
			
			if( trans!=null )
				LoadCommand.Transaction = trans;
			
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(dsTemp,"DeviceTemplate");
			
			dr=dsTemp.Tables["DeviceTemplate"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}

		public static DataRow LoadNoCacheDeviceTemplate(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 PK_DeviceTemplate)
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["DeviceTemplate"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}

			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand;
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();

			LoadCommand = new OdbcCommand("sp_Select_DeviceTemplate", conn);

			LoadCommand.CommandType = CommandType.StoredProcedure;
			LoadCommand.Parameters.Add(new OdbcParameter(PK_DEVICETEMPLATE_PARM, OdbcType.Int,4));
			LoadCommand.Parameters[PK_DEVICETEMPLATE_PARM].Value = PK_DeviceTemplate;
			if( trans!=null )
				LoadCommand.Transaction = trans;
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(ds,"DeviceTemplate");
				dr = ds.Tables["DeviceTemplate"].Rows.Find(PK_DeviceTemplate);
			return dr;
		}

		public static DataRow LoadDeviceTemplate(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 PK_DeviceTemplate)  // marker:3
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["DeviceTemplate"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
				else
				dr = dt.Rows.Find(PK_DeviceTemplate);
			}

			if( dr==null )
			{
				OdbcDataAdapter sqlda = new OdbcDataAdapter();
				OdbcCommand LoadCommand;
				if( conn==null )
					conn = HADataConfiguration.GetOdbcConnection();

				LoadCommand = new OdbcCommand("sp_Select_DeviceTemplate", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new OdbcParameter(PK_DEVICETEMPLATE_PARM, OdbcType.Int,4));
				LoadCommand.Parameters[PK_DEVICETEMPLATE_PARM].Value = PK_DeviceTemplate;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(ds,"DeviceTemplate");
				dr = ds.Tables["DeviceTemplate"].Rows.Find(PK_DeviceTemplate);
			}
			return dr;
		}

		public DeviceTemplateData LoadAll() {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new OdbcCommand("SELECT * FROM DeviceTemplate", m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this);
			return this;

		}

		public static DataRowCollection LoadAll(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {

			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT * FROM DeviceTemplate", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(ds,"DeviceTemplate")==0 )
				return null;
			else
				return ds.Tables["DeviceTemplate"].Rows;

		}

		public DeviceTemplateData ExecuteQuery(String sSQL) {
			return ExecuteQuery(sSQL,DEVICETEMPLATE_TABLE);
		}
		public DeviceTemplateData ExecuteQuery(String sSQL,String sTableName) {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new OdbcCommand(sSQL, m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this,sTableName);

			return this;
		}

		public static DataRowCollection ExecuteQuery(String sSQL,ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {
			return ExecuteQuery(sSQL,ref ds,conn,trans,"DeviceTemplate");
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

		public bool UpdateDeviceTemplate(int CurUserID) {
			m_DSCommand.UpdateCommand = m_UpdateCommand;
			m_DSCommand.UpdateCommand.Parameters[USERID_PARM].Value = CurUserID;

			m_DSCommand.InsertCommand = m_InsertCommand;
			m_DSCommand.InsertCommand.Parameters[USERID_PARM].Value = CurUserID;

			if( m_DeleteCommand != null )
			{
				m_DSCommand.DeleteCommand = m_DeleteCommand;
				m_DSCommand.DeleteCommand.Parameters[USERID_PARM].Value = CurUserID;
			}

			m_DSCommand.Update(this, DeviceTemplateData.DEVICETEMPLATE_TABLE);
			return true;
		}

		public static bool UpdateDeviceTemplate(ref MyDataSet ds, int CurUserID)
		{
			OdbcConnection OdbcConn = HADataConfiguration.GetOdbcConnection();
			return UpdateDeviceTemplate(ref ds,CurUserID,OdbcConn,null);
		}

		public static bool UpdateDeviceTemplate(ref MyDataSet ds, int CurUserID,OdbcConnection OdbcConn,OdbcTransaction Trans)
		{
			DataTable dt = ds.Tables[DEVICETEMPLATE_TABLE];
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

	} // public class DeviceTemplateData
	public class DeviceTemplateDataRow
	{
		public DataRow dr = null;
		public DeviceTemplateDataRow(DataRow d)
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
		public System.Int32 fPK_DeviceTemplate
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
		public System.String fComments
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
		public bool fCommentsIsNull
		{
			get
			{
				return dr[2]==DBNull.Value;
			}
		}
		public void fCommentsSetNull()
		{
			dr[2]=DBNull.Value;
		}
		public System.Int32 fFK_DeviceCategory
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
		public bool fFK_DeviceCategoryIsNull
		{
			get
			{
				return dr[3]==DBNull.Value;
			}
		}
		public void fFK_DeviceCategorySetNull()
		{
			dr[3]=DBNull.Value;
		}
		public System.Int32 fFK_Manufacturer
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
		public bool fFK_ManufacturerIsNull
		{
			get
			{
				return dr[4]==DBNull.Value;
			}
		}
		public void fFK_ManufacturerSetNull()
		{
			dr[4]=DBNull.Value;
		}
		public System.String fDefine
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
		public bool fDefineIsNull
		{
			get
			{
				return dr[5]==DBNull.Value;
			}
		}
		public void fDefineSetNull()
		{
			dr[5]=DBNull.Value;
		}
		public System.Int16 fImplementsDCE
		{
			get
			{
				return Convert.ToInt16(dr[6]);
			}
			set
			{
				dr[6]=value;
			}
		}
		public bool fImplementsDCEIsNull
		{
			get
			{
				return dr[6]==DBNull.Value;
			}
		}
		public void fImplementsDCESetNull()
		{
			dr[6]=DBNull.Value;
		}
		public System.Boolean fIsEmbedded
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
		public bool fIsEmbeddedIsNull
		{
			get
			{
				return dr[7]==DBNull.Value;
			}
		}
		public void fIsEmbeddedSetNull()
		{
			dr[7]=DBNull.Value;
		}
		public System.String fCommandLine
		{
			get
			{
				return Convert.ToString(dr[8]);
			}
			set
			{
				dr[8]=value;
			}
		}
		public bool fCommandLineIsNull
		{
			get
			{
				return dr[8]==DBNull.Value;
			}
		}
		public void fCommandLineSetNull()
		{
			dr[8]=DBNull.Value;
		}
		public System.Boolean fIsPlugAndPlay
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
		public bool fIsPlugAndPlayIsNull
		{
			get
			{
				return dr[9]==DBNull.Value;
			}
		}
		public void fIsPlugAndPlaySetNull()
		{
			dr[9]=DBNull.Value;
		}
		public System.Boolean fIsIPBased
		{
			get
			{
				return Convert.ToBoolean(dr[10]);
			}
			set
			{
				dr[10]=value;
			}
		}
		public bool fIsIPBasedIsNull
		{
			get
			{
				return dr[10]==DBNull.Value;
			}
		}
		public void fIsIPBasedSetNull()
		{
			dr[10]=DBNull.Value;
		}
		public System.Boolean fIsPlugIn
		{
			get
			{
				return Convert.ToBoolean(dr[11]);
			}
			set
			{
				dr[11]=value;
			}
		}
		public bool fIsPlugInIsNull
		{
			get
			{
				return dr[11]==DBNull.Value;
			}
		}
		public void fIsPlugInSetNull()
		{
			dr[11]=DBNull.Value;
		}
		public System.Int32 fIRFrequency
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
		public bool fIRFrequencyIsNull
		{
			get
			{
				return dr[12]==DBNull.Value;
			}
		}
		public void fIRFrequencySetNull()
		{
			dr[12]=DBNull.Value;
		}
		public System.Int32 fFK_StabilityStatus
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
		public bool fFK_StabilityStatusIsNull
		{
			get
			{
				return dr[13]==DBNull.Value;
			}
		}
		public void fFK_StabilityStatusSetNull()
		{
			dr[13]=DBNull.Value;
		}
	} // public class DeviceTemplateDataRow
	public class DeviceTemplateDataReader
	{
		public OdbcDataReader dr;
		bool bCache=false;
		int iRecord=-1,iNumRecords=-1;
		ArrayList al = null;

		public DeviceTemplateDataReader(OdbcDataReader d)
		{
			dr=d;
		}
		public DeviceTemplateDataReader(OdbcCommand cmd)
		{
			dr = cmd.ExecuteReader();
		}

		public DeviceTemplateDataReader(OdbcCommand cmd,bool Cache)
		{
			dr = cmd.ExecuteReader();
			bCache=Cache;
			if( bCache )
				CacheAndClose();
		}

		public DeviceTemplateDataReader(string sSQL)
		{
			OdbcConnection conn = HADataConfiguration.GetOdbcConnection();

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM DeviceTemplate WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public DeviceTemplateDataReader(string sSQL,OdbcConnection conn)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM DeviceTemplate WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public DeviceTemplateDataReader(string sSQL,OdbcConnection conn,OdbcTransaction trans,bool Cache)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM DeviceTemplate WHERE " + sSQL;
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
		public System.Int32 fPK_DeviceTemplate
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
		public System.String fComments
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[2]);
				else
					return Convert.ToString(dr[2]);
			}
		}
		public bool fCommentsIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[2]==DBNull.Value;
				else
					return dr[2]==DBNull.Value;
			}
		}
		public System.Int32 fFK_DeviceCategory
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[3]);
				else
					return Convert.ToInt32(dr[3]);
			}
		}
		public bool fFK_DeviceCategoryIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[3]==DBNull.Value;
				else
					return dr[3]==DBNull.Value;
			}
		}
		public System.Int32 fFK_Manufacturer
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[4]);
				else
					return Convert.ToInt32(dr[4]);
			}
		}
		public bool fFK_ManufacturerIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[4]==DBNull.Value;
				else
					return dr[4]==DBNull.Value;
			}
		}
		public System.String fDefine
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[5]);
				else
					return Convert.ToString(dr[5]);
			}
		}
		public bool fDefineIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[5]==DBNull.Value;
				else
					return dr[5]==DBNull.Value;
			}
		}
		public System.Int16 fImplementsDCE
		{
			get
			{
				if( bCache )
					return Convert.ToInt16(((object[]) al[iRecord])[6]);
				else
					return Convert.ToInt16(dr[6]);
			}
		}
		public bool fImplementsDCEIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[6]==DBNull.Value;
				else
					return dr[6]==DBNull.Value;
			}
		}
		public System.Boolean fIsEmbedded
		{
			get
			{
				if( bCache )
					return Convert.ToBoolean(((object[]) al[iRecord])[7]);
				else
					return Convert.ToBoolean(dr[7]);
			}
		}
		public bool fIsEmbeddedIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[7]==DBNull.Value;
				else
					return dr[7]==DBNull.Value;
			}
		}
		public System.String fCommandLine
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[8]);
				else
					return Convert.ToString(dr[8]);
			}
		}
		public bool fCommandLineIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[8]==DBNull.Value;
				else
					return dr[8]==DBNull.Value;
			}
		}
		public System.Boolean fIsPlugAndPlay
		{
			get
			{
				if( bCache )
					return Convert.ToBoolean(((object[]) al[iRecord])[9]);
				else
					return Convert.ToBoolean(dr[9]);
			}
		}
		public bool fIsPlugAndPlayIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[9]==DBNull.Value;
				else
					return dr[9]==DBNull.Value;
			}
		}
		public System.Boolean fIsIPBased
		{
			get
			{
				if( bCache )
					return Convert.ToBoolean(((object[]) al[iRecord])[10]);
				else
					return Convert.ToBoolean(dr[10]);
			}
		}
		public bool fIsIPBasedIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[10]==DBNull.Value;
				else
					return dr[10]==DBNull.Value;
			}
		}
		public System.Boolean fIsPlugIn
		{
			get
			{
				if( bCache )
					return Convert.ToBoolean(((object[]) al[iRecord])[11]);
				else
					return Convert.ToBoolean(dr[11]);
			}
		}
		public bool fIsPlugInIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[11]==DBNull.Value;
				else
					return dr[11]==DBNull.Value;
			}
		}
		public System.Int32 fIRFrequency
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[12]);
				else
					return Convert.ToInt32(dr[12]);
			}
		}
		public bool fIRFrequencyIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[12]==DBNull.Value;
				else
					return dr[12]==DBNull.Value;
			}
		}
		public System.Int32 fFK_StabilityStatus
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[13]);
				else
					return Convert.ToInt32(dr[13]);
			}
		}
		public bool fFK_StabilityStatusIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[13]==DBNull.Value;
				else
					return dr[13]==DBNull.Value;
			}
		}
	} // public class DeviceTemplateDataReader
	public class DeviceTemplateTable : DataTable
	{
		public DeviceTemplateTable() : base("DeviceTemplate") {}

		public DeviceTemplateDataRow this [System.Int32 PK_DeviceTemplate]
		{
			get
			{
				DeviceTemplateDataRow dr = new DeviceTemplateDataRow(Rows.Find(PK_DeviceTemplate));
				if( !dr.bIsValid  && false /* can't do this with ODBC */  )
				{
					MyDataSet mds = (MyDataSet) DataSet;
					if( mds.m_conn==null )
						return dr;
					OdbcDataAdapter sqlda = new OdbcDataAdapter();
					OdbcCommand LoadCommand = new OdbcCommand("sp_Select_DeviceTemplate", mds.m_conn,mds.m_trans);
					LoadCommand.CommandType = CommandType.StoredProcedure;
					LoadCommand.Parameters.Add(new OdbcParameter("@PK_DeviceTemplate", OdbcType.Int,4));
					LoadCommand.Parameters["@PK_DeviceTemplate"].Value = PK_DeviceTemplate;
					sqlda.SelectCommand = LoadCommand;
					sqlda.Fill(mds,"DeviceTemplate");
					dr = new DeviceTemplateDataRow(Rows.Find(PK_DeviceTemplate));
				}
				return dr;
			}
		}
		public DataRowCollection LoadAll(OdbcConnection conn, OdbcTransaction trans)
		{
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT PK_DeviceTemplate,Description,Comments,FK_DeviceCategory,FK_Manufacturer,Define,ImplementsDCE,IsEmbedded,CommandLine,IsPlugAndPlay,IsIPBased,IsPlugIn,IRFrequency,FK_StabilityStatus FROM DeviceTemplate", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(this.DataSet,"DeviceTemplate")==0 )
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
			DeviceTemplateData.UpdateDeviceTemplate(ref ds,PK_Users,conn,trans);
		}
		public DataColumn cPK_DeviceTemplate
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
		public DataColumn cComments
		{
			get
			{
				return Columns[2];
			}
		}
		public DataColumn cFK_DeviceCategory
		{
			get
			{
				return Columns[3];
			}
		}
		public DataColumn cFK_Manufacturer
		{
			get
			{
				return Columns[4];
			}
		}
		public DataColumn cDefine
		{
			get
			{
				return Columns[5];
			}
		}
		public DataColumn cImplementsDCE
		{
			get
			{
				return Columns[6];
			}
		}
		public DataColumn cIsEmbedded
		{
			get
			{
				return Columns[7];
			}
		}
		public DataColumn cCommandLine
		{
			get
			{
				return Columns[8];
			}
		}
		public DataColumn cIsPlugAndPlay
		{
			get
			{
				return Columns[9];
			}
		}
		public DataColumn cIsIPBased
		{
			get
			{
				return Columns[10];
			}
		}
		public DataColumn cIsPlugIn
		{
			get
			{
				return Columns[11];
			}
		}
		public DataColumn cIRFrequency
		{
			get
			{
				return Columns[12];
			}
		}
		public DataColumn cFK_StabilityStatus
		{
			get
			{
				return Columns[13];
			}
		}
	}
} // namespace HAData.Common.Data
