using System;
using System.Collections;
using System.Data;
using System.Data.SqlClient;

namespace Criteria
{
	/// <summary>
	/// Summary description for Criteria.
	/// </summary>
	public class CriteriaParmList
	{
		int m_iPK_CriteriaParmList;
		string m_sName;
		public ArrayList m_alCriteriaValues;
		public bool m_bAllowText;
		public bool m_bAllowGLComparisson;

		public CriteriaParmList(int ID,string Name,DataRowCollection DataRows,bool AllowText,bool AllowGLComparisson)
		{
			m_alCriteriaValues = new ArrayList();
			foreach(DataRow dr in DataRows)
				m_alCriteriaValues.Add(new CriteriaValue(Convert.ToInt32(dr[0]),dr[1].ToString()));

			m_iPK_CriteriaParmList = ID;
			m_sName = Name;
			m_bAllowText = AllowText;
			m_bAllowGLComparisson=AllowGLComparisson;
		}

		public CriteriaParmList(int ID,string Name,DataRow[] DataRows,bool AllowText,bool AllowGLComparisson)
		{
			m_alCriteriaValues = new ArrayList();
			foreach(DataRow dr in DataRows)
				m_alCriteriaValues.Add(new CriteriaValue(Convert.ToInt32(dr[0]),dr["description"].ToString()));

			m_iPK_CriteriaParmList = ID;
			m_sName = Name;
			m_bAllowText = AllowText;
			m_bAllowGLComparisson=AllowGLComparisson;
		}

		public CriteriaParmList(int ID,string Name,ArrayList CriteriaValues,bool AllowText,bool AllowGLComparisson)
		{
			m_iPK_CriteriaParmList = ID;
			m_sName = Name;
			m_alCriteriaValues = CriteriaValues;
			m_bAllowText = AllowText;
			m_bAllowGLComparisson=AllowGLComparisson;
		}



		public string Name 
		{
			get 
			{
				return m_sName;
			}
		}

		public int ID
		{
			get
			{
				return m_iPK_CriteriaParmList;
			}
		}
	}
}
