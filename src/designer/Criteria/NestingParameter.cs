using System;
using System.Collections;

namespace Criteria
{
	/// <summary>
	/// Summary description for NestingParameter.
	/// </summary>
	public class Operator
	{
		public const int iEquals = 1;
		public const int iNotEquals = 2;
		public const int iGreaterThan = 3;
		public const int iGreaterThanEqual = 4;
		public const int iLessThan = 5;
		public const int iLessThanEqual = 6;


		static public string String(int iOperator)
		{
			switch(iOperator)
			{
				case iEquals:
					return "=";
				case iNotEquals:
					return "<>";
				case iGreaterThan:
					return ">";
				case iGreaterThanEqual:
					return ">=";
				case iLessThan:
					return "<";
				case iLessThanEqual:
					return "<=";

			}
			return "Unknown";
		}
	}

	public class NestingParameter
	{
		public int iID = -1;
		public int iParentID = -1;
		public bool bNot;
		public bool bAnd;
		public ArrayList alCriteriaParm;
		public int iOperator = Operator.iEquals;
		public ArrayList alNestingParameter;
		public NestingParameter m_ParentNestingParameter;
		public DisplayLine m_DisplayLine;
		public int iIndexOfParent;
		string spaces = "                                            "; // todo - really dumb

		public NestingParameter(NestingParameter ParentNestingParameter)
		{
			bNot = false;
			m_ParentNestingParameter = ParentNestingParameter;
			alNestingParameter = new ArrayList();
			alCriteriaParm = new ArrayList();
			//
			// TODO: Add constructor logic here
			//
		}

		public string RenderFormulaAsText(NestingParameter SelectedNestingParameter, int Level,int LineCount,ArrayList alLines,int SelectedLine)
		{
			string s;
			if(this == SelectedNestingParameter)
			{
				if(bNot)
				{
					s = spaces.Substring(0,Level *5) + "NOT ((\r\n";
				}
				else
				{
					s = spaces.Substring(0,Level *5) + "((\r\n";
				}
			}
			else
			{
				if(bNot)
				{
					s = spaces.Substring(0,Level *5) + "NOT (\r\n";
				}
				else
				{
					s = spaces.Substring(0,Level *5) + "(\r\n";
				}
			}
			

			int iCount = -1;
			foreach(CriteriaParm CriteriaParm in alCriteriaParm)
			{
				iCount++;

				s += LineCount==SelectedLine ? "***" : "   ";

				// get rid of spaces
				s += (LineCount++).ToString() + " " + spaces.Substring(0,(Level+1) *5) + CriteriaParm.m_CriteriaParmList.Name + " " + Operator.String(CriteriaParm.m_Operator)
					+ " " + CriteriaParm.m_oValue.ToString();

				alLines.Add(CriteriaParm);
				
				if(iCount < alCriteriaParm.Count - 1 || alNestingParameter.Count > 0)
				{
					s += (bAnd ? " and" : " or"); 
				}
				
				s += "\r\n";
			}
			foreach(NestingParameter np in alNestingParameter)
			{
				s += np.RenderFormulaAsText(SelectedNestingParameter, Level+1, LineCount, alLines,SelectedLine);
			}

			
			if(this == SelectedNestingParameter)
			{
				s += spaces.Substring(0,Level *5) + "))";
			}
			else
			{
				s += spaces.Substring(0,Level *5) + ")";
			}

			if(this.m_ParentNestingParameter != null)
			{
				if(this.m_ParentNestingParameter.alNestingParameter.Count - 1 > this.iIndexOfParent)
				{
					s += (bAnd ? " and" : " or"); 
				}
			}

			s += "\r\n";


			return s;
		}

		public bool Evaluate(CriteriaList criteriaList)
		{
			foreach(CriteriaParm CriteriaParm in alCriteriaParm)
			{
				bool bResult = criteriaList.EvaluateExpression(CriteriaParm);
				if( bAnd==true && bResult==false )
					return !(!bNot ^ false) ;
				if( bAnd==false && bResult==true )
					return !(!bNot ^ true);
			}
			foreach(NestingParameter np in alNestingParameter)
			{
				bool bResult = np.Evaluate(criteriaList);
				if( bAnd==true && bResult==false )
					return !(!bNot ^ false);
				if( bAnd==false && bResult==true )
					return !(!bNot ^ true);
			}
			// If this is an 'and' we're true because we got through everything without a false,
			// If this is an 'or' we're false because we never got a true value
			return !(!bNot ^ bAnd);  
		}


		public void BuildLines(ArrayList alLines, int iLevel, ref int iLineNumber, int iSelectedLineNumber, NestingParameter oSelectedNestingParameter)
		{
			
			DisplayLine objNewLine;
			string s;

			DisplayLine objBeginNestLine;

			
			objNewLine = new DisplayLine();
			objNewLine.iType = DisplayLineType.NestBegin;
			objNewLine.oNestingParameter = this;
			objNewLine.oNestingParameter.m_DisplayLine = objNewLine;
			objNewLine.iLevel = iLevel;
			objNewLine.iLineNumber = iLineNumber++;
			s = "";
			

			s = spaces.Substring(0,iLevel *5);


			if(bNot)
			{
				s += "NOT ";
			}

			//if(objNewLine.iLineNumber == iSelectedLineNumber)
			if(this == oSelectedNestingParameter)
			{
				objNewLine.bSelected = true;
				s += "((";
			}
			else
			{
				s += "(";
			}
			objNewLine.sText = s;
			
			alLines.Add(objNewLine);
			this.m_DisplayLine = objNewLine;

			objBeginNestLine = objNewLine;

			string strValueTemp;
			

			int iCount = -1;
			foreach(CriteriaParm CriteriaParm in alCriteriaParm)
			{
				iCount++;

			
				objNewLine = new DisplayLine();
				objNewLine.iType = DisplayLineType.Criteria;
				objNewLine.oCriteriaParm = (CriteriaParm) CriteriaParm;
				objNewLine.oCriteriaParm.m_DisplayLine = objNewLine;
				objNewLine.iLevel = iLevel;
				objNewLine.iLineNumber = iLineNumber++;
				if(objNewLine.iLineNumber == iSelectedLineNumber)
				{
					objNewLine.bSelected = true;
				}
				
				s = "";
				// get rid of spaces

				CriteriaValue objCriteriaValueTemp = (CriteriaValue) CriteriaParm.m_oValue;
				strValueTemp = objCriteriaValueTemp.Name;
				//strValueTemp = CriteriaParm.m_oValue.ToString();

				s += spaces.Substring(0,(iLevel+1) *5) + CriteriaParm.m_CriteriaParmList.Name + " " + Operator.String(CriteriaParm.m_Operator)
					+ " " + strValueTemp;
				//alLines.Add(CriteriaParm);
				if(iCount < alCriteriaParm.Count - 1 || alNestingParameter.Count > 0)
				{
					s += (bAnd ? " and" : " or"); 
				}
				objNewLine.sText = s;

				alLines.Add(objNewLine);
				


				//s += "\r\n";
			}
			foreach(NestingParameter np in alNestingParameter)
			{
				//s += np.RenderFormulaAsText(SelectedNestingParameter, iLevel+1, LineCount, alLines,SelectedLine);
				np.BuildLines(alLines, iLevel+1, ref iLineNumber, iSelectedLineNumber, oSelectedNestingParameter);
			}

			


			objNewLine = new DisplayLine();
			objNewLine.iType = DisplayLineType.NestEnd;
			objNewLine.oNestingParameter = this;
			objNewLine.iLevel = iLevel;
			objNewLine.iLineNumber = iLineNumber++;

			//attach to mate
			objBeginNestLine.oMate = objNewLine;
			objNewLine.oMate = objBeginNestLine;

				
			s = "";

			//if(objNewLine.iLineNumber == iSelectedLineNumber)
			//if(this == oSelectedNestingParameter)
			//{
			//	objNewLine.bSelected = true;
				//update the string for the beginning (
			//	objNewLine.oMate.sText = spaces.Substring(0,iLevel *5) + "((";
			//}




			//if(objNewLine.iLineNumber == iSelectedLineNumber || objNewLine.oMate.iLineNumber == iSelectedLineNumber)
			if(this == oSelectedNestingParameter)
			{
				s += spaces.Substring(0,iLevel *5) + "))";
			}
			else
			{
				s += spaces.Substring(0,iLevel *5) + ")";
			}

			if(this.m_ParentNestingParameter != null)
			{
				if(this.m_ParentNestingParameter.alNestingParameter.Count - 1 > this.iIndexOfParent)
				{
					s += (bAnd ? " and" : " or"); 
				}
			}

			objNewLine.sText = s;


			


			alLines.Add(objNewLine);

			//s += "\r\n";




		}


	}
}
