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

			
			m_iPK_CriteriaList = 2;

			
			//string strDays = "1,Monday|2,Tuesday|3,Wednesday|4,Thursday|5,Friday|6,Saturday|7,Sunday";
			//string strMonths = "1,January|2,February|3,March|4,April|5,May|6,June|7,July|8,August|9,September|10,October|11,November|12,December";


			//alCriteriaParmList.Add( new CriteriaParmList(CriteriaParmListData.DAY_CONST,"Day of Week",getCriteriaValuesArrayList(strDays),false,false) );
			//alCriteriaParmList.Add( new CriteriaParmList(CriteriaParmListData.MONTH_CONST,"Month",getCriteriaValuesArrayList(strMonths),false,false) );

			alCriteriaParmList.Add( new CriteriaParmList(CriteriaParmListData.ROOM_CONST,"Room",m_mds.tRoom.Select(RoomData.FK_INSTALLATION_FIELD + "=" + intInstallationID, RoomData.DESCRIPTION_FIELD),false,false) );


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
