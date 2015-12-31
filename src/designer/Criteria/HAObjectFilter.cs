using System;
using HAData.Common;
using HAData.DataAccess;

namespace Criteria
{
	/// <summary>
	/// Summary description for HAObjectFilter.
	/// </summary>
	public class HAObjectFilter : CriteriaList
	{
		MyDataSet m_mds = HADataConfiguration.m_mdsCache;

		public HAObjectFilter()
		{
			m_iPK_CriteriaList = CriteriaListData.OBJECT_FILTER_CONST;

			// Temporary - throw in some values
			HADataConfiguration.iFK_System = 1;
			HADataConfiguration.iFK_Skin = 1;
			HADataConfiguration.iFK_Size = 1;
			HADataConfiguration.iFK_Language = 1;

			alCriteriaParmList.Add( new CriteriaParmList(CriteriaParmListData.SYSTEM_CONST,"System",m_mds.tSystem.Rows,false,false) );
			alCriteriaParmList.Add( new CriteriaParmList(CriteriaParmListData.SKIN_CONST,"Skin",m_mds.tSkin.Rows,false,false) );
			alCriteriaParmList.Add( new CriteriaParmList(CriteriaParmListData.INSTALLATION_CONST,"Installation",m_mds.tInstallation.Rows,false,false) );
		}

		public override bool EvaluateExpression(CriteriaParm criteriaParm)
		{
			CriteriaParmList c = criteriaParm.m_CriteriaParmList;
			object oComparer=null;

			switch( c.ID )
			{
				case CriteriaParmListData.SYSTEM_CONST:
					oComparer = HADataConfiguration.iFK_System;
					break;
				case CriteriaParmListData.SKIN_CONST:
					oComparer = HADataConfiguration.iFK_Skin;
					break;
				case CriteriaParmListData.INSTALLATION_CONST:
					oComparer = HADataConfiguration.iFK_Installation;
					break;
			}

			if( oComparer==null ) 
				return false;

			return CompareValues(oComparer,criteriaParm.m_Operator,criteriaParm.m_oValue);
		}
	}
}
