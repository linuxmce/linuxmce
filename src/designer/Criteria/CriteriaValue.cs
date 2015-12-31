using System;

namespace Criteria
{
	/// <summary>
	/// Summary description for CriteriaValue.
	/// </summary>
	public class CriteriaValue
	{
		int m_ID;
		string m_Name;

	
		public CriteriaValue(int ID,string Name)
		{
			m_ID = ID;
			m_Name = Name;
		}

		public int ID
		{
			get
			{
				return m_ID;
			}
		}

		public string Name
		{
			get
			{
				return m_Name;
			}
		}


	}
}
