using System;

namespace Criteria
{
	/// <summary>
	/// Summary description for CriteriaParameter.
	/// </summary>
	public class CriteriaParm
	{
		public int iID = -1;
		public int iParentID = -1;
		public CriteriaParmList m_CriteriaParmList;
		public object m_oValue;
		public int m_Operator;
		public NestingParameter m_ParentNestingParameter;

		public DisplayLine m_DisplayLine;

		public CriteriaParm(CriteriaParmList criteriaParmList,int Operator,object Value)
		{

			int intValueTemp;

			m_CriteriaParmList = criteriaParmList;
			m_Operator = Operator;

			//now, the value will be the CriteriaValue object, not just the value;

			if(criteriaParmList.m_bAllowText)
			{
				m_oValue = Value;
			}
			else
			{
				m_oValue = null;
				intValueTemp = Convert.ToInt32(Value);
				foreach(CriteriaValue objCriteriaValue in criteriaParmList.m_alCriteriaValues)
				{
					if(objCriteriaValue.ID == intValueTemp)
					{
						m_oValue = objCriteriaValue;
						break;
					}
				}
			}
		
		}
	}
}
