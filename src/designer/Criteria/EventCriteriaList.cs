using System;
using System.Collections;
using HAData.Common;
using HAData.DataAccess;

namespace Criteria
{
	/// <summary>
	/// Summary description for EventCriteriaList.
	/// </summary>
	public class EventCriteriaList : CriteriaList
	{

		MyDataSet m_mds = HADataConfiguration.m_mdsCache;



		public EventCriteriaList(int intInstallationID)
		{
			/*
			 * PK_CriteriaParmList	FK_ParameterType	Description	Define
			5	2	Month	MONTH
			8	2	Device	PK_DEVICE
			10	2	Master Device List	PK_MASTERDEVICELIST
			11	1	Time of day	TIME_OF_DAY
			12	2	Device Category	PK_DEVICECATEGORY
			13	2	Room	PK_ROOM
			14	2	Device Group	PK_DEVICEGROUP
			15	2	House Mode	C_MODE_HOUSE
			16	2	Room Mode	C_MODE_ROOM
			17	2	Day Of Week	DAY_OF_WEEK
			18	2	Day Of Month	DAY_OF_MONTH
			19	1	Specific Date	SPECIFIC_DATE
			20	2	Room Type	PK_C_ROOMTYPE
			22	2	Event List	PK_EVENTLIST
			23	2	Event	PK_EVENT
			*/
/*			
			m_iPK_CriteriaList = CriteriaListData.EVENT_CONST;

			
			string strDays = "1,Monday|2,Tuesday|3,Wednesday|4,Thursday|5,Friday|6,Saturday|7,Sunday";
			string strMonths = "1,January|2,February|3,March|4,April|5,May|6,June|7,July|8,August|9,September|10,October|11,November|12,December";


			alCriteriaParmList.Add( new CriteriaParmList(CriteriaParmListData.MONTH_CONST,"Month",getCriteriaValuesArrayList(strMonths),false,false) );
			alCriteriaParmList.Add( new CriteriaParmList(CriteriaParmListData.PK_DEVICE_CONST,"Device",m_mds.tDevice.Select(DeviceData.FK_INSTALLATION_FIELD + "=" + intInstallationID, DeviceData.DESCRIPTION_FIELD),false,false) );
			//			alCriteriaParmList.Add( new CriteriaParmList(CriteriaParmListData.TIME_OF_DAY_CONST,"Time of Day",true) );
			alCriteriaParmList.Add( new CriteriaParmList(CriteriaParmListData.PK_DEVICECATEGORY_CONST,"Device Category",m_mds.tDeviceCategory.Rows,false,false) );
			alCriteriaParmList.Add( new CriteriaParmList(CriteriaParmListData.PK_ROOM_CONST,"Room",m_mds.tRoom.Select(RoomData.FK_INSTALLATION_FIELD + "=" + intInstallationID, RoomData.DESCRIPTION_FIELD),false,false) );
			alCriteriaParmList.Add( new CriteriaParmList(CriteriaParmListData.C_MODE_HOUSE_CONST,"House Mode",m_mds.tC_HouseMode.Rows,false,false) );
			alCriteriaParmList.Add( new CriteriaParmList(CriteriaParmListData.C_MODE_ROOM_CONST,"Room Mode",m_mds.tC_RoomMode.Rows,false,false) );
			alCriteriaParmList.Add( new CriteriaParmList(CriteriaParmListData.DAY_OF_WEEK_CONST,"Day of Week",getCriteriaValuesArrayList(strDays),false,false) );
			//			alCriteriaParmList.Add( new CriteriaParmList(CriteriaParmListData.SPECIFIC_DATE_CONST,"Date",true) );
			alCriteriaParmList.Add( new CriteriaParmList(CriteriaParmListData.PK_C_ROOMTYPE_CONST,"Room Type",m_mds.tC_RoomType.Rows,false,false) );
			alCriteriaParmList.Add( new CriteriaParmList(CriteriaParmListData.PK_EVENTLIST_CONST,"Event List",m_mds.tEventList.Rows,false,false) );
			alCriteriaParmList.Add( new CriteriaParmList(CriteriaParmListData.PK_EVENT_CONST,"Event",m_mds.tEvent.Select(EventData.FK_INSTALLATION_FIELD + "=" + intInstallationID, EventData.DESCRIPTION_FIELD),false,false) );

			alCriteriaParmList.Add( new CriteriaParmList(CriteriaParmListData.EVENT_PARM_CONST,"Event Parameter", false) );
*/
		}


		public ArrayList getCriteriaValuesArrayList(string strIn)
		{
			ArrayList alOut = new ArrayList();
			CriteriaValue objCriteriaValue;
			string[] arPair;
			foreach(string strPair in strIn.Split(Convert.ToChar("|")))
			{
				arPair = strPair.Split(Convert.ToChar(","));
				objCriteriaValue = new CriteriaValue(Convert.ToInt32(arPair[0]),arPair[1]);
				alOut.Add(objCriteriaValue);
			}
			return alOut;
		}


	}
}
