using System;

namespace Criteria
{
	/// <summary>
	/// Summary description for DisplayLine.
	/// </summary>
	public class DisplayLine
	{
		
		public int iType;
		public string sText;
		public bool bSelected;
		public int iLevel;
		public int iLineNumber;

		public NestingParameter oNestingParameter;
		public CriteriaParm oCriteriaParm;

		public DisplayLine oMate;

		public DisplayLine()
		{
			//
			// TODO: Add constructor logic here
			//
		}

		public DisplayLine(int iTypeLocal, string sTextLocal)
		{
			iType = iTypeLocal;
			sText = sTextLocal;
		}

	}

	
	public class DisplayLineType
	{
		public const int Criteria = 1;
		public const int NestBegin = 2;
		public const int NestEnd = 3;
	}


}
