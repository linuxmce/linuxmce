using System;

namespace HADesigner
{
	/// <summary>
	/// Summary description for StringPair.
	/// </summary>
	public class StringPair
	{
		private string m_strID;
		private string m_strDescription;

		public StringPair(string strID, string strDescription)
		{
			this.m_strID = strID;
			this.m_strDescription = strDescription;
		}

		public StringPair(int intID, string strDescription):this(intID.ToString(),strDescription)
		{
		}

		public int intID
		{
			get {return Convert.ToInt32(this.m_strID);}
		}

		public string ID
		{
			get	{return m_strID;}
		}

		public string Description
		{
			get	{return m_strDescription;}
		}

        public static int getIndex(StringPair[] spArray, string id)
		{
			for (int i=0; i<spArray.Length; i++)
			{
				if (spArray[i].ID == id) return i;
			}
			return -1;
		}

		public static bool BothEmptyOrBothSet(string x, string y)
		{
			return (x == "" && y == "") || (x != "" && y != "");
		}
	}
}
