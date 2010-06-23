using System;
using System.Collections;
using System.Data;
using System.Data.SqlClient;
using HAData.Common;
using HAData.DataAccess;

using System.Text.RegularExpressions;


namespace Criteria
{
	/// <summary>
	/// Summary description for CriteriaCollection.
	/// </summary>
	public class CriteriaList
	{
		public int m_iPK_Criteria=0;
		public int m_iPK_CriteriaList=0;
		MyDataSet mds = HADataConfiguration.m_mdsCache;

		public ArrayList alCriteriaParmList = new ArrayList();
		//public NestingParameter m_NestingParameter;

		//following used for building structure from a serialized string
		private ArrayList alSerialNestingParms;
		private ArrayList alSerialCriterias;


		public NestingParameter RootNestingParameter;
		public NestingParameter SelectedNestingParameter;

		public ArrayList alLines;

		public int iSelectedLineNumber = 0;
		
		//public NestingParameter oCurrentNestingParameter;
		
		public CriteriaList()
		{
			//m_NestingParameter = new NestingParameter(null);
			RootNestingParameter = new NestingParameter(null);
			SelectedNestingParameter = RootNestingParameter;
		}

		public void BuildLines()
		{
			int iFirstLineNumber = 0;
			alLines = new ArrayList();
			RootNestingParameter.BuildLines(alLines, 0, ref iFirstLineNumber, iSelectedLineNumber, SelectedNestingParameter);
		}

		public bool Evaluate()
		{
			//return m_NestingParameter.Evaluate(this);
			return RootNestingParameter.Evaluate(this);
		}

		virtual public bool EvaluateExpression(CriteriaParm criteriaParm)
		{
			// this should never get called since the class derived from this needs to handle this method
			throw new Exception("not right");
		}

		public bool CompareValues(object oValue1,int iOperator,object oValue2)
		{
			if( iOperator==Operator.iEquals )
				return oValue1==oValue2;
			if( iOperator==Operator.iNotEquals)
				return oValue1!=oValue2;
			/*
			if( iOperator==Operator.iGreaterThan )
				return oValue1>oValue2;
			if( iOperator==Operator.iGreaterThanEqual)
				return oValue1>=oValue2;
			if( iOperator==Operator.iLessThan)
				return oValue1>oValue2;
			if( iOperator==Operator.iLessThanEqual)
				return oValue1>=oValue2;
			*/
			return false; // shouldn't happen
		}

		public CriteriaParmList GetFromID(int PK_CriteriaParmList)
		{
			foreach(CriteriaParmList c in alCriteriaParmList)
			{
				if( c.ID == PK_CriteriaParmList )
					return c;
			}
			return null;
		}

		public void LoadFromDatabase(int PK_Criteria)
		{
			Criteria_DDataRow drCriteria = mds.tCriteria_D[PK_Criteria];
			m_iPK_CriteriaList = drCriteria.fFK_CriteriaList;
			RootNestingParameter = new NestingParameter(null);
			RootNestingParameter.iID = drCriteria.fFK_CriteriaParmNesting_D;
			RootNestingParameter.iParentID = -1;
			RootNestingParameter.bAnd = drCriteria.fFK_CriteriaParmNesting_D_DataRow.fIsAnd;
			RootNestingParameter.bNot = drCriteria.fFK_CriteriaParmNesting_D_DataRow.fIsNot;
			LoadNesting(drCriteria.fFK_CriteriaParmNesting_D_DataRow,RootNestingParameter);
			SelectedNestingParameter = RootNestingParameter;
		}

		public void LoadNesting(CriteriaParmNesting_DDataRow drCPN,NestingParameter nestingParameter)
		{

			foreach(DataRow dr in drCPN.dr.GetChildRows(MyDataSet.MyRelations.CriteriaParm_D_FK_CriteriaParmNesting_D) )
			{
				CriteriaParm_DDataRow drCP = new CriteriaParm_DDataRow(dr);

				CriteriaParm cp = new CriteriaParm(GetFromID(drCP.fFK_CriteriaParmList),drCP.fOperator,drCP.fValue);
				cp.iID = drCP.fPK_CriteriaParm;
				cp.iParentID = drCPN.fPK_CriteriaParmNesting_D;
				nestingParameter.alCriteriaParm.Add(cp);

			}
			foreach(DataRow dr in drCPN.dr.GetChildRows(MyDataSet.MyRelations.CriteriaParmNesting_D_FK_CriteriaParmNesting_D_Parent) )
			{
				CriteriaParmNesting_DDataRow drCPNChild = new CriteriaParmNesting_DDataRow(dr);
				NestingParameter np = new NestingParameter(nestingParameter);
				np.iID = drCPNChild.fPK_CriteriaParmNesting_D;
				np.iParentID = drCPN.fPK_CriteriaParmNesting_D;
				np.bAnd = drCPNChild.fIsAnd;
				np.bNot = drCPNChild.fIsNot;
				nestingParameter.alNestingParameter.Add(np);
				LoadNesting(drCPNChild,np);
			}
		}

		public void SaveToDatabase(string Description)
		{
			string Define = Description.Replace(" ","_").ToUpper();
			SaveToDatabase(Description,Define);
		}

		public void SaveToDatabase(string Description,string Define)
		{
			Criteria_DDataRow drCriteria;
			if( m_iPK_Criteria==0 )
			{
				drCriteria = new Criteria_DDataRow(mds.tCriteria_D.NewRow());
				drCriteria.fFK_CriteriaList = m_iPK_CriteriaList;
			}
			else
				drCriteria = mds.tCriteria_D[m_iPK_Criteria];

			drCriteria.fDescription = Description;
			drCriteria.fDefine = Define;

			drCriteria.fFK_CriteriaParmNesting_D = SaveNesting(0,RootNestingParameter);

			if( drCriteria.dr.RowState == System.Data.DataRowState.Detached )
				mds.tCriteria_D.Rows.Add(drCriteria.dr);
			mds.tCriteria_D.Update(1,mds.m_conn,mds.m_trans);
			m_iPK_Criteria = drCriteria.fPK_Criteria_D;

			//drCriteria.fFK_CriteriaParmNesting_D = SaveNesting(0,RootNestingParameter);

			mds.Update(1,mds.m_conn,mds.m_trans);
		}

		public int SaveNesting(int FK_CriteriaParmNesting_Parent,NestingParameter nestingParameter)
		{
			CriteriaParmNesting_DDataRow drCPN = new CriteriaParmNesting_DDataRow(mds.tCriteriaParmNesting_D.NewRow());
			if( FK_CriteriaParmNesting_Parent!=0 )
				drCPN.fFK_CriteriaParmNesting_D_Parent= FK_CriteriaParmNesting_Parent;
			drCPN.fIsAnd = nestingParameter.bAnd;
			drCPN.fIsNot = nestingParameter.bNot;
			mds.tCriteriaParmNesting_D.Rows.Add(drCPN.dr);
			mds.tCriteriaParmNesting_D.Update(1,mds.m_conn,mds.m_trans);
			foreach(CriteriaParm cp in nestingParameter.alCriteriaParm)
			{
				CriteriaParm_DDataRow drCP = new CriteriaParm_DDataRow(mds.tCriteriaParm_D.NewRow());
				drCP.fFK_CriteriaParmNesting_D = drCPN.fPK_CriteriaParmNesting_D;
				drCP.fFK_CriteriaParmList = cp.m_CriteriaParmList.ID;
				drCP.fOperator = cp.m_Operator;
				if(cp.m_CriteriaParmList.m_bAllowText)
				{
					drCP.fValue = cp.m_oValue.ToString();
				}
				else
				{
					CriteriaValue objValue = (CriteriaValue) cp.m_oValue;
					drCP.fValue = objValue.ID.ToString();
				}
				mds.tCriteriaParm_D.Rows.Add(drCP.dr);
				mds.tCriteriaParm_D.Update(1,mds.m_conn,mds.m_trans);
			}
			foreach(NestingParameter np in nestingParameter.alNestingParameter)
			{
				SaveNesting(drCPN.fPK_CriteriaParmNesting_D,np);
			}
			return drCPN.fPK_CriteriaParmNesting_D;
		}



		public string GetSerial()
		{
			return BuildNestingParmSerial(RootNestingParameter);
		}

		public void buildFromSerial(string strSerial)
		{

			alSerialNestingParms = new ArrayList();
			alSerialCriterias = new ArrayList();

			//clear out the old stuff

			string[] arCmds = strSerial.Split(Convert.ToChar("|"));
			foreach(string strCmd in arCmds)
			{
				if(strCmd != "")
				{
					string[] arValues = strCmd.Split(Convert.ToChar(","));
					if(arValues[0] == "n")
					{
						addSerialNestingParameter(Convert.ToInt32(arValues[1]), Convert.ToInt32(arValues[2]), ((arValues[3] == "true") ? true : false), ((arValues[4] == "true") ? true : false));
					}
					else
					{
						addSerialCriteria(Convert.ToInt32(arValues[1]), Convert.ToInt32(arValues[2]), Convert.ToInt32(arValues[3]), Convert.ToInt32(arValues[4]), arValues[5]);
					}
				}
			}


			//build the tree
			//first find the root
			RootNestingParameter = null;
			foreach(Object obj in alSerialNestingParms)
			{
				NestingParameter objNP = (NestingParameter) obj;
				if(objNP.iParentID == -1)
				{
					RootNestingParameter = objNP;
				}
			}
			SelectedNestingParameter = RootNestingParameter;

			buildFromSerialRecursive(RootNestingParameter);


		}

		private void buildFromSerialRecursive(NestingParameter objNestingParameter)
		{
			foreach(Object obj in alSerialNestingParms)
			{
				NestingParameter objNP = (NestingParameter) obj;
				if(objNP.iParentID == objNestingParameter.iID)
				{
					objNestingParameter.alNestingParameter.Add(objNP);
					objNP.m_ParentNestingParameter = objNestingParameter;
					buildFromSerialRecursive(objNP);
				}
			}
			foreach(Object obj in alSerialCriterias)
			{
				CriteriaParm objCP = (CriteriaParm) obj;
				if(objCP.iParentID == objNestingParameter.iID)
				{
					objNestingParameter.alCriteriaParm.Add(objCP);
					objCP.m_ParentNestingParameter = objNestingParameter;
				}
			}
		}



		private void addSerialNestingParameter(int intID, int intParentID, bool blnAnd, bool blnNot)
		{
			NestingParameter objNewNestingParameter = new NestingParameter(null);
			objNewNestingParameter.iID = intID;
			objNewNestingParameter.iParentID = intParentID;
			objNewNestingParameter.bAnd = blnAnd;
			objNewNestingParameter.bNot = blnNot;
			alSerialNestingParms.Add(objNewNestingParameter);
		}
		private void addSerialCriteria(int intID, int intParentID, int intProperty, int intCompare, string strValue)
		{		

			/*


			//get the criteriaparmlist
			CriteriaParmList objCPL;
			CriteriaParmList objCPLSelected = null;
			foreach(Object obj in alCriteriaParmList)
			{
				objCPL = (CriteriaParmList) obj;
				if(objCPL.ID == intProperty)
				{
					objCPLSelected = objCPL;
				}
			}

			//get the value
			CriteriaValue objCV;
			CriteriaValue objCVSelected = null;
			foreach(Object obj in objCPLSelected.m_alCriteriaValues)
			{
				objCV = (CriteriaValue) obj;
				if(objCV.ID == intValue)
				{
					objCVSelected = objCV;
				}

			}
			
			*/


			//todo make sure all data is good

			//CriteriaParm objNewCriteriaParm = new CriteriaParm(objCPLSelected, intCompare, objCVSelected);
			CriteriaParm objNewCriteriaParm = new CriteriaParm(GetFromID(intProperty),intCompare, strValue);

			objNewCriteriaParm.iID = intID;
			objNewCriteriaParm.iParentID = intParentID;
			alSerialCriterias.Add(objNewCriteriaParm);

		}




		public string GetParmListJavaScript(string strJSObjectName)
		{
			string strJS = "";

			CriteriaParmList objParmList;
			CriteriaValue objCriteriaValue;

			foreach(CriteriaParmList objPL in this.alCriteriaParmList)
			{
				objParmList = (CriteriaParmList) objPL;

				//objParmList.

				strJS += "with(" + strJSObjectName + ".addParmList(" + objParmList.ID + ", '" + JSParameter(objParmList.Name) + "'";


				strJS += (objParmList.m_bAllowText) ? ",true" : ",false";
				strJS += (objParmList.m_bAllowGLComparisson) ? ",true" : ",false";

				strJS += ")){";

				foreach(Object obj in objParmList.m_alCriteriaValues)
				{
					objCriteriaValue = (CriteriaValue) obj;
					strJS +="	addParmListValue(" + objCriteriaValue.ID + ",'" + JSParameter(objCriteriaValue.Name) + "');";
				}

				strJS +="};";


			}

			return strJS;

		}


		public string JSParameter(string strIn)
		{
			string strOut;
			strOut = Regex.Replace(strIn, "'", "");
			return strOut;
		}




		private string BuildNestingParmSerial(NestingParameter objNestingParameter)
		{
			string strReturn = "";
			
			strReturn += "|n," + Convert.ToString(objNestingParameter.iID);
			
			strReturn += "," + Convert.ToString(objNestingParameter.iParentID);
			strReturn += (objNestingParameter.bAnd ? ",true" : ",false");
			strReturn += (objNestingParameter.bNot ? ",true" : ",false");

			
			foreach(Object obj in objNestingParameter.alNestingParameter)
			{
				NestingParameter objNextNestingParameter = (NestingParameter) obj;
				strReturn += BuildNestingParmSerial(objNextNestingParameter);
			}
			

			foreach(Object obj in objNestingParameter.alCriteriaParm)
			{
				CriteriaParm objCriteriaParm = (CriteriaParm) obj;
				strReturn += "|p," + Convert.ToString(objCriteriaParm.iID);
				strReturn += "," + Convert.ToString(objCriteriaParm.iParentID);
				strReturn += "," + Convert.ToString(objCriteriaParm.m_CriteriaParmList.ID);
				strReturn += "," + Convert.ToString(objCriteriaParm.m_Operator);
				//strReturn += "," + Convert.ToString(objCriteriaParm.m_oValue);
				if(objCriteriaParm.m_CriteriaParmList.m_bAllowText)
				{
					strReturn += "," + objCriteriaParm.m_oValue;
				}
				else
				{
					CriteriaValue objValue = (CriteriaValue) objCriteriaParm.m_oValue;
					strReturn += "," + Convert.ToString(objValue.ID);
				}
			}


			return strReturn;

		}




	}



	


}
