using System;

namespace HAData.Common
{
	/// <summary>
	/// Summary description for HARelation.
	/// </summary>
	public class HARelation
	{
		public static string rDesignObjHeader__DesignObjHeader_DesignObjCategory = "OHOHC";
		public static string rDesignObjCategory__DesignObjCategory_Parent = "OTOT__P";
		public static string rDesignObjHeader__DesignObjDetail = "OHOD";
		public static string rDesignObjCategory__DesignObjHeader_DesignObjCategory = "OTOHO";
		public static string rDesignObjDetail__DesignObjDetail_DesignObjHeader = "ODOH";
		public static string rDesignObjHeader__DesignObjHeader_DesignObjHeader = "OHOHOH";
		public static string rDesignObjHeader__DesignObjHeader_DesignObjHeader_Parent = "OHOHOH_P";
		public static string rDesignObjDetail__DesignObjDetail_DesignObjParameter = "ODOCP";
		public static string rDesignObjHeader__DesignObjHeader_DesignObjParameter = "OHOHCP";
		public static string rDesignObjHeader__DesignObjHeader_Zone = "OHOHZ";
		public static string rDesignObjDetail__DesignObjDetail_Zone = "OHODZ";
		public static string rDesignObjHeader__DesignObjHeader_Text = "OHOHT";
		public static string rDesignObjDetail__DesignObjDetail_Text = "OHODT";
		public static string rDesignObjHeader_Text__DesignObjHeader_Text_Skin_Language = "OHT_OHTSS";
		public static string rDesignObjDetail_Text__DesignObjDetail_Text_Skin_Language = "ODT_ODTSS";

		public static string rDesignObjDetail__DesignObjDetail_Command = "OD_ODA";
		public static string rDesignObjDetail_Command__DesignObjDetail_Command_CommandParameter = "ODA_ODAP";
		public static string rDesignObjDetail_Zone__DesignObjDetail_Command = "ODZ_ODA";
		public static string rDesignObjHeader__DesignObjHeader_Command = "OH_OHA";
		public static string rDesignObjHeader_Command__DesignObjHeader_Command_CommandParameter = "OHA_OHAP";
		public static string rDesignObjHeader_Zone__DesignObjHeader_Command = "OHZ_OHA";
		public static string rDesignObjType__DesignObjType_DesignObjParameter = "OTP";
		public static string rDesignObjParameter__DesignObjType_DesignObjParameter = "OPT";
		public static string rCommand__Command_CommandParameter = "A_AP";

		public static string rInstallation__Controller = "I_C";
		public static string rController__Controller_Variable = "C_CV";
		public static string rController__Controller_C_Mode = "C_C_M";

		public static string rScenario__Scenario_Command = "S_SA";
		public static string rScenario_Command__Scenario_Command_CommandParameter = "SA_SAP";

		public static string rDeviceCategory__Device = "DTD";
		public static string rDeviceCategory__DeviceCategory_Parent = "DTDT__P";
		public static string rCommandCategory__Command = "ATA";
		public static string rCommandCategory__CommandCategory_Parent = "ATAT__P";
		public static string rEventCategory__Event = "ETET";
		public static string rEventCategory__EventCategory_Parent = "ETET__P";
		public static string rTextCategory__Text = "TCT";
		public static string rTextCategory__TextCategory_Parent = "TCT__P";
		public static string rText__Text_LS = "T_TLS";
		public HARelation()
		{
			//
			// TODO: Add constructor logic here
			//
		}
	}
}
