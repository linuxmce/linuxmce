using System;

namespace HAData.Common
{
	/// <summary>
	/// Summary description for IDValue.
	/// </summary>
	public class IDValue
	{
		int m_i;
		string m_s;
		public IDValue(int i,string s)
		{
			m_s=s;
			m_i=i;
		}

		public int i
		{
			get
			{
				return m_i;
			}
		}

		public string s
		{
			get
			{
				return m_s;
			}
		}
	}
}
